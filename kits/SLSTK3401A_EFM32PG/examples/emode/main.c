/**************************************************************************//**
 * @file main.c
 * @brief Demo for energy mode current consumption testing.
 * @version 4.3.0
 ******************************************************************************
 * @section License
 * <b>Copyright 2016 Silicon Laboratories, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include "lcd_setup.h"
#include "emodes.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_assert.h"
#include "bsp.h"
#include <stdint.h>

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  /* Use default settings for DCDC, HFXO and EM4 */
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_STK_DEFAULT;
  CMU_HFXOInit_TypeDef hfxoInit = CMU_HFXOINIT_STK_DEFAULT;
  EMU_EM4Init_TypeDef em4Init   = EMU_EM4INIT_DEFAULT;

  /* Chip errata */
  CHIP_Init();

  /* Init DCDC regulator and HFXO with kit specific parameters */
  EMU_DCDCInit(&dcdcInit);
  CMU_HFXOInit(&hfxoInit);
  EMU_EM4Init(&em4Init);

  /* Sets "setup" variable to user desired energy mode case number,
   * and reconfigures chip to initial state.
   * See files for "lcd_setup" for more information. */
  uint32_t setup = LCD_SelectState();

  /* Start the selected energy mode setup (See Data Sheet Table 4.1.4.2).
   * See files for "emodes" for more information.
   * Copy and paste function definition to replicate setup.*/
  switch (setup)
  {
    case 0:
      em_EM0_Hfxo();
      break;
    case 1:
      em_EM0_Hfrco(cmuHFRCOFreq_38M0Hz);
      break;
    case 2:
      em_EM0_Hfrco(cmuHFRCOFreq_32M0Hz);
      break;
    case 3:
      em_EM0_Hfrco(cmuHFRCOFreq_26M0Hz);
      break;
    case 4:
      em_EM0_Hfrco(cmuHFRCOFreq_19M0Hz);
      break;
    case 5:
      em_EM0_Hfrco(cmuHFRCOFreq_16M0Hz);
      break;
    case 6:
      em_EM1_Hfxo();
      break;
    case 7:
      em_EM1_Hfrco(cmuHFRCOFreq_38M0Hz);
      break;
    case 8:
      em_EM1_Hfrco(cmuHFRCOFreq_32M0Hz);
      break;
    case 9:
      em_EM1_Hfrco(cmuHFRCOFreq_26M0Hz);
      break;
    case 10:
      em_EM1_Hfrco(cmuHFRCOFreq_19M0Hz);
      break;
    case 11:
      em_EM1_Hfrco(cmuHFRCOFreq_16M0Hz);
      break;
    case 12:
      em_EM2_LfrcoRTCC();
      break;
    case 13:
      em_EM3_Ulfrco();
      break;
    case 14:
      em_EM4();
      break;
  }

  // Should not be reached.
  EFM_ASSERT(false);
}
