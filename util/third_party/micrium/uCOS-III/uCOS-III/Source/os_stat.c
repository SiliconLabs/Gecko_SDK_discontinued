/*
************************************************************************************************************************
*                                                      uC/OS-III
*                                                 The Real-Time Kernel
*
*                                  (c) Copyright 2009-2016; Micrium, Inc.; Weston, FL
*                           All rights reserved.  Protected by international copyright laws.
*
*                                                  STATISTICS MODULE
*
* File    : OS_STAT.C
* By      : JJL
* Version : V3.06.00
*
* LICENSING TERMS:
* ---------------
*           uC/OS-III is provided in source form for FREE short-term evaluation, for educational use or
*           for peaceful research.  If you plan or intend to use uC/OS-III in a commercial application/
*           product then, you need to contact Micrium to properly license uC/OS-III for its use in your
*           application/product.   We provide ALL the source code for your convenience and to help you
*           experience uC/OS-III.  The fact that the source is provided does NOT mean that you can use
*           it commercially without paying a licensing fee.
*
*           Knowledge of the source code may NOT be used to develop a similar product.
*
*           Please help us continue to provide the embedded community with the finest software available.
*           Your honesty is greatly appreciated.
*
*           You can find our product's user manual, API reference, release notes and
*           more information at https://doc.micrium.com.
*           You can contact us at www.micrium.com.
************************************************************************************************************************
*/

#define  MICRIUM_SOURCE
#include "os.h"

#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
const  CPU_CHAR  *os_stat__c = "$Id: $";
#endif


#if (OS_CFG_STAT_TASK_EN == DEF_ENABLED)

/*
************************************************************************************************************************
*                                                   RESET STATISTICS
*
* Description: This function is called by your application to reset the statistics.
*
* Argument(s): p_err      is a pointer to a variable that will contain an error code returned by this function.
*
*                             OS_ERR_NONE            The call succeeded
*
* Returns    : none
*
* Note(s)    : none
************************************************************************************************************************
*/

void  OSStatReset (OS_ERR  *p_err)
{
#if (OS_CFG_DBG_EN == DEF_ENABLED)
    OS_TCB      *p_tcb;
#if (OS_MSG_EN == DEF_ENABLED)
    OS_MSG_Q    *p_msg_q;
#endif
#if (OS_CFG_Q_EN == DEF_ENABLED)
    OS_Q        *p_q;
#endif
#endif
    CPU_SR_ALLOC();



#ifdef OS_SAFETY_CRITICAL
    if (p_err == DEF_NULL) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return;
    }
#endif

    CPU_CRITICAL_ENTER();
#if (OS_CFG_STAT_TASK_EN == DEF_ENABLED)
    OSStatTaskCPUUsageMax = 0u;
#if (OS_CFG_TS_EN == DEF_ENABLED)
    OSStatTaskTimeMax     = 0u;
#endif
#endif

#if (OS_CFG_TS_EN == DEF_ENABLED)
    OSTickTaskTimeMax     = 0u;
#endif

#if (OS_CFG_TMR_EN == DEF_ENABLED)
#if (OS_CFG_TS_EN == DEF_ENABLED)
    OSTmrTaskTimeMax      = 0u;
#endif
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN
#if (OS_CFG_TS_EN == DEF_ENABLED)
    OSIntDisTimeMax       = 0u;                                 /* Reset the maximum interrupt disable time             */
    CPU_StatReset();                                            /* Reset CPU-specific performance monitors.             */
#endif
#endif

#if (OS_CFG_SCHED_LOCK_TIME_MEAS_EN == DEF_ENABLED)
    OSSchedLockTimeMax    = 0u;                                 /* Reset the maximum scheduler lock time                */
#endif

#if ((OS_MSG_EN == DEF_ENABLED) && (OS_CFG_DBG_EN == DEF_ENABLED))
    OSMsgPool.NbrUsedMax  = 0u;
#endif
    CPU_CRITICAL_EXIT();

#if (OS_CFG_DBG_EN == DEF_ENABLED)
    CPU_CRITICAL_ENTER();
    p_tcb = OSTaskDbgListPtr;
    CPU_CRITICAL_EXIT();
    while (p_tcb != DEF_NULL) {                                 /* Reset per-Task statistics                            */
        CPU_CRITICAL_ENTER();

#ifdef CPU_CFG_INT_DIS_MEAS_EN
        p_tcb->IntDisTimeMax    = 0u;
#endif

#if (OS_CFG_SCHED_LOCK_TIME_MEAS_EN == DEF_ENABLED)
        p_tcb->SchedLockTimeMax = 0u;
#endif

#if (OS_CFG_TASK_PROFILE_EN == DEF_ENABLED)
#if (OS_CFG_TASK_Q_EN == DEF_ENABLED)
        p_tcb->MsgQPendTimeMax  = 0u;
#endif
        p_tcb->SemPendTimeMax   = 0u;
        p_tcb->CtxSwCtr         = 0u;
        p_tcb->CPUUsage         = 0u;
        p_tcb->CPUUsageMax      = 0u;
        p_tcb->CyclesTotal      = 0u;
        p_tcb->CyclesTotalPrev  = 0u;
#if (OS_CFG_TS_EN == DEF_ENABLED)
        p_tcb->CyclesStart      = OS_TS_GET();
#endif
#endif

#if (OS_CFG_TASK_Q_EN == DEF_ENABLED)
        p_msg_q                 = &p_tcb->MsgQ;
        p_msg_q->NbrEntriesMax  = 0u;
#endif
        p_tcb                   = p_tcb->DbgNextPtr;
        CPU_CRITICAL_EXIT();
    }
#endif

#if (OS_CFG_Q_EN == DEF_ENABLED) && (OS_CFG_DBG_EN == DEF_ENABLED)
    CPU_CRITICAL_ENTER();
    p_q = OSQDbgListPtr;
    CPU_CRITICAL_EXIT();
    while (p_q != DEF_NULL) {                                   /* Reset message queues statistics                      */
        CPU_CRITICAL_ENTER();
        p_msg_q                = &p_q->MsgQ;
        p_msg_q->NbrEntriesMax = 0u;
        p_q                    = p_q->DbgNextPtr;
        CPU_CRITICAL_EXIT();
    }
#endif


   *p_err = OS_ERR_NONE;
}


/*
************************************************************************************************************************
*                                                DETERMINE THE CPU CAPACITY
*
* Description: This function is called by your application to establish CPU usage by first determining how high a 32-bit
*              counter would count to in 1/10 second if no other tasks were to execute during that time.  CPU usage is
*              then determined by a low priority task which keeps track of this 32-bit counter every second but this
*              time, with other tasks running.  CPU usage is determined by:
*
*                                             OS_Stat_IdleCtr
*                 CPU Usage (%) = 100 * (1 - ------------------)
*                                            OS_Stat_IdleCtrMax
*
* Argument(s): p_err      is a pointer to a variable that will contain an error code returned by this function.
*
*                             OS_ERR_NONE              The call was successfu
*                             OS_ERR_OS_NOT_RUNNING    If uC/OS-III is not running yet
*
* Returns    : none
*
* Note(s)    : none
************************************************************************************************************************
*/

void  OSStatTaskCPUUsageInit (OS_ERR  *p_err)
{
    OS_ERR   err;
    OS_TICK  dly;
    CPU_SR_ALLOC();



#ifdef OS_SAFETY_CRITICAL
    if (p_err == DEF_NULL) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return;
    }
#endif

#if (OS_CFG_INVALID_OS_CALLS_CHK_EN == DEF_ENABLED)             /* Is the kernel running?                               */
    if (OSRunning != OS_STATE_OS_RUNNING) {
       *p_err = OS_ERR_OS_NOT_RUNNING;
        return;
    }
#endif

#if ((OS_CFG_TMR_EN == DEF_ENABLED) && (OS_CFG_TASK_SUSPEND_EN == DEF_ENABLED))
    OSTaskSuspend(&OSTmrTaskTCB, &err);
    if (err != OS_ERR_NONE) {
       *p_err = err;
        return;
    }
#endif

    OSTimeDly(2u,                                               /* Synchronize with clock tick                          */
              (OS_OPT  )OS_OPT_TIME_DLY,
              (OS_ERR *)&err);
    if (err != OS_ERR_NONE) {
       *p_err = err;
        return;
    }
    CPU_CRITICAL_ENTER();
    OSStatTaskCtr = 0u;                                         /* Clear idle counter                                   */
    CPU_CRITICAL_EXIT();

    dly = 0u;
    if (OSCfg_TickRate_Hz > OSCfg_StatTaskRate_Hz) {
        dly = (OS_TICK)(OSCfg_TickRate_Hz / OSCfg_StatTaskRate_Hz);
    }
    if (dly == 0u) {
        dly =  (OSCfg_TickRate_Hz / 10u);
    }

    OSTimeDly(dly,                                              /* Determine MAX. idle counter value                    */
              OS_OPT_TIME_DLY,
              &err);

#if ((OS_CFG_TMR_EN == DEF_ENABLED) && (OS_CFG_TASK_SUSPEND_EN == DEF_ENABLED))
    OSTaskResume(&OSTmrTaskTCB, &err);
    if (err != OS_ERR_NONE) {
       *p_err = err;
        return;
    }
#endif

    CPU_CRITICAL_ENTER();
#if (OS_CFG_TS_EN == DEF_ENABLED)
    OSStatTaskTimeMax = 0u;
#endif

    OSStatTaskCtrMax  = OSStatTaskCtr;                          /* Store maximum idle counter count                     */
    OSStatTaskRdy     = OS_STATE_RDY;
    CPU_CRITICAL_EXIT();
   *p_err             = OS_ERR_NONE;
}


/*
************************************************************************************************************************
*                                                    STATISTICS TASK
*
* Description: This task is internal to uC/OS-III and is used to compute some statistics about the multitasking
*              environment.  Specifically, OS_StatTask() computes the CPU usage.  CPU usage is determined by:
*
*                                                   OSStatTaskCtr
*                 OSStatTaskCPUUsage = 100 * (1 - ------------------)     (units are in %)
*                                                  OSStatTaskCtrMax
*
* Arguments  : p_arg     this pointer is not used at this time.
*
* Returns    : none
*
* Note(s)    : 1) This task runs at a priority level higher than the idle task.
*
*              2) You can disable this task by setting the configuration #define OS_CFG_STAT_TASK_EN to 0.
*
*              3) You MUST have at least a delay of 2/10 seconds to allow for the system to establish the maximum value
*                 for the idle counter.
*
*              4) This function is INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

void  OS_StatTask (void  *p_arg)
{
#if (OS_CFG_DBG_EN == DEF_ENABLED)
#if (OS_CFG_TASK_PROFILE_EN == DEF_ENABLED)
    OS_CPU_USAGE usage;
    OS_CYCLES    cycles_total;
    OS_CYCLES    cycles_div;
    OS_CYCLES    cycles_mult;
    OS_CYCLES    cycles_max;
#endif
    OS_TCB      *p_tcb;
#endif
    OS_TICK      ctr_max;
    OS_TICK      ctr_mult;
    OS_TICK      ctr_div;
    OS_ERR       err;
    OS_TICK      dly;
#if (OS_CFG_TS_EN == DEF_ENABLED)
    CPU_TS       ts_start;
    CPU_TS       ts_end;
#endif
    CPU_SR_ALLOC();



    (void)p_arg;                                                /* Prevent compiler warning for not using 'p_arg'       */

    while (OSStatTaskRdy != DEF_TRUE) {
        OSTimeDly(2u * OSCfg_StatTaskRate_Hz,                   /* Wait until statistic task is ready                   */
                  OS_OPT_TIME_DLY,
                  &err);
    }
    OSStatReset(&err);                                          /* Reset statistics                                     */

    dly = (OS_TICK)0;                                           /* Compute statistic task sleep delay                   */
    if (OSCfg_TickRate_Hz > OSCfg_StatTaskRate_Hz) {
        dly = (OSCfg_TickRate_Hz / OSCfg_StatTaskRate_Hz);
    }
    if (dly == 0u) {
        dly =  (OSCfg_TickRate_Hz / 10u);
    }

    for (;;) {
#if (OS_CFG_TS_EN == DEF_ENABLED)
        ts_start        = OS_TS_GET();
#endif
#ifdef  CPU_CFG_INT_DIS_MEAS_EN
        OSIntDisTimeMax = CPU_IntDisMeasMaxGet();
#endif

        CPU_CRITICAL_ENTER();                                   /* ---------------- OVERALL CPU USAGE ----------------- */
        OSStatTaskCtrRun   = OSStatTaskCtr;                     /* Obtain the of the stat counter for the past .1 second*/
        OSStatTaskCtr      = 0u;                                /* Reset the stat counter for the next .1 second        */
        CPU_CRITICAL_EXIT();

        if (OSStatTaskCtrMax > OSStatTaskCtrRun) {              /* Compute CPU Usage with best resolution               */
            if (OSStatTaskCtrMax < 400000u) {                   /* 1 to       400,000                                   */
                ctr_mult = 10000u;
                ctr_div  =     1u;
            } else if (OSStatTaskCtrMax <   4000000u) {         /* 400,000 to     4,000,000                             */
                ctr_mult =  1000u;
                ctr_div  =    10u;
            } else if (OSStatTaskCtrMax <  40000000u) {         /* 4,000,000 to    40,000,000                           */
                ctr_mult =   100u;
                ctr_div  =   100u;
            } else if (OSStatTaskCtrMax < 400000000u) {         /* 40,000,000 to   400,000,000                          */
                ctr_mult =    10u;
                ctr_div  =  1000u;
            } else {                                            /* 400,000,000 and up                                   */
                ctr_mult =     1u;
                ctr_div  = 10000u;
            }
            ctr_max            = OSStatTaskCtrMax / ctr_div;
            OSStatTaskCPUUsage = (OS_CPU_USAGE)((OS_TICK)10000u - ctr_mult * OSStatTaskCtrRun / ctr_max);
            if (OSStatTaskCPUUsageMax < OSStatTaskCPUUsage) {
                OSStatTaskCPUUsageMax = OSStatTaskCPUUsage;
            }
        } else {
            OSStatTaskCPUUsage = 0u;
        }

        OSStatTaskHook();                                       /* Invoke user definable hook                           */


#if (OS_CFG_DBG_EN == DEF_ENABLED)
#if (OS_CFG_TASK_PROFILE_EN == DEF_ENABLED)
        cycles_total = 0u;

        CPU_CRITICAL_ENTER();
        p_tcb = OSTaskDbgListPtr;
        CPU_CRITICAL_EXIT();
        while (p_tcb != DEF_NULL) {                             /* ---------------- TOTAL CYCLES COUNT ---------------- */
            CPU_CRITICAL_ENTER();
            p_tcb->CyclesTotalPrev = p_tcb->CyclesTotal;        /* Save accumulated # cycles into a temp variable       */
            p_tcb->CyclesTotal     = 0u;                        /* Reset total cycles for task for next run             */
            CPU_CRITICAL_EXIT();

            cycles_total          += p_tcb->CyclesTotalPrev;    /* Perform sum of all task # cycles                     */

            CPU_CRITICAL_ENTER();
            p_tcb                  = p_tcb->DbgNextPtr;
            CPU_CRITICAL_EXIT();
        }
#endif


#if (OS_CFG_TASK_PROFILE_EN == DEF_ENABLED)
                                                                /* ------------ INDIVIDUAL TASK CPU USAGE ------------- */
        if (cycles_total > 0u) {                                /* 'cycles_total' scaling ...                           */
            if (cycles_total < 400000u) {                       /* 1 to       400,000                                   */
                cycles_mult = 10000u;
                cycles_div  =     1u;
            } else if (cycles_total <   4000000u) {             /* 400,000 to     4,000,000                             */
                cycles_mult =  1000u;
                cycles_div  =    10u;
            } else if (cycles_total <  40000000u) {             /* 4,000,000 to    40,000,000                           */
                cycles_mult =   100u;
                cycles_div  =   100u;
            } else if (cycles_total < 400000000u) {             /* 40,000,000 to   400,000,000                          */
                cycles_mult =    10u;
                cycles_div  =  1000u;
            } else {                                            /* 400,000,000 and up                                   */
                cycles_mult =     1u;
                cycles_div  = 10000u;
            }
            cycles_max  = cycles_total / cycles_div;
        } else {
            cycles_mult = 0u;
            cycles_max  = 1u;
        }
#endif
        CPU_CRITICAL_ENTER();
        p_tcb = OSTaskDbgListPtr;
        CPU_CRITICAL_EXIT();
        while (p_tcb != DEF_NULL) {
#if (OS_CFG_TASK_PROFILE_EN == DEF_ENABLED)                     /* Compute execution time of each task                  */
            usage = (OS_CPU_USAGE)(cycles_mult * p_tcb->CyclesTotalPrev / cycles_max);
            if (usage > 10000u) {
                usage = 10000u;
            }
            p_tcb->CPUUsage = usage;
            if (p_tcb->CPUUsageMax < usage) {                   /* Detect peak CPU usage                                */
                p_tcb->CPUUsageMax = usage;
            }
#endif

#if (OS_CFG_STAT_TASK_STK_CHK_EN == DEF_ENABLED)
            OSTaskStkChk( p_tcb,                                /* Compute stack usage of active tasks only             */
                         &p_tcb->StkFree,
                         &p_tcb->StkUsed,
                         &err);
#endif

            CPU_CRITICAL_ENTER();
            p_tcb = p_tcb->DbgNextPtr;
            CPU_CRITICAL_EXIT();
        }
#endif

        if (OSStatResetFlag == DEF_TRUE) {                      /* Check if need to reset statistics                    */
            OSStatResetFlag  = DEF_FALSE;
            OSStatReset(&err);
        }

#if (OS_CFG_TS_EN == DEF_ENABLED)
        ts_end = OS_TS_GET() - ts_start;                        /* Measure execution time of statistic task             */
        if (OSStatTaskTimeMax < ts_end) {
            OSStatTaskTimeMax = ts_end;
        }
#endif

        OSTimeDly(dly,
                  OS_OPT_TIME_DLY,
                  &err);
    }
}


/*
************************************************************************************************************************
*                                              INITIALIZE THE STATISTICS
*
* Description: This function is called by OSInit() to initialize the statistic task.
*
* Argument(s): p_err     is a pointer to a variable that will contain an error code returned by this function.
*
*                            OS_ERR_STK_INVALID       If you specified a NULL stack pointer during configuration
*                            OS_ERR_STK_SIZE_INVALID  If you didn't specify a large enough stack.
*                            OS_ERR_PRIO_INVALID      If you specified a priority for the statistic task equal to or
*                                                     lower (i.e. higher number) than the idle task.
*                            OS_ERR_xxx               An error code returned by OSTaskCreate()
*
* Returns    : none
*
* Note(s)    : This function is INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

void  OS_StatTaskInit (OS_ERR  *p_err)
{
    OSStatTaskCtr    = 0u;
    OSStatTaskCtrRun = 0u;
    OSStatTaskCtrMax = 0u;
    OSStatTaskRdy    = OS_STATE_NOT_RDY;                        /* Statistic task is not ready                          */
    OSStatResetFlag  = DEF_FALSE;

                                                                /* --------------- CREATE THE STAT TASK --------------- */
    if (OSCfg_StatTaskStkBasePtr == DEF_NULL) {
       *p_err = OS_ERR_STAT_STK_INVALID;
        return;
    }

    if (OSCfg_StatTaskStkSize < OSCfg_StkSizeMin) {
       *p_err = OS_ERR_STAT_STK_SIZE_INVALID;
        return;
    }

    if (OSCfg_StatTaskPrio >= (OS_CFG_PRIO_MAX - 1u)) {
       *p_err = OS_ERR_STAT_PRIO_INVALID;
        return;
    }

    OSTaskCreate(&OSStatTaskTCB,
                 (CPU_CHAR   *)((void *)"uC/OS-III Stat Task"),
                 OS_StatTask,
                 DEF_NULL,
                 OSCfg_StatTaskPrio,
                 OSCfg_StatTaskStkBasePtr,
                 OSCfg_StatTaskStkLimit,
                 OSCfg_StatTaskStkSize,
                 0u,
                 0u,
                 DEF_NULL,
                 (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 p_err);
}

#endif
