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
#include "heart_rate_monitor.h"
#include "em_usb.h"
#include "rtcdriver.h"

static bool lowBattery = false;
static char *scrolltext  = "Place finger on sensor ";

static void GRAPHICS_CreateString(char *string, int32_t value);


/**************************************************************************//**
 * @brief Initializes the graphics stack.
 * @note This function will /hang/ if errors occur (usually
 *       caused by faulty displays.
 *****************************************************************************/
void GRAPHICS_Init(bool boost)
{
  /* Enable LCD without voltage boost */
  SegmentLCD_Init(boost);
}


/**************************************************************************//**
 * @brief Draws battery status.
 *****************************************************************************/
static void GRAPHICS_DrawBackground ()
{
  SegmentLCD_AllOff();

  if ( lowBattery )
     SegmentLCD_Battery(1);
  else
     SegmentLCD_Battery(4);
}

/**************************************************************************//**
 * @brief Prompts user to select HRM device.
 *****************************************************************************/
void GRAPHICS_ShowLEDSelectMenu()
{

  static int scrollIndex = 0;

  char buffer[8];
  char *scrolltext  = "Push PB0 for GRI. Push PB1 for GGG. ";
  GRAPHICS_DrawBackground();

  if (scrollIndex == 35)
	  scrollIndex = 0;

  buffer[7] = 0x00;



  memcpy(buffer, scrolltext + scrollIndex++, 7);
  SegmentLCD_Write(buffer);


}

/**************************************************************************//**
 * @brief This function outputs HRM status
 *****************************************************************************/
bool GRAPHICS_ShowHRMStatus(bool acquiring, int32_t pulse, bool scrollInfo)
{

  static int scrollIndex = 0;
  static int emIndicator = 4;
  char buffer[8];

  GRAPHICS_DrawBackground();

  if (scrollInfo == false)
	  scrollIndex = 0;

  buffer[7] = 0x00;
  if (emIndicator == 3)
    emIndicator = 4;
  else
    emIndicator--;
  if ( (pulse != 0) || acquiring)
      SegmentLCD_EnergyMode(emIndicator,1);

  if (scrollInfo == false)
  {
	  if ((acquiring) && (pulse == 0))
	  {
	  	  SegmentLCD_Write("WAIT ");
	  }
	  else
	    {
	        SegmentLCD_Write("PULSE ");
	        if (pulse != 0)
	          SegmentLCD_Number(pulse);
	    }
	  return true;
  }
  else
  {

      memcpy(buffer, scrolltext + scrollIndex, 7);
	  SegmentLCD_Write(buffer);
	  if (scrollIndex++ > 16)
		  return true;
	  return false;


  }
}

/**************************************************************************//**
 * @brief This function outputs spo2 status
 *****************************************************************************/
bool GRAPHICS_ShowSpO2Status(bool acquiring, int32_t spo2, bool scrollInfo)
{

  static int scrollIndex = 0;
  static int emIndicator = 4;
  char buffer[8];

  GRAPHICS_DrawBackground();

  if (scrollInfo == false)
	  scrollIndex = 0;

  buffer[7] = 0x00;
  if (emIndicator == 3)
    emIndicator = 4;
  else
    emIndicator--;
  if ( (spo2 != 0) || acquiring)
      SegmentLCD_EnergyMode(emIndicator,1);

  if (scrollInfo == false)
  {
	  if ((acquiring) && (spo2 < 50))
	  {
	  	  SegmentLCD_Write("WAIT ");
	  }
	  else
	    {
	        SegmentLCD_Write("SPO2 ");
	        if (spo2 > 50)
	          SegmentLCD_Number(spo2);
	    }
	  return true;
  }
  else
  {

      memcpy(buffer, scrolltext + scrollIndex, 7);
	  SegmentLCD_Write(buffer);
	  if (scrollIndex++ > 16)
		  return true;
	  return false;


  }

}

/**************************************************************************//**
 * @brief This function sets battery indicator value
 *****************************************************************************/
void GRAPHICS_SetBatteryStatus(bool lowBat)
{
  lowBattery = lowBat;
}


/**************************************************************************//**
 * @brief Helper function for drawing the temperature in Fahrenheit
 * @param tempData
 *        Temperature data (given in Fahrenheit) multiplied by 1000
 *****************************************************************************/
void GRAPHICS_DrawTemperatureF(int32_t tempData)
{
  char string[9];
  GRAPHICS_DrawBackground();
  tempData = ((tempData * 9) / 5) + 32000;
  /* Text representation */
  GRAPHICS_CreateString(string, tempData);
  strcat (string, "F ");
  string[8] = 0;
  SegmentLCD_Write(string);
}


/**************************************************************************//**
 * @brief Helper function for drawing the temperature in Celsius.
 * @param tempData
 *        Temperature data (given in Celsius) multiplied by 1000
 *****************************************************************************/
void GRAPHICS_DrawTemperatureC(int32_t tempData)
{
  char string[9];
  GRAPHICS_DrawBackground();
  /* Text in display */
  GRAPHICS_CreateString(string, tempData);
  strcat (string, "C ");
  string[8] = 0;
  SegmentLCD_Write(string);
}

/**************************************************************************//**
 * @brief Displays error status.
 *****************************************************************************/
void GRAPHICS_DrawError ()
{
  GRAPHICS_DrawBackground();
  /* Text in display */
  SegmentLCD_Write("Error ");
}

/**************************************************************************//**
 * @brief Shows configuration on initialization.
 *****************************************************************************/
void GRAPHICS_DrawInit (HeartRateMonitor_Config_t hrm_config, char *hrmVersion, char*biodemoVersion, bool usbStatus)
{

  GRAPHICS_DrawBackground();
  if (usbStatus)
  {
	  SegmentLCD_Write("Usb ON");
  }
  else
  {
	  SegmentLCD_Write("Usb OFF");

  }
  RTCDRV_Delay(1000);
  if (hrm_config == SI1143_PS)
  {
	  SegmentLCD_Write("1143_PS");
  }
  else if (hrm_config == SI1147_PS)
  {
	  SegmentLCD_Write("1147_PS");
  }
  RTCDRV_Delay(1000);
  /* Text in display */
  SegmentLCD_Write("HRM ver");
  RTCDRV_Delay(1000);
  SegmentLCD_Write(hrmVersion);
  RTCDRV_Delay(1000);
  SegmentLCD_Write("demo ");
  RTCDRV_Delay(1000);
  SegmentLCD_Write(biodemoVersion);
  RTCDRV_Delay(1000);
}

/**************************************************************************//**
 * @brief Helper function for drawing the humidity part of the UI.
 * @param rhData
 *        Relative humidity (in percent), multiplied by 1000.
 *****************************************************************************/
void GRAPHICS_DrawHumidity(uint32_t rhData)
{
  char string2[10];
  char string[9];
  GRAPHICS_DrawBackground();
  string[0] = 'R';
  string[1] = 'H';
  string[2] = 0;
  GRAPHICS_CreateString(string2, rhData);
  strcat (string, string2);
  string[8] = 0;
  SegmentLCD_Write(string);
}

/**************************************************************************//**
 * @brief Helper function for drawing the UV part of the GUI.
 * @param uvData
 *        UV Index
 *****************************************************************************/
void GRAPHICS_DrawUV(uint32_t uvData)
{
  char string[9];
  GRAPHICS_DrawBackground();
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
  string[8] = 0;
  SegmentLCD_Write(string);

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
    SegmentLCD_Write("UV: Mod");
    break;
  case  6:
  case  7:
    SegmentLCD_Write("UV: Hi");
    break;
  case  8:
  case  9:
  case  10:
    SegmentLCD_Write("UV: V Hi");
    break;
  default:
    SegmentLCD_Write("UV: Ext");
  }
#endif

}



/**************************************************************************//**
 * @brief Helper function for printing numbers. Consumes less space than
 *        snprintf. Limited to three digits and one decimal.
 * @param string
 *        The string to print to
 * @param value
 *        The value to print
 *****************************************************************************/
static void GRAPHICS_CreateString(char *string, int32_t value)
{ int i=0;
  if (value < 0)
  {
    value = -value;
    string[i++] = '-';
  }
  else
  {
    string[i++] = ' ';
  }
  string[i] = '0' + (value % 1000000) / 100000;
  if (string[i] != '0')		//if 2 digits then move everything up
  { i++;                  //this is done to accommodate RH display
  }                         //RH display will cut off decimal without this

  string[i] = '0' + (value % 100000) / 10000;
  if ((string[i] == '0') && (i == 1))  //do not clear if middle of 3 digits
	  string[i] = ' ';
  i++;
  string[i++] = '0' + (value % 10000) / 1000;
  string[i++] = '.';
  string[i++] = '0' + (value % 1000) / 100;
  string[i++] = 0;

}
