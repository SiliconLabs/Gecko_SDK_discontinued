/**************************************************************************//**
 * @file emodes.c
 * @brief Pearl Gecko energy mode setups (See Data Sheet Table 4.4-4.6)
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

#include "emodes.h"
#include "em_cmu.h"
#include "em_rtcc.h"
#include "em_cryotimer.h"
#include "em_emu.h"
#include <stdint.h>
#include <stdbool.h>

static void disableClocks(void);

/***************************************************************************//**
 * @brief
 *   Enter EM0 with HFXO running at 38.4MHz.
 *
 * @details
 *   Parameter:
 *     EM0. Active Mode. All peripherals disabled.@n
 *   Condition:
 *     38.4 MHz HFXO, all peripheral clocks disabled.@n
 *
 * @note
 *   To better understand disabling clocks and oscillators for specific modes,
 *   see Reference Manual section EMU-Energy Management Unit and Table 9.2.
 ******************************************************************************/
void em_EM0_Hfxo(void)
{
  // Make sure clocks are disabled.
  disableClocks();

  // Set HFXO for HF clock.
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);

  // Make sure unwanted oscillators are disabled specifically for EM0 and HFXO.
  CMU_OscillatorEnable(cmuOsc_HFRCO, false, true);
  CMU_OscillatorEnable(cmuOsc_LFXO, false, true);
  CMU_OscillatorEnable(cmuOsc_LFRCO, false, true);
  CMU_OscillatorEnable(cmuOsc_AUXHFRCO, false, true);
}

/***************************************************************************//**
 * @brief
 *   Enter EM0 with HFRCO running at desired frequency.
 *
 * @param[in] band
 *   HFRCO frequency to activate (38, 32, 26, 19, 16, 13, 7, 4, 2, 1 MHz).
 *
 * @details
 *   Parameter:
 *     EM0. Active Mode. All peripherals disabled.@n
 *   Condition:
 *     Between 38 and 1 MHz HFRCO, all peripheral clocks disabled.@n
 *
 * @note
 *   To better understand disabling clocks and oscillators for specific modes,
 *   see Reference Manual section EMU-Energy Management Unit and Table 9.2.
 ******************************************************************************/
void em_EM0_Hfrco(CMU_HFRCOFreq_TypeDef band)
{
  // Make sure clocks are disabled.
  disableClocks();

  // Set HFRCO for HF clock.
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFRCO);

  // Set HFRCO frequency.
  CMU_HFRCOBandSet(band);

  // Make sure unwanted oscillators are disabled specifically for EM0 and HFRCO.
  CMU_OscillatorEnable(cmuOsc_HFXO, false, true);
  CMU_OscillatorEnable(cmuOsc_LFXO, false, true);
  CMU_OscillatorEnable(cmuOsc_LFRCO, false, true);
  CMU_OscillatorEnable(cmuOsc_AUXHFRCO, false, true);
}

/***************************************************************************//**
 * @brief
 *   Enter EM1 with HFXO running at 38.4MHz.
 *
 * @details
 *   Parameter:
 *     EM1. Sleep Mode. All peripherals disabled.@n
 *   Condition:
 *     38.4 MHz HFXO, all peripheral clocks disabled.@n
 *
 * @note
 *   To better understand disabling clocks and oscillators for specific modes,
 *   see Reference Manual section EMU-Energy Management Unit and Table 9.2.
 ******************************************************************************/
void em_EM1_Hfxo(void)
{
  // Make sure clocks are disabled.
  disableClocks();

  // Set HFXO for HF clock.
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);

  // Make sure unwanted oscillators are disabled specifically for EM1 and HFXO.
  CMU_OscillatorEnable(cmuOsc_HFRCO, false, true);
  CMU_OscillatorEnable(cmuOsc_LFXO, false, true);
  CMU_OscillatorEnable(cmuOsc_LFRCO, false, true);
  CMU_OscillatorEnable(cmuOsc_AUXHFRCO, false, true);

  // Enter EM1.
  EMU_EnterEM1();
}

/***************************************************************************//**
 * @brief
 *   Enter EM1 with HFRCO running at desired frequency.
 *
 * @param[in] band
 *   HFRCO frequency to activate (38, 32, 26, 19, 16, 13, 7, 4, 2, 1 MHz).
 *
 * @details
 *   Parameter:
 *     EM1. Sleep Mode. All peripherals disabled.@n
 *   Condition:
 *     Between 38 and 1 MHz HFRCO, all peripheral clocks disabled.@n
 *
 * @note
 *   To better understand disabling clocks and oscillators for specific modes,
 *   see Reference Manual section EMU-Energy Management Unit and Table 9.2.
 ******************************************************************************/
void em_EM1_Hfrco(CMU_HFRCOFreq_TypeDef band)
{
  // Make sure clocks are disabled.
  disableClocks();

  // Set HFRCO for HF clock.
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFRCO);

  // Set HFRCO frequency.
  CMU_HFRCOBandSet(band);

  // Make sure unwanted oscillators are disabled specifically for EM1 and HFRCO.
  CMU_OscillatorEnable(cmuOsc_HFXO, false, true);
  CMU_OscillatorEnable(cmuOsc_LFXO, false, true);
  CMU_OscillatorEnable(cmuOsc_LFRCO, false, true);
  CMU_OscillatorEnable(cmuOsc_AUXHFRCO, false, true);

  // Enter EM1.
  EMU_EnterEM1();
}

/***************************************************************************//**
 * @brief
 *   Enter EM2 with RTCC running on a low frequency oscillator.
 *
 * @param[in] osc
 *   Oscillator to run RTCC from (LFXO or LFRCO).
 * @param[in] powerdownRam
 *   Powerdown from full RAM to 4kB RAM if true, else retain full RAM.
 *
 * @details
 *   Parameter:
 *     EM2. Deep Sleep Mode.@n
 *   Condition:
 *     RTCC, 32.768 kHz LFXO or LFRCO.@n
 *
 * @note
 *   To better understand disabling clocks and oscillators for specific modes,
 *   see Reference Manual section EMU-Energy Management Unit and Table 9.2.
 ******************************************************************************/
void em_EM2_RTCC(CMU_Select_TypeDef osc, bool powerdownRam)
{
  // Make sure clocks are disabled.
  disableClocks();

  // Route desired oscillator to LFE to run RTCC.
  CMU_ClockSelectSet(cmuClock_LFE, osc);
  CMU_ClockEnable(cmuClock_RTCC, true);

  // Enable clock to the interface with low energy modules.
  CMU_ClockEnable(cmuClock_HFLE, true);

  // Setup RTC parameters
  RTCC_Init_TypeDef rtccInit = RTCC_INIT_DEFAULT;
  rtccInit.presc = rtccCntPresc_256;
  rtccInit.cntWrapOnCCV1 = true;

  // Initialize RTCC. Configure RTCC with prescaler 256.
  RTCC_Init(&rtccInit);

  // Power down all RAM blocks except block 1
  if(powerdownRam)
  {
    EMU_RamPowerDown(SRAM_BASE, 0);
  }

  // Make sure unwanted oscillators are disabled specifically for EM2 and osc.
  if(osc == cmuSelect_LFXO)
  {
    CMU_OscillatorEnable(cmuOsc_LFRCO, false, true);
  }
  else
  {
    CMU_OscillatorEnable(cmuOsc_LFXO, false, true);
  }

  // Enter EM2.
  EMU_EnterEM2(false);
}

/***************************************************************************//**
 * @brief
 *   Enter EM3 with CRYOTIMER running on ULFRCO.
 *
 * @details
 *   Parameter:
       EM3. Stop Mode.@n
 *   Condition:
 *     CRYOTIMER, Full RAM, 1kHz ULFRCO.@n
 *
 * @note
 *   To better understand disabling clocks and oscillators for specific modes,
 *   see Reference Manual section EMU-Energy Management Unit and Table 9.2.
 ******************************************************************************/
void em_EM3_UlfrcoCRYO()
{
  // High and low frequency clocks are disabled in EM3.

  // Setup CRYOTIMER parameters
  CRYOTIMER_Init_TypeDef cryotimerInit = CRYOTIMER_INIT_DEFAULT;

  // Initialize CRYOTIMER. Configure CRYOTIMER to run from ULFRCO
  cryotimerInit.osc = cryotimerOscULFRCO;
  cryotimerInit.presc = cryotimerPresc_128;
  cryotimerInit.period = cryotimerPeriod_256;
  CRYOTIMER_Init(&cryotimerInit);

  // Make sure unwanted oscillators are disabled specifically for EM2 and ULFRCO.
  CMU_OscillatorEnable(cmuOsc_LFXO, false, true);
  CMU_OscillatorEnable(cmuOsc_LFRCO, false, true);

  // Enter EM3.
  EMU_EnterEM3(false);
}

/***************************************************************************//**
 * @brief
 *   Enter EM4H with RTCC running on a LFXO.
 *
 * @details
 *   Parameter:
 *     EM4H. Hibernate Mode.@n
 *   Condition:
       RTCC, 128 byte RAM, 32.768 kHz LFXO.@n
 *
 * @note
 *   To better understand disabling clocks and oscillators for specific modes,
 *   see Reference Manual section EMU-Energy Management Unit and Table 9.2.
 ******************************************************************************/
void em_EM4H_LfxoRTCC(void)
{
  // Make sure clocks are disabled.
  disableClocks();

  // Route the LFXO clock to RTCC.
  CMU_ClockSelectSet(cmuClock_LFE, cmuSelect_LFXO);
  CMU_ClockEnable(cmuClock_RTCC, true);

  // Enable clock to the interface with low energy modules.
  CMU_ClockEnable(cmuClock_HFLE, true);

  // Setup RTC parameters.
  RTCC_Init_TypeDef rtccInit = RTCC_INIT_DEFAULT;
  rtccInit.presc = rtccCntPresc_1;
  rtccInit.cntWrapOnCCV1 = true;
  rtccInit.debugRun = true;

  // Initialize RTCC. Configure RTCC with prescaler 1.
  RTCC_Init(&rtccInit);

  // Make sure unwanted oscillators are disabled specifically for EM4H and LFXO.
  CMU_OscillatorEnable(cmuOsc_LFRCO, false, true);

  // EM4H retains 128 byte RAM through RTCC by default.

  // Enter EM4H.
  EMU_EM4Init_TypeDef em4Init = EMU_EM4INIT_DEFAULT;
  em4Init.em4State = emuEM4Hibernate;
  em4Init.retainLfxo = true;
  em4Init.pinRetentionMode = emuPinRetentionLatch;
  EMU_EM4Init(&em4Init);
  EMU_EnterEM4();
}

/***************************************************************************//**
 * @brief
 *   Enter EM4H with CRYOTIMER running on a ULFRCO.
 *
 * @details
 *   Parameter:
       EM4H. Hibernate Mode.@n
 *   Condition:
 *     CRYOTIMER, 128 byte RAM, 1kHz ULFRCO.@n
 *
 * @note
 *   To better understand disabling clocks and oscillators for specific modes,
 *   see Reference Manual section EMU-Energy Management Unit and Table 9.2.
 ******************************************************************************/
void em_EM4H_UlfrcoCRYO(void)
{
  // Make sure clocks are disabled.
  disableClocks();

  // ULFRCO is always running from EM0-EM4H.

  // Setup CRYOTIMER parameters
  CRYOTIMER_Init_TypeDef cryotimerInit = CRYOTIMER_INIT_DEFAULT;
  cryotimerInit.osc = cryotimerOscULFRCO;
  cryotimerInit.presc = cryotimerPresc_128;
  cryotimerInit.period = cryotimerPeriod_256;

  // Initialize CRYOTIMER. Configure CRYOTIMER to run from ULFRCO.
  CRYOTIMER_Init(&cryotimerInit);

  // Make sure unwanted oscillators are disabled specifically for EM4H and ULFRCO.
  CMU_OscillatorEnable(cmuOsc_LFXO, false, true);
  CMU_OscillatorEnable(cmuOsc_LFRCO, false, true);

  // EM4H retains 128 byte RAM through RTCC by default.

  // Enter EM4H.
  EMU_EM4Init_TypeDef em4Init = EMU_EM4INIT_DEFAULT;
  em4Init.em4State = emuEM4Hibernate;
  em4Init.pinRetentionMode = emuPinRetentionLatch;
  EMU_EM4Init(&em4Init);
  EMU_EnterEM4();
}

/***************************************************************************//**
 * @brief
 *   Enter EM4H without RTCC or CRYOTIMER.
 *
 * @details
 *   Parameter:
       EM4H. Hibernate Mode.@n
 *   Condition:
 *     No RTCC or CRYOTIMER, 128 byte RAM.@n
 *
 * @note
 *   To better understand disabling clocks and oscillators for specific modes,
 *   see Reference Manual section EMU-Energy Management Unit and Table 9.2.
 ******************************************************************************/
void em_EM4H(void)
{
  // Make sure clocks are disabled.
  disableClocks();

  // Make sure unwanted oscillators are disabled specifically for EM2 and ULFRCO.
  CMU_OscillatorEnable(cmuOsc_LFXO, false, true);
  CMU_OscillatorEnable(cmuOsc_LFRCO, false, true);

  // EM4H retains 128 byte RAM through RTCC by default.

  // Enter EM4H.
  EMU_EM4Init_TypeDef em4Init = EMU_EM4INIT_DEFAULT;
  em4Init.em4State = emuEM4Hibernate;
  em4Init.pinRetentionMode = emuPinRetentionLatch;
  EMU_EM4Init(&em4Init);
  EMU_EnterEM4();
}

/***************************************************************************//**
 * @brief
 *   Enter EM4S without RTCC or CRYOTIMER.
 *
 * @details
 *   Parameter:
       EM4S. Shutoff Mode.@n
 *   Condition:
 *     No RTCC or CRYOTIMER, no RAM.@n
 *
 * @note
 *   To better understand disabling clocks and oscillators for specific modes,
 *   see Reference Manual section EMU-Energy Management Unit and Table 9.2.
 ******************************************************************************/
void em_EM4S(void)
{
  // Make sure clocks are disabled.
  disableClocks();

  // Make sure unwanted oscillators are disabled specifically for EM2 and ULFRCO.
  CMU_OscillatorEnable(cmuOsc_LFXO, false, true);
  CMU_OscillatorEnable(cmuOsc_LFRCO, false, true);

  // No RTCC or RAM retention in EM4S by default.

  // Enter EM4S.
  EMU_EM4Init_TypeDef em4Init = EMU_EM4INIT_DEFAULT;
  em4Init.em4State = emuEM4Shutoff;
  em4Init.pinRetentionMode = emuPinRetentionLatch;
  EMU_EM4Init(&em4Init);
  EMU_EnterEM4();
}

/**************************************************************************//**
 * @brief   Disable high frequency clocks
 *****************************************************************************/
static void disableHFClocks(void)
{
  // Disable High Frequency Peripheral Clocks
  CMU_ClockEnable(cmuClock_HFPER, false);
  CMU_ClockEnable(cmuClock_USART0, false);
  CMU_ClockEnable(cmuClock_USART1, false);
  CMU_ClockEnable(cmuClock_TIMER0, false);
  CMU_ClockEnable(cmuClock_TIMER1, false);
  CMU_ClockEnable(cmuClock_CRYOTIMER, false);
  CMU_ClockEnable(cmuClock_ACMP0, false);
  CMU_ClockEnable(cmuClock_ACMP1, false);
  CMU_ClockEnable(cmuClock_IDAC0, false);
  CMU_ClockEnable(cmuClock_ADC0, false);
  CMU_ClockEnable(cmuClock_I2C0, false);

  // Disable High Frequency Bus Clocks
  CMU_ClockEnable(cmuClock_CRYPTO, false);
  CMU_ClockEnable(cmuClock_LDMA, false);
  CMU_ClockEnable(cmuClock_GPCRC, false);
  CMU_ClockEnable(cmuClock_GPIO, false);
  CMU_ClockEnable(cmuClock_HFLE, false);
  CMU_ClockEnable(cmuClock_PRS, false);
}

/**************************************************************************//**
 * @brief   Disable low frequency clocks
 *****************************************************************************/
static void disableLFClocks(void)
{
  // Enable LFXO for Low Frequency Clock Disables
  CMU_OscillatorEnable(cmuOsc_LFXO, true, true);

  // Disable Low Frequency A Peripheral Clocks
  // Note: LFA clock must be sourced before modifying peripheral clock enables
  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);
  CMU_ClockEnable(cmuClock_LETIMER0, false);
  CMU_ClockEnable(cmuClock_PCNT0, false);
  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_Disabled);

  // Disable Low Frequency B Peripheral Clocks
  // Note: LFB clock must be sourced before modifying peripheral clock enables
  CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFXO);
  CMU_ClockEnable(cmuClock_LEUART0, false);
  CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_Disabled);

  // Disable Low Frequency E Peripheral Clocks
  // Note: LFE clock must be sourced before modifying peripheral clock enables
  CMU_ClockSelectSet(cmuClock_LFE, cmuSelect_LFXO);
  CMU_ClockEnable(cmuClock_RTCC, false);
  CMU_ClockSelectSet(cmuClock_LFE, cmuSelect_Disabled);

  // Disable Low Frequency Oscillator
  CMU_OscillatorEnable(cmuOsc_LFXO, false, true);
}

/**************************************************************************//**
 * @brief   Disable all clocks to achieve lowest current consumption numbers.
 *****************************************************************************/
static void disableClocks(void)
{
  // Disable High Frequency Clocks
  disableHFClocks();

  // Disable Low Frequency Clocks
  disableLFClocks();
}

/**************************************************************************//**
 * @brief   Calculate primes.
 *****************************************************************************/
void primeCalc(void)
{
  uint32_t i, d, n;
  uint32_t primes[64];

  // Find prime numbers forever.
  while (1)
  {
    primes[0] = 1;
    for (i = 1; i < 64;)
    {
      for (n = primes[i - 1] + 1;; n++)
      {
        for (d = 2; d <= n; d++)
        {
          if (n == d)
          {
            primes[i] = n;
            goto nexti;
          }
          if (n % d == 0)
            break;
        }
      }
      nexti:
      i++;
    }
  }
}
