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

#include <string.h>
#include <stddef.h>
#include "em_device.h"
#include "em_emu.h"
#include "em_cmu.h"
#include "em_lcd.h"
#include "em_gpio.h"
#include "segmentlcd.h"
#include "graphics.h"
#include "rtcdriver.h"
#include "string.h"


static void GRAPHICS_DrawTemperatureC(char* string, int32_t tempData);
static void GRAPHICS_DrawTemperatureF(char* string, int32_t tempData);
static void GRAPHICS_DrawHumidity(char* string, uint32_t rhData);
static void GRAPHICS_DrawUV(char* string, uint32_t uvData);
static void GRAPHICS_CreateString(char *string, int32_t value);


/**************************************************************************//**
 * @brief Initializes the graphics stack.
 * @note This function will /hang/ if errors occur (usually
 *       caused by faulty displays.
 *****************************************************************************/
void GRAPHICS_Init(bool boost)
{
  SegmentLCD_Init(boost);

  /* Use Antenna symbol to signify enabling of vboost */
  SegmentLCD_Symbol(LCD_SYMBOL_ANT, boost);
}

/**************************************************************************//**
 * @brief LCD scrolls a text over the display, sort of "polled printf"
 *****************************************************************************/
void ScrollText(char *scrolltext)
{
  int  i, len;
  char buffer[8];

  buffer[7] = 0x00;
  len       = strlen(scrolltext);
  if (len < 7) return;
  for (i = 0; i < (len - 7); i++)
  {
    memcpy(buffer, scrolltext + i, 7);
    SegmentLCD_Write(buffer);
    RTCDRV_Delay(300);
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
  SegmentLCD_AllOff();
  if (removeObject)
  {
    ScrollText ("Remove object in front of sensor board ");
  }
  else
  {
    SegmentLCD_Write("Hvr Hnd ");
  }
  SegmentLCD_Battery(4);
  if (lowBat)
  {
    SegmentLCD_Battery(1);
  }
  if ((!si114x_status) || (!si7013_status))
  {
    SegmentLCD_Write("Snsr Err ");
  }
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
  char string[21];
  char buffer[9];
  SegmentLCD_AllOff();

  if ( lowBat )
  {
    SegmentLCD_Battery(1);
  }
  else
  {
    SegmentLCD_Battery(4);
  }
  if (yoffset == 0)
  {
    GRAPHICS_DrawTemperatureF(&(string[0]), tempData);
  }
  else
  {
    GRAPHICS_DrawTemperatureC(&(string[0]), tempData);
  }

  GRAPHICS_DrawHumidity(&(string[7]), rhData);
  GRAPHICS_DrawUV(&(string[14]), uvData);

  if ((xoffset >= 0) && (xoffset <= 14))
  {
    memcpy(buffer, string + xoffset, 7);
  }
  else if (xoffset < 0)
  {
    memcpy(buffer, string+(21+xoffset), -xoffset);
    memcpy(&(buffer[-xoffset]), string, 7+xoffset);
  }
  else
  {
    memcpy(buffer, string + xoffset, 21-xoffset);
    memcpy(&(buffer[21-xoffset]), string, 7-(21-xoffset));
  }
  buffer[8] = 0;
  SegmentLCD_Write(buffer);
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
static void GRAPHICS_DrawTemperatureF(char *string, int32_t tempData)
{
  tempData = ((tempData * 9) / 5) + 32000;
  /* Text representation */
  GRAPHICS_CreateString(string, tempData);
  strcat (string, "F ");
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
static void GRAPHICS_DrawTemperatureC(char *string, int32_t tempData)
{
  /* Text in display */
  GRAPHICS_CreateString(string, tempData);
  /* Text representation */
  GRAPHICS_CreateString(string, tempData);
  strcat (string, "C ");
}


/**************************************************************************//**
 * @brief Helper function for drawing the humidity part of the UI.
 * @param xoffset
 *        This parameter selects which part of the UI to show.
 * @param rhData
 *        Relative humidity (in percent), multiplied by 1000.
 *****************************************************************************/
static void GRAPHICS_DrawHumidity(char *string, uint32_t rhData)
{
  char string2[10];
  string[0] = 'R';
  string[1] = 'H';
  string[2] = 0;
  GRAPHICS_CreateString(string2, rhData);
  strcat (string, string2);
}


/**************************************************************************//**
 * @brief Helper function for drawing the UV part of the GUI.
 * @param xoffset
 *        This parameter selects which part of the UI to show.
 * @param uvData
 *        UV Index
 *****************************************************************************/
static void GRAPHICS_DrawUV(char *string, uint32_t uvData)
{
  string[0] = 'U';
  string[1] = 'V';
  string[2] = ':';
  string[3] = ' ';

  /* Draw UV data */
  if (uvData > 10)
  {
    string[4] = '1';
    string[5] = '1';
    string[6] = '+';
  }
  else
  {
    string[4] = ' ';
    if (uvData == 10)
    {
      string[4] = '1';
      string[5] = '0';
      string[6] = ' ';
    }
    else
    {
      string[4] = ' ';
      string[5] = '0' + uvData;
      string[6] = ' ';
    }
  }


#if 0
  /* Print 5 point scale string*/
  switch (uvData)
  {
  case  0:
  case  1:
  case  2:
    SegmentLCD_Write("UV: Low");
    break;
  case  3:
  case  4:
  case  5:
    SegmentLCD_Write("UV: Moderate");
    break;
  case  6:
  case  7:
    SegmentLCD_Write("UV: High");
    break;
  case  8:
  case  9:
  case  10:
    SegmentLCD_Write("UV: Very High");
    break;
  default:
    SegmentLCD_Write("UV: Extreme");
  }
#endif
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

  if (value >= 100000)
  {
    string[0] = '1';
  }
  if (string[1] == '0')
  {
    string[1] = ' ';
  }
}
