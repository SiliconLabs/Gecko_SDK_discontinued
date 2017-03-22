/** @file hal/plugin/sim-eeprom/sim-eeprom-size.h
 * @brief File to create defines for the size of the sim-eeprom
 *
 * <!-- Copyright 2014 Silicon Laboratories, Inc.                        *80*-->
 */
#ifndef __SIM_EEPROM_SIZE_H__
#define __SIM_EEPROM_SIZE_H__

#include "hal/micro/cortexm3/memmap.h"
#include "hal/micro/micro-types.h"

#ifndef DOXYGEN_SHOULD_SKIP_THIS

//The default choice is an 8kByte SimEE1.  Defining
//EMBER_SIM_EEPROM_4KB in your build or configuration header
//will still use SimEE1, but consume 4kB of flash for the SimEE.
//Both choices of 8kByte and 4kByte of SimEE size allow for a maximum of
//2kByte of data+mgmt.

#if defined(EMBER_SIMEE1_2KB) || defined(EMBER_SIM_EEPROM_2KB)
  #define SIMEE_SIZE_B        2048  //Use a 2k 8bit SimEE1
  #define SIMEE_BTS_SIZE_B    1024
#elif defined(EMBER_SIMEE1_4KB) || defined(EMBER_SIM_EEPROM_4KB)
  #define SIMEE_SIZE_B        4096  //Use a 4k 8bit SimEE1
  #define SIMEE_BTS_SIZE_B    2048
#elif defined(USE_SIMEE2)
  //NOTE: USE_SIMEE2 defaults to 36k
  #if MFB_SIZE_B < 0x30000 && !defined(SIM_EEPROM_TEST)
    #error Using USE_SIMEE2 is only valid on EM357, EM358x, or EM359x chips!
  #endif
  //Defining USE_SIMEE2 in your build or configuration header
  //will consume 36kB of flash for the SimEE and allows for a maximum
  //of 8kByte of data+mgmt.
  #define SIMEE_SIZE_B        36864 //Use a 36k 8bit SimEE2
  #define SIMEE_BTS_SIZE_B    8192
#else //EMBER_SIMEE1 || EMBER_SIMEE1_8KB
  //NOTE: EMBER_SIMEE1 defaults to 8k
  #define SIMEE_SIZE_B        8192  //Use a 8k 8bit SimEE1
  #define SIMEE_BTS_SIZE_B    2104
#endif
//NOTE: SIMEE_SIZE_B size must be a precise multiple of the page size.
#if ((SIMEE_SIZE_B%MFB_PAGE_SIZE_B) != 0)
  #error Illegal SimEE storage size.  SIMEE_SIZE_B must be a multiple of the flash page size.
#endif

//Convenience define to provide the SimEE size in 16bits.
#define SIMEE_SIZE_HW       (SIMEE_SIZE_B/2)
//Convenience define to provide the flash page size in 16bits.
#define FLASH_PAGE_SIZE_HW  (MFB_PAGE_SIZE_W*2) //derived from a chip's memmap.h

#endif //DOXYGEN_SHOULD_SKIP_THIS

#endif //__SIM_EEPROM_SIZE_H__
