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
* File          : os_trace_trig.c
* Version       : V3.06.00
* Programmer(s) : JPB
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                           INCLUDE FILES
*********************************************************************************************************
*/

#include  <os_cfg_trace.h>
#include  <os.h>
#include  "os_trace_trig.h"




/*
*********************************************************************************************************
*                                   UC/OS-III TRACE TRIGGERS MODULE
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
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

        static  OS_TCB          OSTraceTrigTaskTCB;

        static  CPU_STK         OSTraceTrigTaskStk[OS_CFG_TRACE_TRIG_TASK_STK_SIZE];

                OS_TRACE_TRIGS  OSTraceTrigs;


/*
*********************************************************************************************************
*                                     EXTERNAL GLOBAL VARIABLES
*********************************************************************************************************
*/

extern  const   CPU_INT08U   OSTraceCfgTrigTblLen;


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

                void         OSTraceTrigHookStop(void);
                CPU_INT16S   OSTraceTrigIndexOf (CPU_INT32U id);
        static  void         OSTraceTrigTask    (void *p_arg);


/*
*********************************************************************************************************
*                                     OSTraceTrigSetAllHooks()
*
* Description : Setup the callback functions.
*
* Arguments   : none.
*
* Returns     : none.
*
* Notes       : (1) The tracing facility needs to be included in the project.
*********************************************************************************************************
*/

void OSTraceTrigSetAllHooks (void)
{
    CPU_SR_ALLOC();


    CPU_CRITICAL_ENTER();
#if (defined(OS_CFG_TRACE_EN) && (OS_CFG_TRACE_EN > 0u))
    vTraceStopHookPtr = OSTraceTrigHookStop;
#endif
    CPU_CRITICAL_EXIT();
}


/*
*********************************************************************************************************
*                                       OSTraceTrigInit()
*
* Description : Initialize the uC/OS-III Trace Triggers module.
*
* Arguments   : none.
*
* Returns     : none.
*
* Notes       : (1) The tracing facility needs to be included in the project.
*********************************************************************************************************
*/

void OSTraceTrigInit (void)
{
    OS_ERR      err;
    CPU_INT16S  ix;
    CPU_INT08U  qty;


    OSTraceTrigSetAllHooks();                                   /* Sets the call back functions.                        */
    
    if (OSTraceCfgTrigTblLen > OS_CFG_TRACE_TRIG_MAX_TRIGS) {
        qty = OS_CFG_TRACE_TRIG_MAX_TRIGS;
    } else {
        qty = OSTraceCfgTrigTblLen;
    }

    for (ix = 0; ix < qty; ix++) {
        OSTraceTrigs.Trig[ix].Armed       =  DEF_NO;
        OSTraceTrigs.Trig[ix].Status      =  OS_TRACE_TRIG_STATUS_TRIG_DISARMED;
        OSTraceTrigs.Trig[ix].ID          =  OSTraceCfgTrigTbl[ix].ID;
        OSTraceTrigs.Trig[ix].NamePtr     = &OSTraceCfgTrigTbl[ix].Name[0];
        OSTraceTrigs.Trig[ix].MaxNbrFiles =  OSTraceCfgTrigTbl[ix].MaxNbrFiles;
    }

    OSTraceTrigs.Qty           = qty;
    OSTraceTrigs.Status.Target = OS_TRACE_TRIG_STATUS_EP_READY;
    OSTraceTrigs.Status.Probe  = OS_TRACE_TRIG_STATUS_EP_IDLE;

    OSTaskCreate((OS_TCB     *)&OSTraceTrigTaskTCB,             /* Create the uC/OS-III Trace triggers task             */
                 (CPU_CHAR   *)"uC/OS-III Trace Triggers Task",
                 (OS_TASK_PTR ) OSTraceTrigTask,
                 (void       *) 0,
                 (OS_PRIO     ) OS_CFG_TRACE_TRIG_TASK_PRIO,
                 (CPU_STK    *)&OSTraceTrigTaskStk[0],
                 (CPU_STK_SIZE) OS_CFG_TRACE_TRIG_TASK_STK_SIZE / 10u,
                 (CPU_STK_SIZE) OS_CFG_TRACE_TRIG_TASK_STK_SIZE,
                 (OS_MSG_QTY  ) 0u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);
}    


/*
*********************************************************************************************************
*                                            OSTraceTrig()
*
* Description : Starts the recording if the trigger conditions are met.
*
* Arguments   : Trigger ID as defined in trace_trig_cfg.h.
*
* Returns     : none.
*
* Notes       : (1) The tracing facility needs to be included in the project.
*********************************************************************************************************
*/

void OSTraceTrig (CPU_INT32U id)
{
    CPU_INT16S  ix;
    CPU_SR_ALLOC();


    ix = OSTraceTrigIndexOf(id);

    if (ix >= 0) {
        if (OSTraceTrigs.Trig[ix].Armed == DEF_YES &&
            OSTraceTrigs.Status.Probe   == OS_TRACE_TRIG_STATUS_EP_READY &&
            OSTraceTrigs.Status.Target  == OS_TRACE_TRIG_STATUS_EP_READY) {
            CPU_CRITICAL_ENTER();
            OS_TRACE_STOP();
            OSTraceTrigs.Status.Target   = OS_TRACE_TRIG_STATUS_EP_BUSY;
            OSTraceTrigs.Trig[ix].Status = OS_TRACE_TRIG_STATUS_TRIG_RECORDING;
            OSTraceTrigs.CurIx           = ix;
            OS_TRACE_CLEAR();
            OS_TRACE_START();
            CPU_CRITICAL_EXIT();
        } else if (OSTraceTrigs.Trig[ix].Armed  == DEF_YES &&
                   OSTraceTrigs.Trig[ix].Status == OS_TRACE_TRIG_STATUS_TRIG_ARMED &&
                  (OSTraceTrigs.Status.Probe    != OS_TRACE_TRIG_STATUS_EP_READY ||
                   OSTraceTrigs.Status.Target   != OS_TRACE_TRIG_STATUS_EP_READY)) {
            OSTraceTrigs.Trig[ix].Status = OS_TRACE_TRIG_STATUS_TRIG_IGNORED;
        }
    }
}


/*
*********************************************************************************************************
*                                      OSTraceTrigHookStop()
*
* Description : Callback function in the event the recording is stopped by the tracing facility.
*
* Arguments   : none.
*
* Returns     : none.
*
* Notes       : (1) The tracing facility needs to be included in the project.
*********************************************************************************************************
*/

void OSTraceTrigHookStop (void)
{
    CPU_INT08U  ix;


    ix = OSTraceTrigs.CurIx;

    if (OSTraceTrigs.Status.Probe    == OS_TRACE_TRIG_STATUS_EP_READY &&
        OSTraceTrigs.Status.Target   == OS_TRACE_TRIG_STATUS_EP_BUSY) {
        OSTraceTrigs.Status.Probe     = OS_TRACE_TRIG_STATUS_EP_BUSY;
        OSTraceTrigs.Status.Target    = OS_TRACE_TRIG_STATUS_EP_READY;
        OSTraceTrigs.Trig[ix].Status  = OS_TRACE_TRIG_STATUS_TRIG_READY;
    }
}


/*
*********************************************************************************************************
*                                     OSTraceTrigIndexOf()
*
* Description : Locates and returns the index in the array for the specified Trigger ID.
*
* Arguments   : id      Trigger ID as defined in os_cfg_trace_trig.h.
*
* Returns     : none.
*
* Notes       : (1) The tracing facility needs to be included in the project.
*********************************************************************************************************
*/

CPU_INT16S  OSTraceTrigIndexOf (CPU_INT32U id)
{
    CPU_INT16S  ix;


    for (ix = 0; ix < OS_CFG_TRACE_TRIG_MAX_TRIGS; ix++) {
        if (OSTraceTrigs.Trig[ix].ID == id) {
            return (ix);
        }
    }

    return (-1);
}


/*
*********************************************************************************************************
*                                     OSTraceTrigTask()
*
* Description : This task updates the status of each trigger.
*
* Arguments   : p_arg       Argument passed by 'OSTaskCreate()'.
*
* Returns     : none.
*
* Notes       : (1) The tracing facility needs to be included in the project.
*********************************************************************************************************
*/

static  void  OSTraceTrigTask (void *p_arg)
{
    CPU_INT08U  ix;
    OS_ERR      err;


    while (DEF_ON) {
        for (ix = 0; ix < OSTraceTrigs.Qty; ix++) {
            if (OSTraceTrigs.Trig[ix].Status == OS_TRACE_TRIG_STATUS_TRIG_DISARMED &&
                OSTraceTrigs.Trig[ix].Armed  == DEF_YES) {
                OSTraceTrigs.Trig[ix].Status  = OS_TRACE_TRIG_STATUS_TRIG_ARMED;
            } else if (OSTraceTrigs.Trig[ix].Status == OS_TRACE_TRIG_STATUS_TRIG_ARMED &&
                       OSTraceTrigs.Trig[ix].Armed == DEF_NO) {
                OSTraceTrigs.Trig[ix].Status  = OS_TRACE_TRIG_STATUS_TRIG_DISARMED;
            } else if (OSTraceTrigs.Trig[ix].Status == OS_TRACE_TRIG_STATUS_TRIG_READY &&
                       OSTraceTrigs.Trig[ix].Armed == DEF_NO) {
                OSTraceTrigs.Trig[ix].Status  = OS_TRACE_TRIG_STATUS_TRIG_DISARMED;
            } else if (OSTraceTrigs.Trig[ix].Status == OS_TRACE_TRIG_STATUS_TRIG_IGNORED &&
                       OSTraceTrigs.Trig[ix].Armed == DEF_NO) {
                OSTraceTrigs.Trig[ix].Status  = OS_TRACE_TRIG_STATUS_TRIG_DISARMED;
            }
        }

        OSTimeDlyHMSM(0, 0, 0, 250,
                      OS_OPT_TIME_HMSM_STRICT,
                     &err);
    }
}
#endif                                                          /* End of uC/OS-III Trace Triggering Module.            */
