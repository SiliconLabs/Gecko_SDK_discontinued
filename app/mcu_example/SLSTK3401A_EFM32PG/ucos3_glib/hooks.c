/**************************************************************************//**
 * @file hooks.c
 * @brief uC/OS-III application specific hooks
 * @version 5.1.2
 *
 ******************************************************************************
 * @section License
 * <b>Copyright 2016 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include <os.h>
#include "hooks.h"
#include "em_emu.h"

/**************************************************************************//**
 * @brief
 *   Set all application hooks
 *****************************************************************************/
void App_OS_SetAllHooks(void)
{
  CPU_SR_ALLOC();
  CPU_CRITICAL_ENTER();
  OS_AppIdleTaskHookPtr   = App_OS_IdleTaskHook;
  OS_AppRedzoneHitHookPtr = App_OS_RedzoneHitHook;
  CPU_CRITICAL_EXIT();
}

/**************************************************************************//**
 * @brief
 *   Clear all application hooks
 *****************************************************************************/
void App_OS_ClrAllHooks(void)
{
  CPU_SR_ALLOC();
  CPU_CRITICAL_ENTER();
  OS_AppIdleTaskHookPtr   = (OS_APP_HOOK_VOID)0;
  OS_AppRedzoneHitHookPtr = (OS_APP_HOOK_TCB)0;
  CPU_CRITICAL_EXIT();
}

/**************************************************************************//**
 * @brief
 *   Application idle task hook
 *
 * @details
 *   This function is called by the idle task. This hook has been added to
 *   allow you to do such things as STOP the CPU to conserve power.
 *****************************************************************************/
void App_OS_IdleTaskHook(void)
{
  EMU_EnterEM1();
}

/**************************************************************************//**
 * @brief
 *   Application redzone hit hook
 *
 * @details
 *   This function is called when a task's stack overflowed.
 *
 * @param[in] p_tcb
 *   pointer to the task control block of the offending task. NULL if ISR.
 *****************************************************************************/
void App_OS_RedzoneHitHook(OS_TCB *p_tcb)
{
  (void)&p_tcb;
  CPU_SW_EXCEPTION(;);
}
