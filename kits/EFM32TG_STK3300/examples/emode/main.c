/**************************************************************************//**
 * @file main.c
 * @brief Demo for energy mode current consumption testing.
 * @version 4.2.0
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2015 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include "lcd_setup.h"
#include "tg_emodes.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_assert.h"
#include <stdint.h>

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  /* Sets "setup" variable to user desired energy mode case number,
   * and reconfigures chip to initial state.
   * See files for "lcd_setup" for more information. */
  uint32_t setup = LCD_SelectState();

  /* Start the selected energy mode setup (See Data Sheet Table 3.3).
   * See files for "tg_emodes" for more information.
   * Copy and paste function definition to replicate setup. */
  switch (setup)
  {
    case 0:
      TG_EM0_Hfxo();
      break;
    case 1:
      TG_EM0_Hfrco(cmuHFRCOBand_28MHz);
      break;
    case 2:
      TG_EM0_Hfrco(cmuHFRCOBand_21MHz);
      break;
    case 3:
      TG_EM0_Hfrco(cmuHFRCOBand_14MHz);
      break;
    case 4:
      TG_EM0_Hfrco(cmuHFRCOBand_11MHz);
      break;
    case 5:
      TG_EM0_Hfrco(cmuHFRCOBand_7MHz);
      break;
    case 6:
      TG_EM0_Hfrco(cmuHFRCOBand_1MHz);
      break;
    case 7:
      TG_EM1_Hfxo();
      break;
    case 8:
      TG_EM1_Hfrco(cmuHFRCOBand_28MHz);
      break;
    case 9:
      TG_EM1_Hfrco(cmuHFRCOBand_21MHz);
      break;
    case 10:
      TG_EM1_Hfrco(cmuHFRCOBand_14MHz);
      break;
    case 11:
      TG_EM1_Hfrco(cmuHFRCOBand_11MHz);
      break;
    case 12:
      TG_EM1_Hfrco(cmuHFRCOBand_7MHz);
      break;
    case 13:
      TG_EM1_Hfrco(cmuHFRCOBand_1MHz);
      break;
    case 14:
      TG_EM2_LfrcoRTC();
      break;
    case 15:
      TG_EM3();
      break;
    case 16:
      TG_EM4();
      break;
  }

  // Should not be reached.
  EFM_ASSERT(false);
}
