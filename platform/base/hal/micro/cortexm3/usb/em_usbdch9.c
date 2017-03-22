/**************************************************************************//**
 * @file
 * @brief USB protocol stack library, USB chapter 9 command handler.
 * @author Nathaniel Ting
 * @version 3.20.3
 * <!-- Copyright 2013 by Silicon Laboratories. All rights reserved.     *80*-->
 *****************************************************************************/


#include PLATFORM_HEADER
#include "stack/include/ember.h"
#include "hal/hal.h"

#if defined(CORTEXM3_EM35X_USB)
#include "em_usb.h"
#include "em_usbhal.h"
#include "em_usbtypes.h"
#include "em_usbd.h"

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

static USB_Status_TypeDef ClearFeature     ( USBD_Device_TypeDef *pDev );
static USB_Status_TypeDef GetConfiguration ( USBD_Device_TypeDef *pDev );
static USB_Status_TypeDef GetDescriptor    ( USBD_Device_TypeDef *pDev );
static USB_Status_TypeDef GetInterface     ( USBD_Device_TypeDef *pDev );
static USB_Status_TypeDef GetStatus        ( USBD_Device_TypeDef *pDev );
static USB_Status_TypeDef SetAddress       ( USBD_Device_TypeDef *pDev );
static USB_Status_TypeDef SetConfiguration ( USBD_Device_TypeDef *pDev );
static USB_Status_TypeDef SetFeature       ( USBD_Device_TypeDef *pDev );
static USB_Status_TypeDef SetInterface     ( USBD_Device_TypeDef *pDev );


int USBDCH9_SetupCmd( USBD_Device_TypeDef *device )
{
  int status;
  device->setup = (USB_Setup_TypeDef *)usbBufferA.eps.ep0OUT;
  USB_Setup_TypeDef *p = device->setup;

  #ifdef USB_DEBUG_CH9
  uint8_t i;
  for(i=0;i<8;i++) {
    DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "%02X", usbBufferA.eps.ep0OUT[i]);
  }
  DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "\t");
  #endif

  // DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "bmRequestType = %X\r\n" , p->bmRequestType);
  // DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "bRequest =      %X\r\n" , p->bRequest);
  // DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "wValue =        %X\r\n" , p->wValue);
  // DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "wIndex =        %X\r\n" , p->wIndex);
  // DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "wLength =       %X\r\n" , p->wLength);

  /* Vendor unique, Class or Standard setup commands override ? */
  if ( device->callbacks->setupCmd )
  {
    status = device->callbacks->setupCmd( p );

    if ( status != USB_STATUS_REQ_UNHANDLED )
    {
      return status;
    }
  }

  status = USB_STATUS_REQ_ERR;

  if ( p->Type == USB_SETUP_TYPE_STANDARD )
  {
    switch ( p->bRequest )
    {
      case GET_DESCRIPTOR:
        #ifdef USB_DEBUG_CH9
        DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "GET_DESCRIPTOR -> \r\n");
        #endif
        status = GetDescriptor( device );
        break;

      /*Hardware Handled Standard Commands. Can be disabled.*/
      case GET_STATUS:
        #ifdef USB_DEBUG_CH9
        DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "GET_STATUS -> \r\n");
        #endif
        status = GetStatus( device );
        break;

      case CLEAR_FEATURE:
        #ifdef USB_DEBUG_CH9
        DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "CLEAR_FEATURE -> \r\n");
        #endif
        status = ClearFeature( device );
        break;

      case SET_FEATURE:
        #ifdef USB_DEBUG_CH9
        DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "SET_FEATURE -> \r\n");
        #endif
        status = SetFeature( device );
        break;

      case SET_ADDRESS:
        #ifdef USB_DEBUG_CH9
        DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "SET_ADDRESS -> \r\n");
        #endif
        status = SetAddress( device );
        break;


      case GET_CONFIGURATION:
        #ifdef USB_DEBUG_CH9
        DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "GET_CONFIGURATION -> \r\n");
        #endif
        status = GetConfiguration( device );
        break;

      case SET_CONFIGURATION:
        #ifdef USB_DEBUG_CH9
        DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "SET_CONFIGURATION -> \r\n");
        #endif
        status = SetConfiguration( device );
        break;

      case GET_INTERFACE:
        #ifdef USB_DEBUG_CH9
        DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "GET_INTERFACE -> \r\n");
        #endif
        status = GetInterface( device );
        break;

      case SET_INTERFACE:
        #ifdef USB_DEBUG_CH9
        DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "SET_INTERFACE -> \r\n");
        #endif
        status = SetInterface( device );
        break;

      case SYNCH_FRAME:     /* Synch frame is for isochronous endpoints */
      case SET_DESCRIPTOR:  /* Set descriptor is optional */
      default:
        break;
    }
  }

  return status;
}

static USB_Status_TypeDef GetDescriptor( USBD_Device_TypeDef *pDev )
{
  int index;
  uint16_t length = 0;
  const void *data = NULL;
  USB_Setup_TypeDef *p = pDev->setup;
  USB_Status_TypeDef retVal = USB_STATUS_REQ_ERR;

  if ( ( p->Recipient != USB_SETUP_RECIPIENT_DEVICE ) ||
       ( p->Direction != USB_SETUP_DIR_IN           )    )
  {
    return USB_STATUS_REQ_ERR;
  }

  // identify descriptor request and return the appropriate descriptor
  index = p->wValue & 0xFF;
  switch ( p->wValue >> 8 )
  {
    case USB_DEVICE_DESCRIPTOR :
      #ifdef USB_DEBUG3
      DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "DeviceDescriptor \r\n");
      #endif
      if ( index != 0 )
      {
        break;
      }

      data   = pDev->deviceDescriptor;
      length = pDev->deviceDescriptor->bLength;
      break;

    case USB_CONFIG_DESCRIPTOR :
      #ifdef USB_DEBUG3
      DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "ConfigDescriptor \r\n");
      #endif
      if ( index != 0 )
      {
        break;
      }
      data   = pDev->configDescriptor;
      length = pDev->configDescriptor->wTotalLength;
      USBD_SetUsbState(USBD_STATE_CONFIGURED);
      break;

    case USB_STRING_DESCRIPTOR :
      #ifdef USB_DEBUG3
      DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "StringDescriptor:%d \r\n",index);
      #endif
      if ( index < pDev->numberOfStrings )
      {
        USB_StringDescriptor_TypeDef *s;
        s = ((USB_StringDescriptor_TypeDef**)pDev->stringDescriptors)[index];

        data   = s;
        length = s->len;
      }
      break;
  }

  // call USBD_Write to send data back over EPO, which will also update EP state
  if ( length )
  {
    USBD_Write( 0, (void*)data, EFM32_MIN( length, p->wLength ), NULL );
    retVal = USB_STATUS_OK;
  }

  return retVal;
}


/* Hardware Handled Standard Commands */
static USB_Status_TypeDef ClearFeature( USBD_Device_TypeDef *pDev )
{
  USB_Status_TypeDef retVal = USB_STATUS_REQ_ERR;
  return retVal;
}

static USB_Status_TypeDef GetConfiguration( USBD_Device_TypeDef *pDev )
{
  USB_Status_TypeDef retVal = USB_STATUS_REQ_ERR;
  return retVal;
}

static USB_Status_TypeDef GetInterface( USBD_Device_TypeDef *pDev )
{
  USB_Status_TypeDef retVal = USB_STATUS_REQ_ERR;
  return retVal;
}

static USB_Status_TypeDef GetStatus( USBD_Device_TypeDef *pDev )
{
  USB_Status_TypeDef retVal = USB_STATUS_REQ_ERR;
  return retVal;
}

static USB_Status_TypeDef SetAddress( USBD_Device_TypeDef *pDev )
{
  USB_Status_TypeDef retVal = USB_STATUS_REQ_ERR;
  return retVal;
}

static USB_Status_TypeDef SetConfiguration( USBD_Device_TypeDef *pDev )
{
  USB_Status_TypeDef retVal = USB_STATUS_REQ_ERR;
  return retVal;
}

static USB_Status_TypeDef SetFeature( USBD_Device_TypeDef *pDev )
{
  USB_Status_TypeDef retVal = USB_STATUS_REQ_ERR;
  return retVal;
}

static USB_Status_TypeDef SetInterface( USBD_Device_TypeDef *pDev )
{
  USB_Status_TypeDef retVal = USB_STATUS_REQ_ERR;
  return retVal;
}

/** @endcond */

#endif //CORTEXM3_EM35X_USB

