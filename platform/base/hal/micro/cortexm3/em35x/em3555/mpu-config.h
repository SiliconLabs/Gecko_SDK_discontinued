/** @file hal/micro/cortexm3/em35x/em3555/mpu-config.h
 *
 * @brief MPU configuration for the em3555
 *
 * THIS IS A GENERATED FILE.  DO NOT EDIT.
 *
 * <!-- Copyright 2016 Silicon Laboratories, Inc.                        *80*-->
 */

#ifndef __MPU_H__
  #error This header should not be included directly, use hal/micro/cortexm3/mpu.h
#endif

#ifndef __EM3555_MPU_CFG_H__
#define __EM3555_MPU_CFG_H__

#include "hal/micro/micro.h"

// Define the address offsets for all of our MPU regions
#define FLASH_REGION    (0x08000000 + 0x10)
#define PERIPH_REGION   (0x40000000 + 0x11)
#define USERPER_REGION  (0x40008000 + 0x12)
#define SRAM_REGION     (0x20000000 + 0x13)
#define GUARD_REGION    (0x20000000 + 0x14)
#define SPARE0_REGION   (0x20000000 + 0x15)
#define SPARE1_REGION   (0x20000000 + 0x16)
#define SPARE2_REGION   (0x20000000 + 0x17)

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
// Enabled sub-regions: 08000000 - 0809FFFF
#define FLASH_REGION_ATTR    MATTR(0, PRO_URO, MEM_NORMAL, 0xE0, SIZE_1M, 1)
// Region 1 - System peripherals: no execute, non-shared device
// Enabled sub-regions: 40000000 - 4001FFFF
#define PERIPH_REGION_ATTR   MATTR(1, PRW_URO, MEM_DEVICE, 0x00, SIZE_128K,  1)
// Region 2 - User peripherals: no execute, non-shared device
// Enabled sub-regions: 4000A000 - 4000FFFF
#define USERPER_REGION_ATTR  MATTR(1, PRW_URW, MEM_DEVICE, 0x03, SIZE_32K,  1)
// Region 3 - SRAM: no execute, normal, not shareable
// Enabled sub-regions: 20000000 - 2000FFFF
#define SRAM_REGION_ATTR     MATTR(1, PRW_URW, MEM_NORMAL, 0x00, SIZE_32K,  1)
// Region 4 - Guard region between the heap and stack
#define GUARD_REGION_ATTR_EN    MATTR(1, PNA_UNA, MEM_NORMAL, 0x00, \
                                      HEAP_GUARD_REGION_SIZE, 1)
#define GUARD_REGION_ATTR_DIS   MATTR(1, PNA_UNA, MEM_NORMAL, 0x00, \
                                      HEAP_GUARD_REGION_SIZE, 0)
// Regions 5-7 - unused: disabled (otherwise set up for SRAM)
#define SPARE0_REGION_ATTR   MATTR(1, PRW_URW, MEM_NORMAL, 0x00, SIZE_1K,   0)
#define SPARE1_REGION_ATTR   MATTR(1, PRW_URW, MEM_NORMAL, 0x00, SIZE_1K,   0)
#define SPARE2_REGION_ATTR   MATTR(1, PRW_URW, MEM_NORMAL, 0x00, SIZE_1K,   0)


// Map the regions defined above into more generic versions that are
// appropriate for mpu.c
#define MPU_REGION0_BASE  FLASH_REGION
#define MPU_REGION1_BASE  PERIPH_REGION
#define MPU_REGION2_BASE  USERPER_REGION
#define MPU_REGION3_BASE  SRAM_REGION
#define MPU_REGION4_BASE  GUARD_REGION
#define MPU_REGION5_BASE  SPARE0_REGION
#define MPU_REGION6_BASE  SPARE1_REGION
#define MPU_REGION7_BASE  SPARE2_REGION

#define MPU_REGION0_ATTR  FLASH_REGION_ATTR
#define MPU_REGION1_ATTR  PERIPH_REGION_ATTR
#define MPU_REGION2_ATTR  USERPER_REGION_ATTR
#define MPU_REGION3_ATTR  SRAM_REGION_ATTR
#define MPU_REGION4_ATTR  GUARD_REGION_ATTR_DIS
#define MPU_REGION5_ATTR  SPARE0_REGION_ATTR
#define MPU_REGION6_ATTR  SPARE1_REGION_ATTR
#define MPU_REGION7_ATTR  SPARE2_REGION_ATTR

#endif //__EM3555_MPU_CFG_H__
