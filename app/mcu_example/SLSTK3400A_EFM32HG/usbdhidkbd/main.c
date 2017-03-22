/**************************************************************************//**
 * @file main.c
 * @brief USB HID keyboard device example (demonstrates Low Energy Mode USB).
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
#include "em_emu.h"
#include "em_gpio.h"

#include "em_usb.h"
#include "descriptors.h"
#include "display.h"
#include "scrolllcd.h"
#include "image.h"

/**************************************************************************//**
 *
 * This example shows how a HID keyboard can be implemented with optional LEM
 *
 *****************************************************************************/

/*** Typedef's and defines. ***/

#define SCAN_TIMER              1         /* Timer used to scan keyboard. */
#define SCAN_RATE               50

#define BUTTON0_PORT            gpioPortC	/* Pushbutton PB0 */
#define BUTTON0_PIN             9

#define BUTTON1_PORT            gpioPortC	/* Pushbutton PB1 */
#define BUTTON1_PIN             10

/*** Function prototypes. ***/

static void StateChange( USBD_State_TypeDef oldState,
                         USBD_State_TypeDef newState );

/*** Variables ***/

static int           keySeqNo;           /* Current position in report table. */
static bool          keyPushed;          /* Current pushbutton PB0 (keyboard key) status. */
static bool          leusbTogglePushed;  /* Current pushbutton PB1 (LEUSB toggle) status. */
static volatile bool leusbEnabled;       /* Current status of Low Energy USB mode. */
static volatile bool refreshDisplay;     /* Flag indicates when LCD Display needs update. */

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

static DISPLAY_Device_t displayDevice;            /* Display device handle. */
static char blank_image[ 128 * 16 ];

/**************************************************************************//**
 * @brief main - the entrypoint after reset.
 *****************************************************************************/
int main(void)
{
  /* Initialize LEUSB state variables */
  leusbTogglePushed = false;
  leusbEnabled = false;
  refreshDisplay = false;

  HIDKBD_Init_t hidInitStruct;

  /* Chip errata */
  CHIP_Init();

  /* Go slow to reduce current consumption. */
  CMU_HFRCOBandSet( cmuHFRCOBand_7MHz );

  CMU_ClockEnable( cmuClock_GPIO, true );
  GPIO_PinModeSet( BUTTON0_PORT, BUTTON0_PIN, gpioModeInputPull, 1 );
  GPIO_PinModeSet( BUTTON1_PORT, BUTTON1_PIN, gpioModeInputPull, 1 );

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
                                  /* start column, width */
                                  0, displayDevice.geometry.width,
                                  /* start row, height */
                                  0, displayDevice.geometry.height);
  scrollLcd( &displayDevice, scrollLeft, blank_image, usb_image );

  /* Initialize HID keyboard driver. */
  hidInitStruct.hidDescriptor = (void*)USBDESC_HidDescriptor;
  hidInitStruct.setReportFunc = NULL;
  HIDKBD_Init( &hidInitStruct );

  /* Initialize and start USB device stack. */
  USBD_Init( &usbInitStruct );

  /* Turn off the Low Energy Mode (LEM) features that were enabled in USBD_Init() */
  USB->CTRL &= ~USB_CTRL_LEMIDLEEN;	// LEUSB off to begin demo

  /*
   * When using a debugger it is practical to uncomment the following three
   * lines to force host to re-enumerate the device.
   */
  /* USBD_Disconnect();      */
  /* USBTIMER_DelayMs(1000); */
  /* USBD_Connect();         */

  for (;;)
  {
    if (refreshDisplay)
    {
      refreshDisplay = false; /* Clear the "refresh display" flag */
      if (leusbEnabled)
      {
        /* Update the LCD image to reflect USB Low Energy Mode enabled */
        displayDevice.pPixelMatrixDraw( &displayDevice, (void*)leusb_image,
                                        /* start column, width */
                                        0, displayDevice.geometry.width,
                                        /* start row, height */
                                        0, displayDevice.geometry.height);
      }
      else
      {
        /* Update the LCD image to reflect normal USB HID keyboard demo status */
        displayDevice.pPixelMatrixDraw( &displayDevice, (void*)usb_image,
                                        /* start column, width */
                                        0, displayDevice.geometry.width,
                                        /* start row, height */
                                        0, displayDevice.geometry.height);
      }
    }
    /* Conserve energy ! */
    EMU_EnterEM1();
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
  bool pushedPB0;
  bool pushedPB1;
  HIDKBD_KeyReport_t *report;

  /* Check pushbutton PB0 */
  pushedPB0 = GPIO_PinInGet( BUTTON0_PORT, BUTTON0_PIN ) == 0;

  if ( pushedPB0 != keyPushed )  /* Any change in keyboard status ? */
  {
    if ( pushedPB0 )
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
  if ( pushedPB0 && !keyPushed )
  {
    /* Advance to next position in report table */
    keySeqNo++;
    if ( keySeqNo == (sizeof(USBDESC_reportTable) / sizeof(HIDKBD_KeyReport_t)))
    {
      keySeqNo = 0;
    }
  }
  keyPushed = pushedPB0;

  /* Check pushbutton PB1 */
  pushedPB1 = GPIO_PinInGet( BUTTON1_PORT, BUTTON1_PIN ) == 0;

  if ( pushedPB1 != leusbTogglePushed )  /* Any change in LEUSB mode button ? */
  {
	leusbTogglePushed = pushedPB1;	/* Update PB1 status */
    if ( leusbTogglePushed )	/* LEUSB mode button was pressed (not released) */
    {
      if ( leusbEnabled ) /* User has requested to disable USB LEM */
      {
        /* Turn off Low Energy Mode (LEM) features. */
        USB->CTRL &= ~USB_CTRL_LEMIDLEEN;
      }
      else
      {
        /* Turn on Low Energy Mode (LEM) features. */
        USB->CTRL |= USB_CTRL_LEMIDLEEN;
      }
      refreshDisplay = true; /* Set the "refresh display" flag */
      leusbEnabled = !leusbEnabled;	/* Update LEUSB mode status */
    }
  }

  /* Restart keyboard scan timer */
  USBTIMER_Start( SCAN_TIMER, SCAN_RATE, ScanTimeout );
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
  /* Call HIDKBD drivers state change event handler. */
  HIDKBD_StateChangeEvent( oldState, newState );

  if ( newState == USBD_STATE_CONFIGURED )
  {
    /* We have been configured, start scanning the keyboard ! */
    if ( oldState != USBD_STATE_SUSPENDED ) /* Resume ?   */
    {
      keySeqNo        = 0;
      keyPushed       = false;
    }
    USBTIMER_Start( SCAN_TIMER, SCAN_RATE, ScanTimeout );
  }

  else if ( ( oldState == USBD_STATE_CONFIGURED ) &&
            ( newState != USBD_STATE_SUSPENDED  )    )
  {
    /* We have been de-configured, stop keyboard scanning. */
    USBTIMER_Stop( SCAN_TIMER );
  }

  else if ( newState == USBD_STATE_SUSPENDED )
  {
    /* We have been suspended, stop keyboard scanning. */
    /* Reduce current consumption to below 2.5 mA.     */
    USBTIMER_Stop( SCAN_TIMER );
  }
}
