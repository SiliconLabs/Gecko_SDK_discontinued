/**************************************************************************//**
 * @file hal/micro/cortexm3/usb/em_usbd.c
 * @brief USB protocol stack library, device API.
 * @author Nathaniel Ting
 * @version 3.20.3
 * <!-- Copyright 2013 by Silicon Laboratories. All rights reserved.     *80*-->
 *****************************************************************************/
#include PLATFORM_HEADER
#include "stack/include/ember.h"
#include "hal/hal.h"

#if CORTEXM3_EM35X_USB
#include "em_usb.h"
#include "em_usbhal.h"

#include "em_usbtypes.h"
#include "em_usbd.h"

#include "serial/serial.h"



/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

static USBD_Device_TypeDef device;
USBD_Device_TypeDef *dev = &device;

static const char *stateNames[] =
{
  [ USBD_STATE_NONE       ] = "NONE      ",
  [ USBD_STATE_ATTACHED   ] = "ATTACHED  ",
  [ USBD_STATE_POWERED    ] = "POWERED   ",
  [ USBD_STATE_DEFAULT    ] = "DEFAULT   ",
  [ USBD_STATE_ADDRESSED  ] = "ADDRESSED ",
  [ USBD_STATE_CONFIGURED ] = "CONFIGURED",
  [ USBD_STATE_SUSPENDED  ] = "SUSPENDED ",
  [ USBD_STATE_LASTMARKER ] = "UNDEFINED "
};

/** @endcond */


/** @addtogroup USB_DEVICE
 * @brief
 *
 * See em_usbd.c for source code.
 *  @{*/
/***************************************************************************//**
 * @brief
 *   Abort all pending transfers.
 *
 * @details
 *   Aborts transfers for all endpoints currently in use. Pending
 *   transfers on the default endpoint (EP0) are not aborted.
 ******************************************************************************/
void USBD_AbortAllTransfers( void )
{
  ATOMIC(
  USBDHAL_AbortAllTransfers( USB_STATUS_EP_ABORTED );
  )
}

/***************************************************************************//**
 * @brief
 *   Abort a pending transfer on a specific endpoint.
 *
 * @param[in] epAddr
 *   The address of the endpoint to abort.
 ******************************************************************************/
int USBD_AbortTransfer( int epAddr )
{
  USB_XferCompleteCb_TypeDef callback;
  USBD_Ep_TypeDef *ep = USBD_GetEpFromAddr( epAddr );



  // nUSBD_AbortTransfer(), Illegal request
  assert (ep!=NULL);

  // Assert above prevents dereferencing null pointer
  //cstat !PTR-null-cmp-bef !PTR-null-assign-pos
  // nUSBD_AbortTransfer(), Illegal endpoint
  assert (ep->num!=0);


  DECLARE_INTERRUPT_STATE;
  DISABLE_INTERRUPTS();
  if ( ep->state == D_EP_IDLE )
  {
    RESTORE_INTERRUPTS();
    return USB_STATUS_OK;
  }

  // USBD_AbortEp( ep );

  ep->state = D_EP_IDLE;
  if ( ep->xferCompleteCb )
  {
    callback = ep->xferCompleteCb;
    ep->xferCompleteCb = NULL;

    if ( ( dev->lastState == USBD_STATE_CONFIGURED ) &&
         ( dev->state     == USBD_STATE_ADDRESSED  )    )
    {
      // Assert above prevents dereferencing null pointer
      //cstat !PTR-null-assign-fun-pos
      USBDHAL_DeactivateEp( ep );
    }

    // DEBUG_TRACE_ABORT( USB_STATUS_EP_ABORTED );
    callback( USB_STATUS_EP_ABORTED, ep->xferred, ep->remaining );
  }

  RESTORE_INTERRUPTS();
  return USB_STATUS_OK;
}

/***************************************************************************//**
 * @brief
 *   Start USB device operation.
 *
 * @details
 *   Device operation is started by connecting a pullup resistor on the
 *   appropriate USB data line.
 ******************************************************************************/
void USBD_Connect( void )
{
  ATOMIC(
  USBDHAL_Connect();
  )
}

/***************************************************************************//**
 * @brief
 *   Stop USB device operation.
 *
 * @details
 *   Device operation is stopped by disconnecting the pullup resistor from the
 *   appropriate USB data line. Often referred to as a "soft" disconnect.
 ******************************************************************************/
void USBD_Disconnect( void )
{
  ATOMIC(
  USBDHAL_Disconnect();
  )
}

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
void USBD_SetUsbState( USBD_State_TypeDef newState )
{
  USBD_State_TypeDef currentState;

  currentState = dev->state;
  if ( newState == USBD_STATE_SUSPENDED )
  {
    dev->savedState = currentState;
  }

  dev->lastState = dev->state;
  dev->state = newState;

  if ( ( dev->callbacks->usbStateChange ) &&
       ( currentState != newState       )    )
  {
    dev->callbacks->usbStateChange( currentState, newState );
  }
}
/** @endcond */

/***************************************************************************//**
 * @brief
 *   Get current USB device state.
 *
 * @return
 *   Device USB state. See @ref USBD_State_TypeDef.
 ******************************************************************************/
USBD_State_TypeDef USBD_GetUsbState( void )
{
  return dev->state;
}

/***************************************************************************//**
 * @brief
 *   Get a string naming a device USB state.
 *
 * @param[in] state
 *   Device USB state. See @ref USBD_State_TypeDef.
 *
 * @return
 *   State name string pointer.
 ******************************************************************************/
const char *USBD_GetUsbStateName( USBD_State_TypeDef state )
{
  if ( state > USBD_STATE_LASTMARKER )
    state = USBD_STATE_LASTMARKER;

  return stateNames[ state ];
}


/***************************************************************************//**
 * @brief
 *   Check if an endpoint is busy doing a transfer.
 *
 * @param[in] epAddr
 *   The address of the endpoint to check.
 *
 * @return
 *   True if endpoint is busy, false otherwise.
 ******************************************************************************/
bool USBD_EpIsBusy( int epAddr )
{
  USBD_Ep_TypeDef *ep = USBD_GetEpFromAddr( epAddr );

  // USBD_EpIsBusy(), Illegal endpoint
  assert (ep!=NULL);

  // Assert above prevents dereferencing null pointer
  //cstat !PTR-null-cmp-bef !PTR-null-assign-pos
  if ( ep->state == D_EP_IDLE )
    return false;

  return true;
}

/***************************************************************************//**
 * @brief
 *   Set an endpoint in the stalled (halted) state.
 *
 * @param[in] epAddr
 *   The address of the endpoint to stall.
 *
 * @return
 *   @ref USB_STATUS_OK on success, else an appropriate error code.
 ******************************************************************************/
int USBD_StallEp( int epAddr )
{
  USB_Status_TypeDef retVal;
  USBD_Ep_TypeDef *ep = USBD_GetEpFromAddr( epAddr );


  #ifdef USB_DEBUG_STALL
    DEBUG_BUFFER += sprintf(DEBUG_BUFFER,"Stalling EP%d",ep->num);
    if (ep->in)
      DEBUG_BUFFER += sprintf(DEBUG_BUFFER,"in\r\n");
    else
      DEBUG_BUFFER += sprintf(DEBUG_BUFFER,"out\r\n");
  #endif

  // USBD_StallEp(), Illegal request
  assert (ep!=NULL);

  // USBD_StallEp(), Illegal endpoint
  // assert (ep->num!=0);

  // Assert above prevents dereferencing null pointer
  //cstat !PTR-null-cmp-bef-fun !PTR-null-assign-fun-pos
  ATOMIC(
    retVal = USBDHAL_StallEp( ep );
  )

  if ( retVal != USB_STATUS_OK )
  {
    retVal = USB_STATUS_ILLEGAL;
  }

  return retVal;
}


/***************************************************************************//**
 * @brief
 *   Reset stall state on a stalled (halted) endpoint.
 *
 * @param[in] epAddr
 *   The address of the endpoint to un-stall.
 *
 * @return
 *   @ref USB_STATUS_OK on success, else an appropriate error code.
 ******************************************************************************/
int USBD_UnStallEp( int epAddr )
{
  USB_Status_TypeDef retVal;
  USBD_Ep_TypeDef *ep = USBD_GetEpFromAddr( epAddr );


  #ifdef USB_DEBUG_STALL
    DEBUG_BUFFER += sprintf(DEBUG_BUFFER,"Unstalling EP%d",ep->num);
    if (ep->in)
      DEBUG_BUFFER += sprintf(DEBUG_BUFFER,"in\r\n");
    else
      DEBUG_BUFFER += sprintf(DEBUG_BUFFER,"out\r\n");
  #endif


  // USBD_StallEp(), Illegal request
  assert (ep!=NULL);
  // USBD_StallEp(), Illegal endpoint
  // assert (ep->num!=0);

  // Assert above prevents dereferencing null pointer
  //cstat !PTR-null-cmp-bef-fun !PTR-null-assign-fun-pos
  ATOMIC(
    retVal = USBDHAL_UnStallEp( ep );
  )

  if ( retVal != USB_STATUS_OK )
  {
    retVal = USB_STATUS_ILLEGAL;
  }

  return retVal;
}

/***************************************************************************//**
 * @brief
 *   Stop USB device stack operation.
 *
 * @details
 *   The data-line pullup resistor is turned off, USB interrupts are disabled,
 *   and finally the USB pins are disabled.
 ******************************************************************************/
void USBD_Stop( void )
{
  USBD_Disconnect();
  USBHAL_DisableGlobalInt();
  USBD_SetUsbState( USBD_STATE_NONE );
}


/***************************************************************************//**
 * @brief
 *   Initializes USB device hardware and internal protocol stack data structures,
 *   then connects the data-line (D+ or D-) pullup resistor to signal host that
 *   enumeration can begin.
 *
 * @note
 *   You may later use @ref USBD_Disconnect() and @ref USBD_Connect() to force
 *   reenumeration.
 *
 * @param[in] p
 *   Pointer to device initialization struct. See @ref USBD_Init_TypeDef.
 *
 * @return
 *   @ref USB_STATUS_OK on success, else an appropriate error code.
 ******************************************************************************/
int USBD_Init( const USBD_Init_TypeDef *p )
{

  int numEps;
  USBD_Ep_TypeDef *ep;
  uint8_t txFifoNum;
  uint8_t *conf, *confEnd;
  USB_EndpointDescriptor_TypeDef *epd;
  uint32_t totalRxFifoSize, totalTxFifoSize, numInEps, numOutEps;

  USBTIMER_Init();

  MEMSET( dev, 0, sizeof( USBD_Device_TypeDef ) );

  dev->setup                = dev->setupPkt;
  dev->deviceDescriptor     = p->deviceDescriptor;
  dev->configDescriptor     = (USB_ConfigurationDescriptor_TypeDef*)
                              p->configDescriptor;
  dev->stringDescriptors    = p->stringDescriptors;
  dev->numberOfStrings      = p->numberOfStrings;
  dev->state                = USBD_STATE_LASTMARKER;
  dev->savedState           = USBD_STATE_NONE;
  dev->lastState            = USBD_STATE_NONE;
  dev->callbacks            = p->callbacks;
  #if USB_REMOTEWKUPEN_STATE
  dev->remoteWakeupEnabled  = true;
  #else
  dev->remoteWakeupEnabled  = false;
  #endif


  /* Initialize EP0 */

  ep                 = &dev->ep[ 0 ];
  ep->in             = false;
  ep->buf            = NULL;
  ep->num            = 0;
  ep->mask           = 1;
  ep->addr           = 0;
  ep->type           = USB_EPTYPE_CTRL;
  ep->txFifoNum      = 0;
  ep->packetSize     = USB_EP0_SIZE;
  ep->remaining      = 0;
  ep->xferred        = 0;
  ep->state          = D_EP_IDLE;
  ep->xferCompleteCb = NULL;
  ep->fifoSize       = USB_EP0_SIZE / 4;

  totalTxFifoSize = ep->fifoSize * p->bufferingMultiplier[ 0 ];
  totalRxFifoSize = (ep->fifoSize + 1) * p->bufferingMultiplier[ 0 ];

  /* Parse configuration decriptor */
  numEps = 0;
  numInEps  = 0;
  numOutEps = 0;
  conf = (uint8_t*)dev->configDescriptor;
  confEnd = conf + dev->configDescriptor->wTotalLength;

  txFifoNum = 1;


  #ifdef USB_DEBUG
  // DEBUG_BUFFER += sprintf(DEBUG_BUFFER,"ep-->\tin\ttype\tpacketSize\r\n");
  #endif

  while ( conf < confEnd )
  {
    // USBD_Init(), Illegal configuration descriptor
    assert (*conf);

    if ( *(conf + 1) == USB_ENDPOINT_DESCRIPTOR )
    {
      numEps++;
      epd = (USB_EndpointDescriptor_TypeDef*)conf;


      ep                 = &dev->ep[ numEps ];
      ep->in             = ( epd->bEndpointAddress & USB_SETUP_DIR_MASK ) != 0;
      ep->buf            = NULL;
      ep->addr           = epd->bEndpointAddress;
      ep->num            = ep->addr & USB_EPNUM_MASK;
      ep->mask           = 1 << ep->num;
      ep->type           = epd->bmAttributes & CONFIG_DESC_BM_TRANSFERTYPE;
      ep->packetSize     = epd->wMaxPacketSize;
      ep->remaining      = 0;
      ep->xferred        = 0;
      ep->state          = D_EP_IDLE;
      ep->xferCompleteCb = NULL;


      if ( ep->in )
      {
        numInEps++;
        ep->txFifoNum = txFifoNum++;
        ep->fifoSize = (ep->packetSize/4) * p->bufferingMultiplier[ numEps ];
        dev->inEpAddr2EpIndex[ ep->num ] = numEps;
        totalTxFifoSize += ep->fifoSize;

        // USBD_Init(), Illegal IN EP address
        assert (ep->num < MAX_NUM_IN_EPS);

      }
      else
      {
        numOutEps++;
        ep->fifoSize = (ep->packetSize/4 + 1) * p->bufferingMultiplier[ numEps ];
        dev->outEpAddr2EpIndex[ ep->num ] = numEps;
        totalRxFifoSize += ep->fifoSize;

        // USBD_Init(), Illegal OUT EP address
        assert (ep->num < MAX_NUM_OUT_EPS);
      }
    }
    conf += *conf;

  }

  /* Rx-FIFO size: SETUP packets : 4*n + 6    n=#CTRL EP's
   *               GOTNAK        : 1
   *               Status info   : 2*n        n=#OUT EP's (EP0 included) in HW
   */
  totalRxFifoSize += 10 + 1 + ( 2 * (MAX_NUM_OUT_EPS + 1) );

  // USBD_Init(), Illegal EP count
  assert(numEps == NUM_EP_USED);

  // USBD_Init(), Illegal IN EP count
  assert (numInEps < MAX_NUM_IN_EPS);

  // USBD_Init(), Illegal OUT EP count
  assert (numOutEps < MAX_NUM_OUT_EPS);

  DECLARE_INTERRUPT_STATE;
  DISABLE_INTERRUPTS();

  USBHAL_DisableGlobalInt();

  if ( USBDHAL_CoreInit( 1, 1 ) == USB_STATUS_OK )
  {
    USBDHAL_EnableUsbResetInt();
    USBHAL_EnableGlobalInt();

    // NVIC_ClearPendingIRQ( USB_IRQn );
    // NVIC_EnableIRQ( USB_IRQn );
  }
  else
  {

    RESTORE_INTERRUPTS();
  //   USBD_Init(), FIFO setup error
    assert(0);
    return USB_STATUS_ILLEGAL;
  }

  /* Enable EPs */
  uint8_t i;
  for (i=1;i<=numEps;i++)
  {

    #ifdef USB_DEBUG
    // DEBUG_BUFFER += sprintf(DEBUG_BUFFER,"EP%d\t%d\t%d\t%d\r\n",i,ep->in,ep->type,ep->packetSize);
    #endif
    ep = &dev->ep[ i ];
    if (ep->in)
    {
      USB_ENABLEIN  |= USB_ENABLEINEP0 << ep->num;
      INT_USBCFG    |= INT_USBTXACTIVEEP0 << ep->num;
    }
    else
    {
      USB_ENABLEOUT |= USB_ENABLEOUTEP0 << ep->num;
      INT_USBCFG    |= INT_USBRXVALIDEP0 << ep->num;
    }
  }

  /* Connect USB */
  USBDHAL_Connect();

  RESTORE_INTERRUPTS();
  return USB_STATUS_OK;
}

/***************************************************************************//**
 * @brief
 *   Start a write (IN) transfer on an endpoint.
 *
 * @param[in] epAddr
 *   Endpoint address.
 *
 * @param[in] data
 *   Pointer to transfer data buffer. This buffer must be WORD (4 byte) aligned.
 *
 * @param[in] byteCount
 *   Transfer length.
 *
 * @param[in] callback
 *   Function to be called on transfer completion. Supply NULL if no callback
 *   is needed. See @ref USB_XferCompleteCb_TypeDef.
 *
 * @return
 *   @ref USB_STATUS_OK on success, else an appropriate error code.
 ******************************************************************************/
int USBD_Write( int epAddr, void *data, int byteCount,
                USB_XferCompleteCb_TypeDef callback )
{
  #ifdef USB_DEBUG_WRITE
  DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "USBD_Write:%d\r\n",byteCount);
  #endif

  USBD_Ep_TypeDef *ep = USBD_GetEpFromAddr( epAddr );

  if ( ep == NULL )
  {
    #ifdef USB_DEBUG_WRITE
    DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "USBD_Write(), Illegal endpoint\r\n");
    #endif
    // USBD_Write(), Illegal endpoint
    assert( 0 );
    return USB_STATUS_ILLEGAL;
  }

  // USBD_Write(), Illegal transfer size
  // assert ((byteCount < MAX_XFER_LEN) && ((byteCount / ep->packetSize) < MAX_PACKETS_PR_XFER));

  // USBD_Write(), Misaligned data buffer
  if (data!=NULL)
   assert(!((uint32_t)data & 3));

  DECLARE_INTERRUPT_STATE;
  DISABLE_INTERRUPTS();

  if ( USBDHAL_EpIsStalled( ep ) )
  {
    // INT_Enable();
    RESTORE_INTERRUPTS();
    // USBD_Write(), Endpoint is halted
    #ifdef USB_DEBUG_WRITE
    DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "USBD_Write(), Endpoint is halted\r\n" );
    #endif

    return USB_STATUS_EP_STALLED;
  }

  if ( ep->state != D_EP_IDLE )
  {
    RESTORE_INTERRUPTS();
    #ifdef USB_DEBUG_WRITE
    DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "USBD_Write(), Endpoint is busy\r\n");
    DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "ep->state = %d\r\n",ep->state);
    #endif
    return USB_STATUS_EP_BUSY;
  }

  if ( ( ep->num > 0 ) && ( USBD_GetUsbState() != USBD_STATE_CONFIGURED ) )
  {
    #ifdef USB_DEBUG_WRITE
    DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "USBD_Write(), Device not configured\r\n");
    #endif
    RESTORE_INTERRUPTS();
    return USB_STATUS_DEVICE_UNCONFIGURED;
  }


  //if data is null, this is a zero length packet
  if (data == NULL)
  {
    ep->buf = NULL;
  }
  else
  {
    ep->buf = (uint8_t*)data;
  }

  ep->remaining = byteCount;
  ep->xferred   = 0;


  if ( ep->num == 0 )
  {
    ep->in = true;
  }
  // USBD_Write(), Illegal EP direction
  assert (ep->in == true);

  ep->state          = D_EP_TRANSMITTING;
  ep->xferCompleteCb = callback;

  // kickoff USB transfer
  USBD_ArmEp( ep );
  RESTORE_INTERRUPTS();

  return USB_STATUS_OK;

}

/***************************************************************************//**
 * @brief
 *   Start a read (OUT) transfer on an endpoint.
 *
 * @note
 *   The transfer buffer length must be a multiple of 4 bytes in length and
 *   WORD (4 byte) aligned. When allocating the buffer, round buffer length up.
 *   If it is possible that the host will send more data than your device
 *   expects, round buffer size up to the next multiple of maxpacket size.
 *
 * @param[in] epAddr
 *   Endpoint address.
 *
 * @param[in] data
 *   Pointer to transfer data buffer.
 *
 * @param[in] byteCount
 *   Transfer length.
 *
 * @param[in] callback
 *   Function to be called on transfer completion. Supply NULL if no callback
 *   is needed. See @ref USB_XferCompleteCb_TypeDef.
 *
 * @return
 *   @ref USB_STATUS_OK on success, else an appropriate error code.
 ******************************************************************************/
int USBD_Read( int epAddr, void *data, int byteCount,
               USB_XferCompleteCb_TypeDef callback )
{

  USBD_Ep_TypeDef *ep = USBD_GetEpFromAddr( epAddr );

  #ifdef USB_DEBUG_READ
  DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "USBD_Read:%X - %d\r\nep->in=%d\r\n", epAddr, ep->num, ep->in);
  #endif

  assert (ep!=NULL);


  // assert ((byteCount < MAX_XFER_LEN) && ((byteCount/ep->packetSize) < MAX_PACKETS_PR_XFER));


  if (data!=NULL)
    assert (!((uint32_t)data & 3));


  DECLARE_INTERRUPT_STATE;
  DISABLE_INTERRUPTS();

  // Assert above prevents dereferencing null pointer
  //cstat !PTR-null-assign-fun-pos
  if ( USBDHAL_EpIsStalled( ep ) )
  {
    RESTORE_INTERRUPTS();
    #ifdef USB_DEBUG_READ
    DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "USBD_Read(), Endpoint is halted\r\n" );
    #endif
    return USB_STATUS_EP_STALLED;
  }

  // Assert above prevents dereferencing null pointer
  //cstat !PTR-null-cmp-bef !PTR-null-assign-pos
  if ( ep->state == D_EP_TRANSMITTING )
  {
    RESTORE_INTERRUPTS();
    #ifdef USB_DEBUG_READ
    DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "USBD_Read(), Endpoint is busy\r\n" );
    #endif
    return USB_STATUS_EP_BUSY;
  }

  if ( ( ep->num > 0 ) && ( USBD_GetUsbState() != USBD_STATE_CONFIGURED ) )
  {
    RESTORE_INTERRUPTS();
    #ifdef USB_DEBUG_READ
    DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "\nUSBD_Read(), Device not configured" );
    #endif
    return USB_STATUS_DEVICE_UNCONFIGURED;
  }

  ep->buf       = (uint8_t*)data;
  ep->remaining = byteCount;
  ep->xferred   = 0;


  if ( ep->num == 0 )
  {
    ep->in = false;
  }

  // USBD_Read(), Illegal EP direction
  assert(ep->in == false);

  ep->state          = D_EP_RECEIVING;
  ep->xferCompleteCb = callback;

  // kickoff USB transfer
  USBD_ArmEp( ep );
  RESTORE_INTERRUPTS();
  return USB_STATUS_OK;
}



/***************************************************************************//**
 * @brief
 *   USB suspend delayed service routine
 *
 * @details
 *   This function keeps the device in a low power state in order to meet USB
 *   specification during USB suspend state.
 ******************************************************************************/
void usbSuspendDsr(void)
{
  if(dev->state == USBD_STATE_SUSPENDED) {

    #ifndef EMBER_NO_STACK
    emberStackPowerDown();
    #endif // EMBER_NO_STACK
    // halPowerDown();
    halSuspendCallback();
    //Turn idle sleep into USB sleep which divides down all the chip clocks,
    //by 4, except system timer.
    CPU_CLKSEL |= USBSUSP_CLKSEL_FIELD;
    halSleep(SLEEPMODE_IDLE);
  }
}



/***************************************************************************//**
 * @brief
 *   Perform a remote wakeup signalling sequence.
 *
 * @note
 *   It is the responsibility of the application to ensure that remote wakeup
 *   is not attempted before the device has been suspended for at least 5
 *   miliseconds. This function should not be called from within an interrupt
 *   handler.
 *
 * @return
 *   @ref USB_STATUS_OK on success, else an appropriate error code.
 ******************************************************************************/
int USBD_RemoteWakeup( void )
{
  #ifdef USB_DEBUG_SUSPEND
    DEBUG_BUFFER += sprintf(DEBUG_BUFFER,"RemoteWakeup...");
  #endif

  if ( ( dev->state != USBD_STATE_SUSPENDED ) ||
       ( dev->remoteWakeupEnabled == false  ) )
  {
    // Not suspend or remote wakeup not enabled
    #ifdef USB_DEBUG_SUSPEND
      DEBUG_BUFFER += sprintf(DEBUG_BUFFER,"illegal\r\n");
    #endif
    return USB_STATUS_ILLEGAL;
  }

  USBDHAL_SetRemoteWakeup();
  uint16_t timeout = 4; // Set for 4 ms. Host should take over within 1 ms
  //store start time, compare difference with current time with timeout period
  uint16_t startTime = halCommonGetInt16uMillisecondTick();
  uint16_t curTime = halCommonGetInt16uMillisecondTick();
  while ((elapsedTimeInt16u(startTime, curTime) <= timeout)
         && (dev->state == USBD_STATE_SUSPENDED)) { //exit if USB resumes
    curTime = halCommonGetInt16uMillisecondTick();
  }

  if (dev->state == USBD_STATE_SUSPENDED) { //record failure if USB fails to resume
    #ifdef USB_DEBUG_SUSPEND
      DEBUG_BUFFER += sprintf(DEBUG_BUFFER,"timed out\r\n");
    #endif
    return USB_STATUS_TIMEOUT;
  }
  #ifdef USB_DEBUG_SUSPEND
    DEBUG_BUFFER += sprintf(DEBUG_BUFFER,"succeeded\r\n");
  #endif

  return USB_STATUS_OK;
}


// DEBUG buffer with printout function, note buffer has no overflow protection
#ifdef USB_DEBUG
static char debugbuffer[4000];
char *DEBUG_BUFFER = debugbuffer;

void USBD_PrintDebug(void)
{
  debugbuffer[3999] = '\0';
  emberSerialGuaranteedPrintf(SER232,debugbuffer);
  MEMSET(debugbuffer, 0, 4000);
  DEBUG_BUFFER = debugbuffer;
}
#endif

#if EM_SERIAL3_ENABLED
// interface with serial/uart queues. Enables emberserialprintf
uint8_t dequeueTxIntoBuffer(uint8_t *data)
{
  EmSerialFifoQueue *q = (EmSerialFifoQueue *)emSerialTxQueues[3];
  uint8_t txSize=0;

  //If there are bytes in the Q and txSize hasn't maxed out, pull more
  //bytes off the Q into the DMA buffer
  while((q->used > 0) && (txSize < EP5_SIZE)) {
    *data = FIFO_DEQUEUE(q, emSerialTxQueueWraps[3]);
    data++;
    txSize++;
  }

  return txSize;
}

// interface with serial/uart queues. Enables emberSerialPrintf.
// EP_IN must be defined. Assumes EP1 size, 8 bytes, which may be larger
// depending on endpoint.
void usbTxData ()
{
  if (!USBD_EpIsBusy(CDC_EP_IN))
  {
    uint8_t data[EP5_SIZE];
    uint8_t txSize = dequeueTxIntoBuffer(data);
    if (txSize>0)
    {
      USBD_Write(CDC_EP_IN, data, txSize, NULL );
    }
  }
}

// interface with serial/uart library. Enables emberSerialGuaranteedPrintf
void usbForceTxData (uint8_t *data, uint8_t length)
{
  while (USBD_EpIsBusy(CDC_EP_IN)) {}
  USBD_Write(CDC_EP_IN, data, length, NULL );
  while (USBD_EpIsBusy(CDC_EP_IN)) {}
}

#endif //EM_SERIAL3_ENABLED

/** @} (end addtogroup USB_DEVICE) */

/******** THE REST OF THE FILE IS DOCUMENTATION ONLY !**********************//**
 * @addtogroup usb @{

  The source files for the USB device stack resides in the usb directory
  and follows the naming convention: em_usbd<em>nnn</em>.c/h.

  @li @ref usb_device_intro
  @li @ref usb_device_api
  @li @ref usb_device_conf

@n @section usb_device_intro Introduction

  The USB device protocol stack provides an API which makes it possible to
  create USB devices with a minimum of effort. The device stack supports control,
  bulk and interrupt transfers.

  The stack is highly configurable to suit various needs, it does also contain
  useful debugging features together with several demonstration projects to
  get you started fast.

  We recommend that you read through this documentation, then proceed to build
  and test a few example projects before you start designing your own device.

@n @section usb_device_api The device stack API

  This section contains brief descriptions of the functions in the API. You will
  find detailed information on input and output parameters and return values by
  clicking on the hyperlinked function names. It is also a good idea to study
  the code in the USB demonstration projects.

  Your application code must include one header file: @em em_usb.h.

  All functions defined in the API can be called from within interrupt handlers.

  <b>Pitfalls:</b>@n
    The USB peripheral will fill your receive buffers in quantities of WORD's
    (4 bytes). Transmit and receive buffers must be WORD aligned, in
    addition when allocating storage for receive buffers, round size up to
    next WORD boundary. If it is possible that the host will send more data
    than your device expects, round buffer size up to the next multiple of
    maxpacket size for the relevant endpoint to avoid data corruption.

    Transmit buffers passed to @htmlonly USBD_Write() @endhtmlonly must be
    statically allocated because @htmlonly USBD_Write() @endhtmlonly only
    initiates the transfer. When the host decide to actually perform the
    transfer, your data must be available.

  @n @ref USBD_Init() @n
    This function is called to register your device and all its properties with
    the device stack. The application must fill in a @ref USBD_Init_TypeDef
    structure prior to calling. Refer to @ref DeviceInitCallbacks for the
    optional callback functions defined within this structure. When this
    function has been called your device is ready to be enumerated by the USB
    host.

  @ref USBD_Read(), @ref USBD_Write() @n
    These functions initiate data transfers.
    @n @htmlonly USBD_Read() @endhtmlonly initiate a transfer of data @em
    from host @em to device (an @em OUT transfer in USB terminology).
    @n @htmlonly USBD_Write() @endhtmlonly initiate a transfer of data @em from
    device @em to host (an @em IN transfer).

    When the USB host actually performs the transfer, your application will be
    notified by means of a callback function which you provide (optionally).
    Refer to @ref TransferCallback for details of the callback functionality.

  @ref USBD_AbortTransfer(), @ref USBD_AbortAllTransfers() @n
    These functions terminate transfers that are initiated, but has not yet
    taken place. If a transfer is initiated with @htmlonly USBD_Read()
    or USBD_Write(), @endhtmlonly but the USB host never actually peform
    the transfers, these functions will deactivate the transfer setup to make
    the USB device endpoint hardware ready for new (and potentially) different
    transfers.

  @ref USBD_Connect(), @ref USBD_Disconnect() @n
    These functions turns the data-line (D+ or D-) pullup on or off. They can be
    used to force reenumeration. It's good practice to delay at least one second
    between @htmlonly USBD_Disconnect() and USBD_Connect() @endhtmlonly
    to allow the USB host to unload the currently active device driver.

  @ref USBD_EpIsBusy() @n
    Check if an endpoint is busy.

  @ref USBD_StallEp(), @ref USBD_UnStallEp() @n
    These functions stalls or un-stalls an endpoint. This functionality may not
    be needed by your application, but the USB device stack use them in response
    to standard setup commands SET_FEATURE and CLEAR_FEATURE. They may be useful
    when implementing some USB classes, e.g. a mass storage device use them
    extensively.

  @ref USBD_RemoteWakeup() @n
    Used in SUSPENDED state (see @ref USB_Status_TypeDef) to signal resume to
    host. It's the applications responsibility to adhere to the USB standard
    which states that a device can not signal resume before it has been
    SUSPENDED for at least 5 ms. The function will also check the configuration
    descriptor defined by the application to see if it is legal for the device
    to signal resume.

  @ref USBD_GetUsbState() @n
    Returns the device USB state (see @ref USBD_State_TypeDef). Refer to
    Figure 9-1. "Device State Diagram" in the USB revision 2.0 specification.

  @ref USBD_GetUsbStateName() @n
    Returns a text string naming a given USB device state.

  @n @anchor TransferCallback <b>The transfer complete callback function:</b> @n
    @n USB_XferCompleteCb_TypeDef() is called when a transfer completes. It is
    called with three parameters, the status of the transfer, the number of
    bytes transferred and the number of bytes remaining. It may not always be
    needed to have a callback on transfer completion, but you should keep in
    mind that a transfer may be aborted when you least expect it. A transfer
    will be aborted if host stalls the endpoint, if host resets your device, if
    host unconfigures your device or if you unplug your device cable and the
    device is selfpowered.
    @htmlonly USB_XferCompleteCb_TypeDef() @endhtmlonly is also called if your
    application use @htmlonly USBD_AbortTransfer() or USBD_AbortAllTransfers()
    @endhtmlonly calls.
    @note This callback is called from within an interrupt handler with
          interrupts disabled.

  @n @anchor DeviceInitCallbacks <b>Optional callbacks passed to the stack via
    the @ref USBD_Init() function:</b> @n
    @n These callbacks are all optional, and it is up to the application
    programmer to decide if the application needs the functionality they
    provide.
    @note These callbacks are all called from within an interrupt handler
          with interrupts disabled.

  USBD_UsbResetCb_TypeDef() is called each time reset signalling is sensed on
    the USB wire.

  @n USBD_SofIntCb_TypeDef() is called with framenumber as a parameter on
    each SOF interrupt.

  @n USBD_DeviceStateChangeCb_TypeDef() is called whenever the device state
    change. Useful for detecting e.g. SUSPENDED state change in order to reduce
    current consumption of buspowered devices. The USB HID keyboard example
    project has a good example on how to use this callback.

  @n USBD_IsSelfPoweredCb_TypeDef() is called by the device stack when host
    queries the device with a standard setup GET_STATUS command to check if the
    device is currently selfpowered or buspowered. This feature is only
    applicable on selfpowered devices which also works when only buspower is
    available.

  @n USBD_SetupCmdCb_TypeDef() is called each time a setup command is
    received from host. Use this callback to override or extend the default
    handling of standard setup commands, and to implement class or vendor
    specific setup commands. The USB HID keyboard example project has a good
    example on how to use this callback.

@n @section usb_device_conf Configuring the device stack

  Your application must provide a header file named @em usbconfig.h. This file
  must contain the following \#define:@n @n
  @verbatim
#define NUM_EP_USED n    // Your application use 'n' endpoints in
                         // addition to endpoint 0. @endverbatim

 * @}**************************************************************************/

#endif //CORTEXM3_EM35X_USB

