/**************************************************************************//**
 * @file main.c
 * @brief USB CDC Serial Port adapter example project.
 * @version 5.1.2
 ******************************************************************************
 * @section License
 * <b>Copyright 2016 Silicon Laboratories, Inc. http://www.silabs.com</b>
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

#include "em_usb.h"
#include "cdc.h"
#include "descriptors.h"
#include "display.h"
#include "scrolllcd.h"
#include "image.h"

/**************************************************************************//**
 *
 * This example shows how a CDC based USB to Serial port adapter can be
 * implemented.
 *
 * Use the file EFM32-Cdc.inf to install a USB serial port device driver
 * on the host PC.
 *
 * This implementation uses DMA to transfer data between USART1 and memory
 * buffers.
 *
 *****************************************************************************/

static void StateChange( USBD_State_TypeDef oldState,
                         USBD_State_TypeDef newState );

static const USBD_Callbacks_TypeDef callbacks =
{
  .usbReset        = NULL,
  .usbStateChange  = StateChange,
  .setupCmd        = CDC_SetupCmd,
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
static char blankImage[ 128 * 16 ];

/**************************************************************************//**
 * @brief main - the entrypoint after reset.
 *****************************************************************************/
int main(void)
{
  char const *currentImage, *nextImage;

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

  memset( (void*)blankImage, 0xFF, 128*16 );
  displayDevice.pPixelMatrixDraw( &displayDevice, (void*)blankImage,
                                  /* start coloumn, width */
                                  0, displayDevice.geometry.width,
                                  /* start row, height */
                                  0, displayDevice.geometry.height);
  scrollDisplay = scrollOff;

  /* Initialize the communication class device. */
  CDC_Init();

  /* Initialize and start USB device stack. */
  USBD_Init(&usbInitStruct);

  /*
   * When using a debugger it is practical to uncomment the following three
   * lines to force host to re-enumerate the device.
   */
  /*USBD_Disconnect();         */
  /*USBTIMER_DelayMs(1000);    */
  /*USBD_Connect();            */

  scrollLcd( &displayDevice, scrollLeft, blankImage, usbDisconnectedImage );
  currentImage = usbDisconnectedImage;

  for (;;)
  {
    if ( scrollDisplay != scrollOff )
    {
      if (USBD_GetUsbState() == USBD_STATE_CONFIGURED)
      {
        nextImage = usbConnectedImage;
      }
      else
      {
        nextImage = usbDisconnectedImage;
      }

      scrollLcd( &displayDevice, scrollDisplay, currentImage, nextImage );
      scrollDisplay = scrollOff;
      currentImage = nextImage;
    }
  }
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
  /* Call CDC driver state change event handler. */
  CDC_StateChangeEvent( oldState, newState );

  if ( newState == USBD_STATE_CONFIGURED )
  {
    if ( scrollDisplay == scrollOff )
    {
      scrollDisplay = scrollUp;
    }
  }

  else if ( ( oldState == USBD_STATE_CONFIGURED ) &&
            ( newState == USBD_STATE_SUSPENDED  )    )
  {
    if ( scrollDisplay == scrollOff )
    {
      scrollDisplay = scrollDown;
    }
  }
}
