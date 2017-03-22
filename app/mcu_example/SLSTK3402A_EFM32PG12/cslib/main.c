/**************************************************************************//**
 * Copyright 2016 by Silicon Laboratories Inc. All rights reserved.
 *
 * http://developer.silabs.com/legal/version/v11/Silicon_Labs_Software_License_Agreement.txt
 *****************************************************************************/

#include "em_device.h"
#include "em_chip.h"
#include "em_emu.h"
#include "bspconfig.h"
#include "cslib.h"
#include "device_init.h"
#include "comm_routines.h"


/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_STK_DEFAULT;

  CHIP_Init();

  /* Init DCDC regulator with kit specific parameters. */
  EMU_DCDCInit(&dcdcInit);

  systemInit();

  CSLIB_commInit();

  CSLIB_initHardware();
  CSLIB_initLibrary();

  // Infinite loop
  while (1)
  {
	  CSLIB_update();
	  CSLIB_commUpdate();
	  CSLIB_lowPowerUpdate();
  }
}
