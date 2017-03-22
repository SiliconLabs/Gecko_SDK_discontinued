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
#include "background.h"
#include <string.h>
#include <stdio.h>

static GLIB_Context_t glibContext;          /* Global glib context */

static void GRAPHICS_DrawTemperatureC(int xoffset, int yoffset, int32_t tempData);
static void GRAPHICS_DrawTemperatureF(int xoffset, int yoffset, int32_t tempData);
static void GRAPHICS_DrawHumidity(int xoffset, uint32_t rhData);
static void GRAPHICS_DrawUV(int xoffset, uint32_t uvData);
static void findCoord (int rh, int *x, int *height);
static void GRAPHICS_CreateString(char *string, int32_t value);

/* Humidity sine lookup table for coordinates (1/4 sine wave, 51 points) */
static const int8_t sinLUT [] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 15, 16, 17, 18, 19, 20, 21
, 22, 23, 23, 24, 25, 25, 26, 26, 27, 28, 28, 29, 29, 29, 30, 30, 30, 31, 31, 31
, 31, 32, 32, 32, 32, 32, 32, 32};

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
}

/**************************************************************************//**
 * @brief Draws the background image
 * @param xoffset
 *        The background image is wider than the display. This parameter
 *        selects which part to show. The offset is given in multiples of 8.
 *        If you increase xoffset by 1, the background will be shifted by 8
 *        pixels
 *****************************************************************************/
static void GRAPHICS_DrawBackground(int xoffset)
{
  EMSTATUS status;
  int      y;
  int      img;
  int      size;

  if (xoffset < 0)
  {
    size = -xoffset*8;
  }
  else
  {
    size = 128;
  }

  for (y = 0; y < 128; y++)
  {
    /* Write bitmap to display */
    img = xoffset + y * (BACKGROUND_XSIZE / 8);
    if (xoffset < 0)
      img = (48+xoffset) + y * (BACKGROUND_XSIZE / 8);

    status = DMD_writeData(0, y,
                           background + img,
                           size);

    img = y * (BACKGROUND_XSIZE / 8);

    if (size != 128)
    {
      status |= DMD_writeData(size, y,
                           background + img,
                           128 - size);
    }
    if (status != DMD_OK)
    {
      while (1)
        ;
    }
  }
}

/**************************************************************************//**
 * @brief This function draws the background image
 * @param xoffset
 *        The background image is wider than the display. This parameter
 *        selects which part to show. The offset is given in multiples of 8.
 *        If you increase xoffset by 1, the bacground will be shifted by 8
 *        pixels
 *****************************************************************************/
void GRAPHICS_ShowStatus(bool si114x_status, bool si7013_status, bool removeObject, bool lowBat)
{

  GLIB_clear(&glibContext);

  if (removeObject)
  {
    GLIB_drawString(&glibContext, "Please remove ", 15, 5, 40, 0);
    GLIB_drawString(&glibContext, "object in front", 15, 5, 50, 0);
    GLIB_drawString(&glibContext, "of sensor board", 15, 5, 60, 0);
  }
  else
  {
    GLIB_drawString(&glibContext, "To start demo ", 14, 5, 20, 0);
    GLIB_drawString(&glibContext, "press BTN1 or", 14, 5, 30, 0);
    GLIB_drawString(&glibContext, "hover hand over", 15, 5, 40, 0);
    GLIB_drawString(&glibContext, "sensor board.", 13, 5, 50, 0);
  }

  if (lowBat)
  {
    GLIB_drawString(&glibContext, "Low Battery!", 16, 5, 115, 0);
  }
  else if ((!si114x_status) || (!si7013_status))
  {
    GLIB_drawString(&glibContext, "SensorEXP fail!", 16, 5, 115, 0);
  }
  else
  {
    GLIB_drawString(&glibContext, DEMO_VERSION, 16, 5, 115, 0);
  }
  DMD_updateDisplay();
}

/**************************************************************************//**
 * @brief This function draws the actual UI
 * @param xoffset
 *        This parameter selects which part of the UI to show.
 * @param tempData
 *        Temperature data (given in Celsius) multiplied by 1000
 * @param rhData
 *        Relative humidity (in percent), multiplied by 1000.
 * @param uvData
 *        UV Index
 * @param degF
 *        Set to 0 to display temperature in Celsius, otherwise Fahrenheit.
 *****************************************************************************/
void GRAPHICS_Draw(int xoffset, int32_t tempData, uint32_t rhData,
                   uint32_t uvData, int yoffset, bool lowBat)
{
  /* Draw background */
  GRAPHICS_DrawBackground(xoffset);
  if ( lowBat )
  {
    GLIB_drawString(&glibContext, "Low Battery!", 16, 28, 3, 0);
  }
  /* Draw temperature */
  /* offset < 0 indicates the display needs to wrap around */
  /* yoffset 0 = degC, yoffset 16 = degF */
  GRAPHICS_DrawTemperatureF(xoffset, yoffset, tempData);
  GRAPHICS_DrawTemperatureC(xoffset, yoffset, tempData);

  GRAPHICS_DrawHumidity(xoffset, rhData);
  GRAPHICS_DrawUV(xoffset, uvData);

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
static void GRAPHICS_DrawTemperatureF(int xoffset, int yoffset, int32_t tempData)
{
  char string[10];
  int  dx;
  int  dy;
  int  height;

  if (yoffset > 0)
  {
    dy = (yoffset * 8) - 128;
  }
  else
  {
    dy = (yoffset * 8) + 128;
  }

  dx = xoffset * 8;
  tempData = ((tempData * 9) / 5) + 32000;

  /* Text representation */
  GRAPHICS_CreateString(string, tempData);
  GLIB_drawString(&glibContext, string, strlen(string), 38 - dx, 40 - dy, 0);

  /* Line in thermometer */
  tempData /= 1000;
  if (tempData < 16)
  {
    height = 91;
  }
  else if (tempData > 104)
  {
    height = 14;
  }
  else
  {
    height = 91 - (((tempData - 16) * 223) >> 8); /* *0.87 -> 223/256 */
  }

  if (yoffset != 16)
  {
    height = 91;
  }

  GLIB_drawLineV(&glibContext, 96 - dx, 91, height);
  GLIB_drawLineV(&glibContext, 97 - dx, 91, height);
  GLIB_drawLineV(&glibContext, 98 - dx, 91, height);

  /* Marks on the thermometer */
  GLIB_drawString(&glibContext, "F", 1, 112 - dx, 42 - dy, 0);
  GLIB_drawString(&glibContext, "20", 2, 72 - dx, 85 - dy, 0);
  GLIB_drawString(&glibContext, "100", 3, 64 - dx, 15 - dy, 0);
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
static void GRAPHICS_DrawTemperatureC(int xoffset, int yoffset, int32_t tempData)
{
  char string[10];
  int  dx;
  int  dy;
  int  height;

  dy = yoffset * 8;
  dx = xoffset * 8;

  /* Text in display */
  GRAPHICS_CreateString(string, tempData);
  GLIB_drawString(&glibContext, string, strlen(string), 38 - dx, 40 - dy, 0);
  tempData /= 1000;

  if (tempData <= 0)
  {
    height = 90;
  }
  else if (tempData > 41)
  {
    height = 14;
  }
  else
  {
    height = 91 - ((tempData * 238) >> 7); /* *1.86 -> 238/128 */
  }


  if (yoffset != 0)
  {
    height = 91;
  }

  /* Line in thermometer */
  GLIB_drawLineV(&glibContext, 96 - dx, 91, height);
  GLIB_drawLineV(&glibContext, 97 - dx, 91, height);
  GLIB_drawLineV(&glibContext, 98 - dx, 91, height);

  /* Marks on the thermometer */
  GLIB_drawString(&glibContext, "0", 1, 80 - dx, 85 - dy, 0);
  GLIB_drawString(&glibContext, "40", 2, 72 - dx, 15 - dy, 0);
  GLIB_drawString(&glibContext, "C", 1, 112 - dx, 42 - dy, 0);
}

/**************************************************************************//**
 * @brief Helper function for drawing the humidity part of the UI.
 * @param xoffset
 *        This parameter selects which part of the UI to show.
 * @param rhData
 *        Relative humidity (in percent), multiplied by 1000.
 *****************************************************************************/
static void GRAPHICS_DrawHumidity(int xoffset, uint32_t rhData)
{
  char string[10];
  int  height, x;
  int  dx;

  dx = xoffset * 8;

  GRAPHICS_CreateString(string, rhData);
  GLIB_drawString(&glibContext, string, strlen(string), 181 - dx, 40, 0);
  rhData /= 1000;
  findCoord (rhData,&x,&height);
  if (xoffset == 16)
  {
    GLIB_drawLine(&glibContext, 200 - dx, 105, 200 + x - dx, 105 - height);
  }
}

/**************************************************************************//**
 * @brief Helper function for drawing the UV part of the GUI.
 * @param xoffset
 *        This parameter selects which part of the UI to show.
 * @param uvData
 *        UV Index
 *****************************************************************************/
static void GRAPHICS_DrawUV(int xoffset, uint32_t uvData)
{
  char string[3];
  int  dx;
  if (xoffset > 0)
    dx = xoffset * 8;
  else
    dx = (xoffset+48) * 8;
  /* Draw UV data */
  if (uvData > 10)
  {
      string[0] = '1';
      string[1] = '1';
      string[2] = '+';
  }
  else
  {
    string[2] = ' ';
    if (uvData == 10)
    {
      string[0] = '1';
      string[1] = '0';
    }
    else
    {
      string[0] = ' ';
      string[1] = '0' + uvData;
    }
  }
  GLIB_drawString(&glibContext, string, 3, 316 - dx, 55, 0);

  /* Print 5 point scale string*/
  switch (uvData)
  {
  case  0:
  case  1:
  case  2:
    GLIB_drawString(&glibContext, "   Low", 6, 291 - dx, 96, 0);
    break;
  case  3:
  case  4:
  case  5:
    GLIB_drawString(&glibContext, "Moderate", 8, 291 - dx, 96, 0);
    break;
  case  6:
  case  7:
    GLIB_drawString(&glibContext, "  High", 6, 291 - dx, 96, 0);
    break;
  case  8:
  case  9:
  case  10:
    GLIB_drawString(&glibContext, "Very High", 9, 291 - dx, 96, 0);
    break;
  default:
    GLIB_drawString(&glibContext, "Extreme", 7, 291 - dx, 96, 0);
  }
}

/**************************************************************************//**
 * @brief Helper function for finding the positions to draw the line in the
 *        humidity gauge. (The other position to draw from is fixed.)
 * @param[in] rh
 *        Relative humidity
 * @param[out] x
 *        X coordintate to draw to.
 * @param[out] height
 *        The y coordinate to draw to.
 *****************************************************************************/
static void findCoord (int rh, int *x, int *height)
{
  int index;
  if (rh <= 50)
  {
    index = rh;
  }
  else
  {
    index = (100 - rh);
  }
  *height = sinLUT[index];

  if (rh < 50)
  {
    index = 50 - rh;
    *x = (-sinLUT[index]) ;
  }
  else
  {
    index = rh-50;
    *x = (sinLUT[index]) ;
  }
}

/**************************************************************************//**
 * @brief Helper function for printing numbers. Consumes less space than
 *        snprintf. Limited to two digits and one decimal.
 * @param string
 *        The string to print to
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
