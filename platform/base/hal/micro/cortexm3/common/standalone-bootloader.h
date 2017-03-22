/** @file hal/micro/cortexm3/common/standalone-bootloader.h
 * See @ref cbh_alone for detailed documentation.
 *
 * <!-- Copyright 2009 by Ember Corporation. All rights reserved.       *80* -->   
 */

//[[ Author(s): David Iacobone, diacobone@ember.com
//              Lee Taylor, lee@ember.com
//]]

/** @addtogroup cbh_alone
 * @brief EM35x standalone bootloader public definitions.
 *
 * See standalone-bootloader.h for source code.
 *@{
 */

#ifndef __STANDALONE_BOOTLOADER_H__
#define __STANDALONE_BOOTLOADER_H__


///////////////////////////////////////////////////////////////////////////////
/** @name Required Custom Functions
 *@{
 */
/** @brief This function must be implemented, providing a bootloader menu.
 */
void bootloaderMenu(void);
/**@} */


///////////////////////////////////////////////////////////////////////////////
/** @name Available Bootloader Library Functions
 * Functions implemented by the bootloader library that may be used by
 * custom functions.
 *@{
 */

/** @brief Puts the bootloader into a mode where it will receive an image.
 *   commState indicates whether the image is received via serial (COMM_SERIAL)
 *   or over the air (COMM_RADIO)
 */
BL_Status receiveImage(uint8_t commState);

/** @brief A hook to the bootloader library for it to check for extra menu
 * options.  Only used for ember internal debug builds, not normally needed.
 * 
 * @return true if the option was handled, false if not.
 */
bool checkDebugMenuOption(uint8_t ch);


/** @brief Initialize OTA Bootloader state
 * @note OTA support hooks are subject to change!
 *
 * @return ::BL_Status of the success of the function.
 */
BL_Status initOtaState(void);

/** @brief Check to see if the bootloader has detected an OTA upload start.
 * @note OTA support hooks are subject to change!
 *
 * @return ::BL_Status of the success of the function.
 */
BL_Status checkOtaStart(void);

/** @brief Puts the bootloader into a mode where it will receive an image
 *  over the air.  The function ::checkOtaStart() should have been called
 * first and it should have returned with a status of ::BL_SUCCESS before
 * calling this function.
 * @note OTA support hooks are subject to change!
 *
 * @return ::BL_Status of the success of the function.
 */
BL_Status receiveOtaImage(void);

/** @brief Uses the information in the PHY_CONFIG token to determine if a power
 * amplifier is present in the node design.
 *
 * @note This function must not be called before emBootloaderRadioBoot().
 *
 * @return true if a power amplifier is present, false otherwise.
 */
bool paIsPresent(void);

/** @brief Validate application integrity by running AES-MMO hash and comparing
 * to AAT.
 *
 * @return false if fails integrity check, true if pass
 */
bool halCheckIntegrity(void);

/**@} */


#endif //__STANDALONE_BOOTLOADER_H__

/**@} end of group*/
