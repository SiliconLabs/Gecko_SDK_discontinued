/**************************************************************************//**
 * @file
 * @brief Small demo for illustrating current in energy modes 0-4
 *
 * This application works nicely together with energyAware Profiler
 * as a quick demonstration.
 *
 * Note! For improved and accurate measurements, read the help section
 * of the energyAware Profiler
 *
 * @version 4.1.0
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

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "em_device.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_rtc.h"
#include "em_lcd.h"
#include "em_chip.h"

#include "segmentlcd.h"
#include "rtcdriver.h"
#include "bsp_trace.h"

static int eMode = 0; /* selected energy mode */
volatile uint32_t msTicks; /* counts 1ms timeTicks */

/** Timer used for bringing the system back to EM0. */
static RTCDRV_TimerID_t xTimerForWakeUp;

/**************************************************************************//**
 * @brief SysTick_Handler
 * Interrupt Service Routine for system tick counter
 *****************************************************************************/
void SysTick_Handler(void)
{
  msTicks++;       /* increment counter necessary in Delay()*/
}

/**************************************************************************//**
 * @brief Delays number of msTick Systicks (typically 1 ms)
 * @param dlyTicks Number of ticks to delay
 *****************************************************************************/
void Delay(uint32_t dlyTicks)
{
  uint32_t curTicks;

  curTicks = msTicks;
  while ((msTicks - curTicks) < dlyTicks) ;
}

/**************************************************************************//**
 * @brief GPIO Interrupt handler (PB9)
 *****************************************************************************/
void GPIO_ODD_IRQHandler(void)
{
  /* Acknowledge interrupt */
  GPIO_IntClear(1 << 9);

  if ( eMode > 0)
  {
    eMode = eMode -1;
  }
}

/**************************************************************************//**
 * @brief GPIO Interrupt handler (PB10)
 *****************************************************************************/
void GPIO_EVEN_IRQHandler(void)
{
  /* Acknowledge interrupt */
  GPIO_IntClear(1 << 10);

  if (eMode < 6)
  {
    eMode = eMode + 1;
  }
}

/**************************************************************************//**
 * @brief Setup GPIO interrupt to change demo mode
 *****************************************************************************/
void gpioSetup(void)
{
  /* Enable GPIO clock */
  CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_GPIO;

  /* Configure PB10 as input and enable interrupt  */
  GPIO_PinModeSet(gpioPortB, 10, gpioModeInputPull, 1);
  GPIO_IntConfig(gpioPortB, 10, false, true, true);

  NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
  NVIC_EnableIRQ(GPIO_EVEN_IRQn);

  /* Configure PB9 as input and enable interrupt */
  GPIO_PinModeSet(gpioPortB, 9, gpioModeInputPull, 1);
  GPIO_IntConfig(gpioPortB, 9, false, true, true);

  NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
  NVIC_EnableIRQ(GPIO_ODD_IRQn);
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  int msCountDown;
  const int msDelay = 100;
  char displayString[8];
  LCD_AnimInit_TypeDef anim = {
    true,
    0x00,
    lcdAnimShiftNone,
    0x03,
    lcdAnimShiftLeft,
    lcdAnimLogicOr
  };
  LCD_FrameCountInit_TypeDef fc = {
    true,
    2, /* Update each 2nd frame */
    lcdFCPrescDiv1,
  };

  /* Chip errata */
  CHIP_Init();

  /* If first word of user data page is non-zero, enable eA Profiler trace */
  BSP_TraceProfilerSetup();

  /* Configure push button interrupts */
  gpioSetup();

  /* Setup SysTick Timer for 1 msec interrupts  */
  if (SysTick_Config(SystemCoreClockGet() / 1000)) while (1) ;

  /* Initialize RTC timer. */
  RTCDRV_Init();
  RTCDRV_AllocateTimer( &xTimerForWakeUp);

  /* Initialize LCD controller */
  SegmentLCD_Init(false);

  /* Run countdown for user to select energy mode */
  msCountDown = 4000; /* milliseconds */
  while(msCountDown > 0)
  {
    if ( eMode >=3 && eMode<=4) {
      sprintf(displayString, "EM%d", eMode);
      SegmentLCD_Write(displayString);
    }
    switch( eMode )
    {
    case 0:
      SegmentLCD_Write("EM0 32M");
      break;
    case 1:
      SegmentLCD_Write("EM1 32M");
      break;
    case 2:
      SegmentLCD_Write("EM2 32K");
      break;
    case 5:
      SegmentLCD_Write("EM2+RTC");
      break;
    case 6:
      SegmentLCD_Write("RTC+LCD");
      break;
    }
    SegmentLCD_Number(msCountDown);
    Delay(msDelay);
    msCountDown -= msDelay;
  }
  /* Disable components, reenable when needed */
  SegmentLCD_Disable();
  RTC_Enable(false);

  /* Go to energy mode and wait for reset */
  switch(eMode)
  {
  case 0:
    /* Disable pin input */
    GPIO_PinModeSet(gpioPortB, 10, gpioModeDisabled, 1);

    /* Disable systick timer */
    SysTick->CTRL  = 0;

    /* 32Mhz primes demo - running off HFXO */
    CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);
    /* Disable HFRCO, LFRCO and all unwanted clocks */
    CMU->OSCENCMD = CMU_OSCENCMD_HFRCODIS;
    CMU->OSCENCMD = CMU_OSCENCMD_LFRCODIS;
    CMU->HFPERCLKEN0  = 0x00000000;
    CMU->HFCORECLKEN0 = 0x00000000;
    CMU->LFACLKEN0    = 0x00000000;
    CMU->LFBCLKEN0    = 0x00000000;
    CMU->LFCLKSEL     = 0x00000000;
    /* Supress Conditional Branch Target Prefetch */
    MSC->READCTRL = MSC_READCTRL_MODE_WS1SCBTP;
    {
      #define PRIM_NUMS 64
      uint32_t i, d, n;
      uint32_t primes[PRIM_NUMS];

      /* Find prime numbers forever */
      while (1)
      {
        primes[0] = 1;
        for (i = 1; i < PRIM_NUMS;)
        {
          for (n = primes[i - 1] + 1; ;n++)
          {
            for (d = 2; d <= n; d++)
            {
              if (n == d)
              {
                primes[i] = n;
                goto nexti;
              }
              if (n%d == 0) break;
            }
          }
        nexti:
          i++;
        }
      }
    }
  case 1:
    /* Disable pin input */
    GPIO_PinModeSet(gpioPortB, 10, gpioModeDisabled, 1);

    /* Disable systick timer */
    SysTick->CTRL  = 0;

    CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);
    /* Disable HFRCO, LFRCO and all unwanted clocks */
    CMU->OSCENCMD = CMU_OSCENCMD_HFRCODIS;
    CMU->OSCENCMD = CMU_OSCENCMD_LFRCODIS;
    CMU->HFPERCLKEN0  = 0x00000000;
    CMU->HFCORECLKEN0 = 0x00000000;
    CMU->LFACLKEN0    = 0x00000000;
    CMU->LFBCLKEN0    = 0x00000000;
    CMU->LFCLKSEL     = 0x00000000;
    EMU_EnterEM1();
    break;
  case 2:
    /* Enable LFRCO */
    CMU->OSCENCMD = CMU_OSCENCMD_LFRCOEN;
    /* Disable everything else */
    CMU->OSCENCMD = CMU_OSCENCMD_LFXODIS;
    CMU->HFPERCLKEN0  = 0x00000000;
    CMU->HFCORECLKEN0 = 0x00000000;
    CMU->LFACLKEN0    = 0x00000000;
    CMU->LFBCLKEN0    = 0x00000000;
    CMU->LFCLKSEL     = 0x00000000;
    EMU_EnterEM2(false);
    break;
  case 3:
    /* Disable all clocks */
    CMU->OSCENCMD = CMU_OSCENCMD_LFXODIS;
    CMU->OSCENCMD = CMU_OSCENCMD_LFRCODIS;
    CMU->HFPERCLKEN0  = 0x00000000;
    CMU->HFCORECLKEN0 = 0x00000000;
    CMU->LFACLKEN0    = 0x00000000;
    CMU->LFBCLKEN0    = 0x00000000;
    EMU_EnterEM3(false);
    break;
  case 4:
    EMU_EnterEM4();
    break;
  case 5:
    /* EM2 + RTC - only briefly wake up to reconfigure each second */
    CMU->LFCLKSEL     = CMU_LFCLKSEL_LFA_LFRCO;
    RTC_Enable( true);
    while(1)
    {
      RTCDRV_StartTimer( xTimerForWakeUp, rtcdrvTimerTypeOneshot, 2000, NULL, NULL);
      EMU_EnterEM2(false);
    }
  case 6:
  default:
    /* EM2 + RTC + LCD (if battery slips below 3V vboost should be */
    /* enabled) */
    CMU->LFCLKSEL     = CMU_LFCLKSEL_LFA_LFRCO;
    RTC_Enable( true);
    SegmentLCD_Init(false);
    /* Animate LCD */
    LCD_FrameCountInit(&fc);
    LCD_AnimInit(&anim);
    while(1)
    {
      SegmentLCD_Write("Silicon");
      /* Sleep in EM2 */
      RTCDRV_StartTimer( xTimerForWakeUp, rtcdrvTimerTypeOneshot, 2000, NULL, NULL);
      EMU_EnterEM2(false);

      SegmentLCD_Write(" Labs");
      /* Sleep in EM2 */
      RTCDRV_StartTimer( xTimerForWakeUp, rtcdrvTimerTypeOneshot, 2000, NULL, NULL);
      EMU_EnterEM2(false);
    }
  }

  return 0;
}
