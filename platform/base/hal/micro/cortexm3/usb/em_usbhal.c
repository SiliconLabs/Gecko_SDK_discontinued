/**************************************************************************//**
 * @file
 * @brief USB protocol stack library, low level USB peripheral access.
 * @author Nathaniel Ting
 * @version 3.20.3
 * <!-- Copyright 2013 by Silicon Laboratories. All rights reserved.     *80*-->
 *****************************************************************************/

#include PLATFORM_HEADER
#include "stack/include/ember.h"
#include "hal/hal.h"

#if defined(CORTEXM3_EM35X_USB)
#include "em_usb.h"
#include "em_usbtypes.h"
#include "em_usbhal.h"


/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */


bool usbVbusPresent;
extern USBD_Device_TypeDef *dev;

ALIGNMENT(8)
EndPointUnion usbBufferA = {0,};
uint8_t *EPOUTBUF[7] = {usbBufferA.eps.ep0OUT,
                    usbBufferA.eps.ep1OUT,
                    usbBufferA.eps.ep2OUT,
                    usbBufferA.eps.ep3OUT,
                    usbBufferA.eps.ep4OUT,
                    usbBufferA.eps.ep5OUT,
                    usbBufferA.eps.ep6OUT};
uint8_t *EPINBUF[7]  = {usbBufferA.eps.ep0IN,
                      usbBufferA.eps.ep1IN,
                      usbBufferA.eps.ep2IN,
                      usbBufferA.eps.ep3IN,
                      usbBufferA.eps.ep4IN,
                      usbBufferA.eps.ep5IN,
                      usbBufferA.eps.ep6IN};
volatile uint32_t *EPOUTBUFSIZE[7] = {&USB_RXBUFSIZEEP0A,
                                    &USB_RXBUFSIZEEP1A,
                                    &USB_RXBUFSIZEEP2A,
                                    &USB_RXBUFSIZEEP3A,
                                    &USB_RXBUFSIZEEP4A,
                                    &USB_RXBUFSIZEEP5A,
                                    &USB_RXBUFSIZEEP6A};

volatile uint32_t *EPINBUFSIZE[7] =  {&USB_TXBUFSIZEEP0A,
                                    &USB_TXBUFSIZEEP1A,
                                    &USB_TXBUFSIZEEP2A,
                                    &USB_TXBUFSIZEEP3A,
                                    &USB_TXBUFSIZEEP4A,
                                    &USB_TXBUFSIZEEP5A,
                                    &USB_TXBUFSIZEEP6A};
void USBDHAL_Connect( void )
{
  #if USB_SELFPWRD_STATE==1 && defined(VBUSMON)

  //For self-powered devices, The USB specification requires that the pull-up
  //resistor is disconnected if VBUS is not connected.
  vbusMonCfg();
  if(usbVbusPresent) {
    //Set ENUMCTRL ouput-high to enumerate.
    ENUMCTRL_SETCFG(GPIOCFG_OUT);
    ENUMCTRL_SET();
  }
  #else // USB_SELFPWRD_STATE==1 && defined(VBUSMON)
    //Set ENUMCTRL ouput-high to enumerate.
    ENUMCTRL_SETCFG(GPIOCFG_OUT);
    ENUMCTRL_SET();
  #endif // USB_SELFPWRD_STATE==1 && defined(VBUSMON)

}

void USBDHAL_Disconnect( void )
{
  ENUMCTRL_SETCFG(GPIOCFG_IN);
  ENUMCTRL_CLR();
}

USB_Status_TypeDef USBDHAL_CoreInit( uint32_t totalRxFifoSize,
                                     uint32_t totalTxFifoSize )
{

  //Configure PA0 and PA1 in analog mode for USB
  SET_REG_FIELD(GPIO_PACFGL, PA0_CFG, GPIOCFG_ANALOG);
  SET_REG_FIELD(GPIO_PACFGL, PA1_CFG, GPIOCFG_ANALOG);
  
  USB_BUFBASEA = (uint32_t)usbBufferA.allEps;
  MEMSET(usbBufferA.allEps, 0, USB_BUFFER_SIZE);

  // Double buffering currently not supported
  // #ifdef USB_DOUBLE_BUFFER  
  //   USB_BUFBASEB = (uint32_t)usbBufferB.allEps;
  //   MEMSET(usbBufferB.allEps, 0, USB_BUFFER_SIZE);
  //   //Refer to "handle buffer B" in the driveEp3Tx() function above
  //   //to learn why double buffering is not enabled by default.
  //   
  //   //Enable double buffering bulk EP3.
  //   USB_CTRL |= (USB_ENBUFOUTEP3B | USB_ENBUFINEP3B);
  // #endif // USB_DOUBLE_BUFFER
  
  //Use the SELFPWDRD and REMOTEWKUPEN state defines to set the appropriate
  //bits in USB_CTRL
  USB_CTRL = (USB_CTRL & (~(USB_SELFPWRD_MASK | USB_REMOTEWKUPEN_MASK))) |
             ((USB_SELFPWRD_STATE<<USB_SELFPWRD_BIT) |
              (USB_REMOTEWKUPEN_STATE<<USB_REMOTEWKUPEN_BIT));

#ifdef  EMBER_EMU_TEST
  //Select which GPIO output is used for connect/disconnect. Any
  //value > 47, selects disconnected state.
  EMU_USB_DISCONNECT = PORTA_PIN(2);
#endif //EMBER_EMU_TEST
  

  return USB_STATUS_OK;
}


void USBDHAL_AbortAllTransfers( USB_Status_TypeDef reason )
{
  int i;
  USBD_Ep_TypeDef *ep;
  USB_XferCompleteCb_TypeDef callback;

  if ( reason != USB_STATUS_DEVICE_RESET )
  {
    // unsupported
    // USBDHAL_AbortAllEps();
  }

  for ( i = 1; i <= NUM_EP_USED; i++ )
  {
    ep = &(dev->ep[i]);
    if ( ep->state != D_EP_IDLE )
    {
      ep->state = D_EP_IDLE;
      if ( ep->xferCompleteCb )
      {
        callback = ep->xferCompleteCb;
        ep->xferCompleteCb = NULL;

        if ( ( dev->lastState == USBD_STATE_CONFIGURED ) &&
             ( dev->state     == USBD_STATE_ADDRESSED  )    )
        {
          USBDHAL_DeactivateEp( ep );
        }
        callback( reason, ep->xferred, ep->remaining );
      }
    }
  }
}

#ifdef VBUSMON
//Use a selectable IRQ for monitoring VBUS on VBUSMON.
void vbusMonCfg(void)
{
  //VBUSMON just needs to be a simple input.
  VBUSMON_SETCFG();

  //start from a fresh state, just in case
  VBUSMON_INTCFG = 0;              //disable triggering
  INT_CFGCLR = VBUSMON_INT_EN_BIT; //clear top level int enable
  INT_GPIOFLAG = VBUSMON_FLAG_BIT; //clear stale interrupt
  INT_MISS = VBUSMON_MISS_BIT;     //clear stale missed interrupt
  //configure
  VBUSMON_SEL();             //point IRQ at the desired pin
  VBUSMON_INTCFG  = ((0 << GPIO_INTFILT_BIT) | //no filter
                     (3 << GPIO_INTMOD_BIT));  //3 = both edges
  
  usbVbusPresent = ((VBUSMON_IN & VBUSMON) == VBUSMON);
  
  INT_CFGSET = VBUSMON_INT_EN_BIT; //set top level interrupt enable
}
#endif //VBUSMON

#ifndef EMBER_APPLICATION_HAS_CUSTOM_SUSPEND_CALLBACK
  uint32_t savedPeripheralReg;
  void halSuspendCallback(void)
  {
    halSuspend();
    savedPeripheralReg = PERIPHERAL_DISABLE;
    PERIPHERAL_DISABLE |= ((1 << PERIDIS_ADC_BIT) |   \
                           (1 << PERIDIS_TIM2_BIT)|   \
                           (1 << PERIDIS_TIM1_BIT)|   \
                           (1 << PERIDIS_SC1_BIT) |   \
                           (1 << PERIDIS_SC2_BIT));
  }

  void halResumeCallback(void)
  {
    PERIPHERAL_DISABLE = savedPeripheralReg;
    halResume();
  }
#endif // EMBER_APPLICATION_HAS_CUSTOM_SUSPEND_CALLBACK


/** @endcond */
#endif //CORTEXM3_EM35X_USB

