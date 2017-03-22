/**************************************************************************//**
 * @file analog_clock.c
 * @brief Draws an analog clock using GLIB
 * @version 5.1.2
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
#include "em_cmu.h"
#include "em_types.h"
#include "dmd.h"
#include "glib.h"

/* Analog clock background. */
#define BACKGROUND_WIDTH     (gc.pDisplayGeometry->xSize)
#define BACKGROUND_HEIGHT    (gc.pDisplayGeometry->ySize)

/* Screen center */
#define CENTER_X        (BACKGROUND_WIDTH / 2)
#define CENTER_Y        (BACKGROUND_HEIGHT / 2)

/* Radial position of the second point */
#define SEC_POS         42
/* Length of the minute and hour hands */
#define MIN_START       0
#define MIN_END         40
#define HOUR_START      0
#define HOUR_END        25

/* Distance screen center to center of number label */
#define LABEL_LENGTH    55

/* Define Pi if not already defined. */
#ifndef PI
#define PI    3.14159265358979f
#endif

extern GLIB_Context_t gc;

/* Coordinates for all second-dots in the one quadrant */
static int32_t secondPositions[16];

/**************************************************************************//**
 * @brief   Draw a dot to represent a second on the analog clock
 *****************************************************************************/
static void analogDrawSecondDot(int second)
{
  int32_t quadrant;
  int32_t quadrantPos;
  int32_t x, y;

  quadrant    = (second) / 15;
  quadrantPos = second % 15;

  if (quadrant == 0) {
    x = CENTER_X + secondPositions[quadrantPos];
    y = CENTER_Y - secondPositions[15 - quadrantPos];
  }
  else if (quadrant == 1) {
    x = CENTER_X + secondPositions[15 - quadrantPos];
    y = CENTER_Y + secondPositions[quadrantPos];
  }
  else if (quadrant == 2) {
    x = CENTER_X - secondPositions[quadrantPos];
    y = CENTER_Y + secondPositions[15 - quadrantPos];
  }
  else {
    x = CENTER_X - secondPositions[15 - quadrantPos];
    y = CENTER_Y - secondPositions[quadrantPos];
  }

  GLIB_drawPixel(&gc, x, y);
}

/**************************************************************************//**
 * @brief   Draw the hour hand of the analog clock
 *****************************************************************************/
static void analogDrawHourHand(int32_t hour, int32_t minute)
{
  int32_t    position = hour * 5 + minute / 12;
  float a;

  /* Get angle */
  a = (float) position / 30.0f * PI;

  /* Hour pointer is just a line. Calculate start and end points. */
  GLIB_drawLine(&gc,
                CENTER_X + (int) (HOUR_START * sinf(a)), /* start x */
                CENTER_Y - (int) (HOUR_START * cosf(a)), /* start y */
                CENTER_X + (int) (HOUR_END * sinf(a)),   /* end x   */
                CENTER_Y - (int) (HOUR_END * cosf(a)));  /* end y   */
}

/**************************************************************************//**
 * @brief   Draw the minute hand of the analog clock
 *****************************************************************************/
static void analogDrawMinuteHand(int32_t minute)
{
  float a;

  /* Get angle */
  a = (float) minute / 30.0f * PI;

  /* Minute pointer is just a line. Calculate start and end points. */
  GLIB_drawLine(&gc,
                CENTER_X + (int) (MIN_START * sinf(a)), /* start x */
                CENTER_Y - (int) (MIN_START * cosf(a)), /* start y */
                CENTER_X + (int) (MIN_END * sinf(a)),   /* end x   */
                CENTER_Y - (int) (MIN_END * cosf(a)));  /* end y   */
}

/**************************************************************************//**
 * @brief   Draw the analog clock number plate
 *****************************************************************************/
static void analogDrawNumberPlate(void)
{
  int32_t i, x, y;
  float a;
  char hourLabel[3];

  /* Clear the screen */
  GLIB_clear(&gc);

  /* Draw a circle at center */
  GLIB_drawCircleFilled(&gc, CENTER_X, CENTER_Y, 2);

  /* Loop through all the 12 hour labels */
  for ( i = 1; i <= 12; i++ ) {

    /* Get angle */
    a = (float) i / 6.0f * PI;

    /* Calculate position for the label */
    x = CENTER_X - 4 + (int) (LABEL_LENGTH * sinf(a));
    y = CENTER_Y - 4 + (int) (LABEL_LENGTH * -cosf(a));

    /* Compensate for two-digit numbers */
    if (i > 10)
      x -= 4;

    /* Print number */
    sprintf(hourLabel, "%ld", i);
    GLIB_drawString(&gc, hourLabel, 2, x, y, 0);
  }
}

/**************************************************************************//**
 * @brief   Initialize the frame buffer for drawing an analog clock.
 *****************************************************************************/
void analogClockInitGraphics(void)
{
  int32_t i;
  float angle;

  /* Pre-calculate positions for each second-dot
     Saves about 2 uA compared to calculating it every second */
  for ( i = 0; i < 16; i++ ) {
    angle              = (float) (i) / 30.0f * PI;
    secondPositions[i] = (int32_t) (SEC_POS * sinf(angle) + 0.5f);
  }
}

/**************************************************************************//**
 * @brief   Draw the pointers for the analog clock face at the given time
 *****************************************************************************/
void analogClockUpdate(struct tm *t, bool redraw)
{
  static int32_t previousMin = -1;
  int32_t i;

  if (redraw)
  {
    GLIB_setFont(&gc, (GLIB_Font_t *)&GLIB_FontNarrow6x8);
    gc.backgroundColor = Black;
    gc.foregroundColor = White;
    analogDrawNumberPlate();
  }

  /* Only update the whole clock face every minute */
  if ((t->tm_min != previousMin) || redraw)
  {
    previousMin = t->tm_min;

    analogDrawNumberPlate();
    analogDrawHourHand(t->tm_hour % 12, t->tm_min);
    analogDrawMinuteHand(t->tm_min);

    /* In case we changed clock face or adjusted the time,
     * redraw all second-points */
    for (i = 1; i <= t->tm_sec; i++)
      analogDrawSecondDot(i);
  }
  else
  {
    analogDrawSecondDot(t->tm_sec);
  }

  /* Update display */
  DMD_updateDisplay();
}


