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
#include "em_cmu.h"
#include "bsp.h"
#include "bsp_trace.h"
#include "retargetserial.h"
#include <stdio.h>

#include "em_usb.h"
#include "descriptors.h"

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

/*** Typedef's and defines. ***/

#define LED0            1
#define LED1            2
#define LED2            4
#define LED3            8
#define LED4            16

#define VND_GET_LEDS    0x10
#define VND_SET_LED     0x11

/*** Function prototypes. ***/

static void ConsoleDebugInit(void);
static int  SetupCmd(const USB_Setup_TypeDef *setup);

static const USBD_Callbacks_TypeDef callbacks =
{
  .usbReset        = NULL,
  .usbStateChange  = NULL,
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

/**************************************************************************//**
 * @brief main - the entrypoint after reset.
 *****************************************************************************/
int main(void)
{
  BSP_Init(BSP_INIT_DEFAULT);   /* Initialize DK board register access     */

  /* If first word of user data page is non-zero, enable eA Profiler trace */
  BSP_TraceProfilerSetup();

  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);

  BSP_LedsSet(0);

  ConsoleDebugInit();           /* Initialize DK UART port. */

  printf("\nEFM32 USB LED Vendor Unique Device example\n");

  /* Initialize and start USB device stack. */
  USBD_Init( &usbInitStruct );

  /*
   * When using a debugger it is pratical to uncomment the following three
   * lines to force host to re-enumerate the device.
   */
  /* USBD_Disconnect(); */
  /* USBTIMER_DelayMs( 1000 ); */
  /* USBD_Connect(); */

  for (;;)
  {
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
static int SetupCmd(const USB_Setup_TypeDef *setup)
{
  int             retVal;
  uint16_t        leds;
  static uint32_t buffer;
  uint8_t         *pBuffer = (uint8_t*) &buffer;

  retVal = USB_STATUS_REQ_UNHANDLED;

  if (setup->Type == USB_SETUP_TYPE_VENDOR)
  {
    switch (setup->bRequest)
    {
    case VND_GET_LEDS:
      /********************/
      *pBuffer = BSP_LedsGet() & 0x1F;
      retVal   = USBD_Write(0, pBuffer, setup->wLength, NULL);
      break;

    case VND_SET_LED:
      /********************/
      leds = BSP_LedsGet() & 0x1F;
      if (setup->wValue)
      {
        leds |= LED0 << setup->wIndex;
      }
      else
      {
        leds &= ~(LED0 << setup->wIndex);
      }
      BSP_LedsSet(leds);
      putchar('.');
      retVal = USB_STATUS_OK;
      break;
    }
  }

  return retVal;
}

/**************************************************************************//**
 * @brief Initialize console I/O redirection.
 *****************************************************************************/
void ConsoleDebugInit(void)
{
  RETARGET_SerialInit();                        /* Initialize USART   */
  RETARGET_SerialCrLf(1);                       /* Map LF to CRLF     */
}
