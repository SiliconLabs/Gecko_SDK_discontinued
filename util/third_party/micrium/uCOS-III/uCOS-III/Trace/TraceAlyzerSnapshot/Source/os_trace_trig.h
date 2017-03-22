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
*                                          UC/OS-III TRACE
*
* File          : os_trace_trig.h
* Version       : V3.06.00
* Programmer(s) : JPB
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                              MODULE
*********************************************************************************************************
*/

#ifndef   OS_TRACE_TRIG_H
#define   OS_TRACE_TRIG_H


/*
*********************************************************************************************************
*                                           INCLUDE FILES
*********************************************************************************************************
*/

#include  <trcHardwarePort.h>
#include  <trcConfig.h>
#include  <lib_def.h>


/*
*********************************************************************************************************
*                                  UC/OS-III TRACE TRIGGERS MODULE
*
* Note(s) : The uC/OS-III Trace Triggering mechanism requires the hardware platform to be 
*           supported by uC/Probe.
*
*           If your platform is uC/Probe-ready, then enable this module by setting
*           the macro below to 1.
*********************************************************************************************************
*/

#if (defined(OS_CFG_TRACE_TRIG_EN) && (OS_CFG_TRACE_TRIG_EN > 0u))


/*
*********************************************************************************************************
*                                              DEFINES
*********************************************************************************************************
*/

#define  OS_TRACE_TRIG(id)                    OSTraceTrig(id)


/*
*********************************************************************************************************
*                                            ENUMERATIONS
*********************************************************************************************************
*/

typedef  enum  os_trace_trig_status_ep {
    OS_TRACE_TRIG_STATUS_EP_IDLE             = 0u,
    OS_TRACE_TRIG_STATUS_EP_BUSY             = 1u,
    OS_TRACE_TRIG_STATUS_EP_READY            = 2u
} OS_TRACE_TRIG_STATUS_EP;


typedef  enum  os_trace_trig_status_trig {
    OS_TRACE_TRIG_STATUS_TRIG_DISARMED       = 0u,
    OS_TRACE_TRIG_STATUS_TRIG_ARMED          = 1u,
    OS_TRACE_TRIG_STATUS_TRIG_RECORDING      = 2u,
    OS_TRACE_TRIG_STATUS_TRIG_READY          = 3u,
    OS_TRACE_TRIG_STATUS_TRIG_IGNORED        = 4u
} OS_TRACE_TRIG_STATUS_TRIG;


/*
*********************************************************************************************************
*                                             DATA TYPES
*********************************************************************************************************
*/

typedef  struct  os_trace_trig_status_probe {
    OS_TRACE_TRIG_STATUS_EP    Probe;
    OS_TRACE_TRIG_STATUS_EP    Target;
} OS_TRACE_TRIG_STATUS_PROBE;


typedef  struct  os_trace_trig {
    CPU_INT16U                 ID;                              /* Trigger ID.                                          */
    const CPU_CHAR            *NamePtr;                         /* Trigger Name.                                        */
    CPU_INT08U                 MaxNbrFiles;                     /* Maximum number of recording files before disarming.  */
    CPU_BOOLEAN                Armed;                           /* Flag set by uC/Probe to enable/disable triggering.   */
    OS_TRACE_TRIG_STATUS_TRIG  Status;                          /* Flag set by the Target to display status in uC/Probe */
} OS_TRACE_TRIG;


typedef  struct  os_trace_trigs {
    OS_TRACE_TRIG              Trig[OS_CFG_TRACE_TRIG_MAX_TRIGS];  /* Array of uC/Trace triggers.                       */
    CPU_INT08U                 Qty;                             /* Number of triggers.                                  */
    OS_TRACE_TRIG_STATUS_PROBE Status;                          /* Overall status of uC/Probe and the Target.           */
    CPU_INT08U                 CurIx;                           /* Index of the current recording.                      */
} OS_TRACE_TRIGS;


typedef  const  struct  os_trace_trig_cfg_tbl {
    CPU_INT16U              ID;                                 /* Trigger ID.                                          */
    CPU_CHAR                Name[OS_CFG_TRACE_TRIG_NAME_LEN];   /* Trigger Name.                                        */
    CPU_INT08U              MaxNbrFiles;                        /* Maximum number of recording files before disarming.  */
} OS_TRACE_TRIG_CFG_TBL;


/*
*********************************************************************************************************
*                                    UC/TRACE TRIGGERS CONFIGURATION
*********************************************************************************************************
*/

extern  const  OS_TRACE_TRIG_CFG_TBL  OSTraceCfgTrigTbl[];
extern  const  CPU_INT08U             OSTraceCfgTrigTblLen;


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void  OSTraceTrigInit(void);
void  OSTraceTrig    (CPU_INT32U);

#endif                                                          /* End of uC/Trace Triggering Module.                   */


/*
*********************************************************************************************************
*                                             MODULE END
*********************************************************************************************************
*/

#endif                                                          /* End of trace module include.                         */
