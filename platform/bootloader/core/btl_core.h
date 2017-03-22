/***************************************************************************//**
 * @file btl_core.h
 * @brief Core functionality for Silicon Labs bootloader.
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

#ifndef BTL_CORE_H
#define BTL_CORE_H

#include <stdint.h>

/***************************************************************************//**
 * @addtogroup Core Bootloader Core
 * @brief Core bootloader functionality.
 * @details Core functionality for the Silicon Labs Bootloader.
 * @{
 ******************************************************************************/

/**
 * Initialize bootloader.
 *
 * @return Error code. @ref BOOTLOADER_OK on success, else error code in
 *         @ref BOOTLOADER_ERROR_INIT_BASE range.
 */
int32_t btl_init(void);

/**
 * Deinitialize bootloader.
 *
 * @return Error code. @ref BOOTLOADER_OK on success, else error code in
 *         @ref BOOTLOADER_ERROR_INIT_BASE range.
 */
int32_t btl_deinit(void);

/** @} addtogroup core */

#endif // BTL_CORE_H
