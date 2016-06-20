/**************************************************************************//**
 * @file lcd_setup.c
 * @brief Setup LCD for eMode demo
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
#include "segmentlcd.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_assert.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#define TIMEOUT    4000                   // Time (ms) to wait before energy mode starts.
#define TIME_DELAY 100                    // Time (ms) delay for screen update.
#define PB0_PORT   gpioPortD              // Pushbutton 0 port.
#define PB0_PIN    8                      // Pushbutton 0 pin.
#define PB1_PORT   gpioPortB              // Pushbutton 1 port.
#define PB1_PIN    11                     // Pushbutton 1 pin.

static volatile int eMode;                // Selected energy mode.
static volatile int msCountDown;          // Time left before entering energy mode.
volatile uint32_t msTicks;                // Counts 1ms timeTicks.

// Local function.
void Delay(uint32_t dlyTicks);
void gpioSetup(void);

/**************************************************************************//**
 * @brief  Selects eMode for state using the LCD and buttons.
 *
 * @return uint32_t selected eMode.
 *****************************************************************************/
uint32_t LCD_SelectState(void)
{
  // Configure push button interrupts.
  gpioSetup();

  // Setup SysTick Timer for 1 msec interrupts.
  if (SysTick_Config(SystemCoreClockGet() / 1000))
  {
	EFM_ASSERT(false);
  }

  // Initialize LCD controller.
  SegmentLCD_Init(false);

  // Run countdown for user to select energy mode.
  msCountDown = TIMEOUT; // milliseconds.
  while (msCountDown > 0)
  {
    switch (eMode)
    {
      case 0:
        SegmentLCD_Write("EM0 32M");
        break;
      case 1:
        SegmentLCD_Write("EM0 28M");
        break;
      case 2:
        SegmentLCD_Write("EM0 21M");
        break;
      case 3:
        SegmentLCD_Write("EM0 14M");
        break;
      case 4:
        SegmentLCD_Write("EM0 11M");
        break;
      case 5:
        SegmentLCD_Write("EM0 07M");
        break;
      case 6:
        SegmentLCD_Write("EM0 01M");
        break;
      case 7:
        SegmentLCD_Write("EM1 32M");
        break;
      case 8:
        SegmentLCD_Write("EM1 28M");
        break;
      case 9:
        SegmentLCD_Write("EM1 21M");
        break;
      case 10:
        SegmentLCD_Write("EM1 14M");
        break;
      case 11:
        SegmentLCD_Write("EM1 11M");
        break;
      case 12:
        SegmentLCD_Write("EM1 07M");
        break;
      case 13:
        SegmentLCD_Write("EM1 01M");
        break;
      case 14:
        SegmentLCD_Write("EM2 RTC");
        break;
      case 15:
        SegmentLCD_Write("EM3");
        break;
      case 16:
        SegmentLCD_Write("EM4");
        break;
    }
    SegmentLCD_Number(msCountDown);
    Delay(TIME_DELAY);
    msCountDown -= TIME_DELAY;
  }

  // Get ready to start the energy mode test. Turn off everything we do not need.

  // Clear LCD display.
  SegmentLCD_Write("\f");
  SegmentLCD_Disable();

  // Disable GPIO.
  NVIC_DisableIRQ(GPIO_EVEN_IRQn);
  NVIC_DisableIRQ(GPIO_ODD_IRQn);
  GPIO_PinModeSet(PB0_PORT, PB0_PIN, gpioModeDisabled, 1);
  GPIO_PinModeSet(PB1_PORT, PB1_PIN, gpioModeDisabled, 1);

  // Disable systick timer.
  SysTick ->CTRL = 0;

  // Return desired energy mode.
  return eMode;
}

/**************************************************************************//**
 * @brief SysTick_Handler.
 * Interrupt Service Routine for system tick counter.
 *****************************************************************************/
void SysTick_Handler(void)
{
  msTicks++; // increment counter necessary in Delay().
}

/**************************************************************************//**
 * @brief Delays number of msTick Systicks (typically 1 ms).
 * @param dlyTicks Number of ticks to delay.
 *****************************************************************************/
void Delay(uint32_t dlyTicks)
{
  uint32_t curTicks;

  curTicks = msTicks;
  while ((msTicks - curTicks) < dlyTicks);
}

/**************************************************************************//**
 * @brief GPIO Interrupt handler (PB11).
 *****************************************************************************/
void GPIO_ODD_IRQHandler(void)
{
  // Acknowledge interrupt.
  GPIO_IntClear(1 << 11);

  if (eMode < 16)
  {
    eMode = eMode + 1;
    msCountDown = TIMEOUT;
  }
}

/**************************************************************************//**
 * @brief GPIO Interrupt handler (PD8).
 *****************************************************************************/
void GPIO_EVEN_IRQHandler(void)
{
  // Acknowledge interrupt.
  GPIO_IntClear(1 << 8);

  if (eMode > 0)
  {
    eMode = eMode - 1;
    msCountDown = TIMEOUT;
  }
}

/**************************************************************************//**
 * @brief Setup GPIO interrupt to change demo mode.
 *****************************************************************************/
void gpioSetup(void)
{
  // Enable GPIO in CMU.
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure PD8 and PB11 as input.
  GPIO_PinModeSet(PB0_PORT, PB0_PIN, gpioModeInput, 0);
  GPIO_PinModeSet(PB1_PORT, PB1_PIN, gpioModeInput, 0);

  // Set falling edge interrupt for both ports.
  GPIO_IntConfig(PB0_PORT, PB0_PIN, false, true, true);
  GPIO_IntConfig(PB1_PORT, PB1_PIN, false, true, true);

  // Enable interrupt in core for even and odd gpio interrupts.
  NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
  NVIC_EnableIRQ(GPIO_EVEN_IRQn);

  NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
  NVIC_EnableIRQ(GPIO_ODD_IRQn);
}
