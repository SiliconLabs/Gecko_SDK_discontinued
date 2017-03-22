/**************************************************************************//**
 * @file
 * @brief helper functions for managing capsense GPIO inputs
 * @version 5.1.2
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2015 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include "cpt007b_config.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include <stdint.h>
#include <stdbool.h>


/******************************************************************************
 * @brief GPIO data structure
 * For CPT007B there are 7 capsense outputs, each pin
 * corresponds to a capsenseCurrent bit showing whether a
 * cap-sense button pressed or not, and capsensePrevious
 * bit showing the status of previous cycle.
 *
 * If capsenseCurrent==1 && capsensePrevious==0, detects a "press"
 * If capsenseCurrent==0 && capsensePrevious==1, detects a "release"
 *
 * capsenseCurrent bit-map byte:
 * | N/A | cC6 | cC5 | cC4 | cC3 | cC2 | cC1 | cC0 |
 *
 * capsensePrevious bit-map byte:
 * | N/A | cP6 | cP5 | cP4 | cP3 | cP2 | cP1 | cP0 |
 *
 * *cC,cP are abbreviation for capsenseCurrent and capsensePrevious
 *
 *****************************************************************************/

// GPIO data structure declaration
static uint8_t capsenseCurrent;
static uint8_t capsensePrevious;

/******************************************************************************
 * @brief Return a bitmask containing the current state for all capsense
 * buttons.
 *****************************************************************************/
uint8_t getCapsenseCurrent(void)
{
  return capsenseCurrent;
}

/******************************************************************************
 * @brief Return a bitmask containing the previous state for all capsense
 * buttons.
 *****************************************************************************/
uint8_t getCapsensePrevious(void)
{
  return capsensePrevious;
}

/******************************************************************************
 * @brief GPIO data structure initialization
 *****************************************************************************/
void initGPIO(void)
{
  capsenseCurrent = 0;
  capsensePrevious = 0;

  /* Enable GPIO in CMU */
  CMU_ClockEnable(cmuClock_GPIO, true);

  /* Configure cap-sense input */
  GPIO_PinModeSet(CS0_0_PORT, CS0_0_PIN, gpioModeInput, 0);  // CS00
  GPIO_PinModeSet(CS0_1_PORT, CS0_1_PIN, gpioModeInput, 0);  // CS01
  GPIO_PinModeSet(CS0_2_PORT, CS0_2_PIN, gpioModeInput, 0);  // CS02
  GPIO_PinModeSet(CS0_3_PORT, CS0_3_PIN, gpioModeInput, 0);  // CS03
  GPIO_PinModeSet(CS0_4_PORT, CS0_4_PIN, gpioModeInput, 0);  // CS04
  GPIO_PinModeSet(CS0_5_PORT, CS0_5_PIN, gpioModeInput, 0);  // CS05
  GPIO_PinModeSet(CS0_6_PORT, CS0_6_PIN, gpioModeInput, 0);  // CS06
}

/******************************************************************************
 * @brief update GPIO data structure current level of seven GPIO pins
 *****************************************************************************/
void updateGPIO(void)
{
  // get previous states of Cap-sense button array
  capsensePrevious = capsenseCurrent;

  // update current button states
  capsenseCurrent =  ((GPIO->P[CS0_0_PORT].DIN & (1 << CS0_0_PIN)) ? 0 : CS0_0_PRESENT);
  capsenseCurrent |= ((GPIO->P[CS0_1_PORT].DIN & (1 << CS0_1_PIN)) ? 0 : CS0_1_PRESENT);
  capsenseCurrent |= ((GPIO->P[CS0_2_PORT].DIN & (1 << CS0_2_PIN)) ? 0 : CS0_2_PRESENT);
  capsenseCurrent |= ((GPIO->P[CS0_3_PORT].DIN & (1 << CS0_3_PIN)) ? 0 : CS0_3_PRESENT);
  capsenseCurrent |= ((GPIO->P[CS0_4_PORT].DIN & (1 << CS0_4_PIN)) ? 0 : CS0_4_PRESENT);
  capsenseCurrent |= ((GPIO->P[CS0_5_PORT].DIN & (1 << CS0_5_PIN)) ? 0 : CS0_5_PRESENT);
  capsenseCurrent |= ((GPIO->P[CS0_6_PORT].DIN & (1 << CS0_6_PIN)) ? 0 : CS0_6_PRESENT);
}
