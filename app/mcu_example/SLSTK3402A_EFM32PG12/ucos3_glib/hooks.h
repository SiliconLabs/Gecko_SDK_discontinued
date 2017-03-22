/**************************************************************************//**
 * @file hooks.h
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

#ifndef HOOKS_H
#define HOOKS_H

#include <os.h>

void App_OS_SetAllHooks(void);
void App_OS_ClrAllHooks(void);
void App_OS_IdleTaskHook(void);
void App_OS_RedzoneHitHook(OS_TCB  *p_tcb);

#endif
