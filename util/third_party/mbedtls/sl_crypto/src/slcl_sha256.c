/*
 *  FIPS-180-2 compliant SHA-256 implementation
 *
 *  Copyright (C) 2016, Silicon Labs, http://www.silabs.com
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
 * This file includes an alternative implementation of the standard
 * mbedtls/libary/sha256.c using the CRYPTO hardware accelerator incorporated
 * in MCU devices from Silicon Laboratories.
 */
/*
 *  The SHA-256 Secure Hash Standard was published by NIST in 2002.
 *
 *  http://csrc.nist.gov/publications/fips/fips180-2/fips180-2.pdf
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_SHA256_C)

#if defined(MBEDTLS_SHA256_ALT)

#if defined(MBEDTLS_SLCL_PLUGINS)

#include "em_device.h"

#if defined(CRYPTO_COUNT) && (CRYPTO_COUNT > 0)

#include "mbedtls/sha256.h"
#include "cryptodrv_internal.h"
#include "em_assert.h"
#include <string.h>

/* Implementation that should never be optimized out by the compiler */
static void mbedtls_zeroize( void *v, size_t n ) {
    volatile unsigned char *p = v; while( n-- ) *p++ = 0;
}

/*
 * 32-bit integer manipulation macros (big endian)
 */
#ifndef PUT_UINT32_BE
#define PUT_UINT32_BE(n,b,i)                            \
do {                                                    \
    (b)[(i)    ] = (unsigned char) ( (n) >> 24 );       \
    (b)[(i) + 1] = (unsigned char) ( (n) >> 16 );       \
    (b)[(i) + 2] = (unsigned char) ( (n) >>  8 );       \
    (b)[(i) + 3] = (unsigned char) ( (n)       );       \
} while( 0 )
#endif

void mbedtls_sha256_init( mbedtls_sha256_context *ctx )
{
    memset( ctx, 0, sizeof( mbedtls_sha256_context ) );
    
    /* Set device instance and lock wait ticks to 0 by default. */
    mbedtls_sha256_set_device_instance(ctx, 0);
    mbedtls_sha256_set_device_lock_wait_ticks( ctx, 0 );
}

void mbedtls_sha256_free( mbedtls_sha256_context *ctx )
{
    if( ctx == NULL )
        return;

    mbedtls_zeroize( ctx, sizeof( mbedtls_sha256_context ) );
}

/*
 * Set the device instance of an SHA context.
 */
int mbedtls_sha256_set_device_instance(mbedtls_sha256_context *ctx,
                                       unsigned int            devno)
{
#if defined(CRYPTO_COUNT) && (CRYPTO_COUNT > 0)
    if (devno >= CRYPTO_COUNT)
        return( MBEDTLS_ERR_SHA256_BAD_INPUT );
  
    return cryptodrvSetDeviceInstance( &ctx->cryptodrv_ctx, devno );
#endif /* #if defined(CRYPTO_COUNT) && (CRYPTO_COUNT > 0) */
}

/*
 *   Set the number of ticks to wait for the device lock.
 */
int mbedtls_sha256_set_device_lock_wait_ticks(mbedtls_sha256_context *ctx,
                                              int                   ticks)
{
    int ret = 0;
    
#if defined(CRYPTO_COUNT) && (CRYPTO_COUNT > 0)
    
    ret = cryptodrvSetDeviceLockWaitTicks( &ctx->cryptodrv_ctx, ticks );
    
#else
    
    (void) ctx;
    (void) ticks;
    
#endif /* #if defined(CRYPTO_COUNT) && (CRYPTO_COUNT > 0) */
    
    return ret;
}

void mbedtls_sha256_clone( mbedtls_sha256_context *dst,
                           const mbedtls_sha256_context *src )
{
    (void) dst;
    (void) src;

    /* Cloning a SHA256 CRYPTODRV context is not supported. */
    while(1);
}

/*
 * SHA-256 context setup
 */
int mbedtls_sha256_starts( mbedtls_sha256_context *ctx, int is224 )
{
    CRYPTODRV_Context_t* cryptodrv_ctx = &ctx->cryptodrv_ctx;
    CRYPTO_TypeDef* crypto = cryptodrv_ctx->device->crypto;
    uint32_t init_state[8];
    int ret;

    /* Request CRYPTO usage. */
    ret = CRYPTODRV_Arbitrate(cryptodrv_ctx);
    if (0 != ret)
    {
        return( ret );
    }
    
    /* Enter critial crypto region in order to initialize crypto for
       SHA operation. */
    ret = CRYPTODRV_EnterCriticalRegion(cryptodrv_ctx);
    EFM_ASSERT(0 == ret); /* Assert critical region entry is ok. */
    
    /* Setup CRYPTO for SHA-2 operation: */
    crypto->CTRL     = CRYPTO_CTRL_SHA_SHA2;
    crypto->WAC      = 0;
    crypto->IEN      = 0;

    /* Set result width of MADD32 operation. */
    CRYPTO_ResultWidthSet(crypto, cryptoResult256Bits);
  
    /* Clear sequence control registers */
    crypto->SEQCTRL  = 0;
    crypto->SEQCTRLB = 0;
    
    /* Setup the initial sha digest state */
    if( is224 == 0 )
    {
        /* SHA-256 */
        init_state[0] = 0x6A09E667;
        init_state[1] = 0xBB67AE85;
        init_state[2] = 0x3C6EF372;
        init_state[3] = 0xA54FF53A;
        init_state[4] = 0x510E527F;
        init_state[5] = 0x9B05688C;
        init_state[6] = 0x1F83D9AB;
        init_state[7] = 0x5BE0CD19;
    }
    else
    {
        /* SHA-224 */
        init_state[0] = 0xC1059ED8;
        init_state[1] = 0x367CD507;
        init_state[2] = 0x3070DD17;
        init_state[3] = 0xF70E5939;
        init_state[4] = 0xFFC00B31;
        init_state[5] = 0x68581511;
        init_state[6] = 0x64F98FA7;
        init_state[7] = 0xBEFA4FA4;
    }

    /* Push init vector to crypto module */
    CRYPTO_DDataWrite(&crypto->DDATA1, init_state);

    /* Initialize CRYPTO sequencer to execute main SHA instruction
       sequence. */
    CRYPTO_EXECUTE_2( crypto,
                      CRYPTO_CMD_INSTR_DDATA1TODDATA0,
                      CRYPTO_CMD_INSTR_DDATA1TODDATA2 );
    
    /* Load main SHA instruction sequence */
    CRYPTO_SEQ_LOAD_4( crypto,
                       CRYPTO_CMD_INSTR_SELDDATA0DDATA1,
                       CRYPTO_CMD_INSTR_SHA,
                       CRYPTO_CMD_INSTR_MADD32,
                       CRYPTO_CMD_INSTR_DDATA0TODDATA1 );
    
    ret = CRYPTODRV_ExitCriticalRegion( cryptodrv_ctx );
    EFM_ASSERT(0 == ret); /* Assert critical region exit is ok. */
    
    ctx->total[0] = 0;
    ctx->total[1] = 0;

    ctx->is224 = is224;

    return ( ret );
}

void mbedtls_sha256_process( mbedtls_sha256_context *ctx, const unsigned char data[64] )
{
    CRYPTODRV_Context_t* cryptodrv_ctx = &ctx->cryptodrv_ctx;
    CRYPTO_TypeDef* crypto = cryptodrv_ctx->device->crypto;
    int ret;

    ret = CRYPTODRV_EnterCriticalRegion( cryptodrv_ctx );
    EFM_ASSERT(0 == ret); /* Assert critical region entry is ok. */

    /* Write block to QDATA1.  */
    /* Check data is 32bit aligned, if not move via aligned buffer before writing. */
    if ((uint32_t)data & 0x3)
    {
        uint32_t temp[16];
        memcpy(temp, data, 64);
        CRYPTO_QDataWrite(&crypto->QDATA1BIG, temp);
    }
    else
    {
        CRYPTO_QDataWrite(&crypto->QDATA1BIG, (uint32_t*) data);
    }
    
    /* Execute SHA */
    crypto->CMD = CRYPTO_CMD_SEQSTART;
    
    /* Wait for completion */
    CRYPTO_InstructionSequenceWait(crypto);
    
    ret = CRYPTODRV_ExitCriticalRegion( cryptodrv_ctx );
    EFM_ASSERT(0 == ret); /* Assert critical region exit is ok. */
}

/*
 * SHA-256 process buffer
 */
void mbedtls_sha256_update( mbedtls_sha256_context *ctx, const unsigned char *input,
                    size_t ilen )
{
    size_t fill;
    uint32_t left;

    if( ilen == 0 )
        return;

    left = ctx->total[0] & 0x3F;
    fill = 64 - left;

    ctx->total[0] += (uint32_t) ilen;
    ctx->total[0] &= 0xFFFFFFFF;

    if( ctx->total[0] < (uint32_t) ilen )
        ctx->total[1]++;

    if( left && ilen >= fill )
    {
        memcpy( (void *) (ctx->buffer + left), input, fill );
        mbedtls_sha256_process( ctx, ctx->buffer );
        input += fill;
        ilen  -= fill;
        left = 0;
    }
    
    while( ilen >= 64 )
    {
        mbedtls_sha256_process( ctx, input );
        input += 64;
        ilen  -= 64;
    }

    if( ilen > 0 )
        memcpy( (void *) (ctx->buffer + left), input, ilen );
}

static const unsigned char sha256_padding[64] =
{
 0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*
 * SHA-256 final digest
 */
void mbedtls_sha256_finish( mbedtls_sha256_context *ctx, unsigned char output[32] )
{
    uint32_t last, padn;
    uint32_t high, low;
    unsigned char msglen[8];
    CRYPTODRV_Context_t* cryptodrv_ctx = &ctx->cryptodrv_ctx;
    CRYPTO_TypeDef* crypto = cryptodrv_ctx->device->crypto;
    int ret;

    high = ( ctx->total[0] >> 29 )
         | ( ctx->total[1] <<  3 );
    low  = ( ctx->total[0] <<  3 );

    PUT_UINT32_BE( high, msglen, 0 );
    PUT_UINT32_BE( low,  msglen, 4 );

    last = ctx->total[0] & 0x3F;
    padn = ( last < 56 ) ? ( 56 - last ) : ( 120 - last );

    mbedtls_sha256_update( ctx, sha256_padding, padn );
    mbedtls_sha256_update( ctx, msglen, 8 );

    /* Enter critical CRYPTO region in order to read final SHA digest/state. */
    ret = CRYPTODRV_EnterCriticalRegion( cryptodrv_ctx );
    EFM_ASSERT(0 == ret); /* Assert critical region entry is ok. */
    
    /* Read the digest from crypto (big endian). */
    CRYPTODRV_DDataReadUnaligned(&crypto->DDATA0BIG, output);
    
    ret = CRYPTODRV_ExitCriticalRegion( cryptodrv_ctx );
    EFM_ASSERT(0 == ret); /* Assert critical region exit is ok. */

    /* Finally release CRYPTO since SHA operation has completed. */
    ret = CRYPTODRV_Release( cryptodrv_ctx );
    EFM_ASSERT(0 == ret); /* Assert crypto release is ok. */
    
    if( ctx->is224 )
      memset(&output[28], 0, 4);
}

/*
 * output = SHA-256( input buffer )
 */
int mbedtls_sha256( const unsigned char *input, size_t ilen,
                       unsigned char output[32], int is224 )
{
    mbedtls_sha256_context ctx;
    int ret;

    mbedtls_sha256_init( &ctx );

    ret = mbedtls_sha256_starts( &ctx, is224 );
    if (ret != 0)
      return ret;
    
    mbedtls_sha256_update( &ctx, input, ilen );
    mbedtls_sha256_finish( &ctx, output );
    mbedtls_sha256_free( &ctx );
    
    return( 0 );
}

#if defined(MBEDTLS_SELF_TEST)

#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#include <stdio.h>
#define mbedtls_printf printf
#endif /* MBEDTLS_PLATFORM_C */

/*
 * FIPS-180-2 test vectors
 */
static const unsigned char sha256_test_buf[3][57] =
{
    { "abc" },
    { "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq" },
    { "" }
};

static const int sha256_test_buflen[3] =
{
    3, 56, 1000
};

static const unsigned char sha256_test_sum[6][32] =
{
    /*
     * SHA-224 test vectors
     */
    { 0x23, 0x09, 0x7D, 0x22, 0x34, 0x05, 0xD8, 0x22,
      0x86, 0x42, 0xA4, 0x77, 0xBD, 0xA2, 0x55, 0xB3,
      0x2A, 0xAD, 0xBC, 0xE4, 0xBD, 0xA0, 0xB3, 0xF7,
      0xE3, 0x6C, 0x9D, 0xA7 },
    { 0x75, 0x38, 0x8B, 0x16, 0x51, 0x27, 0x76, 0xCC,
      0x5D, 0xBA, 0x5D, 0xA1, 0xFD, 0x89, 0x01, 0x50,
      0xB0, 0xC6, 0x45, 0x5C, 0xB4, 0xF5, 0x8B, 0x19,
      0x52, 0x52, 0x25, 0x25 },
    { 0x20, 0x79, 0x46, 0x55, 0x98, 0x0C, 0x91, 0xD8,
      0xBB, 0xB4, 0xC1, 0xEA, 0x97, 0x61, 0x8A, 0x4B,
      0xF0, 0x3F, 0x42, 0x58, 0x19, 0x48, 0xB2, 0xEE,
      0x4E, 0xE7, 0xAD, 0x67 },

    /*
     * SHA-256 test vectors
     */
    { 0xBA, 0x78, 0x16, 0xBF, 0x8F, 0x01, 0xCF, 0xEA,
      0x41, 0x41, 0x40, 0xDE, 0x5D, 0xAE, 0x22, 0x23,
      0xB0, 0x03, 0x61, 0xA3, 0x96, 0x17, 0x7A, 0x9C,
      0xB4, 0x10, 0xFF, 0x61, 0xF2, 0x00, 0x15, 0xAD },
    { 0x24, 0x8D, 0x6A, 0x61, 0xD2, 0x06, 0x38, 0xB8,
      0xE5, 0xC0, 0x26, 0x93, 0x0C, 0x3E, 0x60, 0x39,
      0xA3, 0x3C, 0xE4, 0x59, 0x64, 0xFF, 0x21, 0x67,
      0xF6, 0xEC, 0xED, 0xD4, 0x19, 0xDB, 0x06, 0xC1 },
    { 0xCD, 0xC7, 0x6E, 0x5C, 0x99, 0x14, 0xFB, 0x92,
      0x81, 0xA1, 0xC7, 0xE2, 0x84, 0xD7, 0x3E, 0x67,
      0xF1, 0x80, 0x9A, 0x48, 0xA4, 0x97, 0x20, 0x0E,
      0x04, 0x6D, 0x39, 0xCC, 0xC7, 0x11, 0x2C, 0xD0 }
};

/*
 * Checkup routine
 */
int mbedtls_sha256_self_test( int verbose, int device_instance )
{
    int i, j, k, buflen, ret = 0;
    unsigned char buf[1024];
    unsigned char sha256sum[32];
    mbedtls_sha256_context ctx;

    mbedtls_sha256_init( &ctx );

    mbedtls_sha256_set_device_instance(&ctx, device_instance);
    
    for( i = 0; i < 6; i++ )
    {
        j = i % 3;
        k = i < 3;

        if( verbose != 0 )
            mbedtls_printf( "  SHA-%d test #%d: ", 256 - k * 32, j + 1 );

        mbedtls_sha256_starts( &ctx, k );

        if( j == 2 )
        {
            memset( buf, 'a', buflen = 1000 );

            for( j = 0; j < 1000; j++ )
                mbedtls_sha256_update( &ctx, buf, buflen );
        }
        else
            mbedtls_sha256_update( &ctx, sha256_test_buf[j],
                                 sha256_test_buflen[j] );

        mbedtls_sha256_finish( &ctx, sha256sum );

        if( memcmp( sha256sum, sha256_test_sum[i], 32 - k * 4 ) != 0 )
        {
            if( verbose != 0 )
                mbedtls_printf( "failed\n" );

            ret = 1;
            goto exit;
        }

        if( verbose != 0 )
            mbedtls_printf( "passed\n" );
    }

    if( verbose != 0 )
        mbedtls_printf( "\n" );

exit:
    mbedtls_sha256_free( &ctx );

    return( ret );
}

#endif /* MBEDTLS_SELF_TEST */

#endif /* #if defined(CRYPTO_COUNT) && (CRYPTO_COUNT > 0) */

#endif /* #if defined(MBEDTLS_SLCL_PLUGINS) */

#endif /* #if defined(MBEDTLS_SHA256_ALT) */

#endif /* #if defined(MBEDTLS_SHA256_C) */
