//=============================================================================
// FILE
//   mpu.c - Functions to setup and control the EM3588's MPU
//
// DESCRIPTION
//   This file contains the definitions, data table and software used to
//   setup the MPU.
//
//   Copyright 2008-2011 by Ember Corporation. All rights reserved.         *80*
//=============================================================================

#include PLATFORM_HEADER
#include "em_mpu.h"
#include "hal/micro/cortexm3/efm32/mpu.h"
#include "hal/micro/micro.h"

#if __MPU_PRESENT
// Define MPU regions - note that the default vector table at address 0 and
// the ARM PPB peripherals are always accessible in privileged mode.
static MPU_RegionInit_TypeDef mpuConfig[NUM_MPU_REGIONS] =
{
  MPU_REGION0,
  MPU_REGION1,
  MPU_REGION2,
  MPU_REGION3,
  MPU_REGION4
};

// Load the base address and attributes of all 8 MPU regions, then enable
// the MPU. Even though interrupts should be disabled when this function is
// called, the region loading is performed in an atomic block in case they
// are not disabled. After the regions have been defined, the MPU is enabled.
// To be safe, memory barrier instructions are included to make sure that
// the new MPU setup is in effect before returning to the caller.
//
// Note that the PRIVDEFENA bit is not set in the MPU_CTRL register so the 
// default privileged memory map is not enabled. Disabling the default
// memory map enables faults on core accesses (other than vector reads) to 
// the address ranges shown below.
//
//  Address range
//  Flash (ICODE and DCODE)
//    00000000 to 07FFFFFF    no access allowed (read, write or execute)
//    08000000 to 0809FFFF    write not allowed
//    080A0000 to 1FFFFFFF    no access allowed (read, write or execute)
//  SRAM
//    20000000 to 2000FFFF    execute not allowed
//    20010000 to 3FFFFFFF    no access allowed (read, write or execute)
//  Peripheral
//    40020000 to 5FFFFFFF    no access allowed (read, write or execute)
//  External Device / External RAM
//    60000000 to DFFFFFFF    no access allowed (read, write or execute)

void halInternalEnableMPU(void)
{
  ATOMIC(
  MPU_Disable();
  halInternalLoadMPU(mpuConfig);
  MPU_Enable(MPU_CTRL_PRIVDEFENA);
  )
}

void halInternalLoadMPU(MPU_RegionInit_TypeDef *mp)
{

  uint8_t i;
  
  for (i = 0; i < NUM_MPU_REGIONS; i++) 
  {
    MPU_ConfigureRegion(mp);
    mp++;
  }
   _executeBarrierInstructions();
}

void halInternalDisableMPU(void)
{
  MPU_Disable();
  _executeBarrierInstructions();
}

void halInternalSetMPUGuardRegionStart(uint32_t baseAddress)
{

  uint32_t enable = false;

  // Clear the lower 5 bits of the base address to be sure that it's
  // properly aligned
  baseAddress &= 0xFFFFFFE0;

  // If the base address is below the reset info then something weird is
  // going on so just turn off the guard region
  if(baseAddress < (uint32_t)_RESETINFO_SEGMENT_END) {
    enable = false;
  }

  // Add in the guard region number so the MPU knows which region to configure
  baseAddress |= GUARD_REGION & 0x0000001F;

  // Set the base address for the MPU guard region
  ATOMIC(
    mpuConfig[GUARD_REGION & 0xF].baseAddress = baseAddress;
    mpuConfig[GUARD_REGION & 0xF].regionEnable = enable;
    );
}

bool halInternalIAmAnEmulator(void)
{
  return 0;
}
#endif //__MPU_PRESENT
