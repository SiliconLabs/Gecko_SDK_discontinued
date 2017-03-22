/*
 * File: bootloader-interface-app.c
 * Description: Cortex M3 application bootloader interface.
 *              Provides routines used by applications to access and verify
 *              the bootload image.
 *
 * Copyright 2007-2012 by Ember Corporation. All rights reserved.           *80*
 *
 * Notes:
 */

#include PLATFORM_HEADER
#include "stack/include/ember-types.h"
#include "stack/include/error.h"
#include "hal/hal.h"
#include "hal/micro/cortexm3/common/bootloader-common.h"
#include "hal/micro/cortexm3/common/ebl.h"
#include "hal/micro/cortexm3/memmap.h"

// Common bootloader interface
#include "api/btl_interface.h"

// Default to using storage slot 0
static int32_t storageSlot = 0;

#if defined GECKO_INFO_PAGE_BTL \
    || defined APP_GECKO_INFO_PAGE_BTL \
    || defined STA_GECKO_INFO_PAGE_BTL \
    || defined LOCAL_STORAGE_GECKO_INFO_PAGE_BTL
#define NO_BAT

  static void verifyAppBlVersion(uint16_t version)
  {
    return;
  }

  static bool bootloaderIsCommonBootloader(void)
  {
    return true;
  }

#else

  static void verifyAppBlVersion(uint16_t version)
  {
    assert(    halBootloaderAddressTable.baseTable.type == BOOTLOADER_ADDRESS_TABLE_TYPE
            && BOOTLOADER_BASE_TYPE(halBootloaderAddressTable.bootloaderType) == BL_TYPE_APPLICATION
            && halBootloaderAddressTable.baseTable.version >= version );
  }

  static bool bootloaderIsCommonBootloader(void)
  {
    if (halBootloaderAddressTable.baseTable.type == BOOTLOADER_ADDRESS_TABLE_TYPE) {
      return false;
    } else {
      return true;
    }
  }

#endif

static void verifyMainBootloaderVersion(uint32_t version)
{
  // Assert that the main bootloader table pointer points to main flash or bootloader flash
  assert(   ((uint32_t)mainBootloaderTable & 0xFFFF0000) == 0x0
         || ((uint32_t)mainBootloaderTable & 0xFFFF0000) == 0x0FE10000);
  // Assert that the main bootloader table pointer points inside the bootloader
  assert(((uint32_t)mainBootloaderTable & 0x0000FFFF) < 0x4000);

  assert(mainBootloaderTable->header.version >= version);
}

uint8_t halAppBootloaderInit(void)
{
  if (bootloaderIsCommonBootloader())
  {
    verifyMainBootloaderVersion(0x00000000);

    if (mainBootloaderTable->init() == BOOTLOADER_OK) {
      return EEPROM_SUCCESS;
    } else {
      return EEPROM_ERR_INVALID_CHIP;
    }
  } else {
#ifndef NO_BAT
    // version 0x0109 is the first bootloader built for an Energy Micro part
    verifyAppBlVersion(0x0109);

    return halBootloaderAddressTable.eepromInit();
#else
    return EEPROM_ERR_INVALID_CHIP;
#endif
  }
}

HalEepromInformationType fixedEepromInfo;

const HalEepromInformationType *halAppBootloaderInfo(void)
{
  if (bootloaderIsCommonBootloader())
  {
    if (!(mainBootloaderTable->capabilities & BOOTLOADER_CAPABILITY_STORAGE)) {
      return NULL;
    }
    BootloaderStorageInformation_t info;
    mainBootloaderTable->storage->getInfo(&info);

    fixedEepromInfo.version           = info.info->version;
    fixedEepromInfo.capabilitiesMask  = info.info->capabilitiesMask;
    fixedEepromInfo.pageEraseMs       = info.info->pageEraseMs;
    fixedEepromInfo.partEraseMs       = info.info->partEraseMs;
    fixedEepromInfo.pageSize          = info.info->pageSize;
    fixedEepromInfo.partSize          = info.info->partSize;
    MEMCOPY((void*)&fixedEepromInfo.partDescription,
            (void*)&info.info->partDescription,
            sizeof(fixedEepromInfo.partDescription));
    fixedEepromInfo.wordSizeBytes     = info.info->wordSizeBytes;

    return &fixedEepromInfo;
  } else {
#ifndef NO_BAT
    //verifyAppBlVersion(0x0109);

    // For internal flash bootloaders we need to ask the app for the size of
    // internal storage since the bootloader doesn't know at build time, but only
    // if we have actually set an internal storage bottom in this app. If not, then
    // we return the default struct which has a size of 0.
    if((halBootloaderAddressTable.bootloaderType == BL_EXT_TYPE_APP_LOCAL_STORAGE) &&
       ((uint32_t)halAppAddressTable.internalStorageBottom > MFB_BOTTOM)) {
      HalEepromInformationType *temp = (HalEepromInformationType*)halBootloaderAddressTable.eepromInfo();
      MEMCOPY(&fixedEepromInfo, temp, sizeof(fixedEepromInfo));
      fixedEepromInfo.partSize = (MFB_TOP - (uint32_t)halAppAddressTable.internalStorageBottom + 1);
      return &fixedEepromInfo;
    } else {
      return (HalEepromInformationType *)halBootloaderAddressTable.eepromInfo();
    }
#else
    return NULL;
#endif
  }
}

void halAppBootloaderShutdown(void)
{
  if (bootloaderIsCommonBootloader())
  {
    mainBootloaderTable->deinit();
  } else {
#ifndef NO_BAT
    //verifyAppBlVersion(0x0109);

    halBootloaderAddressTable.eepromShutdown();
#endif
  }
}

EepromStateType eepromState;

#if EEPROM_PAGE_SIZE < EBL_MIN_TAG_SIZE
  #error EEPROM_PAGE_SIZE smaller than EBL_MIN_TAG_SIZE
#endif
static uint8_t buff[EEPROM_PAGE_SIZE];
EblConfigType eblConfig;

// Static state alloc
#define VALIDATION_CONTEXT_SIZE 384
uint8_t bootloaderValidationContext[VALIDATION_CONTEXT_SIZE];

void halAppBootloaderImageIsValidReset(void)
{
  if (bootloaderIsCommonBootloader())
  {
    int32_t ret;

    // The bootloader needs to have storage in order to perform validation
    assert(mainBootloaderTable->capabilities & BOOTLOADER_CAPABILITY_STORAGE);

    ret = mainBootloaderTable->storage->initParseImage(
            storageSlot,
            (BootloaderParserContext_t *)bootloaderValidationContext,
            VALIDATION_CONTEXT_SIZE
          );

    assert(ret == BOOTLOADER_OK);
  } else {
#ifndef NO_BAT
    //verifyAppBlVersion(0x0109);

    eepromState.address = EEPROM_IMAGE_START;
    eepromState.pages = 0;
    eepromState.pageBufFinger = 0;
    eepromState.pageBufLen = 0;
    halBootloaderAddressTable.eblProcessInit(&eblConfig,
                                             &eepromState,
                                             buff,
                                             EBL_MIN_TAG_SIZE,
                                             true);
#endif
  }
}

uint16_t halAppBootloaderImageIsValid(void)
{
  if (bootloaderIsCommonBootloader())
  {
    uint32_t ret;

    if (!(mainBootloaderTable->capabilities & BOOTLOADER_CAPABILITY_STORAGE)) {
      return 0;
    }

    ret = mainBootloaderTable->storage->verifyImage(
            (BootloaderParserContext_t *)bootloaderValidationContext,
            NULL
          );

    if (ret == BOOTLOADER_ERROR_PARSE_CONTINUE) {
      return BL_IMAGE_IS_VALID_CONTINUE;
    } else if (ret == BOOTLOADER_ERROR_PARSE_SUCCESS) {
      // Cannot return number of pages, since we don't know.
      // It's at least 1 page, though.
      return 1;
    } else {
      // Error
      return 0;
    }
  } else {
#ifndef NO_BAT
    BL_Status status;

    //verifyAppBlVersion(0x0109);

    status = halBootloaderAddressTable.eblProcess(halBootloaderAddressTable.eblDataFuncs,
                                                  &eblConfig,
                                                  NULL);
    if(status == BL_EBL_CONTINUE) {
      return BL_IMAGE_IS_VALID_CONTINUE;
    } else if(status == BL_SUCCESS) {
      return eepromState.pages;
    } else {
      // error, return invalid
      return 0;
    }
#else
    return 0;
#endif
  }
}

EmberStatus halAppBootloaderInstallNewImage(void)
{
  if (bootloaderIsCommonBootloader())
  {
    if (!(mainBootloaderTable->capabilities & BOOTLOADER_CAPABILITY_STORAGE)) {
      return EMBER_ERR_FATAL;
    }

    if (mainBootloaderTable->storage->setImagesToBootload(&storageSlot, 1) == BOOTLOADER_OK) {
      // should not return
      halInternalSysReset(RESET_BOOTLOADER_BOOTLOAD);
    }
  } else {
#ifndef NO_BAT
    verifyAppBlVersion(0x0109);

    // should not return
    halInternalSysReset(RESET_BOOTLOADER_BOOTLOAD);
#endif
  }

  return EMBER_ERR_FATAL;
}


uint8_t halAppBootloaderWriteRawStorage(uint32_t address,
                                      const uint8_t *data,
                                      uint16_t len)
{
  if (bootloaderIsCommonBootloader())
  {
    if (mainBootloaderTable->capabilities & BOOTLOADER_CAPABILITY_STORAGE) {
      if (mainBootloaderTable->storage->writeRaw(address, (uint8_t *)data, len) == BOOTLOADER_OK) {
        return EEPROM_SUCCESS;
      }
    }
    return EEPROM_ERR;
  } else {
#ifndef NO_BAT
    //verifyAppBlVersion(0x0109);

    return halBootloaderAddressTable.eepromWrite(address, data, len);
#else
    return EEPROM_ERR;
#endif
  }
}

uint8_t halAppBootloaderReadRawStorage(uint32_t address, uint8_t *data, uint16_t len)
{
  if (bootloaderIsCommonBootloader())
  {
    if (mainBootloaderTable->capabilities & BOOTLOADER_CAPABILITY_STORAGE) {
      if (mainBootloaderTable->storage->readRaw(address, data, len) == BOOTLOADER_OK) {
        return EEPROM_SUCCESS;
      }
    }
    return EEPROM_ERR;
  } else {
#ifndef NO_BAT
    //verifyAppBlVersion(0x0109);

    return halBootloaderAddressTable.eepromRead(address, data, len);
#else
    return EEPROM_ERR;
#endif
  }
}

uint8_t halAppBootloaderEraseRawStorage(uint32_t address, uint32_t len)
{
  if (bootloaderIsCommonBootloader())
  {
    if (mainBootloaderTable->capabilities & BOOTLOADER_CAPABILITY_STORAGE) {
      if (mainBootloaderTable->storage->eraseRaw(address, len) == BOOTLOADER_OK) {
        return EEPROM_SUCCESS;
      }
    }
    return EEPROM_ERR;
  } else {
#ifndef NO_BAT
    //verifyAppBlVersion(0x0109);

    return halBootloaderAddressTable.eepromErase(address, len);
#else
    return EEPROM_ERR;
#endif
  }
}

bool halAppBootloaderStorageBusy(void)
{
  if (bootloaderIsCommonBootloader())
  {
    if (mainBootloaderTable->capabilities & BOOTLOADER_CAPABILITY_STORAGE) {
      return mainBootloaderTable->storage->isBusy();
    } else {
      return true;
    }
  } else {
#ifndef NO_BAT
    //verifyAppBlVersion(0x0109);
    return halBootloaderAddressTable.eepromBusy();
#else
    return true;
#endif
  }
}

// halAppBootloaderGetVersion
//
// Returns the application bootloader version
//
uint16_t halAppBootloaderGetVersion(void)
{
  if (bootloaderIsCommonBootloader()) {
    return mainBootloaderTable->header.version >> 16;
  } else {
#ifndef NO_BAT
    verifyAppBlVersion(0x0109);
    return halBootloaderAddressTable.bootloaderVersion;
#else
    return BOOTLOADER_INVALID_VERSION;
#endif
  }
}

// halAppBootloaderSupportsIbr
//
// Returns whether the bootloader suppoerts IBRs
//
bool halAppBootloaderSupportsIbr(void)
{
  if (bootloaderIsCommonBootloader()) {
    return false;
  } else {
#ifndef NO_BAT
    return  halBootloaderAddressTable.baseTable.type == BOOTLOADER_ADDRESS_TABLE_TYPE
            && BOOTLOADER_BASE_TYPE(halBootloaderAddressTable.bootloaderType) == BL_TYPE_APPLICATION
            && halBootloaderAddressTable.baseTable.version >= BAT_MIN_IBR_VERSION;
#else
    return false;
#endif
  }
}
