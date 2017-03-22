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

#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>

/******************************************************************************
 * @brief Return a bitmask containing the current state for all capsense
 * buttons.
 *****************************************************************************/
uint8_t getCapsenseCurrent(void);

/******************************************************************************
 * @brief Return a bitmask containing the previous state for all capsense
 * buttons.
 *****************************************************************************/
uint8_t getCapsensePrevious(void);

/******************************************************************************
 * @brief GPIO data structure initialization
 *****************************************************************************/
void initGPIO(void);

/******************************************************************************
 * @brief update GPIO data structure from current values of seven GPIO pins
 *****************************************************************************/
void updateGPIO(void);

#endif /* GPIO_H */
