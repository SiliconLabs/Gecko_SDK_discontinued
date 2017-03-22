/*
*********************************************************************************************************
*                                          uC/OS-III Trace
*                                      The RTOS Event Analyzer
*
*                         (c) Copyright 2004-2016; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*
*               uC/OS-III Trace is provided in source form to registered licensees ONLY.  
*               It is illegal to distribute this source code to any third party unless you 
*               receive written permission by an authorized Micrium representative.  
*               Knowledge of the source code may NOT be used to develop a similar product.
*
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*
*               You can contact us at www.micrium.com.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                UC/OS-III TRACE RECORDER AND TRIGGERS
*                                         CONFIGURATION FILE
*
*                                              TEMPLATE
*
* File          : os_cfg_trace.h
* Version       : V3.06.00
* Programmer(s) : JPB
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                               MODULE
*********************************************************************************************************
*/

#ifndef   OS_CFG_TRACE_H
#define   OS_CFG_TRACE_H


/*
*********************************************************************************************************
*                                                INCLUDE
*********************************************************************************************************
*/

#include  <cpu.h>


/*
*********************************************************************************************************
*                                    UC/OS-III TRACE TRIGGERS MODULE
*
* Note(s) : The uC/OS-III Trace Triggering mechanism requires the hardware platform 
*           to be supported by uC/Probe.
*
*           If your platform is uC/Probe-ready, then enable this module by setting
*           the macro below to 1.
*********************************************************************************************************
*/

#define  OS_CFG_TRACE_TRIG_EN                     0u

#if (defined(OS_CFG_TRACE_TRIG_EN) && (OS_CFG_TRACE_TRIG_EN > 0u))


/*
*********************************************************************************************************
*                                  UC/OS-III TRACE TRIGGERS SETTINGS
*********************************************************************************************************
*/

#define  OS_CFG_TRACE_TRIG_MAX_TRIGS             16u
#define  OS_CFG_TRACE_TRIG_NAME_LEN              32u


/*
*********************************************************************************************************
*                               UC/OS-III TRACE TRIGGERS TASK SETTINGS
*********************************************************************************************************
*/

#define  OS_CFG_TRACE_TRIG_TASK_STK_SIZE        128u
#define  OS_CFG_TRACE_TRIG_TASK_PRIO             20u


/*
*********************************************************************************************************
*                                     UC/OS-III TRACE TRIGGER IDS
*
* Note(s) : First you create a series of unique IDs for each of your trigger points. 
*           In this example OS_TRACE_CFG_TRIG_ID_SW1 is the trigger point executed when 
*           you press the board's switch 1.
*********************************************************************************************************
*/

#define  OS_CFG_TRACE_TRIG_ID_SW1              1234u
#define  OS_CFG_TRACE_TRIG_ID_SW2              1235u
#define  OS_CFG_TRACE_TRIG_ID_SW3              1236u

#endif                                                          /* End of uC/OS-III Trace Triggering Module.            */

/*
*********************************************************************************************************
*                                             MODULE END
*********************************************************************************************************
*/

#endif                                                          /* End of trace cfg module include.                     */
