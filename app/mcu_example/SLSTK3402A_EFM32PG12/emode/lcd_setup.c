/**************************************************************************//**
 * @file lcd_setup.c
 * @brief Setup LCD for energy mode demo
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

// Frequency of RTC (COMP0) pulses on PRS.
#define RTC_PULSE_FREQUENCY    (LS013B7DH03_POLARITY_INVERSION_FREQUENCY)

static volatile Energy_Mode_TypeDef eMode; // Selected energy mode.
static volatile int startTest;             // Start selected energy mode test.
static volatile bool displayEnabled;       // Status of LCD display.
static volatile uint32_t seconds = 0;      // Seconds elapsed since reset.
static DISPLAY_Device_t displayDevice;     // Display device handle.

static void gpioSetup(void);
static void pcntInit(void);

/**************************************************************************//**
 * @brief  Selects eMode using the LCD and buttons.
 *
 * @return Energy_Mode_TypeDef selected eMode.
 *****************************************************************************/
Energy_Mode_TypeDef LCD_SelectMode(void)
{
  Energy_Mode_TypeDef currentEMode;

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
         "  start test \n\n");

  startTest = false;
  eMode = (Energy_Mode_TypeDef)0;
  currentEMode = NUM_EMODES;

  while (!startTest)
  {
    // Mode change ? If yes, update the display.
    if (eMode != currentEMode)
    {
      currentEMode = eMode;
      switch (eMode)
      {
        case EM0_HFXO_40MHZ_WHILE:
          printf("\r  EM0 38.4MHz   \n"
                   "  No DC/DC      \n"
                   "  (while loop)  \n");
          break;

        case EM0_HFRCO_38MHZ_PRIME:
          printf("\r  EM0 38MHz     \n"
                   "  No DC/DC      \n"
                   "  (primes calc) \n");
          break;

        case EM0_HFRCO_38MHZ_WHILE:
          printf("\r  EM0 38MHz     \n"
                   "  No DC/DC      \n"
                   "  (while loop)  \n");
          break;

        case EM0_HFRCO_38MHZ_COREMARK:
          printf("\r  EM0 38MHz     \n"
                   "  No DC/DC      \n"
                   "  (CoreMark)    \n");
          break;

        case EM0_HFRCO_26MHZ_WHILE:
          printf("\r  EM0 26MHz     \n"
                   "  No DC/DC      \n"
                   "  (while loop)  \n");
          break;

        case EM0_HFRCO_1MHZ_WHILE:
          printf("\r  EM0 1MHz      \n"
                   "  No DC/DC      \n"
                   "  (while loop)  \n");
          break;

        case EM1_HFXO_40MHZ:
          printf("\r  EM1 38.4MHz   \n"
                   "  No DC/DC      \n");
          break;

        case EM1_HFRCO_38MHZ:
          printf("\r  EM1 38MHz     \n"
                   "  No DC/DC      \n");
          break;

        case EM1_HFRCO_26MHZ:
          printf("\r  EM1 26MHz     \n"
                   "  No DC/DC      \n");

          break;

        case EM1_HFRCO_1MHZ:
          printf("\r  EM1 1MHz      \n"
                   "  No DC/DC      \n");
          break;

#if defined(ADVANCED_LOWPOWER_FEATURES)
        case EM0_HFXO_40MHZ_WHILE_LP:
          printf("\r  EM0 38.4MHz   \n"
                   "  DC/DC LP      \n"
                   "  (while loop)  \n");
          break;

        case EM0_HFRCO_38MHZ_PRIME_LP:
          printf("\r  EM0 38MHz     \n"
                   "  DC/DC LP      \n"
                   "  (primes calc) \n");
          break;

        case EM0_HFRCO_38MHZ_WHILE_LP:
          printf("\r  EM0 38MHz     \n"
                   "  DC/DC LP      \n"
                   "  (while loop)  \n");
          break;

        case EM0_HFRCO_38MHZ_COREMARK_LP:
          printf("\r  EM0 38MHz     \n"
                   "  DC/DC LP      \n"
                   "  (CoreMark)    \n");
          break;

        case EM0_HFRCO_26MHZ_WHILE_LP:
          printf("\r  EM0 26MHz     \n"
                   "  DC/DC LP      \n"
                   "  (while loop)  \n");
          break;

        case EM0_HFRCO_1MHZ_WHILE_LP:
          printf("\r  EM0 1MHz      \n"
                   "  DC/DC LP      \n"
                   "  (while loop)  \n");
          break;

        case EM1_HFXO_40MHZ_LP:
          printf("\r  EM1 38.4MHz   \n"
                   "  DC/DC LP      \n"
                   "                \n");
          break;

        case EM1_HFRCO_38MHZ_LP:
          printf("\r  EM1 38MHz     \n"
                   "  DC/DC LP      \n");
          break;

        case EM1_HFRCO_26MHZ_LP:
          printf("\r  EM1 26MHz     \n"
                   "  DC/DC LP      \n");

          break;

        case EM1_HFRCO_1MHZ_LP:
          printf("\r  EM1 1MHz      \n"
                   "  DC/DC LP      \n");
          break;
#endif

        case EM2_LFXO_RTCC:
          printf("\r  EM2 RTCC LFXO \n"
                   "  (Full RAM)    \n");
          break;

        case EM2_LFRCO_RTCC:
          printf("\r  EM2 RTCC LFRCO\n"
                   "  (Min RAM)     \n");
          break;

        case EM3_ULFRCO_CRYO:
          printf("\r EM3 CRYO ULFRCO\n"
                   "  (Full RAM)    \n");
          break;

        case EM4H_LFXO_RTCC:
          printf("\r EM4H RTCC LFXO \n"
                   "  (128b RAM)    \n");
          break;

        case EM4H_ULFRCO_CRYO:
          printf("\rEM4H CRYO ULFRCO\n"
                   "  (128b RAM)    \n");
          break;

        case EM4H:
          printf("\r   EM4H         \n"
                   "  (128b RAM)    \n");
          break;

        case EM4S:
          printf("\r   EM4S         \n"
                   "  (No RAM)      \n");
          break;

        case EM0_HFXO_40MHZ_WHILE_DCDC_DCM:
          printf("\r  EM0 38.4MHz   \n"
                   "  DC/DC LN DCM  \n"
                   "  (while loop)  \n");
          break;

        case EM0_HFRCO_38MHZ_PRIME_DCDC_DCM:
          printf("\r  EM0 38MHz     \n"
                   "  DC/DC LN DCM  \n"
                   "  (primes calc) \n");
          break;

        case EM0_HFRCO_38MHZ_WHILE_DCDC_DCM:
          printf("\r  EM0 38MHz     \n"
                   "  DC/DC LN DCM  \n"
                   "  (while loop)  \n");
          break;

        case EM0_HFRCO_38MHZ_COREMARK_DCDC_DCM:
          printf("\r  EM0 38MHz     \n"
                   "  DC/DC LN DCM  \n"
                   "  (CoreMark)    \n");
          break;

        case EM0_HFRCO_26MHZ_WHILE_DCDC_DCM:
          printf("\r  EM0 26MHz     \n"
                   "  DC/DC LN DCM  \n"
                   "  (while loop)  \n");
          break;

        case EM0_HFXO_40MHZ_WHILE_DCDC_CCM:
          printf("\r  EM0 38.4MHz   \n"
                   "  DC/DC LN CCM  \n"
                   "  (while loop)  \n");
          break;

        case EM0_HFRCO_38MHZ_PRIME_DCDC_CCM:
          printf("\r  EM0 38MHz     \n"
                   "  DC/DC LN CCM  \n"
                   "  (primes calc) \n");
          break;

        case EM0_HFRCO_38MHZ_WHILE_DCDC_CCM:
          printf("\r  EM0 38MHz     \n"
                   "  DC/DC LN CCM  \n"
                   "  (while loop)  \n");
          break;

        case EM0_HFRCO_38MHZ_COREMARK_DCDC_CCM:
          printf("\r  EM0 38MHz     \n"
                   "  DC/DC LN CCM  \n"
                   "  (CoreMark)    \n");
          break;

        case EM0_HFRCO_26MHZ_WHILE_DCDC_CCM:
          printf("\r  EM0 26MHz     \n"
                   "  DC/DC LN CCM  \n"
                   "  (while loop)  \n");
          break;

        case EM1_HFXO_40MHZ_DCDC_DCM:
          printf("\r   EM1 38.4MHz  \n"
                   "   DC/DC LN DCM \n"
                   "                \n");
          break;

        case EM1_HFRCO_38MHZ_DCDC_DCM:
          printf("\r   EM1 38MHz    \n"
                   "   DC/DC LN DCM \n");
          break;

        case EM1_HFRCO_26MHZ_DCDC_DCM:
          printf("\r   EM1 26MHz    \n"
                   "   DC/DC LN DCM \n");
          break;

        case EM1_HFXO_40MHZ_DCDC_CCM:
          printf("\r   EM1 38.4MHz  \n"
                   "   DC/DC LN CCM \n");
          break;

        case EM1_HFRCO_38MHZ_DCDC_CCM:
          printf("\r   EM1 38MHz    \n"
                   "   DC/DC LN CCM \n");
          break;

        case EM1_HFRCO_26MHZ_DCDC_CCM:
          printf("\r   EM1 26MHz    \n"
                   "   DC/DC LN CCM \n");
          break;

        case EM2_LFXO_RTCC_DCDC_LPM:
          printf("\r  EM2 RTCC LFXO \n"
                   "  With DC/DC    \n"
                   "  (Full RAM)    \n");
          break;

        case EM2_LFRCO_RTCC_DCDC_LPM:
          printf("\r  EM2 RTCC LFRCO\n"
                   "  With DC/DC    \n"
                   "  (Min RAM)     \n");
          break;

        case EM3_ULFRCO_CRYO_DCDC:
          printf("\r EM3 CRYO ULFRCO\n"
                   "  With DC/DC    \n"
                   "  (Full RAM)    \n");
          break;

        case EM4H_LFXO_RTCC_DCDC:
          printf("\r EM4H RTCC LFXO \n"
                   "  With DC/DC    \n"
                   "  (128b RAM)    \n");
          break;

        case EM4H_ULFRCO_CRYO_DCDC:
          printf("\rEM4H CRYO ULFRCO\n"
                   "  With DC/DC    \n"
                   "  (128b RAM)    \n");
          break;

        case EM4H_DCDC:
          printf("\r   EM4H         \n"
                   "  With DC/DC    \n"
                   "  (128b RAM)    \n");
          break;

        case EM4S_DCDC:
          printf("\r   EM4S         \n"
                   "  With DC/DC    \n"
                   "  (No RAM)      \n");
          break;
        default:
          EFM_ASSERT(false);
          break;
      }
      printf(TEXTDISPLAY_ESC_SEQ_CURSOR_HOME_VT100);
      printf("\n\n\n\n\n\n\n\n\n\n\n\n");
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
    eMode = (Energy_Mode_TypeDef)(((int)eMode + 1) % (int)NUM_EMODES);
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
