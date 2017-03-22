/***************************************************************************//**
 * @file btl_crc16.h
 * @brief CRC16 functionality for Silicon Labs bootloader
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
#ifndef BTL_CRC16_H
#define BTL_CRC16_H

#include <stdint.h>
#include <stddef.h>

/***************************************************************************//**
 * @addtogroup Plugin
 * @{
 * @addtogroup Security
 * @{
 * @addtogroup CRC16
 * @{
 * @brief CRC16 functionality for the bootloader
 * @details
 ******************************************************************************/

/// CRC16 start value
#define BTL_CRC16_START             0xFFFFU

/***************************************************************************//**
 * Calculate CRC16 on input
 *
 * @param newByte    Byte to append to CRC16 calculation
 * @param prevResult Previous output from CRC algorithm. @ref BTL_CRC16_START
 *                   when starting a new calculation.
 * @return Result of the CRC16 operation
 ******************************************************************************/
uint16_t btl_crc16(const uint8_t newByte, uint16_t prevResult);

/***************************************************************************//**
 * Calculate CRC16 on input stream
 *
 * @param buffer     buffer containing bytes to append to CRC16 calculation
 * @param length     Size of the buffer in bytes
 * @param prevResult Previous output from CRC algorithm. @ref BTL_CRC16_START
 *                   when starting a new calculation.
 * @returns Result of the CRC16 operation
 ******************************************************************************/
uint16_t btl_crc16Stream(const uint8_t* buffer, size_t length, uint16_t prevResult);

/** @} addtogroup CRC16 */
/** @} addtogroup Security */
/** @} addtogroup Plugin */

#endif // BTL_CRC16_H
