/**************************************************************************//**
 * @file
 * @brief Cap sense touch demo.
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

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

/* Device Suppot Library */
#include "em_device.h"
#include "em_chip.h"

/* Drivers */
#include "vddcheck.h"
#include "segmentlcd.h"
#include "capsense.h"
#include "rtcdriver.h"
#include "bsp_trace.h"
#include "em_emu.h"
#include "em_cmu.h"
#include "em_gpio.h"

#define DEMOMODES           3

#define DEMOMODE_SCROLLTEXT 0
#define DEMOMODE_BARS       1
#define DEMOMODE_VALUES     2

volatile uint32_t demoMode;
volatile uint32_t subMode;
volatile uint32_t subModes;

volatile int ledLevels[] = {0, 0, 0, 0};

const char *message = "CAPSENSE TOUCH DEMO  ";

/* Enable or disable vboost */
bool vboost = false;

/** Timer used for bringing the system back to EM0. */
static RTCDRV_TimerID_t xTimerForWakeUp;

/**************************************************************************//**
 * @brief GPIO Interrupt handler (PB10)
 *****************************************************************************/
void GPIO_ODD_IRQHandler(void)
{
  uint32_t max;

  /* Acknowledge interrupt */
  GPIO->IFC = (1 << 9);

  /* Select next submode */
  max = subModes;
  subMode = (subMode + 1) % max;
}

/**************************************************************************//**
 * @brief GPIO Interrupt handler (PB10)
 *****************************************************************************/
void GPIO_EVEN_IRQHandler(void)
{
  /* Acknowledge interrupt */
  GPIO->IFC = (1 << 10);

  /* Set next demo */
  demoMode = (demoMode + 1) % DEMOMODES;

  /* Reset submode */
  subMode = 0;
  /* Set the maximum number of subModes depending on type */
  switch (demoMode)
  {
    case (DEMOMODE_VALUES):
      subModes = 5;
      break;
    default:
      subModes = 1;
      break;
  }
}

/**************************************************************************//**
 * @brief Setup GPIO interrupt to change demo mode
 *****************************************************************************/
void gpioSetup(void)
{
  /* Enable GPIO clock */
  CMU_ClockEnable(cmuClock_GPIO, true);

  /* Configure PB10 as input */
  GPIO_PinModeSet(gpioPortB, 10, gpioModeInput, 0);

  /* Set falling edge interrupt */
  GPIO_IntConfig(gpioPortB, 10, false, true, true);
  NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
  NVIC_EnableIRQ(GPIO_EVEN_IRQn);

  /* Configure PB9 as input */
  GPIO_PinModeSet(gpioPortB, 9, gpioModeInput, 0);

  /* Set falling edge interrupt */
  GPIO_IntConfig(gpioPortB, 9, false, true, true);
  NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
  NVIC_EnableIRQ(GPIO_ODD_IRQn);
}

/**************************************************************************//**
 * @brief Update rings according to slider position
 * @par sliderPos The current Slider position
 *****************************************************************************/
void capSenseAringUpdate(int sliderPos)
{
  int i;
  int stop;

  if (sliderPos == -1)
  {
    /* No ring if touch slider is not touched */
    stop = -1;
  }
  else
  {
    /* Map 8 segments to 48 slider positions */
    stop = (sliderPos * 8) / 48;
  }

  /* Draw ring */
  for (i=0; i < 8; i++)
  {
    if (i <= stop )
    {
      SegmentLCD_ARing(i, 1);
    }
    else
    {
      SegmentLCD_ARing(i, 0);
    }
  }
}

/**************************************************************************//**
 * @brief Cap sense values demo
 *****************************************************************************/
void capSenseBars(void)
{
  int barNum;
  int sliderPos;
  const char bars[3] = {'{', '!', '1' };
  char msg[8];

  sliderPos = CAPSENSE_getSliderPosition();
  SegmentLCD_Number(sliderPos);

  if (sliderPos == -1)
  {
    SegmentLCD_Write("SLIDER");
  }
  else
  {
    /* Clear the msg string */
    snprintf(msg, 7, "         ");
    /* There are 21 possible "bars" on the display, while there are 48 slider
     * positions. This maps these 48 into 21 slider positions. */
    barNum = (sliderPos * 21) / 48;

    msg[barNum / 3] = bars[barNum %3];

    SegmentLCD_Write(msg);
  }
}

/**************************************************************************//**
 * @brief Cap sense values demo
 *****************************************************************************/
void capSenseValues(void)
{
  char msg[8];
  uint8_t channel;

  switch (subMode)
  {
  case(0):
    snprintf(msg, 7, "Pad 0");
    channel = SLIDER_PART0_CHANNEL;
    break;
  case(1):
    snprintf(msg, 7, "Pad 1");
    channel = SLIDER_PART1_CHANNEL;
    break;
  case(2):
    snprintf(msg, 7, "Pad 2");
    channel = SLIDER_PART2_CHANNEL;
    break;
  case(3):
    snprintf(msg, 7, "Pad 3");
    channel = SLIDER_PART3_CHANNEL;
    break;
  default:
    snprintf(msg, 7, "Gecko");
    channel = BUTTON_CHANNEL;
    break;
  }
  /* get the channel number for a given sensor segment */

  SegmentLCD_Write(msg);
  SegmentLCD_Number(CAPSENSE_getNormalizedVal(channel));
}

/**************************************************************************//**
 * @brief  ScrollText demo
 *****************************************************************************/
void capSenseScrollText(void)
{
  char msg[10];
  int sliderPos;
  static int oldSliderPos = -2;

  int offset;

  sliderPos = CAPSENSE_getSliderPosition();
  if (oldSliderPos != sliderPos)
  {
    oldSliderPos = sliderPos;
    SegmentLCD_Number(sliderPos);

    if (sliderPos == -1)
      sliderPos = 0;

    offset = ((strlen(message) - 7) * sliderPos) / 48;

    snprintf(msg, 8, "%s", message + offset);
    SegmentLCD_Write(msg);
  }
}

/**************************************************************************//**
 * @brief  Capsense demo loop
 *****************************************************************************/
void capSenseDemo(void)
{
  int32_t slider;
  bool oldBoost = vboost;

  /* Main loop */
  while (1)
  {
    /* Sense the values */
    CAPSENSE_Sense();
    slider = CAPSENSE_getSliderPosition();
    /* Light Gecko if Gecko is pressed. */
    SegmentLCD_Symbol(LCD_SYMBOL_GECKO, CAPSENSE_getPressed(BUTTON_CHANNEL));
    capSenseAringUpdate(slider);

    /* Check for change in input voltage. Enable vboost if necessary */
    /* Initialize voltage comparator */
    VDDCHECK_Init();

    /* Check if voltage is below 3V, if so use voltage boost */
    if (VDDCHECK_LowVoltage(2.9))
    {
      vboost = true;
      if ( oldBoost != vboost )
      {
        /* Enable vboost */
        SegmentLCD_Init(vboost);
        /* Use antenna symbol to signify enabling of vboost */
        SegmentLCD_Symbol(LCD_SYMBOL_ANT, vboost);
      }
      oldBoost = vboost;
    }
    else
    {
      vboost = false;
    }

    switch (demoMode)
    {
    case (DEMOMODE_SCROLLTEXT):
      capSenseScrollText();
      break;
    case (DEMOMODE_BARS):
      capSenseBars();
      break;
    case (DEMOMODE_VALUES):
      capSenseValues();
      break;
    default:
      break;
    }
    RTCDRV_StartTimer( xTimerForWakeUp, rtcdrvTimerTypeOneshot, 100, NULL, NULL);
    EMU_EnterEM2(true);
  }
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  /* Chip errata */
  CHIP_Init();

  /* Ensure core frequency has been updated */
  SystemCoreClockUpdate();

  /* If first word of user data page is non-zero, enable eA Profiler trace */
  BSP_TraceProfilerSetup();

  /* Initialize RTC */
  RTCDRV_Init();
  RTCDRV_AllocateTimer( &xTimerForWakeUp);

  /* Initialize voltage comparator to check supply voltage */
  VDDCHECK_Init();

  /* Check if voltage is below 3V, if so use voltage boost */
  if (VDDCHECK_LowVoltage(2.9))
  {
    vboost = true;
  }
  else
  {
    vboost = false;
  }

  /* Initialize capsense */
  CAPSENSE_Init();

  /* Initialize GPIO interrupts */
  gpioSetup();

  /* Initialize LCD, use boost if necessary. */
  SegmentLCD_Init(vboost);

  capSenseDemo();

  return 0;
}
