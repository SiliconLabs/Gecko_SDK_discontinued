/***************************************************************************//**
 * @file hal/micro/cortexm3/efm32/bootloader/bootloader-gpio.c
 * @brief EFM32 bootloader gpio interface
 * @version
 *******************************************************************************
 * @section License
 * <b>(C) Copyright 2016 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Silicon Labs has no
 * obligation to support this Software. Silicon Labs is providing the
 * Software "AS IS", with no express or implied warranties of any kind,
 * including, but not limited to, any implied warranties of merchantability
 * or fitness for any particular purpose or warranties against infringement
 * of any proprietary rights of a third party.
 *
 * Silicon Labs will not be liable for any consequential, incidental, or
 * special damages, or any other relief, or for any claim by any third party,
 * arising from your use of this Software.
 *
 ******************************************************************************/

#include PLATFORM_HEADER
#include "stack/include/ember.h"
#include "hal.h"
#include "bootloader-common.h"
// #include "standalone-bootloader.h"
#include "bootloader-gpio.h"

#include "em_gpio.h"
#include "em_cmu.h"

//[[
////////////////////////////////////////////////////////////////////////////
// If you edit this file, be sure to also edit bootloader-gpio-ezsp-spi.c //
////////////////////////////////////////////////////////////////////////////
//]]

#ifdef  USE_BUTTON_RECOVERY
#ifndef BUTTON_RECOVERY_PORT
  // Default to using BUTTON0 or PF6 for the bootloader recovery GPIO.
  // This can be overridden in the BOARD_HEADER.
  #ifdef BUTTON0
  #define BUTTON_RECOVERY_PORT  BUTTON0_PORT
  #define BUTTON_RECOVERY_PIN   BUTTON0
  #else
  #define BUTTON_RECOVERY_PORT  gpioPortF
  #define BUTTON_RECOVERY_PIN   6
  #endif
#endif//BUTTON_RECOVERY_PORT_PIN
#endif//USE_BUTTON_RECOVERY

#define BUTTON_RECOVERY_SET()     GPIO_PinOutSet(BUTTON_RECOVERY_PORT, BUTTON_RECOVERY_PIN)
#define BUTTON_RECOVERY_PRESSED() ((GPIO_PinInGet(BUTTON_RECOVERY_PORT, BUTTON_RECOVERY_PIN) ? false : true))

// Function Name: bootloadForceActivation
// Description:   Decides whether to continue launching the bootloader or vector
//                to the application instead. This decision is usually based on
//                some sort of GPIO logic.
//                (ie. a hardware trigger).
// NOTE!!!        This function is called extremely early in the boot process
//                prior to the chip being fully configured and before memory
//                is initialized, so limited functionality is available.
//                If this function returns false any state altered should
//                be returned to its reset state.
// Parameters:    none
// Returns:       false (0) if bootloader should not be launched.
//                (This will try to execute the application if possible.)
//                true (1) if bootloader should be launched.
//
bool bootloadForceActivation( void )
{
  #if defined(USE_BUTTON_RECOVERY)
    uint32_t i;
    bool pressed;
    // this provides an example of an alternative recovery mode activation
    //  method by utilizing one of the breakout board buttons
    CMU_ClockEnable(cmuClock_HFPER, true);
    CMU_ClockEnable(cmuClock_GPIO, true);

    // Since the button may have decoupling caps, they may not be charged
    //  after a power-on and could give a false positive result.  To avoid
    //  this issue, drive the output as an output for a short time to charge
    //  them up as quickly as possible
    GPIO_PinModeSet(BUTTON_RECOVERY_PORT, BUTTON_RECOVERY_PIN, gpioModePushPull, 1);
    BUTTON_RECOVERY_SET();
    for(i=0; i<100; i++)
      __no_operation();

    // Reconfigure as an input with pullup to read the button state
    GPIO_PinModeSet(BUTTON_RECOVERY_PORT, BUTTON_RECOVERY_PIN, gpioModeInputPull, 1);
    // (IO was already set to enable the pullup above)
    // We have to delay again here so that if the button is depressed the
    //  cap has time to discharge again after being charged up
    //  by the above delay
    for(i=0; i<500; i++)
      __no_operation();
    pressed = BUTTON_RECOVERY_PRESSED();

    return pressed;
  #else
    return false;
  #endif
}


//
// Function Name: bootloadGpioInit
// Description:   This function selects the chip's GPIO configuration. These
//                settings are the minimum base configuration needed to
//                support the bootloader functionality. These settings can
//                be modified for custom applications as long as the base
//                settings are preserved.
// Parameters:    none
// Returns:       none
//
void bootloadGpioInit(void)
{
  halInternalEnableWatchDog();

  #ifndef NO_LED
    halInternalInitLed();
  #endif
}


// bootloadStateIndicator
//
// Called by the bootloader state macros (in bootloader-gpio.h). Used to blink
// the LED's or otherwise signal bootloader activity.
//
// Current settings:
// Enabled: Turn on yellow activity LED on bootloader up and successful bootload
//          Turn off yellow activity LED on bootload failure
// Disabled: Blink red heartbeat LED during idle polling.
void bootloadStateIndicator(enum blState_e state)
{
  // sample state indication using LEDs
  #ifndef NO_LED
    static uint16_t pollCntr = 0;

    switch(state) {
      case BL_ST_UP:                      // bootloader up
        halSetLed(BOARD_ACTIVITY_LED);
        break;

      case BL_ST_DOWN:                    // bootloader going down
        break;

      case BL_ST_POLLING_LOOP:            // Polling for serial or radio input in
                                          // standalone bootloader.
        if(0 == pollCntr--) {
          halToggleLed(BOARD_HEARTBEAT_LED);
          pollCntr = 10000;
        }
        break;

      case BL_ST_DOWNLOAD_LOOP:           // processing download image
        halClearLed(BOARD_HEARTBEAT_LED);
        halToggleLed(BOARD_ACTIVITY_LED);
        break;

      case BL_ST_DOWNLOAD_SUCCESS:
        halSetLed(BOARD_ACTIVITY_LED);
        break;

      case BL_ST_DOWNLOAD_FAILURE:
        halClearLed(BOARD_ACTIVITY_LED);
        break;

      default:
        break;
    }
  #endif
}
