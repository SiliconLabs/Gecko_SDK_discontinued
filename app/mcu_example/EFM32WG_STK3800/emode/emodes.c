/**************************************************************************//**
 * @file emodes.c
 * @brief Wonder Gecko energy mode setups (See Data Sheet Table 3.4)
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
#include "em_rtc.h"
#include "em_emu.h"
#include <stdint.h>
#include <stdbool.h>

static void disableClocks(void);
static void primeCalc(void);

/***************************************************************************//**
 * @brief
 *   Enter EM0 with HFXO running at 48MHz.
 *
 * @details
 *   Parameter:
 *     EM0. No prescaling. Running prime number calculation code
 *     from Flash (Production test condition = 14MHz).@n
 *   Condition:
 *     48 MHz HFXO, all peripheral clocks disabled, VDD= 3.0V.@n
 *
 * @note
 *   To better understand disabling clocks and oscillators for specific modes,
 *   see Reference Manual section EMU-Energy Management Unit and Table 10.1.
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

  // Calculate prime numbers forever.
  primeCalc();
}

/***************************************************************************//**
 * @brief
 *   Enter EM0 with HFRCO running at desired frequency.
 *
 * @param[in] band
 *   HFRCO band to activate (28, 21, 14, 11, 6.6, 1.2 MHz).
 *
 * @details
 *   Parameter:
 *     EM0. No prescaling. Running prime number calculation code
 *     from Flash (Production test condition = 14MHz).@n
 *   Condition:
 *     Between 28 and 1.2 MHz HFRCO, all peripheral clocks disabled, VDD= 3.0V.@n
 *
 * @note
 *   To better understand disabling clocks and oscillators for specific modes,
 *   see Reference Manual section EMU-Energy Management Unit and Table 10.1.
 ******************************************************************************/
void em_EM0_Hfrco(CMU_HFRCOBand_TypeDef band)
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

  // Calculate prime numbers forever.
  primeCalc();
}

/***************************************************************************//**
 * @brief
 *   Enter EM1 with HFXO running at 48MHz.
 *
 * @details
 *   Parameter:
       EM1 (Production test condition = 14 MHz).@n
 *   Condition:
 *     48 MHz HFXO, all peripheral clocks disabled, VDD= 3.0 V.@n
 *
 * @note
 *   To better understand disabling clocks and oscillators for specific modes,
 *   see Reference Manual section EMU-Energy Management Unit and Table 10.1.
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
 *   HFRCO band to activate (28, 21, 14, 11, 6.6, 1.2 MHz).
 *
 * @details
 *   Parameter:
 *     EM1 (Production test condition = 14 MHz).@n
 *   Condition:
 *     Between 28 and 1.2 MHz HFRCO, all peripheral clocks disabled, VDD= 3.0 V.@n
 *
 * @note
 *   To better understand disabling clocks and oscillators for specific modes,
 *   see Reference Manual section EMU-Energy Management Unit and Table 10.1.
 ******************************************************************************/
void em_EM1_Hfrco(CMU_HFRCOBand_TypeDef band)
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
 *   Enter EM2 with RTC running with LFRCO.
 *
 * @details
 *   Parameter:
       EM2.@n
 *   Condition:
 *     RTC prescaled to 1 Hz, 32.768 kHz LFRCO, VDD= 3.0 V.@n
 *
 * @note
 *   To better understand disabling clocks and oscillators for specific modes,
 *   see Reference Manual section EMU-Energy Management Unit and Table 10.1.
 ******************************************************************************/
void em_EM2_LfrcoRTC(void)
{
  // Make sure clocks are disabled.
  disableClocks();

  // Route the LFRCO clock to RTC.
  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFRCO);
  CMU_ClockEnable(cmuClock_RTC, true);

  // Configure RTC to 1Hz.
  CMU_ClockDivSet(cmuClock_RTC, cmuClkDiv_32768);

  // Enable clock to the interface with low energy modules.
  CMU_ClockEnable(cmuClock_CORELE, true);

  // Setup RTC parameters.
  RTC_Init_TypeDef rtcInit = RTC_INIT_DEFAULT;

  // Initialize RTC.
  RTC_Init(&rtcInit);

  // Make sure unwanted oscillators are disabled specifically for EM2 and LFRCO.
  CMU_OscillatorEnable(cmuOsc_LFXO, false, true);

  // Enter EM2.
  EMU_EnterEM2(false);
}

/***************************************************************************//**
 * @brief
 *   Enter EM3.
 *
 * @details
 *   Parameter:
       EM3.@n
 *   Condition:
 *     VDD= 3.0 V@n
 *
 * @note
 *   To better understand disabling clocks and oscillators for specific modes,
 *   see Reference Manual section EMU-Energy Management Unit and Table 10.1.
 ******************************************************************************/
void em_EM3(void)
{
  // High and low frequency clocks are disabled in EM3.

  // All unwanted oscillators are disabled in EM3.

  // Enter EM3.
  EMU_EnterEM3(false);
}

/***************************************************************************//**
 * @brief
 *   Enter EM4.
 *
 * @details
 *   Parameter:
       EM4.
 *   Condition:
 *     VDD= 3.0 V.
 *
 * @note
 *   To better understand disabling clocks and oscillators for specific modes,
 *   see Reference Manual section EMU-Energy Management Unit and Table 10.1.
 ******************************************************************************/
void em_EM4(void)
{
  // High and low frequency clocks are disabled in EM4.

  // All unwanted oscillators are disabled in EM4.

  // Enter EM4.
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
  CMU_ClockEnable(cmuClock_USART2, false);
  CMU_ClockEnable(cmuClock_UART0, false);
  CMU_ClockEnable(cmuClock_UART1, false);
  CMU_ClockEnable(cmuClock_TIMER0, false);
  CMU_ClockEnable(cmuClock_TIMER1, false);
  CMU_ClockEnable(cmuClock_TIMER2, false);
  CMU_ClockEnable(cmuClock_TIMER3, false);
  CMU_ClockEnable(cmuClock_ACMP0, false);
  CMU_ClockEnable(cmuClock_ACMP1, false);
  CMU_ClockEnable(cmuClock_PRS, false);
  CMU_ClockEnable(cmuClock_DAC0, false);
  CMU_ClockEnable(cmuClock_GPIO, false);
  CMU_ClockEnable(cmuClock_VCMP, false);
  CMU_ClockEnable(cmuClock_ADC0, false);
  CMU_ClockEnable(cmuClock_I2C0, false);
  CMU_ClockEnable(cmuClock_I2C1, false);

  // Disable High Frequency Core/Bus Clocks
  CMU_ClockEnable(cmuClock_AES, false);
  CMU_ClockEnable(cmuClock_DMA, false);
  CMU_ClockEnable(cmuClock_HFLE, false);
  CMU_ClockEnable(cmuClock_EBI, false);
  CMU_ClockEnable(cmuClock_USB, false);
  
  // Disable USBC clock by choosing unused oscillator (LFXO)
  CMU_ClockEnable(cmuClock_USBC, true);
  CMU_OscillatorEnable(cmuOsc_LFXO, true, true);
  CMU_ClockSelectSet(cmuClock_USBC, cmuSelect_LFXO);
  CMU_OscillatorEnable(cmuOsc_LFXO, true, false);
  CMU_ClockEnable(cmuClock_USBC, false);
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
  CMU_ClockEnable(cmuClock_RTC, false);
  CMU_ClockEnable(cmuClock_LETIMER0, false);
  CMU_ClockEnable(cmuClock_LCD, false);
  CMU_ClockEnable(cmuClock_PCNT0, false);
  CMU_ClockEnable(cmuClock_PCNT1, false);
  CMU_ClockEnable(cmuClock_PCNT2, false);
  CMU_ClockEnable(cmuClock_LESENSE, false);
  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_Disabled);

  // Disable Low Frequency B Peripheral Clocks
  // Note: LFB clock must be sourced before modifying peripheral clock enables
  CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFXO);
  CMU_ClockEnable(cmuClock_LEUART0, false);
  CMU_ClockEnable(cmuClock_LEUART1, false);
  CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_Disabled);

  // Disable Low Frequency Oscillator
  CMU_OscillatorEnable(cmuOsc_LFXO, false, true);
}

/**************************************************************************//**
 * @brief   Disable all clocks to achieve lowest current consumption numbers.
 *****************************************************************************/
static void disableClocks(void)
{
  // Disable DBG clock by selecting AUXHFRCO
  CMU_ClockSelectSet(cmuClock_DBG, cmuSelect_AUXHFRCO);

  // Disable High Frequency Clocks
  disableHFClocks();

  // Disable Low Frequency Clocks
  disableLFClocks();
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
