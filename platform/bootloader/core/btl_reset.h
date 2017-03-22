/***************************************************************************//**
 * @file btl_reset.h
 * @brief Reset cause signalling for the Silicon Labs bootloader
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
#ifndef BTL_RESET_H
#define BTL_RESET_H

#include "core/btl_util.h"

MISRAC_DISABLE
#include "em_common.h"
MISRAC_ENABLE

/***************************************************************************//**
 * @addtogroup Core Bootloader Core
 * @{
 * @addtogroup Reset
 * @brief Methods to reset from the bootloader to the app
 * @details
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * Reset from the bootloader with a reset cause
 *
 * @param resetReason A reset reason as defined in
 *                    [the bootloader interface](@ref ResetInterface)
 ******************************************************************************/
SL_NORETURN void reset_resetWithReason(uint16_t resetReason);

/***************************************************************************//**
 * Set a reset reason
 *
 * @param resetReason A reset reason as defined in
 *                    [the bootloader interface](@ref ResetInterface)
 ******************************************************************************/
void reset_setResetReason(uint16_t resetReason);

/***************************************************************************//**
 * Get the reset reason without verifying it
 *
 * @return The reset reason
 ******************************************************************************/
uint16_t reset_getResetReason(void);

/***************************************************************************//**
 * Invalidate the reset reason
 ******************************************************************************/
void reset_invalidateResetReason(void);

/***************************************************************************//**
 * Classify reset and get the reset reason
 *
 * @return Reset cause or @ref BOOTLOADER_RESET_REASON_UNKNOWN
 ******************************************************************************/
uint16_t reset_classifyReset(void);

/** @} addtogroup reset */
/** @} addtogroup core */

#endif // BTL_RESET_H
