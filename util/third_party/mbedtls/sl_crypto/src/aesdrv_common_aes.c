/*
 *  Common functions for AES based algorithms for the AES hw module.
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
#include "em_device.h"

#if defined(AES_COUNT) && (AES_COUNT > 0)

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include "aesdrv_internal.h"
#include "aesdrv_common_aes.h"
#include "slcl_device.h"
#include "sl_crypto.h"
#include "em_assert.h"
#include <string.h>

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *  Initialize AES device specifics for given device instance
 *
 * @param devno
 *  AES device instance number.
 *
 * @return
 *   0 if OK, or MBEDTLS_ERR_DEVICE_NOT_SUPPORTED if device number is invalid.
 ******************************************************************************/
int mbedtls_device_specific_init (unsigned int devno)
{
  if (devno >= AES_COUNT)
    return( MBEDTLS_ERR_DEVICE_NOT_SUPPORTED );
  else
    return( 0 );
}
  
/***************************************************************************//**
 * @brief
 *  Deinitialize AES device specifics for given device instance
 *
 * @param devno
 *  AES device instance number.
 *
 * @return
 *   0 if OK, or MBEDTLS_ERR_DEVICE_NOT_SUPPORTED if device number is invalid.
 ******************************************************************************/
int mbedtls_device_specific_free (unsigned int devno)
{
  if (devno >= AES_COUNT)
    return( MBEDTLS_ERR_DEVICE_NOT_SUPPORTED );
  else
    return( 0 );
}

/*
 *   Initializes an AESDRV context structure.
 *   Please refer to aesdrv.h for detailed description.
 */
int AESDRV_Init(AESDRV_Context_t* pAesdrvContext)
{
  /* Clear the driver context. */
  memset(pAesdrvContext, 0, sizeof(AESDRV_Context_t));
  return 0;
}

/*
 *   DeInitializes an AESDRV context structure.
 *   Please refer to aesdrv.h for detailed description.
 */
int AESDRV_DeInit(AESDRV_Context_t* pAesdrvContext)
{
  /* Clear the driver context. */
  memset(pAesdrvContext, 0, sizeof(AESDRV_Context_t));
  return 0;
}

/*
 *   Set the AES device instance.
 *   Please refer to aesdrv.h for detailed description.
 */
int AESDRV_SetDeviceInstance(AESDRV_Context_t*  pAesdrvContext,
                             unsigned int       devno)
{
  (void) pAesdrvContext; /* Multiple instances not supported for AES module. */
  
  if (devno >= AES_COUNT)
    return MBEDTLS_ECODE_AESDRV_INVALID_PARAM;
  else
    return 0;
}

/*
 *   Setup CRYPTO I/O mode.
 *   Please refer to aesdrv.h for detailed description.
 */
int AESDRV_SetIoMode
(
 AESDRV_Context_t*        pAesdrvContext,
 AESDRV_IoMode_t          ioMode,
 AESDRV_IoModeSpecific_t* ioModeSpecific
 )
{
  (void) pAesdrvContext;  /* Not supported for AES module. */
  (void) ioModeSpecific;  /* Not supported for AES module. */
  if (ioMode == aesdrvIoModeCore)
  {
    return 0;
  }
  else
  {
    return MBEDTLS_ECODE_AESDRV_NOT_SUPPORTED;
  }
}

#endif /* #if defined(AES_COUNT) && (AES_COUNT > 0) */
