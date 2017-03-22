/***************************************************************************//**
 * @file hal/micro/cortexm3/efm32/bootloader/bootloader-gpio-ezsp-spi.c
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
#include "spi-protocol-device.h"

///////////////////////////////////////////////////////////////////
// If you edit this file, be sure to also edit bootloader-gpio.c //
///////////////////////////////////////////////////////////////////

#if !defined(BUTTON_RECOVERY_PORT) || !defined(BUTTON_RECOVERY_PIN)
  // Default to using nWake for the bootloader recovery GPIO.
  // This can be overridden in the BOARD_HEADER.
  #if defined(DISABLE_NWAKE)
    #error Please define BUTTON_RECOVERY_PORT and BUTTON_RECOVERY_PIN when disabling nWake
  #endif

  #if defined(BUTTON_RECOVERY_PORT) || defined(BUTTON_RECOVERY_PIN)
    #error Please define both BUTTON_RECOVERY_PORT and BUTTON_RECOVERY_PIN when overriding
  #endif

  #define BUTTON_RECOVERY_PORT      NWAKE_PORT
  #define BUTTON_RECOVERY_PIN       NWAKE_PIN
#endif//!defined(BUTTON_RECOVERY_PORT) || !defined(BUTTON_RECOVERY_PIN)

#define BUTTON_RECOVERY_SET()     GPIO_PinOutSet(BUTTON_RECOVERY_PORT, BUTTON_RECOVERY_PIN)
#define BUTTON_RECOVERY_PRESSED() (GPIO_PinInGet(BUTTON_RECOVERY_PORT, BUTTON_RECOVERY_PIN) == 0)

// Function Name: bootloadForceActivation
// Description:   Decides whether to continue launching the bootloader or vector
//                to the application instead. The nWAKE pin used by the ezsp-spi
//                protocol is read and if it is low, this function returns true.
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
    uint32_t i;

    CMU_ClockEnable(cmuClock_HFPER, true);
    CMU_ClockEnable(cmuClock_GPIO, true);
    bool pressed = false;

    GPIO_PinModeSet(BUTTON_RECOVERY_PORT, BUTTON_RECOVERY_PIN, gpioModeInputPull, 1);
    BUTTON_RECOVERY_SET();
    for (i = 0; i < 10; i++) {
      __no_operation();
    }
    for (i = 0; i < 200; i++) {
      if (BUTTON_RECOVERY_PRESSED()) {
        pressed = true;
        break;
      }
    }

    //restore IO to its reset state
    GPIO_PinModeSet(BUTTON_RECOVERY_PORT, BUTTON_RECOVERY_PIN, gpioModeDisabled, 0);
    CMU_ClockEnable(cmuClock_GPIO, false);

    return pressed;
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
