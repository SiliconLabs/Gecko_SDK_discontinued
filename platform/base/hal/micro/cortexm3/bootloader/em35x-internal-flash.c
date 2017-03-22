/*
 * File: em35x-internal-flash.c
 * Description: Driver that supports the internal flash of the EM35x
 *
 * Copyright 2012 by Ember Corporation. All rights reserved.                *80*
 *
 */

/*
 * When EEPROM_USES_SHUTDOWN_CONTROL is defined in the board header, 
 * logic is enabled in the EEPROM driver which drives PB7 high upon EEPROM 
 * initialization.  In Ember reference designs, PB7 acts as an EEPROM enable 
 * pin and therefore must be driven high in order to use the EEPROM.  
 * This option is intended to be enabled when running app-bootloader on 
 * designs based on current Ember reference designs.
 */

#include PLATFORM_HEADER
#include "hal/micro/bootloader-eeprom.h"
#include "bootloader-common.h"
#include "hal/micro/micro.h"
#include "hal/micro/bootloader-interface.h"

#include "stack/include/error.h"
#include "hal/micro/cortexm3/flash.h"
#include "hal/micro/cortexm3/memmap.h"

#define THIS_DRIVER_VERSION (0x010A)

#if BAT_VERSION != THIS_DRIVER_VERSION
  #error Local Storage driver must be updated to support new API requirements
#endif

// Do not allow this driver to be used unless the BOOTLOADER_TYPE is set
// correctly. Otherwise the app code won't know this is a local storage 
// bootloader and that the storage region size fluctuates.
#if BOOTLOADER_TYPE != BL_EXT_TYPE_APP_LOCAL_STORAGE
  #error BOOTLOADER_TYPE must be set to BL_EXT_TYPE_APP_LOCAL_STORAGE when using local storage driver
#endif

// This is a bootloader, but we need BYPASS_MPU() since the write and erase
// functions are called by the application. To prevent this from bloating 
// all bootloaders we copy it here from mpu.h
#define BYPASS_MPU_BOOT(blah)               \
  {                                         \
   uint32_t MPU_CTRL_SAVED = MPU_CTRL_REG;    \
   MPU_CTRL &= ~MPU_CTRL_ENABLE;            \
   _executeBarrierInstructions();           \
   { blah }                                 \
   MPU_CTRL = MPU_CTRL_SAVED;               \
   _executeBarrierInstructions();           \
   }

// MS units are 1024Hz based
#define TIMING_ERASE_PAGE_MAX_MS    (22)
#define WORD_SIZE_BYTES             (2)

static const HalEepromInformationType em35xFlashInfo = {
  EEPROM_INFO_VERSION,
  (EEPROM_CAPABILITIES_ERASE_SUPPORTED | EEPROM_CAPABILITIES_PAGE_ERASE_REQD |
   EEPROM_CAPABILITIES_BLOCKING_ERASE | EEPROM_CAPABILITIES_BLOCKING_WRITE),
  TIMING_ERASE_PAGE_MAX_MS,
  0, // part erase time is unknown
  MFB_PAGE_SIZE_B,
  0, // storage size is not known by the bootloader at compile time
  "EM35x",
  WORD_SIZE_BYTES
};

uint8_t halEepromInit(void)
{
  // Internal flash is ready to go if we're running code
  return EEPROM_SUCCESS;
}


void halEepromShutdown(void)
{
  // Do nothing
}


const HalEepromInformationType *halEepromInfo(void)
{
  // Return the em35x info
  return &em35xFlashInfo;
}


bool halEepromBusy(void)
{
  // If we can run code then the flash must not be busy
  return false;
}


// Return the size of the storage region in bytes. This will be computed
// by first looking at the AAT on the chip and if there is no AAT a sane
// default value will be assumed.
uint32_t halEepromSize(void)
{
  uint32_t overhead, size;

  // If there is a valid AAT then use whatever it sets for the storage size
  if ( (halAppAddressTable.baseTable.type) == APP_ADDRESS_TABLE_TYPE ) {
    if( ((halAppAddressTable.baseTable.version & AAT_MAJOR_VERSION_MASK) == AAT_MAJOR_VERSION) && 
        (halAppAddressTable.baseTable.version >= 0x0108) &&
        ((uint32_t)halAppAddressTable.internalStorageBottom > MFB_BOTTOM) ){
      // We assume that the internal storage region starts at the top of flash
      overhead = 0;
      size = MFB_TOP - (uint32_t)halAppAddressTable.internalStorageBottom + 1;
    } else {
      // Either we can't parse this AAT or the AAT has a version less than 0x0108
      // and can't have a storage size. To be safe in these cases set the storage
      // size to 0 to prevent app corruption.
      overhead = 0;
      size = 0;
    }
  } else {
    // If not then choose a safe value...
    // (Total Memory - Bootloader Size)/2 is the basic equation, but that doesn't
    // include EBL overhead. Overhead is 8 bytes per flash page, 144 bytes
    // for the header and 8 bytes for the end tag. You could construct an EBL with
    // more overhead if desired, but our tools would never do that.
    size = (MFB_SIZE_B - _BOOTRO_REGION_SEGMENT_SIZE)/2;
    overhead = sizeof(eblHdr3xx_t) +
      sizeof(eblProg3xx_t)*(size/MFB_PAGE_SIZE_B) +
      sizeof(eblEnd_t);
    if(overhead & (~MFB_PAGE_MASK_B)) {
      // round the overhead up to the next flash page size
      overhead = (overhead&MFB_PAGE_MASK_B) + MFB_PAGE_SIZE_B;
    }
  }

  return size + overhead;
}


// Convert the EEPROM offset into an absolute flash address. Data is stored
// starting at the last page of flash and working backwards towards the first
// page. The actual values within a page are stored in order.
//  For example, assuming a page size of 0x800...
//    Offset 0x000 --> MFB_TOP - 0x800
//    Offset 0x010 --> MFB_TOP - 0x800 + 0x10
//    Offset 0x801 --> MFB_TOP - 2*0x800 + 0x1
uint32_t flashAddressFromOffset(uint32_t address) 
{
  uint32_t offset = address & (~MFB_PAGE_MASK_B);
  // Always subtract by an extra MFB_PAGE_SIZE_B value since page 0 begins
  // MFB_PAGE_SIZE_B away from the top of flash.
  uint32_t pageStart = (address&MFB_PAGE_MASK_B) + MFB_PAGE_SIZE_B;

  return (MFB_TOP + 1 - pageStart) + offset;
}


static bool verifyAddressRange(uint32_t address, uint16_t length)
{
  uint32_t endAddr = flashAddressFromOffset(address+length);
  uint32_t storageSize = halEepromSize();

  // Make sure both the start and end addresses are within range
  // for the internal storage size specified
  if((address >= storageSize) || 
     ((address + length) > storageSize) ||
     ((address + length) < address)) {
    return false;
  }

  // Extra check to make sure we never overwrite the bootloader
  //  note: assumes the AAT is at the beginning of a flash page
  if(endAddr < (uint32_t)halBootloaderAddressTable.appAddressTable) {
    return false;
  }

  // must be within range
  return true;
}


uint8_t halEepromRead(uint32_t address, uint8_t *data, uint16_t totalLength)
{
  // Verify the address before computing the absolute address
  if( !verifyAddressRange(address, totalLength) )
    return EEPROM_ERR_ADDR;

  // find the actual address in flash for this offset in internal storage
  address = flashAddressFromOffset(address);

  // Read any data up to the next page boundary
  if(address&(~MFB_PAGE_MASK_B)) {
    uint16_t size = MFB_PAGE_SIZE_B - (address&(~MFB_PAGE_MASK_B));
    if(size > totalLength) {
      size = totalLength;
    }
    MEMCOPY(data, (void*)address, size);

    // the address computed is only valid if (totalLength-size) > 0
    address = (address&MFB_PAGE_MASK_B)-MFB_PAGE_SIZE_B; 
    data += size;
    totalLength -= size;
  }

  // Read out full flash pages in a loop
  while(totalLength > MFB_PAGE_SIZE_B) {
    MEMCOPY(data, (void*)address, MFB_PAGE_SIZE_B);
    address -= MFB_PAGE_SIZE_B;
    data += MFB_PAGE_SIZE_B;
    totalLength -= MFB_PAGE_SIZE_B;
  }
  
  // Read out any left over data
  if(totalLength > 0) {
    MEMCOPY(data, (void*)address, totalLength);
  }

  return EEPROM_SUCCESS;
}


// Wrapper function for halInternalFlashWrite that goes atomic and bypasses the
// MPU. Typically this is handled in halInternalFlashWrite, but the bootloader
// doesn't define these macros forcing us to do it ourselves.
uint8_t halInternalFlashWriteWrapper(uint32_t address, uint16_t *data, uint16_t size)
{
  uint8_t res;
  uint8_t savedPriMask;

  // Save the current value of PRIMASK and then go ATOMIC by setting it
  savedPriMask = __get_PRIMASK();
  __set_PRIMASK(1);

  //Bypass MPU
  BYPASS_MPU_BOOT(
    res = halInternalFlashWrite(address, data, size);
    )

  // Restore the saved value of PRIMASK
  __set_PRIMASK(savedPriMask);

  return res;
}


uint8_t halEepromWrite(uint32_t address, const uint8_t *data, uint16_t totalLength)
{
  EmberStatus result = EMBER_SUCCESS;

  // The length and start address must be multiples of the flash word size
  if((totalLength % WORD_SIZE_BYTES) != 0) {
    return EEPROM_ERR_ADDR;
  }
  if((address % WORD_SIZE_BYTES) != 0) {
    return EEPROM_ERR_ADDR;
  }
  // Verify the address before computing the absolute address
  if( !verifyAddressRange(address, totalLength) ) {
    return EEPROM_ERR_ADDR;
  }

  // Offset the write address to the location of internal storage
  address = flashAddressFromOffset(address);

  // Write any start data up to the next flash page boundary
  if(address&(~MFB_PAGE_MASK_B)) {
    uint16_t size = MFB_PAGE_SIZE_B - (address&(~MFB_PAGE_MASK_B));
    if(size > totalLength) {
      size = totalLength;
    }
    result = halInternalFlashWriteWrapper(address, (uint16_t*)data, size/2);
    // the address computed is only valid if (totalLength-size) > 0
    address = (address&MFB_PAGE_MASK_B)-MFB_PAGE_SIZE_B; 
    data += size;
    totalLength -= size;
  }

  // Write any full flash pages in a loop
  if(result == EMBER_SUCCESS) {
    while(totalLength > MFB_PAGE_SIZE_B) {
      result = halInternalFlashWriteWrapper(address, (uint16_t*)data, MFB_PAGE_SIZE_B/2);
      if(result != EMBER_SUCCESS) {
        break;
      }
      address -= MFB_PAGE_SIZE_B;
      data += MFB_PAGE_SIZE_B;
      totalLength -= MFB_PAGE_SIZE_B;
    }
  }

  // Write any left over data
  if((result == EMBER_SUCCESS) && (totalLength > 0)) {
    result = halInternalFlashWriteWrapper(address, (uint16_t*)data, totalLength/2);
  }

  // Convert the result from the flash write routine to a value that
  // makes sense for the EEPROM
  switch(result) {
  case EMBER_SUCCESS:
    return EEPROM_SUCCESS;
  case EMBER_BAD_ARGUMENT:
    return EEPROM_ERR_ADDR;
  case EMBER_ERR_FLASH_PROG_FAIL:
    return EEPROM_ERR_WRT_DATA;
  case EMBER_ERR_FLASH_WRITE_INHIBITED:
    return EEPROM_ERR_ERASE_REQUIRED;
  case EMBER_ERR_FLASH_VERIFY_FAILED:
  default:
    return EEPROM_ERR;
  }
}


// Wrapper function for halInternalFlashErase that goes atomic and bypasses the
// MPU. Typically this is handled in halInternalFlashErase, but the bootloader
// doesn't define these macros forcing us to do it ourselves.
uint8_t halInternalFlashEraseWrapper(uint8_t eraseType, uint32_t address)
{
  uint8_t res;
  uint8_t savedPriMask;

  // Save the current value of PRIMASK and then go ATOMIC by setting it
  savedPriMask = __get_PRIMASK();
  __set_PRIMASK(1);

  //Bypass MPU
  BYPASS_MPU_BOOT(
    res = halInternalFlashErase(eraseType, address);
    )

  // Restore the saved value of PRIMASK
  __set_PRIMASK(savedPriMask);

  return res;
}


uint8_t halEepromErase(uint32_t address, uint32_t totalLength)
{
  EmberStatus result;

  // Make sure the length and start address are multiples of the flash page size
  if(totalLength & (~MFB_PAGE_MASK_B)) {
    return EEPROM_ERR_PG_SZ;
  } 
  // Make sure the address is on the start of a page boundary
  if(address & (~MFB_PAGE_MASK_B)) {
    return EEPROM_ERR_PG_BOUNDARY;
  }
  // Make sure that the totalLength is within the internal storage region
  if( !verifyAddressRange(address, totalLength) ) {
    return EEPROM_ERR_ADDR;
  }

  // Offset the write address to the location of internal storage
  address = flashAddressFromOffset(address);

  // Erase all of the requested pages
  while(totalLength) {
    result = halInternalFlashEraseWrapper(MFB_PAGE_ERASE, address);
    if(result != EMBER_SUCCESS) {
      break;
    }
    address -= MFB_PAGE_SIZE_B;
    totalLength -= MFB_PAGE_SIZE_B;
  }

  // Convert the Ember result into a EEPROM result
  switch(result) {
  case EMBER_SUCCESS:
    return EEPROM_SUCCESS;
  case EMBER_BAD_ARGUMENT:
    return EEPROM_ERR_ADDR;
  case EMBER_ERR_FLASH_PROG_FAIL:
    return EEPROM_ERR_WRT_DATA;
  case EMBER_ERR_FLASH_WRITE_INHIBITED:
  case EMBER_ERR_FLASH_VERIFY_FAILED:
  default:
    return EEPROM_ERR;
  }
}
