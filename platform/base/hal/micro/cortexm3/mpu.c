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
#include "hal/micro/cortexm3/mpu.h"
#include "hal/micro/micro.h"

// Define MPU regions - note that the default vector table at address 0 and
// the ARM PPB peripherals are always accessible in privileged mode.
static mpu_t mpuConfig[NUM_MPU_REGIONS] =
{
  { MPU_REGION0_BASE, MPU_REGION0_ATTR },
  { MPU_REGION1_BASE, MPU_REGION1_ATTR },
  { MPU_REGION2_BASE, MPU_REGION2_ATTR },
  { MPU_REGION3_BASE, MPU_REGION3_ATTR },
  { MPU_REGION4_BASE, MPU_REGION4_ATTR },
  { MPU_REGION5_BASE, MPU_REGION5_ATTR },
  { MPU_REGION6_BASE, MPU_REGION6_ATTR },
  { MPU_REGION7_BASE, MPU_REGION7_ATTR }
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
  halInternalLoadMPU(mpuConfig);
}

void halInternalLoadMPU(mpu_t *mp)
{
  uint8_t i;

  ATOMIC (
    MPU->CTRL = 0;       // enable default map while MPU is updated
    for (i = 0; i < NUM_MPU_REGIONS; i++) {
      MPU->RBAR = mp->base;
      MPU->RASR = mp->attr;
      mp++;
    }
    MPU->CTRL = MPU_CTRL_ENABLE_Msk;
    _executeBarrierInstructions();
  )

}

void halInternalDisableMPU(void)
{
  MPU->CTRL = 0;
  _executeBarrierInstructions();
}

void halInternalSetMPUGuardRegionStart(uint32_t baseAddress)
{
  uint32_t attr = GUARD_REGION_ATTR_EN;

  // Clear the lower 5 bits of the base address to be sure that it's
  // properly aligned
  baseAddress &= 0xFFFFFFE0;

  // If the base address is below the reset info then something weird is
  // going on so just turn off the guard region
  if(baseAddress < (uint32_t)_RESETINFO_SEGMENT_END) {
    attr = GUARD_REGION_ATTR_DIS;
  }

  // Add in the guard region number so the MPU knows which region to configure
  baseAddress |= GUARD_REGION & 0x0000001F;

  // Set the base address for the MPU guard region
  ATOMIC(
    mpuConfig[GUARD_REGION & 0xF].base = baseAddress;
    mpuConfig[GUARD_REGION & 0xF].attr = attr;
    );
}

bool halInternalIAmAnEmulator(void)
{
  bool retval;

  ATOMIC(
    MPU->CTRL = 0;
    _executeBarrierInstructions();
    retval =  ((I_AM_AN_EMULATOR & 1) == 1);
    MPU->CTRL = MPU_CTRL_ENABLE_Msk;
    _executeBarrierInstructions();
  )
  return retval;
}
