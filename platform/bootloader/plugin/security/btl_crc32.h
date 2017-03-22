/***************************************************************************//**
 * @file btl_crc32.h
 * @brief CRC32 functionality for Silicon Labs bootloader
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
#ifndef BTL_CRC32_H
#define BTL_CRC32_H

#include <stdint.h>
#include <stddef.h>

/***************************************************************************//**
 * @addtogroup Plugin
 * @{
 * @addtogroup Security
 * @{
 * @addtogroup CRC32
 * @{
 * @brief CRC32 functionality for the bootloader
 * @details
 ******************************************************************************/

/// CRC32 start value
#define BTL_CRC32_START             0xFFFFFFFFUL
/// CRC32 end value
#define BTL_CRC32_END               0xDEBB20E3UL

/***************************************************************************//**
 * Calculate CRC32 on input
 *
 * @param newByte    Byte to append to CRC32 calculation
 * @param prevResult Previous output from CRC algorithm. Polynomial if starting
 *   a new calculation.
 * @return Result of the CRC32 operation
 ******************************************************************************/
uint32_t btl_crc32(const uint8_t newByte, uint32_t prevResult);

/***************************************************************************//**
 * Calculate CRC32 on input stream
 *
 * @param buffer     buffer containing bytes to append to CRC32 calculation
 * @param length     Size of the buffer in bytes
 * @param prevResult Previous output from CRC algorithm. Polynomial if starting
 *   a new calculation.
 * @returns Result of the CRC32 operation
 ******************************************************************************/
uint32_t btl_crc32Stream(const uint8_t* buffer, size_t length, uint32_t prevResult);

/** @} addtogroup CRC32 */
/** @} addtogroup Security */
/** @} addtogroup Plugin */

#endif // BTL_CRC32_H
