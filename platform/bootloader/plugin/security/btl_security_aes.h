/***************************************************************************//**
 * @file btl_security_aes.h
 * @brief AES decryption functionality for Silicon Labs bootloader
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
#ifndef BTL_SECURITY_AES_H
#define BTL_SECURITY_AES_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/***************************************************************************//**
 * @addtogroup Plugin
 * @{
 * @addtogroup Security
 * @{
 * @addtogroup AES
 * @{
 * @brief AES decryption functionality for bootloader
 * @details
 ******************************************************************************/

/***************************************************************************//**
 * Initialize AES context
 *
 * Wipes the AES context struct before use.
 *
 * @param ctx Context variable of type @ref AesContext_t
 ******************************************************************************/
void btl_initAesContext(void *ctx);

/***************************************************************************//**
 * Set AES key to use for encryption/decryption
 *
 * Initializes the AES context struct with the key to use.
 *
 * @param ctx               Context variable of type @ref AesContext_t
 * @param key               Pointer to the AES key
 * @param keySize           Size of the key in bits. Can be 128 or 256.
 * @param encryptNotDecrypt True if using this context for encryption, false
 *                          if using for decryption.
 ******************************************************************************/
void btl_setAesKey(void *ctx,
                   uint8_t *key,
                   unsigned int keySize,
                   bool encryptNotDecrypt);

/***************************************************************************//**
 * Process one block of data using AES-ECB
 *
 * Runs one block of data through the AES algorithm. In-place encryption/
 * decryption is supported.
 *
 * @param ctx               Context variable of type @ref AesContext_t
 * @param inputBlock        128-bit (16 byte) buffer/block of data to be
 *                          en/decrypted
 * @param outputBlock       128-bit (16 byte) buffer/block of data to put the
 *                          result of the en/decryption in.
 * @param encryptNotDecrypt True if encryption, false for decryption
 ******************************************************************************/
void btl_processAesBlock(void *ctx,
                         uint8_t *inputBlock,
                         uint8_t *outputBlock,
                         bool encryptNotDecrypt);

/***************************************************************************//**
 * Set up the AES-CTR context structure in CCM mode
 *
 * Initializes an AES-CTR context struct with parameters used in AES-CCM mode.
 * Data can then be en/decrypted using btl_processAesCtrData.
 *
 * @param ctx     Context variable of type @ref AesCtrContext_t
 * @param flags   CCM flags
 * @param nonce   12-byte nonce specific to this transmission
 * @param counter 3-byte running block counter
 * @param key     Pointer to the AES key
 * @param keySize Size of the key in bits. Can be 128 or 256.
 ******************************************************************************/
void btl_initAesCcm(void *ctx,
                    uint8_t flags,
                    uint8_t *nonce,
                    uint32_t counter,
                    uint8_t *key,
                    unsigned int keySize);

/***************************************************************************//**
 * Process data using AES-CTR
 *
 * Runs data for encryption or decryption (which uses the same function) through
 * the AES-CTR algorithm. In-place encryption/decryption is supported.
 *
 * @param ctx     Context variable of type @ref AesCtrContext_t
 * @param input   Raw data to en/decrypt
 * @param output  Output buffer to put en/decrypted data
 * @param length  Size (in bytes) of the input/output buffers
 ******************************************************************************/
void btl_processAesCtrData(void *ctx,
                           const uint8_t *input,
                           uint8_t *output,
                           size_t length);

/** @} addtogroup AES */
/** @} addtogroup Security */
/** @} addtogroup Plugin */

#endif // BTL_SECURITY_AES_H
