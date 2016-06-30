/*
 *  CRYPTO driver implementation including CRYPTO preemption and asynchronous
 *  (non-blocking) support.
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
#include "em_device.h"

#if defined(CRYPTO_COUNT) && (CRYPTO_COUNT > 0)

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include "cryptodrv_internal.h"
#include "em_crypto.h"
#include "em_assert.h"
#include "em_bitband.h"
#include "em_int.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#if defined( CRYPTODRV_PAL_FREERTOS )
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#define CRYPTO_IRQn_PRIORITY            (configMAX_SYSCALL_INTERRUPT_PRIORITY-1)
#endif

/*******************************************************************************
 ********************************   MACROS   ***********************************
 ******************************************************************************/
#if defined(MBEDTLS_CRYPTO_DEVICE_PREEMPTION)

#define CRYPTODRV_CLOCK_ENABLE  CMU->HFBUSCLKEN0 |= CMU_HFBUSCLKEN0_CRYPTO
#define CRYPTODRV_CLOCK_DISABLE CMU->HFBUSCLKEN0 &= ~CMU_HFBUSCLKEN0_CRYPTO

#define RUNNING_AT_INTERRUPT_LEVEL  (SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk)

#if defined( _EFR32_MIGHTY_FAMILY ) || defined( _EFM32_PEARL_FAMILY )
#define MAX_NVIC_ISER  (2)   /* FPUEH_IRQn = 33 is the highest IRQn
                                and all IRQns fits inside 2 ISERs
                                ISER[0] and ISER[1]. */
#elif defined( _EFM32_JADE_FAMILY )
#define MAX_NVIC_ISER  (1)   /* CRYOTIMER_IRQn = 31 is the highest IRQn
                                and all IRQns fits inside one ISER[0]. */
#else
#error Device not supported.
#endif

#endif /* #if defined(MBEDTLS_CRYPTO_DEVICE_PREEMPTION) */

/*******************************************************************************
 ********************************   STATICS   **********************************
 ******************************************************************************/

#if defined(MBEDTLS_CRYPTO_DEVICE_PREEMPTION)

/* Pointer to current owner context of the CRYPTO unit. The @ref cryptoOwner
   pointer serves as the anchor to a double linked list of all current "active"
   CRYPTODRV contexts. The @ref CRYPTODRV_Arbitrate function add a new owner
   if ownership is won, and CRYPTODRV_Release removes a context that is done.*/
static CRYPTODRV_Context_t* cryptoOwner = NULL;

/* Flag which indicates whether a CRYPTO critical region is active. */
static bool     cryptoCriticalRegionActive=false;
static uint32_t nvicIser[MAX_NVIC_ISER];

#if defined( CRYPTODRV_PAL_FREERTOS )

#define CRYPTODRV_PAL_THREADS_LOCK \
  if (false == RUNNING_AT_INTERRUPT_LEVEL) \
    vTaskSuspendAll();
#define CRYPTODRV_PAL_THREADS_UNLOCK \
  if (false == RUNNING_AT_INTERRUPT_LEVEL) \
    xTaskResumeAll();

#define CRYPTODRV_PAL_THREAD_ID_GET  (void*)xTaskGetCurrentTaskHandle()

#define CRYPTODRV_PAL_THREAD_PRIORITY_GET \
  ((unsigned long)uxTaskPriorityGet(NULL))

#define CRYPTODRV_PAL_THREAD_RESUME(threadId) \
  xTaskResumeFromISR((TaskHandle_t)threadId)

#define CRYPTODRV_PAL_WAIT_FOR_OWNERSHIP(pCryptodrvContext) \
  cryptodrvPalWaitForOwnership(pCryptodrvContext)

__STATIC_INLINE void cryptodrvPalWaitForOwnership(CRYPTODRV_Context_t* pCryptodrvContext)
{
  while (pCryptodrvContext != cryptoOwner)
  {
    vTaskSuspend(xTaskGetCurrentTaskHandle());
  }
}

#else  /* #if defined( CRYPTODRV_PAL_FREERTOS ) */

#define CRYPTODRV_PAL_THREADS_LOCK
#define CRYPTODRV_PAL_THREADS_UNLOCK
#define CRYPTODRV_PAL_THREAD_ID_GET       (0)
#define CRYPTODRV_PAL_THREAD_PRIORITY_GET (RUNNING_AT_INTERRUPT_LEVEL ? 1 : 0)
#define CRYPTODRV_PAL_THREAD_RESUME(threadId)
#define CRYPTODRV_PAL_WAIT_FOR_OWNERSHIP(pCryptodrvContext)

#endif /* #if defined( CRYPTODRV_PAL_FREERTOS ) */

#endif /* #if defined(MBEDTLS_CRYPTO_DEVICE_PREEMPTION) */

#if defined(MBEDTLS_INCLUDE_ASYNCH_API)
/* Current callback function called from CRYPTO_IRQHandler. */
static CRYPTODRV_AsynchCallback_t cryptoDrvAsynchCallback = 0;
static void* cryptoDrvAsynchCallbackArgument;
#endif

/*******************************************************************************
 ******************************   FUNCTIONS   **********************************
 ******************************************************************************/
#if ( defined(MBEDTLS_CRYPTO_DEVICE_PREEMPTION) && \
      !defined(MBEDTLS_CRYPTO_CRITICAL_REGION_ALT) )  \
  || defined(MBEDTLS_INCLUDE_ASYNCH_API)
__STATIC_INLINE IRQn_Type cryptodrvGetIrqn(CRYPTO_TypeDef* crypto)
{
#if defined( CRYPTO )
  if (crypto == CRYPTO)
  {
    return CRYPTO_IRQn;
  }
#endif
#if defined( CRYPTO0 )
  if (crypto == CRYPTO0)
  {
    return CRYPTO0_IRQn;
  }
#endif
#if defined( CRYPTO1 )
  if (crypto == CRYPTO1)
  {
    return CRYPTO1_IRQn;
  }
#endif
  /* Assert that we return before this point. */
  EFM_ASSERT(false);
  return (IRQn_Type) -1;
}
#endif

#if defined MBEDTLS_CRYPTO_DEVICE_PREEMPTION

/***************************************************************************//**
 * @brief
 *   Check state of a CRYPTO device instance.
 *
 * @details
 *   Checks if a CRYPTO device is idle and ready for new operation, or busy
 *   running an ongoing operation.
 *
 * @return
 *   MBEDTLS_ECODE_CRYPTODRV_BUSY if CRYPTO is busy running an ongoing operation.
 *   ECODE_OK if idle and ready for new operation.
 ******************************************************************************/
Ecode_t CRYPTODRV_CheckState( void )
{
  /* The 'cryptoOwner' pointer indicates whether someone is already running a
     CRYPTO operation. */
  return cryptoOwner ? MBEDTLS_ECODE_CRYPTODRV_BUSY : ECODE_OK;
}

/***************************************************************************//**
 * @brief
 *  Arbitrate for exclusive access to CRYPTO unit.
 *
 * @details
 *  This function will arbitrate for the ownership of the CRYPTO unit in order
 *  to execute CRYPTO operations. If the ownership is won and there is an
 *  ongoing CRYPTO operation, this function will preempt the ongoing operation
 *  by storing the full CRYPTODRV context inlcuding hardware context
 * (register values) of the CRYPTO unit. When the caller is ready to release
 *  CRYPTO, the caller should call CRYPTODRV_Release in
 *  order to restore the context of the ongoing operation, in order for that to
 *  continue operation. The function call of the ongoing operation may try to
 *  restore and continue after the abort event, but may fail and return
 *  MBEDTLS_ECODE_CRYPTODRV_OPERATION_ABORTED.
 *  This function may be called from an interrupt handler in order to perform
 *  time critical CRYPTO operations.
 *
 * @param pCryptodrvContext
 *  Pointer to CRYPTODRV context which should be used in subsequent calls,
 *  like @ref CRYPTODRV_Release.
 *
 * @return
 *  MBEDTLS_ECODE_CRYPTODRV_OK if success. Error code if failure.
 *  MBEDTLS_ECODE_CRYPTODRV_BUSY if priority is lower than or equal to running thread.
 */
Ecode_t CRYPTODRV_Arbitrate (CRYPTODRV_Context_t* pCryptodrvContext)
{
  CRYPTO_TypeDef*  crypto = pCryptodrvContext->crypto;
  Ecode_t          retval = ECODE_OK;
    
  INT_Disable();
  CRYPTODRV_PAL_THREADS_LOCK;

  if (cryptoCriticalRegionActive)
  {
    retval = MBEDTLS_ECODE_CRYPTODRV_BUSY;
  }
  else
  {    
    /* Check if someone is already running a CRYPTO operation. */
    if (cryptoOwner)
    {
      if (CRYPTODRV_PAL_THREAD_PRIORITY_GET <= cryptoOwner->threadPriority)
      {
        retval = MBEDTLS_ECODE_CRYPTODRV_BUSY;
      }
      else
      {
        CRYPTO_Context_t*  pCryptoContext;
        uint8_t*           pExecCmd;

         /* We are running an ISR or thread of higher priority than the
            current crypto owner. If crypto is running, stop it and mark
            context as aborted. */
        if (crypto->STATUS
            & (CRYPTO_STATUS_INSTRRUNNING | CRYPTO_STATUS_SEQRUNNING))
        {
          crypto->CMD = CRYPTO_CMD_SEQSTOP;
          cryptoOwner->aborted = true;
        }
        else
        {
          cryptoOwner->aborted = false;
        }
   
        cryptoOwner->pContextPreemptor = pCryptodrvContext;

#if defined(MBEDTLS_INCLUDE_ASYNCH_API)
        /* Store the asynch callback state */
        cryptoOwner->asynchCallback         = cryptoDrvAsynchCallback;
        cryptoOwner->asynchCallbackArgument = cryptoDrvAsynchCallbackArgument;
#endif
        
        /* Store the hardware state */
        pCryptoContext = &cryptoOwner->cryptoContext;

        pCryptoContext->CTRL     = crypto->CTRL;
        pCryptoContext->WAC      = crypto->WAC;
        pCryptoContext->SEQCTRL  = crypto->SEQCTRL;
        pCryptoContext->SEQCTRLB = crypto->SEQCTRLB;
        pCryptoContext->IEN      = crypto->IEN;
        pCryptoContext->SEQ[0]   = crypto->SEQ0;
        pCryptoContext->SEQ[1]   = crypto->SEQ1;
        pCryptoContext->SEQ[2]   = crypto->SEQ2;
        pCryptoContext->SEQ[3]   = crypto->SEQ3;
        pCryptoContext->SEQ[4]   = crypto->SEQ4;
        
        /* Search for possible EXEC commands and replace with END. */
        pExecCmd = (uint8_t*) memchr(&pCryptoContext->SEQ,
                                     CRYPTO_CMD_INSTR_EXEC,
                                     sizeof(pCryptoContext->SEQ));
        if (pExecCmd)
        {
          memset(pExecCmd,
                 (uint8_t) CRYPTO_CMD_INSTR_END,
                 sizeof(pCryptoContext->SEQ) -
                 ((uint32_t)pExecCmd-(uint32_t)&pCryptoContext->SEQ));
        }
        CRYPTO_DDataRead(&crypto->DDATA0, pCryptoContext->DDATA[0]);
        CRYPTO_DDataRead(&crypto->DDATA1, pCryptoContext->DDATA[1]);
        CRYPTO_DDataRead(&crypto->DDATA2, pCryptoContext->DDATA[2]);
        CRYPTO_DDataRead(&crypto->DDATA3, pCryptoContext->DDATA[3]);
        CRYPTO_DDataRead(&crypto->DDATA4, pCryptoContext->DDATA[4]);
        
        retval = ECODE_OK;
      }
    }
    else
    {
      CRYPTODRV_CLOCK_ENABLE;
    }

    if (ECODE_OK ==  retval)
    {
      pCryptodrvContext->pContextPreempted = cryptoOwner;
      cryptoOwner = pCryptodrvContext;
      pCryptodrvContext->pContextPreemptor = 0;
      pCryptodrvContext->aborted = false;
      pCryptodrvContext->threadPriority = CRYPTODRV_PAL_THREAD_PRIORITY_GET;
      pCryptodrvContext->threadId = CRYPTODRV_PAL_THREAD_ID_GET;

#if defined(MBEDTLS_INCLUDE_ASYNCH_API)
      CRYPTODRV_SetAsynchCallback(pCryptodrvContext, 0, 0);
#endif
    }
  }

  CRYPTODRV_PAL_THREADS_UNLOCK;
  INT_Enable();
  
  return retval;
}

/***************************************************************************//**
 * @brief
 *  Release a CRYPTODRV context and possibly ownership of CRYPTO unit.
 *
 * @details
 *  This function removes a CRYPTODRV context from the CRYPTODRV context list
 *  (which includes current owner and preempted contexts).
 *  If the specified context is the owner of the CRYPTO unit, this function
 *  will release the ownership of the CRYPTO unit. If there is a preempted
 *  context in the context list, then the preempted context is restored
 *  by updating the hardware state of the CRYPTO unit.
 *
 * @param pCryptodrvContext
 *  The CRYPTODRV context to remove from CRYPTODRV context list.
 *
 * @return
 *  MBEDTLS_ECODE_CRYPTODRV_OK if success. Error code if failure.
 */
Ecode_t CRYPTODRV_Release (CRYPTODRV_Context_t* pCryptodrvContext)
{
  CRYPTODRV_Context_t* preempted =
    (CRYPTODRV_Context_t*) pCryptodrvContext->pContextPreempted;
  CRYPTODRV_Context_t* preemptor =
    (CRYPTODRV_Context_t*) pCryptodrvContext->pContextPreemptor;
  CRYPTO_TypeDef* crypto = pCryptodrvContext->crypto;  
   
  INT_Disable();
  CRYPTODRV_PAL_THREADS_LOCK;

  if ( (0==preempted) && (0==preemptor))
  {
    cryptoOwner = NULL;
    CRYPTODRV_CLOCK_DISABLE;
  }
  else
  {
    /* If _this_ context was preempted, and the preemptor is still running,
       then inform the preemptor that _this_ context is not valid any more
       by linking to _this_ preempted context (which may be NULL). */
    if (preemptor)
    {
      preemptor->pContextPreempted = preempted;
    }

    if (preempted)
    {
      /* Remove _this_ context from preemption list (double linked) by
         replacing preempted preemptor (_this_) with preemptor of _this_
         context (may be NULL if _this_ is owner of crypto). */
      preempted->pContextPreemptor = preemptor;

      /* If _this_ conxtext is the owner of crypto, restore preempted
         context and set it to owner. */
      if (cryptoOwner == pCryptodrvContext)
      {
        CRYPTO_Context_t*  pCryptoContext = &preempted->cryptoContext;
      
        crypto->CTRL     = pCryptoContext->CTRL;
        crypto->WAC      = pCryptoContext->WAC;
        crypto->SEQCTRL  = pCryptoContext->SEQCTRL;
        crypto->SEQCTRLB = pCryptoContext->SEQCTRLB;
        crypto->IEN      = pCryptoContext->IEN;
        crypto->SEQ0     = pCryptoContext->SEQ[0];
        crypto->SEQ1     = pCryptoContext->SEQ[1];
        crypto->SEQ2     = pCryptoContext->SEQ[2];
        crypto->SEQ3     = pCryptoContext->SEQ[3];
        crypto->SEQ4     = pCryptoContext->SEQ[4];
        CRYPTO_DDataWrite(&crypto->DDATA0, pCryptoContext->DDATA[0]);
        CRYPTO_DDataWrite(&crypto->DDATA1, pCryptoContext->DDATA[1]);
        CRYPTO_DDataWrite(&crypto->DDATA2, pCryptoContext->DDATA[2]);
        CRYPTO_DDataWrite(&crypto->DDATA3, pCryptoContext->DDATA[3]);
        CRYPTO_DDataWrite(&crypto->DDATA4, pCryptoContext->DDATA[4]);

#if defined(MBEDTLS_INCLUDE_ASYNCH_API)
        CRYPTODRV_SetAsynchCallback(pCryptodrvContext,
                                    preempted->asynchCallback,
                                    preempted->asynchCallbackArgument);
#endif
        
        cryptoOwner      = preempted;
        /* Resume new owner task (which may be suspended by now). */
        CRYPTODRV_PAL_THREAD_RESUME(cryptoOwner->threadId);
      }
    }
  }

  CRYPTODRV_PAL_THREADS_UNLOCK;
  INT_Enable();
  
  return ECODE_OK;
}

#if !defined( MBEDTLS_CRYPTO_CRITICAL_REGION_ALT )

/***************************************************************************//**
 * @brief
 *  Enter a critical CRYPTO region.
 *
 * @details
 *  This function enters a critical region of a CRYPTO operation by disabling
 *  all interrupts, except the CRYPTO interrupt. If a thread which is not the
 *  owner of CRYPTO tries to enter the critical region, it will be suspended.
 *  The thread will be resumed when the preemptor calls CRYPTODRV_Release.
 *
 * @return
 *  ECODE_OK
*/
Ecode_t CRYPTODRV_EnterCriticalRegion (CRYPTODRV_Context_t* pCryptodrvContext)
{
  int i;
  CRYPTO_TypeDef* crypto = pCryptodrvContext->crypto;
  IRQn_Type irqn = cryptodrvGetIrqn(crypto);
  
  CRYPTODRV_PAL_WAIT_FOR_OWNERSHIP(pCryptodrvContext);
  
  INT_Disable();
  CRYPTODRV_PAL_THREADS_LOCK;

  EFM_ASSERT(!cryptoCriticalRegionActive);
    
  /* Disable all interrupts except the CRYPTO IRQ. Remember which interrupts
     that was enabled in order to enable them when exiting the critical
     region. */
  for (i=0; i<MAX_NVIC_ISER; i++)
  {
    nvicIser[i]   = NVIC->ISER[i];
    NVIC->ICER[i] = nvicIser[i];
  }
  NVIC->ISER[(uint32_t)((int32_t)irqn) >> 5] =
    (uint32_t)(1 << ((uint32_t)((int32_t)irqn) & (uint32_t)0x1F));

  cryptoCriticalRegionActive = true;

  CRYPTODRV_PAL_THREADS_UNLOCK;
  INT_Enable();
  
  return ECODE_OK;
}

/***************************************************************************//**
 * @brief
 *  Exit a critical CRYPTO region.
 *
 * @details
 *  This function exits a critical region of a CRYPTO operation. 
 *  This function will enable all interrupts that was enabled when the initial
 *  nested critical region was entered.
 *
 * @return
 *  ECODE_OK
 */
Ecode_t CRYPTODRV_ExitCriticalRegion (CRYPTODRV_Context_t* pCryptodrvContext)
{
  int i;
  (void) pCryptodrvContext; /* remove compiler warning when pCryptodrvContext
                             is not used.*/
  
  INT_Disable();
  CRYPTODRV_PAL_THREADS_LOCK;

  EFM_ASSERT(cryptoCriticalRegionActive);
  
  /* Enable all interrupts that was enabled when entering the critical
     region. */
  for (i=0; i<MAX_NVIC_ISER; i++)
  {
    NVIC->ISER[i] = nvicIser[i];
  }
  cryptoCriticalRegionActive = false;

  CRYPTODRV_PAL_THREADS_UNLOCK;
  INT_Enable();
  
  return ECODE_OK;
}

#endif /* #if !defined( MBEDTLS_CRYPTO_CRITICAL_REGION_ALT ) */

#endif /* #if defined( MBEDTLS_CRYPTO_DEVICE_PREEMPTION ) */

#if defined(MBEDTLS_INCLUDE_ASYNCH_API)

/***************************************************************************//**
 * @brief
 *  Set asynchronous callback to be called when crypto operations complete.
 *
 * @details
 *  This function sets the asynchronous callback function to be called when
 *  crypto operations complete. This function should be called inside _the_
 *  critical region of an asynchronous operation.
 *
 * @return
 *  N/A
 */
void CRYPTODRV_SetAsynchCallback
(
 CRYPTODRV_Context_t*       pCryptodrvContext,
 CRYPTODRV_AsynchCallback_t asynchCallback,
 void*                      callbackArgument
 )
{
  CRYPTO_TypeDef*      crypto = pCryptodrvContext->crypto;
  if (asynchCallback)
  {
    cryptoDrvAsynchCallback = asynchCallback;
    cryptoDrvAsynchCallbackArgument = callbackArgument;
    crypto->IFC = _CRYPTO_IFC_MASK;
    crypto->IEN = CRYPTO_IEN_SEQDONE;
    NVIC_ClearPendingIRQ(cryptodrvGetIrqn(crypto));
    NVIC_EnableIRQ(cryptodrvGetIrqn(crypto));
#if defined( CRYPTODRV_PAL_FREERTOS )
    /* Set priority below the configured maximum system call priority */
    NVIC_SetPriority(cryptodrvGetIrqn(crypto), CRYPTO_IRQn_PRIORITY);
#endif
  }
  else
  {
    cryptoDrvAsynchCallback = 0;
    crypto->IEN = 0;
    crypto->IFC = _CRYPTO_IFC_MASK;
    NVIC_DisableIRQ(cryptodrvGetIrqn(crypto));
  }
}

/***************************************************************************//**
 * @brief
 *  Interrupt service routine for CRYPTO module instances.
 *
 * @details
 *  CRYPTO_IRQHandler, CRYPTO0_IRQHandler, etc are the first functions to be
 *  called when an interrupt from the respective CRYPTO instance is being
 *  serviced by the MCU. The function cryptoIrqHandlerGeneric is called with
 *  a pointer to the respective CRYPTO unit, and it will clear the interrupt
 *  and call the interrupt service routine associated with the operation that
 *  caused the interrupt. The operation specific ISR must be registered by
 *  calling @ref CRYPTODRV_SetAsynchCallback before the operation is started.
 *
 * @return
 *  N/A
 */
void cryptoIrqHandlerGeneric( CRYPTO_TypeDef* crypto )
{
  IRQn_Type irqn = cryptodrvGetIrqn(crypto);
  uint32_t flags = crypto->IF;

  while (flags)
  {
    crypto->IFC = flags;
    NVIC_ClearPendingIRQ(irqn);
    
    if (flags & CRYPTO_IF_SEQDONE)
    {
      if (cryptoDrvAsynchCallback)
      {
        cryptoDrvAsynchCallback (cryptoDrvAsynchCallbackArgument);
      }
    }
    if (CMU->HFBUSCLKEN0 & CMU_HFBUSCLKEN0_CRYPTO)
      flags = crypto->IF;
    else
      flags = 0;
  }
}
                              
#if defined(CRYPTO)
void CRYPTO_IRQHandler(void)
{
  cryptoIrqHandlerGeneric(CRYPTO);
}
#endif

#if defined(CRYPTO0)
void CRYPTO0_IRQHandler(void)
{
  cryptoIrqHandlerGeneric(CRYPTO0);
}
#endif

#if defined(CRYPTO1)
void CRYPTO1_IRQHandler(void)
{
  cryptoIrqHandlerGeneric(CRYPTO1);
}
#endif

#endif /* #if defined(MBEDTLS_INCLUDE_ASYNCH_API) */

#endif /* #if defined(CRYPTO_COUNT) && (CRYPTO_COUNT > 0) */
