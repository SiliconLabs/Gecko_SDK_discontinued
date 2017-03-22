/*
 * \file cryptodrv.h
 *
 * \brief CRYPTO driver definitions including CRYPTO preemption and
 *  yield when crypto busy management.
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
#ifndef MBEDTLS_CRYPTODRV_H
#define MBEDTLS_CRYPTODRV_H

#if !defined(MBEDTLS_CONFIG_FILE)
#include "config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include "em_device.h"

#if ( defined(CRYPTO_COUNT) && (CRYPTO_COUNT > 0) )

#include "slcl_device.h"
#include "mbedtls_ecode.h"
#include "em_crypto.h"
#include "em_core.h"
#if defined( MBEDTLS_DEVICE_YIELD_WHEN_BUSY )
#include "slpal.h"
#endif
#if defined( MBEDTLS_INCLUDE_IO_MODE_DMA )
#include "dmadrv.h"
#endif
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 ******************************* ERROR CODES  **********************************
 ******************************************************************************/

#define MBEDTLS_ECODE_CRYPTODRV_OPERATION_ABORTED ((int)MBEDTLS_ECODE_CRYPTODRV_BASE | 0x1) /**< Operation was aborted. Possibly by higher priority task. The user will need to retry the operation in order to complete.  */
#define MBEDTLS_ECODE_CRYPTODRV_BUSY              ((int)MBEDTLS_ECODE_CRYPTODRV_BASE | 0x2) /**< The CRYPTO module is busy. */

/*******************************************************************************
 *******************************   TYPEDEFS   **********************************
 ******************************************************************************/

#if defined(MBEDTLS_CRYPTO_DEVICE_PREEMPTION)

/** Context of CRYPTO hardware module. */
typedef struct
{
  uint32_t CTRL;          /**< Control Register  */
  uint32_t WAC;           /**< Wide Arithmetic Configuration  */
  uint32_t SEQCTRL;       /**< Sequence Control  */
  uint32_t SEQCTRLB;      /**< Sequence Control B  */
  uint32_t IEN;           /**< Interrupt Enable Register  */
  uint32_t SEQ[5];        /**< Instruction Sequence registers */
  CRYPTO_Data260_TypeDef DDATA[5]; /**< DDATA registers. Covers all data
                                      registers
                                      of CRYPTO, including DATA(128 bit),
                                      DDATA (256bit/260bit),
                                      QDATA (512bit) registers. */
} CRYPTO_Context_t;
  
#endif /* #if defined(MBEDTLS_CRYPTO_DEVICE_PREEMPTION) */

/** CRYPTO device structure. */
typedef struct
{
  CRYPTO_TypeDef           *crypto;          /**< CRYPTO hw instance */
  IRQn_Type                 irqn;            /**< CRYPTO irq number */
  uint32_t                  clk;             /**< CRYPTO clock */
  mbedtls_device_context  **ppMbedtlsDevice; /**< Pointer to pointer of
                                                  mbedtls device context */
#if defined(MBEDTLS_INCLUDE_IO_MODE_DMA)
  DMADRV_PeripheralSignal_t dmaReqSigChIn;   /**< DMA req signal channel in */
  DMADRV_PeripheralSignal_t dmaReqSigChOut;  /**< DMA req signal channel out */
#endif
} CRYPTO_Device_t;

/** CRYPTODRV operation context. */
typedef struct
{
  const CRYPTO_Device_t *device;          /**< CRYPTO hw instance */

#if defined( MBEDTLS_CRYPTO_DEVICE_PREEMPTION )
  
  CRYPTO_Context_t   cryptoContext;       /**< CRYPTO hw context */
  bool               aborted;             /**< 'true' if an ongoing CRYPTO
                                               instruction sequence was
                                               aborted.*/
  int                lockWaitTicks;       /**< Ticks to wait for ownership */
  unsigned long      priority;            /**< Priority of _this_ context */

  /* Double linked-list for CRYPTO context preemption. */
  void*              pContextPreempted;   /**< Context preempted by _this_
                                               context. */
  void*              pContextPreemptor;   /**< Context of preemptor
                                               of _this_ context. */
#if defined( MBEDTLS_ALLOW_ISR_CALLS_IRQ_MASK )
  CORE_nvicMask_t    nvicState;           /**< State of NVIC registers.
                                               Used to store NVIC state while
                                               interrupts are disabled. */
#endif
#if defined( MBEDTLS_DEVICE_YIELD_WHEN_NOT_OWNER )
  bool               waitOwnership;       /**< Flag indicating that _this_
                                               context is waiting for ownership.
                                          */
  SLPAL_Completion_t ownership;           /**< Completion object signaled by
                                               owner when ownership is returned
                                               to _this_ context. */
#endif
#endif /* #if defined(MBEDTLS_CRYPTO_DEVICE_PREEMPTION) */
  
} CRYPTODRV_Context_t;

#ifdef __cplusplus
}
#endif

#endif /* #if ( defined(CRYPTO_COUNT) && (CRYPTO_COUNT > 0) ) || \
          ( defined(AES_COUNT) && (AES_COUNT > 0) ) */

#endif /* #ifndef MBEDTLS_CRYPTODRV_H */
