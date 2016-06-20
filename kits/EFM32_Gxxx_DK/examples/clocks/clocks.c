/**************************************************************************//**
 * @file
 * @brief Core clocking example for EFM32_G8xx_DK
 * @details
 *   Allows user to configure CPU Core Clock oscillator and Frequency
 *   with joystick with LCD status, as well as EM0-EM2.
 *
 * @par Usage
 * @li Joystick Up and Down selects core clock oscillator and HFRCO band,
 *   (LFRCO/LFXO/HFRCO/HFXO)
 * @li Joystick Left and Right selects the core clock divisor, in 2^n steps
 * @li Joystick Push selects energy mode
 *
 * @note
 *   This example requires BSP version 1.0.6 or later.
 *
 * @version 4.2.0
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2014 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include <string.h>
#include <stdbool.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_emu.h"
#include "em_cmu.h"
#include "em_dbg.h"
#include "em_gpio.h"
#include "em_lcd.h"
#include "bsp.h"
#include "bsp_trace.h"
#include "segmentlcd.h"

/** Minimum allowed core clock in Hz to inhibit becoming totally unresponsive */
#define CORE_CLOCK_MIN 8000

/** HFClock oscillator selection table */
static const CMU_Select_TypeDef oscSelectorTable[] =
{
  cmuSelect_LFRCO,
  cmuSelect_LFXO,
  cmuSelect_HFRCO,
  cmuSelect_HFXO
};

/** HFClock oscillator selection name table (must match oscSelectorTable) */
static const char *oscSelectorNameTable[] =
{
  "LFRC",
  " LFX",
  "HFRC",
  " HFX",
};

/** HFRCO band selection table */
static const CMU_HFRCOBand_TypeDef bandSelectorTable[] =
{
  cmuHFRCOBand_1MHz,
  cmuHFRCOBand_7MHz,
  cmuHFRCOBand_11MHz,
  cmuHFRCOBand_14MHz,
  cmuHFRCOBand_21MHz,
  cmuHFRCOBand_28MHz
};

/** Core clock DIV factor table */
static const CMU_ClkDiv_TypeDef clockDivTable[] =
{
  cmuClkDiv_1,
  cmuClkDiv_2,
  cmuClkDiv_4,
  cmuClkDiv_8,
  cmuClkDiv_16,
  cmuClkDiv_32,
  cmuClkDiv_64,
  cmuClkDiv_128,
  cmuClkDiv_256,
  cmuClkDiv_512
};

/* Current config variables */
static int oscIndex = 2; /* Set to HW reset default in oscSelectorTable */
static int bandIndex = 3; /* Set to HW reset default in bandSelectorTable */
static int divIndex = 0; /* Set to HW reset default in clockDivTable */
static int eMode = 0;

/* Local prototypes */
void clocksIRQInit(void);
void clocksUpdateLCD(void);

/**************************************************************************//**
 * @brief GPIO Interrupt handler
 *****************************************************************************/
void GPIO_EVEN_IRQHandler(void)
{
  uint16_t joystick;
  uint32_t freq;

  /* Switch to HFRCO high frequency in order to finish interrupt as quickly */
  /* as possible. HFRCO takes almost no time to start if disabled. This will */
  /* cause energy consumption to go up during interrupt, but it improves */
  /* joystick usage. */
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFRCO);
  CMU_HFRCOBandSet(cmuHFRCOBand_28MHz);
  /* Important: Divisor must be 1 for handling towards end of this code */
  CMU_ClockDivSet(cmuClock_CORE, cmuClkDiv_1);

  /* Clear interrupt */
  GPIO_IntClear(1 << BSP_GPIO_INT_PIN);
  BSP_InterruptFlagsClear(BC_INTEN_JOYSTICK);

  /* LEDs on to indicate joystick used */
  BSP_LedsSet(0xffff);

  /* Read and store joystick activity - wait for key release */
  joystick = BSP_JoystickGet();
  while (BSP_JoystickGet());

  /* LEDs off to indicate joystick release */
  BSP_LedsSet(0x0000);

  /* LEFT/RIGHT selects core clock divisor */
  if (joystick & BC_JOYSTICK_LEFT)
  {
    divIndex--;

    if (divIndex < 0)
    {
      divIndex = (sizeof(clockDivTable) / sizeof(CMU_ClkDiv_TypeDef)) - 1;
    }
  }

  if (joystick & BC_JOYSTICK_RIGHT)
  {
    divIndex++;

    if (divIndex >= (int)(sizeof(clockDivTable) / sizeof(CMU_ClkDiv_TypeDef)))
    {
      divIndex = 0;
    }
  }

  /* UP/DOWN selects core oscillator/HFRCO band */
  if (joystick & BC_JOYSTICK_UP)
  {
    /* If HFRCO selected, toggle band */
    if (oscSelectorTable[oscIndex] == cmuSelect_HFRCO)
    {

      bandIndex++;

      if (bandIndex >= (int)(sizeof(bandSelectorTable) / sizeof(CMU_HFRCOBand_TypeDef)))
      {
        bandIndex = 0;
        oscIndex++;
      }
    }
    else
    {
      bandIndex = 0;
      oscIndex++;
    }

    if (oscIndex >= (int)(sizeof(oscSelectorTable) / sizeof(CMU_Select_TypeDef)))
    {
      oscIndex = 0;
    }
  }

  if (joystick & BC_JOYSTICK_DOWN)
  {
    /* If HFRCO selected, toggle band */
    if (oscSelectorTable[oscIndex] == cmuSelect_HFRCO)
    {
      bandIndex--;

      if (bandIndex < 0)
      {
        bandIndex = 5;
        oscIndex--;
      }
    }
    else
    {
      bandIndex = 5;
      oscIndex--;
    }

    if (oscIndex < 0)
    {
      oscIndex = (sizeof(oscSelectorTable) / sizeof(CMU_Select_TypeDef)) - 1;
    }
  }

  /* Push selects EM */
  if (joystick & BC_JOYSTICK_CENTER)
  {
    eMode++;

    if (eMode > 2)
    {
      eMode = 0;
    }
  }

  /* Set to selected core clock */
  CMU_ClockSelectSet(cmuClock_HF, oscSelectorTable[oscIndex]);
  CMU_HFRCOBandSet(bandSelectorTable[bandIndex]);

  /* Divisor must be 1 in HW for below check to work; */
  /* has already been set to 1 at start of this function. */

  /* Make sure not going below min defined core clock in this app. */
  freq = CMU_ClockFreqGet(cmuClock_CORE);
  while (((freq >> divIndex) < CORE_CLOCK_MIN) && divIndex)
  {
    divIndex--;
  }

  /* Disable unused oscillators (not LFRCO - used by LCD) */
  if (oscSelectorTable[oscIndex] != cmuSelect_HFXO)
    CMU_OscillatorEnable(cmuOsc_HFXO, false, false);
  if (oscSelectorTable[oscIndex] != cmuSelect_HFRCO)
    CMU_OscillatorEnable(cmuOsc_HFRCO, false, false);
  if (oscSelectorTable[oscIndex] != cmuSelect_LFXO)
    CMU_OscillatorEnable(cmuOsc_LFXO, false, false);

  /* Set divisor, which may have been auto-adjusted to avoid too low core clock */
  CMU_ClockDivSet(cmuClock_CORE, clockDivTable[divIndex]);
}


/**************************************************************************//**
 * @brief Initialize GPIO interrupt for joystick
 *****************************************************************************/
void clocksIRQInit(void)
{
  /* Configure interrupt pin as input with pull-up */
  GPIO_PinModeSet(BSP_GPIO_INT_PORT, BSP_GPIO_INT_PIN, gpioModeInputPull, 1);

  /* Set falling edge interrupt and clear/enable it */
  GPIO_IntConfig(BSP_GPIO_INT_PORT, BSP_GPIO_INT_PIN, false, true, true);

  NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
  NVIC_EnableIRQ(GPIO_EVEN_IRQn);
}

/**************************************************************************//**
 * @brief Update LCD with selected config
 *****************************************************************************/
void clocksUpdateLCD(void)
{
  char lcdtext[8];
  int i;
  uint32_t freq;
  uint32_t div;

  /* Show Gecko if debugger is attached. Energy modes do not behave as */
  /* expected when using the debugger. */
  if (DBG_Connected())
  {
    SegmentLCD_Symbol(LCD_SYMBOL_GECKO, 1);
  }
  else
  {
    SegmentLCD_Symbol(LCD_SYMBOL_GECKO, 0);
  }

  freq = CMU_ClockFreqGet(cmuClock_CORE);

  /* Selected clock source */
  strcpy(lcdtext, oscSelectorNameTable[oscIndex]);

  /* Choose to display Hz/Khz or Mhz according to speed */
  if (freq < 10000)
  {
    strcat(lcdtext, " Hz");
  }
  else if ((freq >= 10000) && (freq < 10000000))
  {
    freq = freq / 1000;
    strcat(lcdtext, "kHz");
  }
  else if (freq >= 10000000)
  {
    freq = freq / 1000000;
    strcat(lcdtext, "MHz");
  }
  SegmentLCD_Write(lcdtext);

  /* Output clock source and frequency on LCD */
  SegmentLCD_Number(freq);

  /* Light "Ring" according to div factor */
  div = CMU_ClockDivGet(cmuClock_CORE);
  for (i = 0; i < 8; i++)
  {
    if ((int)div > i)
    {
      SegmentLCD_ARing(i, 1);
    }
    else
    {
      SegmentLCD_ARing(i, 0);
    }
  }

  /* Show energy mode selected when idle */
  if (eMode < 1)
  {
    SegmentLCD_EnergyMode(0, 1);
  }
  else
  {
    SegmentLCD_EnergyMode(0, 0);
  }

  if (eMode < 2)
  {
    SegmentLCD_EnergyMode(1, 1);
  }
  else
  {
    SegmentLCD_EnergyMode(1, 0);
  }

  if (eMode < 3)
  {
    SegmentLCD_EnergyMode(2, 1);
  }
  else
  {
    SegmentLCD_EnergyMode(2, 0);
  }

  SegmentLCD_EnergyMode(3, 1);
  SegmentLCD_EnergyMode(4, 1);
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  /* Chip revision alignment and errata fixes */
  CHIP_Init();

  /* Initialize DVK board register access */
  BSP_Init(BSP_INIT_DEFAULT);

  /* If first word of user data page is non-zero, enable eA Profiler trace */
  BSP_TraceProfilerSetup();
  BSP_LedsSet(0xaaaa);

  /* Initialize LCD controller without boost */
  SegmentLCD_Init(false);

  /* When clock rate changes, the interrupt rate will go up and down */
  /* since we don't adjust after the new settings */
  SegmentLCD_AllOff();

  SegmentLCD_Symbol(LCD_SYMBOL_COL5, 1);

  /* Enable board control interrupts */
  BSP_InterruptDisable(0xffff);
  BSP_InterruptFlagsClear(0xffff);
  BSP_InterruptEnable(BC_INTEN_JOYSTICK);
  clocksIRQInit();
  BSP_LedsSet(0xc003);

  /* Main loop - just update LCD */
  while (1)
  {
    /* Update LCD display with current settings */
    clocksUpdateLCD();

    /* Enter EM as selected */
    switch (eMode)
    {
    case 1:
      EMU_EnterEM1();
      break;

    case 2:
      EMU_EnterEM2(true);
      break;

    default: /* EM0 */
      break;
    }
  }
}
