/**************************************************************************//**
 * @file
 * @brief Internal EMU temperature sensor demo for SLSTK3402A_EFM32PG
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

#include "em_device.h"
#include "em_cmu.h"
#include "em_chip.h"
#include "em_emu.h"
#include "em_assert.h"
#include "bsp.h"
#include "display.h"
#include "textdisplay.h"
#include "retargettextdisplay.h"
#include "displayconfigapp.h"
#include "tempdrv.h"
#include <stdio.h>

/* Temprature configuration */
#define TEMP_L0_VALUE        0
#define TEMP_L1_VALUE       30
#define TEMP_L2_VALUE       85
#define TEMP_THRESHOLD       2

// Since temperature is stored in 8 bit number therefore, 
// the upper and lower bound of the temperature is set
#define TEMP_UPPER_BOUND   127 
#define TEMP_LOWER_BOUND  -128

typedef enum Temperature_State 
{
  TEMP_LOW     = 0,
  TEMP_MIDLOW  = 1,
  TEMP_MIDHIGH = 2,
  TEMP_HIGH    = 3
} TemperatureState_t;

TemperatureState_t tempState;

static void tempCallback(int8_t temp, TEMPDRV_LimitType_t limit);

/**************************************************************************//**
 * @brief print current temperature range according to state
 *****************************************************************************/
static void printTemperature(void)
{
  switch (tempState)
  {
    case TEMP_LOW:
      printf("\f");
      printf("\n\n\n\n\n\n   ***************  \n");
      printf("\n    Below %d degC", TEMP_L0_VALUE);
      printf("\n\n   ***************  \n");
      break;
    case TEMP_MIDLOW:
      printf("\f");
      printf("\n\n\n\n\n\n   ***************  \n");
      printf("\n Between %d & %d degC", TEMP_L0_VALUE, TEMP_L1_VALUE);
      printf("\n\n   ***************  \n");
      break;
    case TEMP_MIDHIGH:
      printf("\f");
      printf("\n\n\n\n\n\n   ***************  \n");
      printf("\n    Above %d degC", TEMP_L1_VALUE);
      printf("\n\n   ***************  \n");
      break;
    case TEMP_HIGH:
      printf("\f");
      printf("\n\n\n\n\n\n   ***************  \n");
      printf("\n    Above %d degC", TEMP_L2_VALUE);
      printf("\n\n   ***************  \n");
      break;
    default:
      printf("\f");
      printf("\n\n\n\n\n\n   ***************  \n");
      printf("\n    Unknown Temperature State");
      printf("\n\n   ***************  \n");
      break;
  }
}

/**************************************************************************//**
 * @brief setup current temperature state
 *
 * @param[in] currentTemp value set by tempdrv. It contains the value
 *            of current temperature.
 *
 * @param[in] init If true initialize state else update it
 * 
 * @param[in] limit Limit value set by tempdrv. If init is true this
 *            parameter has no use.
 *
 * @details This function updates or initializes the current temperature 
 *          state based on parameter init. After state finalization,
 *          the corresponding callbacks using the state high and low
 *          temperatures are registered. This is done to trigger next
 *          event when the temperature goes outside the specified range.
 *          In the end the current temperature range is printed on the LCD.
 *****************************************************************************/
static void setupCurrentTempState(int8_t currentTemp, TEMPDRV_LimitType_t limit,
                                  bool init)
{
  // Define low temperature range based on temperature state
  int8_t stateLowTemp[4] = {TEMP_LOWER_BOUND,
                              (TEMP_L0_VALUE 
                               - TEMP_THRESHOLD),
                              (TEMP_L1_VALUE 
                               - TEMP_THRESHOLD),
                              (TEMP_L2_VALUE 
                               - TEMP_THRESHOLD)};

  // Define high temperature range based on temperature state
  int8_t stateHighTemp[4] = {(TEMP_L0_VALUE 
                                + TEMP_THRESHOLD),
                               (TEMP_L1_VALUE 
                                + TEMP_THRESHOLD),
                               (TEMP_L2_VALUE 
                                + TEMP_THRESHOLD),
                               TEMP_UPPER_BOUND};
  
  if (init == true)  // Intialize device temperature state
  {
    if (currentTemp < stateHighTemp[TEMP_LOW])
    {
      tempState = TEMP_LOW;
    }
    else if (currentTemp > stateLowTemp[TEMP_HIGH])
    {
      tempState = TEMP_HIGH;
    }
    else if (currentTemp >= stateHighTemp[TEMP_MIDLOW])
    {
      tempState = TEMP_MIDHIGH;
    }
    else if (currentTemp >= stateHighTemp[TEMP_LOW])
    {
      tempState = TEMP_MIDLOW;
    }
  }
  else  // Update device temperature state
  {
    if (limit == TEMPDRV_LIMIT_HIGH)
    {
      tempState++;
    }
    else if (limit == TEMPDRV_LIMIT_LOW)
    {
      tempState--;
    }
  }
  
  // Register the callback function according to the current 
  // device temperature state
  TEMPDRV_RegisterCallback(stateLowTemp[tempState],
                                     TEMPDRV_LIMIT_LOW,
                                     tempCallback);
    
  TEMPDRV_RegisterCallback(stateHighTemp[tempState],
                                     TEMPDRV_LIMIT_HIGH,
                                     tempCallback);
  
  // Print current temperature range based on its state
  printTemperature();
}

/**************************************************************************//**
 * @brief update temperature state when received callback 
 *****************************************************************************/
static void tempCallback(int8_t temp, TEMPDRV_LimitType_t limit)
{
  // Update temperature state
  setupCurrentTempState(temp, limit, false);
}

/**************************************************************************//**
 * @brief setup LCD display
 *****************************************************************************/
static void setupDisplay(void)
{
  // Initialize the display module
  DISPLAY_Init();
  
  // Retarget stdio to a text display
  if (RETARGET_TextDisplayInit() != TEXTDISPLAY_EMSTATUS_OK)
  {
    while (1)
    {}
  }
}

/**************************************************************************//**
 * @brief main function
 *****************************************************************************/
int main(void){

  int8_t currentemp = 0;
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_STK_DEFAULT;

  // Chip errata 
  CHIP_Init();

  // Init DCDC regulator with kit specific parameters 
  EMU_DCDCInit(&dcdcInit);
  
  // Setup system display
  setupDisplay();
  
  // Initialize temperature driver
  TEMPDRV_Init();
  
  // Get current temperature
  currentemp = TEMPDRV_GetTemp();
 
  // Initialize temperature state-machine
  setupCurrentTempState(currentemp, TEMPDRV_LIMIT_LOW, true);
  
  while (1)
  {
    // re-enter energy mode3 whenever there is a wakeup 
    // due to temprature sensor
    EMU_EnterEM3(true);
  }
}
