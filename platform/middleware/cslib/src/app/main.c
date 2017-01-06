/**************************************************************************//**
 * Copyright 2016 by Silicon Laboratories Inc. All rights reserved.
 *
 * http://developer.silabs.com/legal/version/v11/Silicon_Labs_Software_License_Agreement.txt
 *****************************************************************************/

#include "em_device.h"
#include "em_chip.h"
#if defined( _EMU_DCDCCTRL_MASK )
#include "em_emu.h"
#include "bspconfig.h"
#endif
#include "cslib.h"
#include "device_init.h"
#include "comm_routines.h"


/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
#if defined( _EMU_DCDCCTRL_MASK )
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_STK_DEFAULT;
#endif

  CHIP_Init();

#if defined( _EMU_DCDCCTRL_MASK )
  /* Init DCDC regulator with kit specific parameters. */
  EMU_DCDCInit(&dcdcInit);
#endif

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
