/*
 *  CRYPTO driver implementation including CRYPTO preemption and yield when
 *  crypto busy management.
 *
 *  Copyright (C) 2016, Silicon Labs, httgp://www.silabs.com
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
#include "slcl_device_internal.h"
#include "slpal.h"
#include "em_crypto.h"
#include "em_assert.h"
#include "em_core.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

/*******************************************************************************
 ********************************   MACROS   ***********************************
 ******************************************************************************/
#if defined(MBEDTLS_CRYPTO_DEVICE_PREEMPTION) || \
    defined(MBEDTLS_DEVICE_YIELD_WHEN_BUSY)

#define CRYPTODRV_CLOCK_ENABLE(clk)  CMU->HFBUSCLKEN0 |= clk
#define CRYPTODRV_CLOCK_DISABLE(clk) CMU->HFBUSCLKEN0 &= ~(clk)

#endif

/*******************************************************************************
 ********************************   STATICS   **********************************
 ******************************************************************************/

#if !defined( MBEDTLS_DEVICE_INIT_INTERNAL_DISABLE )
/* Device structures for internal initialization in order for backwards
   compatibility. Use MBEDTLS_DEVICE_INIT_INTERNAL_DISABLE to disable
   the internal device initialization. Internal initialization consumes
   more RAM because the device structures are instantiated statically here,
   inside the mbedTLS library. */
static mbedtls_device_context _mbedtls_device_ctx[MBEDTLS_DEVICE_COUNT];
#endif

/* CRYPTO device instance structures. */
static const CRYPTO_Device_t cryptoDevice[CRYPTO_COUNT] =
{
#if defined( CRYPTO0 )
  {
    CRYPTO0,
    CRYPTO0_IRQn,
    CMU_HFBUSCLKEN0_CRYPTO0,
    &p_mbedtls_device_context[0]
#if defined( MBEDTLS_INCLUDE_IO_MODE_DMA )
    ,
    dmadrvPeripheralSignal_CRYPTO0_DATA0WR,
    dmadrvPeripheralSignal_CRYPTO0_DATA0RD
#endif
  }
#elif defined( CRYPTO )
  {
    CRYPTO,
    CRYPTO_IRQn,
    CMU_HFBUSCLKEN0_CRYPTO,
    &p_mbedtls_device_context[0]
#if defined( MBEDTLS_INCLUDE_IO_MODE_DMA )
    ,
    dmadrvPeripheralSignal_CRYPTO_DATA0WR,
    dmadrvPeripheralSignal_CRYPTO_DATA0RD
#endif
  }
#endif
#if defined( CRYPTO1 )
  ,
  {
    CRYPTO1,
    CRYPTO1_IRQn,
    CMU_HFBUSCLKEN0_CRYPTO1,
    &p_mbedtls_device_context[1]
#if defined( MBEDTLS_INCLUDE_IO_MODE_DMA )
    ,
    dmadrvPeripheralSignal_CRYPTO1_DATA0WR,
    dmadrvPeripheralSignal_CRYPTO1_DATA0RD
#endif
  }
#endif
};

#if defined(MBEDTLS_CRYPTO_DEVICE_PREEMPTION)

#if defined( MBEDTLS_FREERTOS ) || defined( MBEDTLS_UCOS3 )

__STATIC_INLINE int waitForOwnership(CRYPTODRV_Context_t *pCryptodrvContext )
{
  mbedtls_device_context *pMbedtlsDevice =
    *pCryptodrvContext->device->ppMbedtlsDevice;
  int ret;

  ret = SLPAL_TakeMutex( &pMbedtlsDevice->lock, SLPAL_WAIT_FOREVER );
  EFM_ASSERT(ret == 0);
  
  while (pCryptodrvContext != pMbedtlsDevice->owner)
  {
#if defined( MBEDTLS_DEVICE_YIELD_WHEN_NOT_OWNER )
    pCryptodrvContext->waitOwnership = true;
    EFM_ASSERT(SLPAL_InitCompletion(&pCryptodrvContext->ownership) == 0);
#endif
    ret = SLPAL_GiveMutex( &pMbedtlsDevice->lock );
    EFM_ASSERT(ret == 0);
#if defined( MBEDTLS_DEVICE_YIELD_WHEN_NOT_OWNER )
    ret = SLPAL_WaitForCompletion(&pCryptodrvContext->ownership,
                                  SLPAL_WAIT_FOREVER);
    SLPAL_FreeCompletion(&pCryptodrvContext->ownership);
#endif
    ret = SLPAL_TakeMutex( &pMbedtlsDevice->lock, SLPAL_WAIT_FOREVER );
    EFM_ASSERT(ret == 0);
  }
  return 0;
}

#else  /* #if defined( MBEDTLS_FREERTOS )  || defined( MBEDTLS_UCOS3 ) */

#if defined (CRYPTODRV_CRITICAL_ENTER_FAIL_IF_NOT_OWNER)
__STATIC_INLINE int waitForOwnership(CRYPTODRV_Context_t *pCryptodrvContext)
{
  mbedtls_device_context  *pMbedtlsDevice =
    *pCryptodrvContext->device->ppMbedtlsDevice;
  int ret = SLPAL_TakeMutex( &pMbedtlsDevice->lock,
                             pCryptodrvContext->lockWaitTicks );
  if (pCryptodrvContext != pMbedtlsDevice->owner)
  {
    ret = SLPAL_GiveMutex( &pMbedtlsDevice->lock );
    EFM_ASSERT(ret == 0);
    return MBEDTLS_ECODE_CRYPTODRV_BUSY;
  }
  return ret;
}
#else
__STATIC_INLINE int waitForOwnership(CRYPTODRV_Context_t *pCryptodrvContext)
{
  mbedtls_device_context  *pMbedtlsDevice =
    *pCryptodrvContext->device->ppMbedtlsDevice;
  return SLPAL_TakeMutex( &pMbedtlsDevice->lock, SLPAL_WAIT_FOREVER );
}
#endif

#endif /* #if defined( MBEDTLS_FREERTOS )  || defined( MBEDTLS_UCOS3 ) */

#if defined( MBEDTLS_CRYPTO_PREEMPTION_PRIORITY_SET )
#define CRYPTO_CONTEXT_PRIORITY_GET (pCryptodrvContext->priority)
#else
#define CRYPTO_CONTEXT_PRIORITY_GET SLPAL_ThreadPriorityGet()
#endif

#endif /* #if defined(MBEDTLS_CRYPTO_DEVICE_PREEMPTION) */

/*******************************************************************************
 ******************************   FUNCTIONS   **********************************
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *   Initialize CRYPTO device specifics for given device instance
 *
 * @param devno
 *  CRYPTO device instance number.
 *
 * @return
 *   0 if OK, or MBEDTLS_ERR_DEVICE_NOT_SUPPORTED if device number is invalid.
 ******************************************************************************/
int mbedtls_device_specific_init (unsigned int devno)
{
#if defined( MBEDTLS_DEVICE_YIELD_WHEN_BUSY )

  const CRYPTO_Device_t* pCryptoDevice;
  CRYPTO_TypeDef*        crypto;
  IRQn_Type              irqn;
  
  if (devno >= CRYPTO_COUNT)
    return( MBEDTLS_ERR_DEVICE_NOT_SUPPORTED );
  
  pCryptoDevice = &cryptoDevice[devno];
  crypto        = pCryptoDevice->crypto;
  irqn          = pCryptoDevice->irqn;
  
  /* Enable CRYPTO clock in order to clear interrupts. */
  CRYPTODRV_CLOCK_ENABLE(pCryptoDevice->clk);
  
  crypto->IFC = _CRYPTO_IFC_MASK;
  
  NVIC_ClearPendingIRQ(irqn);
  NVIC_EnableIRQ(irqn);
  NVIC_SetPriority(irqn, SLPAL_CRYPTO_IRQ_PRIORITY);

  CRYPTODRV_CLOCK_DISABLE(pCryptoDevice->clk);

#else
  
  (void) devno;
  
#endif
  
  return( 0 );
}

/***************************************************************************//**
 * @brief
 *   Deinitialize CRYPTO device specifics for given device instance
 *
 * @param devno
 *  CRYPTO device instance number.
 *
 * @return
 *   0 if OK, or MBEDTLS_ERR_DEVICE_NOT_SUPPORTED if device number is invalid.
 ******************************************************************************/
int mbedtls_device_specific_free (unsigned int devno)
{
#if defined( MBEDTLS_DEVICE_YIELD_WHEN_BUSY )

  const CRYPTO_Device_t* pCryptoDevice;
  CRYPTO_TypeDef*        crypto;
  IRQn_Type              irqn;
  
  if (devno >= CRYPTO_COUNT)
    return( MBEDTLS_ERR_DEVICE_NOT_SUPPORTED );
  
  pCryptoDevice = &cryptoDevice[devno];
  crypto        = pCryptoDevice->crypto;
  irqn          = pCryptoDevice->irqn;
  
  /* Enable CRYPTO clock in order to clear interrupts. */
  CRYPTODRV_CLOCK_ENABLE(pCryptoDevice->clk);
  
  crypto->IEN = 0;
  crypto->IFC = _CRYPTO_IFC_MASK;
  
  NVIC_DisableIRQ(irqn);
    
  CRYPTODRV_CLOCK_DISABLE(pCryptoDevice->clk);
  
#else
  
  (void) devno;
  
#endif
  
  return( 0 );
}

/***************************************************************************//**
 * @brief
 *   Select which CRYPTO device instance to use in CRYPTO context.
 *
 * @param pCryptoContext
 *  Pointer to CRYPTODRV context.
 *
 * @param devno
 *  CRYPTO device instance number.
 *
 * @return
 *   0 if OK, or MBEDTLS_ERR_DEVICE_NOT_SUPPORTED if device number is invalid.
 ******************************************************************************/
int cryptodrvSetDeviceInstance(CRYPTODRV_Context_t* pCryptodrvContext,
                               unsigned int         devno)
{
  if (devno >= CRYPTO_COUNT)
    return( MBEDTLS_ERR_DEVICE_NOT_SUPPORTED );

  pCryptodrvContext->device = &cryptoDevice[devno];

#if !defined( MBEDTLS_DEVICE_INIT_INTERNAL_DISABLE )
  /* Initialize the mbedtls device structures internally for backwards
     compatibility. Use MBEDTLS_DEVICE_INIT_INTERNAL_DISABLE to disable
     the internal device initialization. Internal initialization consumes
     more RAM because the device structures are instantiated statically
     inside the mbedTLS library. */
  if (p_mbedtls_device_context[devno] == 0)
  {
    mbedtls_device_init(&_mbedtls_device_ctx[devno]);
    mbedtls_device_set_instance(&_mbedtls_device_ctx[devno], devno);
  }
#endif
  
#if defined( MBEDTLS_DEVICE_YIELD_WHEN_NOT_OWNER )
  pCryptodrvContext->waitOwnership = false;
#endif
  
  return( 0 );
}

/***************************************************************************//**
 * @brief
 *   Set the number of ticks to wait for the device lock.
 *
 * @param pCryptoContext
 *  Pointer to CRYPTODRV context.
 *
 * @param ticks
 *  Number of ticks to wait for the device to become unlocked. If the value is 0
 *  this function will return immediately even if the lock was not acquired.
 *  If the value is -1 this funciton will wait forever (not time out) for the
 *  lock to become available.
 *
 * @return
 *   Always 0 for OK.
 ******************************************************************************/
int cryptodrvSetDeviceLockWaitTicks(CRYPTODRV_Context_t* pCryptodrvContext,
                                    int                  ticks)
{
#if defined( MBEDTLS_CRYPTO_DEVICE_PREEMPTION )
  pCryptodrvContext->lockWaitTicks = ticks;
#else
  (void) pCryptodrvContext;
  (void) ticks;
#endif  
  return( 0 );
}

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
 *   0 if idle and ready for new operation.
 ******************************************************************************/
int CRYPTODRV_CheckState( unsigned int devno )
{
  /* The 'owner' pointer indicates whether someone is already running a
     CRYPTO operation. */
  return p_mbedtls_device_context[devno]->owner ?
    MBEDTLS_ECODE_CRYPTODRV_BUSY : 0;
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
 *  0 if success. Error code if failure.
 *  MBEDTLS_ECODE_CRYPTODRV_BUSY if priority is lower than or equal to the
 *   priority of the running thread.
 */
int CRYPTODRV_Arbitrate (CRYPTODRV_Context_t* pCryptodrvContext)
{
  const CRYPTO_Device_t  *pCryptoDevice  = pCryptodrvContext->device;
  CRYPTO_TypeDef         *crypto         = pCryptoDevice->crypto;
  mbedtls_device_context *pMbedtlsDevice = *pCryptoDevice->ppMbedtlsDevice;
  int                     retval         = 0;
  CRYPTODRV_Context_t    *pCryptoOwner;
  int                     ret;
  unsigned int            challengerPriority = CRYPTO_CONTEXT_PRIORITY_GET;
  
  if (false == RUNNING_AT_INTERRUPT_LEVEL)
  {
    ret = SLPAL_TakeMutex( &pMbedtlsDevice->lock,
                           pCryptodrvContext->lockWaitTicks );
  
    if ( ret == SLPAL_ERROR_TIMEOUT )
    {
      return MBEDTLS_ECODE_CRYPTODRV_BUSY;
    }
  }
  
#if defined( MBEDTLS_ALLOW_ISR_CALLS_IRQ_MASK )
  /* Disable the interrupts that are allowed to call mbedTLS APIs
     from the corresponding ISR. */
  CORE_EnterNvicMask( &pCryptodrvContext->nvicState,
                      &MBEDTLS_ALLOW_ISR_CALLS_IRQ_MASK );
#endif
  
  pCryptoOwner = pMbedtlsDevice->owner;
  
  /* Check if someone is already running a CRYPTO operation. */
  if (pCryptoOwner)
  {
    if ( challengerPriority <= pCryptoOwner->priority )
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
        pCryptoOwner->aborted = true;
      }
      else
      {
        pCryptoOwner->aborted = false;
      }
      
      pCryptoOwner->pContextPreemptor = pCryptodrvContext;

      /* Store the hardware state */
      pCryptoContext = &pCryptoOwner->cryptoContext;

      /* Store CRYPTO register values in context structure. */
      cryptodrvReadCryptoContext(crypto, pCryptoContext);
      
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
      
      retval = 0;
    }
  }
  else
  {
    CRYPTODRV_CLOCK_ENABLE(pCryptodrvContext->device->clk);
  }
  
  if (0 ==  retval)
  {
    pCryptodrvContext->pContextPreempted = pCryptoOwner;
    pMbedtlsDevice->owner = pCryptodrvContext;
    pCryptodrvContext->pContextPreemptor = 0;
    pCryptodrvContext->aborted = false;
    pCryptodrvContext->priority = challengerPriority;
  }

#if defined( MBEDTLS_ALLOW_ISR_CALLS_IRQ_MASK )
  /* Enable the interrupts that are allowed to call mbedTLS APIs
     from the corresponding ISR. */
  CORE_NvicEnableMask( &pCryptodrvContext->nvicState );
#endif
  
  if (false == RUNNING_AT_INTERRUPT_LEVEL)
  {
    SLPAL_GiveMutex( &pMbedtlsDevice->lock );
  }
  
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
int CRYPTODRV_Release (CRYPTODRV_Context_t* pCryptodrvContext)
{
  const CRYPTO_Device_t  *pCryptoDevice  = pCryptodrvContext->device;
  mbedtls_device_context *pMbedtlsDevice = *pCryptoDevice->ppMbedtlsDevice;
  CRYPTODRV_Context_t    *preempted;
  CRYPTODRV_Context_t    *preemptor;
  int                     ret;
  
  if (false == RUNNING_AT_INTERRUPT_LEVEL)
  {
    ret = SLPAL_TakeMutex( &pMbedtlsDevice->lock, SLPAL_WAIT_FOREVER );
    EFM_ASSERT(ret == 0);
  }

#if defined( MBEDTLS_ALLOW_ISR_CALLS_IRQ_MASK )
  /* Disable the interrupts that are allowed to call mbedTLS APIs
     from the corresponding ISR. */
  CORE_EnterNvicMask( &pCryptodrvContext->nvicState,
                      &MBEDTLS_ALLOW_ISR_CALLS_IRQ_MASK );
#endif
  
  preempted =
    (CRYPTODRV_Context_t*) pCryptodrvContext->pContextPreempted;
  preemptor =
    (CRYPTODRV_Context_t*) pCryptodrvContext->pContextPreemptor;

  if ( (0==preempted) && (0==preemptor))
  {
    EFM_ASSERT(pCryptodrvContext==pMbedtlsDevice->owner);
    pMbedtlsDevice->owner = NULL;
    CRYPTODRV_CLOCK_DISABLE(pCryptoDevice->clk);
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
      if (pMbedtlsDevice->owner == pCryptodrvContext)
      {
        /* Load context values into CRYPTO registers. */
        cryptodrvWriteCryptoContext(pCryptoDevice->crypto,
                                    &preempted->cryptoContext);
        /* Set owner pointer to preempted context. */
        pMbedtlsDevice->owner = preempted;
#if defined( MBEDTLS_DEVICE_YIELD_WHEN_NOT_OWNER )
        if (preempted->waitOwnership)
        {
          preempted->waitOwnership = false;
          int status = SLPAL_Complete(&preempted->ownership);
          EFM_ASSERT(status == 0);
        }
#endif
      }
    }
  }
  
#if defined( MBEDTLS_ALLOW_ISR_CALLS_IRQ_MASK )
  /* Enable the interrupts that are allowed to call mbedTLS APIs
     from the corresponding ISR. */
  CORE_NvicEnableMask( &pCryptodrvContext->nvicState );
#endif
  
  if (false == RUNNING_AT_INTERRUPT_LEVEL)
  {
    SLPAL_GiveMutex( &pMbedtlsDevice->lock );
  }
  
  return( 0 );
}

/***************************************************************************//**
 * @brief
 *  Enter a critical CRYPTO region.
 *
 * @details
 *  This function enters a critical section of a CRYPTO operation by locking
 *  the CRYPTO device and disabling interrupts that are allowed to call the
 *  mbedTLS API from the corresponding ISRs.
 *  In multi-threaded OSes, if a thread which is not the owner (preempted
 *  context) of CRYPTO tries to enter the critical section, it will loop until
 *  the ownership is given back from the preemptor.
 *
 * @return
 *  0. If CRYPTODRV_CRITICAL_ENTER_FAIL_IF_NOT_OWNER is defined
 *  MBEDTLS_ECODE_CRYPTODRV_BUSY will be returned if the caller is not the owner.
*/
int CRYPTODRV_EnterCriticalRegion (CRYPTODRV_Context_t* pCryptodrvContext)
{
  int ret = 0;
  
  if (false == RUNNING_AT_INTERRUPT_LEVEL)
  {
    ret = waitForOwnership( pCryptodrvContext );
  }
  
#if defined( MBEDTLS_ALLOW_ISR_CALLS_IRQ_MASK )
  if (ret == 0)
  {
    /* Disable the interrupts that are allowed to call mbedTLS APIs
       from the corresponding ISR. */
    CORE_EnterNvicMask( &pCryptodrvContext->nvicState,
                        &MBEDTLS_ALLOW_ISR_CALLS_IRQ_MASK );
  }
#endif

  return( ret );
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
 *  0
 */
int CRYPTODRV_ExitCriticalRegion (CRYPTODRV_Context_t* pCryptodrvContext)
{
  mbedtls_device_context *pMbedtlsDevice =
    *pCryptodrvContext->device->ppMbedtlsDevice;
  
#if defined( MBEDTLS_ALLOW_ISR_CALLS_IRQ_MASK )
  /* Enable the interrupts that are allowed to call mbedTLS APIs
     from the corresponding ISR. */
  CORE_NvicEnableMask( &pCryptodrvContext->nvicState );
#endif
  
  if (false == RUNNING_AT_INTERRUPT_LEVEL)
  {
    SLPAL_GiveMutex( &pMbedtlsDevice->lock );
  }
  
  return( 0 );
}

#endif /* #if defined( MBEDTLS_CRYPTO_DEVICE_PREEMPTION ) */

#if defined(MBEDTLS_DEVICE_YIELD_WHEN_BUSY)
                              
/***************************************************************************//**
 * @brief
 *  Interrupt service routine for CRYPTO module instances.
 *
 * @details
 *  CRYPTO_IRQHandler, CRYPTO0_IRQHandler, etc are the first functions to be
 *  called when an interrupt from the respective CRYPTO instance is being
 *  serviced by the MCU. The function cryptoIrqHandlerGeneric is called with
 *  a pointer to the respective CRYPTO unit, and it will clear the interrupt
 *  and signal the event.
 *
 * @return
 *  N/A
 */
void cryptoIrqHandlerGeneric( const CRYPTO_Device_t* cryptoDevice )
{
  CRYPTO_TypeDef* crypto = cryptoDevice->crypto;
  uint32_t flags = crypto->IF;

  while (flags)
  {
    crypto->IFC = flags;
    NVIC_ClearPendingIRQ(cryptoDevice->irqn);
    
    if (flags & CRYPTO_IF_SEQDONE)
    {
      mbedtls_device_context  *pMbedtlsDevice = *cryptoDevice->ppMbedtlsDevice;
      int status = SLPAL_Complete(&pMbedtlsDevice->operation);
      EFM_ASSERT(status == 0);
    }
    if (CMU->HFBUSCLKEN0 & cryptoDevice->clk)
      flags = crypto->IF;
    else
      flags = 0;
  }
}

#if defined(CRYPTO)
void CRYPTO_IRQHandler(void)
{
  SLPAL_IsrEnter();
  cryptoIrqHandlerGeneric( &cryptoDevice[0] );
  SLPAL_IsrExit();
}
#endif

#if defined(CRYPTO0)
void CRYPTO0_IRQHandler(void)
{
  SLPAL_IsrEnter();
  cryptoIrqHandlerGeneric( &cryptoDevice[0] );
  SLPAL_IsrExit();
}
#endif

#if defined(CRYPTO1)
void CRYPTO1_IRQHandler(void)
{
  SLPAL_IsrEnter();
  cryptoIrqHandlerGeneric( &cryptoDevice[1] );
  SLPAL_IsrExit();
}
#endif

#endif /* #if defined(MBEDTLS_DEVICE_YIELD_WHEN_BUSY) */

#endif /* #if defined(CRYPTO_COUNT) && (CRYPTO_COUNT > 0) */
