/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                          (c) Copyright 2003-2010; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                         uC/OS-II example code
*                                      Master include header file
*
*                                   Silicon Labs EFM32 (EFM32GG990F1024)
*                                              with the
*                                   Silicon Labs DK3750 Development Kit
*
* @file   includes.h
* @brief
* @version 5.1.2
******************************************************************************
* @section License
* <b>Copyright 2015 Silicon Labs, Inc. http://www.silabs.com</b>
*******************************************************************************
*
* This file is licensed under the Silabs License Agreement. See the file
* "Silabs_License_Agreement.txt" for details. Before using this software for
* any purpose, you must agree to the terms of that agreement.
*
******************************************************************************/
#ifndef  __INCLUDES_H
#define  __INCLUDES_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef  OS_MASTER_FILE

/*
*********************************************************************************************************
*                                          STANDARD LIBRARIES
*********************************************************************************************************
*/
#include  <stdarg.h>
#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>


/*
*********************************************************************************************************
*                                              LIBRARIES
*********************************************************************************************************
*/
#include  <cpu.h>
#include  <lib_def.h>
#include  <lib_ascii.h>
#include  <lib_mem.h>


/*
*********************************************************************************************************
*                                                  OS
*********************************************************************************************************
*/
#include  <ucos_ii.h>


/*
*********************************************************************************************************
*                                               APP/BSP
*********************************************************************************************************
*/
#include  "app_task_one.h"
#include  "app_task_two.h"
#include  "app_task_three.h"
#include  "app_cfg.h"
#include  "retargettft.h"


/*
*********************************************************************************************************
*                                      Silicon Labs HEADER FILES
*********************************************************************************************************
*/
#include "bsp.h"
#include "bsp_trace.h"
#include "bsp-uart.h"

/*
*********************************************************************************************************
*                                      Silicon Labs LIBRARY FILES
*********************************************************************************************************
*/
#include <em_cmu.h>
#include <em_emu.h>
#include <em_gpio.h>
#include <em_system.h>
#include <em_usart.h>
#include <em_chip.h>


/*
*********************************************************************************************************
*                                       Silicon Labs DRIVER FILES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                          MACRO DEFINITIONS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                          GLOBAL VARIABLES
*********************************************************************************************************
*/

/* declaration of global queue object for inter-task communication */
extern OS_EVENT *pSerialQueObj;


#endif /* end of OS_MASTER_FILE */

#ifdef __cplusplus
}
#endif

#endif /* end of __INCLUDES_H (do not include header files after this!) */
