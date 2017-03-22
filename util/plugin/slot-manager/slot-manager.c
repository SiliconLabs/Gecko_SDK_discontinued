/**************************************************************************//**
 * Copyright 2017 Silicon Laboratories, Inc.
 *
 *****************************************************************************/

#include PLATFORM_HEADER
#include "slot-manager.h"

#ifndef SLOT_MANAGER_NUM_SLOTS
 #define SLOT_MANAGER_NUM_SLOTS 8
#endif

static bool     gBootloaderInitialized = false;
static uint32_t gBootloaderNumSlots;
static uint8_t  gVerificationContext[SLOT_MANAGER_VERIFICATION_CONTEXT_SIZE];
static int32_t  gSlotsToBoot[SLOT_MANAGER_NUM_SLOTS];

// Prototypes
uint8_t setSlotToBootload(uint32_t slotId);

// App framework init callback
void emberAfPluginSlotManagerInitCallback(void)
{
  BootloaderStorageInformation_t storageInfo;
  int32_t rv = bootloader_init();

  if (BOOTLOADER_OK == rv) {
    gBootloaderInitialized = true;

    bootloader_getStorageInfo(&storageInfo);
    gBootloaderNumSlots = storageInfo.numStorageSlots;
  } else {
    slotManagerPrintln("Slot Manager: failed to initialize bootloader (error "
                       "0x%4x)", rv);
  }
}

void emberAfPluginSlotManagerPrintExternalFlashInfo(void)
{
  BootloaderStorageInformation_t storageInfo;

  if (!gBootloaderInitialized) {
    return;
  }

  bootloader_getStorageInfo(&storageInfo);
  slotManagerPrintln("Version     : %d",   storageInfo.info->version);
  slotManagerPrintln("Part        : %s",   storageInfo.info->partDescription);
  slotManagerPrintln("Capabilities: 0x%x", storageInfo.info->capabilitiesMask);
  slotManagerPrintln("Part size   : %d B", storageInfo.info->partSize);
  slotManagerPrintln("Page size   : %d B", storageInfo.info->pageSize);
  slotManagerPrintln("Word size   : %d B", storageInfo.info->wordSizeBytes);
  slotManagerPrintln("Slots       : %d",   gBootloaderNumSlots);
}

uint8_t emberAfPluginSlotManagerReadExtFlash(uint32_t address,
                                             uint8_t* data,
                                             uint8_t len)
{
  int32_t rv;

  if (!data || (0 == len)) {
    slotManagerPrintln("Slot Manager: cannot read from external flash with "
                       "buffer (%s) and length (%d)",
                       data ? "valid" : "NULL",
                       len);
    return SLOT_MANAGER_BAD_ARG;
  }

  if (!gBootloaderInitialized) {
    return SLOT_MANAGER_INVALID_CALL;
  }

  rv = bootloader_readRawStorage(address, data, len);
  if (BOOTLOADER_OK != rv) {
    slotManagerPrintln("Slot Manager: failed to read from external flash "
                       "(error %d)", rv);
    return SLOT_MANAGER_ERROR;
  }

  return SLOT_MANAGER_SUCCESS;
}

uint8_t emberAfPluginSlotManagerVerifyAndBootloadSlot(uint32_t slotId)
{
  uint8_t status = SLOT_MANAGER_SUCCESS;
  int32_t rv;

  if (!gBootloaderInitialized) {
    return SLOT_MANAGER_INVALID_CALL;
  }

  if (slotId >= gBootloaderNumSlots) {
    slotManagerPrintln("Slot Manager: cannot bootload out-of-range slot %d (max"
                       " slot %d)", slotId, gBootloaderNumSlots-1);
    return SLOT_MANAGER_BAD_ARG;
  }

  // Verify the image
  slotManagerPrint("Slot Manager: beginning image verification...");

  pokeStackOrWatchDog();
  rv = bootloader_initVerifyImage(slotId,
                                  (void*)gVerificationContext,
                                  SLOT_MANAGER_VERIFICATION_CONTEXT_SIZE);
  pokeStackOrWatchDog();

  while (rv == BOOTLOADER_OK) {
    pokeStackOrWatchDog();
    rv = bootloader_continueVerifyImage((void*)gVerificationContext, NULL);
    if (rv == BOOTLOADER_ERROR_PARSE_SUCCESS) {
      break;
    } else if (rv == BOOTLOADER_ERROR_PARSE_CONTINUE) {
      slotManagerPrint(".");
      rv = BOOTLOADER_OK;
    }
  }

  // Bootload the image if it was verified with success
  if (rv != BOOTLOADER_ERROR_PARSE_SUCCESS) {
    slotManagerPrintln(".failed! (Error 0x%4x)", rv);
    status = SLOT_MANAGER_ERROR;
  } else {
    slotManagerPrintln(".passed! Booting slot %d", slotId);
    rv = setSlotToBootload(slotId);
  }

  slotManagerPrintln("");

  return status;
}

uint8_t emberAfPluginSlotManagerGetSlotInfo(uint32_t slotId,
                                            SlotManagerSlotInfo_t* slotInfo)
{
  int32_t  rv;
  uint32_t btlVersion;

  if (!gBootloaderInitialized) {
    return SLOT_MANAGER_INVALID_CALL;
  }

  if (slotId >= gBootloaderNumSlots) {
    return SLOT_MANAGER_BAD_ARG;
  }

  if (!slotInfo) {
    return SLOT_MANAGER_BAD_ARG;
  }

  MEMSET(slotInfo, 0, sizeof(*slotInfo));

  rv = bootloader_getStorageSlotInfo(slotId, &(slotInfo->slotStorageInfo));
  if (BOOTLOADER_OK != rv) {
    return SLOT_MANAGER_ERROR;
  }

  rv = bootloader_getImageInfo(slotId, &(slotInfo->slotAppInfo), &btlVersion);

  return (BOOTLOADER_OK == rv) ? SLOT_MANAGER_SUCCESS : SLOT_MANAGER_ERROR;
}

uint8_t emberAfPluginSlotManagerWriteToSlot(uint32_t slotId,
                                            uint32_t offset,
                                            uint8_t* buffer,
                                            size_t length)
{
  int32_t rv;
  BootloaderStorageSlot_t storageSlot;

  if (!gBootloaderInitialized) {
    return SLOT_MANAGER_INVALID_CALL;
  }

  if (slotId >= gBootloaderNumSlots) {
    slotManagerPrintln("Slot Manager: cannot write to out-of-range slot %d (max"
                       " slot %d)", slotId, gBootloaderNumSlots-1);
    return SLOT_MANAGER_BAD_ARG;
  }

  if (!buffer || (0 == length)) {
    slotManagerPrintln("Slot Manager: cannot write to slot with buffer (%s) and"
                       " length (%d)",
                       buffer ? "valid" : "NULL",
                       length);
    return SLOT_MANAGER_BAD_ARG;
  }

  rv = bootloader_getStorageSlotInfo(slotId, &storageSlot);
  if (BOOTLOADER_OK != rv) {
    slotManagerPrintln("Slot Manager: failed to get info for slot %d", slotId);
    return SLOT_MANAGER_BAD_ARG;
  }

  // Report an error if too many bytes are being written into the slot
  if ((offset + length) > storageSlot.length) {
    slotManagerPrintln("Slot Manager: slot write failure. Address offset (%d) +"
                       " write length (%d) greater than total slot length (%d)",
                       offset, length, storageSlot.length);
    return SLOT_MANAGER_BAD_ARG;
  }

  rv = bootloader_writeStorage(slotId, offset, buffer, length);

  return (BOOTLOADER_OK == rv) ? SLOT_MANAGER_SUCCESS : SLOT_MANAGER_ERROR;
}

uint8_t emberAfPluginSlotManagerEraseSlot(uint32_t slotId)
{
  int32_t rv = BOOTLOADER_OK;
  BootloaderStorageInformation_t storageInfo;
  BootloaderStorageSlot_t storageSlotInfo;
  uint32_t bytesToErase;
  uint32_t address;

  if (!gBootloaderInitialized) {
    return SLOT_MANAGER_INVALID_CALL;
  }

  if (slotId >= gBootloaderNumSlots) {
    slotManagerPrintln("Slot Manager: cannot erase to out-of-range slot %d (max"
                       " slot %d)", slotId, gBootloaderNumSlots-1);
    return SLOT_MANAGER_BAD_ARG;
  }

  bootloader_getStorageInfo(&storageInfo);

  rv = bootloader_getStorageSlotInfo(slotId, &storageSlotInfo);

  if (BOOTLOADER_OK != rv) {
    return SLOT_MANAGER_ERROR;
  }

  bytesToErase = (storageSlotInfo.length / storageInfo.info->pageSize) *
                  storageInfo.info->pageSize;

  // Check for a misaligned slot
  // This shouldn't happen unless the user configures something improperly, and
  // even then, the bootloader may complain when being compiled/run
  if (storageSlotInfo.length % storageInfo.info->pageSize) {
    slotManagerPrintln("Slot Manager warning: slot %d length (%d) not aligned "
                       "to page size (%d). The entire slot will not be erased.",
                       slotId,
                       storageSlotInfo.length,
                       storageInfo.info->pageSize);
  }

  address = storageSlotInfo.address;

  // Erase the slot in page chunks
  while ((BOOTLOADER_OK == rv) &&
         ((address - storageSlotInfo.address) < bytesToErase)) {
    pokeStackOrWatchDog();
    rv = bootloader_eraseRawStorage(address, storageInfo.info->pageSize);
    address += storageInfo.info->pageSize;
  }

  if (BOOTLOADER_OK != rv) {
    slotManagerPrintln("Slot Manager: failed to erase %d bytes in slot %d at "
                       "address 0x%4x (error 0x%x)",
                       storageInfo.info->pageSize,
                       slotId,
                       address - storageInfo.info->pageSize,
                       rv);
  } else {
    slotManagerPrintln("Slot Manager: erased %d bytes in slot %d",
                       bytesToErase, slotId);
  }

  return (BOOTLOADER_OK == rv) ? SLOT_MANAGER_SUCCESS : SLOT_MANAGER_ERROR;
}

uint8_t setSlotToBootload(uint32_t slotId)
{
  int32_t  rv;
  uint32_t index;
  uint32_t slotsToPush = gBootloaderNumSlots;

  // First, fetch the ordered list of slots to bootload
  rv = bootloader_getImagesToBootload(gSlotsToBoot, gBootloaderNumSlots);
  if (BOOTLOADER_OK != rv) {
    // If this fails, it means there is no bootload info metadata in flash
    // This can happen for a number of reasons (e.g. user didn't flash metadata
    // to right address, metadata is corrupt, etc)
    // Simply writing the slot-to-boot will fix this condition
    rv = bootloader_setImageToBootload(slotId);
    if (BOOTLOADER_OK != rv) {
      slotManagerPrintln("Slot Manager: failed to set slot to boot (error "
                         "0x%4x)", rv);
    } else {
      bootloader_rebootAndInstall();
    }
    return (BOOTLOADER_OK == rv) ? SLOT_MANAGER_SUCCESS : SLOT_MANAGER_ERROR;
  }

  // Second, search if we are already in the slots-to-boot
  for (index = 0; index < gBootloaderNumSlots; index++) {
    if (slotId == gSlotsToBoot[index]) {
      slotsToPush = index;
      break;
    }
  }

  // Third, push all the slots over
  // If we weren't in the slots-to-boot list, then we push all elements over one
  if (slotsToPush == gBootloaderNumSlots) {
    slotsToPush--;
  }

  if (slotsToPush > 0) {
    for (index = slotsToPush; index > 0; index--) {
      gSlotsToBoot[index] = gSlotsToBoot[index-1];
    }
  }
  gSlotsToBoot[0] = slotId;

  // Fourth, set the new slots-to-boot order through the bootloader
  rv = bootloader_setImagesToBootload(gSlotsToBoot, gBootloaderNumSlots);
  if (BOOTLOADER_OK != rv) {
    slotManagerPrintln("Slot Manager: failed to set list of slots to boot "
                       "(error 0x%4x)", rv);
    return SLOT_MANAGER_ERROR;
  }

  rv = bootloader_getImagesToBootload(gSlotsToBoot, gBootloaderNumSlots);
  if (BOOTLOADER_OK != rv) {
    slotManagerPrintln("Slot Manager: failed to get new list of slots to boot "
                       "(error 0x%4x)", rv);
    return SLOT_MANAGER_ERROR;
  }

  // Finally, boot the new list of slots to boot. This call won't return.
  bootloader_rebootAndInstall();

  return SLOT_MANAGER_SUCCESS;
}
