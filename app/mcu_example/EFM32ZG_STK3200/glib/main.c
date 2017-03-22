/**************************************************************************//**
 * @file main.c
 * @brief GLIB example for EFM32ZG-STK3200
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
#include "em_pcnt.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "display.h"
#include "dmd.h"
#include "glib.h"
#include "bspconfig.h"

/* Frequency of RTC (COMP0) pulses on PRS channel 2. */
#define RTC_PULSE_FREQUENCY    (LS013B7DH03_POLARITY_INVERSION_FREQUENCY)

#define GLIB_FONT_WIDTH   (glibContext.font.fontWidth + glibContext.font.charSpacing)
#define GLIB_FONT_HEIGHT  (glibContext.font.fontHeight)

/* Center of display */
#define CENTER_X (glibContext.pDisplayGeometry->xSize / 2)
#define CENTER_Y (glibContext.pDisplayGeometry->ySize / 2)

#define MAX_X (glibContext.pDisplayGeometry->xSize - 1)
#define MAX_Y (glibContext.pDisplayGeometry->ySize - 1)

#define MIN_X           0
#define MIN_Y           0

#define INIT_DEMO_NO    0

/* The GLIB context */
static GLIB_Context_t   glibContext;

/* The current time reference. Number of seconds since midnight
 * January 1, 1970.  */
static time_t curTime = 0;

static volatile uint32_t demoNo = INIT_DEMO_NO;

/* Forward static function declararations */
static void GlibDemo(void);

/**************************************************************************//**
 * @brief Setup GPIO interrupt for pushbuttons.
 *****************************************************************************/
static void GpioSetup(void)
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
*        PB0 Reset to test zero
*        PB1 Next test
*****************************************************************************/
void GPIO_Unified_IRQ(void)
{
  /* Get and clear all pending GPIO interrupts */
  uint32_t interruptMask = GPIO_IntGet();
  GPIO_IntClear(interruptMask);

  /* Act on interrupts */
  if (interruptMask & (1 << BSP_GPIO_PB0_PIN))
  {
    /* PB0: Reset test */
    demoNo = 0;
  }

  if (interruptMask & (1 << BSP_GPIO_PB1_PIN))
  {
    /* PB1: Next test */
    demoNo++;
  }
}

/**************************************************************************//**
 * @brief GPIO Interrupt handler (PB0)
 *        Reset to test 0
 *****************************************************************************/
void GPIO_EVEN_IRQHandler(void)
{
  GPIO_Unified_IRQ();
}


/**************************************************************************//**
 * @brief GPIO Interrupt handler (PB1)
 *        Next test
 *****************************************************************************/
void GPIO_ODD_IRQHandler(void)
{
  GPIO_Unified_IRQ();
}


/**************************************************************************//**
 * @brief   Set up PCNT to generate an interrupt every second.
 *
 *****************************************************************************/
void PcntInit(void)
{
  PCNT_Init_TypeDef pcntInit = PCNT_INIT_DEFAULT;

  /* Enable PCNT clock */
  CMU_ClockEnable(cmuClock_PCNT0, true);
  /* Set up the PCNT to count RTC_PULSE_FREQUENCY pulses -> one second */
  pcntInit.mode = pcntModeOvsSingle;
  pcntInit.top = RTC_PULSE_FREQUENCY / RTC_PULSE_FREQUENCY;
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

  curTime++;
}


/**************************************************************************//**
 * @brief   Calculate 4 poly points
 *
 *****************************************************************************/
static int32_t * gen4PolyPoints(uint32_t radius, int32_t xOff, int32_t yOff)
{
  static int32_t polyCoords[8];

  polyCoords[0] = CENTER_X - radius + xOff;
  polyCoords[1] = CENTER_Y + yOff;
  polyCoords[2] = CENTER_X + xOff;
  polyCoords[3] = CENTER_Y - radius + yOff;
  polyCoords[4] = CENTER_X + radius + xOff;
  polyCoords[5] = CENTER_Y + yOff;
  polyCoords[6] = CENTER_X + xOff;
  polyCoords[7] = CENTER_Y + radius + yOff;

  return (int32_t *)polyCoords;
}

/**************************************************************************//**
 * @brief  GLIB draw text demo function
 *
 *****************************************************************************/
static void GlibDemoDrawText(GLIB_Context_t *pContext)
{
  #define STR_LEN 48
  char str[ STR_LEN ];

/* Print welcome message using NORMAL 8x8 font. */
  GLIB_setFont(pContext, (GLIB_Font_t *)&GLIB_FontNormal8x8);
  snprintf( str, STR_LEN, "  EFM32 GLIB \nNormal 8x8 font" );
  GLIB_drawString(&glibContext,
                  str,
                  strlen(str),
                  CENTER_X - ((GLIB_FONT_WIDTH * strlen(str)) / 4),
                  5,
                  0);

  /* Use the NARROW 6x8 font */
  GLIB_setFont(&glibContext, (GLIB_Font_t *)&GLIB_FontNarrow6x8);
  snprintf( str, STR_LEN, "GLIB strings & fonts\n   Narrow 6x8 font  " );
  GLIB_drawString(&glibContext,
                  str,
                  strlen(str),
                  CENTER_X - ((GLIB_FONT_WIDTH * strlen(str)) / 4),
                  5 + (3 * GLIB_FONT_HEIGHT),
                  0);

  /* Add 3 extra char spacing pixels between each font. Use 6 pixels line spacing. */
  glibContext.font.charSpacing = 3;
  glibContext.font.lineSpacing = 6;
  snprintf( str, STR_LEN, "  GLIB demo \nNarrow (6+3)x8" );
  GLIB_drawString(&glibContext,
                  str,
                  strlen(str),
                  CENTER_X - ((strlen(str) * GLIB_FONT_WIDTH) / 4),
                  5 + (6 * GLIB_FONT_HEIGHT),
                  0);

  GLIB_setFont(&glibContext, (GLIB_Font_t *)&GLIB_FontNumber16x20);
  snprintf( str, STR_LEN, "12345\n16:20" );
  GLIB_drawString(&glibContext,
                  str,
                  strlen(str),
                  CENTER_X - ((strlen(str) * GLIB_FONT_WIDTH) / 4),
                  CENTER_Y + 14,
                  0);

  GLIB_setFont(pContext, (GLIB_Font_t *)&GLIB_FontNormal8x8);
  #undef STR_LEN
}


/**************************************************************************//**
 * @brief  GLIB demo function
 *
 *****************************************************************************/
static void GlibDemo(void)
{
  uint32_t currentDemoNo = 1;
  int32_t m = 0;
  int32_t n = 0;

  /* Setup display colours */
  glibContext.backgroundColor = White;
  glibContext.foregroundColor = Black;

  /* Some test elements that cannot be declared inside the switch */
  GLIB_Rectangle_t rect0 = {5, 5, CENTER_X, CENTER_Y};
  GLIB_Rectangle_t rect1 = {CENTER_X, CENTER_Y, MAX_X - 5, MAX_Y - 5};

  while(true) {
    if (currentDemoNo != demoNo) {
      currentDemoNo = demoNo;

      switch(demoNo) {

      case 0:
        GLIB_clear(&glibContext);
        GlibDemoDrawText(&glibContext);
        DMD_updateDisplay();
        break;

      case 1:
        GLIB_clear(&glibContext);
        GLIB_drawLine(&glibContext, MIN_X - 1, CENTER_Y, MAX_X + 1, CENTER_Y);
        GLIB_drawLine(&glibContext, CENTER_X, MIN_Y - 1, CENTER_X, MAX_Y + 1);
        DMD_updateDisplay();
        break;

      case 2:
        m = MIN_X - DISPLAY0_WIDTH / 4;
        n = MIN_X + DISPLAY0_WIDTH / 4;
      case 3:
      case 4:
      case 5:
      case 6:
        GLIB_clear(&glibContext);
        GLIB_drawLine(&glibContext, m, MIN_Y + 10, n, MAX_Y - 10);
        DMD_updateDisplay();
        m += DISPLAY0_WIDTH / 4;
        n += DISPLAY0_WIDTH / 4;
        break;

      case 7:
        m = MIN_Y - DISPLAY0_HEIGHT / 4;
        n = MIN_Y + DISPLAY0_HEIGHT / 4;
      case 8:
      case 9:
      case 10:
      case 11:
        GLIB_clear(&glibContext);
        GLIB_drawLine(&glibContext, MIN_X + 10, m, MAX_X - 10,  n);
        DMD_updateDisplay();
        m += DISPLAY0_WIDTH / 4;
        n += DISPLAY0_WIDTH / 4;
        break;

      case 12:
        m = DISPLAY0_HEIGHT / 8;
      case 13:
      case 14:
      case 15:
        GLIB_clear(&glibContext);
        GLIB_drawCircle(&glibContext, CENTER_X, CENTER_Y, m);
        m += DISPLAY0_HEIGHT / 4;
        DMD_updateDisplay();
        break;

      case 16:
        m = DISPLAY0_HEIGHT - (DISPLAY0_HEIGHT / 4);
      case 17:
      case 18:
      case 19:
        GLIB_clear(&glibContext);
        GLIB_drawCircleFilled(&glibContext, CENTER_X, CENTER_Y, m);
        m -= DISPLAY0_HEIGHT / 5;
        DMD_updateDisplay();
        break;

      case 20:
        GLIB_clear(&glibContext);
        GLIB_drawCircle(&glibContext, 0, CENTER_Y, DISPLAY0_HEIGHT / 4);
        DMD_updateDisplay();
        break;

      case 21:
        GLIB_clear(&glibContext);
        GLIB_drawCircle(&glibContext, CENTER_X, 0, DISPLAY0_HEIGHT / 4);
        DMD_updateDisplay();
        break;

      case 22:
        GLIB_clear(&glibContext);
        GLIB_drawCircleFilled(&glibContext, MAX_X, CENTER_Y, DISPLAY0_HEIGHT / 4);
        DMD_updateDisplay();
        break;

      case 23:
        GLIB_clear(&glibContext);
        GLIB_drawCircleFilled(&glibContext, CENTER_X, MAX_Y, DISPLAY0_HEIGHT / 4);
        DMD_updateDisplay();
        break;

      case 24:
        GLIB_clear(&glibContext);
        GLIB_drawCircle(&glibContext, CENTER_X - (DISPLAY0_WIDTH / 8), CENTER_Y, DISPLAY0_HEIGHT / 4);
        GLIB_drawCircle(&glibContext, CENTER_X + (DISPLAY0_WIDTH / 8), CENTER_Y, DISPLAY0_HEIGHT / 4);
        DMD_updateDisplay();
        break;

      case 25:
        GLIB_drawCircleFilled(&glibContext, CENTER_X, CENTER_Y - (DISPLAY0_HEIGHT / 8), DISPLAY0_HEIGHT / 4);
        GLIB_drawCircleFilled(&glibContext, CENTER_X, CENTER_Y + (DISPLAY0_HEIGHT / 8), DISPLAY0_HEIGHT / 4);
        DMD_updateDisplay();
        break;

      case 26:
        GLIB_clear(&glibContext);
        GLIB_drawPolygon(&glibContext, 4, gen4PolyPoints(20, 0 ,0));
        GLIB_drawPolygon(&glibContext, 4, gen4PolyPoints(40, 0 ,0));
        GLIB_drawPolygon(&glibContext, 4, gen4PolyPoints(60, 0 ,0));
        GLIB_drawPolygon(&glibContext, 4, gen4PolyPoints(80, 0 ,0));
        GLIB_drawPolygon(&glibContext, 4, gen4PolyPoints(100, 0 ,0));
        GLIB_drawPolygon(&glibContext, 4, gen4PolyPoints(120, 0 ,0));
        DMD_updateDisplay();
        break;

      case 27:
        GLIB_clear(&glibContext);
        GLIB_drawPolygon(&glibContext, 4, gen4PolyPoints(50, (DISPLAY0_WIDTH / 2) ,0));
        DMD_updateDisplay();
        break;

      case 28:
        GLIB_clear(&glibContext);
        GLIB_drawPolygon(&glibContext, 4, gen4PolyPoints(50, 0 ,0));
        DMD_updateDisplay();
        break;

      case 29:
        GLIB_clear(&glibContext);
        GLIB_drawPolygon(&glibContext, 4, gen4PolyPoints(50, -1 * (DISPLAY0_WIDTH / 2) ,0));
        DMD_updateDisplay();
        break;

      case 30:
        GLIB_clear(&glibContext);
        GLIB_drawRect(&glibContext, &rect0);
        GLIB_drawRectFilled(&glibContext, &rect1);
        DMD_updateDisplay();
        break;

      case 31:
        GLIB_clear(&glibContext);
        GLIB_drawPartialCircle(&glibContext, CENTER_X, CENTER_Y, 50, 0x9F);
        GLIB_drawCircle(&glibContext, CENTER_X, MAX_Y + 10, 60);
        GLIB_drawLine(&glibContext, MIN_X, MAX_Y, MAX_X, MIN_Y);
        GLIB_drawLine(&glibContext, MIN_X, MIN_Y, MAX_X, MAX_Y);
        DMD_updateDisplay();
        break;

      default:
        demoNo = INIT_DEMO_NO;
        break;
      }
    }
  }
}


/**************************************************************************//**
 * @brief  Main function of GLIB example.
 *
 *****************************************************************************/
int main(void)
{
  EMSTATUS status;

  /* Chip errata */
  CHIP_Init();

  /* Setup GPIO for pushbuttons. */
  GpioSetup();

  /* Initialize the display module. */
  status = DISPLAY_Init();
  if (DISPLAY_EMSTATUS_OK != status)
    while(1);

  /* Initialize the DMD module for the DISPLAY device driver. */
  status = DMD_init(0);
  if (DMD_OK != status)
    while(1);

  status = GLIB_contextInit(&glibContext);
  if (GLIB_OK != status)
    while(1);

  /* Set PCNT to generate interrupt every second */
  PcntInit();

  /* Enter infinite loop */
  while (1)
  {
    GlibDemo();
  }
}

