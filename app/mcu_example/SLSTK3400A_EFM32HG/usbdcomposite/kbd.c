/**************************************************************************//**
 * @file main.c
 * @brief USB HID keyboard.
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

#include "hidkbd.h"
#include "descriptors.h"

/**************************************************************************//**
 *
 * This example shows how a composite USB device can be implemented.
 *
 *****************************************************************************/

/*** Typedef's and defines. ***/

#define SCAN_RATE               50
#define BUTTON_PORT             gpioPortC
#define BUTTON_PIN              9

/*** Function prototypes. ***/

static void ScanTimeout( void );

/*** Variables ***/

static int      keySeqNo;           /* Current position in report table. */
static bool     keyPushed;          /* Current pushbutton status. */

/**************************************************************************//**
 * @brief HID keyboard device initialization.
 *****************************************************************************/
void KBD_Init( void )
{
  HIDKBD_Init_t hidInitStruct;

  CMU_ClockEnable( cmuClock_GPIO, true );
  GPIO_PinModeSet( BUTTON_PORT, BUTTON_PIN, gpioModeInputPull, 1 );

  /* Initialize HID keyboard driver. */
  hidInitStruct.hidDescriptor = (void*)USBDESC_HidDescriptor;
  hidInitStruct.setReportFunc = NULL;
  HIDKBD_Init( &hidInitStruct );
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
int KBD_SetupCmd( const USB_Setup_TypeDef *setup )
{
  return HIDKBD_SetupCmd( setup );
}

/**************************************************************************//**
 * @brief
 *   Callback function called each time the USB device state is changed.
 *   Starts keyboard scanning when device has been configured by USB host.
 *
 * @param[in] oldState The device state the device has just left.
 * @param[in] newState The new device state.
 *****************************************************************************/
void KBD_StateChangeEvent( USBD_State_TypeDef oldState,
                           USBD_State_TypeDef newState)
{
  HIDKBD_StateChangeEvent( oldState, newState );

  if ( newState == USBD_STATE_CONFIGURED )
  {
    /* We have been configured, start scanning the keyboard ! */
    if ( oldState != USBD_STATE_SUSPENDED ) /* Resume ?   */
    {
      keySeqNo        = 0;
      keyPushed       = false;
    }
    USBTIMER_Start( HIDKBD_SCAN_TIMER_ID, SCAN_RATE, ScanTimeout );
  }

  else if ( ( oldState == USBD_STATE_CONFIGURED ) &&
            ( newState != USBD_STATE_SUSPENDED  )    )
  {
    /* We have been de-configured, stop keyboard scanning. */
    USBTIMER_Stop( HIDKBD_SCAN_TIMER_ID );
  }

  else if ( newState == USBD_STATE_SUSPENDED )
  {
    /* We have been suspended, stop keyboard scanning. */
    /* Reduce current consumption to below 2.5 mA.     */
    USBTIMER_Stop( HIDKBD_SCAN_TIMER_ID );
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
  pushed = GPIO_PinInGet( BUTTON_PORT, BUTTON_PIN ) == 0;

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
  }
  keyPushed = pushed;

  /* Restart keyboard scan timer */
  USBTIMER_Start( HIDKBD_SCAN_TIMER_ID, SCAN_RATE, ScanTimeout );
}
