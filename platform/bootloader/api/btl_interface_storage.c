/**
 * @file btl_interface_storage.c
 * @brief Application interface to the storage plugin of the bootloader.
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

#include "btl_interface.h"

// Make assert no-op if not configured
#ifndef BTL_ASSERT
#define BTL_ASSERT(x)
#endif

/// Context size for bootloader verification context
#define BTL_STORAGE_VERIFICATION_CONTEXT_SIZE   (384)

// -----------------------------------------------------------------------------
// Functions

void bootloader_getStorageInfo(BootloaderStorageInformation_t *info)
{
  if (!BTL_TABLE_PTR_VALID(mainBootloaderTable)) {
    return;
  }
  mainBootloaderTable->storage->getInfo(info);
}

int32_t bootloader_getStorageSlotInfo(uint32_t slotId,
                                   BootloaderStorageSlot_t *slot)
{
  if (!BTL_TABLE_PTR_VALID(mainBootloaderTable)) {
    return BOOTLOADER_ERROR_INIT_TABLE;
  }
  return mainBootloaderTable->storage->getSlotInfo(slotId, slot);
}

int32_t bootloader_readStorage(uint32_t slotId,
                            uint32_t offset,
                            uint8_t *buffer,
                            size_t length)
{
  if (!BTL_TABLE_PTR_VALID(mainBootloaderTable)) {
    return BOOTLOADER_ERROR_INIT_TABLE;
  }
  return mainBootloaderTable->storage->read(slotId, offset, buffer, length);
}


int32_t bootloader_writeStorage(uint32_t slotId,
                             uint32_t offset,
                             uint8_t *buffer,
                             size_t length)
{
  if (!BTL_TABLE_PTR_VALID(mainBootloaderTable)) {
    return BOOTLOADER_ERROR_INIT_TABLE;
  }
  return mainBootloaderTable->storage->write(slotId, offset, buffer, length);
}

int32_t bootloader_eraseStorageSlot(uint32_t slotId)
{
  if (!BTL_TABLE_PTR_VALID(mainBootloaderTable)) {
    return BOOTLOADER_ERROR_INIT_TABLE;
  }
  return mainBootloaderTable->storage->erase(slotId);
}

int32_t bootloader_setImageToBootload(int32_t slotId)
{
  if (!BTL_TABLE_PTR_VALID(mainBootloaderTable)) {
    return BOOTLOADER_ERROR_INIT_TABLE;
  }
  return mainBootloaderTable->storage->setImagesToBootload(&slotId, 1);
}

int32_t bootloader_setImagesToBootload(int32_t *slotIds, size_t length)
{
  if (!BTL_TABLE_PTR_VALID(mainBootloaderTable)) {
    return BOOTLOADER_ERROR_INIT_TABLE;
  }
  return mainBootloaderTable->storage->setImagesToBootload(slotIds, length);
}

int32_t bootloader_getImagesToBootload(int32_t *slotIds, size_t length)
{
  if (!BTL_TABLE_PTR_VALID(mainBootloaderTable)) {
    return BOOTLOADER_ERROR_INIT_TABLE;
  }
  return mainBootloaderTable->storage->getImagesToBootload(slotIds, length);
}

int32_t bootloader_appendImageToBootloadList(int32_t slotId)
{
  if (!BTL_TABLE_PTR_VALID(mainBootloaderTable)) {
    return BOOTLOADER_ERROR_INIT_TABLE;
  }
  return mainBootloaderTable->storage->appendImageToBootloadList(slotId);
}

int32_t bootloader_initVerifyImage(uint32_t slotId, void *context, size_t contextSize)
{
  int32_t retval;
  if (!BTL_TABLE_PTR_VALID(mainBootloaderTable)) {
    return BOOTLOADER_ERROR_PARSE_STORAGE;
  }

  // Check that the bootloader has image verification capability
  if (mainBootloaderTable->storage == NULL) {
    return BOOTLOADER_ERROR_PARSE_STORAGE;
  }

  retval = mainBootloaderTable->storage->initParseImage(slotId,
                                            (BootloaderParserContext_t *)context,
                                            contextSize);

  return retval;
}

int32_t bootloader_continueVerifyImage(void *context, BootloaderParserCallback_t metadataCallback)
{
  if (!BTL_TABLE_PTR_VALID(mainBootloaderTable)) {
    return BOOTLOADER_ERROR_PARSE_STORAGE;
  }

  return mainBootloaderTable->storage->verifyImage((BootloaderParserContext_t *)context,
                                                   metadataCallback);
}

int32_t bootloader_verifyImage(uint32_t slotId, BootloaderParserCallback_t metadataCallback)
{
  uint8_t context[BTL_STORAGE_VERIFICATION_CONTEXT_SIZE];
  int32_t retval;

  if (!BTL_TABLE_PTR_VALID(mainBootloaderTable)) {
    return BOOTLOADER_ERROR_PARSE_STORAGE;
  }

  retval = bootloader_initVerifyImage(slotId, context, BTL_STORAGE_VERIFICATION_CONTEXT_SIZE);

  if (retval != BOOTLOADER_OK) {
    return retval;
  }

  do {
    retval = bootloader_continueVerifyImage(context, metadataCallback);
  } while (retval == BOOTLOADER_ERROR_PARSE_CONTINUE);

  if (retval == BOOTLOADER_ERROR_PARSE_SUCCESS) {
    return BOOTLOADER_OK;
  } else {
    return retval;
  }
}

int32_t bootloader_getImageInfo(uint32_t slotId,
                                ApplicationData_t *appInfo,
                                uint32_t *bootloaderVersion)
{
  uint8_t context[BTL_STORAGE_VERIFICATION_CONTEXT_SIZE];
  int32_t retval;

  if (!BTL_TABLE_PTR_VALID(mainBootloaderTable)) {
    return BOOTLOADER_ERROR_PARSE_STORAGE;
  }

  // Check that the bootloader has image verification capability
  BTL_ASSERT(mainBootloaderTable->storage != NULL);

  retval = mainBootloaderTable->storage->initParseImage(slotId,
                                            (BootloaderParserContext_t *)context,
                                            BTL_STORAGE_VERIFICATION_CONTEXT_SIZE);
  if (retval != BOOTLOADER_OK) {
    return retval;
  }

  retval = mainBootloaderTable->storage->getImageInfo((BootloaderParserContext_t *)context,
                                                      appInfo,
                                                      bootloaderVersion);

  return retval;
}

bool bootloader_storageIsBusy(void)
{
  if (!BTL_TABLE_PTR_VALID(mainBootloaderTable)) {
    return true;
  }
  return mainBootloaderTable->storage->isBusy();
}

int32_t bootloader_readRawStorage(uint32_t address,
                               uint8_t *buffer,
                               size_t length)
{
  if (!BTL_TABLE_PTR_VALID(mainBootloaderTable)) {
    return BOOTLOADER_ERROR_INIT_STORAGE;
  }
  return mainBootloaderTable->storage->readRaw(address, buffer, length);
}

int32_t bootloader_writeRawStorage(uint32_t address,
                                uint8_t *buffer,
                                size_t length)
{
  if (!BTL_TABLE_PTR_VALID(mainBootloaderTable)) {
    return BOOTLOADER_ERROR_INIT_STORAGE;
  }
  return mainBootloaderTable->storage->writeRaw(address, buffer, length);
}

int32_t bootloader_eraseRawStorage(uint32_t address,
                                size_t length)
{
  if (!BTL_TABLE_PTR_VALID(mainBootloaderTable)) {
    return BOOTLOADER_ERROR_INIT_STORAGE;
  }
  return mainBootloaderTable->storage->eraseRaw(address, length);
}
