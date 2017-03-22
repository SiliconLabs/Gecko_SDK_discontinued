/** @file hal/micro/cortexm3/mpu.h
 *
 * <!-- Copyright 2008 by Ember Corporation. All rights reserved.-->   
 */


#ifndef __MPU_H__
#define __MPU_H__

#include "hal/micro/micro.h"

#if __MPU_PRESENT

// A region is defined by a struct with two parts, base address and attributes,
// that are loaded into the MPU_BASE and MPU_ATTR registers, respectively.
typedef struct mpu
{
  uint32_t base;
  uint32_t attr;
} mpu_t;

// Number of MPU regions on the Cortex M3
#define NUM_MPU_REGIONS 5

// Region size (SIZE) field values
// Size must be a power of two, from 32 bytes to 4 gigabytes.
// Regions less than 256 bytes cannot be divided into 8 sub regions
#define SIZE_32B    0x04
#define SIZE_64B    0x05
#define SIZE_128B   0x06
#define SIZE_256B   0x07
#define SIZE_512B   0x08
#define SIZE_1K     0x09
#define SIZE_2K     0x0A
#define SIZE_4K     0x0B
#define SIZE_8K     0x0C
#define SIZE_16K    0x0D
#define SIZE_32K    0x0E
#define SIZE_64K    0x0F
#define SIZE_128K   0x10
#define SIZE_256K   0x11
#define SIZE_512K   0x12
#define SIZE_1M     0x13
#define SIZE_2M     0x14
#define SIZE_4M     0x15
#define SIZE_8M     0x16
#define SIZE_16M    0x17
#define SIZE_32M    0x18
#define SIZE_54M    0x19
#define SIZE_128M   0x1A
#define SIZE_256M   0x1B
#define SIZE_512M   0x1C
#define SIZE_1G     0x1D
#define SIZE_2G     0x1E
#define SIZE_4G     0x1F

#include "hal/micro/cortexm3/mpu-config.h"

//A simple utility macro for temporarily turning off the MPU.  Turning off
//the MPU is dangerous and should only be done in critical situations, such
//as executing from RAM and writing Flash.
#ifdef _HAL_MPU_UNUSED_  // bootloaders and min hal don't use MPU
  #define BYPASS_MPU(blah) blah
#else
  #define BYPASS_MPU(blah)                                  \
    {                                                       \
      uint32_t mpuEnabled = MPU->CTRL & MPU_CTRL_ENABLE_Msk;  \
      if(mpuEnabled) {                                      \
        halInternalDisableMPU();                            \
      }                                                     \
      { blah }                                              \
      if(mpuEnabled) {                                      \
       halInternalEnableMPU();                              \
      }                                                     \
    }
#endif

void halInternalLoadMPU(MPU_RegionInit_TypeDef *mp);
void halInternalEnableMPU(void);
void halInternalDisableMPU(void);
void halInternalSetMPUGuardRegionStart(uint32_t baseAddress);

//[[
bool halInternalIAmAnEmulator(void);
//]]
#endif //__MPU_PRESENT

#endif//__MPU_H__
