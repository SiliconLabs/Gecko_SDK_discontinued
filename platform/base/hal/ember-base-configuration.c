/** @file ember-base-configuration.c
 * @brief User-configurable stack memory allocation and convenience stubs 
 * for little-used callbacks. 
 *
 * \b Note: Application developers should \b not modify any portion
 * of this file. Doing so may lead to mysterious bugs. Allocations should be 
 * adjusted only with macros in a custom CONFIGURATION_HEADER. 
 *
 * <!--Copyright 2009 by Ember Corporation. All rights reserved.         *80*-->
 */
#include PLATFORM_HEADER 
#include "stack/include/ember.h"
#include "hal/hal.h"
#ifdef EMBER_STACK_COBRA
  #include "hal_uart.h"
#endif

#include "stack/config/ember-configuration-defaults.h"

#ifndef EMBER_APPLICATION_HAS_CUSTOM_RADIO_CALIBRATION_CALLBACK
// See stack-info.h for more information.
void emberRadioNeedsCalibratingHandler(void)
{
  // TODO: Failsafe any critical processes or peripherals.
  emberCalibrateCurrentChannel();
}
#endif//EMBER_APPLICATION_HAS_CUSTOM_RADIO_CALIBRATION_CALLBACK

#ifndef EMBER_APPLICATION_HAS_BUTTON_HANDLER
void halButtonIsr(uint8_t button, uint8_t state)
{
}
#endif

#if defined (CORTEXM3) || defined (EMBER_STACK_COBRA)
#ifndef EMBER_APPLICATION_HAS_CUSTOM_SIM_EEPROM_CALLBACK
#include "hal/plugin/sim-eeprom/sim-eeprom.h"
// The Simulated EEPROM Callback function.
void halSimEepromCallback(EmberStatus status)
{
  switch (status) {
  case EMBER_SIM_EEPROM_ERASE_PAGE_GREEN:
    //SimEE is asking for one page to be erased.
#ifdef EMBER_STACK_COBRA
    HalUARTRestrain();      // temporarily hold off serial input from host
#endif
    (void)halSimEepromErasePage();
    break;
  case EMBER_SIM_EEPROM_ERASE_PAGE_RED:
  case EMBER_SIM_EEPROM_FULL:
  { //SimEE says we're out of room!  Erase all pages now or data
    //currently being written will be dropped.
#ifdef EMBER_STACK_COBRA
    HalUARTRestrain();      // temporarily hold off serial input from host
#endif
    if(halSimEepromPagesRemainingToBeErased() > 0) {
      //If there is at least one HW page to erase, loop over ErasePage until
      //all HW pages are erased.  Without this explicit check before
      //ErasePage, when halSimEepromErasePage() returns 0 the code wont
      //know if a page was actually erased or not and properly break out.
      while(halSimEepromErasePage()) {
      }
      // A fresh virtual page now exists to use
      break;
    }
    //If there are still pages to erase, then we have a situation where page
    //rotation is stuck because live tokens still exist in the
    //page we want to erase.  In this case we must do a repair to
    //get all live tokens into one virtual page. [BugzId:14392]
    //This bug pertains to SimEE2.
    //Fall into...
  }
  case EMBER_ERR_FLASH_WRITE_INHIBITED:
  case EMBER_ERR_FLASH_VERIFY_FAILED:
  { //Something went wrong while writing a token.  There is stale data and the
    //token the app expected to write did not get written.  Also there may
    //now be "stray" data written in the flash that could inhibit future token
    //writes.  To deal with stray/stale data, we must repair the Simulated
    //EEPROM.  Because the expected token write failed and will not be retried,
    //it is best to reset the chip and let normal boot sequences take over.
    //Since halInternalSimEeRepair() could potentially result in another write
    //failure, we use a simple semaphore to prevent recursion.
    static bool repairActive = false;
    if(!repairActive) {
      repairActive = true;
      halInternalSimEeRepair(false);
      switch (status) {
      case EMBER_SIM_EEPROM_ERASE_PAGE_RED:
      case EMBER_SIM_EEPROM_FULL:
        //Don't reboot - return to let SimEE code retry the token write
        //[BugzId:14392]
        break;
      case EMBER_ERR_FLASH_VERIFY_FAILED:
        //[[WBB350FIXME -It would be nice to have a unified, ]]
        //[[             platform independant reboot machanism]]
        #if defined (CORTEXM3)
          halInternalSysReset(RESET_FLASH_VERIFY);
        #elif defined (EMBER_STACK_COBRA)
          halInternalSystemReset(RESET_FLASH);
        #else
          assert(0);
        #endif
        break;
      case EMBER_ERR_FLASH_WRITE_INHIBITED:
        #if defined (CORTEXM3)
          halInternalSysReset(RESET_FLASH_INHIBIT);
        #elif defined (EMBER_STACK_COBRA)
          halInternalSystemReset(RESET_FLASH);
        #else
          assert(0);
        #endif
        break;
      default:
        assert(0);
        break;
      }
      repairActive = false;
    }
    break;
  }
  case EMBER_SIM_EEPROM_REPAIRING:
    // While there's nothing for an app to do when the SimEE is going to
    // repair itself (SimEE has to be fully functional for the rest of the
    // system to work), alert the application to the fact that repairing
    // is occuring.  There are debugging scenarios where an app might want
    // to know that repairing is happening; such as monitoring frequency.
    // NOTE:  Common situations will trigger an expected repair, such as
    //        using an erased chip or changing token definitions.
    break;
  default:
    // this condition indicates an unexpected problem.
    assert(0);
    break;
  }
}

#endif//EMBER_APPLICATION_HAS_CUSTOM_SIM_EEPROM_CALLBACK

#ifndef EMBER_APPLICATION_HAS_CUSTOM_ISRS
uint16_t halInternalSc2Isr(uint16_t interrupt, uint16_t pcbContext)
{
  return interrupt;
}
#endif//EMBER_APPLICATION_HAS_CUSTOM_ISRS

#ifndef EMBER_APPLICATION_HAS_CUSTOM_RADIO_POWER_UP_HANDLER
void halRadioPowerUpHandler(void) {}
#endif // EMBER_APPLICATION_HAS_CUSTOM_RADIO_POWER_UP_HANDLER

#ifndef EMBER_APPLICATION_HAS_CUSTOM_RADIO_POWER_DOWN_HANDLER
void halRadioPowerDownHandler(void) {}
#endif // EMBER_APPLICATION_HAS_CUSTOM_RADIO_POWER_DOWN_HANDLER

#endif
