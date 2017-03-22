/*
 * \file slcl_device.h
 *
 * \brief Silicon Labs CRYPTO/AES device interface
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
#ifndef MBEDTLS_SLCL_DEVICE_H
#define MBEDTLS_SLCL_DEVICE_H

#if !defined(MBEDTLS_CONFIG_FILE)
#include "config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include "em_device.h"

#if ( defined(CRYPTO_COUNT) && (CRYPTO_COUNT > 0) ) || \
          ( defined(AES_COUNT) && (AES_COUNT > 0) )

#include "mbedtls_ecode.h"
#include "slpal.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *******************************   DEFINES    **********************************
 ******************************************************************************/

/* Error codes: */
#define MBEDTLS_ERR_DEVICE_NOT_SUPPORTED  ((int)MBEDTLS_ERR_DEVICE_BASE | 0x1)

/* Device counts supported. */
#if defined(AES_COUNT) && (AES_COUNT > 0)
#define MBEDTLS_DEVICE_COUNT AES_COUNT
#endif
#if defined(CRYPTO_COUNT) && (CRYPTO_COUNT > 0)
#define MBEDTLS_DEVICE_COUNT CRYPTO_COUNT
#endif

/*******************************************************************************
 *******************************   TYPEDEFS   **********************************
 ******************************************************************************/

/** Device context structure. */
typedef struct
{
    unsigned int             devno;       /* Device instance number */

#if defined(MBEDTLS_CRYPTO_DEVICE_PREEMPTION)
    /* Device owner context.  The owner pointer serves as the anchor to a
       double linked list of all current "active" contexts. The ownership
       may be won when calling the mbedTLS API if the priority is higher than
       the current owner context and the current context is not inside a
       critical section. If ownership is won the new owner context will
       be added in front of the double linked list. The context will be removed
       when the operation is complete before the mbedTLS API returns. */
    void                    *owner;

    /* Mutex used to protect the data associated with the device
       when running in critical sections. */
    SLPAL_Mutex_t            lock;
  
#endif /* #if defined(MBEDTLS_CRYPTO_DEVICE_PREEMPTION) */
  
#if defined(MBEDTLS_DEVICE_YIELD_WHEN_BUSY)
    SLPAL_Completion_t       operation;   /* Device operation completion
                                             object. */
#endif
  
} mbedtls_device_context;

/*******************************************************************************
 *******************************   GLOBALS    **********************************
 ******************************************************************************/
  
/* Table of pointers to mbedtls device contexts of each CRYPTO/AES hardware
   instance. */
extern mbedtls_device_context *p_mbedtls_device_context[MBEDTLS_DEVICE_COUNT];

/*******************************************************************************
 *******************************  FUNCTIONS   **********************************
 ******************************************************************************/
  
/**
 * \brief
 *   Initialize an mbedtls device context.
 *
 * \details
 *   This function initializes an mbedtls device context. 
 *   By default the mbedtls_device_init function is called internally in order
 *   for backwards compatibility. However the internal initialization consumes
 *   some RAM because the device structures are instantiated statically inside
 *   the mbedTLS library. The user application may define
 *   MBEDTLS_DEVICE_INIT_INTERNAL_DISABLE in order to disable the internal
 *   device initialization, and take responsibility of allocating and
 *   initializing the device context structures itself.
 *   If MBEDTLS_DEVICE_INIT_INTERNAL_DISABLE is defined the user application is
 *   required to call mbedtls_device_init and \ref mbedtls_device_set_instance
 *   before any subsequent calls to mbedtls APIs involving SLCL plugins.
 *   The \ref mbedtls_device_set_instance function will store a pointer to the
 *   mbedtls context in the internal device table in in order to retrieve
 *   context and corresponding attributes in subsequent calls to mbedtls.
 *
 * \note
 *   The user should only initialize one mbedtls device context per
 *   CRYPTO/AES hardware device.
 *
 * \param[in] ctx
 *   mbedtls device context.
 *  
 ******************************************************************************/
void mbedtls_device_init( mbedtls_device_context *ctx );

/**
 * \brief
 *   Free an mbedtls device context.
 *
 * \details
 *   This function frees the resources kept by an mbedtls device context,
 *   and removes the pointer to the mbedtls context from the internal device
 *   table.
 *
 * \param[in] ctx
 *   mbedtls device context.
 *  
 ******************************************************************************/
void mbedtls_device_free( mbedtls_device_context *ctx );

/**
 * \brief
 *   Set the device instance of an mbedtls device context.
 *
 * \details
 *   This function sets the AES/CRYPTO device instance of an mbedtls device
 *   context. Subsequent calls to API functions with this context will use
 *   the given AES/CRYPTO device instance. \ref mbedtls_device_set_instance
 *   will store a pointer to the mbedtls context in the internal device
 *   table in in order to retrieve context and corresponding attributes in
 *   subsequent calls to mbedtls.
 *   By default the mbedtls_device_set_instance function is called internally
 *   for backwards compatibility, and the user application will not need to
 *   call mbedtls_device_set_instance. If MBEDTLS_DEVICE_INIT_INTERNAL_DISABLE
 *   is defined the user is required to call \ref mbedtls_device_init and
 *   \ref mbedtls_device_set_instance before any other mbedTLS API calls in
 *   order to initialize the CRYPTO device context structure(s)
 *   (\ref mbedtls_device_context)  Please refer to
 *   \ref mbedtls_device_init for more information.
 *
 * \param[in] ctx
 *   mbedtls device context.
 *  
 * \param[in] devno
 *   AES/CRYPTO hardware device instance to use.
 *  
 * \return
 *   0 if success. Error code if failure.
 ******************************************************************************/
int mbedtls_device_set_instance( mbedtls_device_context* ctx,
                                 unsigned int devno );
  
#ifdef __cplusplus
}
#endif

#endif /* #if ( defined(CRYPTO_COUNT) && (CRYPTO_COUNT > 0) ) || \
          ( defined(AES_COUNT) && (AES_COUNT > 0) ) */

#endif /* #ifndef MBEDTLS_SLCL_DEVICE_H */
