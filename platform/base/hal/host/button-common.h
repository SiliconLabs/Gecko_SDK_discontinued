/** @file hal/host/button-common.h
 * See @ref button and micro specific modules for documentation.
 *
 * <!-- Copyright 2010 by Ember Corporation. All rights reserved.        *80*-->
 */

/** @addtogroup button
 * @brief  Sample generic API funtions for using push-buttons.
 *
 * @note The micro specific definitions, @ref stm32f103ret_button, is
 * chosen by the build include path pointing at the appropriate directoy.
 *
 * See button-common.h for source code.
 *@{
 */

#ifndef __BUTTON_COMMON_H__
#define __BUTTON_COMMON_H__


/** @name Button State Definitions
 * A set of numerical definitions for use with the button APIs indicating the
 * state of a button.
 *@{
 */
 
/** @brief Button state is pressed.
 */
#define BUTTON_PRESSED  1

/** @brief Button state is released.
 */
#define BUTTON_RELEASED 0

/**@} END Button State Definitions */

/** @brief Initializes the buttons.  Must be called before the buttons can
 * be used.
 */
void halInternalInitButton(void);

/** @brief Returns the current state (pressed or released) of a button.
 *
 * @note This function is correlated with ::halButtonIsr() and so returns 
 * the shadow state rather than reading the actual state of the pin.
 *   
 * @param button  The button being queried, either BUTTON0 or BUTTON1 as 
 * defined in button-specific.h.
 *
 * @return ::BUTTON_PRESSED if the button is pressed or ::BUTTON_RELEASED 
 * if the button is not pressed.
 */
uint8_t halButtonState(uint8_t button);

/** @brief Returns the current state (pressed or released) of the
 * pin associated with a button.  
 *
 * This reads the actual state of the pin and can be used on startup to
 * determine the initial position of the buttons.
 *   
 * @param button  The button being queried, either BUTTON0 or BUTTON1 as 
 * defined in button-specific.h.
 *
 * @return  ::BUTTON_PRESSED if the button is pressed or ::BUTTON_RELEASED 
 * if the button is not pressed.
 */
uint8_t halButtonPinState(uint8_t button);

/** @brief A callback called in interrupt context whenever a button
 * changes its state.
 * 
 * @appusage Must be implemented by the application.  This function should
 * contain the functionality to be executed in response to changes of state
 * in each of the buttons, or callbacks to the appropriate functionality.
 *  
 * @param button  The button which has changed state, either BUTTON0 or BUTTON1 
 * as defined in button-specific.h.
 *  
 * @param state   The new state of the button referenced by the button parameter,
 * either ::BUTTON_PRESSED if the button has been pressed or ::BUTTON_RELEASED if 
 * the button has been released.
 */
void halButtonIsr(uint8_t button, uint8_t state);


//Pull in the micro specific button definitions.  The specific header is chosen
//by the build include path pointing at the appropriate directory.
#include "button-specific.h"


#endif //__BUTTON_COMMON_H__

/**@} //END addtogroup 
 */

