/**************************************************************************//**
 * @file main.c
 * @brief uC/OS-III + GLIB example
 * @version 5.1.2
 *
 ******************************************************************************
 * @section License
 * <b>Copyright 2016 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "display.h"
#include "dmd.h"
#include "bspconfig.h"
#include "task.h"

#include <os.h>

/**************************************************************************//**
 * @brief
 *   Initialize graphics
 *****************************************************************************/
static void GRAPHICS_Init(void)
{
  EMSTATUS status;
  
  /* Initialize the display module. */
  status = DISPLAY_Init();
  EFM_ASSERT(DISPLAY_EMSTATUS_OK == status);

  /* Initialize the DMD module for the DISPLAY device driver. */
  status = DMD_init(0);
  EFM_ASSERT(DMD_OK == status);
}

/**************************************************************************//**
 * @brief
 *   Main function of uC/OS-III GLIB example.
 *****************************************************************************/
int main(void)
{
  OS_ERR err;
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_STK_DEFAULT;
  CMU_HFXOInit_TypeDef hfxoInit = CMU_HFXOINIT_STK_DEFAULT;

  /* Chip errata */
  CHIP_Init();

  /* Init DCDC regulator and HFXO with kit specific parameters */
  EMU_DCDCInit(&dcdcInit);
  CMU_HFXOInit(&hfxoInit);

  /* Switch HFCLK to HFXO and disable HFRCO */
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);
  CMU_OscillatorEnable(cmuOsc_HFRCO, false, false);

  /* Initialize graphics subsystem */
  GRAPHICS_Init();

  /* Initialize tasks and uC/OS-III */
  TASK_Init();

  /* Start uC/OS-III */
  OSStart(&err);

  /* We should never get here */
  while (1) 
    ;
}

