/**************************************************************************//**
 * @file emodes.c
 * @brief Zero Gecko energy mode setups (See Data Sheet Table 3.3)
 * @version 4.4.0
 ******************************************************************************
 * @section License
 * <b>Copyright 2015 Silicon Labs, Inc. http://www.silabs.com</b>
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
#include "em_emu.h"
#include <stdint.h>
#include <stdbool.h>

static void primeCalc(void);

/***************************************************************************//**
 * @brief
 *   Enter EM0 with HFXO running at 40 MHz.
 *
 * @details
 *   Parameter:
 *     EM0. No prescaling. Running prime number calculation code
 *     from Flash (Production test condition = 19MHz).@n
 *   Condition:
 *     40 MHz HFXO, all peripheral clocks disabled, VDD= 3.3V.@n
 *
 * @note
 *   To better understand disabling clocks and oscillators for specific modes,
 *   see Reference Manual section EMU-Energy Management Unit and Table 9.1.
 ******************************************************************************/
void em_EM0_Hfxo(void)
{
  // Make sure clocks are disabled specifically for EM0.
  CMU ->HFPERCLKEN0 = 0x00000000;
  CMU ->HFBUSCLKEN0 = 0x00000000;
  CMU ->LFACLKEN0 = 0x00000000;
  CMU ->LFBCLKEN0 = 0x00000000;
  CMU ->LFECLKEN0 = 0x00000000;

  // Set HFXO for HF clock.
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);

  // Make sure unwanted oscillators are disabled specifically for EM0 and HFXO.
  CMU_OscillatorEnable(cmuOsc_HFRCO, false, true);
  CMU_OscillatorEnable(cmuOsc_LFXO, false, true);
  CMU_OscillatorEnable(cmuOsc_LFRCO, false, true);
  CMU_OscillatorEnable(cmuOsc_AUXHFRCO, false, true);

  // Calculate prime numbers forever.
  primeCalc();
}

/***************************************************************************//**
 * @brief
 *   Enter EM0 with HFRCO running at desired frequency.
 *
 * @param[in] band
 *   HFRCO band to activate (38, 32, 26, 19, 16 MHz).
 *
 * @details
 *   Parameter:
 *     EM0. No prescaling. Running prime number calculation code
 *     from Flash (Production test condition = 19 MHz).@n
 *   Condition:
 *     Between 38 and 1 MHz HFRCO, all peripheral clocks disabled, VDD= 3.3 V.@n
 *
 * @note
 *   To better understand disabling clocks and oscillators for specific modes,
 *   see Reference Manual section EMU-Energy Management Unit and Table 9.1.
 ******************************************************************************/
void em_EM0_Hfrco(CMU_HFRCOFreq_TypeDef band)
{
  // Make sure clocks are disabled specifically for EM0.
  CMU ->HFPERCLKEN0 = 0x00000000;
  CMU ->HFBUSCLKEN0 = 0x00000000;
  CMU ->LFACLKEN0 = 0x00000000;
  CMU ->LFBCLKEN0 = 0x00000000;
  CMU ->LFECLKEN0 = 0x00000000;

  // Set HFRCO for HF clock.
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFRCO);

  // Set HFRCO frequency.
  CMU_HFRCOBandSet(band);

  // Make sure unwanted oscillators are disabled specifically for EM0 and HFRCO.
  CMU_OscillatorEnable(cmuOsc_HFXO, false, true);
  CMU_OscillatorEnable(cmuOsc_LFXO, false, true);
  CMU_OscillatorEnable(cmuOsc_LFRCO, false, true);
  CMU_OscillatorEnable(cmuOsc_AUXHFRCO, false, true);
#if defined(CMU_OSCENCMD_USHFRCOEN)
  CMU_OscillatorEnable(cmuOsc_USHFRCO, false, true);
#endif

  // Calculate prime numbers forever.
  primeCalc();
}

/***************************************************************************//**
 * @brief
 *   Enter EM1 with HFXO running at 40 MHz.
 *
 * @details
 *   Parameter:
 *     EM1 (Production test condition = 19 MHz).@n
 *   Condition:
 *     40 MHz HFXO, all peripheral clocks disabled, VDD= 3.3 V.@n
 *
 * @note
 *   To better understand disabling clocks and oscillators for specific modes,
 *   see Reference Manual section EMU-Energy Management Unit and Table 9.1.
 ******************************************************************************/
void em_EM1_Hfxo(void)
{
  // Make sure clocks are disabled specifically for EM1.
  CMU ->HFPERCLKEN0 = 0x00000000;
  CMU ->HFBUSCLKEN0 = 0x00000000;
  CMU ->LFACLKEN0 = 0x00000000;
  CMU ->LFBCLKEN0 = 0x00000000;
  CMU ->LFECLKEN0 = 0x00000000;

  // Set HFXO for HF clock.
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);

  // Make sure unwanted oscillators are disabled specifically for EM1 and HFXO.
  CMU_OscillatorEnable(cmuOsc_HFRCO, false, true);
  CMU_OscillatorEnable(cmuOsc_LFXO, false, true);
  CMU_OscillatorEnable(cmuOsc_LFRCO, false, true);
  CMU_OscillatorEnable(cmuOsc_AUXHFRCO, false, true);
#if defined(CMU_OSCENCMD_USHFRCOEN)
  CMU_OscillatorEnable(cmuOsc_USHFRCO, false, true);
#endif

  // Enter EM1.
  EMU_EnterEM1();
}

/***************************************************************************//**
 * @brief
 *   Enter EM1 with HFRCO running at desired frequency.
 *
 * @param[in] band
 *   HFRCO band to activate (38, 32, 29, 19, 16 MHz).
 *
 * @details
 *   Parameter:
 *     EM1 (Production test condition = 19 MHz).@n
 *   Condition:
 *     Between38 and 1 MHz HFRCO, all peripheral clocks disabled, VDD= 3.3 V.@n
 *
 * @note
 *   To better understand disabling clocks and oscillators for specific modes,
 *   see Reference Manual section EMU-Energy Management Unit and Table 9.1.
 ******************************************************************************/
void em_EM1_Hfrco(CMU_HFRCOFreq_TypeDef band)
{
  // Make sure clocks are disabled specifically for EM1.
  CMU ->HFPERCLKEN0 = 0x00000000;
  CMU ->HFBUSCLKEN0 = 0x00000000;
  CMU ->LFACLKEN0 = 0x00000000;
  CMU ->LFBCLKEN0 = 0x00000000;
  CMU ->LFECLKEN0 = 0x00000000;

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
 *   Enter EM2 with RTCC running with LFRCO.
 *
 * @details
 *   Parameter:
 *     EM2.@n
 *   Condition:
 *     RTC prescaled to 1 Hz, 32.768 kHz LFRCO, VDD= 3.3 V.@n
 *
 * @note
 *   To better understand disabling clocks and oscillators for specific modes,
 *   see Reference Manual section EMU-Energy Management Unit and Table 9.1.
 ******************************************************************************/
void em_EM2_LfrcoRTCC(void)
{
  // Make sure clocks are disabled specifically for EM2.
  CMU ->HFPERCLKEN0 = 0x00000000;
  CMU ->HFBUSCLKEN0 = 0x00000000;
  CMU ->LFACLKEN0 = 0x00000000;
  CMU ->LFBCLKEN0 = 0x00000000;

  // Route the LFRCO clock to RTCC.
  CMU_ClockSelectSet(cmuClock_LFE, cmuSelect_LFRCO);
  CMU_ClockEnable(cmuClock_RTCC, true);

  // Enable clock to the interface with low energy modules.
  CMU_ClockEnable(cmuClock_HFLE, true);

  // Setup RTC parameters
  RTCC_Init_TypeDef rtccInit = RTCC_INIT_DEFAULT;

  // Initialize RTCC. Configure RTCC counter to 1Hz.
  rtccInit.presc = rtccCntPresc_32768;
  RTCC_Init(&rtccInit);

  // Make sure unwanted oscillators are disabled specifically for EM2 and LFRCO.
  CMU_OscillatorEnable(cmuOsc_LFXO, false, true);

  // Enter EM2.
  EMU_EnterEM2(false);
}

/***************************************************************************//**
 * @brief
 *   Enter EM3 with ULFRCO.
 *
 * @details
 *   Parameter:
 *     EM3.@n
 *   Condition:
 *     ULFRCO enabled, LFRCO/LFXO disabled, VDD= 3.3 V.@n
 *
 * @note
 *   To better understand disabling clocks and oscillators for specific modes,
 *   see Reference Manual section EMU-Energy Management Unit and Table 9.1.
 ******************************************************************************/
void em_EM3_Ulfrco(void)
{
  // High and low frequency clocks are disabled in EM3.

  // ULFRCO is always running from EM0-EM3.

  // All unwanted oscillators are disabled in EM3.

  // Enter EM3.
  EMU_EnterEM3(false);
}

/***************************************************************************//**
 * @brief
 *   Enter EM4S.
 *
 * @details
 *   Parameter:
 *     EM4.@n
 *   Condition:
 *     VDD= 3.3 V.@n
 *
 * @note
 *   To better understand disabling clocks and oscillators for specific modes,
 *   see Reference Manual section EMU-Energy Management Unit and Table 9.1.
 ******************************************************************************/
void em_EM4(void)
{
  // High and low frequency clocks are disabled in EM4H/S.

  // All unwanted oscillators are disabled in EM4S.

  // Enter EM4S.
  EMU_EnterEM4();
}

/**************************************************************************//**
 * @brief   Calculate primes.
 *****************************************************************************/
static void primeCalc(void)
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
