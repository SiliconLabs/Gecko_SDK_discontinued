/**************************************************************************//**
 * @file main.c
 * @brief Vendor unique USB device example.
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
#include "bsp.h"
#include "em_usb.h"
#include "descriptors.h"
#include "display.h"
#include "scrolllcd.h"
#include "image.h"

/**************************************************************************//**
 *
 * This example shows how a vendor unique device can be implemented.
 * A vendor unique device is a device which does not belong to any
 * USB class.
 *
 * Use the file EFM32_Vendor_Unique_Device.inf to install libusb device driver
 * on the host PC. This file reside in example subdirectory:
 * ./host/libusb/efm32-vendor-unique-device-1.2.5.0
 *
 *****************************************************************************/

#define LED0            0
#define LED1            1

#define VND_GET_LEDS    0x10
#define VND_SET_LED     0x11

static int SetupCmd( const USB_Setup_TypeDef *setup );
static void StateChange( USBD_State_TypeDef oldState,
                         USBD_State_TypeDef newState );

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
  .numberOfStrings     = sizeof( USBDESC_strings )/sizeof( void* ),
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
int main( void )
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

  /* Initialize LED driver */
  BSP_LedsInit();
  BSP_LedClear( LED0 );
  BSP_LedClear( LED1 );

  /* Initialize and start USB device stack. */
  USBD_Init( &usbInitStruct );

  /*
   * When using a debugger the tree following lines are pratical to
   * force host to re-enumerate the device.
   */
  /*USBD_Disconnect();        */
  /*USBTIMER_DelayMs( 1000 ); */
  /*USBD_Connect();           */

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
 *   Handle USB setup commands.
 *
 * @param[in] setup Pointer to the setup packet received.
 *
 * @return USB_STATUS_OK if command accepted.
 *         USB_STATUS_REQ_UNHANDLED when command is unknown, the USB device
 *         stack will handle the request.
 *****************************************************************************/
static int SetupCmd( const USB_Setup_TypeDef *setup )
{
  int             retVal;
  static uint32_t buffer;
  uint8_t         *pBuffer = (uint8_t*) &buffer;

  retVal = USB_STATUS_REQ_UNHANDLED;

  if ( setup->Type == USB_SETUP_TYPE_VENDOR )
  {
    switch ( setup->bRequest )
    {
    case VND_GET_LEDS:
      /********************/
      *pBuffer = (uint8_t)BSP_LedsGet();
      retVal   = USBD_Write( 0, pBuffer, setup->wLength, NULL );
      break;

    case VND_SET_LED:
      /********************/
      if ( setup->wValue )
      {
        if ( setup->wIndex == LED0 )
          BSP_LedSet( LED0 );
        else if ( setup->wIndex == LED1 )
          BSP_LedSet( LED1 );
      }
      else
      {
        if ( setup->wIndex == LED0 )
          BSP_LedClear( LED0 );
        else if ( setup->wIndex == LED1 )
          BSP_LedClear( LED1 );
      }
      retVal = USB_STATUS_OK;
      break;
    }
  }

  return retVal;
}

/**************************************************************************//**
 * @brief
 *   Callback function called each time the USB device state is changed.
 *
 * @param[in] oldState The device state the device has just left.
 * @param[in] newState The new device state.
 *****************************************************************************/
static void StateChange( USBD_State_TypeDef oldState,
                         USBD_State_TypeDef newState)
{
  if ( scrollDisplay == scrollOff )
  {
    if ( newState == USBD_STATE_CONFIGURED )
    {
      scrollDisplay = scrollUp;
    }

    else if ( oldState == USBD_STATE_CONFIGURED )
    {
      scrollDisplay = scrollDown;
    }
  }
}
