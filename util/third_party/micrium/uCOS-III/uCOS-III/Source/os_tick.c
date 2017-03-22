/*
***********************************************************************************************************************
*                                                      uC/OS-III
*                                                 The Real-Time Kernel
*
*                                  (c) Copyright 2009-2016; Micrium, Inc.; Weston, FL
*                           All rights reserved.  Protected by international copyright laws.
*
*                                                   TICK MANAGEMENT
*
* File    : OS_TICK.C
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
const  CPU_CHAR  *os_tick__c = "$Id: $";
#endif

#if (OS_CFG_TASK_TICK_EN == DEF_ENABLED)
/*
************************************************************************************************************************
*                                                 FUNCTION PROTOTYPES
************************************************************************************************************************
*/

static  CPU_TS  OS_TickListUpdateDly     (OS_TICK ticks);
static  CPU_TS  OS_TickListUpdateTimeout (OS_TICK ticks);

/*
************************************************************************************************************************
*                                                      TICK TASK
*
* Description: This task is internal to uC/OS-III and is triggered by the tick interrupt.
*
* Arguments  : p_arg     is an argument passed to the task when the task is created (unused).
*
* Returns    : none
*
* Note(s)    : This function is INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

void  OS_TickTask (void  *p_arg)
{
    OS_ERR   err;
#if (OS_CFG_TS_EN == DEF_ENABLED)
    CPU_TS   ts_delta;
    CPU_TS   ts_delta_dly;
    CPU_TS   ts_delta_timeout;
#endif
#if (OS_CFG_DYN_TICK_EN == DEF_ENABLED)
    OS_TICK  tick_step_dly;
    OS_TICK  tick_step_timeout;
#endif
    OS_TICK  tick_step;
    CPU_SR_ALLOC();


    (void)p_arg;                                                /* Prevent compiler warning                             */

    for (;;) {
        (void)OSTaskSemPend(0u,
                            OS_OPT_PEND_BLOCKING,
                            DEF_NULL,
                            &err);                              /* Wait for signal from tick interrupt                  */
        if (err == OS_ERR_NONE) {
            CPU_CRITICAL_ENTER();

#if (OS_CFG_DYN_TICK_EN == DEF_ENABLED)
            tick_step = OSTickCtrPend;
            OSTickCtr += tick_step;
            OSTickCtrPend = 0;
#else
            tick_step = 1u;                                     /* Always tick once when dynamic tick is disabled       */
            OSTickCtr++;                                        /* Keep track of the number of ticks                    */
#endif

            OS_TRACE_TICK_INCREMENT(OSTickCtr);

#if (OS_CFG_TS_EN == DEF_ENABLED)
            ts_delta_dly     = OS_TickListUpdateDly(tick_step);
            ts_delta_timeout = OS_TickListUpdateTimeout(tick_step);
            ts_delta         = ts_delta_dly + ts_delta_timeout; /* Compute total execution time of list updates         */
            if (OSTickTaskTimeMax < ts_delta) {
                OSTickTaskTimeMax = ts_delta;
            }
#else
            (void)OS_TickListUpdateDly(tick_step);
            (void)OS_TickListUpdateTimeout(tick_step);
#endif

#if (OS_CFG_DYN_TICK_EN == DEF_ENABLED)
            tick_step_dly = (OS_TICK)-1;
            tick_step_timeout = (OS_TICK)-1;
            if(OSTickListDly.TCB_Ptr != DEF_NULL) {
                tick_step_dly = OSTickListDly.TCB_Ptr->TickRemain;
            }
            if(OSTickListTimeout.TCB_Ptr != DEF_NULL) {
                tick_step_timeout = OSTickListTimeout.TCB_Ptr->TickRemain;
            }
            OSTickCtrStep = (tick_step_dly < tick_step_timeout) ? tick_step_dly : tick_step_timeout;
            BSP_OS_TickNextSet(OSTickCtrStep);
#endif
            CPU_CRITICAL_EXIT();
        }
    }
}

/*
************************************************************************************************************************
*                                                 INITIALIZE TICK TASK
*
* Description: This function is called by OSInit() to create the tick task.
*
* Arguments  : p_err   is a pointer to a variable that will hold the value of an error code:
*
*                          OS_ERR_TICK_STK_INVALID   if the pointer to the tick task stack is a NULL pointer
*                          OS_ERR_TICK_STK_SIZE      indicates that the specified stack size
*                          OS_ERR_PRIO_INVALID       if the priority you specified in the configuration is invalid
*                                                      (There could be only one task at the Idle Task priority)
*                                                      (Maybe the priority you specified is higher than OS_CFG_PRIO_MAX-1
*                          OS_ERR_??                 other error code returned by OSTaskCreate()
*
* Returns    : none
*
* Note(s)    : This function is INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

void  OS_TickTaskInit (OS_ERR  *p_err)
{
    OSTickCtr                    = 0u;                          /* Clear the tick counter                               */

#if (OS_CFG_DYN_TICK_EN == DEF_ENABLED)
    OSTickCtrStep                = (OS_TICK)-1;
    OSTickCtrPend                = 0u;
#endif

    OSTickListDly.TCB_Ptr        = DEF_NULL;
    OSTickListTimeout.TCB_Ptr    = DEF_NULL;

#if (OS_CFG_DBG_EN == DEF_ENABLED)
    OSTickListDly.NbrEntries     = 0u;
    OSTickListDly.NbrUpdated     = 0u;

    OSTickListTimeout.NbrEntries = 0u;
    OSTickListTimeout.NbrUpdated = 0u;
#endif

                                                                /* --------------- CREATE THE TICK TASK --------------- */
    if (OSCfg_TickTaskStkBasePtr == DEF_NULL) {
       *p_err = OS_ERR_TICK_STK_INVALID;
        return;
    }

    if (OSCfg_TickTaskStkSize < OSCfg_StkSizeMin) {
       *p_err = OS_ERR_TICK_STK_SIZE_INVALID;
        return;
    }

    if (OSCfg_TickTaskPrio >= (OS_CFG_PRIO_MAX - 1u)) {         /* Only one task at the 'Idle Task' priority            */
       *p_err = OS_ERR_TICK_PRIO_INVALID;
        return;
    }

    OSTaskCreate(&OSTickTaskTCB,
                 (CPU_CHAR   *)("uC/OS-III Tick Task"),
                 OS_TickTask,
                 DEF_NULL,
                 OSCfg_TickTaskPrio,
                 OSCfg_TickTaskStkBasePtr,
                 OSCfg_TickTaskStkLimit,
                 OSCfg_TickTaskStkSize,
                 0u,
                 0u,
                 DEF_NULL,
                 (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_NO_TLS),
                 p_err);
}

/*
************************************************************************************************************************
*                                                      INSERT
*
* Description: This task is internal to uC/OS-III and allows the insertion of a task in a tick list.
*
* Arguments  : p_list      is a pointer to the desired list
*
*              p_tcb       is a pointer to the TCB to insert in the list
*
*              time        is the amount of time remaining (in ticks) for the task to become ready
*
* Returns    : none
*
* Note(s)    : This function is INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

void  OS_TickListInsert (OS_TICK_LIST  *p_list,
                         OS_TCB        *p_tcb,
                         OS_TICK        time)
{
    OS_TCB  *p_tcb1;
    OS_TCB  *p_tcb2;
    OS_TICK  remain;
#if (OS_CFG_DYN_TICK_EN == DEF_ENABLED)
    OS_TICK  tick_step = (OS_TICK)-1;
#endif


    if (p_list->TCB_Ptr == DEF_NULL) {                          /* Is the list empty?                                   */
        p_tcb->TickRemain  = time;                              /* Yes, Store time in TCB                               */
        p_tcb->TickNextPtr = DEF_NULL;
        p_tcb->TickPrevPtr = DEF_NULL;
        p_tcb->TickListPtr = p_list;                            /* Link to this list                                    */
        p_list->TCB_Ptr    = p_tcb;                             /* Point to TCB of task to place in the list            */
#if (OS_CFG_DBG_EN == DEF_ENABLED)
        p_list->NbrEntries = 1u;                                /* List contains 1 entry                                */
#endif
#if (OS_CFG_DYN_TICK_EN == DEF_ENABLED)
        tick_step = time;
#endif
    } else {
        p_tcb1 = p_list->TCB_Ptr;
        p_tcb2 = p_list->TCB_Ptr;                               /* No,  Insert somewhere in the list in delta order     */
        remain = time;
        while (p_tcb2 != DEF_NULL) {
            if (remain <= p_tcb2->TickRemain) {
                if (p_tcb2->TickPrevPtr == DEF_NULL) {          /* Insert before the first entry in the list?           */
                    p_tcb->TickRemain   = remain;               /* Yes, Store remaining time                            */
                    p_tcb->TickPrevPtr  = DEF_NULL;
                    p_tcb->TickNextPtr  = p_tcb2;
                    p_tcb->TickListPtr  = p_list;               /* Link TCB to this list                                */
                    p_tcb2->TickRemain -= remain;               /* Reduce time of next entry in the list                */
                    p_tcb2->TickPrevPtr = p_tcb;
                    p_list->TCB_Ptr     = p_tcb;                /* Add TCB to the list                                  */
#if (OS_CFG_DBG_EN == DEF_ENABLED)
                    p_list->NbrEntries++;                       /* List contains an extra entry                         */
#endif
#if (OS_CFG_DYN_TICK_EN == DEF_ENABLED)
                    tick_step = remain;
#endif
                } else {                                        /* No,  Insert somewhere further in the list            */
                    p_tcb1              = p_tcb2->TickPrevPtr;
                    p_tcb->TickRemain   = remain;               /* Store remaining time                                 */
                    p_tcb->TickPrevPtr  = p_tcb1;
                    p_tcb->TickNextPtr  = p_tcb2;
                    p_tcb->TickListPtr  = p_list;               /* TCB points to this list                              */
                    p_tcb2->TickRemain -= remain;               /* Reduce time of next entry in the list                */
                    p_tcb2->TickPrevPtr = p_tcb;
                    p_tcb1->TickNextPtr = p_tcb;
#if (OS_CFG_DBG_EN == DEF_ENABLED)
                    p_list->NbrEntries++;                       /* List contains an extra entry                         */
#endif
                }

#if (OS_CFG_DYN_TICK_EN == DEF_ENABLED)
                if (tick_step < OSTickCtrStep) {
                    OSTickCtrStep = tick_step;
                    BSP_OS_TickNextSet(tick_step);
                }
#endif

                return;
            } else {
                remain -= p_tcb2->TickRemain;                   /* Point to the next TCB in the list                    */
                p_tcb1  = p_tcb2;
                p_tcb2  = p_tcb2->TickNextPtr;
            }
        }
        p_tcb->TickRemain   = remain;
        p_tcb->TickPrevPtr  = p_tcb1;
        p_tcb->TickNextPtr  = DEF_NULL;
        p_tcb->TickListPtr  = p_list;                           /* Link the list to the TCB                             */
        p_tcb1->TickNextPtr = p_tcb;
#if (OS_CFG_DBG_EN == DEF_ENABLED)
        p_list->NbrEntries++;                                   /* List contains an extra entry                         */
#endif
    }

#if (OS_CFG_DYN_TICK_EN == DEF_ENABLED)
    if (tick_step < OSTickCtrStep) {
        OSTickCtrStep = tick_step;
        BSP_OS_TickNextSet(tick_step);
    }
#endif
}

/*
************************************************************************************************************************
*                                            ADD TASK TO DELAYED TICK LIST
*
* Description: This function is called to place a task in a list of task waiting for either time to expire
*
* Arguments  : p_tcb          is a pointer to the OS_TCB of the task to add to the tick list
*              -----
*
*              time           represents either the 'match' value of OSTickCtr or a relative time from the current
*                             value of OSTickCtr as specified by the 'opt' argument..
*
*                             relative when 'opt' is set to OS_OPT_TIME_DLY
*                             relative when 'opt' is set to OS_OPT_TIME_TIMEOUT
*                             match    when 'opt' is set to OS_OPT_TIME_MATCH
*                             periodic when 'opt' is set to OS_OPT_TIME_PERIODIC
*
*              opt            is an option specifying how to calculate time.  The valid values are:
*              ---
*                                 OS_OPT_TIME_DLY
*                                 OS_OPT_TIME_TIMEOUT
*                                 OS_OPT_TIME_PERIODIC
*                                 OS_OPT_TIME_MATCH
*
*              p_err          is a pointer to a variable that will contain an error code returned by this function.
*              -----
*                                 OS_ERR_NONE           the call was successful and the time delay was scheduled.
*                                 OS_ERR_TIME_ZERO_DLY  if delay is zero or already occurred.
*
* Returns    : None
*
* Note(s)    : 1) This function is INTERNAL to uC/OS-III and your application MUST NOT call it.
*
*              2) This function is assumed to be called with interrupts disabled.
************************************************************************************************************************
*/

void  OS_TickListInsertDly (OS_TCB   *p_tcb,
                            OS_TICK   time,
                            OS_OPT    opt,
                            OS_ERR   *p_err)
{
    OS_TICK   remain;
    OS_TICK   tick_ctr;


#if (OS_CFG_DYN_TICK_EN == DEF_ENABLED)
    tick_ctr = BSP_OS_TickGet();
#else
    tick_ctr = OSTickCtr;
#endif

    if (opt == OS_OPT_TIME_MATCH) {                             /* MATCH to absolute OSTickCtr value mode               */
        remain = time - tick_ctr;
        if ((remain > OS_TICK_TH_RDY) ||                        /* If delay already occurred, ...                       */
            (remain == 0u)) {
            p_tcb->TickRemain = 0u;
           *p_err             =  OS_ERR_TIME_ZERO_DLY;          /* ... do NOT delay.                                    */
            return;
        }

    } else if (opt == OS_OPT_TIME_PERIODIC) {                   /* PERIODIC mode.                                       */
        if ((tick_ctr - p_tcb->TickCtrPrev) > time) {
            remain             = time;                          /* ... first time we load .TickCtrPrev                  */
            p_tcb->TickCtrPrev = tick_ctr + time;
        } else {
            remain = time - (tick_ctr - p_tcb->TickCtrPrev);
            if ((remain > OS_TICK_TH_RDY) ||                    /* If delay time has already passed, ...                */
                (remain == 0u)) {
                p_tcb->TickCtrPrev += time + (time * ((tick_ctr - p_tcb->TickCtrPrev) / time)); /* Try to recover the period*/
                p_tcb->TickRemain   = 0u;
               *p_err               =  OS_ERR_TIME_ZERO_DLY;    /* ... do NOT delay.                                    */
                return;
            }
            p_tcb->TickCtrPrev += time;
        }

    } else {                                                    /* RELATIVE time delay mode                             */
        remain = time;
    }

   *p_err = OS_ERR_NONE;

    p_tcb->TaskState = OS_TASK_STATE_DLY;
    OS_TickListInsert(&OSTickListDly, p_tcb, remain + (tick_ctr - OSTickCtr));
}

/*
************************************************************************************************************************
*                                         REMOVE A TASK FROM THE TICK LIST
*
* Description: This function is called to remove a task from the tick list
*
* Arguments  : p_tcb          Is a pointer to the OS_TCB to remove.
*              -----
*
* Returns    : none
*
* Note(s)    : 1) This function is INTERNAL to uC/OS-III and your application MUST NOT call it.
*
*              2) This function is assumed to be called with interrupts disabled.
************************************************************************************************************************
*/

void  OS_TickListRemove (OS_TCB  *p_tcb)
{
    OS_TICK_LIST  *p_list;
    OS_TCB        *p_tcb1;
    OS_TCB        *p_tcb2;


    p_list = p_tcb->TickListPtr;
    p_tcb1 = p_tcb->TickPrevPtr;
    p_tcb2 = p_tcb->TickNextPtr;
    if (p_tcb1 == DEF_NULL) {
        if (p_tcb2 == DEF_NULL) {                               /* Remove ONLY entry in the list?                       */
            p_list->TCB_Ptr    = DEF_NULL;
#if (OS_CFG_DBG_EN == DEF_ENABLED)
            p_list->NbrEntries = 0u;
#endif
            p_tcb->TickRemain   = 0u;
            p_tcb->TickListPtr  = DEF_NULL;
        } else {
            p_tcb2->TickPrevPtr = DEF_NULL;
            p_tcb2->TickRemain += p_tcb->TickRemain;            /* Add back the ticks to the delta                      */
            p_list->TCB_Ptr    = p_tcb2;
#if (OS_CFG_DBG_EN == DEF_ENABLED)
            p_list->NbrEntries--;
#endif
            p_tcb->TickNextPtr  = DEF_NULL;
            p_tcb->TickRemain   = 0u;
            p_tcb->TickListPtr  = DEF_NULL;
        }
    } else {
        p_tcb1->TickNextPtr = p_tcb2;
        if (p_tcb2 != DEF_NULL) {
            p_tcb2->TickPrevPtr = p_tcb1;
            p_tcb2->TickRemain += p_tcb->TickRemain;            /* Add back the ticks to the delta list                 */
        }
        p_tcb->TickPrevPtr  = DEF_NULL;
#if (OS_CFG_DBG_EN == DEF_ENABLED)
        p_list->NbrEntries--;
#endif
        p_tcb->TickNextPtr  = DEF_NULL;
        p_tcb->TickRemain   = 0u;
        p_tcb->TickListPtr  = DEF_NULL;
    }
}

/*
************************************************************************************************************************
*                                           UPDATE THE LIST OF TASKS DELAYED
*
* Description: This function updates the delta list which contains tasks that have been delayed.
*
* Arguments  : ticks          the number of ticks which have elapsed.
*
* Returns    : none
*
* Note(s)    : 1) This function is INTERNAL to uC/OS-III and your application MUST NOT call it.
************************************************************************************************************************
*/

static  CPU_TS  OS_TickListUpdateDly (OS_TICK  ticks)
{
    OS_TCB       *p_tcb;
    OS_TICK_LIST *p_list;
#if (OS_CFG_TS_EN == DEF_ENABLED)
    CPU_TS        ts_start;
    CPU_TS        ts_delta_dly;
#endif
#if (OS_CFG_DBG_EN == DEF_ENABLED)
    OS_OBJ_QTY    nbr_updated;
#endif

                                                                /*  ========= UPDATE TASKS WAITING FOR DELAY =========  */
#if (OS_CFG_TS_EN == DEF_ENABLED)
    ts_start    = OS_TS_GET();
#endif
#if (OS_CFG_DBG_EN == DEF_ENABLED)
    nbr_updated = (OS_OBJ_QTY)0u;
#endif
    p_list      = &OSTickListDly;
    p_tcb       = p_list->TCB_Ptr;
    if (p_tcb != DEF_NULL) {
        if (p_tcb->TickRemain <= ticks) {
            ticks = ticks - p_tcb->TickRemain;
            p_tcb->TickRemain = 0u;
        } else {
            p_tcb->TickRemain -= ticks;
        }

        while (p_tcb->TickRemain == 0u) {
#if (OS_CFG_DBG_EN == DEF_ENABLED)
            nbr_updated++;                                      /* Keep track of the number of TCBs updated             */
#endif
            if (p_tcb->TaskState == OS_TASK_STATE_DLY) {
                p_tcb->TaskState = OS_TASK_STATE_RDY;
                OS_RdyListInsert(p_tcb);                        /* Insert the task in the ready list                    */

            } else {
                if (p_tcb->TaskState == OS_TASK_STATE_DLY_SUSPENDED) {
                    p_tcb->TaskState = OS_TASK_STATE_SUSPENDED;
                }
            }

            p_list->TCB_Ptr = p_tcb->TickNextPtr;
            p_tcb           = p_list->TCB_Ptr;                  /* Get 'p_tcb' again for loop                           */
            if (p_tcb == DEF_NULL) {
#if (OS_CFG_DBG_EN == DEF_ENABLED)
                p_list->NbrEntries = 0u;
#endif
                break;
            } else {
#if (OS_CFG_DBG_EN == DEF_ENABLED)
                p_list->NbrEntries--;
#endif
                p_tcb->TickPrevPtr = DEF_NULL;
            }

            if (p_tcb->TickRemain <= ticks) {
                ticks = ticks - p_tcb->TickRemain;
                p_tcb->TickRemain = 0u;
            } else {
                p_tcb->TickRemain -= ticks;
            }
        }
    }
#if (OS_CFG_DBG_EN == DEF_ENABLED)
    p_list->NbrUpdated = nbr_updated;
#endif
#if (OS_CFG_TS_EN == DEF_ENABLED)
    ts_delta_dly       = OS_TS_GET() - ts_start;                /* Measure execution time of the update                 */
#endif

#if (OS_CFG_TS_EN == DEF_ENABLED)
    return (ts_delta_dly);
#else
    return (0u);
#endif
}


/*
************************************************************************************************************************
*                                       UPDATE THE LIST OF TASKS PENDING WITH TIMEOUT
*
* Description: This function updates the delta list which contains tasks that are pending with a timeout.
*
* Arguments  : ticks          the number of ticks which have elapsed.
*
* Returns    : none
*
* Note(s)    : 1) This function is INTERNAL to uC/OS-III and your application MUST NOT call it.
************************************************************************************************************************
*/

static  CPU_TS  OS_TickListUpdateTimeout (OS_TICK  ticks)
{
    OS_TCB       *p_tcb;
    OS_TICK_LIST *p_list;
#if (OS_CFG_TS_EN == DEF_ENABLED)
    CPU_TS        ts_start;
    CPU_TS        ts_delta_timeout;
#endif
#if (OS_CFG_DBG_EN == DEF_ENABLED)
    OS_OBJ_QTY    nbr_updated;
#endif
#if (OS_CFG_MUTEX_EN == DEF_ENABLED)
    OS_TCB       *p_tcb_owner;
    OS_PRIO       prio_new;
#endif

                                                                /*  ======= UPDATE TASKS WAITING WITH TIMEOUT ========  */
#if (OS_CFG_TS_EN == DEF_ENABLED)
    ts_start    = OS_TS_GET();
#endif
#if (OS_CFG_DBG_EN == DEF_ENABLED)
    nbr_updated = 0u;
#endif
    p_list      = &OSTickListTimeout;
    p_tcb       = p_list->TCB_Ptr;
    if (p_tcb != DEF_NULL) {
        if (p_tcb->TickRemain <= ticks) {
            ticks = ticks - p_tcb->TickRemain;
            p_tcb->TickRemain = 0u;
        } else {
            p_tcb->TickRemain -= ticks;
        }

        while (p_tcb->TickRemain == 0u) {
#if (OS_CFG_DBG_EN == DEF_ENABLED)
            nbr_updated++;
#endif

#if (OS_CFG_MUTEX_EN == DEF_ENABLED)
            p_tcb_owner = (OS_TCB *)DEF_NULL;
            if (p_tcb->PendOn == OS_TASK_PEND_ON_MUTEX) {
                p_tcb_owner = (OS_TCB *)((OS_MUTEX *)p_tcb->PendObjPtr)->OwnerTCBPtr;
            }
#endif

#if (OS_MSG_EN == DEF_ENABLED)
            p_tcb->MsgPtr  = DEF_NULL;
            p_tcb->MsgSize = 0u;
#endif
#if (OS_CFG_TS_EN == DEF_ENABLED)
            p_tcb->TS      = OS_TS_GET();
#endif
            OS_PendListRemove(p_tcb);                           /* Remove task from pend list                           */
            if (p_tcb->TaskState == OS_TASK_STATE_PEND_TIMEOUT) {
                OS_RdyListInsert(p_tcb);                        /* Insert the task in the ready list                    */
                p_tcb->TaskState  = OS_TASK_STATE_RDY;

            } else {
                if (p_tcb->TaskState == OS_TASK_STATE_PEND_TIMEOUT_SUSPENDED) {
                    p_tcb->TaskState  = OS_TASK_STATE_SUSPENDED;
                }
            }
            p_tcb->PendStatus = OS_STATUS_PEND_TIMEOUT;         /* Indicate pend timed out                              */
            p_tcb->PendOn     = OS_TASK_PEND_ON_NOTHING;        /* Indicate no longer pending                           */

#if (OS_CFG_MUTEX_EN == DEF_ENABLED)
            if (p_tcb_owner != DEF_NULL) {
                if ((p_tcb_owner->Prio != p_tcb_owner->BasePrio) &&
                    (p_tcb_owner->Prio == p_tcb->Prio)) {       /* Has the owner inherited a priority?                  */
                    prio_new = OS_MutexGrpPrioFindHighest(p_tcb_owner);
                    prio_new = (prio_new > p_tcb_owner->BasePrio) ? p_tcb_owner->BasePrio : prio_new;
                    if(prio_new != p_tcb_owner->Prio) {
                        OS_TaskChangePrio(p_tcb_owner, prio_new);
                        OS_TRACE_MUTEX_TASK_PRIO_DISINHERIT(p_tcb_owner, p_tcb_owner->Prio);
                    }
                }
            }
#endif

            p_list->TCB_Ptr = p_tcb->TickNextPtr;
            p_tcb           = p_list->TCB_Ptr;                  /* Get 'p_tcb' again for loop                           */
            if (p_tcb == DEF_NULL) {
#if (OS_CFG_DBG_EN == DEF_ENABLED)
                p_list->NbrEntries = 0u;
#endif
                break;
            } else {
#if (OS_CFG_DBG_EN == DEF_ENABLED)
                p_list->NbrEntries--;
#endif
                p_tcb->TickPrevPtr = DEF_NULL;
            }
            if (p_tcb->TickRemain <= ticks) {
                ticks = ticks - p_tcb->TickRemain;
                p_tcb->TickRemain = 0u;
            } else {
                p_tcb->TickRemain -= ticks;
            }
        }
    }
#if (OS_CFG_DBG_EN == DEF_ENABLED)
    p_list->NbrUpdated = nbr_updated;
#endif
#if (OS_CFG_TS_EN == DEF_ENABLED)
    ts_delta_timeout   = OS_TS_GET() - ts_start;                /* Measure execution time of the update                 */
#endif

#if (OS_CFG_TS_EN == DEF_ENABLED)
    return (ts_delta_timeout);
#else
    return (0u);
#endif
}
#endif
