/***************************************************************************//**
 * @file btl_internal_flash.h
 * @brief Abstraction of internal flash read and write routines.
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
#ifndef BTL_INTERNAL_FLASH_H
#define BTL_INTERNAL_FLASH_H

#include "core/btl_util.h"
MISRAC_DISABLE
#include "em_device.h"
MISRAC_ENABLE

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/***************************************************************************//**
 * @addtogroup Core Bootloader Core
 * @{
 * @addtogroup Flash
 * @brief Interface to internal flash
 * @details Used for writing application images to the main flash.
 * @{
 ******************************************************************************/

/**
 * Initialize the flash driver
 */
void flash_init(void);

/**
 * Deinitialize the flash driver
 */
void flash_deinit(void);

/**
 * Erase a flash page
 *
 * @param[in] address Start address of the flash page to erase.
 * @return True if operation was successful
 */
bool flash_erasePage(uint32_t address);

/**
 * Write a word to internal flash
 *
 * @param address Address in internal flash to write word to
 * @param data Word to write
 * @returns True if operation was successful
 */
bool flash_writeWord(uint32_t address, uint32_t data);

/**
 * Write buffer to internal flash
 *
 * @param address Starting address to write data to. Must be half-word aligned.
 * @param data    Data buffer to write to internal flash
 * @param length  Amount of bytes in the data buffer to write
 * @return True if operation was successful
 */
bool flash_writeBuffer(uint32_t       address,
                       void           *data,
                       size_t         length);

/** @} addtogroup internal_flash */
/** @} addtogroup core */

#endif // BTL_INTERNAL_FLASH_H
