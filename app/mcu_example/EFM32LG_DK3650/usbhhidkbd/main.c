/**************************************************************************//**
 * @file main.c
 * @brief USB host stack HID keyboard example project.
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
#include "usbkbdscancodes.h"
#include <string.h>

/**************************************************************************//**
 *
 * This example shows how the USB host stack can be used to serve a
 * HID keyboard.
 *
 *****************************************************************************/

/*** Typedef's and defines. ***/

//#define DO_SUSPEND_CURRENT_TEST       /* For internal Silicon Labs testing. */
//#define DO_RESET_FROM_SUSPEND_TEST    /* For internal Silicon Labs testing. */

/* Timer indices. */
#define KBD_LED_TIMER     0
#define KBD_POLL_TIMER    1
#define KBD_SUSPEND_TIMER 2

#define KBD_USB_ADDRESS   1     /* USB address used for keyboard. */
#define KBD_INT_IN_HC     2     /* Host channel nr. used for keyboard     */
                                /* interrupt IN endpoint. Ch. 0 and 1     */
                                /* defaults to the control endpoint (EP0) */
#define KBD_IN_REPORT_LEN 8     /* Length of keyboard interrupt IN report.*/

/* USB HID Descriptor. */
SL_PACK_START(1)
typedef struct
{
  uint8_t   bLength;            /* Numeric expression that is the total size of the HID descriptor.       */
  uint8_t   bDescriptorType;    /* Constant name specifying type of HID descriptor.                       */
  uint16_t  bcdHID;             /* Numeric expression identifying the HID Class Specification release.    */
  uint8_t   bCountryCode;       /* Numeric expression identifying country code of the localized hardware. */
  uint8_t   bNumDescriptors;    /* Numeric expression specifying the number of class descriptors.         */
  uint8_t   bReportDescriptorType;  /* Constant name identifying type of class descriptor.                */
  uint16_t  wDescriptorLength;  /* Numeric expression that is the total size of the Report descriptor.    */
} SL_ATTRIBUTE_PACKED USB_HIDDescriptor_TypeDef;
SL_PACK_END()

/*** Function prototypes. ***/

static bool CheckDevice( void );
static void ConsoleDebugInit( void );
static void HidCheckKeyPress( void );
static int  HidSetIdle( int durationMs, uint8_t reportId );
static int  HidSetProtocol( void );
static int  HidSetReport( uint8_t value );
static void InitKbd( void );
static void LedTimeout( void );
static void PollTimeout( void );
static void PrintDeviceStrings( USBH_Device_TypeDef *dev );
static void SuspendTimeout( void );

/*** Variables ***/

STATIC_UBUF(                tmpBuf, 1024 );
static USBH_Device_TypeDef  device;
static USBH_Ep_TypeDef      ep;
static volatile bool        ledTimerDone;
static volatile bool        pollTimerDone;
static volatile bool        suspendTimerDone;
static uint8_t              currentKeypressStatus[6];

/**************************************************************************//**
 * @brief main - the entrypoint after reset.
 *****************************************************************************/
int main( void )
{
  int connectionResult;
  uint8_t hidReport;
  USBH_Init_TypeDef initstruct = USBH_INIT_DEFAULT;

  BSP_Init(BSP_INIT_DEFAULT);   /* Initialize DK board register access */

  /* If first word of user data page is non-zero, enable eA Profiler trace */
  BSP_TraceProfilerSetup();

  CMU_ClockSelectSet( cmuClock_HF, cmuSelect_HFXO );

  ConsoleDebugInit();           /* Initialize DK UART port. */

  printf( "\n\nEFM32 USB Host HID keyboard example.\n" );


  for (;;)
  {
    /* Initialize USB HOST stack */
    USBH_Init( &initstruct );

    /* Wait for ever on keyboard attach... */
    printf( "\nWaiting for USB keyboard plug-in...\n" );
    connectionResult = USBH_WaitForDeviceConnectionB( tmpBuf, 0 );

    if ( connectionResult == USB_STATUS_DEVICE_MALFUNCTION )
    {
      printf( "\nA malfunctioning device was attached, please remove device.\n" );
    }

    else if ( connectionResult == USB_STATUS_PORT_OVERCURRENT )
    {
      printf( "\nVBUS overcurrent condition, please remove device.\n" );
    }

    else if ( connectionResult == USB_STATUS_OK )
    {
      printf( "\nA device was attached...\n" );

      /* Check if a valid USB HID keyboard device is attached. */
      if ( CheckDevice() )
      {
        /* Initialize and configure keyboard. */
        InitKbd();

        hidReport        = 0;
        memset(currentKeypressStatus, 0, sizeof(currentKeypressStatus));
        ledTimerDone     = false;
        pollTimerDone    = false;
        suspendTimerDone = false;
        USBTIMER_Start( KBD_LED_TIMER, 1000, LedTimeout );
        USBTIMER_Start( KBD_POLL_TIMER, ep.epDesc.bInterval, PollTimeout );
        USBTIMER_Start( KBD_SUSPEND_TIMER, 5000, SuspendTimeout );

        /*--------------- MAIN Keyboard polling loop ------------------------*/
        while ( USBH_DeviceConnected() )
        {
          if ( ledTimerDone )
          {
            hidReport = ( hidReport + 1 ) % 8;
            HidSetReport( hidReport );            /* Set keyboard LED's. */
            ledTimerDone = false;
            USBTIMER_Start( KBD_LED_TIMER, 1000, LedTimeout );
          }

          if ( pollTimerDone )
          {
            HidCheckKeyPress();                  /* Any key pressed ? */
            pollTimerDone = false;
            USBTIMER_Start( KBD_POLL_TIMER, ep.epDesc.bInterval, PollTimeout );
          }

          if ( suspendTimerDone )
          {
            suspendTimerDone = false;

#if defined( DO_SUSPEND_CURRENT_TEST )      /* Internal Silicon Labs testing. */

            /* Do a control transfer immediately before suspend. */
            hidReport = ( hidReport + 1 ) % 8;
            HidSetReport( hidReport );

            USBH_PortSuspend();
            printf( "\nSuspend device" );
            USBTIMER_DelayMs( 2000 );

#if defined( DO_RESET_FROM_SUSPEND_TEST )
            printf( "\nReset device while suspended" );
            USBH_PortReset();
            break;
#else
            USBTIMER_DelayMs( 2000 );
            printf( "\nResume device" );
            USBH_PortResume();

            /* Test Resume recovery time. */
            USBTIMER_DelayMs( 10 );
            hidReport = ( hidReport + 1 ) % 8;
            HidSetReport( hidReport );
            USBTIMER_Start( KBD_SUSPEND_TIMER, 5000, SuspendTimeout );
#endif
#endif
          }
        }
        printf( "\n\nDevice removal detected...\n" );
        /*-------------------------------------------------------------------*/
      }
    }

    USBTIMER_Stop( KBD_LED_TIMER );
    USBTIMER_Stop( KBD_POLL_TIMER );
    USBTIMER_Stop( KBD_SUSPEND_TIMER );

    /* Wait for malfunctional device or unknown HID keyboard removal. */
    while ( USBH_DeviceConnected() ){}

    /* Disable USB peripheral, power down USB port. */
    USBH_Stop();
  }
}

/**************************************************************************//**
 * @brief
 *   Check if a newly attached device is a valid HID keyboard.
 *
 * @return True if valid HID keyboard device, false otherwise.
 *****************************************************************************/
static bool CheckDevice( void )
{
  int i;
  USBH_Device_TypeDef *dev;

  /* Get all device descriptors. */

  if ( USBH_QueryDeviceB( tmpBuf, sizeof(tmpBuf), USBH_GetPortSpeed() )
       == USB_STATUS_OK)
  {
    /* Print basic device information. */
    dev = (USBH_Device_TypeDef*)tmpBuf;
    printf( "\nDevice VID/PID is 0x%04X/0x%04X, device bus speed is %s",
            dev->devDesc.idVendor, dev->devDesc.idProduct,
            USBH_GetPortSpeed() == PORT_FULL_SPEED ? "FULL" : "LOW" );
    PrintDeviceStrings( (USBH_Device_TypeDef*)tmpBuf );

    /* Print all descriptors on console. */
    USBH_PrintDeviceDescriptor(        USBH_QGetDeviceDescriptor(        tmpBuf ) );
    USBH_PrintConfigurationDescriptor( USBH_QGetConfigurationDescriptor( tmpBuf, 0 ), USB_CONFIG_DESCSIZE );
    USBH_PrintInterfaceDescriptor(     USBH_QGetInterfaceDescriptor(     tmpBuf, 0, 0 ) );

    for ( i=0; i<USBH_QGetInterfaceDescriptor( tmpBuf, 0, 0 )->bNumEndpoints; i++ )
    {
      USBH_PrintEndpointDescriptor(    USBH_QGetEndpointDescriptor( tmpBuf, 0, 0, i ) );
    }

    /* Qualify the device */
    if ( ( USBH_QGetInterfaceDescriptor( tmpBuf, 0, 0 )->bInterfaceClass    == USB_CLASS_HID          ) &&
         ( USBH_QGetInterfaceDescriptor( tmpBuf, 0, 0 )->bInterfaceProtocol == USB_CLASS_HID_KEYBOARD )    )
    {
      printf( "\nValid HID keyboard device.\n" );
      return true;
    }
    else
    {
      printf( "\nNot a valid HID keyboard device, please remove device.\n" );
    }
  }
  else
  {
    printf( "\nDevice enumeration failure, please remove device.\n" );
  }

  return false;
}

/**************************************************************************//**
 * @brief Initialize console I/O redirection.
 *****************************************************************************/
static void ConsoleDebugInit( void )
{
  RETARGET_SerialInit();                        /* Initialize USART   */
  RETARGET_SerialCrLf( 1 );                     /* Map LF to CRLF     */
}

/**************************************************************************//**
 * @brief
 *   Check if a button has been pressed on the keyboard. Prints the ASCII
 *   equivalent of the keyboard scancode on the console.
 *****************************************************************************/
static void HidCheckKeyPress( void )
{
  char c;
  bool found;
  unsigned i,j;

  if ( USBH_ReadB( &ep, tmpBuf, ep.epDesc.wMaxPacketSize, 3 )
       == KBD_IN_REPORT_LEN )
  {
    // This loop handles n-key rollover (NKRO)
    for (i=0;i<sizeof(currentKeypressStatus);i++)
    {
      c = tmpBuf[i+2];
      if (c && (c != 0x01 /*Keyboard ErrorRollOver*/))
      {
        found = false;
        // Check if key is currently pressed
        for (j=0;j<sizeof(currentKeypressStatus);j++)
        {
          if (c==currentKeypressStatus[j])
          {
            found = true;
            break;
          }
        }
        if (!found)
        {
          // New keypress, output on serialport
          putchar(USB_HidScancodeToAscii(c));
        }
      }
    }
    if (c != 0x01 /*Keyboard ErrorRollOver*/)
    {
      memcpy(currentKeypressStatus, tmpBuf, sizeof(currentKeypressStatus));
    }
  }
}

/**************************************************************************//**
 * @brief
 *   Send HID class USB_HID_SET_IDLE setup command to the keyboard.
 *
 * @param[in] durationMs  How often the keyboard shall report status changes.
 *                        Unit is millisecond
 * @param[in] reportId    The ID of the status change report.
 *
 * @return USB_STATUS_OK, or an appropriate USB transfer error code.
 *****************************************************************************/
static int HidSetIdle( int durationMs, uint8_t reportId )
{
  if ( durationMs >= ( 4 * 255 ) )
  {
    durationMs = 4 * 255;
  }

  return USBH_ControlMsgB(
                    &device.ep0,
                    USB_SETUP_DIR_H2D | USB_SETUP_RECIPIENT_INTERFACE |
                    USB_SETUP_TYPE_CLASS_MASK,            /* bmRequestType */
                    USB_HID_SET_IDLE,                     /* bRequest      */
                    ((durationMs/4)<<8)| reportId,        /* wValue        */
                    0,                                    /* wIndex        */
                    0,                                    /* wLength       */
                    NULL,                                 /* void* data    */
                    1000 );                               /* int timeout   */
}

/**************************************************************************//**
 * @brief
 *   Send HID class USB_HID_SET_PROTOCOL setup command to the keyboard.
 *
 * @return USB_STATUS_OK, or an appropriate USB transfer error code.
 *****************************************************************************/
static int HidSetProtocol( void )
{
  return USBH_ControlMsgB(
                    &device.ep0,
                    USB_SETUP_DIR_H2D | USB_SETUP_RECIPIENT_INTERFACE |
                    USB_SETUP_TYPE_CLASS_MASK,            /* bmRequestType */
                    USB_HID_SET_PROTOCOL,                 /* bRequest      */
                    0,                                    /* wValue 0=boot protocol */
                    0,                                    /* wIndex        */
                    0,                                    /* wLength       */
                    NULL,                                 /* void* data    */
                    1000 );                               /* int timeout   */
}

/**************************************************************************//**
 * @brief
 *   Send HID class USB_HID_SET_REPORT setup command to the keyboard.
 *
 * @param[in] value Bitmask corresponding to keyboard LED indicators.
 *
 * @return USB_STATUS_OK, or an appropriate USB transfer error code.
 *****************************************************************************/
static int HidSetReport( uint8_t value )
{
  tmpBuf[ 0 ] = value;

  return USBH_ControlMsgB(
                    &device.ep0,
                    USB_SETUP_DIR_H2D | USB_SETUP_RECIPIENT_INTERFACE |
                    USB_SETUP_TYPE_CLASS_MASK,            /* bmRequestType */
                    USB_HID_SET_REPORT,                   /* bRequest      */
                    2<<8,                                 /* wValue msb=2 => output report */
                    0,                                    /* wIndex        */
                    1,                                    /* wLength       */
                    tmpBuf,                               /* void* data    */
                    1000 );                               /* int timeout   */
}

/**************************************************************************//**
 * @brief
 *   Enumerate and configure a HID keyboard.
 *****************************************************************************/
static void InitKbd( void )
{
  USB_HIDDescriptor_TypeDef *hidd;

  /* Print the HID descriptor, assume that it is located just after   */
  /* the interface descriptor.                                        */

  hidd = (USB_HIDDescriptor_TypeDef*)
         ( (uint8_t*)USBH_QGetInterfaceDescriptor( tmpBuf, 0, 0 ) +
           sizeof( USB_InterfaceDescriptor_TypeDef ) );

  printf( "\nHID descriptor:" );
  printf( "\n bLength                %d",     hidd->bLength               );
  printf( "\n bDescriptorType        0x%02X", hidd->bDescriptorType       );
  printf( "\n bcdHID                 0x%04X", hidd->bcdHID                );
  printf( "\n bCountryCode           0x%02X", hidd->bCountryCode          );
  printf( "\n bNumDescriptors        %d",     hidd->bNumDescriptors       );
  printf( "\n bReportDescriptorType  0x%02X", hidd->bReportDescriptorType );
  printf( "\n wDescriptorLength      %d\n",   hidd->wDescriptorLength     );

  /* Initialize device data structure, assume device has 1 endpoint. */
  USBH_InitDeviceData( &device, tmpBuf, &ep, 1, USBH_GetPortSpeed() );

  USBH_SetAddressB( &device, KBD_USB_ADDRESS );
  USBH_SetConfigurationB( &device, device.confDesc.bConfigurationValue );

  /* Assign a Host Channel to the interrupt IN endpoint. */
  USBH_AssignHostChannel( &ep, KBD_INT_IN_HC );

  HidSetIdle( 0, 0 );
  HidSetProtocol();
}

/**************************************************************************//**
 * @brief
 *   Called each time the keyboard LED timer expires.
 *****************************************************************************/
static void LedTimeout( void )
{
  ledTimerDone = true;
}

/**************************************************************************//**
 * @brief
 *   Called each time the keyboard interrupt IN endpoint should be checked.
 *****************************************************************************/
static void PollTimeout( void )
{
  pollTimerDone = true;
}

/***************************************************************************//**
 * @brief
 *   Print USB device strings.
 *
 * @param[in] dev Pointer to device data structure.
 ******************************************************************************/
static void PrintDeviceStrings( USBH_Device_TypeDef *dev )
{
  uint8_t tmp[ 256 ];

  /* Get and print device string descriptors. */

  if ( dev->devDesc.iManufacturer )
  {
    USBH_GetStringB( dev, tmp, 255, dev->devDesc.iManufacturer,USB_LANGID_ENUS);
    USBH_PrintString( "\niManufacturer = \"",
                      (USB_StringDescriptor_TypeDef*)tmp, "\"" );
  }
  else
  {
    printf( "\niManufacturer = <NONE>" );
  }

  if ( dev->devDesc.iProduct )
  {
    USBH_GetStringB( dev, tmp, 255, dev->devDesc.iProduct, USB_LANGID_ENUS );
    USBH_PrintString( "\niProduct      = \"",
                      (USB_StringDescriptor_TypeDef*)tmp, "\"" );
  }
  else
  {
    printf( "\niProduct      = <NONE>" );
  }

  if ( dev->devDesc.iSerialNumber )
  {
    USBH_GetStringB( dev, tmp, 255, dev->devDesc.iSerialNumber,USB_LANGID_ENUS);
    USBH_PrintString( "\niSerialNumber = \"",
                      (USB_StringDescriptor_TypeDef*)tmp, "\"\n" );
  }
  else
  {
    printf( "\niSerialNumber = <NONE>\n" );
  }
}

static void SuspendTimeout( void )
{
  suspendTimerDone = true;
}
