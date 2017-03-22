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
*                            UC/OS-III TRACE TRIGGERS CONFIGURATION FILE
*
*                                              TEMPLATE
*
* File          : os_cfg_trace.c
* Version       : V3.06.00
* Programmer(s) : JPB
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                           INCLUDE FILES
*********************************************************************************************************
*/

#include  "os_cfg_trace.h"


/*
*********************************************************************************************************
*                                      UC/TRACE TRIGGERS MODULE
*
* Note(s) : The uC/OS-III Trace Triggering mechanism requires the hardware platform  
*           to be supported by uC/Probe.
*
*           If your platform is uC/Probe-ready, then enable this module by setting
*           the macro below to 1.
*********************************************************************************************************
*/

#if (defined(OS_CFG_TRACE_TRIG_EN) && (OS_CFG_TRACE_TRIG_EN > 0u))


/*
*********************************************************************************************************
*                                           INCLUDE FILES
*********************************************************************************************************
*/

#include  <os_trace_trig.h>


/*
*********************************************************************************************************
*                             UC/OS-III TRACE TRIGGERS CONFIGURATION TABLE
*
* Example : { [Trigger ID], [Trigger Source Name], [Max # Files to Record Before Disarming] }
*
* Note(s) : Make sure the number of entries in this configuration table is < OS_TRACE_CFG_TRIG_MAX_TRIGS
*           otherwise, any other triggers in excess will be ignored.
*********************************************************************************************************
*/

const  OS_TRACE_TRIG_CFG_TBL  OSTraceCfgTrigTbl[] =
{
    {OS_CFG_TRACE_TRIG_ID_SW1, "Signal Task # 1   (SWITCH 1)", 3},
    {OS_CFG_TRACE_TRIG_ID_SW2, "Post Q Task # 2   (SWITCH 2)", 3},
    {OS_CFG_TRACE_TRIG_ID_SW3, "Other Kernel Objs (SWITCH 3)", 1}
};

const  CPU_INT08U      OSTraceCfgTrigTblLen = sizeof(OSTraceCfgTrigTbl) / sizeof(OS_TRACE_TRIG_CFG_TBL);

#endif                                                          /* End of uC/OS-III Trace Triggering Module.            */
