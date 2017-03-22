/***************************************************************************//**
 * @file btl_driver_spislave.h
 * @brief Universal SPI slave driver for the Silicon Labs Bootloader.
 * @author Silicon Labs
 * @version 1.0.0
 *******************************************************************************
 * @section License
 * <b>Copyright 2016 Silicon Laboratories, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/
#include "config/btl_config.h"

#include "btl_driver_delay.h"
#include "btl_driver_spislave.h"
#include "api/btl_interface.h"

#include "em_cmu.h"
#include "em_usart.h"
#include "em_gpio.h"
#include "em_ldma.h"
#include "em_bus.h"

#include "plugin/debug/btl_debug.h"

#if BTL_SPISLAVE_NUMBER == 0
#define BTL_SPISLAVE        USART0
#define BTL_SPISLAVE_CLOCK  cmuClock_USART0
#define BTL_SPISLAVE_LDMA_TXBL_SIGNAL    (ldmaPeripheralSignal_USART0_TXBL)
#define BTL_SPISLAVE_LDMA_RXDATAV_SIGNAL (ldmaPeripheralSignal_USART0_RXDATAV)
#elif BTL_SPISLAVE_NUMBER == 1
#define BTL_SPISLAVE        USART1
#define BTL_SPISLAVE_CLOCK  cmuClock_USART1
#define BTL_SPISLAVE_LDMA_TXBL_SIGNAL    (ldmaPeripheralSignal_USART1_TXBL)
#define BTL_SPISLAVE_LDMA_RXDATAV_SIGNAL (ldmaPeripheralSignal_USART1_RXDATAV)
#elif BTL_SPISLAVE_NUMBER == 2
#define BTL_SPISLAVE        USART2
#define BTL_SPISLAVE_CLOCK  cmuClock_USART2
#define BTL_SPISLAVE_LDMA_TXBL_SIGNAL    (ldmaPeripheralSignal_USART2_TXBL)
#define BTL_SPISLAVE_LDMA_RXDATAV_SIGNAL (ldmaPeripheralSignal_USART2_RXDATAV)
#elif BTL_SPISLAVE_NUMBER == 3
#define BTL_SPISLAVE        USART3
#define BTL_SPISLAVE_CLOCK  cmuClock_USART3
#define BTL_SPISLAVE_LDMA_TXBL_SIGNAL    (ldmaPeripheralSignal_USART3_TXBL)
#define BTL_SPISLAVE_LDMA_RXDATAV_SIGNAL (ldmaPeripheralSignal_USART3_RXDATAV)
#elif BTL_SPISLAVE_NUMBER == 4
#define BTL_SPISLAVE        USART4
#define BTL_SPISLAVE_CLOCK  cmuClock_USART4
#define BTL_SPISLAVE_LDMA_TXBL_SIGNAL    (ldmaPeripheralSignal_USART4_TXBL)
#define BTL_SPISLAVE_LDMA_RXDATAV_SIGNAL (ldmaPeripheralSignal_USART4_RXDATAV)
#elif BTL_SPISLAVE_NUMBER == 5
#define BTL_SPISLAVE        USART5
#define BTL_SPISLAVE_CLOCK  cmuClock_USART5
#define BTL_SPISLAVE_LDMA_TXBL_SIGNAL    (ldmaPeripheralSignal_USART5_TXBL)
#define BTL_SPISLAVE_LDMA_RXDATAV_SIGNAL (ldmaPeripheralSignal_USART5_RXDATAV)
#elif BTL_SPISLAVE_NUMBER == 6
#define BTL_SPISLAVE        USART6
#define BTL_SPISLAVE_CLOCK  cmuClock_USART6
#define BTL_SPISLAVE_LDMA_TXBL_SIGNAL    (ldmaPeripheralSignal_USART6_TXBL)
#define BTL_SPISLAVE_LDMA_RXDATAV_SIGNAL (ldmaPeripheralSignal_USART6_RXDATAV)
#else
#error "Invalid BTL_SPISLAVE"
#endif


// ‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
// Configuration validation
#if (BTL_SPISLAVE_RX_BUFFER_SIZE % 2) != 0
#error "SPI Slave RX buffer size is not even"
#endif

// ‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
// Static variables

/// Flag to indicate hardware is up and running
static bool    initialized = false;
/// Receive buffer (forced to even size).
/// LDMA will pingpong between two halves of this buffer.
static uint8_t rxBuffer[BTL_SPISLAVE_RX_BUFFER_SIZE];
/// Transmit buffer for LDMA use.
static uint8_t txBuffer[BTL_SPISLAVE_TX_BUFFER_SIZE];

/// Index into the receive buffer indicating which byte is due to be read next.
static size_t  rxHead;

/// LDMA channel configuration triggering on free space in hardware FIFO
static const LDMA_TransferCfg_t ldmaTxTransfer = LDMA_TRANSFER_CFG_PERIPHERAL(
  BTL_SPISLAVE_LDMA_TXBL_SIGNAL
);
/// LDMA channel configuration triggering on available byte in hardware FIFO
static const LDMA_TransferCfg_t ldmaRxTransfer = LDMA_TRANSFER_CFG_PERIPHERAL(
  BTL_SPISLAVE_LDMA_RXDATAV_SIGNAL
);

/// LDMA transfer for copying transmit buffer to hardware FIFO
static LDMA_Descriptor_t        ldmaTxDesc  = LDMA_DESCRIPTOR_SINGLE_M2P_BYTE(
  txBuffer,
  &(BTL_SPISLAVE->TXDATA),
  0
);

static const LDMA_Descriptor_t  ldmaRxDesc[4] = {
  /// First half of receive pingpong configuration
  LDMA_DESCRIPTOR_LINKREL_P2M_BYTE(
    &(BTL_SPISLAVE->RXDATA),
    &(rxBuffer[0]),
    BTL_SPISLAVE_RX_BUFFER_SIZE/2,
    1
  ),
  // Sync structure waiting for SYNC[1], clearing SYNC[0]
  LDMA_DESCRIPTOR_LINKREL_SYNC(
    0,
    1 << 0,
    1 << 1,
    1 << 1,
    1
  ),
  /// Second half of receive pingpong configuration
  LDMA_DESCRIPTOR_LINKREL_P2M_BYTE(
    &(BTL_SPISLAVE->RXDATA),
    &(rxBuffer[BTL_SPISLAVE_RX_BUFFER_SIZE/2]),
    BTL_SPISLAVE_RX_BUFFER_SIZE/2,
    1
  ),
  /// Sync structure waiting for SYNC[0], clearing SYNC[1]
  LDMA_DESCRIPTOR_LINKREL_SYNC(
    0,
    1 << 1,
    1 << 0,
    1 << 0,
    -3
  )
};

// ‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
// Functions

/**
 * Initialize the configured USART peripheral for SPI slave operation.
 *  Also sets up GPIO settings for MOSI, MISO, SCLK and SS.
 *
 * @return BOOTLOADER_OK if succesful, error code otherwise
 */
void spislave_init(void)
{
  // Clock peripherals
  CMU->CTRL        |= CMU_CTRL_HFPERCLKEN;
  CMU->HFBUSCLKEN0 |= CMU_HFBUSCLKEN0_GPIO;
  CMU->HFBUSCLKEN0 |= CMU_HFBUSCLKEN0_LDMA;
  CMU_ClockEnable(BTL_SPISLAVE_CLOCK, true);

  // Set up USART
  spislave_deinit();

  GPIO_PinModeSet(BTL_SPISLAVE_MISO_PORT,
                  BTL_SPISLAVE_MISO_PIN,
                  gpioModePushPull,
                  1);
  GPIO_PinModeSet(BTL_SPISLAVE_MOSI_PORT,
                  BTL_SPISLAVE_MOSI_PIN,
                  gpioModeInput,
                  1);
  GPIO_PinModeSet(BTL_SPISLAVE_CLK_PORT,
                  BTL_SPISLAVE_CLK_PIN,
                  gpioModeInput,
                  1);
  GPIO_PinModeSet(BTL_SPISLAVE_CS_PORT,
                  BTL_SPISLAVE_CS_PIN,
                  gpioModeInput,
                  1);

  // Configure route
  BTL_SPISLAVE->ROUTELOC0
    = BTL_SPISLAVE_MOSI_LOCATION << _USART_ROUTELOC0_TXLOC_SHIFT
    | BTL_SPISLAVE_MISO_LOCATION << _USART_ROUTELOC0_RXLOC_SHIFT
    | BTL_SPISLAVE_CLK_LOCATION << _USART_ROUTELOC0_CLKLOC_SHIFT
    | BTL_SPISLAVE_CS_LOCATION << _USART_ROUTELOC0_CSLOC_SHIFT;

  BTL_SPISLAVE->ROUTEPEN   = USART_ROUTEPEN_TXPEN
                           | USART_ROUTEPEN_CSPEN
                           | USART_ROUTEPEN_CLKPEN;
  // Bump USART into SPI mode
  BTL_SPISLAVE->CTRL |= USART_CTRL_SYNC
                      | USART_CTRL_CLKPOL_IDLELOW
                      | USART_CTRL_CLKPHA_SAMPLELEADING
                      | USART_CTRL_MSBF;

  // Configure databits to one byte.
  BTL_SPISLAVE->FRAME = USART_FRAME_DATABITS_EIGHT;

  // Enable TX/RX
  BTL_SPISLAVE->CMD = USART_CMD_RXEN
                    | USART_CMD_TXEN
                    | USART_CMD_RXBLOCKEN;

  // Reset LDMA
  LDMA->CTRL    = _LDMA_CTRL_RESETVALUE;
  LDMA->CHEN    = _LDMA_CHEN_RESETVALUE;
  LDMA->DBGHALT = _LDMA_DBGHALT_RESETVALUE;
  LDMA->REQDIS  = _LDMA_REQDIS_RESETVALUE;
  LDMA->IEN     = _LDMA_IEN_RESETVALUE;

  // Set up channel 0 as RX transfer
  LDMA->CH[BTL_SPISLAVE_LDMA_RX_CHANNEL].REQSEL = ldmaRxTransfer.ldmaReqSel;
  LDMA->CH[BTL_SPISLAVE_LDMA_RX_CHANNEL].LOOP
    = (ldmaRxTransfer.ldmaLoopCnt << _LDMA_CH_LOOP_LOOPCNT_SHIFT);
  LDMA->CH[BTL_SPISLAVE_LDMA_RX_CHANNEL].CFG
    = (ldmaRxTransfer.ldmaCfgArbSlots << _LDMA_CH_CFG_ARBSLOTS_SHIFT)
    | (ldmaRxTransfer.ldmaCfgSrcIncSign << _LDMA_CH_CFG_SRCINCSIGN_SHIFT)
    | (ldmaRxTransfer.ldmaCfgDstIncSign << _LDMA_CH_CFG_DSTINCSIGN_SHIFT);

  // Set up channel 1 as TX transfer
  LDMA->CH[BTL_SPISLAVE_LDMA_TX_CHANNEL].REQSEL = ldmaTxTransfer.ldmaReqSel;
  LDMA->CH[BTL_SPISLAVE_LDMA_TX_CHANNEL].LOOP
    = (ldmaTxTransfer.ldmaLoopCnt << _LDMA_CH_LOOP_LOOPCNT_SHIFT);
  LDMA->CH[BTL_SPISLAVE_LDMA_TX_CHANNEL].CFG
    = (ldmaTxTransfer.ldmaCfgArbSlots << _LDMA_CH_CFG_ARBSLOTS_SHIFT)
    | (ldmaTxTransfer.ldmaCfgSrcIncSign << _LDMA_CH_CFG_SRCINCSIGN_SHIFT)
    | (ldmaTxTransfer.ldmaCfgDstIncSign << _LDMA_CH_CFG_DSTINCSIGN_SHIFT);

  // Kick off background RX
  LDMA->CH[BTL_SPISLAVE_LDMA_RX_CHANNEL].LINK
    = (uint32_t)(&ldmaRxDesc[0]) & _LDMA_CH_LINK_LINKADDR_MASK;

  LDMA->LINKLOAD = (1 << BTL_SPISLAVE_LDMA_RX_CHANNEL);

  // Mark second half of RX buffer as ready
  BUS_RegMaskedSet(&LDMA->SYNC, 1 << 1);

  delay_init();
  initialized = true;
}

/**
 * Resets the configured USART peripheral and returns MOSI, MISO, CLK and SS
 * to a Hi-Z state.
 */
void spislave_deinit(void)
{
  GPIO_PinModeSet(BTL_SPISLAVE_MISO_PORT,
                  BTL_SPISLAVE_MISO_PIN,
                  gpioModeDisabled,
                  0);
  GPIO_PinModeSet(BTL_SPISLAVE_MOSI_PORT,
                  BTL_SPISLAVE_MOSI_PIN,
                  gpioModeDisabled,
                  0);
  GPIO_PinModeSet(BTL_SPISLAVE_CLK_PORT,
                  BTL_SPISLAVE_CLK_PIN,
                  gpioModeDisabled,
                  0);
  GPIO_PinModeSet(BTL_SPISLAVE_CS_PORT,
                  BTL_SPISLAVE_CS_PIN,
                  gpioModeDisabled,
                  0);

  BTL_SPISLAVE->CMD       = USART_CMD_RXDIS
                          | USART_CMD_TXDIS
                          | USART_CMD_MASTERDIS
                          | USART_CMD_RXBLOCKDIS
                          | USART_CMD_TXTRIDIS
                          | USART_CMD_CLEARTX
                          | USART_CMD_CLEARRX;
  BTL_SPISLAVE->CTRL      = _USART_CTRL_RESETVALUE;
  BTL_SPISLAVE->CTRLX     = _USART_CTRLX_RESETVALUE;
  BTL_SPISLAVE->FRAME     = _USART_FRAME_RESETVALUE;
  BTL_SPISLAVE->TRIGCTRL  = _USART_TRIGCTRL_RESETVALUE;
  BTL_SPISLAVE->CLKDIV    = _USART_CLKDIV_RESETVALUE;
  BTL_SPISLAVE->IEN       = _USART_IEN_RESETVALUE;
  BTL_SPISLAVE->IFC       = _USART_IFC_MASK;
  BTL_SPISLAVE->ROUTEPEN  = _USART_ROUTEPEN_RESETVALUE;
  BTL_SPISLAVE->ROUTELOC0 = _USART_ROUTELOC0_RESETVALUE;
  BTL_SPISLAVE->ROUTELOC1 = _USART_ROUTELOC1_RESETVALUE;

  initialized = false;
}

/**
 * Write a data buffer to the master next time the master starts clocking SCLK.
 * This transfer will be non-blocking, and its progress can be tracked through
 * @ref spislave_getTxBytesLeft
 *
 * @param[in] buffer   The data buffer to send
 * @param[in] length   Amount of bytes in the buffer to send
 *
 * @return BOOTLOADER_OK if successful, error code otherwise
 */
int32_t spislave_sendBuffer(uint8_t* buffer,
                            size_t length)
{
  size_t iterator;

  BTL_ASSERT(initialized == true);
  BTL_ASSERT(length > 0);

  if (length >= BTL_SPISLAVE_TX_BUFFER_SIZE) {
    return BOOTLOADER_ERROR_SPISLAVE_ARGUMENT;
  }


  if (spislave_getTxBytesLeft() != 0) {
    return BOOTLOADER_ERROR_SPISLAVE_BUSY;
  }

  // Copy buffer
  for (iterator = 0; iterator < length; iterator++) {
    txBuffer[iterator] = buffer[iterator];
  }

  // DMA count field for transferring N bytes is defined as N-1.
  ldmaTxDesc.xfer.xferCnt = length - 1;

  // Kick off transfer. Done flag was already cleared by buffer flush.
  LDMA->CH[BTL_SPISLAVE_LDMA_TX_CHANNEL].LINK = (uint32_t)(&ldmaTxDesc)
                                          & _LDMA_CH_LINK_LINKADDR_MASK;
  LDMA->LINKLOAD = 1 << BTL_SPISLAVE_LDMA_TX_CHANNEL;

  // Wait for the DMA transfer to kick into action (otherwise getTxBytesLeft
  // gets confused)
  while (spislave_getTxBytesLeft() == 0);
  spislave_enableTransmitter(true);

  return BOOTLOADER_OK;
}

/**
 * Write one byte to the master in a blocking fashion.
 *  Warning: if the master goes down, this will block forever!
 *
 * @param[in] byte The byte to send
 *
 * @return BOOTLOADER_OK if succesful, error code otherwise
 */
int32_t spislave_sendByte(uint8_t byte)
{
  BTL_ASSERT(initialized == true);

  // Wait until previous LDMA transfer is done
  while (spislave_getTxBytesLeft() != 0);

  // Wait until there is room for one more byte
  while (!(BTL_SPISLAVE->STATUS & USART_STATUS_TXBL));

  // Send byte
  BTL_SPISLAVE->TXDATA = byte;

  // Wait until byte has been fully sent out
  while (!(BTL_SPISLAVE->STATUS & USART_STATUS_TXIDLE));

  return BOOTLOADER_OK;
}

/**
 * Gets the amount of bytes left in the TX data buffer.
 * WARNING: There is currently no way of correctly accounting for the last
 * three bytes going out, so this function might indicate 0 while it is
 * still clocking out the last 2/3 bytes.
 *
 * @return Amount of bytes in the transmit buffer still needing to go out
 */
size_t  spislave_getTxBytesLeft(void)
{
  size_t txLeft, txLeftPrev;

  BTL_ASSERT(initialized == true);

  if ((LDMA->CHEN & (1 << BTL_SPISLAVE_LDMA_TX_CHANNEL)) != 0) {
    // 1 byte in shift register, 2 in FIFO.
    txLeft = BTL_SPISLAVE_TX_BUFFER_SIZE + 1;
    txLeftPrev = BTL_SPISLAVE_TX_BUFFER_SIZE + 2;

    // Guard against hardware race conditions
    while (txLeftPrev != txLeft) {
      // TX bytes left = bytes in SPI plus bytes in DMA
      txLeftPrev = txLeft;
      // First, bytes remaining in LDMA
      txLeft = ((LDMA->CH[BTL_SPISLAVE_LDMA_TX_CHANNEL].CTRL
                 & _LDMA_CH_CTRL_XFERCNT_MASK)
                >> _LDMA_CH_CTRL_XFERCNT_SHIFT)
               + 1;
      // Then, add bytes remaining in USART
      txLeft += ((BTL_SPISLAVE->STATUS & _USART_STATUS_TXBUFCNT_MASK)
                 >> _USART_STATUS_TXBUFCNT_SHIFT);
    }
    return txLeft;
  } else {
    // DMA transfer is already done, but there might be leftover bytes in USART
    return ((BTL_SPISLAVE->STATUS & _USART_STATUS_TXBUFCNT_MASK)
            >> _USART_STATUS_TXBUFCNT_SHIFT);
  }
}

/**
 * Enable/disable MISO output
 *
 * @param[in] enable True to enable the transmitter, false to disable
 * @return BOOTLOADER_OK if succesful, error code otherwise
 */
void spislave_enableTransmitter(bool enable)
{
  BTL_ASSERT(initialized == true);

  if (enable) {
    BUS_RegMaskedSet(&BTL_SPISLAVE->ROUTEPEN, USART_ROUTEPEN_RXPEN);
  } else {
    BUS_RegMaskedClear(&BTL_SPISLAVE->ROUTEPEN, USART_ROUTEPEN_RXPEN);
    spislave_flush(true, false);
  }
}

/**
 * Enable/disable receiving bytes from the master into our internal buffer.
 *
 * @param[in] enable True to enable the receiver, false to disable
 * @return BOOTLOADER_OK if succesful, error code otherwise
 */
void spislave_enableReceiver(bool enable)
{
  BTL_ASSERT(initialized == true);

  if (enable) {
    BTL_SPISLAVE->CMD = USART_CMD_RXBLOCKDIS;
  } else {
    BTL_SPISLAVE->CMD = USART_CMD_RXBLOCKEN;
  }
}

/**
 * Gets the amount of bytes ready for reading.
 *
 * @return Amount of bytes in the receive buffer available for reading
 */
size_t  spislave_getRxAvailableBytes(void)
{
  size_t ldmaHead;
  uint32_t dst;

  BTL_ASSERT(initialized == true);

  // Get destination address for next transfer
  dst = LDMA->CH[BTL_SPISLAVE_LDMA_RX_CHANNEL].DST;

  if (dst == 0x0101) {
    // SYNC descriptor with bit 0 of MATCHEN and MATCHVAL set
    ldmaHead = 0;
  } else if (dst == 0x0202) {
    // SYNC descriptor with bit 1 of MATCHEN and MATCHVAL set
    ldmaHead = BTL_SPISLAVE_RX_BUFFER_SIZE/2;
  } else {
    // XFER descriptor with absolute address in buffer
    ldmaHead = dst - (uint32_t)(rxBuffer);
  }

  // Return difference between received head and LDMA head
  if (rxHead == ldmaHead) {
    return 0;
  } else if (rxHead < ldmaHead) {
    return ldmaHead - rxHead;
  } else {
    return BTL_SPISLAVE_RX_BUFFER_SIZE - (rxHead - ldmaHead);
  }
}

/**
 * Read from the RX buffer into a local buffer.
 *
 * @param[out] buffer The data buffer to receive into
 * @param[in] requestedLength Amount of bytes we'd like to read
 * @param[out] receivedLength Amount of bytes read
 * @param[in] blocking Indicate whether we should wait for requestedLength
 *   bytes to be available and read before returning, or we can read out
 *   whatever is currently in the buffer and return.
 * @param[in] timeout Number of milliseconds to wait for data in blocking mode
 *
 * @return BOOTLOADER_OK if succesful, error code otherwise
 */
int32_t spislave_receiveBuffer(uint8_t* buffer,
                           size_t requestedLength,
                           size_t* receivedLength,
                           bool blocking,
                           uint32_t timeout)
{
  size_t copiedBytes;

  BTL_ASSERT(initialized == true);
  BTL_ASSERT(requestedLength < BTL_SPISLAVE_RX_BUFFER_SIZE);

  // Check whether we have enough data
  // Optional spin for timeout cycles
  if (blocking) {
    if (timeout != 0) {
      delay_milliseconds(timeout, false);
    }

    while (spislave_getRxAvailableBytes() < requestedLength) {
      if (timeout != 0 && delay_expired()) {
        break;
      }
    }
  }

  if (requestedLength > spislave_getRxAvailableBytes()) {
    requestedLength = spislave_getRxAvailableBytes();
  }

  // Copy up to requested bytes to given buffer
  copiedBytes = 0;
  while (copiedBytes < requestedLength) {
    buffer[copiedBytes] = rxBuffer[rxHead];
    copiedBytes++;
    rxHead++;

    if (rxHead == BTL_SPISLAVE_RX_BUFFER_SIZE) {
      rxHead = 0;
      // Completed processing of second half of the buffer, mark it as available
      // for LDMA again by setting SYNC[1]
      BUS_RegMaskedSet(&LDMA->SYNC, 1 << 1);
    }
    else if (rxHead == BTL_SPISLAVE_RX_BUFFER_SIZE/2) {
      // Completed processing of first half of the buffer, mark it as available
      // for LDMA again by setting SYNC[0]
      BUS_RegMaskedSet(&LDMA->SYNC, 1 << 0);
    }

    // TODO: Check overflow by checking both SYNC[0] and SYNC[1]
  }

  if ((uint32_t)receivedLength != 0UL) {
    *receivedLength = copiedBytes;
  }

  return BOOTLOADER_OK;
}

/**
 * Get one byte from the SPI slave in a blocking fashion.
 *  Warning: if the master never clocks in a byte,
 *  this function will block forever!
 *
 * @param[out] byte The byte to send
 *
 * @return BOOTLOADER_OK if succesful, error code otherwise
 */
int32_t spislave_receiveByte(uint8_t* byte)
{
  return spislave_receiveBuffer(byte, 1, (size_t*)0UL, true, 0);
}

/**
 * Flushes one or both buffers.
 *
 * @param[in] flushTx Flushes the transmit buffer when true
 * @param[in] flushRx Flushes the receive buffer when true
 */
void spislave_flush(bool flushTx, bool flushRx)
{
  BTL_ASSERT(initialized == true);

  if (flushTx) {
    BUS_RegMaskedClear(&LDMA->CHEN, 1 << BTL_SPISLAVE_LDMA_TX_CHANNEL);
    BTL_SPISLAVE->CMD = USART_CMD_CLEARTX;
  }

  if (flushRx) {
    BUS_RegMaskedClear(&LDMA->CHEN, 1 << BTL_SPISLAVE_LDMA_RX_CHANNEL);
    LDMA->CH[BTL_SPISLAVE_LDMA_RX_CHANNEL].LINK = (uint32_t)(&ldmaRxDesc[0])
                                            & _LDMA_CH_LINK_LINKADDR_MASK;
    rxHead = 0;
    BTL_SPISLAVE->CMD = USART_CMD_CLEARRX;
    LDMA->LINKLOAD = (1 << BTL_SPISLAVE_LDMA_RX_CHANNEL);

    // Mark second half of RX buffer as ready
    BUS_RegMaskedSet(&LDMA->SYNC, 1 << 1);
  }
}
