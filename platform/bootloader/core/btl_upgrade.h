/***************************************************************************//**
 * @file btl_upgrade.h
 * @brief Bootloader upgrade functionality for the Silicon Labs bootloader
 * @author Silicon Labs
 * @version 1.0.0
 *******************************************************************************
 * # License
 * <b>Copyright 2016 Silicon Laboratories, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/
#ifndef BTL_UPGRADE_H
#define BTL_UPGRADE_H

#include <stdbool.h>

/***************************************************************************//**
 * @addtogroup Core
 * @{
 * @addtogroup Upgrade
 * @brief Methods to verify and upgrade the main bootloader
 * @details
 * @{
 ******************************************************************************/

/**
 * Check if there is a bootloader upgrade available.
 *
 * @return True if there is an upgrade image in the upgrade location
 */
bool btl_checkForUpgrade(void);

/**
 * Apply a bootloader upgrade.
 *
 * @return True if the bootloader upgrade was applied successfully.
 */
bool btl_applyUpgrade(void);

/** @} addtogroup upgrade */
/** @} addtogroup core */

#endif // BTL_UPGRADE_H
