/**************************************************************************//**
 * @file main.c
 * @brief Clock example
 * @version 5.1.2
 *
 * This example shows how to optimize your code in order to drive
 * a graphical display in an energy friendly way.
 *
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
#include <math.h>
#include <stdio.h>
#include <time.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_rtc.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "display.h"
#include "textdisplay.h"
#include "retargettextdisplay.h"
#include "bspconfig.h"

/* Frequency of RTC clock. */
#define RTC_FREQUENCY    (64)

/* Clock mode */
typedef enum
{
  CLOCK_MODE_ANALOG,
  CLOCK_MODE_DIGITAL
} ClockMode_t;
ClockMode_t clockMode                = CLOCK_MODE_ANALOG;

/* RTC callback parameters. */
static void (*rtcCallback)(void*) = 0;
static void * rtcCallbackArg         = 0;

/* The current time reference. Number of seconds since midnight
 * January 1, 1970.  */
static volatile time_t curTime       = 0;

/* RTC interrupt counter */
static volatile int rtcIrqCount      = 0;

/* Flag to check when we should redraw a frame */
static volatile bool updateDisplay   = true;

static volatile bool isAdjustingTime = false;

/* Analog clock prototypes */
void ANALOG_Init(void);
void ANALOG_ClockFaceDraw(struct tm *t);
void ANALOG_ForceRedraw(void);

#if !defined(__CROSSWORKS_ARM) && defined(__GNUC__)
/* sniprintf does not process floats, but occupy less flash memory ! */
#define snprintf    sniprintf
#endif

/* Holds pushbutton number of even/odd pins. */
static uint32_t evenPin;
static uint32_t oddPin;

/**************************************************************************//**
 * @brief Setup GPIO interrupt for pushbuttons.
 *****************************************************************************/
static void GpioSetup(void)
{
  /* Enable GPIO clock */
  CMU_ClockEnable(cmuClock_GPIO, true);

  /* Configure pushbutton gpio's as input and enable interrupt  */
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInputPull, 1);
  GPIO_IntConfig( BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, false, true, true);
  GPIO_PinModeSet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, gpioModeInputPull, 1);
  GPIO_IntConfig( BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, false, true, true);

  NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
  NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
  NVIC_EnableIRQ(GPIO_EVEN_IRQn);
  NVIC_EnableIRQ(GPIO_ODD_IRQn);

  evenPin = BSP_GPIO_PB0_PIN & 1 ? BSP_GPIO_PB1_PIN : BSP_GPIO_PB0_PIN;
  oddPin  = BSP_GPIO_PB0_PIN & 1 ? BSP_GPIO_PB0_PIN : BSP_GPIO_PB1_PIN;
}

/**************************************************************************//**
 * @brief Take action in GPIO pin interrupts.
 *        Switches between analog and digital clock modes,
 *        or increments the time by one minute.
 *****************************************************************************/
static void pinIrq(uint32_t pin)
{
  /* Acknowledge interrupt */
  GPIO_IntClear(1 << pin);

  if (pin == BSP_GPIO_PB0_PIN)
  {
    /* Toggle clock face (analog/digital). */
    if (clockMode == CLOCK_MODE_ANALOG)
    {
      clockMode = CLOCK_MODE_DIGITAL;
    }
    else
    {
      ANALOG_ForceRedraw();
      clockMode = CLOCK_MODE_ANALOG;
    }
  }
  else
  {
    /* Increase time by 1 minute (60 seconds). */
    curTime += 60;
    isAdjustingTime = true;
  }

  updateDisplay = true;
}

/**************************************************************************//**
 * @brief GPIO EVEN Interrupt handler.
 *****************************************************************************/
void GPIO_EVEN_IRQHandler(void)
{
  pinIrq(evenPin);
}

/**************************************************************************//**
 * @brief GPIO ODD Interrupt handler.
 *        Increments the time by one minute.
 *****************************************************************************/
void GPIO_ODD_IRQHandler(void)
{
  pinIrq(oddPin);
}

/**************************************************************************//**
 * @brief   Register a callback function at the given frequency.
 *
 * @param[in] pFunction  Pointer to function that should be called at the
 *                       given frequency.
 * @param[in] argument   Argument to be given to the function.
 * @param[in] frequency  Frequency at which to call function at.
 *
 * @return  0 for successful or
 *         -1 if the requested frequency does not match the RTC frequency.
 *****************************************************************************/
int RtcIntCallbackRegister(void (*pFunction)(void*),
                           void* argument,
                           unsigned int frequency)
{
  /* Verify that the requested frequency is the same as the RTC frequency.*/
  if (RTC_FREQUENCY != frequency)
    return -1;

  rtcCallback    = pFunction;
  rtcCallbackArg = argument;

  return 0;
}

/**************************************************************************//**
 * @brief   Set up RTC to generate an interrupt every second.
 *****************************************************************************/
void RtcInit(void)
{
  RTC_Init_TypeDef rtcInit = RTC_INIT_DEFAULT;

  /* Enable LE domain registers */
  CMU_ClockEnable(cmuClock_CORELE, true);

  /* Enable LFXO as LFACLK in CMU. This will also start LFXO */
  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);

  /* Enable RTC clock */
  CMU_ClockEnable(cmuClock_RTC, true);

  /* Initialize RTC */
  rtcInit.enable   = false;  /* Do not start RTC after initialization is complete. */
  rtcInit.debugRun = false;  /* Halt RTC when debugging. */
  rtcInit.comp0Top = true;   /* Wrap around on COMP0 match. */
  RTC_Init(&rtcInit);

  /* Interrupt at specified frequency. */
  RTC_CompareSet(0, (CMU_ClockFreqGet(cmuClock_RTC) / RTC_FREQUENCY) - 1);

  /* Enable interrupt */
  NVIC_EnableIRQ(RTC_IRQn);
  RTC_IntEnable(RTC_IEN_COMP0);

  /* Start counter */
  RTC_Enable(true);
}


/**************************************************************************//**
 * @brief   This interrupt is triggered every second by the RTC.
 *****************************************************************************/
void RTC_IRQHandler(void)
{
  RTC_IntClear(RTC_IF_COMP0);

  /* Execute callback function if registered. */
  if (rtcCallback)
    (*rtcCallback)(rtcCallbackArg);

  if (RTC_FREQUENCY == ++rtcIrqCount)
  {
    /* One second reached, reset irqCount */
    rtcIrqCount = 0;

    /* Increase time with 1s */
    curTime++;

    /* Notify main loop to redraw clock on display. */
    updateDisplay = true;
  }
}

/**************************************************************************//**
 * @brief  Increments the clock quickly while PB1 is pressed.
 *         A callback is used to update either the analog or the digital clock.
 *****************************************************************************/
void AdjustTime(void (*drawClock)(struct tm*))
{
  unsigned int i = 0;
  struct tm    *time;

  /* Wait 0.5 seconds before starting to adjust quickly */
  int waitForRtcIrqCount = (rtcIrqCount + RTC_FREQUENCY / 2) % RTC_FREQUENCY;

  isAdjustingTime = false;
  while (rtcIrqCount != waitForRtcIrqCount)
  {
    /* Return if the button is released */
    if (GPIO_PinInGet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN) == 1)
      return;

    /* Keep updating the second counter while waiting */
    if (updateDisplay == true)
    {
      time = localtime((time_t const *) &curTime);
      drawClock(time);
    }

    EMU_EnterEM2(false);
  }

  /* Keep incrementing the time while the button is pressed */
  while (GPIO_PinInGet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN) == 0)
  {
    if (i % 1000 == 0)
    {
      /* Increase time by 1 minute (60 seconds). */
      curTime += 60;

      time = localtime((time_t const *) &curTime);
      drawClock(time);
    }

    i++;
  }
}

/**************************************************************************//**
 * @brief  Shows an analog clock on the display.
 *****************************************************************************/
void AnalogClockShow(void)
{
  struct tm *time = localtime((time_t const *) &curTime);

  ANALOG_Init();

  ANALOG_ClockFaceDraw(time);

  while (CLOCK_MODE_ANALOG == clockMode)
  {
    if (updateDisplay)
    {
      /* Convert time format */
      time = localtime((time_t const *) &curTime);

      /* Draw clock face to frame buffer */
      ANALOG_ClockFaceDraw(time);

      updateDisplay = false;

      if (isAdjustingTime)
      {
        AdjustTime(ANALOG_ClockFaceDraw);
      }
    }

    /* Sleep between each frame update */
    EMU_EnterEM2(false);
  }
}

/**************************************************************************//**
 * @brief  Updates the digital clock.
 *****************************************************************************/
void UpdateDigitalClock(struct tm *time)
{
#ifdef USE_PRINTF
#ifdef TEXTDISPLAY_FONT_6x8
  printf("\r      ");
#endif
#ifdef TEXTDISPLAY_FONT_8x8
  printf("\r    ");
#endif
#ifdef TEXTDISPLAY_NUMBER_FONT_16x20
  printf("\r");
#endif
  printf("%02d:%02d:%02d", time->tm_hour, time->tm_min, time->tm_sec);

#else  /* USE_PRINTF */
  putchar('\r');
  putchar('0' + time->tm_hour / 10);
  putchar('0' + time->tm_hour % 10);
  putchar(':');
  putchar('0' + time->tm_min / 10);
  putchar('0' + time->tm_min % 10);
  putchar(':');
  putchar('0' + time->tm_sec / 10);
  putchar('0' + time->tm_sec % 10);
#endif  /* USE_PRINTF */
}

/**************************************************************************//**
 * @brief  Shows an digital clock on the display.
 *****************************************************************************/
void DigitalClockShow(void)
{
  struct tm *time;

  /* Clear screen and Set cursor at wanted position. */
#ifdef USE_PRINTF
#if defined TEXTDISPLAY_FONT_8x8 || defined TEXTDISPLAY_FONT_6x8
  printf("\f\n\n\n\n\n\n\n\n");
#endif
#ifdef TEXTDISPLAY_NUMBER_FONT_16x20
  printf("\f\n\n\n");
#endif

#else /* USE_PRINTF */
  puts("\f\n\n");
#endif /* USE_PRINTF */

  while (CLOCK_MODE_DIGITAL == clockMode)
  {
    if (updateDisplay)
    {
      /* Convert time format */
      time = localtime((time_t const *) &curTime);

      UpdateDigitalClock(time);

      updateDisplay = false;

      if (isAdjustingTime)
      {
        AdjustTime(UpdateDigitalClock);
      }
    }

    EMU_EnterEM2(true);
  }
}


/**************************************************************************//**
 * @brief  Main function of clock example.
 *****************************************************************************/
int main(void)
{
  /* Chip errata */
  CHIP_Init();

  /* Use the 21 MHz band in order to decrease time spent awake */
  CMU_ClockSelectSet( cmuClock_HF, cmuSelect_HFRCO  );
  CMU_HFRCOBandSet( cmuHFRCOBand_21MHz );

  /* Setup GPIO for pushbuttons. */
  GpioSetup();

  /* Initialize the display module. */
  DISPLAY_Init();

  /* Retarget stdio to the display. */
  if (TEXTDISPLAY_EMSTATUS_OK != RETARGET_TextDisplayInit())
  {
    /* Text display initialization failed. */
    while(1);
  }

  /* Set RTC to generate interrupt every second */
  RtcInit();

  /* Enter infinite loop that switches between analog and digitcal clock
   * modes, toggled by pressing the button PB0. */
  while (1)
  {
    if (CLOCK_MODE_ANALOG == clockMode)
      AnalogClockShow();
    else
      DigitalClockShow();
  }
}
