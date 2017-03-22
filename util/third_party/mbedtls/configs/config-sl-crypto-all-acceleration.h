/*
 *  Configuration for enabling CRYPTO hardware acceleration in all mbedtls
 *  modules when running on SiliconLabs devices.
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
 
/**
 * @defgroup sl_crypto_config Silicon Labs CRYPTO Hardware Acceleration Configuration
 * @addtogroup sl_crypto_config
 *
 * @brief 
 *  mbed TLS configuration for Silicon Labs CRYPTO hardware acceleration 
 *
 * @details
 *  mbed TLS configuration is composed of settings in this Silicon Labs specific CRYPTO hardware acceleration file located in mbedtls/configs and the mbed TLS configuration file in mbedtls/include/mbedtls/config.h. 
 *  This configuration can be used as a starting point to evaluate hardware acceleration available on Silicon Labs devices. 
 *
 * @{
 */

#ifndef MBEDTLS_CONFIG_SL_CRYPTO_ALL_ACCELERATION_H
#define MBEDTLS_CONFIG_SL_CRYPTO_ALL_ACCELERATION_H

#include "em_device.h"

#if !defined(NO_CRYPTO_ACCELERATION)
/**
 * @name SECTION: Silicon Labs Acceleration settings
 *
 * This section sets Silicon Labs Acceleration settings.
 * @{
   
 */
/**
 * \def MBEDTLS_SLCL_PLUGINS
 *
 * Enable class 2 (slcl_xxx.c) plugins including support for CRYPTO preemption,
 * yield when device busy, DMA I/O mode, and support for classic EFM32
 * devices with AES module, EFM32GG, etc.
 *
 * Module:  sl_crypto/src/slcl_xxx.c
 *
 * Comment/uncomment to disable/enable.
 */
//#define MBEDTLS_SLCL_PLUGINS

/**
 * \def MBEDTLS_CRYPTO_DEVICE_PREEMPTION
 *
 * Enable CRYPTO preemption. The CRYPTO preemption support allows a higher
 * priority thread to preempt a lower priority thread that currently owns the
 * CRYPTO module. The context of the lower priority thread will be saved and
 * restored when the higher priority thread is done and releases the ownership
 * of CRYPTO.
 *
 * Module:  sl_crypto/src/cryptodrv.c
 *          
 * Caller:  sl_crypto/src/slcl_aes.c
 *          sl_crypto/src/slcl_ccm.c
 *          sl_crypto/src/slcl_cmac.c
 *          sl_crypto/src/slcl_ecp.c
 *          sl_crypto/src/slcl_sha1.c
 *          sl_crypto/src/slcl_sha256.c
 *
 * Requires: MBEDTLS_SLCL_PLUGINS
 *           and (CRYPTO_COUNT > 0)
 *
 * Comment/uncomment macros to disable/enable
 */
#if defined(MBEDTLS_SLCL_PLUGINS) && defined(CRYPTO_COUNT) && (CRYPTO_COUNT > 0)
//#define MBEDTLS_CRYPTO_DEVICE_PREEMPTION
#endif

/**
 * \def MBEDTLS_AES_ALT
 *
 * Enable hardware acceleration for the AES block cipher
 *
 * Module:  sl_crypto/src/sl_aes.c
 *          or
 *          sl_crypto/src/slcl_aes.c if MBEDTLS_SLCL_PLUGINS is defined.
 *
 * See MBEDTLS_AES_C for more information.
 */
#define MBEDTLS_AES_ALT

/**
 * \def MBEDTLS_CCM_ALT
 *
 * Enable hardware acceleration for the Counter with CBC-MAC (CCM) mode for
 * 128-bit block cipher.
 *
 * Module:  sl_crypto/src/slcl_ccm.c
 *
 * Requires: MBEDTLS_SLCL_PLUGINS
 *
 * See MBEDTLS_CCM_C for more information.
 */
#if defined(MBEDTLS_SLCL_PLUGINS)
#define MBEDTLS_CCM_ALT
#endif

/**
 * \def MBEDTLS_CMAC_C
 *
 * Enable Cipher-based Message Authentication Code (CMAC) based on AES-128.
 * Note: MBEDTLS_CMAC_ALT is required to enable the API since a pure C
 * implementation of CMAC is not supported.
 *
 * Module:  sl_crypto/src/slcl_cmac.c
 *
 * Requires: MBEDTLS_SLCL_PLUGINS, MBEDTLS_CMAC_ALT and (CRYPTO_COUNT > 0)
 */
#if defined(MBEDTLS_SLCL_PLUGINS) && defined(CRYPTO_COUNT) && (CRYPTO_COUNT > 0)
#define MBEDTLS_CMAC_C
#endif

/**
 * \def MBEDTLS_CMAC_ALT
 *
 * Enable hardware acceleration for the Cipher-based Message Authentication Code
 * (CMAC) based on AES-128.
 *
 * Module:  sl_crypto/src/slcl_cmac.c
 *
 * Requires: MBEDTLS_SLCL_PLUGINS, MBEDTLS_CMAC_C and (CRYPTO_COUNT > 0)
 */
#if defined(MBEDTLS_SLCL_PLUGINS) && defined(MBEDTLS_CMAC_C) && \
  defined(CRYPTO_COUNT) && (CRYPTO_COUNT > 0)
#define MBEDTLS_CMAC_ALT
#endif

/**
 * \def MBEDTLS_DEVICE_YIELD_WHEN_BUSY
 *
 * Enable class 2 plugin slcl_ecp.c to yield the CPU core when CRYPTO device
 * is busy. The slcl_ecp.c plugin accelerates ECC algorithms and needs some
 * rather lengthy CRYPTO instruction sequences. The longest instruction
 * sequences will consume more than 1000 clock cycles and in order to utilize
 * these cycles the slcl_ecp.c plugin can yield the CPU core to other thread(s)
 * while waiting for the CRYPTO hardware to complete the instruction sequence.
 *
 * Module:  sl_crypto/src/slcl_ecp.c
 *
 * Caller:  library/ecp.c
 *          library/ecdh.c
 *          library/ecdsa.c
 *          library/ecjpake.c
 *
 * Requires: MBEDTLS_SLCL_PLUGINS
 *           and (CRYPTO_COUNT > 0)
 *
 * Comment/uncomment macros to disable/enable
 */
#if defined(MBEDTLS_SLCL_PLUGINS) && defined(CRYPTO_COUNT) && (CRYPTO_COUNT > 0)
//#define MBEDTLS_DEVICE_YIELD_WHEN_BUSY
#endif

/**
 * \def MBEDTLS_ECP_DEVICE_ALT
 * \def MBEDTLS_ECP_DEVICE_ALT
 * \def MBEDTLS_ECP_DOUBLE_JAC_ALT
 * \def MBEDTLS_ECP_DEVICE_ADD_MIXED_ALT
 * \def MBEDTLS_ECP_NORMALIZE_JAC_ALT
 * \def MBEDTLS_ECP_NORMALIZE_JAC_MANY_ALT
 * \def MBEDTLS_MPI_MODULAR_DIVISION_ALT
 *
 * Enable hardware acceleration for the elliptic curve over GF(p) library.
 *
 * Module:  sl_crypto/src/sl_ecp.c
 *          or
 *          sl_crypto/src/slcl_ecp.c if MBEDTLS_SLCL_PLUGINS is defined.
 * Caller:  library/ecp.c
 *          library/ecdh.c
 *          library/ecdsa.c
 *          library/ecjpake.c
 *
 * Requires: MBEDTLS_BIGNUM_C, MBEDTLS_ECP_C and at least one
 * MBEDTLS_ECP_DP_XXX_ENABLED and (CRYPTO_COUNT > 0)
 */
#if defined(CRYPTO_COUNT) && (CRYPTO_COUNT > 0)
#define MBEDTLS_ECP_DEVICE_ALT
#define MBEDTLS_ECP_DOUBLE_JAC_ALT
#define MBEDTLS_ECP_DEVICE_ADD_MIXED_ALT
#define MBEDTLS_ECP_NORMALIZE_JAC_ALT
#define MBEDTLS_ECP_NORMALIZE_JAC_MANY_ALT
#define MBEDTLS_MPI_MODULAR_DIVISION_ALT
#endif

/**
 * \def MBEDTLS_ECP_ALT
 * \def MBEDTLS_ECP_GROUP_LOAD_ALT
 *
 * Enable CRYPTO preemption for the elliptic curve over GF(p) library.
 *
 * Module:  sl_crypto/src/slcl_ecp.c
 *          
 * Caller:  library/ecp.c
 *          library/ecdh.c
 *          library/ecdsa.c
 *          library/ecjpake.c
 *
 * Requires: MBEDTLS_BIGNUM_C,
 *           MBEDTLS_ECP_C,
 *           MBEDTLS_ECP_DEVICE_ALT,
 *           MBEDTLS_SLCL_PLUGINS
 *           MBEDTLS_CRYPTO_DEVICE_PREEMPTION
 *           at least one MBEDTLS_ECP_DP_XXX_ENABLED
 *           and (CRYPTO_COUNT > 0)
 *
 * Comment/uncomment macros to disable/enable
 */
#if defined(MBEDTLS_SLCL_PLUGINS) && defined(MBEDTLS_CRYPTO_DEVICE_PREEMPTION) && defined(CRYPTO_COUNT) && (CRYPTO_COUNT > 0)
#define MBEDTLS_ECP_ALT
#define MBEDTLS_ECP_GROUP_LOAD_ALT
#endif

/**
 * \def MBEDTLS_ECP_CRITICAL_SHORT
 *
 * Enable shorter critical regions in the class 2 plugin slcl_ecp.c.
 * By defalt the slcl_ecp.c plugin implements rather lengthy critical regions
 * in order to optimize for speed. However the long critical regions will block
 * higher priority threads from accessing CRYPTO for a substantial amount of
 * time. The lengthiest critical regions consume more than 50000 clock cycles
 * when compiling with IAR High Speed Optimization and ARM GCC -O3.
 * MBEDTLS_ECP_CRITICAL_SHORT will split up the long critical regions into
 * shorter critical regions which should be less than 1500 clock cycles.
 * when compiling with IAR High Speed Optimization and ARM GCC -O3.
 *
 * Module:  sl_crypto/src/slcl_ecp.c
 *
 * Caller:  library/ecp.c
 *          library/ecdh.c
 *          library/ecdsa.c
 *          library/ecjpake.c
 *
 * Requires: MBEDTLS_SLCL_PLUGINS
 *           MBEDTLS_CRYPTO_DEVICE_PREEMPTION
 *           and (CRYPTO_COUNT > 0)
 *
 * Comment/uncomment macros to disable/enable
 */
#if defined(MBEDTLS_SLCL_PLUGINS) && defined(MBEDTLS_CRYPTO_DEVICE_PREEMPTION) && defined(CRYPTO_COUNT) && (CRYPTO_COUNT > 0)
//#define MBEDTLS_ECP_CRITICAL_SHORT
#endif

/**
 * \def MBEDTLS_INCLUDE_IO_MODE_DMA
 *
 * Enable support for moving data to/from CRYPTO with DMA.
 *
 * Module:  sl_crypto/src/slcl_aes.c
 *          sl_crypto/src/slcl_ccm.c
 *
 * Requires: MBEDTLS_SLCL_PLUGINS
 *           MBEDTLS_AES_ALT or MBEDTLS_CCM_ALT
 *           and (CRYPTO_COUNT > 0)
 *
 * Comment/uncomment macros to disable/enable
 */
#if defined(MBEDTLS_SLCL_PLUGINS) && \
  (defined(MBEDTLS_AES_ALT) || defined(MBEDTLS_CCM_ALT)) && \
  defined(CRYPTO_COUNT) && (CRYPTO_COUNT > 0)
//#define MBEDTLS_INCLUDE_IO_MODE_DMA
#endif

/**
 * \def MBEDTLS_SHA1_ALT
 *
 * Enable hardware acceleration for the SHA1 cryptographic hash algorithm.
 *
 * Module:  sl_crypto/src/sl_sha1.c
 *          or
 *          sl_crypto/src/slcl_sha1.c if MBEDTLS_SLCL_PLUGINS is defined.
 * Caller:  library/mbedtls_md.c
 *          library/ssl_cli.c
 *          library/ssl_srv.c
 *          library/ssl_tls.c
 *          library/x509write_crt.c
 *
 * Requires: MBEDTLS_SHA1_C and (CRYPTO_COUNT > 0)
 * See MBEDTLS_SHA1_C for more information.
 */
#if defined(CRYPTO_COUNT) && (CRYPTO_COUNT > 0)
#define MBEDTLS_SHA1_ALT
#endif

/**
 * \def MBEDTLS_SHA256_ALT
 *
 * Enable hardware acceleration for the SHA-224 and SHA-256 cryptographic
 * hash algorithms.
 *
 * Module:  sl_crypto/src/sl_sha256.c
 *          or
 *          sl_crypto/src/slcl_sha256.c if MBEDTLS_SLCL_PLUGINS is defined.
 * Caller:  library/entropy.c
 *          library/mbedtls_md.c
 *          library/ssl_cli.c
 *          library/ssl_srv.c
 *          library/ssl_tls.c
 *
 * Requires: MBEDTLS_SHA256_C and (CRYPTO_COUNT > 0)
 * See MBEDTLS_SHA256_C for more information.
 */
#if defined(CRYPTO_COUNT) && (CRYPTO_COUNT > 0)
#define MBEDTLS_SHA256_ALT
#endif

#endif /* #if !defined(NO_CRYPTO_ACCELERATION) */

/**
 * \def MBEDTLS_TIMING_ALT
 *
 * Enable timing support for SiliconLabs devices including
 * mbedtls_timing_init(), mbedtls_timing_free() and
 * mbedtls_timing_hardclock() using the DWT cycle counter to return
 * timestamps.
 *
 * Requires MBEDTLS_TIMING_C and DWT (defined by ARM CMSIS interface
 * for some Cortex-M cores).
 */
#if defined (DWT)
#define MBEDTLS_TIMING_ALT
#endif

/**
 * \def MBEDTLS_TRNG_C
 *
 * Enable software support for the True Random Number Generator (TRNG)
 * incorporated from Series 1 Configuration 2 devices (EFR32MG12, etc.)
 * from Silicon Labs.
 *
 * Requires TRNG_COUNT>0
 */
#if defined(TRNG_COUNT) && (TRNG_COUNT > 0)
#define MBEDTLS_TRNG_C
#endif

/**
 * \def MBEDTLS_TRNG_IGNORE_NOISE_ALARMS
 *
 * Ignore AIS-31 Noise Alarms and AIS-31 Preliminary Noise Alarms from
 * the TRNG. The TRNG runs an online AIS-31 test that reports a Preliminary
 * Noise Alarm if one AIS-31 test suite fails (including up to 8KiB of
 * random data). If 3 test suites in a row fail, the TRNG will report a Noise
 * Alarm. The probability of a Noise Alarm is non-zero and the application
 * should assess whether the frequency of Noise Alarms is critical.
 * If MBEDTLS_TRNG_IGNORE_NOISE_ALARMS is not defined the TRNG module will
 * return an error code when the mbedtls entropy module requests random data
 * which may prevent the application from collecting random data.
 * The MBEDTLS_TRNG_IGNORE_NOISE_ALARMS does not disable the noise alarms.
 * The purpose of MBEDTLS_TRNG_IGNORE_NOISE_ALARMS is to _not_ return an
 * error code from the mbedtls_trng_poll function which will prevent the
 * entropy accumulator from collecting data.
 *
 * Requires TRNG_COUNT>0 and MBEDTLS_TRNG_C.
 */
#if defined(TRNG_COUNT) && (TRNG_COUNT > 0) && defined(MBEDTLS_TRNG_C)
#define MBEDTLS_TRNG_IGNORE_NOISE_ALARMS
#endif

/**
 * \def MBEDTLS_TRNG_IGNORE_ALL_ALARMS
 *
 * The TRNG is permanently monitoring the generated random numbers using various
 * tests. The test results for the numbers contained inside the TRNG FIFO can be
 * read from the TRNG status register. Normally the TRNG read functions will
 * check these status flags before each read, and reset the TRNG whenever a test
 * fails.
 *
 * When MBEDTLS_TRNG_IGNORE_ALL_ALARMS is used then the TRNG FIFO will be read
 * without checking the status flags before each read. This configuration can be
 * used when an application doesn't care about the TRNG self monitoring tests and
 * just want the fastest and most deterministic way to generate random data.
 *
 * Requires TRNG_COUNT>0 and MBEDTLS_TRNG_C.
 */
#if defined(TRNG_COUNT) && (TRNG_COUNT > 0) && defined(MBEDTLS_TRNG_C)
//#define MBEDTLS_TRNG_IGNORE_ALL_ALARMS
#endif

/**
 * \def MBEDTLS_ENTROPY_ALT
 * \def MBEDTLS_ENTROPY_INIT_ALT
 * \def MBEDTLS_ENTROPY_FREE_ALT
 *
 * Enable the TRNG as an entropy source which can serve the entropy module
 * of mbedtls with random data.
 *
 * Requires TRNG_COUNT>0 and MBEDTLS_TRNG_C.
 */
#if defined(TRNG_COUNT) && (TRNG_COUNT > 0) && defined(MBEDTLS_TRNG_C)
#define MBEDTLS_ENTROPY_ALT
#define MBEDTLS_ENTROPY_INIT_ALT
#define MBEDTLS_ENTROPY_FREE_ALT
#endif

/* Default ECC configuration for Silicon Labs devices: */

/* ECC curves supported by CRYPTO hardware module: */
#define MBEDTLS_ECP_DP_SECP192R1_ENABLED
#define MBEDTLS_ECP_DP_SECP224R1_ENABLED
#define MBEDTLS_ECP_DP_SECP256R1_ENABLED

/* Save RAM by adjusting to our exact needs */
#define MBEDTLS_ECP_MAX_BITS   256
#ifndef MBEDTLS_MPI_MAX_SIZE
#define MBEDTLS_MPI_MAX_SIZE    32 // 384 bits is 48 bytes
#endif

/* 
   Set MBEDTLS_ECP_WINDOW_SIZE to configure
   ECC point multiplication window size, see ecp.h:
   2 = Save RAM at the expense of speed
   3 = Improve speed at the expense of RAM
   4 = Optimize speed at the expense of RAM
*/
#define MBEDTLS_ECP_WINDOW_SIZE        3
#define MBEDTLS_ECP_FIXED_POINT_OPTIM  0

/* Significant speed benefit at the expense of some ROM */
#define MBEDTLS_ECP_NIST_OPTIM

/* Include the default mbed TLS config file */
#include "mbedtls/config.h"

#undef MBEDTLS_FS_IO
#define MBEDTLS_NO_PLATFORM_ENTROPY
/* Hardware entropy source is not yet supported. Uncomment this macro to 
   provide your own implementation of an entropy collector. */
//#define MBEDTLS_ENTROPY_HARDWARE_ALT

/* Exclude and/or change default config here. E.g.: */
//#undef MBEDTLS_ECP_DP_SECP384R1_ENABLED
//#undef MBEDTLS_ECP_DP_SECP521R1_ENABLED
//#undef MBEDTLS_ECP_DP_BP384R1_ENABLED
//#undef MBEDTLS_ECP_DP_BP512R1_ENABLED
//#undef MBEDTLS_SHA512_C

#include "mbedtls/check_config.h"

/** @} (end section sl_crypto_config) */
/** @} (end addtogroup sl_crypto_config) */

#endif /* MBEDTLS_CONFIG_SL_CRYPTO_ALL_ACCELERATION_H */
