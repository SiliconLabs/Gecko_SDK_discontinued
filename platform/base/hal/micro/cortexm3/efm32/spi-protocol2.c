/** @file hal/micro/cortexm3/efm32/spi-protocol2.c
 *  @brief SPI protocol implementation for ZIP NCPs (ip-modem-app-spi et al)
 *
 * Copyright 2015 by Silicon Laboratories. All rights reserved.             *80*
 */

#include PLATFORM_HEADER
#include "stack/include/ember.h"
#include "hal/hal.h"
#include "serial/com.h"
#include "hal/micro/cortexm3/spi-protocol2.h"
#include "spidrv.h"
#include "em_gpio.h"
#include "gpiointerrupt.h"
#include "hal/micro/cortexm3/efm32/spi-protocol-device.h"

typedef struct
{
  LDMA_Descriptor_t desc[2];
} DmaXfer_t;

static DmaXfer_t dmaXfer;

typedef struct NcpSpiRxBuffer
{
  uint8_t buffer[NCP_SPI_BUFSIZE];
  uint8_t * packet; // start address of the packet within the buffer (may not be
                  // 0 due to padding)
  uint8_t * payload; // start address of the payload within the buffer (will be
                   // NCP_SPI_PKT_HEADER_LEN bytes after 'packet'
} NcpSpiRxBuffer;

typedef struct NcpSpiTxBuffer
{
  uint8_t buffer[NCP_SPI_BUFSIZE];
  uint16_t head; // index of first byte in the circular buffer
  uint16_t tail; // index after the last byte in the buffer, i.e. the index at
               // which the next byte enqueued will be placed
  uint16_t length; // number of bytes in the buffer -- if we're currently copying
                 // data into the buffer, this may not be the same as the
                 // number of bytes indicated by head and tail

} NcpSpiTxBuffer;

static SPIDRV_HandleData_t spiHandleData;
static SPIDRV_Handle_t spiHandle = &spiHandleData;
const LDMA_TransferCfg_t spiXferCfg = LDMA_TRANSFER_CFG_PERIPHERAL( 0 );
const LDMA_Descriptor_t m2pdesc = LDMA_DESCRIPTOR_SINGLE_M2P_BYTE( NULL, NULL, 0 );

static NcpSpiTxBuffer txBuffer; // NCP to host
static NcpSpiRxBuffer rxBuffers[SPI_NCP_RX_BUFFERS]; // host to NCP
static NcpSpiRxBuffer* activeRxBuffer = rxBuffers;

static bool spipFlagWakeFallingEdge; // Falling edge on nWAKE
static bool spipFlagIdleHostInt;     // Idle nHOST_INT at proper time
static bool spipFlagNcpReset = true; // SPIP has just booted
static bool spipFlagRxComplete;      // Complete but unprocessed transaction
static bool spipFlagIgnoreRx;        // Ignore RX (error recovery)
static uint16_t spipBytesReceived;   // number of bytes RX'd in this transaction

static void dequeueTx(int32_t length);
static bool findRxPacketStart(NcpSpiRxBuffer* buf, uint16_t bytesReceived);
static inline uint16_t halHostTxBytesUsed(void);
static void nSSEL_ISR(uint8_t pin);
static void nWAKE_ISR(uint8_t pin);
static void restartSpiRx(void);
static bool rxPacketHasHeader(NcpSpiRxBuffer* buf, uint16_t bytesReceived);
static uint16_t setTxHeader(void);
static bool spiRxDone(unsigned int channel,
                      unsigned int sequenceNo,
                      void *userParam );
static inline void wipeAndRestartSpi(void);
static void wipeSpi(void);
/**************************************************************************//**
* @brief Setup SPI NCP
*****************************************************************************/
void halHostSerialInit(void)
{
  // hack to get around not being able to init structs with array members at
  // static initialization time in a portable way using our version of C
  txBuffer.tail = NCP_SPI_PKT_HEADER_LEN;
  txBuffer.length = NCP_SPI_PKT_HEADER_LEN;

  spipFlagWakeFallingEdge = false; //start with no edge on nWAKE
  spipFlagRxComplete = false;
  spipFlagIgnoreRx = false;

  wipeAndRestartSpi();
  // Initialize nHOST_INT as output
  GPIO_PinModeSet(NHOST_INT_PORT, NHOST_INT_PIN, gpioModePushPull, 1);
  GPIO_PinOutSet(NHOST_INT_PORT, NHOST_INT_PIN);

#ifndef DISABLE_NWAKE
  // Initialize nWAKE as input with falling edge interrupt
  GPIO_PinModeSet (NWAKE_PORT, NWAKE_PIN, gpioModeInputPullFilter, 1);
  GPIO_IntConfig(NWAKE_PORT, NWAKE_PIN, false, true, true);
#endif

  // Initialize nSSEL as input with falling/rising edge interrupts
  GPIO_PinModeSet (NSSEL_PORT, NSSEL_PIN, gpioModeInputPullFilter, 1);
  GPIO_IntConfig(NSSEL_PORT, NSSEL_PIN, true, true, true);

  GPIOINT_Init();
  GPIOINT_CallbackRegister(NSSEL_PIN, nSSEL_ISR);
}

void halHostSerialTickFullDuplex()
{
  uint8_t bytesToProcess = 0;
  static uint8_t consumed = 0;

  if (spipFlagIgnoreRx) {
    restartSpiRx();
  }

  if (spipFlagRxComplete) {
    if (rxPacketHasHeader(activeRxBuffer, spipBytesReceived)) {
      bytesToProcess = MIN(spipBytesReceived
                           - (activeRxBuffer->payload - activeRxBuffer->buffer),
                           activeRxBuffer->packet[NCP_SPI_LENGTH_INDEX]);
      consumed += serialCopyFromRx(activeRxBuffer->payload + consumed,
                                   bytesToProcess - consumed);
    } else {
      // Can't find any payload data to process -- this is probably bad
      // because we might have ACKed what the host sent, but best we can
      // do is throw it away and hope higher layers realize something is
      // missing and initiate appropriate recovery.
      consumed = bytesToProcess;
    }

    if (consumed < bytesToProcess) {
    } else {

      // free the buffer we were processing
      consumed = 0;
      bytesToProcess = 0;
      restartSpiRx();
    }
  }
}

/**************************************************************************//**
* @brief SPI NCP tick function
*****************************************************************************/
void halHostSerialTick(void)
{
  if (!(USART_StatusGet(SPI_NCP_USART) & _USART_STATUS_RXENS_MASK)) {
    // SPI port (re)initialization is deferred to Tick time
    // so app can choose to sleep/wake without bringing up SPI
    // host communications until it needs to
    halHostSerialInit();
    assert((USART_StatusGet(SPI_NCP_USART) & _USART_STATUS_RXENS_MASK));
  }
  
  int remaining;
  DMADRV_TransferRemainingCount(spiHandle->rxDMACh, &remaining);
  if (spipFlagWakeFallingEdge) { //detected falling edge on nWAKE, handshake  
    GPIO_PinOutClear(NHOST_INT_PORT, NHOST_INT_PIN);
    while( nWAKE_IS_ASSERTED() ) { halResetWatchdog();}
    GPIO_PinOutSet(NHOST_INT_PORT, NHOST_INT_PIN);
    spipFlagWakeFallingEdge = false;
    //The wake handshake is complete, but spipFlagIdleHostInt is saying
    //that there is a callback pending.
    if(!spipFlagIdleHostInt) {
      spipFlagIdleHostInt = true;
      halCommonDelayMicroseconds(50); //delay 50us so Host can get ready
      GPIO_PinOutClear(NHOST_INT_PORT, NHOST_INT_PIN);  //indicate the pending callback
    }
  } 
  else
  {
    if (nSSEL_IS_ASSERTED() && (NCP_SPI_BUFSIZE - remaining - 1 > 0))
    {
      GPIO_PinOutSet(NHOST_INT_PORT, NHOST_INT_PIN);
    }
    // perform processing specific to one mode or the other
    halHostSerialTickFullDuplex();
  }

  emLoadSerialTx();
}

/***************************************************************************//**
 * @brief Signal to SPI protocol that there is data to send
 *
 * @param[in] haveData boolean to indicate available data
 ******************************************************************************/
void halHostCallback(bool haveData)
{
  if(haveData) {
    //only assert nHOST_INT if we are outside a wake handshake (wake==1)
    //and outside of a current transaction (nSSEL=1)
    //if inside a wake handshake or transaction, delay asserting nHOST_INT
    //until the SerialTick
    if( nWAKE_IS_NEGATED() && nSSEL_IS_NEGATED() ) {
      GPIO_PinOutClear(NHOST_INT_PORT, NHOST_INT_PIN);
    }
    spipFlagIdleHostInt = false;
  } else {
    spipFlagIdleHostInt = true;
  }
}

/**************************************************************************//**
 * @brief check if TX is idle. Must be called from within ATOMIC().
 * 
 * @return
 *    @ref True if TX is idle
*****************************************************************************/
bool halHostTxIsIdle(void)
{
  return halHostTxBytesUsed() == NCP_SPI_PKT_HEADER_LEN;
}

/***************************************************************************//**
 * @brief Enqueue data for TX to host. Will cause a host callback (assert nHOST_INT)
 * Safe to call outside ATOMIC()
 *
 * @param[in] data pointer to data to TX
 *
 * @param[in] length length of data
 * 
 * @return
 *    Number of bytes queued (may not equal @ref length)
 ******************************************************************************/
uint16_t halHostEnqueueTx(const uint8_t* data, uint16_t length)
{
  // if we ever want to support datagram-style use of this protocol, we would
  // want to return 0 here if there is already a packet in the pipe instead of
  // growing said packet

  uint16_t chunkStart;

  // figure out how much space we have, and reserve it
  ATOMIC(
    length = MIN(length, halHostTxSpaceAvailable());
    chunkStart = txBuffer.tail;
    txBuffer.tail = (txBuffer.tail + length) % NCP_SPI_BUFSIZE;
  )

  if (length == 0) {
    return 0;
  }

  uint16_t chunkLength = length;
  uint16_t dataOffset = 0;

  if (chunkStart + chunkLength >= NCP_SPI_BUFSIZE) { // it's going to wrap
    MEMMOVE(txBuffer.buffer + chunkStart, data, NCP_SPI_BUFSIZE - chunkStart);
    chunkLength -= NCP_SPI_BUFSIZE - chunkStart;
    dataOffset = NCP_SPI_BUFSIZE - chunkStart;
    chunkStart = 0;
  }

  MEMMOVE(txBuffer.buffer + chunkStart, data + dataOffset, chunkLength);
  // store the length, which is the signal to the consumer that the data is
  // really available
  ATOMIC(
    txBuffer.length += length;
  )

  halHostCallback(true);

  return length;
}

static uint16_t packetLen;
/**************************************************************************//**
* @brief nSSEL signals the start/end of a SPI transaction
*****************************************************************************/
static void nSSEL_ISR(uint8_t pin)
{
  // falling edge
  if (nSSEL_IS_ASSERTED())
  {
    // set TX packet header values
    uint16_t payloadLen = setTxHeader();

    spiHandle->initData.port->CMD = USART_CMD_CLEARTX;

    /* include tail padding.  this means that the padded bytes will be whatever
     * comes next in the buffer, possibly useful data, possibly garbage.  we're
     * relying on the host respecting the length byte of the header to not
     * process these
     */
    packetLen = NCP_SPI_PKT_HEADER_LEN + payloadLen + NCP_SPI_PKT_TAIL_LEN;

    if (txBuffer.head + packetLen <= NCP_SPI_BUFSIZE)
    {
      DMADRV_MemoryPeripheral( spiHandle->txDMACh,
                               spiHandle->txDMASignal,
                               (void *)&(spiHandle->initData.port->TXDATA),
                               (void *)(txBuffer.buffer + txBuffer.head),
                               true,
                               packetLen,
                               dmadrvDataSize1,
                               NULL,
                               NULL );
    }
    else //wraps, so set up back to back DMA transfers
    {
      LDMA_TransferCfg_t xfer;
      LDMA_Descriptor_t *desc;
      xfer = spiXferCfg;
      desc = &dmaXfer.desc[0];
      
      // Set up first descriptor to transfer from head to end of txBuffer
      *desc = m2pdesc;
      xfer.ldmaReqSel    = spiHandle->txDMASignal;
      desc->xfer.xferCnt = NCP_SPI_BUFSIZE - txBuffer.head - 1;
      desc->xfer.dstAddr = (uint32_t)&(spiHandle->initData.port->TXDATA);
      desc->xfer.srcAddr = (uint32_t)(txBuffer.buffer + txBuffer.head);
      desc->xfer.size    = dmadrvDataSize1;
      // Set next descriptor to mirror first but transfer from the begninning
      // of txBuffer
      dmaXfer.desc[1] = *desc;
      dmaXfer.desc[1].xfer.srcAddr = (uint32_t)&(txBuffer.buffer);
      dmaXfer.desc[1].xfer.xferCnt = packetLen - (NCP_SPI_BUFSIZE - txBuffer.head) - 1;
      // Link first descriptor to second descriptor for back to back transfer
      desc->xfer.linkMode = ldmaCtrlDstAddrModeRel;
      desc->xfer.link     = 1;
      desc->xfer.linkAddr = 4;
      // initiate LDMA
      LDMA_StartTransfer( spiHandle->txDMACh, &xfer, desc );
    }
  }
  else // rising edge
  {
    int remainingRx;
    int remainingTx = 0;
    uint32_t txDmaLink;
    DMADRV_TransferRemainingCount(spiHandle->rxDMACh, &remainingRx);
    DMADRV_TransferRemainingCount(spiHandle->txDMACh, &remainingTx);
    txDmaLink = LDMA->CH [spiHandle->txDMACh].LINK & _LDMA_CH_LINK_LINK_MASK;
    // previous DMA was linked to wrap around queue
    if (!LDMA_TransferDone(spiHandle->txDMACh))
    {
      if (dmaXfer.desc[0].xfer.link == 1)
      {
        // clear link bit which serves as flag for wrapping
        dmaXfer.desc[0].xfer.link = 0;
        // check if we're still transmitting the first descriptor by checking
        // the source address of the LDMA channel, which shows the next transfer.
        // If the next transfer starts at the beginning of the buffer, then
        // we haven't started and should add the second xferCnt
        if (txDmaLink)
        {
          // xfer count is one less than actual transfer number
          remainingTx+=dmaXfer.desc[1].xfer.xferCnt+1;
        }
      }
    }
    // Stop transfers in case they're still running
    DMADRV_StopTransfer( spiHandle->rxDMACh );
    DMADRV_StopTransfer( spiHandle->txDMACh );
    if (spipFlagNcpReset && packetLen >= 1) { // we sent the byte indicating a reset
      spipFlagNcpReset = false;
    }
    spipFlagRxComplete = true;
    spipBytesReceived = NCP_SPI_BUFSIZE - remainingRx - 1;

    if (remainingTx>0)
    {
      remainingTx-=1;
      SPI_NCP_USART->CMD = USART_CMD_CLEARTX;
    }
    // dequeue the data we just sent
    dequeueTx(packetLen-remainingTx-NCP_SPI_PKT_TAIL_LEN);

    // if there's still data to send, indicate that to the host
    halHostCallback(txBuffer.length > NCP_SPI_PKT_HEADER_LEN);
  }
}

/**************************************************************************//**
 * @brief Check for space in TX queue
 * 
 * @return
 *    Number of bytes available
*****************************************************************************/
uint16_t halHostTxSpaceAvailable(void)
{
  return NCP_SPI_BUFSIZE - halHostTxBytesUsed() - 1;
}

/**************************************************************************//**
* Internal Functions
*****************************************************************************/

/**************************************************************************//**
* @brief shift bytes off the TX buffer.  
* the packet header is included in these bytes, and this function guarantees 
* that the first bytes of the buffer are reserved for the packet header, so if 
* you attempt to shift fewer bytes than the size of the packet header, no change 
* will occur.
* @param length  the number of bytes to shift off the buffer.  if this is
* greater than the length of the packet, the packet length will be made zero.
*****************************************************************************/

static void dequeueTx(int32_t length)
{
  assert(length <= txBuffer.length);

  // keep enough bytes for the packet header reserved that don't have actual
  // data in them
  if (length < NCP_SPI_PKT_HEADER_LEN) {
    return;
  }

  length -= NCP_SPI_PKT_HEADER_LEN;

  ATOMIC(
    txBuffer.length -= length;
    txBuffer.head = (txBuffer.head + length) % NCP_SPI_BUFSIZE;
  )
}

/**************************************************************************//**
 * @brief Check how many bytes are in the TX Queue
 * 
 * @return
 *    Number of bytes
*****************************************************************************/
static inline uint16_t halHostTxBytesUsed(void)
{
  return (txBuffer.tail + NCP_SPI_BUFSIZE - txBuffer.head) % NCP_SPI_BUFSIZE;
}

/**************************************************************************//**
* @brief Set up SPI RX transaction
*****************************************************************************/
static void restartSpiRx(void)
{
  spipFlagRxComplete = false;
  spipBytesReceived = 0;

  activeRxBuffer->packet = activeRxBuffer->buffer; // this isn't actually the start of the
                                     // packet due to possible padding, but it
                                     // is used as the point where we begin
                                     // searching for the start of the packet
  // clear RX buffer
  spiHandle->initData.port->CMD = USART_CMD_CLEARRX;
  // Start new DMA transfer
  DMADRV_PeripheralMemory( spiHandle->rxDMACh,
                           spiHandle->rxDMASignal,
                           (void *) activeRxBuffer->buffer,
                           (void *)&(spiHandle->initData.port->RXDATA),
                           true,
                           NCP_SPI_BUFSIZE - 1,
                           dmadrvDataSize1,
                           spiRxDone,
                           NULL );
  // enable byte interrupt (will be deiabled after first byte)
  USART_IntClear(SPI_NCP_USART, USART_IF_RXDATAV);
  USART_IntEnable(SPI_NCP_USART, USART_IF_RXDATAV);
}

/**************************************************************************//**
* @brief Searches through the first few bytes of the received packet to find
 * the start, ignoring any padding bytes it finds on the way.  This function
 * relies on state inside the buffer object to avoid repetitive searching; that
 * state must be reset before the next RX operation.
 * @param bytesReceived  the number of bytes received so far of the packet,
 * needed for when the full packet hasn't been received yet
*****************************************************************************/
static bool findRxPacketStart(NcpSpiRxBuffer* buf, uint16_t bytesReceived)
{
  // TODO unwind loop for speed?
  for (int i = buf->packet - buf->buffer;
       i < NCP_SPI_RX_SLOP && i < bytesReceived;
       ++i) {
    if (*buf->packet == 0xFF || *buf->packet == 0x00) { // padding
      ++buf->packet;
      continue;
    } else if ((*buf->packet & NCP_SPI_PKT_CMD_PAT_MASK)
               == NCP_SPI_PKT_CMD_PAT
               && (*buf->packet & NCP_SPI_PKT_CMD_RSP)
               == DIRECTION_HOST_TO_NCP) {
      // assert the payload bit is set; it might not always be, but
      // we don't know how to deal with it if it's not.  TODO ideally,
      // replace this with some code that can handle it
      assert(*buf->packet & NCP_SPI_PKT_CMD_PAY);
      buf->payload = buf->packet + NCP_SPI_PKT_HEADER_LEN;
      return true;
    } else {
      return false;
    }
  }

  return false;
}

/***************************************************************************//**
 * @brief check to see if rxPacket has proper header
 *
 * @param[in] buf pointer to the NcpSpiRxBuffer
 *
 * @param[in] bytesReceived number of bytes in the buffer
 * 
 * @return
 *    boolean confirming proper receipt of packet
 ******************************************************************************/
static bool rxPacketHasHeader(NcpSpiRxBuffer* buf, uint16_t bytesReceived)
{
  if (!findRxPacketStart(buf, bytesReceived)) {
    return false;
  }

  return bytesReceived >= (buf->payload - buf->buffer);
}

/**************************************************************************//**
* @brief sets the packet header fields for a TX packet based on global state.
* @note this should only be called immediately before loading the DMA, from the ISR
 * 
 * @return
 *    length of TX packet
*****************************************************************************/
static uint16_t setTxHeader(void)
{
  // set command byte constant fields
  txBuffer.buffer[txBuffer.head] = NCP_SPI_PKT_CMD_PAT
                                 | NCP_SPI_PKT_CMD_PAY
                                 | NCP_SPI_PKT_CMD_RSP;

  // did we reset?
  if (spipFlagNcpReset) {
    txBuffer.buffer[txBuffer.head] |= NCP_SPI_PKT_CMD_RST;
  }

  // if RX DMA isn't loaded, tell host we're not receiving, unless we're
  // intentionally ignoring RX
  bool active;
  DMADRV_TransferActive(spiHandle->rxDMACh, &active);
  if (!(active) && !spipFlagIgnoreRx) {
    txBuffer.buffer[txBuffer.head] |= NCP_SPI_PKT_CMD_NAK;
  }
  // set length byte
  uint16_t len = txBuffer.length - NCP_SPI_PKT_HEADER_LEN;
  if (len > NCP_SPI_MAX_PAYLOAD) {
    len = NCP_SPI_MAX_PAYLOAD;
  }

  txBuffer.buffer[(txBuffer.head + 1) % NCP_SPI_BUFSIZE] = len;

  return len;
}

/**************************************************************************//**
* @brief SPI RX callback
*****************************************************************************/
static bool spiRxDone( unsigned int channel,
                       unsigned int sequenceNo,
                       void *userParam )
{
  return true;
}

/**************************************************************************//**
* @brief Reset SPI state and restart RX
*****************************************************************************/
static inline void wipeAndRestartSpi(void)
{
  wipeSpi();
  restartSpiRx();
}

/**************************************************************************//**
* @brief Reset SPI state
*****************************************************************************/
static void wipeSpi(void)
{
  DMADRV_StopTransfer( spiHandle->txDMACh );
  DMADRV_StopTransfer( spiHandle->rxDMACh );
  // Deinitialize SPI driver
  SPIDRV_DeInit(spiHandle);

  SPIDRV_Init_t initData = SPI_NCP_USART_INIT;

  // Initialize a SPI driver instance
  SPIDRV_Init(spiHandle, &initData);

  /* Clear previous RX interrupts */
  USART_IntClear(SPI_NCP_USART, USART_IF_RXDATAV);
  NVIC_ClearPendingIRQ(SPI_NCP_USART_IRQn);

  /* Enable RX interrupts */
  USART_IntEnable(SPI_NCP_USART, USART_IF_RXDATAV);
  NVIC_EnableIRQ(SPI_NCP_USART_IRQn);
}

// IRQ handler so that NCP wakes up on SPI transfers and de-asserts nHOST_INT
void SPI_NCP_USART_IRQ_NAME(void)
{
  if (USART_IntGetEnabled(SPI_NCP_USART) & _USART_IF_RXDATAV_MASK)
  {
    GPIO_PinOutSet(NHOST_INT_PORT, NHOST_INT_PIN);
    USART_IntDisable(SPI_NCP_USART, USART_IF_RXDATAV);
    USART_IntClear(SPI_NCP_USART, USART_IF_RXDATAV);
  }
}
