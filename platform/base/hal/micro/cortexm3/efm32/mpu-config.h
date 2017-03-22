/** @file hal/micro/cortexm3/efm32/mpu-config.h
 *
 * @brief MPU configuration for the efr32/ezr32
 *
 * <!-- Copyright 2015 Silicon Laboratories, Inc.                        *80*-->
 */

#ifndef __MPU_H__
  #error This header should not be included directly, use hal/micro/cortexm3/mpu.h
#endif

#ifndef EFM32_MPU_CONFIG_H
#define EFM32_MPU_CONFIG_H

#include "hal/micro/micro.h"

#if __MPU_PRESENT

// Define the address offsets for all of our MPU regions
// For EFR32 we do NOT include the enable bit and region number in the
// lower 5 bits -- emlib's em_mpu.c::MPU_ConfigureRegion() does not
// expect those bits to be set and will assert if they are.
#define FLASH_REGION    (0x00000000)
#define PERIPH_REGION   (0x40000000)
#define USERPER_REGION  (0x40000000)
#define SRAM_REGION     (0x20000000)
#define GUARD_REGION    (0x20000000)
#define SPARE0_REGION   (0x20000000)
#define SPARE1_REGION   (0x20000000)
#define SPARE2_REGION   (0x20000000)

//=============================================================================
// Define the data used to initialize the MPU. Each of the 8 MPU regions
// has a programmable size and various attributes. A region must be a power of
// two in size, and its base address must be a multiple of that size. Regions
// are divided into 8 equal-sized sub-regions that can be individually disabled.
// A region is defined by what is written to MPU_BASE and MPU_ATTR.
// MPU_BASE holds the region base address, with some low order bits ignored
// depending on the region size. If B4 is set, then B3:0 set the region number.
// The MPU_ATTR fields are:
// XN (1 bit) - set to disable instruction execution
// AP (2 bits) - selects Privilege & User access- None, Read-only or Read-Write
// TEX,S,C,B (6 bits) - configures memory type, write ordering, shareable, ...
// SRD (8 bits) - a set bit disables the corresponding sub-region
// SIZE (5 bits) - specifies the region size as a power of two
// ENABLE (1 bit) - set to enable the region, except any disabled sub-regions
//=============================================================================


// Region 0 - Flash, including main, fixed and customer info blocks: 
//            execute, normal, not shareable
// Enabled sub-regions: 08000000 - 0802FFFF
// #define FLASH_REGION_ATTR    MATTR(0, PRO_URO, MEM_NORMAL, 0xE8, SIZE_512K, 1)
/** Default configuration of MPU region init structure for onchip peripherals.*/
#define MPU_FLASH_REGION                                        \
  {                                                             \
    true,                   /* Enable MPU region.            */ \
    0,                      /* MPU Region number.            */ \
    FLASH_REGION,           /* Flash base address.           */ \
    mpuRegionSize256Kb,      /* Size - Set to max.            */ \
    mpuRegionApPRwURo,      /* Access permissions.           */ \
    false,                  /* Execution allowed.            */ \
    false,                  /* Not shareable.                */ \
    true,                   /* Cacheable.                    */ \
    true,                   /* Bufferable.                   */ \
    0xE8,                   /* Disabled subregions.          */ \
    1                       /* TEX attributes.               */ \
  }

// Region 1 - System peripherals: no execute, non-shared device
// Enabled sub-regions: 40000000 - 40008000, 4000A000 - 4000FFFF
// #define PERIPH_REGION_ATTR   MATTR(1, PRW_URO, MEM_DEVICE, 0x10, SIZE_64K,  1)
/** Default configuration of MPU region init structure for onchip peripherals.*/
#define MPU_PERIPH_REGION                                       \
  {                                                             \
    false,                   /* Enable MPU region.            */ \
    1,                      /* MPU Region number.            */ \
    PERIPH_REGION,          /* Region base address.          */ \
    mpuRegionSize64Kb,       /* Size - Set to minimum         */ \
    mpuRegionApPRwURo,      /* Access permissions.           */ \
    false,                  /* Execution not allowed.        */ \
    false,                  /* Not shareable.                */ \
    false,                  /* Not cacheable.                */ \
    false,                  /* Not bufferable.               */ \
    0x10,                   /* Disabled subregions.          */ \
    2                       /* TEX attributes.               */ \
  }


// Region 2 - User peripherals: no execute, non-shared device
// Enabled sub-regions: 4000A000 - 4000FFFF
// #define USERPER_REGION_ATTR  MATTR(1, PRW_URW, MEM_DEVICE, 0x03, SIZE_32K,  1)
/** Default configuration of MPU region init structure for onchip peripherals.*/
#define MPU_USERPER_REGION                                       \
  {                                                             \
    false,                   /* Enable MPU region.            */ \
    2,                      /* MPU Region number.            */ \
    USERPER_REGION,         /* Region base address.          */ \
    mpuRegionSize32Kb,       /* Size - Set to minimum         */ \
    mpuRegionApFullAccess,  /* Access permissions.           */ \
    false,                  /* Execution not allowed.        */ \
    false,                  /* Not shareable.                */ \
    false,                  /* Not cacheable.                */ \
    false,                  /* Not bufferable.               */ \
    0x03,                   /* Disabled subregions.          */ \
    1                       /* TEX attributes.               */ \
  }

// Region 3 - SRAM: no execute, normal, not shareable
// Enabled sub-regions: 20000000 - 20002FFF
// #define SRAM_REGION_ATTR     MATTR(1, PRW_URW, MEM_NORMAL, 0xC0, SIZE_16K,  1)
/** Default configuration of MPU region init structure for sram memory.      */
#define MPU_SRAM_REGION                                         \
  {                                                             \
    true,                   /* Enable MPU region.            */ \
    3,                      /* MPU Region number.            */ \
    SRAM_REGION,            /* SRAM base address.            */ \
    mpuRegionSize32Kb,       /* Size - Set to max.            */ \
    mpuRegionApFullAccess,  /* Access permissions.           */ \
    false,                  /* Execution allowed.            */ \
    false,                  /* Not shareable.                */ \
    true,                   /* Cacheable.                    */ \
    true,                   /* Bufferable.                   */ \
    0xC0,                   /* Disabled subregions.          */ \
    1                       /* TEX attributes.               */ \
  }

// Region 4 - Guard region between the heap and stack
// #define GUARD_REGION_ATTR_EN    MATTR(1, PNA_UNA, MEM_NORMAL, 0x00, \
//                                       HEAP_GUARD_REGION_SIZE, 1)
#define MPU_GUARD_REGION_EN                                     \
  {                                                             \
    true,                   /* Enable MPU region.            */ \
    4,                      /* MPU Region number.            */ \
    GUARD_REGION,           /* Region base address.          */ \
    HEAP_GUARD_REGION_SIZE, /* Size - Set to minimum         */ \
    mpuRegionNoAccess,      /* Access permissions.           */ \
    false,                  /* Execution not allowed.        */ \
    false,                  /* Not shareable.                */ \
    false,                  /* Not cacheable.                */ \
    false,                  /* Not bufferable.               */ \
    0x00,                   /* Disabled subregions.          */ \
    1                       /* TEX attributes.               */ \
  }
// #define GUARD_REGION_ATTR_DIS   MATTR(1, PNA_UNA, MEM_NORMAL, 0x00, \
//                                       HEAP_GUARD_REGION_SIZE, 0)
#define MPU_GUARD_REGION_DIS                                    \
  {                                                             \
    false,                  /* Enable MPU region.            */ \
    4,                      /* MPU Region number.            */ \
    GUARD_REGION,           /* Region base address.          */ \
    HEAP_GUARD_REGION_SIZE, /* Size - Set to minimum         */ \
    mpuRegionNoAccess,      /* Access permissions.           */ \
    false,                  /* Execution not allowed.        */ \
    false,                  /* Not shareable.                */ \
    false,                  /* Not cacheable.                */ \
    false,                  /* Not bufferable.               */ \
    0x00,                   /* Disabled subregions.          */ \
    1                       /* TEX attributes.               */ \
  }


// Map the regions defined above into more generic versions that are
// appropriate for mpu.c
#define MPU_REGION0  MPU_FLASH_REGION
#define MPU_REGION1  MPU_PERIPH_REGION
#define MPU_REGION2  MPU_USERPER_REGION
#define MPU_REGION3  MPU_SRAM_REGION
#define MPU_REGION4  MPU_GUARD_REGION_DIS

#endif // __MPU_PRESENT

#endif // EFM32_MPU_CONFIG_H
