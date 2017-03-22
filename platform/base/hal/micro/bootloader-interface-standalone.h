/** @file hal/micro/bootloader-interface-standalone.h
 * See @ref alone_bootload for documentation.
 *
 * <!-- Copyright 2006-2009 by Ember Corporation. All rights reserved.  *80* -->   
 */

/** @addtogroup alone_bootload
 * @brief Definition of the standalone bootloader interface.
 *
 * Some functions in this file return an ::EmberStatus value. See 
 * error-def.h for definitions of all ::EmberStatus return values.
 *
 * See bootloader-interface-standalone.h for source code.
 *@{
 */

#ifndef __BOOTLOADER_STANDALONE_H__
#define __BOOTLOADER_STANDALONE_H__

/** @brief Detects if the standalone bootloader is installed, and if so
 *    returns the installed version.
 *
 *  A returned version of 0x1234 would indicate version 1.2 build 34
 *
 * @return ::BOOTLOADER_INVALID_VERSION if the standalone bootloader is not
 *    present, or the version of the installed standalone bootloader.
 */
uint16_t halGetStandaloneBootloaderVersion(void);

/** @brief Define a numerical value for NO BOOTLOADER mode.  In other words,
 *    the bootloader should not be run.
 */
#define NO_BOOTLOADER_MODE                 0xFF

/** @brief Define a numerical value for the normal bootloader mode.
 */
#define STANDALONE_BOOTLOADER_NORMAL_MODE   1

/** @brief Define a numerical value for the recovery bootloader mode.
 */
#define STANDALONE_BOOTLOADER_RECOVERY_MODE 0

/** @brief Quits the current application and launches the standalone
 * bootloader (if installed). The function returns an error if the standalone
 * bootloader is not present.
 *   
 * @param mode  Controls the mode in which the standalone bootloader will run.
 * See the bootloader Application Note for full details.  Options are:
 *   - ::STANDALONE_BOOTLOADER_NORMAL_MODE
 *     Will listen for an over-the-air
 *     image transfer on the current channel with current power settings.
 *   - ::STANDALONE_BOOTLOADER_RECOVERY_MODE
 *     Will listen for an over-the-air
 *     image transfer on the default channel with default power settings.
 *   .
 *   Both modes also allow an image transfer to begin via serial xmodem.
 * 
 * @return  An ::EmberStatus error if the standalone bootloader is not present,
 * or ::EMBER_SUCCESS.
 */
EmberStatus halLaunchStandaloneBootloader(uint8_t mode);


#endif //__BOOTLOADER_STANDALONE_H__

/**@} // END addtogroup
 */



