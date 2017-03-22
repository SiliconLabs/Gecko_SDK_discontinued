/**************************************************************************//**
 * @file usb_debug.c
 * @brief USB CDC device setup for HRM debugging.
 * @version 5.1.2
 ******************************************************************************
 * @section License
 * <b>Copyright 2015 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include <stdio.h>
#include "em_device.h"
#include "em_usb.h"
#include "em_assert.h"
#include "em_cmu.h"
#include "em_chip.h"
#include "usbconfig.h"
#include "em_emu.h"
#include "si114xhrm.h"

extern const USBD_Init_TypeDef initstruct;
/* Flag used to determine if USB is connected */
static int usbConfigured = 0;

static uint8_t usb_data_buffer[((99)+3)&~3] SL_ATTRIBUTE_ALIGN(4);
static int8_t master = 1;

/**************************************************************************//**
 *
 * Use the file EFM32-Cdc.inf to install a USB serial port device driver
 * on the host PC.
 *
 *****************************************************************************/

/* The serial port LINE CODING data structure, used to carry information  */
/* about serial port baudrate, parity etc. between host and device.       */
SL_PACK_START(1)
typedef struct
{
  uint32_t dwDTERate;               /** Baudrate                            */
  uint8_t  bCharFormat;             /** Stop bits, 0=1 1=1.5 2=2            */
  uint8_t  bParityType;             /** 0=None 1=Odd 2=Even 3=Mark 4=Space  */
  uint8_t  bDataBits;               /** 5, 6, 7, 8 or 16                    */
  uint8_t  dummy;                   /** To ensure size is a multiple of 4 bytes.*/
} SL_ATTRIBUTE_PACKED cdcLineCoding_TypeDef;
SL_PACK_END()


/*** Variables ***/

/*
 * The LineCoding variable must be 4-byte aligned as it is used as USB
 * transmit and receive buffer
 */
SL_ALIGN(4)
SL_PACK_START(1)
static cdcLineCoding_TypeDef SL_ATTRIBUTE_ALIGN(4) cdcLineCoding =
{
  115200, 0, 0, 8, 0
};
SL_PACK_END()

/**************************************************************************//**
 * @brief usbcomposite_Init - Initialize the USB Device
 *****************************************************************************/
void USBDebug_Init( void )
{
  Si114xhrmHandle_t _handle = NULL;
  int debug = 0;

  /* Setup USB debug function */
  si114xhrm_SetupDebug(_handle, (int *)&debug);

  /* Start USB */
  USBD_Init(&initstruct);
}

/**************************************************************************//**
 * @brief
 *   Called whenever a USB setup command is received.
 *
 * @param[in] setup
 *   Pointer to an USB setup packet.
 *
 * @return
 *   An appropriate status/error code. See USB_Status_TypeDef.
 *****************************************************************************/
int USBDebug_SetupCmd(const USB_Setup_TypeDef *setup)
{
	int retVal = USB_STATUS_REQ_UNHANDLED;

	  if ( ( setup->Type      == USB_SETUP_TYPE_CLASS          ) &&
	       ( setup->Recipient == USB_SETUP_RECIPIENT_INTERFACE )    )
	  {
	    switch (setup->bRequest)
	    {
	    case USB_CDC_GETLINECODING:
	      /********************/
	      if ( ( setup->wValue    == 0                     ) &&
	           ( setup->wIndex    == CDC_CTRL_INTERFACE_NO ) && /* Interface no. */
	           ( setup->wLength   == 7                     ) && /* Length of cdcLineCoding */
	           ( setup->Direction == USB_SETUP_DIR_IN      )    )
	      {
	        /* Send current settings to USB host. */
	        USBD_Write(0, (void*) &cdcLineCoding, 7, NULL);
	        retVal = USB_STATUS_OK;
	      }
	      break;

	    case USB_CDC_SETLINECODING:
	      /********************/
	      if ( ( setup->wValue    == 0                     ) &&
	           ( setup->wIndex    == CDC_CTRL_INTERFACE_NO ) && /* Interface no. */
	           ( setup->wLength   == 7                     ) && /* Length of cdcLineCoding */
	           ( setup->Direction != USB_SETUP_DIR_IN      )    )
	      {
	        /* Get new settings from USB host. */
	    	  USBD_Read(0, (void*) &cdcLineCoding, 7, NULL);
	        retVal = USB_STATUS_OK;
	      }
	      break;

	    case USB_CDC_SETCTRLLINESTATE:
	      /********************/
	      if ( ( setup->wIndex  == CDC_CTRL_INTERFACE_NO ) &&   /* Interface no.  */
	           ( setup->wLength == 0                     )    ) /* No data        */
	      {
	        /* Do nothing ( Non compliant behaviour !! ) */
	        retVal = USB_STATUS_OK;
	      }
	      break;
	    }
	  }

  return retVal;
}

/**************************************************************************//**
 * @brief
 *   Called whenever the USB device has changed its device state.
 *
 * @param[in] oldState
 *   The device USB state just leaved. See USBD_State_TypeDef.
 *
 * @param[in] newState
 *   New (the current) USB device state. See USBD_State_TypeDef.
 *****************************************************************************/
void USBDebug_StateChangeEvent( USBD_State_TypeDef oldState,
                                USBD_State_TypeDef newState )
{
  (void) oldState;
  /* Call device StateChange event handler. */
  Si114xhrmHandle_t _handle = NULL;
  int debug;
  /* Determine if USB is connected. Based on this, determine whether or not to */
  /* enable the data dump feature and/or to enter low power mode.              */
  if(newState == USBD_STATE_CONFIGURED)
  {
	  usbConfigured = 1;
    debug = 1;
  	si114xhrm_SetupDebug(_handle, (int *)&debug);
  }
  else
  {
	  usbConfigured = 0;
    debug = 0;
    si114xhrm_SetupDebug(_handle, (int *)&debug);
  }
}

/**************************************************************************//**
 * @brief Check if USB is connected
 *****************************************************************************/
bool USBDebug_IsUSBConfigured()
{
	if (usbConfigured)
  {
		return true;
  }
	return false;
}

#ifdef USB_DEBUG

/**************************************************************************//**
 * @brief byteToHexChar - Converts hex byte to ASCII character
 *****************************************************************************/
static uint8_t byteToHexChar( uint8_t hex )
{
   if( hex < 0x0A )
   {
      return (hex + 0x30);
   }
   else
   {
      return (hex + 0x37);
   }
}

/**************************************************************************//**
 * @brief Output data buffer to USB port
 *****************************************************************************/
void USBDebug_ProcessUSBOutput(int8_t *data_buffer)
{
    uint8_t counter;
    int16_t data_irqstatus;
    uint8_t *Usb_Buffer[1] = {usb_data_buffer};

    usb_data_buffer[0] = byteToHexChar(((uint8_t)(master<<1) + 1) >> 4);
    usb_data_buffer[1] = byteToHexChar(((uint8_t)(master<<1) + 1) & 0x0F);

    usb_data_buffer[4] = byteToHexChar((uint8_t)(data_buffer[2] >> 4) & 0x0F);
    usb_data_buffer[5] = byteToHexChar((uint8_t)data_buffer[2] & 0x0F);
    usb_data_buffer[2] = byteToHexChar((uint8_t)(data_buffer[3] >> 4) & 0x0F);
    usb_data_buffer[3] = byteToHexChar((uint8_t)data_buffer[3] & 0x0F);

    data_irqstatus = data_buffer[5];

    usb_data_buffer[6] = byteToHexChar((uint8_t)data_irqstatus >> 4);
    usb_data_buffer[7] = byteToHexChar((uint8_t)data_irqstatus & 0x0F);
    for( counter = 1; counter < 13; counter+=2 )
    {
       usb_data_buffer[8+counter*2] = byteToHexChar((uint8_t)data_buffer[counter+5] >> 4);
       usb_data_buffer[9+counter*2] = byteToHexChar((uint8_t)data_buffer[counter+5] & 0x0F);
       usb_data_buffer[6+counter*2] = byteToHexChar((uint8_t)data_buffer[counter+6] >> 4);
       usb_data_buffer[7+counter*2] = byteToHexChar((uint8_t)data_buffer[counter+6] & 0x0F);
    }

    usb_data_buffer[32] = 0x0D;
    USBD_Write(CDC_EP_DATA_IN, (void*) Usb_Buffer[0], 33, NULL);
}

/**************************************************************************//**
 * @brief Output Si114x HRM configuration data to USB port
 *****************************************************************************/
void USBDebug_ProcessConfigurationMessage(int8_t *message, Si114xhrmConfiguration_t *configuration)
{
	message[0] = byteToHexChar((uint8_t)(configuration->hrmSpo2PsSelect >> 12) & 0x0f);
	message[1] = byteToHexChar((uint8_t)(configuration->hrmSpo2PsSelect >> 8) & 0x0f);
	message[2] = byteToHexChar((uint8_t)(configuration->hrmSpo2PsSelect >> 4) & 0x0f);
	message[3] = byteToHexChar((uint8_t)(configuration->hrmSpo2PsSelect) & 0x0f);
	message[4] = byteToHexChar((uint8_t)(configuration->ledCurrent >> 12) & 0x0f);
	message[5] = byteToHexChar((uint8_t)(configuration->ledCurrent >> 8) & 0x0f);
	message[6] = byteToHexChar((uint8_t)(configuration->ledCurrent >> 4) & 0x0f);
	message[7] = byteToHexChar((uint8_t)(configuration->ledCurrent) & 0x0f);
	message[8] = byteToHexChar((uint8_t)(configuration->tasklist >> 12) & 0x0f);
	message[9] = byteToHexChar((uint8_t)(configuration->tasklist >> 8) & 0x0f);
	message[10] = byteToHexChar((uint8_t)(configuration->tasklist >> 4) & 0x0f);
	message[11] = byteToHexChar((uint8_t)(configuration->tasklist) & 0x0f);
	message[12] = byteToHexChar((uint8_t)(configuration->psLedSelect >> 12) & 0x0f);
	message[13] = byteToHexChar((uint8_t)(configuration->psLedSelect >> 8) & 0x0f);
	message[14] = byteToHexChar((uint8_t)(configuration->psLedSelect >> 4) & 0x0f);
	message[15] = byteToHexChar((uint8_t)(configuration->psLedSelect) & 0x0f);
	message[16] = byteToHexChar((uint8_t)(configuration->measurementRate >> 12) & 0x0f);
	message[17] = byteToHexChar((uint8_t)(configuration->measurementRate >> 8) & 0x0f);
	message[18] = byteToHexChar((uint8_t)(configuration->measurementRate >> 4) & 0x0f);
	message[19] = byteToHexChar((uint8_t)(configuration->measurementRate) & 0x0f);
	message[20] = byteToHexChar((uint8_t)(configuration->adcGain >> 12) & 0x0f);
	message[21] = byteToHexChar((uint8_t)(configuration->adcGain >> 8) & 0x0f);
	message[22] = byteToHexChar((uint8_t)(configuration->adcGain >> 4) & 0x0f);
	message[23] = byteToHexChar((uint8_t)(configuration->adcGain) & 0x0f);
	message[24] = byteToHexChar((uint8_t)(configuration->adcMisc >> 12) & 0x0f);
	message[25] = byteToHexChar((uint8_t)(configuration->adcMisc >> 8) & 0x0f);
	message[26] = byteToHexChar((uint8_t)(configuration->adcMisc >> 4) & 0x0f);
	message[27] = byteToHexChar((uint8_t)(configuration->adcMisc) & 0x0f);
	message[28] = byteToHexChar((uint8_t)(configuration->adcMux >> 12) & 0x0f);
	message[29] = byteToHexChar((uint8_t)(configuration->adcMux >> 8) & 0x0f);
	message[30] = byteToHexChar((uint8_t)(configuration->adcMux >> 4) & 0x0f);
	message[31] = byteToHexChar((uint8_t)(configuration->adcMux) & 0x0f);
	message[32] = byteToHexChar((uint8_t)(configuration->psAlign >> 12) & 0x0f);
	message[33] = byteToHexChar((uint8_t)(configuration->psAlign >> 8) & 0x0f);
	message[34] = byteToHexChar((uint8_t)(configuration->psAlign >> 4) & 0x0f);
	message[35] = byteToHexChar((uint8_t)(configuration->psAlign) & 0x0f);
	message[36] = 0x0;
}

#endif
