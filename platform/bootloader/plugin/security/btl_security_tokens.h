/***************************************************************************//**
 * @file btl_security_tokens.h
 * @brief Manufacturing token handling
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
#ifndef BTL_SECURITY_TOKENS_H
#define BTL_SECURITY_TOKENS_H

#include <stdint.h>

/***************************************************************************//**
 * @addtogroup Plugin
 * @{
 * @addtogroup Security
 * @{
 * @addtogroup Tokens
 * @{
 * @brief Manufacturing token handling for the bootloader
 * @details
 ******************************************************************************/

/***************************************************************************//**
 * Get the X component of the ECDSA secp256r1 public key
 *
 * @return Pointer to X component of public key
 ******************************************************************************/
uint8_t* btl_getSignedBootloaderKeyXPtr(void);

/***************************************************************************//**
 * Get the Y component of the ECDSA secp256r1 public key
 *
 * @return Pointer to Y component of public key
 ******************************************************************************/
uint8_t* btl_getSignedBootloaderKeyYPtr(void);

/***************************************************************************//**
 * Get the AES-CCM encryption key
 *
 * @return Pointer to AES-CCM key
 ******************************************************************************/
uint8_t* btl_getImageFileEncryptionKeyPtr(void);

/** @} addtogroup Tokens */
/** @} addtogroup Security */
/** @} addtogroup Plugin */

#endif // BTL_SECURITY_TOKENS_H
