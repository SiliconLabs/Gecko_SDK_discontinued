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

#include "api/btl_interface.h"

extern uint8_t emGetPhyRadioChannel(void);
extern int8_t emGetPhyRadioPower(void);

#if defined GECKO_INFO_PAGE_BTL \
    || defined APP_GECKO_INFO_PAGE_BTL \
    || defined STA_GECKO_INFO_PAGE_BTL \
    || defined LOCAL_STORAGE_GECKO_INFO_PAGE_BTL
#define NO_BAT

  static bool bootloaderIsCommonBootloader(void)
  {
    return true;
  }

#else

  static bool bootloaderIsCommonBootloader(void)
  {
    if (halBootloaderAddressTable.baseTable.type == BOOTLOADER_ADDRESS_TABLE_TYPE) {
      return false;
    } else {
      return true;
    }
  }

#endif

EmberStatus halLaunchStandaloneBootloader(uint8_t mode)
{
  if (bootloaderIsCommonBootloader())
  {
    if (!(mainBootloaderTable->capabilities & BOOTLOADER_CAPABILITY_COMMUNICATION)) {
      return EMBER_ERR_FATAL;
    }
  } else {
#ifndef NO_BAT
    if(BOOTLOADER_BASE_TYPE(halBootloaderAddressTable.bootloaderType)
       != BL_TYPE_STANDALONE) {
      return EMBER_ERR_FATAL;
    }
#else
    return EMBER_ERR_FATAL;
#endif
  }
  if(   (mode == STANDALONE_BOOTLOADER_NORMAL_MODE)
     || (mode == STANDALONE_BOOTLOADER_RECOVERY_MODE)) {
    // should never return
    // standard bootloader reset
    halInternalSysReset(RESET_BOOTLOADER_BOOTLOAD);
  }

  return EMBER_ERR_FATAL;
}

uint16_t halGetStandaloneBootloaderVersion(void)
{
  if (bootloaderIsCommonBootloader()) {
    return mainBootloaderTable->header.version >> 16;
  } else {
#ifndef NO_BAT
    if(BOOTLOADER_BASE_TYPE(halBootloaderAddressTable.bootloaderType)
       == BL_TYPE_STANDALONE) {
      return halGetBootloaderVersion();
    } else {
      return BOOTLOADER_INVALID_VERSION;
    }
#else
    return BOOTLOADER_INVALID_VERSION;
#endif
  }
}
