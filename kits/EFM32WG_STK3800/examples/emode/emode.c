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
#include "em_burtc.h"
#include "em_lcd.h"
#include "em_chip.h"
#include "em_rmu.h"
#include "segmentlcd.h"
#include "bsp_trace.h"

static int          eMode = 0; /* selected energy mode */
static volatile int msCountDown;
volatile uint32_t   msTicks;   /* counts 1ms timeTicks */

/* Local prototypes */
static void Delay(uint32_t dlyTicks);
static void GpioSetup(void);

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
static void Delay(uint32_t dlyTicks)
{
  uint32_t curTicks;

  curTicks = msTicks;
  while ((msTicks - curTicks) < dlyTicks) ;
}


/**************************************************************************//**
 * @brief GPIO Interrupt handler (PB10)
 *****************************************************************************/
void GPIO_EVEN_IRQHandler(void)
{
  /* Acknowledge interrupt */
  GPIO_IntClear(1 << 10);

  if (eMode < 9)
  {
    eMode       = eMode + 1;
    msCountDown = 4000;
  }
}


/**************************************************************************//**
 * @brief GPIO Interrupt handler (PB9)
 *****************************************************************************/
void GPIO_ODD_IRQHandler(void)
{
  /* Acknowledge interrupt */
  GPIO_IntClear(1 << 9);

  if (eMode > 0)
  {
    eMode       = eMode - 1;
    msCountDown = 4000;
  }
}


/**************************************************************************//**
 * @brief Setup GPIO interrupt to change demo mode
 *****************************************************************************/
static void GpioSetup(void)
{
  /* Enable GPIO in CMU */
  CMU_ClockEnable(cmuClock_GPIO, true);

  /* Configure PB9 and PB10 as input */
  GPIO_PinModeSet(gpioPortB, 9, gpioModeInput, 0);
  GPIO_PinModeSet(gpioPortB, 10, gpioModeInput, 0);

  /* Set falling edge interrupt for both ports */
  GPIO_IntConfig(gpioPortB, 9, false, true, true);
  GPIO_IntConfig(gpioPortB, 10, false, true, true);

  /* Enable interrupt in core for even and odd gpio interrupts */
  NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
  NVIC_EnableIRQ(GPIO_EVEN_IRQn);

  NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
  NVIC_EnableIRQ(GPIO_ODD_IRQn);
}

/**************************************************************************//**
 * @brief Interrupt handler for BURTC.
 *****************************************************************************/
void BURTC_IRQHandler( void )
{
  BURTC_IntDisable(BURTC_IEN_COMP0);
  BURTC_IntClear(BURTC_IFC_COMP0);
  BURTC_Enable(false);
}

/**************************************************************************//**
 * @brief Setup BURTC to give interrupt after msec milliseconds has elapsed.
 * @param msec   Number of millisecs to interrupt.
 * @param clkSel BURTC clock source.
 *****************************************************************************/
static void BurtcTrigger( uint32_t msec, BURTC_ClkSel_TypeDef clkSel )
{
  uint32_t burtcFreq      = 0;
  BURTC_Init_TypeDef init = BURTC_INIT_DEFAULT;
  EMU_EM4Init_TypeDef em4Init   = EMU_EM4INIT_DEFAULT;

  init.enable = true;
  init.mode   = burtcModeEM4;
  init.clkSel = clkSel;

  /* Always use divider 1. */
  if ( clkSel == burtcClkSelULFRCO )
    init.clkDiv = burtcClkDiv_2;
  else
    init.clkDiv = burtcClkDiv_1;

  BURTC_Init( &init );
  EMU_EM4Init(&em4Init);
  BURTC_StatusClear();

  if ( clkSel == burtcClkSelULFRCO )
    burtcFreq = SystemULFRCOClockGet();

  else if ( clkSel == burtcClkSelLFRCO )
    burtcFreq = SystemLFRCOClockGet();

  else if ( clkSel == burtcClkSelLFXO )
    burtcFreq = SystemLFXOClockGet();

  if ( burtcFreq )
  {
    BURTC_CompareSet(0, BURTC_CounterGet() + ((burtcFreq * msec) / 1000) );

    BURTC_IntClear(BURTC_IFC_COMP0);
    NVIC_ClearPendingIRQ(BURTC_IRQn);
    NVIC_EnableIRQ(BURTC_IRQn);
    BURTC_IntEnable(BURTC_IEN_COMP0);
  }
}

void EnterIntoEm4(void)
{
  /* Disable LFA & LFB clock selects. */
  CMU->LFCLKSEL &= ~(_CMU_LFCLKSEL_LFA_MASK | _CMU_LFCLKSEL_LFB_MASK);
  /* SegmentLCD_Init will configure LFCLK A as LFRCO */
  SegmentLCD_Init(false);
  SegmentLCD_Write("ULFRCO");
  Delay(1000);
  SegmentLCD_Disable();
  /* Sleep in EM4, wake up on BURTC trigger */
  BURTC_CounterReset();
  BurtcTrigger( 5000, burtcClkSelULFRCO );
  EMU_EnterEM4();
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  const int msDelay = 100;
  uint32_t resetCause;
  /* Chip errata */
  CHIP_Init();

  /* If first word of user data page is non-zero, enable eA Profiler trace */
  BSP_TraceProfilerSetup();

  CMU_ClockEnable(cmuClock_CORELE, true);
  resetCause = RMU_ResetCauseGet();

  /* Powerdown unused special RAM blocks to reduce current consumption further. */
  RMU->CTRL = 0;
  BURTC->POWERDOWN = BURTC_POWERDOWN_RAM;
  LESENSE->POWERDOWN = LESENSE_POWERDOWN_RAM;

  CMU_ClockEnable(cmuClock_CORELE, false);
  RMU_ResetControl(rmuResetBU, rmuResetModeClear);

  /* Configure push button interrupts */
  GpioSetup();

  /* Setup SysTick Timer for 1 msec interrupts  */
  if (SysTick_Config(SystemCoreClockGet() / 1000)) while (1) ;

  RMU_ResetCauseClear();

  /* Initialize LCD controller */
  SegmentLCD_Init(false);

  if(resetCause & RMU_RSTCAUSE_EM4WURST)
    EnterIntoEm4();

  /* Run countdown for user to select energy mode */
  msCountDown = 4000; /* milliseconds */
  while (msCountDown > 0)
  {
    switch (eMode)
    {
    case 0:
      SegmentLCD_Write("EM0 48M");
      break;
    case 1:
      SegmentLCD_Write("EM1 48M");
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
      SegmentLCD_Write("RTC+LCD");
      break;
    case 6:
      SegmentLCD_Write("EM2+RTC");
      break;
    case 7:
      SegmentLCD_Write("EM3+RTC");
      break;
    case 8:
      SegmentLCD_Write("EM4+RTC");
      break;
    case 9:
      SegmentLCD_Write("USER");
      break;
    }
    SegmentLCD_Number(msCountDown);
    Delay(msDelay);
    msCountDown -= msDelay;
  }
  /* Disable components, reenable when needed */
  SegmentLCD_Disable();

  /* Go to energy mode and wait for reset */
  switch (eMode)
  {
  case 0:
    /* Disable pin input */
    GPIO_PinModeSet(gpioPortB, 10, gpioModeDisabled, 1);

    /* Disable systick timer */
    SysTick->CTRL = 0;

    /* 48Mhz primes demo - running off HFXO */
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
    MSC->READCTRL = MSC_READCTRL_MODE_WS2SCBTP;
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
    /* EM2 + BURTC + LCD (if battery slips below 3V vboost should be enabled) */
    /* Disable LFA & LFB clock selects. */
    CMU->LFCLKSEL &= ~(_CMU_LFCLKSEL_LFA_MASK | _CMU_LFCLKSEL_LFB_MASK);
    SegmentLCD_Init(false);
    while (1)
    {
      SegmentLCD_Write("Silicon");
      /* Sleep in EM2 */
      BurtcTrigger( 2000, burtcClkSelLFRCO );
      EMU_EnterEM2(false);

      SegmentLCD_Write(" Labs");
      /* Sleep in EM2 */
      BurtcTrigger( 2000, burtcClkSelLFRCO );
      EMU_EnterEM2(false);
    }
  case 6:
    /* EM2 + BURTC - only briefly wake up to reconfigure every 2 seconds. */
    /* Disable LFA & LFB clock selects. */
    CMU->LFCLKSEL &= ~(_CMU_LFCLKSEL_LFA_MASK | _CMU_LFCLKSEL_LFB_MASK);
    while (1)
    {
      BurtcTrigger( 2000, burtcClkSelLFRCO );
      EMU_EnterEM2(false);
    }
  case 7:
    /* EM3 + BURTC - only briefly wake up to reconfigure every ~5 seconds */
    while (1)
    {
      /* Disable LFA & LFB clock selects. */
      CMU->LFCLKSEL &= ~(_CMU_LFCLKSEL_LFA_MASK | _CMU_LFCLKSEL_LFB_MASK);
      BurtcTrigger( 5000, burtcClkSelULFRCO );
      /* Sleep in EM3, wake up on BURTC trigger */
      EMU_EnterEM3(false);
      /* SegmentLCD_Init will configure LFCLK A as LFRCO */
      SegmentLCD_Init(false);
      SegmentLCD_Write("ULFRCO");
      Delay(1000);
      SegmentLCD_Disable();
    }
  case 8:
      EnterIntoEm4();
    break;
  case 9:
  default:
    /* User defined */
    break;
  }

  return 0;
}
