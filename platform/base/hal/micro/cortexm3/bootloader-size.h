/** @file hal/micro/cortexm3/bootloader-size.h
 * @brief File to create defines for the size of the bootloader
 *
 * <!-- Copyright 2009-2012 by Ember Corporation.        All rights reserved.-->
 */
#ifndef __BOOTLOADER_SIZE_H__
#define __BOOTLOADER_SIZE_H__

#include "hal/micro/cortexm3/memmap.h"

// Create the BOOTLOADER_SIZE_B define
#if defined(NULL_BTL) \
    || defined(GECKO_INFO_PAGE_BTL) \
    || defined(APP_GECKO_INFO_PAGE_BTL) \
    || defined(STA_GECKO_INFO_PAGE_BTL) \
    || defined(LOCAL_STORAGE_GECKO_INFO_PAGE_BTL)
  #define BOOTLOADER_SIZE_B (0)
#elif defined(BOOTLOADER_16K)
  #define BOOTLOADER_SIZE_B (16*1024)
#elif defined(BOOTLOADER_10K)
  #define BOOTLOADER_SIZE_B (10*1024)
#elif defined(BOOTLOADER_8K)
  #define BOOTLOADER_SIZE_B (8*1024)
#elif defined(BOOTLOADER_4K)
  #define BOOTLOADER_SIZE_B (4*1024)
#else
  // Choose whatever the default is for this chip
  #define BOOTLOADER_SIZE_B (DEFAULT_BTL_SIZE_B)
#endif

#endif //__BOOTLOADER_SIZE_H__
