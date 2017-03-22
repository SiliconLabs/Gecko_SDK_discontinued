/*
 *  Platform Abstraction Layer interface for uC/OS-III.
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

#ifndef MBEDTLS_SLPAL_UCOS3_H
#define MBEDTLS_SLPAL_UCOS3_H

#if !defined(MBEDTLS_CONFIG_FILE)
#include "config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined( MBEDTLS_UCOS3 )

#include "slpal_common.h"
#include "includes.h"

/*******************************************************************************
 ******************************   Defines     **********************************
 ******************************************************************************/

/* In order to wait forever in blocking functions the user can pass the
   following value.  */
#define SLPAL_WAIT_FOREVER  (0)
/* In order to return immediately in blocking functions the user can pass the
   following value.  */
#define SLPAL_NON_BLOCKING  (-1)

/* Priority to use for CRYPTO IRQ */
#if defined(MBEDTLS_CRYPTO_IRQ_PRIORITY)
#define SLPAL_CRYPTO_IRQ_PRIORITY MBEDTLS_CRYPTO_IRQ_PRIORITY
#else
#define SLPAL_CRYPTO_IRQ_PRIORITY ( 0 )
#endif

/* The lowest priority of uC/OS-III is OS_CFG_PRIO_MAX, and 0 is highest.
   In SLPAL_ThreadPriorityGet we inverse the order of priorities.
   I.e. 0 is lowest in mbedtls crypto preemption. */
#define UCOS3_LOWEST_TASK_PRIO                    (OS_CFG_PRIO_MAX-2)

/* Max thread priority of SLPAL interface, corresponds to the lowest
   UCOS3 priority we allow inside this abstractaion module. */
#define SLPAL_MAX_PRIORITY (UCOS3_LOWEST_TASK_PRIO)

/*******************************************************************************
 ******************************   TYPEDEFS    **********************************
 ******************************************************************************/

/* Completion type used to wait for and signal end of operation. */
typedef OS_SEM   SLPAL_Completion_t;
/* Mutex object used for mutual exclusion, e.g. locking resources.
   Shall support priority inheritance. */
typedef OS_MUTEX SLPAL_Mutex_t;

/*******************************************************************************
 ******************************   FUNCTIONS   **********************************
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *  Enter a critical region.
 *
 * @details
 *  This function enters a critical region using the standard critical region
 *  enter method of uC/OS-III. SLPAL_CriticalEnter returns the current IRQ state
 *  which subsequently should be passed to SLPAL_CriticalExit in order to
 *  restore the IRQ state.
 *
 * @return
 *   IRQ state.
 ******************************************************************************/
__STATIC_INLINE SLPAL_irqState_t SLPAL_CriticalEnter(void)
{
  /* The CPU_CRITICAL_ENTER() macro requires a variable called cpu_sr which is
     set to the return value of __get_PRIMASK(). */
  CPU_SR  cpu_sr;
  CPU_CRITICAL_ENTER();
  return (SLPAL_irqState_t) cpu_sr;
}

/***************************************************************************//**
 * @brief
 *  Exit a critical region. 
 *
 * @details
 *  This function enters a critical region using the standard critical region
 *  exit method of uC/OS-III.  SLPAL_CriticalEnter returns the current IRQ state
 *  which subsequently should be passed to SLPAL_CriticalExit in order to
 *  restore the IRQ state.
 *
 * @param irqState
 *  IRQ state.
 ******************************************************************************/
__STATIC_INLINE void SLPAL_CriticalExit(SLPAL_irqState_t irqState)
{
  /* The CPU_CRITICAL_EXIT() macro requires a variable called cpu_sr which is
     given to __set_PRIMASK(cpu_sr). I.e. cpu_sr is the same value as returned
     from CPU_CRITICAL_ENTER.
  */
  CPU_SR  cpu_sr = (CPU_SR) irqState;
  CPU_CRITICAL_EXIT();
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
  OSIntEnter();
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
  OSIntExit();
}

/***************************************************************************//**
 * @brief
 *  Get thread priority of calling thread.
 *
 * @details
 *  This function returns the uC/OS-III task priority of the calling task.
 *
 * @return
 *   Thread priority.
 ******************************************************************************/
__STATIC_INLINE unsigned long SLPAL_ThreadPriorityGet(void)
{
  return  (unsigned long) (UCOS3_LOWEST_TASK_PRIO - OSTCBCurPtr->Prio);
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
  OS_ERR err;
  OSSemCreate((OS_SEM *) pComp, NULL, (OS_SEM_CTR) 0, &err);
  EFM_ASSERT(err == OS_ERR_NONE);
  return (err == OS_ERR_NONE ? 0 : SLPAL_ERROR_OS_SPECIFIC);
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
  OS_ERR err;
  OSSemDel((OS_SEM *) pComp, OS_OPT_DEL_ALWAYS, &err);
  EFM_ASSERT(err == OS_ERR_NONE);
  return (err == OS_ERR_NONE ? 0 : SLPAL_ERROR_OS_SPECIFIC);
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
  OS_ERR err;
  int ret;
  OSSemPend((OS_SEM *) pComp,
            (OS_TICK) ticks,
            (OS_OPT) (ticks==SLPAL_NON_BLOCKING ?
                      OS_OPT_PEND_NON_BLOCKING : OS_OPT_PEND_BLOCKING),
            NULL,
            &err);
  if (err == OS_ERR_NONE)
  {
    ret = 0;
  }
  else
  {
    if ( (err == OS_ERR_TIMEOUT) || (err == OS_ERR_PEND_WOULD_BLOCK) )
    {
      ret = SLPAL_ERROR_TIMEOUT;
    }
    else
    {
      /* Assert that no other error codes occur. */
      EFM_ASSERT( false );
      ret = SLPAL_ERROR_OS_SPECIFIC;
    }
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
 *   0 for success, SLPAL_ERROR_OS_SPECIFIC for error.
********************************************************************************/
__STATIC_INLINE int SLPAL_Complete(SLPAL_Completion_t* pComp)
{
  OS_ERR err;
  OSSemPost((OS_SEM*) pComp, (OS_OPT) OS_OPT_POST_1, &err);
  EFM_ASSERT(err == OS_ERR_NONE); 
  return (err == OS_ERR_NONE ? 0 : SLPAL_ERROR_OS_SPECIFIC);
}

/*******************************************************************************
 * @brief
 *  Initialize a mutex object with support for priority inheritance.
 *
 * @param pSem
 *  Pointer to an SLPAL_Mutex_t object allocated by the user.
 *
 * @return
 *  0 for success, SLPAL_ERROR_OS_SPECIFIC for error.
*******************************************************************************/
__STATIC_INLINE int SLPAL_InitMutex(SLPAL_Mutex_t *pMutex)
{
  OS_ERR err;
  OSMutexCreate((OS_MUTEX *) pMutex, NULL, &err);
  EFM_ASSERT(err == OS_ERR_NONE);
  return (err == OS_ERR_NONE ? 0 : SLPAL_ERROR_OS_SPECIFIC);
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
  OS_ERR err;
  OSMutexDel((OS_MUTEX *) pMutex, OS_OPT_DEL_ALWAYS, &err);
  EFM_ASSERT(err == OS_ERR_NONE);
  return (err == OS_ERR_NONE ? 0 : SLPAL_ERROR_OS_SPECIFIC);
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
  OS_ERR err;
  int ret;
  OSMutexPend((OS_MUTEX *) pMutex,
              (OS_TICK) ticks,
              (OS_OPT) (ticks==SLPAL_NON_BLOCKING ?
                        OS_OPT_PEND_NON_BLOCKING : OS_OPT_PEND_BLOCKING),
              NULL,
              &err);
  if (err == OS_ERR_NONE)
  {
    ret = 0;
  }
  else
  {
    if ( (err == OS_ERR_TIMEOUT) || (err == OS_ERR_PEND_WOULD_BLOCK) )
    {
      ret = SLPAL_ERROR_TIMEOUT;
    }
    else
    {
      /* Assert that no other error codes occur. */
      EFM_ASSERT( false );
      ret = SLPAL_ERROR_OS_SPECIFIC;
    }
  }
  return ret;
}

/*******************************************************************************
 * @brief
 *  Give mutex.
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
  OS_ERR err;
  OSMutexPost((OS_MUTEX*) pMutex, (OS_OPT) OS_OPT_POST_NONE, &err);
  EFM_ASSERT(err == OS_ERR_NONE); 
  return (err == OS_ERR_NONE ? 0 : SLPAL_ERROR_OS_SPECIFIC);
}

#endif /* MBEDTLS_UCOS3 */

#endif /* MBEDTLS_SLPAL_UCOS3_H */
