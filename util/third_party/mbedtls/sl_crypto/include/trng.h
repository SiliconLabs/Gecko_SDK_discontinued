/**
 *  \file trng.h
 *
 *  \brief True Random Number Generator (TRNG) driver for Silicon Labs devices
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
#ifndef MBEDTLS_TRNG_H
#define MBEDTLS_TRNG_H

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_TRNG_C)

/***************************************************************************//**
 * \addtogroup rng_module
 * \{
 ******************************************************************************/

/***************************************************************************//**
 * \addtogroup sl_crypto_trng Silicon Labs True Random Number Generator Plugin
 * \brief True Random Number Generator (TRNG) driver for Silicon Labs devices.
 *
 * \details The EFR32xG12 and EFM32xG12 and newer Silicon Labs devices contains
 * a True Random Number Generator (TRNG) peripheral. The TRNG is a
 * non-deterministic random number generator based on a full hardware solution.
 * The TRNG contains a 64 x 32-bit FIFO for reading out random numbers.
 *
 * The samples from entropy source within the TRNG are monitored permanently by
 * 4 built in tests that detect issues with the noise source. The tests are
 * specified in NIST-800-90B and AIS31. The tests that are always checking the
 * entropy source are "Repetition Count Test", "Adaptive Proportion Test
 * (64-sample window)", "Adaptive Proportion Test (4096-sample window)" and
 * the "AIS31 Online Test".
 *
 * In addition the TRNG has options for running startup tests. When these tests
 * are enabled the TRNG FIFO will not contains any data before all the startup
 * tests have passed. There are 4 TRNG startup tests, 3 of the tests are
 * specified in NIST-800-90B. These are the "Repetition Count Test", "Adaptive
 * Proportion Test (64-sample window)" and "Adaptive Proportion Test
 * (4096-sample window)". The last startup test is the AIS31 startup test. By
 * default when using this driver all the startup tests are enabled.

 * The TRNG module implements an entropy source plugin module for mbed TLS that can 
 * be used in applications needing random numbers or indirectly using mbed TLS 
 * modules that depend on the random number generation interfaces of mbed TLS. 
 * The define @ref MBEDTLS_TRNG_C will compile the TRNG module. The TRNG is enbabled as 
 * an mbed TLS entropy source by defining @ref MBEDTLS_ENTROPY_ALT, 
 * @ref MBEDTLS_ENTROPY_INIT_ALT and @ref MBEDTLS_ENTROPY_FREE_ALT. 
 * The TRNG functions are declared in the 'mbedtls/sl_crypto/include/trng.h' file.
 *
 * \{
 ******************************************************************************/

#include "em_device.h"
#include "mbedtls_ecode.h"
#include <stddef.h>

/* TRNG specific error codes: */

/** Conditioning test failed. */
#define MBEDTLS_ERR_TRNG_CONDITIONING_TEST_FAILED             ((int)MBEDTLS_ERR_TRNG_BASE | 0x00000001)

/** No data received in the TRNG FIFO. */
#define MBEDTLS_ERR_TRNG_NO_DATA                              ((int)MBEDTLS_ERR_TRNG_BASE | 0x00000002)

/** Repetition Count test failed. The repetition count test fails when the
 *  TRNG detects that the output become "stuck" on a single value for a long
 *  period of time. The repetition count test is described in NIST-800-90B.
 *
 *  If an application detects this error then the TRNG should be reset. The
 *  repetition count test is always enabled. */
#define MBEDTLS_ERR_TRNG_REPETITION_COUNT_TEST_FAILED         ((int)MBEDTLS_ERR_TRNG_BASE | 0x00000003)

/** Adaptive Proportion test over 64 samples failed. The adaptive proportion
 *  test is designed to detect a large loss of entropy that might occur as a
 *  result of some physical failure or environmental change affecting the
 *  TRNG.
 *
 *  The test will fail when a 2 bit sample from the TRNG is repeated an
 *  unusual amount of times within a window of 64 bits. The adaptive
 *  proportion test is further described in NIST-800-90B.
 *
 *  If an application detects this error then the TRNG should be reset. The
 *  adaptive proportion test over 64 samples is always enabled. */
#define MBEDTLS_ERR_TRNG_ADAPTIVE_PROPORTION_TEST_64_FAILED   ((int)MBEDTLS_ERR_TRNG_BASE | 0x00000004)

/** Adaptive Proportion test over 4096 samples failed. The adaptive proportion
 *  test is designed to detect a large loss of entropy that might occur as a
 *  result of some physical failure or environmental change affecting the
 *  TRNG.
 *
 *  The test will fail when a 2 bit sample from the TRNG is repeated an
 *  unusual amount of times within a window of 4096 bits. The adaptive
 *  proportion test is further described in NIST-800-90B.
 *
 *  If an application detects this error then the TRNG should be reset. The
 *  adaptive proportion test over 4096 samples is always enabled. */
#define MBEDTLS_ERR_TRNG_ADAPTIVE_PROPORTION_TEST_4096_FAILED ((int)MBEDTLS_ERR_TRNG_BASE | 0x00000005)

/** AIS31 test noise alarm. The AIS31 test is designed to monitor and verify
 *  the statistical distribution of the random numbers from the TRNG. The test
 *  performs 512 consecutive 128 bit X^2 calculations with 4 bit words. The
 *  details of the AIS31 test can be found in the AIS31 specification.
 *
 *  The test will fail when an unusual statistical distribution of the TRNG
 *  output is found.
 *
 *  If an application detects this error then the TRNG should be reset. The
 *  AIS31 test is always enabled. */
#define MBEDTLS_ERR_TRNG_NOISE_ALARM                          ((int)MBEDTLS_ERR_TRNG_BASE | 0x00000006)

/** AIS31 test Preliminary Noise alarm. The preliminary noise alarms generated
 *  from the same AIS31 test that generates \ref MBEDTLS_ERR_TRNG_NOISE_ALARM.
 *  The difference between a preliminary noise alarm and a noise alarm is the
 *  severity and the expected frequency. A preliminary noise alarm will happen
 *  more frequently than a noise alarm, and a preliminary noise alarm is not
 *  considered critical. The preliminary noise alarm is not uncommon and should
 *  be expected from time to time when reading data from the TRNG.
 *
 *  If an application detects a preliminary noise alarm then the recommended
 *  action is to flush the TRNG FIFO, or reset the TRNG. */
#define MBEDTLS_ERR_TRNG_PRELIMINARY_NOISE_ALARM              ((int)MBEDTLS_ERR_TRNG_BASE | 0x00000007)

/**
 * \brief          TRNG context structure
 */
typedef struct
{
    TRNG_TypeDef  *trng;      /*!<  TRNG register block pointer */
}
mbedtls_trng_context;

/**
 * \brief          Initialize TRNG context
 *
 * \details        This function will enable the TRNG0 peripheral by starting
 *                 the TRNG0 clock, initializing the control register, perform
 *                 soft reset and wait until data is available in the FIFO.
 *                 Normally the application will need only one TRNG context.
 *                 If more than one TRNG contexts are instantiated the user
 *                 should be aware that mbedtls_trng_free will disable the TRNG0
 *                 by stopping the clock, requiring mbedtls_trng_init to be
 *                 called again before any TRNG API calls can be used.
 *                 When the TRNG module is used as a standard mbedtls entropy
 *                 source the context passed to
 *                 void mbedtls_entropy_init( mbedtls_entropy_context *ctx )
 *                 will include one TRNG context and initialize it. The
 *                 application should not want to keep more than one
 *                 mbedtls_entropy_context structures as it will consume
 *                 memory.
 *
 * \param ctx      TRNG context to be initialized
 */
void mbedtls_trng_init( mbedtls_trng_context *ctx );

/**
 * \brief          Free TRNG context
 *
 * \details        This function will disable the TRNG0 peripheral by stopping
 *                 the TRNG0 clock. If more than one TRNG contexts are
 *                 instantiated the user should be aware that one call to
 *                 mbedtls_trng_free with any of the trng contexts will disable
 *                 the TRNG0 and effectively disable all other calls in the
 *                 TRNG API, except mbedtls_trng_init which will enable the
 *                 TRNG0 again. Normally the application will need only one
 *                 TRNG context.
 *
 * \param ctx      TRNG context to be released
 */
void mbedtls_trng_free( mbedtls_trng_context *ctx );

/**
 * \brief          Check the TRNG conditioning function
 *
 * \param ctx      TRNG context
 * \param key      128-bit AES key
 *  
 * \return
 *   0 if success. Error code if failure.
 */
int mbedtls_trng_set_key( mbedtls_trng_context *ctx, const unsigned char *key );

/**
 * \brief          Check the TRNG conditioning function
 *
 * \param ctx      TRNG context
 *  
 * \return
 *   0 if success. \ref MBEDTLS_ERR_TRNG_CONDITIONING_TEST_FAILED on failure.
 */
int mbedtls_trng_check_conditioning( mbedtls_trng_context *ctx );

/**
 * \brief          Check the TRNG entropy source is producing random data
 *
 * \param ctx      TRNG context
 *  
 * \return
 *   0 if success. Error code if failure.
 */
int mbedtls_trng_check_entropy( mbedtls_trng_context *ctx );

/**
 * \brief           Poll for entropy data
 *
 * \details         This function will read available random data from the TRNG
 *                  FIFO and place it into the output buffer. The len parameter
 *                  tells this function the maximum number of bytes to read.
 *
 *                  Note that the number of bytes read from the TRNG might differ
 *                  from the number of bytes requested. If any alarms are signaled
 *                  or the TRNG FIFO is empty then this function will return early.
 *
 *                  The return value should be used to see if the operation was
 *                  successful of if an alarm was encountered while reading the
 *                  FIFO. The content of the olen parameter can be used to check
 *                  how many bytes were actually read.
 *
 * \param ctx       TRNG context
 * \param output    Buffer to fill with data from the TRNG
 * \param len       Maximum number of bytes to fill in output buffer.
 * \param olen      The actual amount of bytes put into the buffer (Can be 0)
 *
 * \return          \li 0 if no critical failures occurred,
 *                  \li MBEDTLS_ERR_TRNG_PRELIMINARY_NOISE_ALARM if a AIS31
 *                  preliminary noise alarm was detected while reading the FIFO,
 *                  \li MBEDTLS_ERR_TRNG_NOISE_ALARM if an AIS31 noise alarm
 *                  was detected.
 *                  \li MBEDTLS_ERR_TRNG_REPETITION_COUNT_TEST_FAILED if the
 *                  repetition count test failed while reading the FIFO.
 *                  \li MBEDTLS_ERR_TRNG_ADAPTIVE_PROPORTION_TEST_64_FAILED if the
 *                  adaptive proportion test over 64 samples failed while reading
 *                  the FIFO.
 *                  \li MBEDTLS_ERR_TRNG_ADAPTIVE_PROPORTION_TEST_4096_FAILED if
 *                  the adaptive proportion test over 4096 samples failed while
 *                  reading from the FIFO.
 */
int mbedtls_trng_poll( mbedtls_trng_context *ctx,
                       unsigned char *output, size_t len, size_t *olen );

/**
 * \brief           Execute TRNG soft reset
 *
 * \details         This function performs a TRNG soft reset. The TRNG soft
 *                  reset can be used to clear error conditions such as Noise
 *                  Alarms, etc.
 *
 * \param ctx       TRNG context
 */
void mbedtls_trng_soft_reset( mbedtls_trng_context *ctx );

#endif /* MBEDTLS_TRNG_C */

/** \} (end addtogroup sl_crypto) */
/** \} (end addtogroup sl_crypto_trng) */

#endif /* MBEDTLS_TRNG_H */
