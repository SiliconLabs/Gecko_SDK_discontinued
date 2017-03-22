#include PLATFORM_HEADER
#include "stack/include/ember.h"
#include "hal.h"
#include "bootloader-common.h"
#include "standalone-bootloader.h"
#include "bootloader-gpio.h"

///////////////////////////////////////////////////////////////////
// If you edit this file, be sure to also edit bootloader-gpio.c //
///////////////////////////////////////////////////////////////////

#ifndef BUTTON_RECOVERY_PORT_PIN
  // Default to using PB6 nWAKE for the bootloader recovery GPIO.
  // This can be overridden in the BOARD_HEADER.
  #define BUTTON_RECOVERY_PORT_PIN  PORTB_PIN(6)
  #define BUTTON_RECOVERY_SET()     (GPIO_PBSET = PB6)
  #define BUTTON_RECOVERY_PRESSED() ((GPIO_PBIN & PB6) ? false : true)
#endif//BUTTON_RECOVERY_PORT_PIN

// Function Name: bootloadForceActivation
// Description:   Decides whether to continue launching the bootloader or vector
//                to the application instead. PB6, used as nWAKE by the ezsp-spi
//                protocol, is read and if it is low, this function returns true.
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
    bool pressed = false;

    halGpioSetConfig(BUTTON_RECOVERY_PORT_PIN, GPIOCFG_IN_PUD);
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
    halGpioSetConfig(BUTTON_RECOVERY_PORT_PIN, GPIOCFG_IN);

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

  #ifdef BTL_HAS_RADIO
    if (paIsPresent()) {
      // The PHY_CONFIG token indicates that a power amplifier is present.
      // Not all power amplifiers require both TX_ACTIVE and nTX_ACTIVE.  If
      // your design does not require TX_ACTIVE or nTX_ACTIVE, you can remove
      // the GPIO configuration for the unused signal to save a bit of flash.

      // Configure GPIO PC5 to support TX_ACTIVE alternate output function.
      halGpioSetConfig(PORTC_PIN(5),GPIOCFG_OUT_ALT);

      // Configure GPIO PC6 to support nTX_ACTIVE alternate output function.
      halGpioSetConfig(PORTC_PIN(6),GPIOCFG_OUT_ALT);
    }
  #endif

  #ifndef NO_LED
    halGpioSetConfig(BOARD_ACTIVITY_LED, GPIOCFG_OUT);
    halClearLed(BOARD_ACTIVITY_LED);
    halGpioSetConfig(BOARD_HEARTBEAT_LED, GPIOCFG_OUT);
    halSetLed(BOARD_HEARTBEAT_LED);
  #endif

  CONFIGURE_EXTERNAL_REGULATOR_ENABLE();
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
