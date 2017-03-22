/**************************************************************************//**
 * @file main.c
 * @brief USB host stack device enumeration example project.
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
#include "em_usb.h"

/**************************************************************************//**
 *
 * This example shows how the USB host stack can be used to "probe" the device
 * properties of any device which is attached to the host port.
 *
 * The device attached will not be configured.
 *
 *****************************************************************************/

/*** Function prototypes. ***/

static void ConsoleDebugInit(void);
static void GetDeviceStrings(void);


/*** Variables ***/

STATIC_UBUF( tmpBuf, 1024 );
static USBH_Device_TypeDef device;

/**************************************************************************//**
 * @brief main - the entrypoint after reset.
 *****************************************************************************/
int main(void)
{
  int connectionResult;
  USBH_Init_TypeDef is = USBH_INIT_DEFAULT;

  BSP_Init(BSP_INIT_DEFAULT);   /* Initialize DK board register access */

  /* If first word of user data page is non-zero, enable eA Profiler trace */
  BSP_TraceProfilerSetup();

  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);

  ConsoleDebugInit();           /* Initialize DK UART port. */

  printf("\n\nEFM32 USB Host device enumeration example.\n");

  USBH_Init(&is);               /* Initialize USB HOST stack */

  for (;;)
  {
    /* Wait for device connection */
    printf("\nWaiting for USB device plug-in...\n");

    /* Wait for maximum 10 seconds. */
    connectionResult = USBH_WaitForDeviceConnectionB( tmpBuf, 10 );

    if ( connectionResult == USB_STATUS_OK )
    {
      printf("\nA device was attached...\n");

      if (USBH_QueryDeviceB(tmpBuf, sizeof(tmpBuf), USBH_GetPortSpeed())
          == USB_STATUS_OK)
      {
        USBH_InitDeviceData(&device, tmpBuf, NULL, 0, USBH_GetPortSpeed());

        printf( "\nDevice VID/PID is 0x%04X/0x%04X, device bus speed is %s",
                device.devDesc.idVendor, device.devDesc.idProduct,
                USBH_GetPortSpeed() == PORT_FULL_SPEED ? "FULL" : "LOW" );

        GetDeviceStrings();
      }
      else
      {
        printf("\nDevice enumeration failure, please unplug device...\n");
      }

      while ( USBH_DeviceConnected() ){}
      printf("\n\nDevice removal detected...");
    }

    else if ( connectionResult == USB_STATUS_DEVICE_MALFUNCTION )
    {
      printf("\nA malfunctioning device was attached, please remove...\n");
    }

    else if ( connectionResult == USB_STATUS_PORT_OVERCURRENT )
    {
      printf( "\nVBUS overcurrent condition, please remove device.\n" );
    }

    else if ( connectionResult == USB_STATUS_TIMEOUT )
    {
      printf("\nNo device was attached...\n");
    }
  }
}

/**************************************************************************//**
 * @brief Print USB device strings on console.
 *****************************************************************************/
static void GetDeviceStrings(void)
{
  /* Get and print device string descriptors. */

  if (device.devDesc.iManufacturer)
  {
    USBH_GetStringB(&device, tmpBuf, 255, device.devDesc.iManufacturer,
                    USB_LANGID_ENUS);
    USBH_PrintString("\niManufacturer = \"",
                     (USB_StringDescriptor_TypeDef*) tmpBuf, "\"");
  }
  else
  {
    printf("\niManufacturer = <NONE>");
  }

  if (device.devDesc.iProduct)
  {
    USBH_GetStringB(&device, tmpBuf, 255, device.devDesc.iProduct,
                    USB_LANGID_ENUS);
    USBH_PrintString("\niProduct      = \"",
                     (USB_StringDescriptor_TypeDef*) tmpBuf, "\"");
  }
  else
  {
    printf("\niProduct      = <NONE>");
  }

  if (device.devDesc.iSerialNumber)
  {
    USBH_GetStringB(&device, tmpBuf, 255, device.devDesc.iSerialNumber,
                    USB_LANGID_ENUS);
    USBH_PrintString("\niSerialNumber = \"",
                     (USB_StringDescriptor_TypeDef*) tmpBuf, "\"\n");
  }
  else
  {
    printf("\niSerialNumber = <NONE>\n");
  }
}

/**************************************************************************//**
 * @brief Initialize console I/O redirection.
 *****************************************************************************/
static void ConsoleDebugInit(void)
{
  RETARGET_SerialInit();                        /* Initialize USART   */
  RETARGET_SerialCrLf(1);                       /* Map LF to CRLF     */
}
