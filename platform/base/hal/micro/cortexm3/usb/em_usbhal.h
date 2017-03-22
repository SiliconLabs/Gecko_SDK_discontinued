/***************************************************************************//**
 * @file
 * @brief USB protocol stack library, low level USB peripheral access.
 * @author Nathaniel Ting
 * @version 3.20.3
 * <!-- Copyright 2013 by Silicon Laboratories. All rights reserved.     *80*-->
 *****************************************************************************/
#ifndef __EM_USBHAL_H
#define __EM_USBHAL_H

#include "em_usbtypes.h"

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef VBUSMON
void vbusMonCfg(void);
#endif //VBUSMON
void halSuspendCallback(void);
void halResumeCallback(void);

#define INT_USBTXACTIVE ( INT_USBTXACTIVEEP0 | \
                          INT_USBTXACTIVEEP1 | \
                          INT_USBTXACTIVEEP2 | \
                          INT_USBTXACTIVEEP3 | \
                          INT_USBTXACTIVEEP4 | \
                          INT_USBTXACTIVEEP5 | \
                          INT_USBTXACTIVEEP6 )

#define INT_USBRXVALID ( INT_USBRXVALIDEP0 | \
                         INT_USBRXVALIDEP1 | \
                         INT_USBRXVALIDEP2 | \
                         INT_USBRXVALIDEP3 | \
                         INT_USBRXVALIDEP4 | \
                         INT_USBRXVALIDEP5 | \
                         INT_USBRXVALIDEP6 )

//Define the size of all the endpoints.
#define EP0_SIZE (8)   //CTRL endpoint
#define EP1_SIZE (8)   //BULK\INTERRUPT
#define EP2_SIZE (8)   //BULK\INTERRUPT
#define EP3_SIZE (64)  //BULK\INTERRUPT
#define EP4_SIZE (32)  //BULK\INTERRUPT
#define EP5_SIZE (64)  //BULK\INTERRUPT
#define EP6_SIZE (512) //ISOCHRONOUS endpoint
//The USB core in the chip does not distinguish between INTERRUPT and BULK.
//The BULK/INTERRUPT endpoints can be morphed between each other's type by
//simply updating the endpoint descriptor.

//All physical endpoints are packed together and accessed through
//a contiguous block of RAM.  Therefore, define the overall size of the
//buffer by adding all the endpoints together.  The multiply by 2 is
//because each endpoint is bidirection with IN and OUT.
#define USB_BUFFER_SIZE ( (EP0_SIZE*2) + \
                          (EP1_SIZE*2) + \
                          (EP2_SIZE*2) + \
                          (EP3_SIZE*2) + \
                          (EP4_SIZE*2) + \
                          (EP5_SIZE*2) + \
                          (EP6_SIZE*2) )

//Define all of the endpoints as byte arrays.
typedef struct
{
  uint8_t ep0IN[EP0_SIZE];
  uint8_t ep0OUT[EP0_SIZE];

  uint8_t ep1IN[EP1_SIZE];
  uint8_t ep1OUT[EP1_SIZE];

  uint8_t ep2IN[EP2_SIZE];
  uint8_t ep2OUT[EP2_SIZE];

  uint8_t ep3IN[EP3_SIZE];
  uint8_t ep3OUT[EP3_SIZE];

  uint8_t ep4IN[EP4_SIZE];
  uint8_t ep4OUT[EP4_SIZE];

  uint8_t ep5IN[EP5_SIZE];
  uint8_t ep5OUT[EP5_SIZE];

  uint8_t ep6IN[EP6_SIZE];
  uint8_t ep6OUT[EP6_SIZE];
} EndPointStruct;

//Overlay the endpoint structure on top of a flat byte array of the entire
//memory block that USB will be accessing.  This way an enpoint can be
//accessed specifically by name or the entire block can be accessed.
typedef union
{
  uint8_t allEps[USB_BUFFER_SIZE];
  EndPointStruct eps;
} EndPointUnion;

extern EndPointUnion usbBufferA;
extern EndPointUnion usbBufferB;

extern uint8_t *EPOUTBUF[7];
extern uint8_t *EPINBUF[7];
extern volatile uint32_t *EPOUTBUFSIZE[7];
extern volatile uint32_t *EPINBUFSIZE[7];

extern bool usbVbusPresent;

static inline void USBHAL_DisableGlobalInt( void )
{
  NVIC_DisableIRQ(USB_IRQn);
  /* Disable all interrupts. */
  INT_USBCFG = 0;

  /* Clear pending interrupts */
  INT_USBFLAG = 0xFFFFFFFF;
}

static inline void USBHAL_EnableGlobalInt( void )
{
  NVIC_EnableIRQ(USB_IRQn);
}

static inline void USBDHAL_EnableUsbResetInt( void )
{
  /* Disable all interrupts. */
  INT_USBCFG = 0;

  /* Clear pending interrupts */
  INT_USBFLAG = 0xFFFFFFFF;

  #ifndef BOOTLOADER
  INT_USBCFG = (INT_USBRESET  |
                INT_USBRXVALIDEP0|
                INT_USBTXACTIVEEP0 |
                INT_USBRESUME      |
                INT_USBWAKEUP      |
                INT_USBSUSPEND     );
  #else
  INT_USBCFG = (INT_USBRESET  |
                INT_USBRXVALIDEP0|
                INT_USBTXACTIVEEP0 );

  #endif
}

static inline void USBDHAL_StartEp0Setup( USBD_Device_TypeDef *dev )
{
  dev->ep[ 0 ].in = false;
  dev->setup = dev->setupPkt;
}

static inline void USBDHAL_ReenableEp0Setup( USBD_Device_TypeDef *dev )
{
  dev->setup = dev->setupPkt;
}

static inline void USBDHAL_DeactivateEp( USBD_Ep_TypeDef *ep )
{
  #ifdef USB_DEBUG_EP
      DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "Deactivating EP%d",ep->num);
  #endif
  if (ep->in)
  {
    #ifdef USB_DEBUG_EP
      #ifdef USB_DEBUG_VERBOSE
        DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "in-->\r\n");
      #else
        DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "in\r\n");
      #endif
    #endif
    USB_ENABLEIN &= ~(USB_ENABLEINEP0    << ep->num);
    USB_CTRL     &= ~(USB_ENBUFINEP0B    << ep->num);
    INT_USBCFG   &= ~(INT_USBTXACTIVEEP0 << ep->num);
  }
  else
  {
    #ifdef USB_DEBUG_EP
      #ifdef USB_DEBUG_VERBOSE
        DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "out-->\r\n");
      #else
        DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "out\r\n");
      #endif
    #endif
    USB_ENABLEOUT &= ~(USB_ENABLEOUTEP0  << ep->num);
    USB_CTRL      &= ~(USB_ENBUFOUTEP0B  << ep->num);
    INT_USBCFG    &= ~(INT_USBRXVALIDEP0 << ep->num);
  }

  #ifdef USB_DEBUG_EP
    #ifdef USB_DEBUG_VERBOSE
      DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "USB_ENABLEIN\tUSB_ENABLEOUT\tUSB_CTRL\tINT_USBCFG\r\n");
      DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "%X          \t%X           \t%X      \t%X\r\n",USB_ENABLEIN,USB_ENABLEOUT,USB_CTRL,INT_USBCFG);
    #endif
  #endif
}

static inline USB_Status_TypeDef USBDHAL_StallEp( USBD_Ep_TypeDef *ep )
{
  USB_Status_TypeDef retVal = USB_STATUS_REQ_ERR;

  if ( ep->in == true )
  {
    USB_STALLIN |= USB_STALLINEP0 << ep->num;
    retVal = USB_STATUS_OK;
  }
  else
  {
    USB_STALLOUT |= USB_STALLOUTEP0 << ep->num;
    retVal = USB_STATUS_OK;
  }

  return retVal;
}

static inline USB_Status_TypeDef USBDHAL_UnStallEp( USBD_Ep_TypeDef *ep )
{
  USB_Status_TypeDef retVal = USB_STATUS_REQ_ERR;

  if ( ep->in == true )
  {
    USB_STALLIN &= ~(USB_STALLINEP0 << ep->num);
    retVal = USB_STATUS_OK;
  }
  else
  {
    USB_STALLOUT &= ~(USB_STALLOUTEP0 << ep->num);
    retVal = USB_STATUS_OK;
  }

  return retVal;
}

static inline USB_Status_TypeDef USBDHAL_GetStallStatusEp(
                                   USBD_Ep_TypeDef *ep, uint16_t *halt )
{
  USB_Status_TypeDef retVal = USB_STATUS_REQ_ERR;

  if ( ep->in == true )
  {
    *halt = USB_STALLIN & (USB_STALLINEP0 << ep->num) ? 1: 0;
    retVal = USB_STATUS_OK;
  }
  else
  {
    *halt = USB_STALLOUT & (USB_STALLOUTEP0 << ep->num) ? 1: 0;
    retVal = USB_STATUS_OK;
  }

  return retVal;
}


static inline bool USBDHAL_EpIsStalled( USBD_Ep_TypeDef *ep )
{
  bool retVal = false;
  uint16_t stallStatus;

  if ( USBDHAL_GetStallStatusEp( ep, &stallStatus ) == USB_STATUS_OK )
  {
    retVal = stallStatus & 1 ? true : false;
  }
  return retVal;
}

static inline void USBDHAL_StartEpIn( USBD_Ep_TypeDef *ep )
{
  uint8_t txSize = (ep->remaining > ep->packetSize) ? ep->packetSize : ep->remaining;

  if(txSize > 0)
  {
    if (ep->buf!=NULL)
    {
      MEMMOVE(EPINBUF[ep->num], ep->buf, txSize);
    }
  }
  ep->buf += txSize;


  #if (defined (USB_DEBUG_WRITE) && defined(USB_DEBUG_VERBOSE))
    uint8_t i;
    for(i=0;i<txSize;i++)
    {
      DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "%02X ", EPINBUF[ep->num][i]);
    }
    DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "\r\n");
  #endif

  *EPINBUFSIZE[ep->num] = txSize;

  USB_TXLOAD = USB_TXLOADEP0A << ep->num;
}

static void USBDHAL_StartEpOut( USBD_Ep_TypeDef *ep )
{

  #if defined(USB_DEBUG_EP) && defined(USB_DEBUG_VERBOSE)
    uint8_t i;
    for(i=0;i<8;i++)
    {
      DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "%c", EPOUTBUF[ep->num][i]);
    }
    DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "\r\n");
  #endif

  // USB_ENABLEOUT |= EPOUT[ep->num].ENABLE;
  // INT_USBCFG |= EPOUT[ep->num].INT;
  // USB_RXVALID |= (EPOUT[ep->num].RXVALIDA | EPOUT[ep->num].RXVALIDB);
}


static inline void USBDHAL_SetRemoteWakeup( void )
{
  USB_RESUME = 1;
}

// not supported
void USBDHAL_AbortAllEps( void );

void USBDHAL_Connect( void );
void USBDHAL_Disconnect(        void );
USB_Status_TypeDef USBDHAL_CoreInit( uint32_t totalRxFifoSize,
                                     uint32_t totalTxFifoSize );
void USBDHAL_AbortAllTransfers( USB_Status_TypeDef reason );

#ifdef __cplusplus
}
#endif

/** @endcond */
#endif /* __EM_USBHAL_H */
