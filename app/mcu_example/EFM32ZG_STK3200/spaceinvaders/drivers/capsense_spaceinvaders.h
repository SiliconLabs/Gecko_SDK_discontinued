/**************************************************************************//**
 * @file
 * @brief Capacitive sense driver
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

#ifndef __CAPSENSE_H_
#define __CAPSENSE_H_

#include <stdint.h>
#include <stdbool.h>

/***************************************************************************//**
 * @addtogroup Drivers
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup CapSense
 * @{
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

uint8_t  CAPSENSE_getSegmentChannel(uint8_t capSegment);
uint8_t  CAPSENSE_getButton0Channel(void);
uint8_t  CAPSENSE_getButton1Channel(void);
uint32_t CAPSENSE_getVal(uint8_t channel);
uint32_t CAPSENSE_getNormalizedVal(uint8_t channel);
bool CAPSENSE_getPressed(uint8_t channel);
int32_t CAPSENSE_getSliderPosition(void);
void CAPSENSE_Sense(void);
void CAPSENSE_Init(void);
void CAPSENSE_timerEventHandler(void);

#ifdef __cplusplus
}
#endif

/** @} (end group CapSense) */
/** @} (end group Drivers) */

#endif /* __CAPSENSE_H_ */
