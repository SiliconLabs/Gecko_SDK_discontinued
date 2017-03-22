/***************************************************************************//**
 * @file btl_security_ecdsa.c
 * @brief ECDSA signing functionality for Silicon Labs bootloader
 * @author Silicon Labs
 * @version 1.0.0
 *******************************************************************************
 * @section License
 * <b>Copyright 2016 Silicon Laboratories, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/
#include "btl_security_ecdsa.h"
#include "ecc/ecc.h"
#include "em_device.h"
#include "em_cmu.h"
#include "btl_security_tokens.h"

#include <stddef.h>
#include <string.h> // For memset

/** Verify the ECDSA signature of the SHA hash, using
 *  the public key in the relevant token, with the signature contained in
 *  the byte arrays pointed to.
 */
int32_t btl_verifyEcdsaP256r1(const uint8_t *sha256,
                              const uint8_t *signatureR,
                              const uint8_t *signatureS)
{
  if(sha256 == NULL || signatureR == NULL || signatureS == NULL)
  {
    return BOOTLOADER_ERROR_SECURITY_INVALID_PARAM;
  }

  // Re-enable the clock of the CRYPTO module, since mbedtls turned it off
  CMU_ClockEnable(cmuClock_CRYPTO, true);

  ECC_Point_t pubkey;
  memset(&pubkey, 0, sizeof(ECC_Point_t));
  ECC_ByteArrayToBigInt(pubkey.X, btl_getSignedBootloaderKeyXPtr());
  ECC_ByteArrayToBigInt(pubkey.Y, btl_getSignedBootloaderKeyYPtr());

  ECC_EcdsaSignature_t ecc_signature;
  ECC_ByteArrayToBigInt(ecc_signature.r, signatureR);
  ECC_ByteArrayToBigInt(ecc_signature.s, signatureS);

  return ECC_ECDSA_VerifySignatureP256(CRYPTO, sha256, sizeof(ECC_BigInt_t), &pubkey, &ecc_signature);
}
