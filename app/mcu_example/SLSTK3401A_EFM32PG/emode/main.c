/**************************************************************************//**
 * @file main.c
 * @brief Demo for energy mode current consumption testing.
 * @version 5.1.2
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
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_assert.h"
#include <stdint.h>

/**************************************************************************//**
 * @brief  Main function
 *
 * @note Hard reset must occur to enter energy mode correctly.
 *****************************************************************************/
int main(void)
{
  /* Use default settings for DCDC, EM23, and HFXO */
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_STK_DEFAULT;
  EMU_EM23Init_TypeDef em23Init = EMU_EM23INIT_DEFAULT;
  CMU_HFXOInit_TypeDef hfxoInit = CMU_HFXOINIT_DEFAULT;

#if defined(ADVANCED_LOWPOWER_FEATURES)
  EMU_EM01Init_TypeDef em01Init = EMU_EM01INIT_DEFAULT;
  EMU_EM4Init_TypeDef em4Init = EMU_EM4INIT_DEFAULT;

  /* Enable voltage downscaling in EM0 to 4. */
  em01Init.vScaleEM01LowPowerVoltageEnable = true;
  em23Init.vScaleEM23Voltage = emuVScaleEM23_LowPower;
  em4Init.vScaleEM4HVoltage = emuVScaleEM4H_LowPower;
#endif

  /* Chip errata */
  CHIP_Init();

  /* Unlatch EM4 pin retention */
  EMU_UnlatchPinRetention();

  /* Initialize DCDC. Always start in low-noise mode. */
  dcdcInit.dcdcMode = emuDcdcMode_LowNoise;
  EMU_DCDCInit(&dcdcInit);

  /* Initialize EM23 and HFXO with default parameters */
  EMU_EM23Init(&em23Init);
  CMU_HFXOInit(&hfxoInit);

#if defined(ADVANCED_LOWPOWER_FEATURES)
  /* Initialize EM0123 and HFXO with default parameters */
  EMU_EM01Init(&em01Init);
  EMU_EM4Init(&em4Init);

  /* Disable peripheral retention. */
  EMU_PeripheralRetention(emuPeripheralRetention_ALL, false);
#endif

  /* Sets "mode" variable to user desired energy mode enum,
   * and reconfigures chip to initial state.
   * See files for "lcd_setup" for more information. */
  Energy_Mode_TypeDef mode = LCD_SelectMode();

  /* Start the selected energy mode setup (See Data Sheet Table 4.5-4.7).
   * See files for "emodes" for more information.
   * Copy and paste DCDC configuration and emode function definition to replicate setup.*/

  /* Configure DCDC for selected energy mode */
  switch (mode)
  {
    /* Switch DCDC to bypass mode. */
    case EM0_HFXO_40MHZ_WHILE:
    case EM0_HFRCO_38MHZ_PRIME:
    case EM0_HFRCO_38MHZ_WHILE:
    case EM0_HFRCO_38MHZ_COREMARK:
    case EM0_HFRCO_26MHZ_WHILE:
    case EM0_HFRCO_1MHZ_WHILE:
    case EM1_HFXO_40MHZ:
    case EM1_HFRCO_38MHZ:
    case EM1_HFRCO_26MHZ:
    case EM1_HFRCO_1MHZ:
    case EM2_LFXO_RTCC:
    case EM2_LFRCO_RTCC:
    case EM3_ULFRCO_CRYO:
    case EM4H_LFXO_RTCC:
    case EM4H_ULFRCO_CRYO:
    case EM4H:
    case EM4S:
      EMU_DCDCModeSet(emuDcdcMode_Bypass);
      break;

#if defined(ADVANCED_LOWPOWER_FEATURES)
    /* Switch EM0/1-LP mode. */
    case EM0_HFXO_40MHZ_WHILE_LP:
    case EM0_HFRCO_38MHZ_PRIME_LP:
    case EM0_HFRCO_38MHZ_WHILE_LP:
    case EM0_HFRCO_38MHZ_COREMARK_LP:
    case EM0_HFRCO_26MHZ_WHILE_LP:
    case EM0_HFRCO_1MHZ_WHILE_LP:
    case EM1_HFXO_40MHZ_LP:
    case EM1_HFRCO_38MHZ_LP:
    case EM1_HFRCO_26MHZ_LP:
    case EM1_HFRCO_1MHZ_LP:
      EMU_DCDCModeSet(emuDcdcMode_LowPower);
      break;
#endif

    /* DCDC in Low-noise DCM (default for EFM32). */
    case EM0_HFXO_40MHZ_WHILE_DCDC_DCM:
    case EM0_HFRCO_38MHZ_PRIME_DCDC_DCM:
    case EM0_HFRCO_38MHZ_WHILE_DCDC_DCM:
    case EM0_HFRCO_38MHZ_COREMARK_DCDC_DCM:
    case EM0_HFRCO_26MHZ_WHILE_DCDC_DCM:
    case EM1_HFXO_40MHZ_DCDC_DCM:
    case EM1_HFRCO_38MHZ_DCDC_DCM:
    case EM1_HFRCO_26MHZ_DCDC_DCM:
      break;

    /* DCDC in Low-noise CCM. */
    case EM0_HFXO_40MHZ_WHILE_DCDC_CCM:
    case EM0_HFRCO_38MHZ_PRIME_DCDC_CCM:
    case EM0_HFRCO_38MHZ_WHILE_DCDC_CCM:
    case EM0_HFRCO_38MHZ_COREMARK_DCDC_CCM:
    case EM0_HFRCO_26MHZ_WHILE_DCDC_CCM:
    case EM1_HFXO_40MHZ_DCDC_CCM:
    case EM1_HFRCO_38MHZ_DCDC_CCM:
    case EM1_HFRCO_26MHZ_DCDC_CCM:
      EMU_DCDCConductionModeSet(emuDcdcConductionMode_ContinuousLN, true);
      break;

    /* DCDC in Low-power mode.
       NOTE: DCDC will automatically switch to Low-power mode in EM2. */
    case EM2_LFXO_RTCC_DCDC_LPM:
    case EM2_LFRCO_RTCC_DCDC_LPM:
      break;

    /* DCDC enabled. */
    case EM3_ULFRCO_CRYO_DCDC:
    case EM4H_LFXO_RTCC_DCDC:
    case EM4H_ULFRCO_CRYO_DCDC:
    case EM4H_DCDC:
    case EM4S_DCDC:
      break;

    default:
      EFM_ASSERT(false);
      break;
  }

  /* Configure clocks and enter selected energy mode */
  switch (mode)
  {
    case EM0_HFXO_40MHZ_WHILE:
#if defined(ADVANCED_LOWPOWER_FEATURES)
    case EM0_HFXO_40MHZ_WHILE_LP:
#endif
    case EM0_HFXO_40MHZ_WHILE_DCDC_DCM:
    case EM0_HFXO_40MHZ_WHILE_DCDC_CCM:
      em_EM0_Hfxo();
      while(1);
    case EM0_HFRCO_38MHZ_PRIME:
#if defined(ADVANCED_LOWPOWER_FEATURES)
    case EM0_HFRCO_38MHZ_PRIME_LP:
#endif
    case EM0_HFRCO_38MHZ_PRIME_DCDC_DCM:
    case EM0_HFRCO_38MHZ_PRIME_DCDC_CCM:
      em_EM0_Hfrco(cmuHFRCOFreq_38M0Hz);
      primeCalc();
      break;

    case EM0_HFRCO_38MHZ_WHILE:
#if defined(ADVANCED_LOWPOWER_FEATURES)
    case EM0_HFRCO_38MHZ_WHILE_LP:
#endif
    case EM0_HFRCO_38MHZ_WHILE_DCDC_DCM:
    case EM0_HFRCO_38MHZ_WHILE_DCDC_CCM:
      em_EM0_Hfrco(cmuHFRCOFreq_38M0Hz);
      while(1);
    case EM0_HFRCO_38MHZ_COREMARK:
#if defined(ADVANCED_LOWPOWER_FEATURES)
    case EM0_HFRCO_38MHZ_COREMARK_LP:
#endif
    case EM0_HFRCO_38MHZ_COREMARK_DCDC_DCM:
    case EM0_HFRCO_38MHZ_COREMARK_DCDC_CCM:
      em_EM0_Hfrco(cmuHFRCOFreq_38M0Hz);
      CMU_ClockEnable(cmuClock_HFPER, true); /* enable timer for CoreMark */
      while(1){
        CoreMark_Main();
      }
    case EM0_HFRCO_26MHZ_WHILE:
#if defined(ADVANCED_LOWPOWER_FEATURES)
    case EM0_HFRCO_26MHZ_WHILE_LP:
#endif
    case EM0_HFRCO_26MHZ_WHILE_DCDC_DCM:
    case EM0_HFRCO_26MHZ_WHILE_DCDC_CCM:
      em_EM0_Hfrco(cmuHFRCOFreq_26M0Hz);
      while(1);
    case EM0_HFRCO_1MHZ_WHILE:
#if defined(ADVANCED_LOWPOWER_FEATURES)
    case EM0_HFRCO_1MHZ_WHILE_LP:
#endif
      em_EM0_Hfrco(cmuHFRCOFreq_1M0Hz);
      while(1);
    case EM1_HFXO_40MHZ:
#if defined(ADVANCED_LOWPOWER_FEATURES)
    case EM1_HFXO_40MHZ_LP:
#endif
    case EM1_HFXO_40MHZ_DCDC_DCM:
    case EM1_HFXO_40MHZ_DCDC_CCM:
      em_EM1_Hfxo();
      break;

    case EM1_HFRCO_38MHZ:
#if defined(ADVANCED_LOWPOWER_FEATURES)
    case EM1_HFRCO_38MHZ_LP:
#endif
    case EM1_HFRCO_38MHZ_DCDC_DCM:
    case EM1_HFRCO_38MHZ_DCDC_CCM:
      em_EM1_Hfrco(cmuHFRCOFreq_38M0Hz);
      break;

    case EM1_HFRCO_26MHZ:
#if defined(ADVANCED_LOWPOWER_FEATURES)
    case EM1_HFRCO_26MHZ_LP:
#endif
    case EM1_HFRCO_26MHZ_DCDC_DCM:
    case EM1_HFRCO_26MHZ_DCDC_CCM:
      em_EM1_Hfrco(cmuHFRCOFreq_26M0Hz);
      break;

    case EM1_HFRCO_1MHZ:
#if defined(ADVANCED_LOWPOWER_FEATURES)
    case EM1_HFRCO_1MHZ_LP:
#endif
      em_EM1_Hfrco(cmuHFRCOFreq_1M0Hz);
      break;

    case EM2_LFXO_RTCC:
    case EM2_LFXO_RTCC_DCDC_LPM:
      em_EM2_RTCC(cmuSelect_LFXO, false); /* Enable full RAM power-down */
      break;

    case EM2_LFRCO_RTCC:
    case EM2_LFRCO_RTCC_DCDC_LPM:
      em_EM2_RTCC(cmuSelect_LFRCO, true); /* Enable full RAM power-down */
      break;

    case EM3_ULFRCO_CRYO:
    case EM3_ULFRCO_CRYO_DCDC:
      em_EM3_UlfrcoCRYO(); /* Full RAM */
      break;

    case EM4H_LFXO_RTCC:
    case EM4H_LFXO_RTCC_DCDC:
      em_EM4H_LfxoRTCC(); /* 128b RAM */
      break;

    case EM4H_ULFRCO_CRYO:
    case EM4H_ULFRCO_CRYO_DCDC:
      em_EM4H_UlfrcoCRYO(); /* 128b RAM */
      break;

    case EM4H:
    case EM4H_DCDC:
      em_EM4H(); /* 128b RAM */
      break;

    case EM4S:
    case EM4S_DCDC:
      em_EM4S(); /* No RAM */
      break;

    default:
      EFM_ASSERT(false);
      break;
  }

  /* Should not be reached. */
  EFM_ASSERT(false);
}
