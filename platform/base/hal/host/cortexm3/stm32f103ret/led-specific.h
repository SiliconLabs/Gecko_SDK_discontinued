/** @file hal/host/cortexm3/stm32f103ret/led-specific.h
 * See @ref led and @ref stm32f103ret_led for documentation.
 *  
 * <!-- Copyright 2010 by Ember Corporation. All rights reserved.        *80*-->
 */

/** @addtogroup stm32f103ret_led
 * @brief  Sample micro specific API funtions and defines for controlling LEDs.
 *
 * See @ref led for common documentation.
 *
 * The definitions in the micro specific header provide the necessary
 * pieces to link the common functionality to a specific micro.
 *
 * See led-specific.h for source code.
 *@{
 */

#ifndef __LED_SPECIFIC_H__
#define __LED_SPECIFIC_H__


/**
 * @brief Assign each LED to a convenient name that is a simple identifier.
 * BOARD_ACTIVITY_LED and BOARD_HEARTBEAT_LED provide a further layer of
 * abstraction ontop of the LEDs for verbose coding.
 */
enum HalBoardLedPins {
  BOARDLED0 = 0, //Just a simple identifier for switch statements
  BOARDLED1 = 1, //Just a simple identifier for switch statements
  BOARD_ACTIVITY_LED  = BOARDLED0,
  BOARD_HEARTBEAT_LED = BOARDLED1
};


/**
 * @brief The actual pin that BOARDLED0 is connected to.
 */
#define BOARDLED0_PIN   GPIO_Pin_8

/**
 * @brief The actual port that BOARDLED0 is connected to.
 */
#define BOARDLED0_PORT  GPIOB


/**
 * @brief The actual pin that BOARDLE1 is connected to.
 */
#define BOARDLED1_PIN   GPIO_Pin_9

/**
 * @brief The actual port that BOARDLED1 is connected to.
 */
#define BOARDLED1_PORT  GPIOB


#endif //__LED_SPECIFIC_H__

/**@} //END addtogroup 
 */

