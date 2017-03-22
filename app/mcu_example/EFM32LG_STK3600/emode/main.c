/**************************************************************************//**
 * @file main.c
 * @brief Demo for energy mode current consumption testing for Leopard Gecko.
 * @version 5.1.2
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

#include "lcd_setup.h"
#include "emodes.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_assert.h"
#include <stdint.h>

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  /* Use default settings for EM23, EM4 */
  EMU_EM23Init_TypeDef em23Init = EMU_EM23INIT_DEFAULT;
  EMU_EM4Init_TypeDef em4Init = EMU_EM4INIT_DEFAULT;

  /* Chip errata */
  CHIP_Init();

  /* Initialize EM23 and EM4 with default parameters */
  EMU_EM23Init(&em23Init);
  EMU_EM4Init(&em4Init);

  /* Sets "mode" variable to user desired energy mode enum,
   * and reconfigures chip to initial state.
   * See files for "lcd_setup" for more information. */
  Energy_Mode_TypeDef mode = LCD_SelectMode();

  /* Start the selected energy mode setup (See Data Sheet Table 3.4).
   * See files for "emodes" for more information.
   * Copy and paste function definition to replicate setup. */
  switch (mode)
  {
     case EM0_HFXO_48MHZ:
      em_EM0_Hfxo();
      break;
    case EM0_HFRCO_28MHZ:
      em_EM0_Hfrco(cmuHFRCOBand_28MHz);
      break;
    case EM0_HFRCO_21MHZ:
      em_EM0_Hfrco(cmuHFRCOBand_21MHz);
      break;
    case EM0_HFRCO_14MHZ:
      em_EM0_Hfrco(cmuHFRCOBand_14MHz);
      break;
    case EM0_HFRCO_11MHZ:
      em_EM0_Hfrco(cmuHFRCOBand_11MHz);
      break;
    case EM0_HFRCO_7MHZ:
      em_EM0_Hfrco(cmuHFRCOBand_7MHz);
      break;
    case EM0_HFRCO_1MHZ:
      em_EM0_Hfrco(cmuHFRCOBand_1MHz);
      break;
    case EM1_HFXO_48MHZ:
      em_EM1_Hfxo();
      break;
    case EM1_HFRCO_28MHZ:
      em_EM1_Hfrco(cmuHFRCOBand_28MHz);
      break;
    case EM1_HFRCO_21MHZ:
      em_EM1_Hfrco(cmuHFRCOBand_21MHz);
      break;
    case EM1_HFRCO_14MHZ:
      em_EM1_Hfrco(cmuHFRCOBand_14MHz);
      break;
    case EM1_HFRCO_11MHZ:
      em_EM1_Hfrco(cmuHFRCOBand_11MHz);
      break;
    case EM1_HFRCO_7MHZ:
      em_EM1_Hfrco(cmuHFRCOBand_7MHz);
      break;
    case EM1_HFRCO_1MHZ:
      em_EM1_Hfrco(cmuHFRCOBand_1MHz);
      break;
    case EM2_LFRCO_RTC:
      em_EM2_LfrcoRTC();
      break;
    case EM3_ULFRCO:
      em_EM3();
      break;
    case EM4:
      em_EM4();
      break;
    default:
      EFM_ASSERT(false);
      break;
  }

  // Should not be reached.
  EFM_ASSERT(false);
}
