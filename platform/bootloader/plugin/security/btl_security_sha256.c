/***************************************************************************//**
 * @file btl_security_sha256.c
 * @brief SHA-256 digest functionality for Silicon Labs bootloader
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

#include "btl_security_sha256.h"
#include "btl_security_types.h"
#include "api/btl_errorcode.h"

#if BTL_SECURITY_SHA256_DIGEST_LENGTH % 4 != 0
#error "SHA digest size is not a multiple of native data type"
#endif

/** This function will initialize the CCM state struct and must be called
 *  before using the struct in any processing.
 */
void btl_initSha256(void *ctx)
{
  Sha256Context_t *context = (Sha256Context_t *)ctx;
  mbedtls_sha256_init(&(context->shaContext));       //Zero out the context struct
  mbedtls_sha256_starts(&(context->shaContext), 0);  //Load the SHA256 IV
}

/** Push data into the SHA algorithm. If the data is not a full SHA block,
 *  mbedTLS will buffer until it has a full one.
 */
void btl_updateSha256(void *ctx, const void *data, size_t length)
{
  Sha256Context_t *context = (Sha256Context_t *)ctx;
  mbedtls_sha256_update(&(context->shaContext), data, length);
}

/** Finalize the SHA hash. This will run the remainder of the buffer through
 *  the algorithm, padding and adding the counter as necessary.
 */
void btl_finalizeSha256(void *ctx)
{
  Sha256Context_t *context = (Sha256Context_t *)ctx;
  mbedtls_sha256_finish(&(context->shaContext), context->sha);
}

/** Verify the SHA hash contained in shaState with the one in the byte array
 *  pointed to. Check the length, too.
 */
int32_t btl_verifySha256(void *ctx, const void *sha)
{
  Sha256Context_t *context = (Sha256Context_t *)ctx;

  if(context == NULL || sha == NULL) {
    return BOOTLOADER_ERROR_SECURITY_INVALID_PARAM;
  }
  unsigned int* sha_calculated = (unsigned int *)context->sha;
  unsigned int* sha_verifying  = (unsigned int *)sha;

  for(unsigned int word = 0; word < (BTL_SECURITY_SHA256_DIGEST_LENGTH / sizeof(unsigned int)); word++) {
    if(sha_verifying[word] != sha_calculated[word]) {
      return BOOTLOADER_ERROR_SECURITY_REJECTED;
    }
  }

  return BOOTLOADER_OK;
}
