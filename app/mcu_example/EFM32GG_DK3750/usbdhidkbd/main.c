/**************************************************************************//**
 * @file main.c
 * @brief USB HID keyboard device example.
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
#include "em_gpio.h"
#include "bsp.h"
#include "bsp_trace.h"
#include "retargetserial.h"
#include <stdio.h>

#include "em_usb.h"
#include "descriptors.h"

/**************************************************************************//**
 *
 * This example shows how a HID keyboard can be implemented.
 *
 *****************************************************************************/

/*** Typedef's and defines. ***/

#define SCAN_TIMER              1       /* Timer used to scan keyboard. */
#define SCAN_RATE               50

#define HEARTBEAT_MASK          0xF
#define KEYLED_MASK             0x8000
#define KBDLED_MASK             0xF00
#define ACTIVITY_LED_PORT       gpioPortE   /* The blue led labeled STATUS. */
#define ACTIVITY_LED_PIN        1
#define BUTTON_PORT             gpioPortE
#define BUTTON_PIN              0


/*** Function prototypes. ***/

static void OutputReportReceived( uint8_t report );
static void StateChange( USBD_State_TypeDef oldState,
                         USBD_State_TypeDef newState );

/*** Variables ***/

static int      keySeqNo;           /* Current position in report table. */
static bool     keyPushed;          /* Current pushbutton status. */
static uint16_t leds;

static const USBD_Callbacks_TypeDef callbacks =
{
  .usbReset        = NULL,
  .usbStateChange  = StateChange,
  .setupCmd        = HIDKBD_SetupCmd,
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
int main( void )
{
  HIDKBD_Init_t hidInitStruct;

#if defined( BUSPOWERED )
  CMU_ClockEnable( cmuClock_GPIO, true );
  GPIO_PinModeSet( BUTTON_PORT, BUTTON_PIN, gpioModeInputPull, 1 );
  GPIO_PinModeSet( ACTIVITY_LED_PORT, ACTIVITY_LED_PIN, gpioModePushPull, 0 );
#else
  BSP_Init( BSP_INIT_DEFAULT ); /* Initialize DK board register access     */

  /* If first word of user data page is non-zero, enable eA Profiler trace */
  BSP_TraceProfilerSetup();
#endif

  CMU_ClockSelectSet( cmuClock_HF, cmuSelect_HFXO );

  leds = 0;
#if !defined( BUSPOWERED )
  BSP_LedsSet( leds );

  /* Initialize console I/O redirection. */
  RETARGET_SerialInit();        /* Initialize DK UART port */
  RETARGET_SerialCrLf( 1 );     /* Map LF to CRLF          */

  printf("\nEFM32 USB HID Keyboard device example\n");
#endif

  /* Initialize HID keyboard driver. */
  hidInitStruct.hidDescriptor = (void*)USBDESC_HidDescriptor;
  hidInitStruct.setReportFunc = OutputReportReceived;
  HIDKBD_Init( &hidInitStruct );

  /* Initialize and start USB device stack. */
  USBD_Init( &usbInitStruct );

  /*
   * When using a debugger it is practical to uncomment the following three
   * lines to force host to re-enumerate the device.
   */
  /* USBD_Disconnect();      */
  /* USBTIMER_DelayMs(1000); */
  /* USBD_Connect();         */

  for (;;)
  {
  }
}

/**************************************************************************//**
 * @brief
 *   Timeout function for keyboard scan timer.
 *   Scan keyboard to check for key press/release events.
 *   This function is called at a fixed rate.
 *****************************************************************************/
static void ScanTimeout( void )
{
  bool pushed;
  HIDKBD_KeyReport_t *report;

  /* Check pushbutton */
#if defined( BUSPOWERED )
  pushed = GPIO_PinInGet( BUTTON_PORT, BUTTON_PIN ) == 0;
#else
  pushed = BSP_PushButtonsGet() & 1;
#endif

  /* Update LED's */
  leds = (leds & ~(HEARTBEAT_MASK | KEYLED_MASK)) |
         (((leds & HEARTBEAT_MASK) + 1) & HEARTBEAT_MASK) |
         (pushed ? KEYLED_MASK : 0);

#if defined( BUSPOWERED )
  if ( !keyPushed )
    GPIO_PinOutToggle( ACTIVITY_LED_PORT, ACTIVITY_LED_PIN );
#else
  BSP_LedsSet( leds );
#endif

  if ( pushed != keyPushed )  /* Any change in keyboard status ? */
  {
    if ( pushed )
    {
      report = (void*)&USBDESC_reportTable[ keySeqNo ];
    }
    else
    {
      report = (void*)&USBDESC_noKeyReport;
    }

    /* Pass keyboard report on to the HID keyboard driver. */
    HIDKBD_KeyboardEvent( report );
  }

  /* Keep track of the new keypush event (if any) */
  if ( pushed && !keyPushed )
  {
    /* Advance to next position in report table */
    keySeqNo++;
    if ( keySeqNo == (sizeof(USBDESC_reportTable) / sizeof(HIDKBD_KeyReport_t)))
    {
      keySeqNo = 0;
    }
#if defined( BUSPOWERED )
    GPIO_PinOutSet( ACTIVITY_LED_PORT, ACTIVITY_LED_PIN );
#else
    putchar( '.' );
#endif
  }
  keyPushed = pushed;

  /* Restart keyboard scan timer */
  USBTIMER_Start( SCAN_TIMER, SCAN_RATE, ScanTimeout );
}

/**************************************************************************//**
 * @brief
 *   Callback function called each time the USB device state is changed.
 *   Starts HID operation when device has been configured by USB host.
 *
 * @param[in] oldState The device state the device has just left.
 * @param[in] newState The new device state.
 *****************************************************************************/
static void StateChange( USBD_State_TypeDef oldState,
                         USBD_State_TypeDef newState )
{
  /* Call HIDKBD drivers state change event handler. */
  HIDKBD_StateChangeEvent( oldState, newState );

  if ( newState == USBD_STATE_CONFIGURED )
  {
    /* We have been configured, start HID functionality ! */
    if ( oldState != USBD_STATE_SUSPENDED )   /* Resume ?   */
    {
      leds            = 0;
      keySeqNo        = 0;
      keyPushed       = false;
#if defined( BUSPOWERED )
      GPIO_PinOutSet( ACTIVITY_LED_PORT, ACTIVITY_LED_PIN );
#else
      BSP_LedsSet( leds );
#endif
    }
    USBTIMER_Start( SCAN_TIMER, SCAN_RATE, ScanTimeout );
  }

  else if ( ( oldState == USBD_STATE_CONFIGURED ) &&
            ( newState != USBD_STATE_SUSPENDED  )    )
  {
    /* We have been de-configured, stop HID functionality */
    USBTIMER_Stop( SCAN_TIMER );
#if defined( BUSPOWERED )
    GPIO_PinOutClear( ACTIVITY_LED_PORT, ACTIVITY_LED_PIN );
#endif
  }

  else if ( newState == USBD_STATE_SUSPENDED )
  {
    /* We have been suspended, stop HID functionality */
    /* Reduce current consumption to below 2.5 mA.    */
#if defined( BUSPOWERED )
    GPIO_PinOutClear( ACTIVITY_LED_PORT, ACTIVITY_LED_PIN );
#endif
    USBTIMER_Stop( SCAN_TIMER );
  }

#if !defined( BUSPOWERED )
  putchar( '\n' );
  printf( USBD_GetUsbStateName( oldState ) );
  printf( " -> ");
  printf( USBD_GetUsbStateName( newState ) );
#endif
}

/**************************************************************************//**
 * @brief
 *   Callback function called when the data stage of a USB_HID_SET_REPORT
 *   setup command has completed.
 *
 * @param[in] report Output report byte.
 *                   @n Bit 0 : State of keyboard NumLock LED.
 *                   @n Bit 1 : State of keyboard CapsLock LED.
 *                   @n Bit 2 : State of keyboard ScrollLock LED.
 *****************************************************************************/
static void OutputReportReceived( uint8_t report )
{
  /* We have received new data for NumLock, CapsLock and ScrollLock LED's */

  leds = (leds & ~KBDLED_MASK) | (report << 8);

#if !defined( BUSPOWERED )
  BSP_LedsSet( leds );
  putchar( '.' );
#endif
}
