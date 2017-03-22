/*
 * \file slcl_device_internal.h
 *
 * \brief Silicon Labs CRYPTO/AES device internal interface
 *
 * \details This file defines the CRYPTO/AES device driver interface
 *          including per device contexts, preemption contexts, and
 *          user callbacks for events associated with device preemption
 *          and device busy.
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
#ifndef MBEDTLS_SLCL_DEVICE_INTERNAL_H
#define MBEDTLS_SLCL_DEVICE_INTERNAL_H

#if !defined(MBEDTLS_CONFIG_FILE)
#include "config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include "em_device.h"

#if ( defined(CRYPTO_COUNT) && (CRYPTO_COUNT > 0) ) || \
          ( defined(AES_COUNT) && (AES_COUNT > 0) )

#include "slcl_device.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *******************************  FUNCTIONS   **********************************
 ******************************************************************************/

/**
 * \brief
 *  Initialize CRYPTO/AES device specifics for given device instance
 *
 * \param devno
 *  CRYPTO or AES device instance number.
 *
 * \return
 *   0 if OK, or MBEDTLS_ERR_DEVICE_NOT_SUPPORTED if device number is invalid.
 ******************************************************************************/
int mbedtls_device_specific_init (unsigned int devno);
  
/**
 * \brief
 *  Deinitialize CRYPTO/AES device specifics for given device instance
 *
 * \param devno
 *  CRYPTO or AES device instance number.
 *
 * \return
 *   0 if OK, or MBEDTLS_ERR_DEVICE_NOT_SUPPORTED if device number is invalid.
 ******************************************************************************/
int mbedtls_device_specific_free (unsigned int devno);
  
#ifdef __cplusplus
}
#endif

#endif /* #if ( defined(CRYPTO_COUNT) && (CRYPTO_COUNT > 0) ) || \
          ( defined(AES_COUNT) && (AES_COUNT > 0) ) */

#endif /* #ifndef MBEDTLS_SLCL_DEVICE_INTERNAL_H */
