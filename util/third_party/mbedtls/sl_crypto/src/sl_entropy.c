/*
 *  Entropy accumulator implementation using Silicon Labs hardware.
 *
 *  Copyright (C) 2016 Silicon Labs, http://www.silabs.com
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
 * This file implements an mbedtls entropy interface using the entropy
 * source(s) of the Silicon Labs device(s).
 *
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_ENTROPY_ALT)

#if defined(MBEDTLS_TRNG_C)

#include "mbedtls/entropy.h"
#include "mbedtls/entropy_poll.h"
#include <string.h>

/* Implementation that should never be optimized out by the compiler */
static void mbedtls_zeroize( void *v, size_t n ) {
    volatile unsigned char *p = v; while( n-- ) *p++ = 0;
}

#if defined(MBEDTLS_ENTROPY_INIT_ALT)

void mbedtls_entropy_init( mbedtls_entropy_context *ctx )
{
    memset( ctx, 0, sizeof(mbedtls_entropy_context) );

#if defined(MBEDTLS_THREADING_C)
    mbedtls_mutex_init( &ctx->mutex );
#endif

#if defined(MBEDTLS_TRNG_C)
    mbedtls_trng_init( &ctx->trng_ctx );
#endif
    
#if defined(MBEDTLS_ENTROPY_SHA512_ACCUMULATOR)
    mbedtls_sha512_starts( &ctx->accumulator, 0 );
#else
    mbedtls_sha256_init( &ctx->accumulator );
    mbedtls_sha256_starts( &ctx->accumulator, 0 );
#endif
#if defined(MBEDTLS_HAVEGE_C)
    mbedtls_havege_init( &ctx->havege_data );
#endif

#if !defined(MBEDTLS_NO_DEFAULT_ENTROPY_SOURCES)
#if !defined(MBEDTLS_NO_PLATFORM_ENTROPY)
    mbedtls_entropy_add_source( ctx, mbedtls_platform_entropy_poll, NULL,
                                MBEDTLS_ENTROPY_MIN_PLATFORM,
                                MBEDTLS_ENTROPY_SOURCE_STRONG );
#endif
#if defined(MBEDTLS_TIMING_C)
    mbedtls_entropy_add_source( ctx, mbedtls_hardclock_poll, NULL,
                                MBEDTLS_ENTROPY_MIN_HARDCLOCK,
                                MBEDTLS_ENTROPY_SOURCE_WEAK );
#endif
#if defined(MBEDTLS_HAVEGE_C)
    mbedtls_entropy_add_source( ctx, mbedtls_havege_poll, &ctx->havege_data,
                                MBEDTLS_ENTROPY_MIN_HAVEGE,
                                MBEDTLS_ENTROPY_SOURCE_STRONG );
#endif
#if defined(MBEDTLS_ENTROPY_HARDWARE_ALT)
#if defined(MBEDTLS_TRNG_C)
    mbedtls_entropy_add_source( ctx, mbedtls_hardware_poll, &ctx->trng_ctx,
                                MBEDTLS_ENTROPY_MIN_HARDWARE,
                                MBEDTLS_ENTROPY_SOURCE_STRONG );
#else
    mbedtls_entropy_add_source( ctx, mbedtls_hardware_poll, NULL,
                                MBEDTLS_ENTROPY_MIN_HARDWARE,
                                MBEDTLS_ENTROPY_SOURCE_STRONG );
#endif
#endif
#endif /* MBEDTLS_NO_DEFAULT_ENTROPY_SOURCES */
}

#endif /* MBEDTLS_ENTROPY_INIT_ALT */

#if defined(MBEDTLS_ENTROPY_FREE_ALT)

void mbedtls_entropy_free( mbedtls_entropy_context *ctx )
{
#if defined(MBEDTLS_TRNG_C)
    mbedtls_trng_free( &ctx->trng_ctx );
#endif
#if defined(MBEDTLS_HAVEGE_C)
    mbedtls_havege_free( &ctx->havege_data );
#endif
#if defined(MBEDTLS_THREADING_C)
    mbedtls_mutex_free( &ctx->mutex );
#endif
    mbedtls_zeroize( ctx, sizeof( mbedtls_entropy_context ) );
}

#endif /* MBEDTLS_ENTROPY_FREE_ALT */

#if defined(MBEDTLS_ENTROPY_HARDWARE_ALT)

int mbedtls_hardware_poll( void *data,
                           unsigned char *output, size_t len, size_t *olen )
{
#if defined(MBEDTLS_TRNG_C)
    return mbedtls_trng_poll( (mbedtls_trng_context *)data,
                              output, len, olen );
#else
    *olen = 0;
    return( MBEDTLS_ERR_ENTROPY_SOURCE_FAILED );
#endif
}

#endif /* MBEDTLS_ENTROPY_HARDWARE_ALT */

#endif /* MBEDTLS_TRNG_C */

#endif /* MBEDTLS_ENTROPY_ALT */
