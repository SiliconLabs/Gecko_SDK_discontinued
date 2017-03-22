/**************************************************************************//**
 * @file bsp_os.c
 * @brief uC/OS-III BSP source file
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

#include <cpu.h>
#include <os.h>

#include "em_cmu.h"

/**************************************************************************//**
 * @brief
 *   Initialize the os tick interrupt.
 *****************************************************************************/
void  BSP_OSTickInit (void)
{
  OS_CPU_SysTickInitFreq(SystemCoreClockGet());
}

/**************************************************************************//**
 * @brief
 *   Disable the os tick interrupt
 *****************************************************************************/
void BSP_OSTickDisable(void)
{
  SysTick->CTRL = 0x0000000;
}
