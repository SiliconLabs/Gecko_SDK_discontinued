/**************************************************************************//**
 * @file
 * @brief USB protocol stack library, USB device peripheral interrupt handlers.
 * @author Nathaniel Ting
 * @version 3.20.3
 * <!-- Copyright 2013 by Silicon Laboratories. All rights reserved.     *80*-->
 *****************************************************************************/

#include PLATFORM_HEADER
#include "stack/include/ember.h"
#include "hal/hal.h"

#if defined(CORTEXM3_EM35X_USB)
// Only define this ISR if USB is on and EnergyMicro USB is enabled. When this
// function is defined it will cause extra code to be pulled in even if the ISR
// will never fire.

#include "em_usb.h"
#include "em_usbhal.h"
#include "em_usbtypes.h"
#include "em_usbd.h"
#include "serial/serial.h"

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

USBD_Ep_TypeDef *ep;

void halUsbIsr(void)
{
  uint32_t byteCount;

  #if defined(BOOTLOADER) && defined (VBUSMON)
  VBUSMON_ISR();
  #endif

  uint32_t status = INT_USBFLAG;
  INT_USBFLAG = status;

  if ( status == 0 )
  {
    return;
  }
  //Ensure EP0 IN is not stalled.  Refer to receiving getQualifierDescriptor
  //for more information.
  USB_STALLIN &= ~USB_STALLINEP0;

  if(status & INT_USBWAKEUP & INT_USBCFG)
  {
    #ifdef USB_DEBUG_SUSPEND
      DEBUG_BUFFER += sprintf(DEBUG_BUFFER,"INT_USBWAKEUP\r\n");
    #endif

    //The INT_USBWAKEUP interrupt triggers for a successful remote wakeup
    //This is essentially a resume only intiated by the device instaed of host
    //need INT_USBCFG mask to enable turning on/off remote wakeup interrupt

    #ifndef BOOTLOADER
      USBD_SetUsbState( dev->lastState );
      halResumeCallback();
      #ifndef EMBER_NO_STACK
      emberStackPowerUp();
      #endif // EMBER_NO_STACK
    #endif
  }

  if(status & INT_USBRESUME)
  {
    #ifdef USB_DEBUG_SUSPEND
      DEBUG_BUFFER += sprintf(DEBUG_BUFFER,"INT_USBRESUME\r\n");
    #endif
    //The INT_USBRESUME interrupt pulls us out of suspend while the USB
    //core auto clears the USBSUSP_CLKSEL bit (used when entering suspend).

    #ifndef BOOTLOADER
      USBD_SetUsbState( dev->lastState );
      halResumeCallback();
      #ifndef EMBER_NO_STACK
      emberStackPowerUp();
      #endif // EMBER_NO_STACK
    #endif
  }

  if(status & INT_USBSUSPEND)
  {
    #ifdef USB_DEBUG_SUSPEND
      DEBUG_BUFFER += sprintf(DEBUG_BUFFER,"INT_USBSUSPEND\r\n");
    #endif
    //If suspend if being supported, we now have 7ms to get down to
    //suspend current.  Setting usb state to USBD_STATE_SUSPENDED will allow
    //the usbSuspendDsr() function to place us in the appropriate low
    //power clocking and idle mode.
    //
    //NOTE:  The stack and HAL should be shutdown like deep sleeping before
    //       suspend is entered.  USB sleep involves slowing down peripheral
    //       clocking, but it does not stop the clocks.  This means
    //       peripherals will keep running if they're enabled.  Being at
    //       a slower clock speed can severely disrupt application behavior
    //       that depends on clean peripherial behavior at a desired clock
    //       rate.  For example, problems with the UART would be very obvious.

    #ifndef BOOTLOADER
    USBD_SetUsbState( USBD_STATE_SUSPENDED );
    #endif
    //USBTODO- Software needs to remember when INT_USBSUSPEND fires so that
    //  it can return to suspend if a non-USB event pulls us out of
    //  the USB sleep mode.  Only INT_USBRESUME is allowed to keep us
    //  out of suspend.
  }

  if(status & INT_USBRESET)
  {
    #ifdef USB_DEBUG_INT
      DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "\r\n\r\nINT_USBRESET\r\n");
    #endif
    //When a USB reset occurs it resets the core but not the DMA.  To ensure
    //transactions after a reset are fresh all buffers (DMA) need to be
    //cleared.
    USB_BUFCLR = (USB_BUFCLRINEP6 |
                  USB_BUFCLRINEP5 |
                  USB_BUFCLRINEP4 |
                  USB_BUFCLRINEP3 |
                  USB_BUFCLRINEP2 |
                  USB_BUFCLRINEP1 |
                  USB_BUFCLRINEP0 );

    /* Setup EP0 to receive SETUP packets */
    USBDHAL_StartEp0Setup( dev );
    USBD_Ep_TypeDef *ep;
    ep = &dev->ep[ 0 ];
    ep->state = D_EP_IDLE;
    ep->remaining = 0;

    ep->in = true;
    USBDHAL_UnStallEp( ep );              /* Stall Ep0 IN                 */
    ep->in = false;                     /* OUT for next SETUP           */
    USBDHAL_UnStallEp( ep );              /* Stall Ep0 OUT                */

    if ( dev->callbacks->usbReset )
    {
      dev->callbacks->usbReset();
    }

    // USBD_SetUsbState( USBD_STATE_DEFAULT );
    // USBDHAL_AbortAllTransfers( USB_STATUS_DEVICE_RESET );
  }

  if(USB_RXBUFSIZEEP0A == 0)
  {
    USB_BUFCLR = USB_BUFCLRINEP0;

    USBDHAL_StartEp0Setup( dev );
    USBD_Ep_TypeDef *ep;
    ep = &dev->ep[ 0 ];
    ep->state = D_EP_IDLE;
    ep->remaining = 0;
  }

  //==== RX functionality ====
  if (status & INT_USBRXVALID)
  {
    int epnum;
    uint16_t epmask;

    // sweep through eps to determine which EP the interrupt is on
    for ( epnum = 0,                epmask = INT_USBRXVALIDEP0;
          epnum <= MAX_NUM_OUT_EPS;
          epnum++,                  epmask <<= 1 )
    {
      if (status & epmask)
      {
        // determine how many bytes were received
        volatile uint32_t *bufsize = EPOUTBUFSIZE[epnum];

        #ifdef USB_DEBUG_INT
          #ifdef USB_DEBUG_VERBOSE
            DEBUG_BUFFER += sprintf(DEBUG_BUFFER,"RX%d-->",epnum);
            uint8_t i;
            for(i=0;i<*bufsize;i++)
            {
              DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "%02X", EPOUTBUF[epnum][i]);
            }
            DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "  %d bytes\r\n", *bufsize);
          #else
            DEBUG_BUFFER += sprintf(DEBUG_BUFFER,"RX%d-->%d bytes\r\n",epnum, *bufsize);
          #endif
        #endif

        // update endpoint state
        ep = USBD_GetEpFromAddr( epnum );
        byteCount = EFM32_MIN(ep->remaining,*bufsize);
        ep->remaining -= byteCount;
        ep->xferred   += byteCount;

        // #ifdef USB_DEBUG_INT
        //   DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "  %d remaining, %d xferred\r\n", ep->remaining, ep->xferred);
        // #endif

        // copy buffer to endpoint specified buffer location before erasing
        if (ep->state == D_EP_RECEIVING)
        {
          MEMMOVE(ep->buf,EPOUTBUF[ep->num], *bufsize);
        }

        // dispatch endpoint handler, erase buffer, reenable endpoint RX
        if (epnum ==0)
        {
          USBDEP_Ep0Handler( dev );
        }
        else
        {
          #if EM_SERIAL3_ENABLED
            halInternalUart3RxIsr(EPOUTBUF[ep->num], *bufsize);
          #endif //EM_SERIAL3_ENABLED

          USBDEP_EpHandler(ep->addr);
        }

        MEMSET(EPOUTBUF[epnum], 0, *bufsize);
        USB_RXVALID = ((USB_RXVALIDEP0A<<epnum) | (USB_RXVALIDEP0B<<epnum));
      }
    }
  }

  //==== TX functionality ====
  //NOTE: INT_USBTXACTIVEEPx interrupts fire on USB_TXACTIVEEPxy falling edge.
  if (status & INT_USBTXACTIVE)
  {
    int epnum;
    uint16_t epmask;

    // sweep through eps to determine which EP the interrupt is on
    for ( epnum = 0,                epmask = INT_USBTXACTIVEEP0;
          epnum <= MAX_NUM_OUT_EPS;
          epnum++,                  epmask <<= 1 )
    {
      if (status & epmask)
      {
        // determine how many bytes were transmitted
        volatile uint32_t *bufsize = EPINBUFSIZE[epnum];

        #ifdef USB_DEBUG_INT
          DEBUG_BUFFER += sprintf(DEBUG_BUFFER,"TX%d\r\n",epnum);
        #endif

        // update endpoint state
        ep = USBD_GetEpFromAddr( USB_SETUP_DIR_MASK | epnum );
        byteCount = EFM32_MIN(ep->remaining,*bufsize);
        ep->remaining -= byteCount;
        ep->xferred   += byteCount;

        // dispatch endpoint handler
        if (epnum == 0)
        {
          USBDEP_Ep0Handler( dev );
        }
        else
        {
          USBDEP_EpHandler(ep->addr);
        }
      }
    }
  }
}

#ifdef VBUSMON
void VBUSMON_ISR(void)
{
  //VBUSMON is used for sensing VBUS indicating if USB is physically connected.
  //An IRQ is used to monitor for any edge change on VBUSMON.
  //For self-powered devices, VBUS monitoring is required for the EM358 device
  //to function cleanly across physical connect/disconnects of the USB.
  //The USB specification requires that the pull-up resistor for
  //enumerating is disconnected if VBUS is not connected.

  //clear int before read to avoid potential of missing interrupt
  INT_MISS = VBUSMON_MISS_BIT;     //clear missed interrupt flag
  INT_GPIOFLAG = VBUSMON_FLAG_BIT; //clear top level interrupt flag

  //Because this ISR/IRQ triggers on both edges, usbVbusPresent is used
  //to track the state of VBUS.
  //Always directly read VBUSMON to ensure this ISR is changing to the correct,
  //current state.
  //Falling edge: VBUS removed - Must set ENUMCTRL to input-low.
  //Rrising edge: VBUS applied - If usbConfigEnumerate() has been called and
  //this device is ready to enumerate, set ENUMCTRL to output-high and
  //perform enumeration.
  usbVbusPresent = ((VBUSMON_IN & VBUSMON) == VBUSMON);

  if(usbVbusPresent)
  {
    if (USBD_GetUsbState()==USBD_STATE_CONFIGURED)
      return;
    //Only attempt enumerate if usbConfigEnumerate has already been called.
    //Set ENUMCTRL ouput-high to enumerate.
    ENUMCTRL_SETCFG(GPIOCFG_OUT);
    ENUMCTRL_SET();

    // halSetLed(BOARDLED0);
    USBD_SetUsbState(USBD_STATE_DEFAULT);
  }
  else
  {
    //Configure ENUMCTRL as an input so the device de-enumurates and
    //the pin does not put any load on the bus.  (The spec says the
    //pull-up resistor used for enumeration should be tri-state.  On
    //EM358 devices the input configuration is the best available choice
    //since tri-state isn't possible.)
    //Clear down ENUMCTRL to be ready for the next time enumerating.
    ENUMCTRL_SETCFG(GPIOCFG_IN);
    ENUMCTRL_CLR();
    // halClearLed(BOARDLED0);
    USBD_SetUsbState(USBD_STATE_NONE);
  }
}
#endif // VBUSMON

/** @endcond */

#endif // CORTEXM3_EM35X_USB
