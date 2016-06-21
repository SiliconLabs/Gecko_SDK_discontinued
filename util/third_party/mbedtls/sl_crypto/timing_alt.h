/**
 * \file timing_alt.h
 *
 * \brief Portable interface to the CPU cycle counter
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
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
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */
#ifndef MBEDTLS_TIMING_ALT_H
#define MBEDTLS_TIMING_ALT_H

#if !defined(MBEDTLS_CONFIG_FILE)
#include "config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_TIMING_ALT)
/* SiliconLabs timing implementation */

#include "em_device.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief          Initialize timing resources.
 *
 */
void mbedtls_timing_init( void );

/**
 * \brief          Stop and free up timing resources.
 *
 */
void mbedtls_timing_free( void );

/**
 * \brief          Return the CPU cycle counter value
 *
 */
static inline unsigned long mbedtls_timing_hardclock( void )
{
    return( DWT->CYCCNT );
}


#ifdef __cplusplus
}
#endif

#endif /* MBEDTLS_TIMING_ALT */

#endif /* MBEDTLS_TIMING_ALT_H */
