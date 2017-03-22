/***************************************************************************//**
 * @file btl_security_aes.c
 * @brief AES decryption functionality for Silicon Labs bootloader
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

#include "btl_security_aes.h"
#include "btl_security_types.h"

#include <string.h> // For memory copy functions

// Debug
#include "plugin/debug/btl_debug.h"
#include <stdio.h>

// Initialize AES context variable
void btl_initAesContext(void *ctx)
{
  AesContext_t *context = (AesContext_t *)ctx;
  mbedtls_aes_init(&(context->aesContext));
}

// Set initial key
void btl_setAesKey(void *ctx, uint8_t *key, unsigned int keySize, bool encryptNotDecrypt)
{
  AesContext_t *context = (AesContext_t *)ctx;
  if(encryptNotDecrypt) {
    mbedtls_aes_setkey_enc(&(context->aesContext), key, keySize);
  } else {
    mbedtls_aes_setkey_dec(&(context->aesContext), key, keySize);
  }
}

// Process AES block. Block size is 16 bytes == 128 bit.
void btl_processAesBlock(void *ctx, uint8_t *inputBlock, uint8_t *outputBlock, bool encryptNotDecrypt)
{
  AesContext_t *context = (AesContext_t *)ctx;
  if(encryptNotDecrypt) {
    mbedtls_aes_crypt_ecb(&(context->aesContext), MBEDTLS_AES_ENCRYPT, inputBlock, outputBlock);
  } else {
    mbedtls_aes_crypt_ecb(&(context->aesContext), MBEDTLS_AES_DECRYPT, inputBlock, outputBlock);
  }
}

// Initialize AES-CTR algorithm in CCM mode as used by EBL
void btl_initAesCcm(void *ctx, uint8_t flags, uint8_t *nonce, uint32_t counter, uint8_t *key, unsigned int keySize)
{
  AesCtrContext_t *context = (AesCtrContext_t *)ctx;
  // Store the key
  mbedtls_aes_init(&(context->aesContext));
  mbedtls_aes_setkey_enc(&(context->aesContext), key, keySize);

  // Indicate start of stream by setting offset to 0
  context->offsetInBlock = 0;

  // CCM uses counter mode with the following concatenated:
  //  * flags   (1  byte)
  //  * nonce   (12 bytes)
  //  * counter (3  bytes)
  context->counter[0] = flags;
  memcpy(&(context->counter[1]), nonce, 12);
  context->counter[13] = (counter & 0x00FF0000) >> 16;
  context->counter[14] = (counter & 0x0000FF00) >> 8;
  context->counter[15] = (counter & 0x000000FF);
}

// Process AES-CTR data.
void btl_processAesCtrData(void *ctx, const uint8_t *input, uint8_t *output, size_t length)
{
  AesCtrContext_t *context = (AesCtrContext_t *)ctx;
  mbedtls_aes_crypt_ctr(&(context->aesContext),
                        length,
                        &(context->offsetInBlock),
                        context->counter,
                        context->streamBlock,
                        input,
                        output);
}
