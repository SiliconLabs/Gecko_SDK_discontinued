/***************************************************************************//**
 * @file btl_security_ecdsa.h
 * @brief ECDSA signing functionality for Silicon Labs bootloader
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
#ifndef BTL_SECURITY_ECDSA_H
#define BTL_SECURITY_ECDSA_H

#include <stdint.h>
#include <stdbool.h>
#include "api/btl_errorcode.h"

/***************************************************************************//**
 * @addtogroup Plugin
 * @{
 * @addtogroup Security
 * @{
 * @addtogroup ECDSA
 * @{
 * @brief ECDSA signing functionality for the bootloader
 * @details
 ******************************************************************************/

/// Number of bytes of data to verify the signature against
#define BTL_SECURITY_ECDSA_SHA256_LENGTH    32
/// Number of bytes in the EC points that the signature consists of
#define BTL_SECURITY_ECDSA_POINT_LENGTH     32

/***************************************************************************//**
 * Verify an ECDSA signature of a SHA256-hash using secp256r1
 *
 * Verifies the authenticity of data by checking the ECDSA signature of the
 * data's SHA256-hash. This function is only for use with the secp256r1
 * curve. The public key which the signature is validated against will be
 * retrieved from the respective tokens in the lockbits-page.
 *
 * @param sha256     The hash of the data which is authenticated
 * @param signatureR byte array (MSB first) of R-point of the ECDSA signature
 * @param signatureS byte array (MSB first) of S-point of the ECDSA signature
 * @return @ref BOOTLOADER_OK if signature is valid, else error code
 *         in @ref BOOTLOADER_ERROR_SECURITY_BASE range.
 ******************************************************************************/
int32_t btl_verifyEcdsaP256r1(const uint8_t *sha256,
                              const uint8_t *signatureR,
                              const uint8_t *signatureS);

/** @} addtogroup ECDSA */
/** @} addtogroup Security */
/** @} addtogroup Plugin */

#endif // BTL_SECURITY_ECDSA_H
