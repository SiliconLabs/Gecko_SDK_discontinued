/**
 * \file sl_crypto.h
 *
 * \brief Silicon Labs specific mbedTLS defintitions
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
#ifndef MBEDTLS_SL_CRYPTO_H
#define MBEDTLS_SL_CRYPTO_H

#include "em_device.h"

#if ( defined(CRYPTO_COUNT) && (CRYPTO_COUNT > 0) ) ||  \
  ( defined(AES_COUNT) && (AES_COUNT > 0) )

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined( MBEDTLS_SLCL_PLUGINS )

#if defined(CRYPTO_COUNT) && (CRYPTO_COUNT > 0)
#include "cryptodrv.h"
#endif

#include "mbedtls_ecode.h"
#include <stdbool.h>

/** Error codes */

#if defined(CRYPTO_COUNT) && (CRYPTO_COUNT > 0)
#define MBEDTLS_ERR_ABORTED      ((int)MBEDTLS_ECODE_CRYPTODRV_OPERATION_ABORTED) /**< Operation was aborted. */
#define MBEDTLS_ERR_DEVICE_BUSY  ((int)MBEDTLS_ECODE_CRYPTODRV_BUSY)              /**< CRYPTO/AES device busy */
#endif
#endif /* #if defined( MBEDTLS_SLCL_PLUGINS ) */

#ifdef __cplusplus
extern "C" {
#endif

/** Enum defines which data I/O mode to use for moving data to/from the
    AES/CRYPTO hardware module. */
typedef enum
{
  MBEDTLS_DEVICE_IO_MODE_CORE, /*!< Core CPU moves data to/from the data
                                 registers. */
  
#if defined( MBEDTLS_INCLUDE_IO_MODE_DMA )
  MBEDTLS_DEVICE_IO_MODE_DMA   /*!< DMA moves data to/from the CRYPTO data
                                 registers. */
#endif
} mbedtls_device_io_mode;

#if defined( MBEDTLS_INCLUDE_IO_MODE_DMA )
/** DMA I/O mode specific configuration structure. */
typedef struct
{
  unsigned int dma_ch_in;  /*!< DMA input channel. Allocated by mbedtls. */
  unsigned int dma_ch_out; /*!< DMA output channel. Allocated by mbedtls. */
} mbedtls_device_dma_config;
#endif

/** Data I/O mode specific configuration structure. */
typedef union
{
#if defined( MBEDTLS_INCLUDE_IO_MODE_DMA )
  mbedtls_device_dma_config  dma_config;  /*!< DMA specific config */
#else
  void* placeholder;                      /*!< Dummy place holder to
                                           keep compiler happy when
                                           DMA is disabled. */
#endif
} mbedtls_device_io_mode_specific;

#ifdef __cplusplus
}
#endif

#endif /* #if ( defined(CRYPTO_COUNT) && (CRYPTO_COUNT > 0) ) ||  \
          ( defined(AES_COUNT) && (AES_COUNT > 0) ) */

#endif /* #ifndef MBEDTLS_SL_CRYPTO_H */
