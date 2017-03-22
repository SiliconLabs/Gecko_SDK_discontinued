/***************************************************************************//**
 * @file btl_bootload.h
 * @brief Bootloading functionality for the Silicon Labs bootloader
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
#ifndef BTL_BOOTLOAD_H
#define BTL_BOOTLOAD_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/***************************************************************************//**
 * @addtogroup Core Bootloader Core
 * @{
 * @addtogroup Bootload
 * @brief Methods to verify and bootload application images
 * @details
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * Verify the app starting at address startAddress
 *
 * @param[in] startAddress Starting address of the application
 *
 * @return True if the image signature is valid
 ******************************************************************************/
bool bootload_verifyApplication(uint32_t startAddress);

/***************************************************************************//**
 * Bootloader upgrade callback implementation
 *
 * @param offset          Offset of bootloader data (byte counter incrementing from 0)
 * @param data            Raw bootloader data
 * @param length          Size in bytes of raw bootloader data.
 * @param context         A context variable defined by the implementation that
 *                        is implementing this callback.
 ******************************************************************************/
void bootload_bootloaderCallback(uint32_t offset,
                                 uint8_t  data[],
                                 size_t   length,
                                 void     *context);

/***************************************************************************//**
 * Image data callback implementation
 *
 * @param address         Address (inside the raw image) the data starts at
 * @param data            Raw image data
 * @param length          Size in bytes of raw image data. Constrained to always
 *                        be a multiple of four.
 * @param context         A context variable defined by the implementation that
 *                        is implementing this callback.
 ******************************************************************************/
void bootload_applicationCallback(uint32_t address,
                                  uint8_t data[],
                                  size_t length,
                                  void *context);

/** @} addtogroup bootload */
/** @} addtogroup core */

#endif // BTL_BOOTLOAD_H
