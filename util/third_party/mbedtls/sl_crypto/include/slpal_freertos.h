/*
 *  Platform Abstraction Layer interface for FreeRTOS.
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

#ifndef MBEDTLS_SLPAL_FREERTOS_H
#define MBEDTLS_SLPAL_FREERTOS_H

#if !defined(MBEDTLS_CONFIG_FILE)
#include "config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined( MBEDTLS_FREERTOS )

#include "slpal_common.h"
#include "em_assert.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include <stdbool.h>

/*******************************************************************************
 ******************************   Defines     **********************************
 ******************************************************************************/

/* In order to wait forever in blocking functions the user can pass the
   following value.  */
#define SLPAL_WAIT_FOREVER  ((int)portMAX_DELAY)
/* In order to return immediately in blocking functions the user can pass the
   following value.  */
#define SLPAL_NON_BLOCKING  (0)

#if defined(MBEDTLS_CRYPTO_IRQ_PRIORITY)
#define SLPAL_CRYPTO_IRQ_PRIORITY MBEDTLS_CRYPTO_IRQ_PRIORITY
#else
#define SLPAL_CRYPTO_IRQ_PRIORITY ( configMAX_SYSCALL_INTERRUPT_PRIORITY >> ( 8U - __NVIC_PRIO_BITS ) )
#endif

#if  ( SLPAL_CRYPTO_IRQ_PRIORITY < ( configMAX_SYSCALL_INTERRUPT_PRIORITY >> ( 8U - __NVIC_PRIO_BITS ) ) )
#error CRYPTO IRQ priority should be numerically higher than or equal to the syscall interrupt.
#endif

/* Max thread priority of system */
#define SLPAL_MAX_PRIORITY (configMAX_PRIORITIES-1)

/* Max count value of semaphores */
#define SEM_COUNT_MAX (65535)

/*******************************************************************************
 ******************************   TYPEDEFS    **********************************
 ******************************************************************************/

/* Completion object used to wait for and signal end of an operation. */
typedef SemaphoreHandle_t SLPAL_Completion_t;
/* Mutex object used for mutual exclusion, e.g. locking resources.
   Shall support priority inheritance. */
typedef SemaphoreHandle_t SLPAL_Mutex_t;

/*******************************************************************************
 ******************************   FUNCTIONS   **********************************
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *  Enter a critical region.
 *
 * @details
 *  This function enters a critical region using the standard critical region
 *  enter method of FreeRTOS. SLPAL_CriticalEnter returns the current IRQ state,
 *  which subsequently should be passed to SLPAL_CriticalExit in order to
 *  restore the IRQ state.
 *
 * @return
 *   IRQ state.
 ******************************************************************************/
__STATIC_INLINE SLPAL_irqState_t SLPAL_CriticalEnter(void)
{
  SLPAL_irqState_t irqState;
  
  if (RUNNING_AT_INTERRUPT_LEVEL)
  {
    irqState = taskENTER_CRITICAL_FROM_ISR();
  }
  else
  {
    irqState = __get_BASEPRI();
    taskENTER_CRITICAL();
  }
  
  return irqState;
}

/***************************************************************************//**
 * @brief
 *  Exit a critical region. 
 *
 * @details
 *  This function enters a critical region using the standard critical region
 *  exit method of FreeRTOS.  SLPAL_CriticalEnter returns the current IRQ state,
 *  which subsequently should be passed to SLPAL_CriticalExit in order to
 *  restore the IRQ state.
 *
 * @param irqState
 *  IRQ state.
 ******************************************************************************/
__STATIC_INLINE void SLPAL_CriticalExit(SLPAL_irqState_t irqState)
{
  if (RUNNING_AT_INTERRUPT_LEVEL)
  {
    taskEXIT_CRITICAL_FROM_ISR(irqState);
  }
  else
  {
    (void) irqState;
    taskEXIT_CRITICAL();
  }
}

/***************************************************************************//**
 * @brief
 *  Enter an ISR.
 *
 * @details
 *  This function informs the OS that we have entered an ISR. This function
 *  should be called first thing in all ISRs.
 ******************************************************************************/
__STATIC_INLINE void SLPAL_IsrEnter(void)
{
}

/***************************************************************************//**
 * @brief
 *  Exit an ISR.
 *
 * @details
 *  This function informs the OS that we are about to exit an ISR. This function
 *  should be called last in all ISRs.
 ******************************************************************************/
__STATIC_INLINE void SLPAL_IsrExit(void)
{
}

/***************************************************************************//**
 * @brief
 *  Get thread priority of calling thread.
 *
 * @details
 *  This function returns the FreeRTOS task priority of the calling task.
 *
 * @return
 *   Thread priority.
 ******************************************************************************/
__STATIC_INLINE unsigned long SLPAL_ThreadPriorityGet(void)
{
  return  (unsigned long)uxTaskPriorityGet(NULL);
}

/*******************************************************************************
 * @brief
 *  Initialize a completion object.
 *
 * @param pComp
 *  Pointer to an SLPAL_Completion_t object allocated by the user.
 *
 * @return
 *  0 for success, SLPAL_ERROR_OS_SPECIFIC for error.
*******************************************************************************/
__STATIC_INLINE int SLPAL_InitCompletion(SLPAL_Completion_t *pComp)
{
  *pComp = (SLPAL_Completion_t) xSemaphoreCreateBinary();
  EFM_ASSERT(*pComp != NULL);
  return (*pComp == NULL ? SLPAL_ERROR_OS_SPECIFIC : 0);
}

/*******************************************************************************
 * @brief
 *  Free a completion object.
 *
 * @param pComp
 *  Pointer to an SLPAL_Completion_t object.
 *
 * @return
 *  0 for success, SLPAL_ERROR_OS_SPECIFIC for error.
*******************************************************************************/
__STATIC_INLINE int SLPAL_FreeCompletion(SLPAL_Completion_t *pComp)
{
  (void) pComp;
  /*
    Removed call to
    vSemaphoreDelete( (SemaphoreHandle_t) *pComp );

    Semaphores can not be deleted with default heap management of the current
    port. The following comment is from the vPortFree function of FreeRTOS
    which is called from vSemaphoreDelete (and subsequently vQueueDelete).
  */
	/* Memory cannot be freed using this scheme.  See heap_2.c, heap_3.c and
     heap_4.c for alternative implementations, and the memory management pages of
     http://www.FreeRTOS.org for more information. */
  
  return ( 0 );
}

/*******************************************************************************
 * @brief
 *  Wait for completion event.
 *
 * @param[in]  pComp
 *  Pointer to completion object which must be initialized by calling
 *  SLPAL_CompletionInit before calling this function.
 * 
 * @param[in]  ticks
 *  Ticks to wait for the completion.
 *  Pass a value of SLPAL_WAIT_FOREVER in order to wait forever. 
 *  Pass a value of SLPAL_NON_BLOCKING in order to return immediately.
 *  
 * @return
 *  0 if success, and SLPAL_ERROR_TIMEOUT if the completion was not completed
 *  within the timeout.
********************************************************************************/
__STATIC_INLINE int SLPAL_WaitForCompletion(SLPAL_Completion_t *pComp, int ticks)
{
  int        ret;
  BaseType_t status =
    xSemaphoreTake( (SemaphoreHandle_t) *pComp, (TickType_t) ticks );
  if (status == pdTRUE)
  {
    ret = 0;
  }
  else
  {
    ret = SLPAL_ERROR_TIMEOUT;
  }
  return ret;
}

/*******************************************************************************
 * @brief
 *  Signal completion.
 *
 * @param[in]  pComp
 *  Pointer to completion object which must be initialized by calling
 *  SLPAL_CompletionInit before calling this function.
 *  
 * @return
 *  0 for success, SLPAL_ERROR_OS_SPECIFIC for error.
********************************************************************************/
__STATIC_INLINE int SLPAL_Complete(SLPAL_Completion_t* pComp)
{
  BaseType_t status;
  if (RUNNING_AT_INTERRUPT_LEVEL)
  {
    BaseType_t HigherPriorityTaskWoken;
    status = xSemaphoreGiveFromISR( (SemaphoreHandle_t) *pComp,
                                    &HigherPriorityTaskWoken );
  }
  else
  {
    status = xSemaphoreGive( (SemaphoreHandle_t) *pComp );
  }
  EFM_ASSERT(status == pdTRUE);
  return (status == pdTRUE ? 0 : SLPAL_ERROR_OS_SPECIFIC );
}

/*******************************************************************************
 * @brief
 *  Initialize a mutex object with support for priority inheritance.
 *
 * @param pMutex
 *  Pointer to an SLPAL_Mutex_t object allocated by the user.
 *
 * @return
 *  0 for success, SLPAL_ERROR_OS_SPECIFIC for error.
*******************************************************************************/
__STATIC_INLINE int SLPAL_InitMutex(SLPAL_Mutex_t *pMutex)
{
  *pMutex = (SLPAL_Mutex_t) xSemaphoreCreateMutex();
  EFM_ASSERT(*pMutex != NULL);
  return (*pMutex == NULL ? SLPAL_ERROR_OS_SPECIFIC : 0);
}

/*******************************************************************************
 * @brief
 *  Free a mutex object.
 *
 * @param pMutex
 *  Pointer to an SLPAL_Mutex_t object.
 *
 * @return
 *  0 for success, SLPAL_ERROR_OS_SPECIFIC for error.
*******************************************************************************/
__STATIC_INLINE int SLPAL_FreeMutex(SLPAL_Mutex_t *pMutex)
{
  (void) pMutex;
  /*
    Removed call to
    vSemaphoreDelete( (SemaphoreHandle_t) *pComp );

    Mutex semaphores can not be deleted with default heap management of the
    current port. The following comment is from the vPortFree function of
    FreeRTOS which is called from vSemaphoreDelete (and subsequently
    vQueueDelete).
  */
	/* Memory cannot be freed using this scheme.  See heap_2.c, heap_3.c and
     heap_4.c for alternative implementations, and the memory management pages
     of http://www.FreeRTOS.org for more information. */
  
  return ( 0 );
}

/*******************************************************************************
 * @brief
 *  Take (and optionally wait for) a mutex to be given.
 *
 * @param[in]  pMutex
 *  Pointer to mutex object which must be initialized by calling
 *  SLPAL_MutexInit before calling this function.
 * 
 * @param[in]  ticks
 *  Ticks to wait for the mutex.
 *  Pass a value of SLPAL_WAIT_FOREVER in order to wait forever. 
 *  Pass a value of SLPAL_NON_BLOCKING in order to return immediately.
 *  
 * @return
 *  0 if success, and SLPAL_ERROR_TIMEOUT if the mutex was not given
 *  within the timeout.
********************************************************************************/
__STATIC_INLINE int SLPAL_TakeMutex(SLPAL_Mutex_t *pMutex, int ticks)
{
  int        ret;
  BaseType_t status =
    xSemaphoreTake( (SemaphoreHandle_t) *pMutex, (TickType_t) ticks );
  if (status == pdTRUE)
  {
    ret = 0;
  }
  else
  {
    ret = SLPAL_ERROR_TIMEOUT;
  }
  return ret;
}

/*******************************************************************************
 * @brief
 *  Give a mutex.
 *
 * @param[in]  pMutex
 *  Pointer to mutex object which must be initialized by calling
 *  SLPAL_MutexInit before calling this function.
 *  
 * @return
 *   0 for success, SLPAL_ERROR_OS_SPECIFIC for error.
********************************************************************************/
__STATIC_INLINE int SLPAL_GiveMutex(SLPAL_Mutex_t* pMutex)
{
  BaseType_t status;
  if (RUNNING_AT_INTERRUPT_LEVEL)
  {
    BaseType_t HigherPriorityTaskWoken;
    status = xSemaphoreGiveFromISR( (SemaphoreHandle_t) *pMutex,
                                    &HigherPriorityTaskWoken );
  }
  else
  {
    status = xSemaphoreGive( (SemaphoreHandle_t) *pMutex );
  }
  EFM_ASSERT(status == pdTRUE);
  return (status == pdTRUE ? 0 : SLPAL_ERROR_OS_SPECIFIC );
}

#endif /* MBEDTLS_FREERTOS */

#endif /* MBEDTLS_SLPAL_FREERTOS_H */
