/**************************************************************************//**
 * @file
 * @brief Small demo for illustrating current in Energy Modes 0-4
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

static int          eMode = 0; /* selected energy mode */
static volatile int msCountDown;
volatile uint32_t   msTicks;   /* counts 1ms timeTicks */

/** Timer used for bringing the system back to EM0. */
static RTCDRV_TimerID_t xTimerForWakeUp;

/* Local prototypes */
void Delay(uint32_t dlyTicks);
void gpioSetup(void);

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
 * @brief GPIO Interrupt handler (PB11)
 *****************************************************************************/
void GPIO_ODD_IRQHandler(void)
{
  /* Acknowledge interrupt */
  GPIO_IntClear(1 << 11);

  if (eMode < 8)
  {
    eMode       = eMode + 1;
    msCountDown = 4000;
  }
}


/**************************************************************************//**
 * @brief GPIO Interrupt handler (PD8)
 *****************************************************************************/
void GPIO_EVEN_IRQHandler(void)
{
  /* Acknowledge interrupt */
  GPIO_IntClear(1 << 8);

  if (eMode > 0)
  {
    eMode       = eMode - 1;
    msCountDown = 4000;
  }
}


/**************************************************************************//**
 * @brief Setup GPIO interrupt to change demo mode
 *****************************************************************************/
void gpioSetup(void)
{
  /* Enable GPIO in CMU */
  CMU_ClockEnable(cmuClock_GPIO, true);

  /* Configure PD8 and PB11 as input */
  GPIO_PinModeSet(gpioPortD, 8, gpioModeInput, 0);
  GPIO_PinModeSet(gpioPortB, 11, gpioModeInput, 0);

  /* Set falling edge interrupt for both ports */
  GPIO_IntConfig(gpioPortD, 8, false, true, true);
  GPIO_IntConfig(gpioPortB, 11, false, true, true);

  /* Enable interrupt in core for even and odd gpio interrupts */
  NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
  NVIC_EnableIRQ(GPIO_EVEN_IRQn);

  NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
  NVIC_EnableIRQ(GPIO_ODD_IRQn);
}


/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  const int msDelay = 100;

  /* Chip errata */
  CHIP_Init();

  /* If first word of user data page is non-zero, enable eA Profiler trace */
  BSP_TraceProfilerSetup();

    /* Power down special RAM blocks to reduce current consumption with some nA. */
  CMU_ClockEnable(cmuClock_CORELE, true);
  LESENSE->POWERDOWN = LESENSE_POWERDOWN_RAM;
  CMU_ClockEnable(cmuClock_CORELE, false);

  /* Initialize RTC */
  RTCDRV_Init();
  RTCDRV_AllocateTimer( &xTimerForWakeUp);

  /* Configure push button interrupts */
  gpioSetup();

  /* Setup SysTick Timer for 1 msec interrupts  */
  if (SysTick_Config(SystemCoreClockGet() / 1000)) while (1) ;

  /* Initialize LCD controller */
  SegmentLCD_Init(false);

  /* Run countdown for user to select energy mode */
  msCountDown = 4000; /* milliseconds */
  while (msCountDown > 0)
  {
    switch (eMode)
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
    case 3:
      SegmentLCD_Write("EM3");
      break;
    case 4:
      SegmentLCD_Write("EM4");
      break;
    case 5:
      SegmentLCD_Write("EM2+RTC");
      break;
    case 6:
      SegmentLCD_Write("RTC+LCD");
      break;
    case 7:
      SegmentLCD_Write("EM3+RTC");
      break;
    case 8:
      SegmentLCD_Write("USER");
      break;
    }
    SegmentLCD_Number(msCountDown);
    Delay(msDelay);
    msCountDown -= msDelay;
  }
  /* Disable components, reenable when needed */
  SegmentLCD_Disable();
  RTC_Enable(false);

  /* GPIO->ROUTE = 0x00000000; */

  /* Go to energy mode and wait for reset */
  switch (eMode)
  {
  case 0:
    /* Disable pin input */
    GPIO_PinModeSet(gpioPortB, 10, gpioModeDisabled, 1);

    /* Disable systick timer */
    SysTick->CTRL = 0;

    /* 32Mhz primes demo - running off HFXO */
    CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);
    /* Disable HFRCO, LFRCO and all unwanted clocks */
    CMU->OSCENCMD     = CMU_OSCENCMD_HFRCODIS;
    CMU->OSCENCMD     = CMU_OSCENCMD_LFRCODIS;
    CMU->HFPERCLKEN0  = 0x00000000;
    CMU->HFCORECLKEN0 = 0x00000000;
    CMU->LFACLKEN0    = 0x00000000;
    CMU->LFBCLKEN0    = 0x00000000;
    CMU->LFCLKSEL     = 0x00000000;
    /* Supress Conditional Branch Target Prefetch */
    MSC->READCTRL = MSC_READCTRL_MODE_WS1SCBTP;
    {
      #define PRIM_NUMS    64
      uint32_t i, d, n;
      uint32_t primes[PRIM_NUMS];

      /* Find prime numbers forever */
      while (1)
      {
        primes[0] = 1;
        for (i = 1; i < PRIM_NUMS;)
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
              if (n % d == 0) break;
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
    SysTick->CTRL = 0;

    CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);
    /* Disable HFRCO, LFRCO and all unwanted clocks */
    CMU->OSCENCMD     = CMU_OSCENCMD_HFRCODIS;
    CMU->OSCENCMD     = CMU_OSCENCMD_LFRCODIS;
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
    CMU->OSCENCMD     = CMU_OSCENCMD_LFXODIS;
    CMU->HFPERCLKEN0  = 0x00000000;
    CMU->HFCORECLKEN0 = 0x00000000;
    CMU->LFACLKEN0    = 0x00000000;
    CMU->LFBCLKEN0    = 0x00000000;
    /* Disable LFB clock select */
    CMU->LFCLKSEL     = 0x00000000;
    EMU_EnterEM2(false);
    break;
  case 3:
    /* Disable all clocks */
    CMU->OSCENCMD     = CMU_OSCENCMD_LFXODIS;
    CMU->OSCENCMD     = CMU_OSCENCMD_LFRCODIS;
    CMU->HFPERCLKEN0  = 0x00000000;
    CMU->HFCORECLKEN0 = 0x00000000;
    CMU->LFACLKEN0    = 0x00000000;
    CMU->LFBCLKEN0    = 0x00000000;
    CMU->LFCLKSEL     = 0x00000000;
    EMU_EnterEM3(false);
    break;
  case 4:
    /* Go straight down to EM4 */
    EMU_EnterEM4();
    break;
  case 5:
    /* EM2 + RTC - only briefly wake up to reconfigure every 2 seconds */
    /* Disable LFB clock select */
    CMU->LFCLKSEL     &= ~(_CMU_LFCLKSEL_LFB_MASK);
    RTC_Enable( true);
    while (1)
    {
      RTCDRV_StartTimer( xTimerForWakeUp, rtcdrvTimerTypeOneshot, 2000, NULL, NULL);
      EMU_EnterEM2(false);
    }
  case 6:
    /* EM2 + RTC + LCD (if battery slips below 3V vboost should be enabled) */
    /* Disable LFB clock select */
    CMU->LFCLKSEL     &= ~(_CMU_LFCLKSEL_LFB_MASK);
    SegmentLCD_Init(false);
    RTC_Enable( true);
    while (1)
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
  case 7:
    /* EM3 + RTC - only briefly wake up to reconfigure every ~5 seconds */
    CMU_OscillatorEnable( cmuOsc_ULFRCO, true, true);
    RTC_Enable( true);
    while (1)
    {
      /* Disable LFB clock select */
      CMU->LFCLKSEL     &= ~(_CMU_LFCLKSEL_LFB_MASK);
      // Select the ULFRCO as clock source for RTC because it will run in EM3
      // Remember to scale the timeout value because ULFRCO is slower than LFXO
      CMU_ClockSelectSet( cmuClock_LFA, cmuSelect_ULFRCO);
      CMU_OscillatorEnable( cmuOsc_LFXO, false, false);
      CMU_OscillatorEnable( cmuOsc_LFRCO, false, false);
      // Start the timer with proper scaling
      RTCDRV_StartTimer( xTimerForWakeUp, rtcdrvTimerTypeOneshot, 5000/32, NULL, NULL);
      /* Sleep in EM3, wake up on RTC trigger */
      EMU_EnterEM3(false);

      /* SegmentLCD_Init will configure LFCLK A as LFRCO */
      SegmentLCD_Init(false);
      SegmentLCD_Write("ULFRCO");
      Delay(1000);
      SegmentLCD_Disable();

    }
  case 8:
  default:
    /* User defined */
    break;
  }

  return 0;
}
