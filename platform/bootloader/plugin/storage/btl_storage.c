/***************************************************************************//**
 * @file btl_storage.c
 * @brief Storage plugin for the Gecko Bootloader.
 * @author Silicon Labs
 * @version 1.0.0
 *******************************************************************************
 * @section License
 * <b>Copyright 2016 Silicon Laboratories, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/
#include "config/btl_config.h"

#include "api/btl_interface.h"
#include "btl_storage.h"

#include "core/btl_reset.h"
#include "core/btl_parse.h"
#include "plugin/debug/btl_debug.h"

uint32_t storage_getBaseAddress(void)
{
#ifdef BTL_STORAGE_BASE_ADDRESS
  return BTL_STORAGE_BASE_ADDRESS;
#else
  return 0;
#endif
}

int32_t storage_main(void)
{
  BootloaderParserContext_t parseContext;
  int32_t ret;
  int32_t slotIds[BTL_STORAGE_BOOTLOAD_LIST_LENGTH] = {-1};
  bool bootloadSuccess = false;

  ret = storage_getBootloadList(slotIds, BTL_STORAGE_BOOTLOAD_LIST_LENGTH);
  if (ret != BOOTLOADER_OK) {
    BTL_DEBUG_PRINTLN("BI err");
    return ret;
  }

  // Attempt to bootload given images in sequence
  for (size_t id = 0; id < BTL_STORAGE_BOOTLOAD_LIST_LENGTH; id++) {
    int32_t slotId = slotIds[id];
    BTL_DEBUG_PRINT("Slot: ");
    BTL_DEBUG_PRINT_WORD_HEX(slotId);
    BTL_DEBUG_PRINT_LF();

    if(slotId == -1) {
      // Invalid slot ID; try the next one
      continue;
    }

    // Get info about the image marked for bootloading
    storage_initParseSlot(slotId,
                          &parseContext,
                          sizeof(BootloaderParserContext_t));

    ret = BOOTLOADER_ERROR_PARSE_CONTINUE;
    do {
      ret = storage_verifySlot(&parseContext, NULL);
    } while (ret == BOOTLOADER_ERROR_PARSE_CONTINUE);

    if (ret != BOOTLOADER_ERROR_PARSE_SUCCESS) {
      // Image in slot is non-existant or corrupt.
      // Continue to next image
      BTL_DEBUG_PRINTLN("Verify fail");
      bootloadSuccess = false;
      continue;
    }

    if (parseContext.imageProperties.imageContainsBootloader
        && !parseContext.imageProperties.imageContainsApplication) {
      BTL_DEBUG_PRINTLN("BL upg with no app");
      bootloadSuccess = false;
      continue;
    }

    if (parseContext.imageProperties.imageContainsBootloader) {
      BTL_DEBUG_PRINT("BL upg ");
      BTL_DEBUG_PRINT_WORD_HEX(mainBootloaderTable->header.version);
      BTL_DEBUG_PRINT(" -> ");
      BTL_DEBUG_PRINT_WORD_HEX(parseContext.imageProperties.bootloaderVersion);
      BTL_DEBUG_PRINT_LF();
    }

    if (parseContext.imageProperties.imageContainsBootloader
        && (parseContext.imageProperties.bootloaderVersion
            > mainBootloaderTable->header.version)) {
      // This is a bootloader upgrade, and we also have an application
      // available for after the bootloader upgrade is complete
      if(!storage_bootloadBootloaderFromSlot(parseContext.slotId,
                                             parseContext.imageProperties.bootloaderVersion)) {
        // Bootloader upgrade failed; not a valid image
        BTL_DEBUG_PRINTLN("Btl bootload fail");
        // Continue to next image
        bootloadSuccess = false;
        continue;
      } else {
        // Bootloader successfully written to internal flash; reboot and apply
        reset_resetWithReason(BOOTLOADER_RESET_REASON_UPGRADE);
      }
    } else {
      // This should be an application upgrade
      if(!parseContext.imageProperties.imageContainsApplication) {
        // ...but there is no app in the EBL
        BTL_DEBUG_PRINTLN("No app in slot");
        // Continue to next image
        bootloadSuccess = false;
        continue;
      } else if (!storage_bootloadApplicationFromSlot(parseContext.slotId,
                                                      parseContext.imageProperties.application.version)) {
        // App upgrade failed.
        BTL_DEBUG_PRINTLN("App bootload fail");
        // Continue to next image
        bootloadSuccess = false;
        continue;
      } else {
        // Application was updated. Reboot into new image.
        bootloadSuccess = true;
        break;
      }
    }
  }

  if (bootloadSuccess) {
    return BOOTLOADER_OK;
  } else {
    return BOOTLOADER_ERROR_STORAGE_BOOTLOAD;
  }
}
