/*
 * File: bootloader-interface-standalone.c
 * Description: EM3XX-specific standalone bootloader HAL functions
 *
 * Copyright 2008 by Ember Corporation. All rights reserved.                *80*
 */

#include PLATFORM_HEADER
#include "stack/include/ember-types.h"
#include "stack/include/error.h"
#include "stack/include/stack-info.h"
#include "hal/hal.h"
#include "hal/micro/cortexm3/memmap.h"
#include "hal/micro/cortexm3/diagnostic.h"

extern uint8_t emGetPhyRadioChannel(void);
extern int8_t emGetPhyRadioPower(void);

EmberStatus halLaunchStandaloneBootloader(uint8_t mode)
{
  if(BOOTLOADER_BASE_TYPE(halBootloaderAddressTable.bootloaderType) 
     == BL_TYPE_STANDALONE) {
    // should never return
    if(mode == STANDALONE_BOOTLOADER_NORMAL_MODE) {
      // Version 0x0106 is where OTA bootloader support was added and the 
      //  RESET_BOOTLOADER_OTAVALID reset type was introduced.
      if(halBootloaderAddressTable.baseTable.version < 0x0106) {
        halInternalSysReset(RESET_BOOTLOADER_BOOTLOAD);
      } else {
        // Convert channel number to index (bootloader only uses index).
        uint8_t channelIndex =  emGetPhyRadioChannel() - 11;
       #if     (defined(PHY_EM250) || defined(PHY_EM3XX) \
               || (defined(PHY_DUAL) && defined(CORTEXM3_EMBER_MICRO)))
        tokTypeStackCalData calData;

        halCommonGetIndexedToken(&calData, TOKEN_STACK_CAL_DATA, channelIndex);
        halResetInfo.boot.radioLnaCal  = calData.lna;
       #endif//(defined(PHY_EM250) || defined(PHY_EM3XX))

        halResetInfo.boot.panId        = PAN_ID_REG;
        halResetInfo.boot.radioChannel = channelIndex;
        halResetInfo.boot.radioPower   = emGetPhyRadioPower();
        // ota parameters valid bootloader reset
        halInternalSysReset(RESET_BOOTLOADER_OTAVALID);
      }
    } else if(mode == STANDALONE_BOOTLOADER_RECOVERY_MODE) {
      // standard bootloader reset
      halInternalSysReset(RESET_BOOTLOADER_BOOTLOAD);
    }
  }
  return EMBER_ERR_FATAL;
}

uint16_t halGetStandaloneBootloaderVersion(void)
{
  if(BOOTLOADER_BASE_TYPE(halBootloaderAddressTable.bootloaderType) 
     == BL_TYPE_STANDALONE) {
    return halGetBootloaderVersion();
  } else {
    return BOOTLOADER_INVALID_VERSION;
  }
}
