/***************************************************************************//**
 * @file btl_security_sha256.h
 * @brief SHA-256 digest functionality for Silicon Labs bootloader.
 * @version 1.0.0
 *******************************************************************************
 * # License
 * <b>(C) Copyright 2016 Silicon Laboratories, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/
#ifndef BTL_SECURITY_SHA256_H
#define BTL_SECURITY_SHA256_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/***************************************************************************//**
 * @addtogroup Plugin
 * @{
 * @addtogroup Security
 * @{
 * @addtogroup SHA-256
 * @{
 * @brief SHA-256 digest functionality for the bootloader
 * @details
 ******************************************************************************/

/// Number of bytes in a SHA-256 digest
#define BTL_SECURITY_SHA256_DIGEST_LENGTH   32

/***************************************************************************//**
 * Initialize SHA256 context variable
 *
 * @param ctx Pointer to the SHA256 context variable to be initialized
 *
 * Wipes out the SHA256 context variable and sets it up for re-use.
 ******************************************************************************/
void btl_initSha256(void *ctx);

/***************************************************************************//**
 * Run data through the SHA256 hashing function
 *
 * @param ctx  Pointer to the SHA256 context variable
 * @param data Pointer to array of binary data to add to the SHA256 calculation
 *   in progress
 * @param length Length of the byte array with data.
 *
 ******************************************************************************/
void btl_updateSha256(void *ctx, const void *data, size_t length);

/***************************************************************************//**
 * Finalize SHA256 calculation
 *
 * @param ctx Pointer to the SHA256 context variable to be initialized
 *
 * Finalizes the running SHA256 calculation. After finalization, the SHA value
 *   in the context variable will be valid, and no more data can be added.
 ******************************************************************************/
void btl_finalizeSha256(void *ctx);

/***************************************************************************//**
 * Compare SHA256 from context variable to a known value
 *
 * @param ctx    Pointer to the SHA256 context variable to be initialized
 * @param sha256 Byte array containing sha256 value to compare to
 * @return @ref BOOTLOADER_OK if both hash values are equal, else error code
 *         from @ref BOOTLOADER_ERROR_SECURITY_BASE range.
 *
 * Convenience function to compare a known SHA256 hash against the result of
 *   a calculation.
 ******************************************************************************/
int32_t btl_verifySha256(void *ctx, const void *sha256);

/** @} addtogroup SHA-256 */
/** @} addtogroup Security */
/** @} addtogroup Plugin */

#endif // BTL_SECURITY_SHA256_H
