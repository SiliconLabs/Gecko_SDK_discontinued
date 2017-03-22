
#include PLATFORM_HEADER
#include "stack/include/ember.h"
#include "hal/hal.h"
#include "hal/micro/cortexm3/usb/em_usb.h"
#if defined(CORTEXM3_EM35X_USB) && EM_SERIAL3_ENABLED
#include "descriptors.h"

// The serial port LINE CODING data structure, used to carry information
// about serial port baudrate, parity etc. between host and device.
// In this module we dont act on LineCoding commands from hosts, but we
// keep a copy of data received and echo that back if host asks.
EFM32_PACK_START(1)
typedef struct
{
  uint32_t dwDTERate;               /** Baudrate                            */
  uint8_t  bCharFormat;             /** Stop bits, 0=1 1=1.5 2=2            */
  uint8_t  bParityType;             /** 0=None 1=Odd 2=Even 3=Mark 4=Space  */
  uint8_t  bDataBits;               /** 5, 6, 7, 8 or 16                    */
  uint8_t  dummy;                   /** To ensure size is a multiple of 4 bytes */
} __attribute__ ((packed)) cdcLineCoding_TypeDef;
EFM32_PACK_END()

// CDC LineCoding variable.
EFM32_ALIGN(4)
EFM32_PACK_START(1)
volatile static cdcLineCoding_TypeDef __attribute__ ((aligned(4))) cdcLineCoding =
{
  115200, 0, 0, 8, 0
};
EFM32_PACK_END()

/**************************************************************************//**
 * @brief
 *   Handle USB setup commands. Implements CDC class specific commands.
 *
 * @param[in] setup Pointer to the setup packet received.
 *
 * @return USB_STATUS_OK if command accepted.
 *         USB_STATUS_REQ_UNHANDLED when command is unknown, the USB device
 *         stack will handle the request.
 *****************************************************************************/
int USBCDC_SetupCmd(const USB_Setup_TypeDef *setup)
{
  int retVal = USB_STATUS_REQ_UNHANDLED;

  if ((setup->Type == USB_SETUP_TYPE_CLASS) &&
      (setup->Recipient == USB_SETUP_RECIPIENT_INTERFACE))
  {
    switch (setup->bRequest)
    {
    case USB_CDC_GETLINECODING:

      #ifdef USB_DEBUG_CDC
      DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "CDC_GETLINECODING -> \r\n");
      #endif
      /********************/
      if ((setup->wValue == 0) &&
          (setup->wIndex == 0) &&               /* Interface no.            */
          (setup->wLength == 7) &&              /* Length of cdcLineCoding  */
          (setup->Direction == USB_SETUP_DIR_IN))
      {
        /* Send current settings to USB host. */
        retVal = USBD_Write(0, (void*) &cdcLineCoding, 7, NULL);
      }
      break;

    case USB_CDC_SETLINECODING:

      #ifdef USB_DEBUG_CDC
      DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "CDC_SETLINECODING -> \r\n");
      #endif
      /********************/
      if ((setup->wValue == 0) &&
          (setup->wIndex == 0) &&               /* Interface no.            */
          (setup->wLength == 7) &&              /* Length of cdcLineCoding  */
          (setup->Direction != USB_SETUP_DIR_IN))
      {
        /* Get new settings from USB host. */
        retVal = USBD_Read(0, (void*) &cdcLineCoding, 7, NULL);
      }
      break;

    case USB_CDC_SETCTRLLINESTATE:
      #ifdef USB_DEBUG_CDC
      DEBUG_BUFFER += sprintf(DEBUG_BUFFER, "CDC_SETLINESTATE -> \r\n");
      #endif
      /********************/
      if ((setup->wIndex == 0) &&               /* Interface no.  */
          (setup->wLength == 0))                /* No data        */
      {
        /* Ignore command ( Non compliant behaviour !! ) */
        retVal = USB_STATUS_OK;
      }
      break;
    }
  }

  return retVal;
}

void USBCDC_stateChange(USBD_State_TypeDef oldState, USBD_State_TypeDef newState)
{
  /* Print state transition to debug output */
  // emberSerialPrintf(SER232,"%s => %s\r\n", USBD_GetUsbStateName(oldState), USBD_GetUsbStateName(newState));

  if (newState == USBD_STATE_CONFIGURED)
  {
    /* Start waiting for the 'tick' messages */
    // emberSerialPrintf(SER232,"starting read\r\n");
    uint8_t t='\r';

    USBD_Read(CDC_EP_OUT, &t, 1, NULL);
  }
}

#endif //CORTEXM3_EM35X_USB && EM_SERIAL3_ENABLED

