/**************************************************************************//**
 * @file lcd_setup.c
 * @brief Setup LCD for energy mode demo
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

#include "bspconfig.h"
#include "lcd_setup.h"
#include "display.h"
#include "textdisplay.h"
#include "retargettextdisplay.h"
#include "bspconfig.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_pcnt.h"
#include "em_assert.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

// Frequency of RTC (COMP0) pulses on PRS channel 2.
#define RTC_PULSE_FREQUENCY    (LS013B7DH03_POLARITY_INVERSION_FREQUENCY)
#define NO_OF_EMODE_TESTS      (15)       // Number of energy modes.

static volatile int eMode;                // Selected energy mode.
static volatile int startTest;            // Start selected energy mode test.
static volatile bool displayEnabled;      // Status of LCD display.
static volatile uint32_t seconds = 0;     // Seconds elapsed since reset.
static DISPLAY_Device_t displayDevice;    // Display device handle.

static void gpioSetup(void);
static void pcntInit(void);

/**************************************************************************//**
 * @brief  Selects eMode for state using the LCD and buttons.
 *
 * @return uint32_t selected eMode.
 *****************************************************************************/
uint32_t LCD_SelectState(void)
{
  int currentEMode;

  // Setup GPIO for pushbuttons.
  gpioSetup();

  // Initialize the display module.
  displayEnabled = true;
  DISPLAY_Init();

  // Retrieve the properties of the display.
  if (DISPLAY_DeviceGet(0, &displayDevice) != DISPLAY_EMSTATUS_OK)
  {
    // Unable to get display handle.
	EFM_ASSERT(false);
  }

  // Retarget stdio to the display.
  if (TEXTDISPLAY_EMSTATUS_OK != RETARGET_TextDisplayInit())
  {
    // Text display initialization failed.
	EFM_ASSERT(false);
  }

  // Select LFRCO as clock source for LFA (PCNT)
  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFRCO);

  // Set PCNT to generate an interrupt every second.
  pcntInit();

  printf("\n  ENERGY MODE"
         "\n    CURRENT"
         "\n  CONSUMPTION"
         "\n\n\n  Push BTN1 to\n"
         "  cycle modes\n"
         "\n  Push BTN0 to\n"
         "  start test \n\n\n");

  startTest = false;
  eMode = 0;
  currentEMode = -1;

  while (!startTest)
  {
    // Mode change ? If yes, update the display.
    if (eMode != currentEMode)
    {
      currentEMode = eMode;
      switch (eMode)
      {
        case 0:
          printf("\r   EM0 40MHz    \n"
                 "  (primes calc)");
          break;
        case 1:
          printf(TEXTDISPLAY_ESC_SEQ_CURSOR_HOME_VT100);
          printf("\n\n\n\n\n\n\n\n\n\n\n\n\n");
          printf("\r   EM0 38MHz    \n"
                 "  (primes calc)");
          break;
        case 2:
          printf(TEXTDISPLAY_ESC_SEQ_CURSOR_HOME_VT100);
          printf("\n\n\n\n\n\n\n\n\n\n\n\n\n");
          printf("\r   EM0 32MHz    \n"
                 "  (primes calc)");
          break;
        case 3:
          printf(TEXTDISPLAY_ESC_SEQ_CURSOR_HOME_VT100);
          printf("\n\n\n\n\n\n\n\n\n\n\n\n\n");
          printf("\r   EM0 26MHz    \n"
                 "  (primes calc)");
          break;
        case 4:
          printf(TEXTDISPLAY_ESC_SEQ_CURSOR_HOME_VT100);
          printf("\n\n\n\n\n\n\n\n\n\n\n\n\n");
          printf("\r   EM0 19MHz   \n"
                 "  (primes calc)");
          break;
        case 5:
          printf(TEXTDISPLAY_ESC_SEQ_CURSOR_HOME_VT100);
          printf("\n\n\n\n\n\n\n\n\n\n\n\n\n");
          printf("\r   EM0 16MHz   \n"
                 "  (primes calc)");
          break;
        case 6:
          printf("\r               ");
          printf(TEXTDISPLAY_ESC_SEQ_CURSOR_HOME_VT100);
          printf("\n\n\n\n\n\n\n\n\n\n\n\n\n   EM1 40MHz ");
          break;
        case 7:
          printf("\r   EM1 38MHz");
          break;
        case 8:
          printf("\r   EM1 32MHz");
          break;
        case 9:
          printf("\r   EM1 26MHz");
          break;
        case 10:
          printf("\r   EM1 19MHz");
          break;
        case 11:
          printf("\r   EM1 16MHz");
          break;
        case 12:
          printf("\r   EM2 RTC 1Hz");
          break;
        case 13:
          printf("\r   EM3 ULFRCO ");
          break;
        case 14:
          printf("\r   EM4S       ");
          break;
      }
    }
  }

  // Get ready to start the energy mode test. Turn off everything we do not need.

  // Disable GPIO.
  NVIC_DisableIRQ(GPIO_EVEN_IRQn);
  NVIC_DisableIRQ(GPIO_ODD_IRQn);
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeDisabled, 1);
  GPIO_PinModeSet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, gpioModeDisabled, 1);

  // Clear LCD display.
  printf("\f");

  // Power down LCD display and disable the RTCC.
  displayEnabled = false;
  NVIC_DisableIRQ(RTCC_IRQn);
  NVIC_DisableIRQ(PCNT0_IRQn);
  displayDevice.pDisplayPowerOn(&displayDevice, false);

  // Return desired energy mode.
  return eMode;
}

/**************************************************************************//**
 * @brief Setup GPIO interrupt for pushbuttons.
 *****************************************************************************/
static void gpioSetup(void)
{
  // Enable GPIO clock.
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure PB0 as input and enable interrupt.
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInputPull, 1);
  GPIO_IntConfig(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, false, true, true);

  // Configure PB1 as input and enable interrupt.
  GPIO_PinModeSet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, gpioModeInputPull, 1);
  GPIO_IntConfig(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, false, true, true);

  NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
  NVIC_EnableIRQ(GPIO_EVEN_IRQn);

  NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
  NVIC_EnableIRQ(GPIO_ODD_IRQn);
}

/**************************************************************************//**
 * @brief Unified GPIO Interrupt handler (pushbuttons).
 *        PB0 Starts selected test.
 *        PB1 Cycles through the available tests.
 *****************************************************************************/
void GPIO_Unified_IRQ(void)
{
  // Get and clear all pending GPIO interrupts.
  uint32_t interruptMask = GPIO_IntGet();
  GPIO_IntClear(interruptMask);

  // Act on interrupts.
  if (interruptMask & (1 << BSP_GPIO_PB0_PIN))
  {
    // BTN0: Start test.
    startTest = true;
  }

  if (interruptMask & (1 << BSP_GPIO_PB1_PIN))
  {
    // BTN1: cycle through tests.
    eMode = (eMode + 1) % NO_OF_EMODE_TESTS;
  }
}

/**************************************************************************//**
 * @brief GPIO Interrupt handler for even pins.
 *****************************************************************************/
void GPIO_EVEN_IRQHandler(void)
{
  GPIO_Unified_IRQ();
}

/**************************************************************************//**
 * @brief GPIO Interrupt handler for odd pins.
 *****************************************************************************/
void GPIO_ODD_IRQHandler(void)
{
  GPIO_Unified_IRQ();
}

/**************************************************************************//**
 * @brief   Set up PCNT to generate an interrupt every second.
 *          There is already a timebase from the RTC since we have to toggle
 *          the display inversion pin regularly. We can use that same signal
 *          to keep a one-second timebase in the +LCD modes, so we can update
 *          the spinner.
 *****************************************************************************/
static void pcntInit(void)
{
  PCNT_Init_TypeDef pcntInit = PCNT_INIT_DEFAULT;

  // Enable PCNT clock.
  CMU_ClockEnable(cmuClock_PCNT0, true);
  // Set up the PCNT to count RTC_PULSE_FREQUENCY pulses -> one second.
  pcntInit.mode = pcntModeOvsSingle;
  pcntInit.top = RTC_PULSE_FREQUENCY;
  pcntInit.s1CntDir = false;
  // The PRS channel used depends on the configuration and which pin the
  // LCD inversion toggle is connected to. So use the generic define here.
  pcntInit.s0PRS = (PCNT_PRSSel_TypeDef) LCD_AUTO_TOGGLE_PRS_CH;

  PCNT_Init(PCNT0, &pcntInit);

  // Select PRS as the input for the PCNT.
  PCNT_PRSInputEnable(PCNT0, pcntPRSInputS0, true);

  // Enable PCNT interrupt every second.
  NVIC_EnableIRQ(PCNT0_IRQn);
  PCNT_IntEnable(PCNT0, PCNT_IF_OF);
}

/**************************************************************************//**
 * @brief   This interrupt is triggered at every second by the PCNT.
 *****************************************************************************/
void PCNT0_IRQHandler(void)
{
  PCNT_IntClear(PCNT0, PCNT_IF_OF);

  seconds++;
}
