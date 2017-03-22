/**************************************************************************//**
 * @file main.c
 * @brief USB composite device example.
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
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpio.h"

#include "em_usb.h"
#include "descriptors.h"
#include "display.h"
#include "scrolllcd.h"
#include "image.h"
#include "cdc.h"
#include "vud.h"
#include "kbd.h"

/**************************************************************************//**
 *
 * This example shows how a composite USB device can be implemented.
 *
 *****************************************************************************/

/*** Function prototypes. ***/

static int SetupCmd( const USB_Setup_TypeDef *setup );
static void StateChange( USBD_State_TypeDef oldState,
                         USBD_State_TypeDef newState );

/*** Variables ***/

static const USBD_Callbacks_TypeDef callbacks =
{
  .usbReset        = NULL,
  .usbStateChange  = StateChange,
  .setupCmd        = SetupCmd,
  .isSelfPowered   = NULL,
  .sofInt          = NULL
};

static const USBD_Init_TypeDef usbInitStruct =
{
  .deviceDescriptor    = &USBDESC_deviceDesc,
  .configDescriptor    = USBDESC_configDesc,
  .stringDescriptors   = USBDESC_strings,
  .numberOfStrings     = sizeof(USBDESC_strings)/sizeof(void*),
  .callbacks           = &callbacks,
  .bufferingMultiplier = USBDESC_bufferingMultiplier,
  .reserved            = 0
};

static DISPLAY_Device_t displayDevice;            /* Display device handle. */
static volatile scrollDirection_t scrollDisplay;
static char blank_image[ 128 * 16 ];

/**************************************************************************//**
 * @brief main - the entrypoint after reset.
 *****************************************************************************/
int main(void)
{
  /* Chip errata */
  CHIP_Init();

  CMU_ClockSelectSet( cmuClock_HF, cmuSelect_HFXO );

  /* Initialize the display module. */
  DISPLAY_Init();

  /* Retrieve the properties of the display. */
  if ( DISPLAY_DeviceGet( 0, &displayDevice ) != DISPLAY_EMSTATUS_OK )
  {
    /* Unable to get display handle. */
    while( 1 );
  }

  memset( (void*)blank_image, 0xFF, 128*16 );
  displayDevice.pPixelMatrixDraw( &displayDevice, (void*)blank_image,
                                  /* start coloumn, width */
                                  0, displayDevice.geometry.width,
                                  /* start row, height */
                                  0, displayDevice.geometry.height);
  scrollDisplay = scrollOff;

  VUD_Init();                   /* Initialize the vendor unique device. */
  CDC_Init();                   /* Initialize the communication class device. */
  KBD_Init();                   /* Initialize the HID class keyboard device. */

  /* Initialize and start USB device stack. */
  USBD_Init( &usbInitStruct );

  /*
   * When using a debugger it is practical to uncomment the following three
   * lines to force host to re-enumerate the device.
   */
  /* USBD_Disconnect();      */
  /* USBTIMER_DelayMs(1000); */
  /* USBD_Connect();         */

  scrollLcd( &displayDevice, scrollLeft, blank_image, usb_image );

  for (;;)
  {
    if ( scrollDisplay != scrollOff )
    {
      scrollLcd( &displayDevice, scrollDisplay, usb_image, usb_image );
      scrollDisplay = scrollOff;
    }
  }
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
static int SetupCmd(const USB_Setup_TypeDef *setup)
{
  int retVal;

  /* Call SetupCmd handlers for all functions within the composite device. */

  retVal =  KBD_SetupCmd( setup );

  if ( retVal == USB_STATUS_REQ_UNHANDLED )
  {
    retVal = VUD_SetupCmd( setup );
  }

  if ( retVal == USB_STATUS_REQ_UNHANDLED )
  {
    retVal = CDC_SetupCmd( setup );
  }

  return retVal;
}

/**************************************************************************//**
 * @brief
 *   Callback function called each time the USB device state is changed.
 *   Starts keyboard scanning when device has been configured by USB host.
 *
 * @param[in] oldState The device state the device has just left.
 * @param[in] newState The new device state.
 *****************************************************************************/
static void StateChange(USBD_State_TypeDef oldState,
                        USBD_State_TypeDef newState)
{
  /* Call device StateChange event handlers for all relevant functions within
     the composite device. */
  CDC_StateChangeEvent( oldState, newState );
  KBD_StateChangeEvent( oldState, newState );

  if ( newState == USBD_STATE_CONFIGURED )
  {
    if ( scrollDisplay == scrollOff )
    {
      scrollDisplay = scrollUp;
    }
  }

  if ( ( oldState == USBD_STATE_CONFIGURED ) &&
       ( newState == USBD_STATE_SUSPENDED  )    )
  {
    if ( scrollDisplay == scrollOff )
    {
      scrollDisplay = scrollDown;
    }
  }
}
