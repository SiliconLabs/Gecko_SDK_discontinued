/**
 * \file aesdrv.h
 *
 * \brief Definitions for AES based ciphers with CRYPTO hw acceleration 
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

#ifndef __SILICON_LABS_AESDRV_H
#define __SILICON_LABS_AESDRV_H

#if !defined(MBEDTLS_CONFIG_FILE)
#include "config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include "em_device.h"
#if ( defined(CRYPTO_COUNT) && (CRYPTO_COUNT > 0) ) || \
  ( defined(AES_COUNT) && (AES_COUNT > 0) )

#include "mbedtls_ecode.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 ******************************* ERROR CODES  **********************************
 ******************************************************************************/

#define MBEDTLS_ECODE_AESDRV_NOT_SUPPORTED         ((int)MBEDTLS_ECODE_AESDRV_BASE | 0x1)
#define MBEDTLS_ECODE_AESDRV_AUTHENTICATION_FAILED ((int)MBEDTLS_ECODE_AESDRV_BASE | 0x2)
#define MBEDTLS_ECODE_AESDRV_OUT_OF_RESOURCES      ((int)MBEDTLS_ECODE_AESDRV_BASE | 0x3)
#define MBEDTLS_ECODE_AESDRV_INVALID_PARAM         ((int)MBEDTLS_ECODE_AESDRV_BASE | 0x4)

/*******************************************************************************
 *******************************   TYPEDEFS   **********************************
 ******************************************************************************/

/** Enum defines which data I/O mode to use for moving data to/from the
    AES/CRYPTO hardware module. */
typedef enum
{
  aesdrvIoModeCore,  /** Core CPU moves data to/from the data registers. */
  aesdrvIoModeDma    /** DMA moves data to/from the CRYPTO data registers. */
} AESDRV_IoMode_t;

/** DMA I/O mode specific configuration structure. */
typedef struct
{
  unsigned int dmaChIn;  /**< DMA input channel. Allocated by AESDRV. */
  unsigned int dmaChOut; /**< DMA output channel. Allocated by AESDRV. */
} AESDRV_DmaConfig_t;

/** Data I/O mode specific configuration structure. */
typedef union
{
  AESDRV_DmaConfig_t  dmaConfig;
} AESDRV_IoModeSpecific_t;

/** Prototype of counter callback function provided by user. */
typedef void (*AESDRV_CtrCallback_t)(uint8_t *ctr);

#if ( defined(CRYPTO_COUNT) && (CRYPTO_COUNT > 0) )
#include "cryptodrv.h"

/* Main AESDRV context structure. */
typedef struct
{
  CRYPTODRV_Context_t     cryptodrvContext; /**< CRYPTO driver context */
  AESDRV_IoMode_t         ioMode;           /**< Data I/O mode. */
  AESDRV_IoModeSpecific_t ioModeSpecific;   /**< Data I/O mode specific
                                               config. */
  bool                    authTagOptimize;  /**< Enable/disable optimized 
                                               handling of authentication
                                               tag in CCM/GCM. Tag
                                               optimization expects tag size
                                               0,4,8,12 or 16 bytes.*/
} AESDRV_Context_t;
  
#elif ( defined(AES_COUNT) && (AES_COUNT > 0) )

typedef struct
{
  uint32_t                ccmCounter[4];    /**< CCM counter */
} AESDRV_Context_t;
  
#endif

#ifdef __cplusplus
}
#endif

#endif /* #if ( defined(CRYPTO_COUNT) && (CRYPTO_COUNT > 0) ) || \
          ( defined(AES_COUNT) && (AES_COUNT > 0) ) */

#endif /* __SILICON_LABS_AESDRV_H */
