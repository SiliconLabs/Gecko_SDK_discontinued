/***************************************************************************//**
 * @file bootloader-gpio.h
 * @brief Bootloader GPIO definitions.
 * See @ref cbhc_gpio for detailed documentation.
 * @version 3.20.2
 *******************************************************************************
 * @section License
 * <b>(C) Copyright 2016 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Silicon Labs has no
 * obligation to support this Software. Silicon Labs is providing the
 * Software "AS IS", with no express or implied warranties of any kind,
 * including, but not limited to, any implied warranties of merchantability
 * or fitness for any particular purpose or warranties against infringement
 * of any proprietary rights of a third party.
 *
 * Silicon Labs will not be liable for any consequential, incidental, or
 * special damages, or any other relief, or for any claim by any third party,
 * arising from your use of this Software.
 *
 ******************************************************************************/
/** @addtogroup cbhc_gpio
 * @brief Bootloader GPIO definitions.
 *
 * See bootloader-gpio.h for source code.
 *@{
 */
 
#ifndef __BOOTLOADER_GPIO_H__
#define __BOOTLOADER_GPIO_H__

/** @name State Indicator Macros
 * The bootloader indicates which state it is in by calling these
// macros. Map them to the ::halBootloadStateIndicator function
// (in bootloder-gpio.c) if you want to display that bootloader state.
// Used to blink the LED's or otherwise signal bootloader activity.
 *@{
 */
//

/** @brief Finished init sequence, ready for bootload.
 */
#define BL_STATE_UP() do { bootloadStateIndicator(BL_ST_UP); } while(0)

/** @brief Called right before bootloader resets to application. Use to
 * cleanup and reset GPIO's to leave node in known state for app start,
 * if necessary.
 */
#define BL_STATE_DOWN() do { bootloadStateIndicator(BL_ST_DOWN); } while(0)

/** @brief Standalone bootloader polling serial/radio interface.
 */
#define BL_STATE_POLLING_LOOP() do { bootloadStateIndicator(BL_ST_POLLING_LOOP); } while(0)

/** @brief Processing download image.
 */
#define BL_STATE_DOWNLOAD_LOOP() do { bootloadStateIndicator(BL_ST_DOWNLOAD_LOOP); } while(0)

/** @brief Download process was a success.
 */
#define BL_STATE_DOWNLOAD_SUCCESS() do { bootloadStateIndicator(BL_ST_DOWNLOAD_SUCCESS); } while(0)

/** @brief Download process failed.
 */
#define BL_STATE_DOWNLOAD_FAILURE() do { bootloadStateIndicator(BL_ST_DOWNLOAD_FAILURE); } while(0)
/**@} */

/** @brief Defines various bootloader states.  Use in LED code to signal
 * bootload activity.
 */
enum blState_e {
  /** bootloader up */
  BL_ST_UP,
  /** bootloader going down */
  BL_ST_DOWN,
  /** polling interfaces */
  BL_ST_POLLING_LOOP,
  /** downloading */
  BL_ST_DOWNLOAD_LOOP,
  /** download failure */
  BL_ST_DOWNLOAD_FAILURE,
  /** download success */
  BL_ST_DOWNLOAD_SUCCESS
};

/** @brief Initialize GPIO.
 */
void bootloadGpioInit(void);

/** @brief Helper function used for displaying bootloader state (for example:
 * with LEDs).
 */
void bootloadStateIndicator(enum blState_e state);

/** @brief Force activation of bootloader.
 */
bool bootloadForceActivation( void );

#endif // __BOOTLOADER_GPIO_H__

/**@} end of group*/

