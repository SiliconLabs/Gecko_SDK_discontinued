/**************************************************************************//**
 * @file main.c
 * @brief Demo for energy mode current consumption testing.
 * @version 5.0.0
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
#include "coremark/core_main.h"
#include "bsp.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_assert.h"
#include <stdint.h>

/**************************************************************************//**
 * @brief  Main function
 *
 * @note Hard reset must occur to enter energy mode correctly. Move
 *****************************************************************************/
int main(void)
{
  /* Use default settings for DCDC, EM23, and HFXO */
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_STK_DEFAULT;
  EMU_EM23Init_TypeDef em23Init = EMU_EM23INIT_DEFAULT;
  CMU_HFXOInit_TypeDef hfxoInit = CMU_HFXOINIT_DEFAULT;

  /* Chip errata */
  CHIP_Init();

  /* Initialize EM23 and HFXO with default parameters */
  EMU_EM23Init(&em23Init);
  CMU_HFXOInit(&hfxoInit);

  /* Sets "mode" variable to user desired energy mode enum,
   * and reconfigures chip to initial state.
   * See files for "lcd_setup" for more information. */
  Energy_Mode_TypeDef mode = LCD_SelectMode();

  /* Start the selected energy mode setup (See Data Sheet Table 4.4-4.6).
   * See files for "emodes" for more information.
   * Copy and paste function definition to replicate setup.*/
  switch (mode)
  {
    /* Case "EM0_HFXO_40MHZ_WHILE" to "EM4S" correspond to Table 4.4 and 4.5.
     * NOTE: DCDC is initialized in bypass mode to disable DCDC.
     */
    case EM0_HFXO_40MHZ_WHILE:
      dcdcInit.dcdcMode = emuDcdcMode_Bypass;
      EMU_DCDCInit(&dcdcInit);
      em_EM0_Hfxo();
      while(1);
    case EM0_HFRCO_38MHZ_PRIME:
      dcdcInit.dcdcMode = emuDcdcMode_Bypass;
      EMU_DCDCInit(&dcdcInit);
      em_EM0_Hfrco(cmuHFRCOFreq_38M0Hz);
      primeCalc();
      break;
    case EM0_HFRCO_38MHZ_WHILE:
      dcdcInit.dcdcMode = emuDcdcMode_Bypass;
      EMU_DCDCInit(&dcdcInit);
      em_EM0_Hfrco(cmuHFRCOFreq_38M0Hz);
      while(1);
    case EM0_HFRCO_38MHZ_COREMARK:
      dcdcInit.dcdcMode = emuDcdcMode_Bypass;
      EMU_DCDCInit(&dcdcInit);
      em_EM0_Hfrco(cmuHFRCOFreq_38M0Hz);
  	  CMU_ClockEnable(cmuClock_HFPER, true); // enable timer for CoreMark
      while(1){
        CoreMark_Main();
      }
    case EM0_HFRCO_26MHZ_WHILE:
      dcdcInit.dcdcMode = emuDcdcMode_Bypass;
      EMU_DCDCInit(&dcdcInit);
      em_EM0_Hfrco(cmuHFRCOFreq_26M0Hz);
      while(1);
    case EM0_HFRCO_1MHZ_WHILE:
      dcdcInit.dcdcMode = emuDcdcMode_Bypass;
      EMU_DCDCInit(&dcdcInit);
      em_EM0_Hfrco(cmuHFRCOFreq_1M0Hz);
      while(1);
    case EM1_HFXO_40MHZ:
      dcdcInit.dcdcMode = emuDcdcMode_Bypass;
      EMU_DCDCInit(&dcdcInit);
      em_EM1_Hfxo();
      break;
    case EM1_HFRCO_38MHZ:
      dcdcInit.dcdcMode = emuDcdcMode_Bypass;
      EMU_DCDCInit(&dcdcInit);
      em_EM1_Hfrco(cmuHFRCOFreq_38M0Hz);
      break;
    case EM1_HFRCO_26MHZ:
      dcdcInit.dcdcMode = emuDcdcMode_Bypass;
      EMU_DCDCInit(&dcdcInit);
      em_EM1_Hfrco(cmuHFRCOFreq_26M0Hz);
      break;
    case EM1_HFRCO_1MHZ:
      dcdcInit.dcdcMode = emuDcdcMode_Bypass;
      EMU_DCDCInit(&dcdcInit);
      em_EM1_Hfrco(cmuHFRCOFreq_1M0Hz);
      break;
    case EM2_LFXO_RTCC:
      dcdcInit.dcdcMode = emuDcdcMode_Bypass;
      EMU_DCDCInit(&dcdcInit);
      // Full RAM
      em_EM2_RTCC(cmuSelect_LFXO, false); // disable RAM powerdown
      break;
    case EM2_LFRCO_RTCC:
      dcdcInit.dcdcMode = emuDcdcMode_Bypass;
      EMU_DCDCInit(&dcdcInit);
      // 4kB RAM
      em_EM2_RTCC(cmuSelect_LFRCO, true); // enable RAM powerdown
      break;
    case EM3_ULFRCO_CRYO:
      dcdcInit.dcdcMode = emuDcdcMode_Bypass;
      EMU_DCDCInit(&dcdcInit);
      // Full RAM
      em_EM3_UlfrcoCRYO();
      break;
    case EM4H_LFXO_RTCC:
      dcdcInit.dcdcMode = emuDcdcMode_Bypass;
      EMU_DCDCInit(&dcdcInit);
      // 128b RAM
      em_EM4H_LfxoRTCC();
      break;
    case EM4H_ULFRCO_CRYO:
      dcdcInit.dcdcMode = emuDcdcMode_Bypass;
      EMU_DCDCInit(&dcdcInit);
      // 128b RAM
      em_EM4H_UlfrcoCRYO();
      break;
    case EM4H:
      dcdcInit.dcdcMode = emuDcdcMode_Bypass;
      EMU_DCDCInit(&dcdcInit);
      // 128b RAM
      em_EM4H();
      break;
    case EM4S:
      dcdcInit.dcdcMode = emuDcdcMode_Bypass;
      EMU_DCDCInit(&dcdcInit);
      // No RAM
      em_EM4S();
      break;

    /* Case "EM0_HFXO_40MHZ_WHILE_DCDC" to "EM4S_DCDC" correspond to Table 4.6.
     * NOTE: DCDC is initialized to match configurations in Table 4.6 by
     *       using the default settings, except for in EM1 which is manually
     *       set to Low Power.
     */
    case EM0_HFXO_40MHZ_WHILE_DCDC:
      EMU_DCDCInit(&dcdcInit);
      em_EM0_Hfxo();
      while(1);
    case EM0_HFRCO_38MHZ_PRIME_DCDC:
      EMU_DCDCInit(&dcdcInit);
      em_EM0_Hfrco(cmuHFRCOFreq_38M0Hz);
      primeCalc();
      break;
    case EM0_HFRCO_38MHZ_WHILE_DCDC:
      EMU_DCDCInit(&dcdcInit);
      em_EM0_Hfrco(cmuHFRCOFreq_38M0Hz);
      while(1);
    case EM0_HFRCO_38MHZ_COREMARK_DCDC:
      EMU_DCDCInit(&dcdcInit);
      em_EM0_Hfrco(cmuHFRCOFreq_38M0Hz);
  	  CMU_ClockEnable(cmuClock_HFPER, true); // enable timer for CoreMark
      while(1){
        CoreMark_Main();
      }
    case EM0_HFRCO_26MHZ_WHILE_DCDC:
      EMU_DCDCInit(&dcdcInit);
      em_EM0_Hfrco(cmuHFRCOFreq_26M0Hz);
      while(1);
    case EM1_HFXO_40MHZ_DCDC:
      dcdcInit.dcdcMode = (EMU_DcdcMode_TypeDef)EMU_DCDCCTRL_DCDCMODE_LOWPOWER;
      EMU_DCDCInit(&dcdcInit);
      em_EM1_Hfxo();
      break;
    case EM1_HFRCO_38MHZ_DCDC:
      dcdcInit.dcdcMode = (EMU_DcdcMode_TypeDef)EMU_DCDCCTRL_DCDCMODE_LOWPOWER;
      EMU_DCDCInit(&dcdcInit);
      em_EM1_Hfrco(cmuHFRCOFreq_38M0Hz);
      break;
    case EM1_HFRCO_26MHZ_DCDC:
      dcdcInit.dcdcMode = (EMU_DcdcMode_TypeDef)EMU_DCDCCTRL_DCDCMODE_LOWPOWER;
      EMU_DCDCInit(&dcdcInit);
      em_EM1_Hfrco(cmuHFRCOFreq_26M0Hz);
      break;
    case EM1_HFRCO_1MHZ_DCDC:
      dcdcInit.dcdcMode = (EMU_DcdcMode_TypeDef)EMU_DCDCCTRL_DCDCMODE_LOWPOWER;
      EMU_DCDCInit(&dcdcInit);
      em_EM1_Hfrco(cmuHFRCOFreq_1M0Hz);
      break;
    case EM2_LFXO_RTCC_DCDC:
      EMU_DCDCInit(&dcdcInit);
      // Full RAM
      em_EM2_RTCC(cmuSelect_LFXO, false); // disable RAM powerdown
      break;
    case EM2_LFRCO_RTCC_DCDC:
      EMU_DCDCInit(&dcdcInit);
      // 4kB RAM
      em_EM2_RTCC(cmuSelect_LFRCO, true); // enable RAM powerdown
      break;
    case EM3_ULFRCO_CRYO_DCDC:
      EMU_DCDCInit(&dcdcInit);
      // Full RAM
      em_EM3_UlfrcoCRYO();
      break;
    case EM4H_LFXO_RTCC_DCDC:
      EMU_DCDCInit(&dcdcInit);
      // 128b RAM
      em_EM4H_LfxoRTCC();
      break;
    case EM4H_ULFRCO_CRYO_DCDC:
      EMU_DCDCInit(&dcdcInit);
      // 128b RAM
      em_EM4H_UlfrcoCRYO();
      break;
    case EM4H_DCDC:
      EMU_DCDCInit(&dcdcInit);
      // 128b RAM
      em_EM4H();
      break;
    case EM4S_DCDC:
      EMU_DCDCInit(&dcdcInit);
      // No RAM
      em_EM4S();
      break;
    default:
      EFM_ASSERT(false);
      break;
  }

  // Should not be reached.
  EFM_ASSERT(false);
}
