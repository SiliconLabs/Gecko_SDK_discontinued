/**************************************************************************//**
 * @file
 * @brief Energy Mode demo for SLSTK3401A
 * @brief Demo for energy mode current consumption testing.
 * @version 5.1.2
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
#include "em4config.h"

/* Frequency of RTCC (CCV1) pulses on PRS channel 2. */
#define RTC_PULSE_FREQUENCY    (LS013B7DH03_POLARITY_INVERSION_FREQUENCY / 2)
#define SLEEP_TIME             (1)

static volatile bool      displayEnabled = false; /* Status of LCD display. */
static volatile bool      enterEM4 = false;
static volatile uint32_t  seconds = 0;     /* Seconds elapsed since reset.  */

static DISPLAY_Device_t displayDevice;    /* Display device handle.         */

static void CheckEM4Entry(void);
static void EnterEMode(int mode, uint32_t secs);
static void GpioSetup(void);
static void PcntInit(void);

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_STK_DEFAULT;
  CMU_HFXOInit_TypeDef hfxoInit = CMU_HFXOINIT_STK_DEFAULT;
  EMU_EM4Init_TypeDef em4init = EMU_EM4INIT_DEFAULT;

  /* Chip errata */
  CHIP_Init();

  /* Init DCDC regulator and HFXO with kit specific parameters */
  EMU_DCDCInit(&dcdcInit);
  CMU_HFXOInit(&hfxoInit);
  EMU_EM4Init(&em4init);

  /* Select ULFRCO as clock source for LFA */
  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_ULFRCO);

  /* Setup GPIO for pushbuttons. */
  GpioSetup();

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
  PcntInit();

  printf( "\n\n Cycling through"
          "\n energy modes"
          "\n EM0-EM3"
          "\n\n Push " EM4_NON_WU_PB_STR " to"
          "\n enter EM4\n\n\n\n" );

  /* Turn on LFXO to be able to see the difference between EM2 and EM3. */
  CMU_OscillatorEnable(cmuOsc_LFXO, true, false );

  for (;;)
  {
    printf("\r      EM0");
    EnterEMode(0, SLEEP_TIME);
    CheckEM4Entry();

    printf("\r      EM1");
    EnterEMode(1, SLEEP_TIME);
    CheckEM4Entry();

    printf("\r      EM2");
    EnterEMode(2, SLEEP_TIME);
    CheckEM4Entry();

    printf("\r      EM3");
    EnterEMode(3, SLEEP_TIME);
    CheckEM4Entry();
  }
}

/**************************************************************************//**
 * @brief  Check if PB0 is pushed and EM4 entry is due.
 *****************************************************************************/
static void CheckEM4Entry(void)
{
  if (enterEM4)
  {
    enterEM4 = false;

    printf("\f\n\n Ready to enter"
           "\n energy mode"
           "\n EM4"
           "\n\n Push " EM4_NON_WU_PB_STR " to"
           "\n enter EM4"
           "\n\n Wakeup from"
           "\n EM4 by pushing"
           "\n " EM4_WU_PB_STR " or the"
           "\n reset button");

    while (enterEM4 == false);
    enterEM4 = false;

    /* Disable the RTC and PCNT. */
    printf("\f");
    displayEnabled = false;
    NVIC_DisableIRQ(RTCC_IRQn);
    NVIC_DisableIRQ(PCNT0_IRQn);

    /* Power down the display. */
    displayDevice.pDisplayPowerOn(&displayDevice, false);

    /* Enable wakeup from EM4 on PB1 GPIO pin. */
    GPIO_EM4EnablePinWakeup(EM4_WU_PB_EN, 0);
    EMU_EnterEM4();
  }
}

/**************************************************************************//**
 * @brief   Enter a Energy Mode for a given number of seconds.
 *
 * @param[in] mode  Energy Mode to enter (0..3).
 * @param[in] secs  Time to stay in Energy Mode <mode>.
 *****************************************************************************/
static void EnterEMode(int mode, uint32_t secs)
{
  if (secs)
  {
    uint32_t startTime = seconds;

    if (mode == 0)
    {
      int cnt = 0;

      while ((seconds - startTime) < secs)
      {
        if      (cnt == 0) printf("\r  - - EM0 - -");
        else if (cnt == 1) printf("\r  \\ \\ EM0 / /");
        else if (cnt == 2) printf("\r  | | EM0 | |");
        else if (cnt == 3) printf("\r  / / EM0 \\ \\");
        cnt = (cnt + 1) % 4;
        if (enterEM4)
        {
          printf("\r      EM0    ");
          return;
        }
      }
      printf("\r      EM0    ");
    }
    else
    {
      while ((seconds - startTime) < secs)
      {
        switch (mode)
        {
          case 1: EMU_EnterEM1(); break;
          case 2: EMU_EnterEM2(true); break;
          case 3: EMU_EnterEM3(true); break;
        }
        if (enterEM4)
        {
          return;
        }
      }
    }
  }
}

/**************************************************************************//**
 * @brief Setup GPIO interrupt for pushbuttons.
 *****************************************************************************/
static void GpioSetup(void)
{
  /* Enable GPIO clock. */
  CMU_ClockEnable(cmuClock_GPIO, true);

  /* Configure PF6 as input and enable interrupt. */
  GPIO_PinModeSet(EM4_NON_WU_PB_PORT, EM4_NON_WU_PB_PIN, gpioModeInputPull, 1);
  GPIO_IntConfig(EM4_NON_WU_PB_PORT, EM4_NON_WU_PB_PIN, false, true, true);

  NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
  NVIC_EnableIRQ(GPIO_EVEN_IRQn);

  NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
  NVIC_EnableIRQ(GPIO_ODD_IRQn);

  /* Configure PF7 as input. */
  GPIO_PinModeSet(EM4_WU_PB_PORT, EM4_WU_PB_PIN, gpioModeInputPullFilter, 1);
}

/**************************************************************************//**
* @brief Unified GPIO Interrupt handler (pushbuttons)
*        PB0 Starts selected test
*        PB1 Cycles through the available tests
*****************************************************************************/
void GPIO_Unified_IRQ(void) {
  /* Get and clear all pending GPIO interrupts */
  uint32_t interruptMask = GPIO_IntGet();
  GPIO_IntClear(interruptMask);

  /* Act on interrupts */
  if (interruptMask & (1 << EM4_NON_WU_PB_PIN)) {
    /* ALT_PB */
    enterEM4 = true;
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
 *****************************************************************************/
void PcntInit(void)
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
