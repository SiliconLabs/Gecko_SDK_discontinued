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
*                                  Application configuration header file
*
*                                   Silicon Labs EFM32 (EFM32WG990F256)
*                                              with the
*                               Silicon Labs EFM32WG990F256-STK Starter Kit
*
* @file   app_cfg.h
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
#ifndef  __APP_CFG_H
#define  __APP_CFG_H

#ifdef __cplusplus
extern "C" {
#endif

/*
*********************************************************************************************************
*                                         TASKS PRIORITIES
*********************************************************************************************************
*/
#define  OS_TASK_TMR_PRIO       (OS_LOWEST_PRIO-2U)

#define  APP_CFG_TASK_START_PRIO                20U /* lowest priority task */
#define  APP_CFG_TASK_ONE_PRIO                   6U
#define  APP_CFG_TASK_TWO_PRIO                   4U /* highest priority task */
#define  APP_CFG_TASK_THREE_PRIO                 5U


/*
*********************************************************************************************************
*                                         TASK STACK SIZES
*                          Size of the task stacks (# of OS_STK entries)
*********************************************************************************************************
*/
#define  APP_CFG_TASK_START_STK_SIZE           512U
#define  APP_CFG_TASK_ONE_STK_SIZE             512U
#define  APP_CFG_TASK_TWO_STK_SIZE             512U
#define  APP_CFG_TASK_THREE_STK_SIZE           512U

#ifdef __cplusplus
}
#endif

#endif /* end of __APP_CFG_MODULE_H */
