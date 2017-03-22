/*
 *  FIPS-180-1 compliant SHA-1 implementation
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
 * mbedtls/libary/sha1.c using the CRYPTO hardware accelerator incorporated
 * in MCU devices from Silicon Laboratories.
 */
/*
 *  The SHA-1 standard was published by NIST in 1993.
 *
 *  http://www.itl.nist.gov/fipspubs/fip180-1.htm
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_SHA1_C)

#if defined(MBEDTLS_SHA1_ALT)

#if defined(MBEDTLS_SLCL_PLUGINS)

#include "em_device.h"

#if defined(CRYPTO_COUNT) && (CRYPTO_COUNT > 0)

#include "mbedtls/sha1.h"
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
{                                                       \
    (b)[(i)    ] = (unsigned char) ( (n) >> 24 );       \
    (b)[(i) + 1] = (unsigned char) ( (n) >> 16 );       \
    (b)[(i) + 2] = (unsigned char) ( (n) >>  8 );       \
    (b)[(i) + 3] = (unsigned char) ( (n)       );       \
}
#endif

void mbedtls_sha1_init( mbedtls_sha1_context *ctx )
{
    memset( ctx, 0, sizeof( mbedtls_sha1_context ) );

    /* Set device instance and lock wait ticks to 0 by default. */
    mbedtls_sha1_set_device_instance( ctx, 0 );
    mbedtls_sha1_set_device_lock_wait_ticks( ctx, 0 );
}

void mbedtls_sha1_free( mbedtls_sha1_context *ctx )
{
    if( ctx == NULL )
        return;

    mbedtls_zeroize( ctx, sizeof( mbedtls_sha1_context ) );
}

/*
 * Set the device instance of an SHA context.
 */
int mbedtls_sha1_set_device_instance(mbedtls_sha1_context *ctx,
                                     unsigned int          devno)
{
#if defined(CRYPTO_COUNT) && (CRYPTO_COUNT > 0)
    if (devno >= CRYPTO_COUNT)
        return( MBEDTLS_ERR_SHA1_BAD_INPUT );
  
    return cryptodrvSetDeviceInstance( &ctx->cryptodrv_ctx, devno );
#endif /* #if defined(CRYPTO_COUNT) && (CRYPTO_COUNT > 0) */
}

/*
 *   Set the number of ticks to wait for the device lock.
 */
int mbedtls_sha1_set_device_lock_wait_ticks(mbedtls_sha1_context *ctx,
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

void mbedtls_sha1_clone( mbedtls_sha1_context *dst,
                         const mbedtls_sha1_context *src )
{
    (void) dst;
    (void) src;
    
    /* Cloning a SHA256 CRYPTODRV context is not supported. */
    while(1);
}

/*
 * SHA-1 context setup
 */
int mbedtls_sha1_starts( mbedtls_sha1_context *ctx )
{
    CRYPTODRV_Context_t* cryptodrv_ctx = &ctx->cryptodrv_ctx;
    CRYPTO_TypeDef* crypto = cryptodrv_ctx->device->crypto;
    uint32_t init_state[8];
    int ret;

    /* Request CRYPTO usage. */
    ret = CRYPTODRV_Arbitrate(cryptodrv_ctx);
    if (0 != ret)
    {
        return ret;
    }
    
    /* Enter critial crypto region in order to initialize crypto for
       SHA operation. */
    ret = CRYPTODRV_EnterCriticalRegion(cryptodrv_ctx);
    EFM_ASSERT(0 == ret); /* Assert critical region entry is ok. */
    
    /* Setup CRYPTO for SHA-1 operation: */
    crypto->CTRL = CRYPTO_CTRL_SHA_SHA1;
    crypto->WAC  = 0;
    crypto->IEN  = 0;

    /* Set result width of MADD32 operation. */
    CRYPTO_ResultWidthSet(crypto, cryptoResult256Bits);
  
    /* Clear sequence control registers */
    crypto->SEQCTRL  = 0;
    crypto->SEQCTRLB = 0;
    
    /* Setup the initial sha digest state */
    init_state[0] = 0x67452301;
    init_state[1] = 0xEFCDAB89;
    init_state[2] = 0x98BADCFE;
    init_state[3] = 0x10325476;
    init_state[4] = 0xC3D2E1F0;
    init_state[5] = 0x0;
    init_state[6] = 0x0;
    init_state[7] = 0x0;

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
    
    return ( ret );
}

void mbedtls_sha1_process( mbedtls_sha1_context *ctx, const unsigned char data[64] )
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
 * SHA-1 process buffer
 */
void mbedtls_sha1_update( mbedtls_sha1_context *ctx, const unsigned char *input, size_t ilen )
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
        mbedtls_sha1_process( ctx, ctx->buffer );
        input += fill;
        ilen  -= fill;
        left = 0;
    }

    while( ilen >= 64 )
    {
        mbedtls_sha1_process( ctx, input );
        input += 64;
        ilen  -= 64;
    }
    
    if( ilen > 0 )
        memcpy( (void *) (ctx->buffer + left), input, ilen );
}

static const unsigned char sha1_padding[64] =
{
 0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*
 * SHA-1 final digest
 */
void mbedtls_sha1_finish( mbedtls_sha1_context *ctx, unsigned char output[20] )
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

    mbedtls_sha1_update( ctx, sha1_padding, padn );
    mbedtls_sha1_update( ctx, msglen, 8 );

    /* Enter critical CRYPTO region in order to read final SHA digest/state. */
    ret = CRYPTODRV_EnterCriticalRegion( cryptodrv_ctx );
    EFM_ASSERT(0 == ret); /* Assert critical region entry is ok. */
    
    /* Read the digest from crypto (big endian). */
    ((uint32_t*)output)[0] = crypto->DDATA0BIG;
    ((uint32_t*)output)[1] = crypto->DDATA0BIG;
    ((uint32_t*)output)[2] = crypto->DDATA0BIG;
    ((uint32_t*)output)[3] = crypto->DDATA0BIG;
    ((uint32_t*)output)[4] = crypto->DDATA0BIG;
    {
      /* Read 3 remaining 32-bit words from DDATA0BIG (shift register). */
      volatile uint32_t temp;
      temp = crypto->DDATA0BIG;
      temp = crypto->DDATA0BIG;
      temp = crypto->DDATA0BIG;
      (void) temp;
    }
    
    ret = CRYPTODRV_ExitCriticalRegion( cryptodrv_ctx );
    EFM_ASSERT(0 == ret); /* Assert critical region exit is ok. */

    /* Finally release CRYPTO since SHA operation has completed. */
    ret = CRYPTODRV_Release( cryptodrv_ctx );
    EFM_ASSERT(0 == ret); /* Assert crypto release is ok. */
}

/*
 * output = SHA-1( input buffer )
 */
int mbedtls_sha1( const unsigned char *input, size_t ilen, unsigned char output[20] )
{
    mbedtls_sha1_context ctx;
    int ret;
    
    mbedtls_sha1_init( &ctx );
    
    ret = mbedtls_sha1_starts( &ctx );
    if (ret != 0)
      return ret;
    
    mbedtls_sha1_update( &ctx, input, ilen );
    mbedtls_sha1_finish( &ctx, output );
    mbedtls_sha1_free( &ctx );

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
 * FIPS-180-1 test vectors
 */
static const unsigned char sha1_test_buf[3][57] =
{
    { "abc" },
    { "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq" },
    { "" }
};

static const int sha1_test_buflen[3] =
{
    3, 56, 1000
};

static const unsigned char sha1_test_sum[3][20] =
{
    { 0xA9, 0x99, 0x3E, 0x36, 0x47, 0x06, 0x81, 0x6A, 0xBA, 0x3E,
      0x25, 0x71, 0x78, 0x50, 0xC2, 0x6C, 0x9C, 0xD0, 0xD8, 0x9D },
    { 0x84, 0x98, 0x3E, 0x44, 0x1C, 0x3B, 0xD2, 0x6E, 0xBA, 0xAE,
      0x4A, 0xA1, 0xF9, 0x51, 0x29, 0xE5, 0xE5, 0x46, 0x70, 0xF1 },
    { 0x34, 0xAA, 0x97, 0x3C, 0xD4, 0xC4, 0xDA, 0xA4, 0xF6, 0x1E,
      0xEB, 0x2B, 0xDB, 0xAD, 0x27, 0x31, 0x65, 0x34, 0x01, 0x6F }
};

/*
 * Checkup routine
 */
int mbedtls_sha1_self_test( int verbose, int device_instance )
{
    int i, j, buflen, ret = 0;
    unsigned char buf[1024];
    unsigned char sha1sum[20];
    mbedtls_sha1_context ctx;

    mbedtls_sha1_init( &ctx );

    mbedtls_sha1_set_device_instance(&ctx, device_instance);

    /*
     * SHA-1
     */
    for( i = 0; i < 3; i++ )
    {
        if( verbose != 0 )
            mbedtls_printf( "  SHA-1 test #%d: ", i + 1 );

        mbedtls_sha1_starts( &ctx );
        
        if( i == 2 )
        {
            memset( buf, 'a', buflen = 1000 );

            for( j = 0; j < 1000; j++ )
                mbedtls_sha1_update( &ctx, buf, buflen );
        }
        else
            mbedtls_sha1_update( &ctx, sha1_test_buf[i],
                               sha1_test_buflen[i] );

        mbedtls_sha1_finish( &ctx, sha1sum );

        if( memcmp( sha1sum, sha1_test_sum[i], 20 ) != 0 )
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
    mbedtls_sha1_free( &ctx );

    return( ret );
}

#endif /* MBEDTLS_SELF_TEST */

#endif /* #if defined(CRYPTO_COUNT) && (CRYPTO_COUNT > 0) */

#endif /* #if defined(MBEDTLS_SLCL_PLUGINS) */

#endif /* MBEDTLS_SHA1_ALT */

#endif /* MBEDTLS_SHA1_C */
