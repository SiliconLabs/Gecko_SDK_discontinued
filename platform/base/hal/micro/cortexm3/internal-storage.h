/** @file hal/micro/cortexm3/em35x/internal-storage.h
 * @brief Header for an internal flash storage region on the em35x.
 * This file creates some useful #defines for the internal storage
 * region and do some basic storage size error checking.
 *
 * <!-- Copyright 2012 by Ember Corporation. All rights reserved.        *80*-->
 */

#ifndef __INTERNAL_STORAGE_H_
#define __INTERNAL_STORAGE_H_

#include PLATFORM_HEADER
#include "hal/micro/cortexm3/memmap.h"
#include "hal/micro/cortexm3/bootloader-size.h"

// Loader does not have a fixed SIMEE (or even page) size, which is assumed by
// the following header
#ifndef LOADER
#include "hal/plugin/sim-eeprom/sim-eeprom-size.h"
#endif

// Create some min/max sizes for the storage region to prevent problems
//  The logic here is that we don't want to have this size accidentally set to a value
//  that is so small we can never bootload a new image into it to fix things. Since
//  our standalone bootloader is 8KiB make sure that there is alway 8KiB available in
//  storage AND that storage is never so big that we can't fit an 8KiB app in the MFB.
#define MIN_RECOVERY_IMAGE_SIZE      (8*1024)
#define MIN_INTERNAL_STORAGE_SIZE_B  (MIN_RECOVERY_IMAGE_SIZE)
#define MAX_INTERNAL_STORAGE_SIZE_B  (MFB_SIZE_B-BOOTLOADER_SIZE_B-MIN_RECOVERY_IMAGE_SIZE)

// If we're using a local storage bootloader then attempt to calculate the size
// of internal storage unless it's already been specified.
#if defined(INTERNAL_STORAGE_SIZE_KB)
  // Use the size specified on the command line if it's available
  #define INTERNAL_STORAGE_SIZE_B (INTERNAL_STORAGE_SIZE_KB*1024)
#else
  #if defined(LOCAL_STORAGE_BTL) || defined(LOCAL_STORAGE_GECKO_INFO_PAGE_BTL)
    // The logical storage size is half of your available flash + 1 extra page for
    // any EBL overhead. The available flash is (total flash - simee - bootloader).
    #define INTERNAL_STORAGE_SIZE_B ((MFB_SIZE_B-SIMEE_SIZE_B-BOOTLOADER_SIZE_B)/2 + MFB_PAGE_SIZE_B)
  #else
    #define INTERNAL_STORAGE_SIZE_B (0)
  #endif
#endif

// If we're using an internal storage region then add some error checking of the
// specified size since there are invalid values
#if INTERNAL_STORAGE_SIZE_B > 0
  // Check to make sure that the internal storage region size is a multiple of the
  // flash page size. If this was not true we wouldn't be able to erase storage
  // pages independently from regular flash
  #if (INTERNAL_STORAGE_SIZE_B&(~MFB_PAGE_MASK_B)) != 0
    #error INTERNAL_STORAGE_SIZE_KB must be a multiple of the flash page size!
  #endif
   
  // Enforce the min and max internal storage sizes defined above
  #if INTERNAL_STORAGE_SIZE_B < MIN_INTERNAL_STORAGE_SIZE_B
    #error INTERNAL_STORAGE_SIZE_KB is smaller than the minimum
  #endif
  #if INTERNAL_STORAGE_SIZE_B > MAX_INTERNAL_STORAGE_SIZE_B
    #error INTERNAL_STORAGE_SIZE_KB is larger than the maximum
  #endif
#endif

#endif //__INTERNAL_STORAGE_H_
