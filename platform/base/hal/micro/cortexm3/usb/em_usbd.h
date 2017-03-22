/***************************************************************************//**
 * @file
 * @brief USB protocol stack library API for EFM32.
 * @author Nathaniel Ting
 * @version 3.20.3
 * <!-- Copyright 2013 by Silicon Laboratories. All rights reserved.     *80*-->
 *****************************************************************************/
#ifndef __EM_USBD_H
#define __EM_USBD_H

#include "em_usb.h"

#include PLATFORM_HEADER
#include "stack/include/ember.h"
#include "hal/hal.h"
#include "em_usbhal.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */


extern USBD_Device_TypeDef *dev;
extern volatile bool USBD_poweredDown;


static inline void USBD_ArmEp0( USBD_Ep_TypeDef *ep );
static inline void USBD_ArmEpN( USBD_Ep_TypeDef *ep );
static inline void USBD_AbortEp( USBD_Ep_TypeDef *ep );

void USBD_SetUsbState( USBD_State_TypeDef newState );

int  USBDCH9_SetupCmd( USBD_Device_TypeDef *device );

void USBDEP_Ep0Handler( USBD_Device_TypeDef *device );
void USBDEP_EpHandler( uint8_t epAddr );

static inline void USBD_ActivateAllEps( bool forceIdle )
{
  // int i;

  // for ( i = 1; i <= NUM_EP_USED; i++ )
  // {
  //   USBDHAL_ActivateEp( &dev->ep[ i ], forceIdle );
  // }
}

static inline void USBD_ArmEp( USBD_Ep_TypeDef *ep )
{
  if ( ep->num == 0 )
  {
    USBD_ArmEp0( ep );
  }
  else
  {
    USBD_ArmEpN( ep );
  }
}

static inline void USBD_ArmEp0( USBD_Ep_TypeDef *ep )
{
  if ( ep->in )
  {
    uint8_t txSize = (ep->remaining > ep->packetSize) ? ep->packetSize : ep->remaining;

    #ifdef USB_DEBUG_EP0
      #ifdef USB_DEBUG_VERBOSE
        DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "in-->");
        uint8_t i;
        for(i=0;i<txSize;i++)
        {
          DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "%02X", ep->buf[i]);
        }
        DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "\r\n");
      #else

        DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "in\r\n");
      #endif
    #endif

    if (txSize>0)
    {
      MEMMOVE(EPINBUF[0], ep->buf, txSize);
    }
    ep->buf += txSize;

    USB_TXBUFSIZEEP0A = txSize;
    USB_TXLOAD = USB_TXLOADEP0A;
  }
  else
  {
    // assert (0);
  }
}

static inline void USBD_ArmEpN( USBD_Ep_TypeDef *ep )
{
  #if defined(USB_DEBUG_READ) || defined(USB_DEBUG_WRITE)
  DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "armEp%d",ep->num);
  #endif

  if ( ep->in )
  {
    #ifdef USB_DEBUG_WRITE
    DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "in\r\n");
    #endif
    USBDHAL_StartEpIn( ep );
  }
  else
  {
    #ifdef USB_DEBUG_READ
    DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "out\r\n");
    #endif
    USBDHAL_StartEpOut( ep );
  }

  #ifdef USB_DEBUG_EP
    #ifdef USB_DEBUG_VERBOSE
      DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "armEp%d-->\r\n",ep->num);
      DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "USB_ENABLEIN\tUSB_ENABLEOUT\tUSB_CTRL\tINT_USBCFG\r\n");
      DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "%X          \t%X           \t%X      \t%X\r\n\n",USB_ENABLEIN,USB_ENABLEOUT,USB_CTRL,INT_USBCFG);
    #else
      DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "armEp%d\r\n",ep->num);
    #endif
  #endif
}

static inline void USBD_DeactivateAllEps( USB_Status_TypeDef reason )
{
  int i;
  USBD_Ep_TypeDef *ep;

  for ( i = 1; i <= NUM_EP_USED; i++ )
  {
    ep = &dev->ep[ i ];

    if ( ep->state == D_EP_IDLE )
    {
      USBDHAL_DeactivateEp( ep );
    }
  }

  USBDHAL_AbortAllTransfers( reason );
}

static inline USBD_Ep_TypeDef *USBD_GetEpFromAddr( uint8_t epAddr )
{
  int epIndex;
  USBD_Ep_TypeDef *ep = NULL;

  if ((epAddr & USB_EPNUM_MASK) > MAX_USB_EP_NUM)
  {
    return NULL;
  }
  if ( epAddr & USB_SETUP_DIR_MASK )
  {
    // Above check prevents out of bound access since inEpAddr2EpIndex has
    // size MAX_USB_EP_NUM + 1.
    //cstat !ARR-inv-index-pos !MISRAC2012-Rule-18.1_b
    epIndex = dev->inEpAddr2EpIndex[ epAddr & USB_EPNUM_MASK ];
  }
  else
  {
    // Above check prevents out of bound access since outEpAddr2EpIndex has
    // size MAX_USB_EP_NUM + 1.
    //cstat !ARR-inv-index-pos !MISRAC2012-Rule-18.1_b
    epIndex = dev->outEpAddr2EpIndex[ epAddr & USB_EPNUM_MASK ];
  }

  if ( epIndex )
  {
    ep = &dev->ep[ epIndex ];
  }
  else if ( ( epAddr & USB_EPNUM_MASK ) == 0 )
  {
    ep = &dev->ep[ 0 ];
  }

  return ep;
}

static inline USBD_Ep_TypeDef *USBD_GetEpFromNum( uint8_t epnum )
{
  USBD_Ep_TypeDef *ep = NULL;
  ep = &dev->ep[ epnum ];
  return ep;
}

static inline void USBD_ReArmEp0( USBD_Ep_TypeDef *ep )
{
  USBD_ArmEp0(ep);
}

static inline void USBD_AbortEp( USBD_Ep_TypeDef *ep )
{
  if ( ep->state == D_EP_IDLE )
  {
    return;
  }

  if ( ep->in )
  {
    // Unsupported
    // USBDHAL_AbortEpIn( ep );
  }
  else
  {
    // Unsupported
    // USBDHAL_AbortEpOut( ep );
  }
}

/** @endcond */

#ifdef __cplusplus
}
#endif

#endif /* __EM_USBD_H */
