/** @file hal/host/led-common.h
 * See @ref led and micro specific modules for documentation.
 *  
 * <!-- Copyright 2010 by Ember Corporation. All rights reserved.        *80*-->
 */

/** @addtogroup led
 * @brief Sample generic API funtions for controlling LEDs.
 *
 * When specifying an LED to use, always use the BOARDLEDx definitions that
 * are defined in the HalBoardLedPins enum in the micro specific led header.
 *
 * @note The micro specific definitions, @ref stm32f103ret_led, is
 * chosen by the build include path pointing at the appropriate directoy.
 *
 * See led-common.h for source code.
 *@{
 */

#ifndef __LED_COMMON_H__
#define __LED_COMMON_H__


/** @brief Configures GPIOs pertaining to the control of LEDs.
 */
void halInternalInitLed(void);


/** @brief Ensures that the definitions for the LEDs
 *  are always used as parameters to the LED functions.
 *
 * @note Even though many compilers will use 16 bits for an enum instead of 8, 
 * we choose to use an enum here.  The possible compiler inefficiency does not 
 * affect stack-based parameters and local variables, which is the
 * general case for led paramters.
 */
typedef enum HalBoardLedPins HalBoardLed;


/** @brief Atomically wraps an XOR or similar operation for a single GPIO
 *  pin attached to an LED.
 * 
 *  @param led  Identifier for the LED to be toggled.
 */
void halToggleLed(HalBoardLed led);


/** @brief Turns on (sets) a GPIO pin connected to an LED so that the LED 
 *  turns on.
 * 
 *  @param led  Identifier for the LED to turn on.
 */
void halSetLed(HalBoardLed led);


/** @brief Turns off (clears) a GPIO pin connected to an LED, which turns 
 *  off the LED.
 *  
 *  @param led  Identifier for the LED to turn off.
 */
void halClearLed(HalBoardLed led);


//Pull in the micro specific LED definitions.  The specific header is chosen
//by the build include path pointing at the appropriate directory.
#include "led-specific.h"


#endif //__LED_COMMON_H__

/**@} //END addtogroup 
 */

