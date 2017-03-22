/***************************************************************************//**
 * @file hal/micro/cortexm3/common/app-bootloader.h
 * @brief Application bootloader
 * @version 3.20.2
 * See @ref cbh_app for detailed documentation.
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
/** @addtogroup cbh_app
 * @brief Application bootloader and generic EEPROM Interface
 *
 * See app-bootloader.h for source code.
 *@{
 */
 
#ifndef __APP_BOOTLOADER_H__
#define __APP_BOOTLOADER_H__

///////////////////////////////////////////////////////////////////////////////
/** @name Required Custom Functions
 *@{
 */
void bootloaderInit();
/** @brief Drives the app bootloader.  If the ::runRecovery parameter is ::true,
 * the recovery mode should be activated, otherwise it should attempt to
 * install an image.  This function should not return.  It should always exit
 * by resetting the the bootloader.
 *
 * @param runRecovery If ::true, recover mode is activated.  Otherwise, normal
 * image installation is activated.
 */
void bootloaderAction(bool runRecovery);
/**@} */

///////////////////////////////////////////////////////////////////////////////
/** @name Available Bootloader Library Functions
 * Functions implemented by the bootloader library that may be used by
 * custom functions.
 *@{
 */

/** @brief Activates ::recoveryMode to receive a new image over xmodem.
 * @return ::BL_SUCCESS if an image was successfully received.
 */
BL_Status recoveryMode(void);

/** @brief Processes an image in the external eeprom.
 * @param install If ::false, it will simply validate the image without
 * touching main flash.  If ::true, the image will be programmed to main flash.
 * @return ::BL_SUCCESS if an image was successfully installed/validated
 */
BL_Status processImage(bool install);
/**@} */


#endif //__APP_BOOTLOADER_H__

/**@} end of group*/

