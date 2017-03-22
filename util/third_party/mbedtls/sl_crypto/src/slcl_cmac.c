/**
 *  CMAC cipher mode based on 128 bit AES and CRYPTO hw acceleration
 *
 *  Copyright (C) 2016, Silicon Labs, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

/*
 *  This file implements CMAC (Cipher-based Message Authentication Code)
 *  cipher mode encryption/decryption based on 128 bit AES.
 *  For a general description please see
 *  https://en.wikipedia.org/wiki/CMAC
 *  or for detailed specification see
 *  http://csrc.nist.gov/publications/nistpubs/800-38B/SP_800-38B.pdf
 *  http://dl.acm.org/citation.cfm?id=2206249
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined( MBEDTLS_CMAC_C )

#if defined( MBEDTLS_CMAC_ALT )

#include "em_device.h"

#if defined(CRYPTO_COUNT) && (CRYPTO_COUNT > 0)

#if defined( MBEDTLS_SLCL_PLUGINS )

#include "mbedtls/cmac.h"
#include "mbedtls/aes.h"
#include "sl_crypto.h"
#include "aesdrv_internal.h"
#include "cryptodrv_internal.h"
#include "em_assert.h"
#include <string.h>

#if defined(MBEDTLS_SELF_TEST) && defined(MBEDTLS_AES_C)
#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#include <stdio.h>
#define mbedtls_printf printf
#endif /* MBEDTLS_PLATFORM_C */
#endif /* MBEDTLS_SELF_TEST && MBEDTLS_AES_C */

/* Implementation that should never be optimized out by the compiler */
static void mbedtls_zeroize( void *v, size_t n ) {
    volatile unsigned char *p = v; while( n-- ) *p++ = 0;
}

/*
 * Initialize context
 */
void mbedtls_cmac_init( mbedtls_cmac_context *ctx )
{
    memset( ctx, 0, sizeof( mbedtls_cmac_context ) );
    
    AESDRV_Init ( &ctx->aesdrv_ctx );
    AESDRV_SetDeviceInstance ( &ctx->aesdrv_ctx, 0 );
    mbedtls_cmac_set_device_lock_wait_ticks( ctx, 0 );
    AESDRV_SetIoMode         ( &ctx->aesdrv_ctx, aesdrvIoModeCore, 0 );
}

/*
 * CMAC key setup
 */
int mbedtls_cmac_setkey( mbedtls_cmac_context *ctx,
                         mbedtls_cipher_id_t cipher,
                         const unsigned char *key,
                         unsigned int keybits )
{
    if ( cipher != MBEDTLS_CIPHER_ID_AES )
        return( MBEDTLS_ERR_CMAC_BAD_INPUT );
    
    switch( keybits )
    {
    case 128:
      break;
    case 192:
    case 256:
        return( MBEDTLS_ERR_AES_INVALID_KEY_LENGTH );
    default:
        return( MBEDTLS_ERR_CMAC_BAD_INPUT );
    }
    
    ctx->keybits = keybits;
    memcpy(ctx->key, key, keybits/8);
    
    return( 0 );
}

/*
 * Set the device instance of an CMAC context.
 */
int mbedtls_cmac_set_device_instance(mbedtls_cmac_context *ctx,
                                     unsigned int          devno)
{
#if defined(AES_COUNT) && (AES_COUNT > 0)
    (void) ctx;
    if ((devno >= AES_COUNT) || (devno != 0))
        return( MBEDTLS_ERR_CMAC_BAD_INPUT );
    else
        return( 0 );
#endif
  
#if defined(CRYPTO_COUNT) && (CRYPTO_COUNT > 0)
    if (devno >= CRYPTO_COUNT)
        return( MBEDTLS_ERR_CMAC_BAD_INPUT );
  
    return cryptodrvSetDeviceInstance( &ctx->aesdrv_ctx.cryptodrvContext,
                                       devno );
#endif /* #if defined(CRYPTO_COUNT) && (CRYPTO_COUNT > 0) */
}

/*
 *   Set the number of ticks to wait for the device lock.
 */
int mbedtls_cmac_set_device_lock_wait_ticks(mbedtls_cmac_context *ctx,
                                            int                   ticks)
{
    int ret = 0;
    
#if defined(CRYPTO_COUNT) && (CRYPTO_COUNT > 0)
    
    ret = cryptodrvSetDeviceLockWaitTicks( &ctx->aesdrv_ctx.cryptodrvContext,
                                           ticks );
#else
    
    (void) ctx;
    (void) ticks;

#endif /* #if defined(CRYPTO_COUNT) && (CRYPTO_COUNT > 0) */
    
    return ret;
}

/*
 * Free context
 */
void mbedtls_cmac_free( mbedtls_cmac_context *ctx )
{
    AESDRV_DeInit ( &ctx->aesdrv_ctx );
    
    mbedtls_zeroize( ctx, sizeof( mbedtls_cmac_context ) );
}

/*
 * Generate CMAC tag
 */
int mbedtls_cmac_generate_tag( mbedtls_cmac_context *ctx,
                               const unsigned char  *data,
                               size_t                data_len,
                               unsigned char        *tag,
                               size_t                tag_len )
{
    int ret = AESDRV_CMAC(&ctx->aesdrv_ctx,
                                data, data_len,
                                (uint8_t*)ctx->key, 128/8,
                                tag, tag_len,
                                true);
    return ( 0 == ret ? 0 :
             ( MBEDTLS_ECODE_AESDRV_INVALID_PARAM == ret ?
               MBEDTLS_ERR_CMAC_BAD_INPUT : (int)ret
               ) );
}

/*
 * Verify CMAC tag
 *
 */
int mbedtls_cmac_verify_tag( mbedtls_cmac_context *ctx,
                             const unsigned char  *data,
                             size_t                data_len,
                             unsigned char        *tag,
                             size_t                tag_len )
{
    int ret = AESDRV_CMAC(&ctx->aesdrv_ctx,
                                data, data_len,
                                (uint8_t*)ctx->key, 128/8,
                                tag, tag_len,
                                false);
    return ( 0 == ret ? 0 :
             ( MBEDTLS_ECODE_AESDRV_AUTHENTICATION_FAILED == ret ?
               MBEDTLS_ERR_CMAC_AUTH_FAILED :
               ( MBEDTLS_ECODE_AESDRV_INVALID_PARAM == ret ?
                 MBEDTLS_ERR_CMAC_BAD_INPUT : (int)ret
                 ) ) );
}

#if defined(MBEDTLS_SELF_TEST) && defined(MBEDTLS_CMAC_C)

/*
 * CMAC self test
 */

#include "timing.h"
#include <ctype.h>

typedef struct {
  char* key;
  int   keylen;
  char* plaintext;
  int   plaintextlen;
  char* authtag;
  int   authtaglen;
} cmac_test_vector_t;

static const cmac_test_vector_t cmac_test_vectors [];
static int hex2uint8array(uint8_t* u8a, int u8alen, const char* hex);

/* Test the CMAC algorithm. */
static int test_single_cmac (const cmac_test_vector_t* tv,
                             int verbose, int device_instance )
{
    uint8_t* key          = (uint8_t*) mbedtls_calloc (1, tv->keylen);
    uint8_t* message      = (uint8_t*) mbedtls_calloc (1, tv->plaintextlen+16);
    uint8_t* authTag      = (uint8_t*) mbedtls_calloc (1, 16);
    uint8_t* authTagExpected = (uint8_t*) mbedtls_calloc (1, 16);
    int      ret;
    int      cycles;
    mbedtls_cmac_context ctx;
  
    mbedtls_cmac_init( &ctx );
    mbedtls_cmac_set_device_instance(&ctx, device_instance);

    if ( (NULL==key) ||
         (NULL==message) ||
         (NULL==authTag) ||
         (NULL==authTagExpected) )
    {
        if (verbose)
            mbedtls_printf("ERROR %s, %d: Unable to allocate memory buffers.\n",
                           __FILE__, __LINE__);
        return -1;
    }

    mbedtls_timing_init();

    hex2uint8array(key,                tv->keylen,         tv->key);
    hex2uint8array(message,            tv->plaintextlen,   tv->plaintext);
    hex2uint8array(authTagExpected,    tv->authtaglen,     tv->authtag);

    /* Set key */
    ret = mbedtls_cmac_setkey(&ctx, MBEDTLS_CIPHER_ID_AES, key, 128);
    if (0 != ret)
    {
        mbedtls_printf("mbedtls_cmac_setkey returned error code 0x%x.\n", ret);
        ret = -1;
        goto exit;
    }
        
    cycles = mbedtls_timing_hardclock();
        
    /* Perform the CMAC */
    ret = mbedtls_cmac_generate_tag(&ctx,
                                    message,
                                    tv->plaintextlen*8,
                                    authTag,
                                    tv->authtaglen*8);

    if (0 != ret)
    {
        mbedtls_printf("mbedtls_cmac_encrypt_tag returned error code 0x%x.\n", ret);
        ret = -1;
        goto exit;
    }
        
    if (0 != memcmp(authTagExpected, authTag, tv->authtaglen))
    {
        mbedtls_printf("mbedtls_cmac_encrypt_tag failed to produce expected tag.\n");
        ret = -1;
        goto exit;
    }
        
    if (verbose)
    {
        mbedtls_printf("%10d %12d %6d\n",
                       tv->authtaglen, tv->plaintextlen, cycles);
    }
 exit:
    mbedtls_cmac_free( &ctx );
  
    if (key)                 mbedtls_free(key);
    if (message)             mbedtls_free(message);
    if (authTag)             mbedtls_free(authTag);
    if (authTagExpected)     mbedtls_free(authTagExpected);

    return( ret );
}

int mbedtls_cmac_self_test( int verbose, int device_instance )
{
  const cmac_test_vector_t* tv = cmac_test_vectors;

  mbedtls_printf("\nCMAC Hashing Test AES-128\n"
                 "AuthTagLen PlaintextLen Cycles\n");
  while (tv->key)
  {
    if (test_single_cmac( tv, verbose, device_instance ) != 0)
      return -1;
    tv++;
  }
  return 0;
}

static int hex2uint8array(uint8_t* u8a, int u8alen, const char* hex)
{
  int i, j, k, c;
  int num;

  if ((hex == NULL) || (*hex == '\0')) return(0);

  for (i=0; isxdigit((unsigned char) hex[i]); i++)
    ;
  
  num=i;

  if (num % 2)
    /* hex array must be a multiple of 2 */
    return -1;

  if (u8a == NULL) return(num);
  
  if (u8alen != num/2)
    /* size of uint8 array must be half the size of the hex array. */
    return -1;
  
  memset(u8a, 0, u8alen);

  for (i = 0; i < num/2; i++)
  {
    for (j=0; j<2; j++)
    {
      c=hex[i*2+j];
      if ((c >= '0') && (c <= '9')) k=c-'0';
      else if ((c >= 'a') && (c <= 'f')) k=c-'a'+10;
      else if ((c >= 'A') && (c <= 'F')) k=c-'A'+10;
      else k=0; /* paranoia */
      u8a[i]=(u8a[i]<<4)|k;
    }
  }
  return(num);
}

/*
** From file CMACGenAES128.rsp :

# CAVS 14.0
# CMAC Encrypt with keysize 128 test information
# Generated on Fri Aug 31 11:23:06 2012
*/
static const cmac_test_vector_t cmac_test_vectors [] =
{
  { "2b7e151628aed2a6abf7158809cf4f3c",
    16,
    "",
    0,
    "bb1d6929e95937287fa37d129b756746",
    16
  },
  { "8eeca0d146fd09ffbbe0d47edcddfcec",
    16,
    "",
    0,
    "c3642ce5",
    4
  },
  { "f7f922c86706277a4e98d28e1197413b",
    16,
    "33ce44bdb1ea6fffe5a29004e2cbf66c",
    16,
    "b8768355644df5a9fdff2def763f63",
    15
  },
  { "6533780fc328a88d605268d62f295dc6",
    16,
    "02749f4f9ad82fa7ba41d935a6f1aa6376b30b8775b6445ac89b3eac50cd8d56",
    32,
    "0bfa134a",
    4
  },
  { "e4abe343f98a2df09413c3defb85b56a",
    16,
    "f799876d19ac1b849a1a43fe9912bcaf6e1e3896ea58bcb2dfdc4716e379b440",
    32,
    "e08428dbbc13ff9432048c0ad95731",
    15
  },
  {0}
};

#endif /* MBEDTLS_SELF_TEST && MBEDTLS_CMAC_C */

#endif /* MBEDTLS_SLCL_PLUGINS */

#endif /* #if defined(CRYPTO_COUNT) && (CRYPTO_COUNT > 0) */

#endif /* MBEDTLS_CMAC_ALT */

#endif /* MBEDTLS_CMAC_C */
