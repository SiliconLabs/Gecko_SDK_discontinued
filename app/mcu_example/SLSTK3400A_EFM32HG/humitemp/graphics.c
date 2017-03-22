/**************************************************************************//**
 * @brief Draws the graphics on the display
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

#include "graphics.h"
#include "em_types.h"
#include "glib.h"
#include "dmd.h"
#include "display.h"
#include "textdisplay.h"
#include "retargettextdisplay.h"
#include <string.h>
#include <stdio.h>

static GLIB_Context_t glibContext;          /* Global glib context */

static void GRAPHICS_DrawThermometer(int32_t xoffset, int32_t yoffset, uint32_t max, int32_t level, char scale);
static void GRAPHICS_DrawTemperatureC(int32_t xoffset, int32_t yoffset, int32_t tempData);
static void GRAPHICS_DrawTemperatureF(int32_t xoffset, int32_t yoffset, int32_t tempData);
static void GRAPHICS_DrawHumidity(int32_t xoffset, int32_t yoffset, uint32_t rhData);
static void GRAPHICS_CreateString(char *string, int32_t value);

/**************************************************************************//**
 * @brief Initializes the graphics stack.
 * @note This function will /hang/ if errors occur (usually
 *       caused by faulty displays.
 *****************************************************************************/
void GRAPHICS_Init(void)
{
  EMSTATUS status;

  /* Initialize the display module. */
  status = DISPLAY_Init();
  if (DISPLAY_EMSTATUS_OK != status)
    while (1)
      ;

  /* Initialize the DMD module for the DISPLAY device driver. */
  status = DMD_init(0);
  if (DMD_OK != status)
    while (1)
      ;

  status = GLIB_contextInit(&glibContext);
  if (GLIB_OK != status)
    while (1)
      ;

  glibContext.backgroundColor = Black;
  glibContext.foregroundColor = White;

  /* Use Narrow font */
  GLIB_setFont(&glibContext, (GLIB_Font_t *)&GLIB_FontNarrow6x8);
}


/**************************************************************************//**
 * @brief This function draws the background image
 * @param xoffset
 *        The background image is wider than the display. This parameter
 *        selects which part to show. The offset is given in multiples of 8.
 *        If you increase xoffset by 1, the bacground will be shifted by 8
 *        pixels
 *****************************************************************************/
void GRAPHICS_ShowStatus(bool si7013_status, bool lowBat)
{
  GLIB_clear(&glibContext);

  if (lowBat)
  {
    GLIB_drawString(&glibContext, "Low battery!", 16, 5, 115, 0);
  }
  else if (!si7013_status)
  {
    GLIB_drawString(&glibContext, "Failed to detect\nsi7021 sensor.", 32, 5, 5, 0);
  }
  else
  {
    GLIB_drawString(&glibContext, "si7021 sensor ready.\n"DEMO_VERSION, 32, 5, 5, 0);
  }
  DMD_updateDisplay();
}


/**************************************************************************//**
 * @brief This function draws the UI
 * @param tempData
 *        Temperature data (given in Celsius) multiplied by 1000
 * @param rhData
 *        Relative humidity (in percent), multiplied by 1000.
  * @param degF
 *        Set to 0 to display temperature in Celsius, otherwise Fahrenheit.
 *****************************************************************************/
void GRAPHICS_Draw(int32_t tempData, uint32_t rhData, bool lowBat)
{
  GLIB_clear(&glibContext);

  if (lowBat)
  {
    GLIB_drawString(&glibContext, "LOW BATTERY!", 12, 5, 120, 0);
  }
  else
  {
    /* Draw temperature and RH */
    GRAPHICS_DrawTemperatureC(6, 3, tempData);
    GRAPHICS_DrawTemperatureF(64 - 17, 3, tempData);
    GRAPHICS_DrawHumidity(127 - 40, 3, rhData);
  }
  DMD_updateDisplay();
}


/**************************************************************************//**
 * @brief Helper function for drawing the temperature in Fahrenheit
 * @param xoffset
 *        This parameter selects which part of the UI to show.
 * @param yoffset
 *        This parameter selects which part of the UI to show.
 * @param tempData
 *        Temperature data (given in Celsius) multiplied by 1000
 *****************************************************************************/
static void GRAPHICS_DrawTemperatureF(int32_t xoffset, int32_t yoffset, int32_t tempData)
{
  char string[10];

  tempData = ((tempData * 9) / 5) + 32000;

  GRAPHICS_CreateString(string, tempData);
  GLIB_drawString(&glibContext, string, strlen(string), xoffset, yoffset, 0);

  GRAPHICS_DrawThermometer(xoffset + 15, yoffset + 17, 95, tempData / 1000, 'F');
}


/**************************************************************************//**
 * @brief Helper function for drawing the temperature in Celsius.
 * @param xoffset
 *        This parameter selects which part of the UI to show.
 * @param yoffset
 *        This parameter selects which part of the UI to show.
 * @param tempData
 *        Temperature data (given in Celsius) multiplied by 1000
 *****************************************************************************/
static void GRAPHICS_DrawTemperatureC(int32_t xoffset, int32_t yoffset, int32_t tempData)
{
  char string[10];

  GRAPHICS_CreateString(string, tempData);
  GLIB_drawString(&glibContext, string, strlen(string), xoffset, yoffset, 0);

  GRAPHICS_DrawThermometer(xoffset + 15, yoffset + 17, 35, tempData / 1000, 'C');
}


/**************************************************************************//**
 * @brief Helper function for drawing a thermometer.
 * @param xoffset
 *        Top left pixel X offset
 * @param yoffset
 *        Top left pixel Y offset
 *****************************************************************************/
static void GRAPHICS_DrawThermometer(int32_t xoffset,
                                     int32_t yoffset,
                                     uint32_t max,
                                     int32_t level,
                                     char scale)
{
  GLIB_Rectangle_t thermoScale;
  GLIB_Rectangle_t mercuryLevel;
  const uint32_t minLevelY = yoffset + 76;
  const uint32_t maxLevelY = yoffset + 3;
  uint32_t curLevelY;
  bool levelNeg;
  char string[10];

  /* Draw outer frame */
  thermoScale.xMin = xoffset - 4;
  thermoScale.xMax = xoffset + 4;
  thermoScale.yMin = yoffset;
  thermoScale.yMax = yoffset + 90;
  GLIB_drawCircleFilled(&glibContext, xoffset, yoffset + 90, 12);
  GLIB_drawRectFilled(&glibContext, &thermoScale);

  glibContext.backgroundColor = White;
  glibContext.foregroundColor = Black;

  /* Draw the "mercury" */
  levelNeg = (level < 0);
  /* Abs value and saturate at max */
  level = levelNeg ? level * -1 : level;
  level = level > (int32_t)max ? (int32_t)max : level;
  curLevelY = yoffset + (((minLevelY - maxLevelY) * (max - level)) / max);

  /* Moving part */
  mercuryLevel.xMin = xoffset - 2;
  mercuryLevel.xMax = xoffset + 2;
  mercuryLevel.yMin = curLevelY;
  mercuryLevel.yMax = minLevelY;
  GLIB_drawRectFilled(&glibContext, &mercuryLevel);

  /* Non-moving part */
  mercuryLevel.xMin = xoffset - 2;
  mercuryLevel.xMax = xoffset + 2;
  mercuryLevel.yMin = minLevelY - 1;
  mercuryLevel.yMax = minLevelY + 5;
  GLIB_drawRectFilled(&glibContext, &mercuryLevel);

  /* Glass bulp */
  GLIB_drawCircleFilled(&glibContext, xoffset, yoffset + 90, 9);


  glibContext.backgroundColor = Black;
  glibContext.foregroundColor = White;

  /* Draw min/max lines and numbers */
  GLIB_drawLineH(&glibContext, xoffset - 6, minLevelY, xoffset + 6);
  GLIB_drawLineH(&glibContext, xoffset - 6, maxLevelY, xoffset + 6);

  GLIB_drawString(&glibContext, "0", 1, xoffset + 8, minLevelY - 4, 0);
  snprintf(string, 4, "%ld", max);
  GLIB_drawString(&glibContext, string, 4, xoffset + 8, maxLevelY - 4, 0);

  if (levelNeg)
  {
    GLIB_drawString(&glibContext, "-", 1, xoffset - 2, yoffset + 87, 0);
  }
  else
  {
    GLIB_drawString(&glibContext, (char *)&scale, 1, xoffset - 2, yoffset + 87, 0);
  }
}


/**************************************************************************//**
 * @brief Helper function for drawing the humidity part of the UI.
 * @param xoffset
 *        This parameter selects which part of the UI to show.
 * @param rhData
 *        Relative humidity (in percent), multiplied by 1000.
 *****************************************************************************/
static void GRAPHICS_DrawHumidity(int32_t xoffset, int32_t yoffset, uint32_t rhData)
{
  char string[10];

  GRAPHICS_CreateString(string, rhData);
  GLIB_drawString(&glibContext, string, strlen(string), xoffset, yoffset, 0);

  GRAPHICS_DrawThermometer(xoffset + 15, yoffset + 17, 100, rhData / 1000, '%');
}


/**************************************************************************//**
 * @brief Helper function for printing numbers. Consumes less space than
 *        snprintf. Limited to two digits and one decimal.
 * @param string
 *        The string to print32_t to
 * @param value
 *        The value to print
 *****************************************************************************/
static void GRAPHICS_CreateString(char *string, int32_t value)
{
  if (value < 0)
  {
    value = -value;
    string[0] = '-';
  }
  else
  {
    string[0] = ' ';
  }
  string[5] = 0;
  string[4] = '0' + (value % 1000) / 100;
  string[3] = '.';
  string[2] = '0' + (value % 10000) / 1000;
  string[1] = '0' + (value % 100000) / 10000;

  if (string[1] == '0')
  {
    string[1] = ' ';
  }
}
