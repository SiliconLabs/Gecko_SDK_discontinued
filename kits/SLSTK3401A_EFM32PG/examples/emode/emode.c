/**************************************************************************//**
 * @file
 * @brief Demo for energy mode current consumption testing.
 * @version 4.1.0
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

#include <stdio.h>

#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_pcnt.h"

#include "display.h"
#include "textdisplay.h"
#include "retargettextdisplay.h"
#include "bspconfig.h"

/* Number of energy mode tests. */
#define NO_OF_EMODE_TESTS     (9)
/* Frequency of RTCC (CCV1) pulses on PRS channel 4. */
#define RTC_PULSE_FREQUENCY   (LS013B7DH03_POLARITY_INVERSION_FREQUENCY / 2)
/* Number of prime numbers to crunch in EM0. */
#define PRIM_NUMS             (64)

static volatile int      eMode;          /* Selected energy mode.            */
static volatile bool     startTest;      /* Start selected energy mode test. */
static volatile bool     displayEnabled; /* Status of LCD display.           */
static volatile uint32_t seconds = 0;    /* Seconds elapsed since reset.     */
static DISPLAY_Device_t  displayDevice;  /* Display device handle.           */

static void gpioSetup(void);
static void pcntInit(void);
static void selectClock(CMU_Select_TypeDef hfClockSelect,
                        uint32_t clockDisableMask);
static void enterEMode(int mode, uint32_t secs);

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  int currentEMode;

  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_STK_DEFAULT;
  CMU_HFXOInit_TypeDef hfxoInit = CMU_HFXOINIT_STK_DEFAULT;

  /* Chip errata */
  CHIP_Init();

  /* Init DCDC regulator and HFXO with kit specific parameters */
  EMU_DCDCInit(&dcdcInit);
  CMU_HFXOInit(&hfxoInit);

  /* Select ULFRCO as clock source for LFA */
  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_ULFRCO);

  /* Setup GPIO for pushbuttons. */
  gpioSetup();

  /* Initialize the display module. */
  displayEnabled = true;
  DISPLAY_Init();

  /* Retrieve the properties of the display. */
  if (DISPLAY_DeviceGet( 0, &displayDevice ) != DISPLAY_EMSTATUS_OK)
  {
    /* Unable to get display handle. */
    while (1);
  }

  /* Retarget stdio to the display. */
  if (TEXTDISPLAY_EMSTATUS_OK != RETARGET_TextDisplayInit())
  {
    /* Text display initialization failed. */
    while (1);
  }

  /* Set PCNT to generate an interrupt every second. */
  pcntInit();

  printf("\n\n\n Push PB1 to\n"
         " cycle through\n"
         " the energy mode\n"
         " tests available"
         "\n\n Push PB0 to\n"
         " start test.\n\n\n");

  startTest    = false;
  eMode        = 0;
  currentEMode = -1;

  /* Disable LFB clock tree. */
  CMU->LFBCLKSEL &= ~(_CMU_LFBCLKSEL_LFB_MASK);

  while (1)
  {
    /* Mode change ? If yes, update the display. */
    if (eMode != currentEMode)
    {
      currentEMode = eMode;
      switch (eMode)
      {
        case 0:
          printf("\r   EM0 38.4MHz\n"
                   "  (primes calc)");
          break;

        case 1:
          printf("\r               " );
          printf( TEXTDISPLAY_ESC_SEQ_CURSOR_HOME_VT100 );
          printf("\n\n\n\n\n\n\n\n\n\n\n\n   EM1 38.4MHz");
          break;

        case 2:
          printf("\r    EM2 32kHz ");
          break;

        case 3:
          printf("\r       EM3    ");
          break;

        case 4:
          printf("\r       EM4 ");
          break;

        case 5:
          printf("\r     EM2+RTC ");
          break;

        case 6:
          printf("\r   EM2+RTC+LCD");
          break;

        case 7:
          printf("\r   EM3+RTC+LCD");
          break;

        case 8:
          printf("\r      USER    ");
          break;
      }
    }

    if (startTest)
    {
      /* Get ready to start the energy mode test. Turn off everything we     */
      /* don't need. Use energyAware Profiler to observe energy consumption. */

      /* Disable GPIO. */
      NVIC_DisableIRQ(GPIO_EVEN_IRQn);
      NVIC_DisableIRQ(GPIO_ODD_IRQn);
      GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeDisabled, 1);
      GPIO_PinModeSet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, gpioModeDisabled, 1);

      /* Clear LCD display. */
      printf("\f");

      switch (eMode)
      {
        case 0:           /* EM0 38.4MHz (primes) */
        case 1:           /* EM1 38.4MHz */
        case 2:           /* EM2 32kHz */
        case 3:           /* EM3 */
        case 4:           /* EM4 */
          /* Power down LCD display and disable the RTC. */
          displayEnabled = false;
          NVIC_DisableIRQ(RTCC_IRQn);
          NVIC_DisableIRQ(PCNT0_IRQn);
          displayDevice.pDisplayPowerOn(&displayDevice, false);
          break;

        case 5:           /* EM2+RTC */
          /* Power down LCD display. */
          displayEnabled = false;
          displayDevice.pDisplayPowerOn(&displayDevice, false);
          break;

        case 6:           /* EM2+RTC+LCD */
        case 7:           /* EM3+RTC+LCD */
          break;

        case 8:           /* USER */
          break;
      }

      /* Do the slected energy mode test. */
      switch (eMode)
      {
        case 0:  /* EM0 38.4MHz (primes) */
          selectClock(cmuSelect_HFXO,               /* HF clock           */
                      CMU_OSCENCMD_HFRCODIS |       /* Clock disable mask */
                      CMU_OSCENCMD_LFXODIS  |
                      CMU_OSCENCMD_LFRCODIS);
          {
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
          /*break;*/

        case 1:  /* EM1 38.4MHz */
          selectClock(cmuSelect_HFXO,               /* HF clock           */
                      CMU_OSCENCMD_HFRCODIS |       /* Clock disable mask */
                      CMU_OSCENCMD_LFXODIS  |
                      CMU_OSCENCMD_LFRCODIS );
          enterEMode(1, 1);
          break;

        case 2:  /* EM2 32kHz */
          selectClock(cmuSelect_LFRCO,              /* HF clock           */
                      CMU_OSCENCMD_HFXODIS  |       /* Clock disable mask */
                      CMU_OSCENCMD_HFRCODIS |
                      CMU_OSCENCMD_LFXODIS  );
          enterEMode(2, 1);
          break;

        case 3:  /* EM3 */
          enterEMode(3, 1);
          break;

        case 4:  /* EM4 */
          enterEMode(4, 1);
          break;

        case 5:  /* EM2+RTC */
          /* Wake up on each PCNT interrupt. */
          while (1)
          {
            enterEMode(2, 1);
            /* This loop will be visible in the Simplicity Profiler. */
            { volatile int i; for (i=0; i<10000; i++); }
          }
          /*break;*/

        case 6:  /* EM2+RTC+LCD */
          /* Wake up on each PCNT interrupt. */
          printf("\n\n\n\n\n\n\n\n");
          while (1)
          {
            enterEMode(2, 1);
            printf("\r  EM2+RTC+LCD -");
            enterEMode(2, 1);
            printf("\r  EM2+RTC+LCD \\");
            enterEMode(2, 1);
            printf("\r  EM2+RTC+LCD |");
            enterEMode(2, 1);
            printf("\r  EM2+RTC+LCD /");
          }
          /*break;*/

        case 7:  /* EM3+RTC+LCD */
          /* Wake up on each PCNT interrupt. */
          printf("\n\n\n\n\n\n\n\n");
          while (1)
          {
            /* Disable LFB clock select */
            enterEMode(3, 1);
            printf("\r  EM3+RTC+LCD -");
            enterEMode(3, 1);
            printf("\r  EM3+RTC+LCD \\");
            enterEMode(3, 1);
            printf("\r  EM3+RTC+LCD |");
            enterEMode(3, 1);
            printf("\r  EM3+RTC+LCD /");
          }
          /*break;*/

        case 8:           /* USER */
          for (;;);
          /*break;*/
      }

      /* We should never end up here ! */
      EFM_ASSERT(false);
    }
  }
}

/**************************************************************************//**
 * @brief   Enter and stay in Energy Mode for a given number of seconds.
 *
 * @param[in] mode  Energy Mode to enter (1..4).
 * @param[in] secs  Time to stay in Energy Mode <mode>.
 *****************************************************************************/
static void enterEMode(int mode, uint32_t secs)
{
  if (secs)
  {
    uint32_t startTime = seconds;

    while ((seconds - startTime) < secs)
    {
      switch (mode)
      {
        case 1: EMU_EnterEM1();       break;
        case 2: EMU_EnterEM2(false);  break;
        case 3: EMU_EnterEM3(false);  break;
        case 4: EMU_EnterEM4();       break;
      default:
        /* Invalid mode. */
        while (1);
      }
    }
  }
}

/**************************************************************************//**
* @brief Setup GPIO interrupt for pushbuttons.
*****************************************************************************/
static void gpioSetup(void)
{
  /* Enable GPIO clock */
  CMU_ClockEnable(cmuClock_GPIO, true);

  /* Configure PB0 as input and enable interrupt  */
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInputPull, 1);
  GPIO_IntConfig(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, false, true, true);

  /* Configure PB1 as input and enable interrupt */
  GPIO_PinModeSet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, gpioModeInputPull, 1);
  GPIO_IntConfig(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, false, true, true);

  NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
  NVIC_EnableIRQ(GPIO_EVEN_IRQn);

  NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
  NVIC_EnableIRQ(GPIO_ODD_IRQn);
}

/**************************************************************************//**
* @brief Unified GPIO Interrupt handler (pushbuttons)
*        PB0 Starts selected test
*        PB1 Cycles through the available tests
*****************************************************************************/
void GPIO_Unified_IRQ(void)
{
  /* Get and clear all pending GPIO interrupts */
  uint32_t interruptMask = GPIO_IntGet();
  GPIO_IntClear(interruptMask);

  /* Act on interrupts */
  if (interruptMask & (1 << BSP_GPIO_PB0_PIN))
  {
    /* PB0: Start test */
    startTest = true;
  }

  if (interruptMask & (1 << BSP_GPIO_PB1_PIN))
  {
    /* PB1: cycle through tests */
    eMode = (eMode + 1) % NO_OF_EMODE_TESTS;
  }
}

/**************************************************************************//**
* @brief GPIO Interrupt handler for even pins
*****************************************************************************/
void GPIO_EVEN_IRQHandler(void)
{
    GPIO_Unified_IRQ();
}

/**************************************************************************//**
* @brief GPIO Interrupt handler for odd pins
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
void pcntInit(void)
{
  PCNT_Init_TypeDef pcntInit = PCNT_INIT_DEFAULT;

  /* Enable PCNT clock */
  CMU_ClockEnable(cmuClock_PCNT0, true);
  /* Set up the PCNT to count RTC_PULSE_FREQUENCY pulses -> one second */
  pcntInit.mode = pcntModeOvsSingle;
  pcntInit.top = RTC_PULSE_FREQUENCY;
  pcntInit.s1CntDir = false;
  /* The PRS channel used depends on the configuration and which pin the
     LCD inversion toggle is connected to. So use the generic define here. */
  pcntInit.s0PRS = (PCNT_PRSSel_TypeDef)LCD_AUTO_TOGGLE_PRS_CH;

  PCNT_Init(PCNT0, &pcntInit);

  /* Select PRS as the input for the PCNT */
  PCNT_PRSInputEnable(PCNT0, pcntPRSInputS0, true);

  /* Enable PCNT interrupt every second */
  NVIC_EnableIRQ(PCNT0_IRQn);
  PCNT_IntEnable(PCNT0, PCNT_IF_OF);
}


/**************************************************************************//**
 * @brief   This interrupt is triggered at every second by the PCNT
 *
 *****************************************************************************/
void PCNT0_IRQHandler(void)
{
  PCNT_IntClear(PCNT0, PCNT_IF_OF);

  seconds++;

  return;
}


/**************************************************************************//**
 * @brief   Select a clock source for HF clock, optionally disable other clocks.
 *
 * @param[in] hfClockSelect      The HF clock to select.
 * @param[in] clockDisableMask   Bit masks with clocks to disable.
 *****************************************************************************/
static void selectClock(CMU_Select_TypeDef hfClockSelect,
                          uint32_t clockDisableMask)
{
  /* Select HF clock. */
  CMU_ClockSelectSet(cmuClock_HF, hfClockSelect);

  /* Disable unwanted clocks. */
  CMU->OSCENCMD     = clockDisableMask;

  /* Turn off clock enables. */
  CMU->HFPERCLKEN0  = 0x00000000;
  CMU->HFBUSCLKEN0  = 0x00000000;
  CMU->LFACLKEN0    = 0x00000000;
  CMU->LFBCLKEN0    = 0x00000000;
  CMU->LFACLKSEL    = 0x00000000;
  CMU->LFBCLKSEL    = 0x00000000;
  CMU->LFECLKSEL    = 0x00000000;
}
