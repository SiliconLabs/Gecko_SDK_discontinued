/*
************************************************************************************************************************
*                                                      uC/OS-III
*                                                 The Real-Time Kernel
*
*                                  (c) Copyright 2009-2016; Micrium, Inc.; Weston, FL
*                           All rights reserved.  Protected by international copyright laws.
*
*                                                 MONITOR MANAGEMENT
*
* File    : OS_MON.C
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
const  CPU_CHAR  *os_mon__c = "$Id: $";
#endif

#if (OS_CFG_MON_EN == DEF_ENABLED)
/*
************************************************************************************************************************
*                                                  CREATE A MONITOR
*
* Description: This function creates a monitor.
*
* Arguments  : p_mon         Pointer to the monitor to initialize.  Your application is responsible for
*                            allocating storage for the monitor.
*
*              p_name        Pointer to the name you would like to give the monitor.
*
*              p_mon_data    Pointer to the monitor global data.
*
*              p_err         Pointer to a variable that will contain an error code returned by this function.
*
*                                OS_ERR_NONE                    if the call was successful
*                                OS_ERR_CREATE_ISR              if you called this function from an ISR
*                                OS_ERR_ILLEGAL_CREATE_RUN_TIME if you are trying to create the monitor after you
*                                                                 called OSSafetyCriticalStart().
*                                OS_ERR_NAME                    if 'p_name' is a NULL pointer
*                                OS_ERR_OBJ_CREATED             if the monitor has already been created
*                                OS_ERR_OBJ_PTR_NULL            if 'p_mon'  is a NULL pointer
*                                OS_ERR_OBJ_TYPE                if 'p_mon' has already been initialized to a different
*                                                               object type
*
* Returns    : none
*
* Note(s)    : none
************************************************************************************************************************
*/

void  OSMonCreate (OS_MON   *p_mon,
                   CPU_CHAR *p_name,
                   void     *p_mon_data,
                   OS_ERR   *p_err)
{
    CPU_SR_ALLOC();


#ifdef OS_SAFETY_CRITICAL
    if (p_err == DEF_NULL) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return;
    }
#endif

#ifdef OS_SAFETY_CRITICAL_IEC61508
    if (OSSafetyCriticalStartFlag == DEF_TRUE) {
       *p_err = OS_ERR_ILLEGAL_CREATE_RUN_TIME;
        return;
    }
#endif

#if (OS_CFG_CALLED_FROM_ISR_CHK_EN == DEF_ENABLED)
    if (OSIntNestingCtr > 0u) {                                 /* Not allowed to be called from an ISR                 */
       *p_err = OS_ERR_CREATE_ISR;
        return;
    }
#endif

#if (OS_CFG_ARG_CHK_EN == DEF_ENABLED)
    if (p_mon == DEF_NULL) {                                    /* Validate 'p_mon'                                     */
       *p_err = OS_ERR_OBJ_PTR_NULL;
        return;
    }
#endif

    CPU_CRITICAL_ENTER();
#if (OS_OBJ_TYPE_REQ == DEF_ENABLED)
    p_mon->Type    = OS_OBJ_TYPE_MON;                           /* Mark the data structure as a monitor                 */
#endif

#if (OS_CFG_DBG_EN == DEF_ENABLED)
    p_mon->NamePtr = p_name;                                    /* Save the name of the monitor                         */
#else
    (void)p_name;
#endif

    OS_PendListInit(&p_mon->PendList);

#if (OS_CFG_DBG_EN == DEF_ENABLED)
    OS_MonDbgListAdd(p_mon);
    OSMonQty++;
#endif

    if (p_mon_data != DEF_NULL) {
        p_mon->MonDataPtr = p_mon_data;
    }

    CPU_CRITICAL_EXIT();

   *p_err = OS_ERR_NONE;
}


/*
************************************************************************************************************************
*                                                  DELETE A MONITOR
*
* Description: This function deletes a monitor.
*
* Arguments  : p_mon         is a pointer to the monitor to delete
*
*              opt           determines delete options as follows:
*
*                                OS_OPT_DEL_NO_PEND          Delete monitor ONLY if no task pending
*                                OS_OPT_DEL_ALWAYS           Deletes the monitor even if tasks are waiting.
*                                                            In this case, all the tasks pending will be readied.
*
*              p_err         is a pointer to a variable that will contain an error code returned by this function.
*
*                                OS_ERR_NONE                    The call was successful and the monitor was deleted
*                                OS_ERR_DEL_ISR                 If you attempted to delete the monitor from an ISR
*                                OS_ERR_ILLEGAL_DEL_RUN_TIME    If you are trying to delete the monitor after you called
*                                                                 OSStart()
*                                OS_ERR_OBJ_PTR_NULL            If 'p_mon' is a NULL pointer
*                                OS_ERR_OBJ_TYPE                If 'p_mon' is not pointing at a monitor
*                                OS_ERR_OPT_INVALID             An invalid option was specified
*                                OS_ERR_OS_NOT_RUNNING          If uC/OS-III is not running yet
*                                OS_ERR_TASK_WAITING            One or more tasks were waiting on the monitor
*
* Returns    : == 0          if no tasks were waiting on the monitor, or upon error.
*              >  0          if one or more tasks waiting on the monitor are now readied and informed.
*
* Note(s)    : 1) This function must be used with care.  Tasks that would normally expect the presence of the monitor
*                 MUST check the return code of OSMonOp().
*              2) Because ALL tasks pending on the monitor will be readied, you MUST be careful in applications where
*                 the monitor is used for mutual exclusion because the resource(s) will no longer be guarded by the
*                 monitor.
************************************************************************************************************************
*/

#if (OS_CFG_MON_DEL_EN == DEF_ENABLED)
OS_OBJ_QTY  OSMonDel (OS_MON  *p_mon,
                      OS_OPT   opt,
                      OS_ERR  *p_err)
{
    OS_OBJ_QTY     nbr_tasks;
    OS_PEND_LIST  *p_pend_list;
    OS_TCB        *p_tcb;
    CPU_SR_ALLOC();


#ifdef OS_SAFETY_CRITICAL
    if (p_err == DEF_NULL) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return (0u);
    }
#endif

#ifdef OS_SAFETY_CRITICAL_IEC61508
    if (OSSafetyCriticalStartFlag == DEF_TRUE) {
       *p_err = OS_ERR_ILLEGAL_DEL_RUN_TIME;
        return (0u);
    }
#endif

#if (OS_CFG_CALLED_FROM_ISR_CHK_EN == DEF_ENABLED)
    if (OSIntNestingCtr > 0u) {                                 /* Not allowed to delete a monitor from an ISR          */
       *p_err = OS_ERR_DEL_ISR;
        return (0u);
    }
#endif

#if (OS_CFG_INVALID_OS_CALLS_CHK_EN == DEF_ENABLED)             /* Is the kernel running?                               */
    if (OSRunning != OS_STATE_OS_RUNNING) {
       *p_err = OS_ERR_OS_NOT_RUNNING;
        return (0u);
    }
#endif

#if (OS_CFG_ARG_CHK_EN == DEF_ENABLED)
    if (p_mon == DEF_NULL) {                                    /* Validate 'p_mon'                                     */
       *p_err = OS_ERR_OBJ_PTR_NULL;
        return (0u);
    }
#endif

#if (OS_CFG_OBJ_TYPE_CHK_EN == DEF_ENABLED)
    if (p_mon->Type != OS_OBJ_TYPE_MON) {                       /* Make sure monitor was created                        */
       *p_err = OS_ERR_OBJ_TYPE;
        return (0u);
    }
#endif

    CPU_CRITICAL_ENTER();
    p_pend_list = &p_mon->PendList;
    nbr_tasks   = 0u;
    switch (opt) {
         case OS_OPT_DEL_NO_PEND:                                /* Delete monitor only if no task waiting               */
              if (p_pend_list->HeadPtr == DEF_NULL) {
 #if (OS_CFG_DBG_EN == DEF_ENABLED)
                  OS_MonDbgListRemove(p_mon);
                  OSMonQty--;
 #endif
                  OS_MonClr(p_mon);
                  CPU_CRITICAL_EXIT();
                 *p_err = OS_ERR_NONE;
              } else {
                  CPU_CRITICAL_EXIT();
                 *p_err = OS_ERR_TASK_WAITING;
              }
              break;

         case OS_OPT_DEL_ALWAYS:                                 /* Always delete the monitor                            */
              while (p_pend_list->HeadPtr != DEF_NULL) {         /* Remove all tasks on the pend list                    */
                  p_tcb = p_pend_list->HeadPtr;
                  OS_PendAbort(p_tcb,
                               0,
                               OS_STATUS_PEND_DEL);
                  nbr_tasks++;
              }
 #if (OS_CFG_DBG_EN == DEF_ENABLED)
              OS_MonDbgListRemove(p_mon);
              OSMonQty--;
 #endif
              OS_MonClr(p_mon);
              CPU_CRITICAL_EXIT();
              OSSched();                                         /* Find highest priority task ready to run              */
             *p_err = OS_ERR_NONE;
              break;

         default:
              CPU_CRITICAL_EXIT();
             *p_err = OS_ERR_OPT_INVALID;
              break;
     }
     return (nbr_tasks);
}
#endif


/*
************************************************************************************************************************
*                                               PERFORM AND OPERATION ON A MONITOR
*
* Description: This function performs an operation on a monitor.
*
* Arguments  : p_mon      Pointer to the monitor
*
*              timeout    Optional timeout to be applied if the monitor blocks (pend).
*
*              p_arg      Argument of the monitor.
*
*              p_on_enter Callback called at the entry of the OSMonOp.
*
*              p_on_eval  Callback to be registered as the monitor's evaluation function.
*
*              p_mon_ctx  Monitor context. Unused should be DEF_NULL.
*
*              opt        Possible options are :
*                           OS_OPT_POST_NO_SCHED     Do not call the scheduler
*
*              p_err      Pointer to a variable that will contain an error code returned by this function.
*
*                           OS_ERR_NONE          The call was successful and the monitor was signaled.
*                           OS_ERR_OBJ_PTR_NULL  If 'p_mon' is a NULL pointer.
*                           OS_ERR_OBJ_TYPE      If 'p_mon' is not pointing at a monitor
*
* Returns    : none.
*
* Note(s)    : none.
************************************************************************************************************************
*/

void  OSMonOp (OS_MON               *p_mon,
               OS_TICK               timeout,
               void                 *p_arg,
               OS_MON_ON_ENTER_PTR   p_on_enter,
               OS_MON_ON_EVAL_PTR    p_on_eval,
               OS_OPT                opt,
               OS_ERR               *p_err)
{
    CPU_INT32U     op_res;
    CPU_INT32U     mon_res;
    OS_PEND_LIST  *p_pend_list;
    OS_TCB        *p_tcb;
    OS_TCB        *p_tcb_next;
    void          *p_eval_data;
    CPU_BOOLEAN    sched;
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

#if (OS_CFG_ARG_CHK_EN == DEF_ENABLED)
    if (p_mon == DEF_NULL) {                                    /* Validate 'p_mon'                                     */
       *p_err  = OS_ERR_OBJ_PTR_NULL;
        return;
    }
#endif

    sched = DEF_NO;

    CPU_CRITICAL_ENTER();

    if (p_on_enter != DEF_NULL) {
        op_res = (*p_on_enter)(p_mon, p_arg);
    } else {
        op_res = OS_MON_RES_BLOCK | OS_MON_RES_STOP_EVAL;
    }

    if (DEF_BIT_IS_SET(op_res, OS_MON_RES_BLOCK) == DEF_YES) {
        OS_Pend((OS_PEND_OBJ *)(p_mon),                         /* Block task pending on Condition Variable             */
                OS_TASK_PEND_ON_COND_VAR,
                timeout);

        sched = DEF_YES;
    }

    OSTCBCurPtr->MonData.p_eval_data = p_arg;
    OSTCBCurPtr->MonData.p_on_eval   = p_on_eval;

    if (DEF_BIT_IS_CLR(op_res, OS_MON_RES_STOP_EVAL) == DEF_YES) {
        p_pend_list = &p_mon->PendList;
        if (p_pend_list->HeadPtr != DEF_NULL) {
            p_tcb = p_pend_list->HeadPtr;
            while (p_tcb != DEF_NULL) {
                p_tcb_next = p_tcb->PendNextPtr;

                p_on_eval   = p_tcb->MonData.p_on_eval;
                p_eval_data = p_tcb->MonData.p_eval_data;

                if (p_on_eval != DEF_NULL) {
                    mon_res = (*p_on_eval)(p_mon, p_eval_data, p_arg);
                } else {
                    mon_res = OS_MON_RES_STOP_EVAL;
                }

                if (DEF_BIT_IS_CLR(mon_res, OS_MON_RES_BLOCK) == DEF_YES) {
                    OS_Post((OS_PEND_OBJ *)(p_mon), p_tcb, DEF_NULL, 0u, 0u);
                    if (DEF_BIT_IS_CLR(opt, OS_OPT_POST_NO_SCHED) == DEF_YES) {
                        sched = DEF_YES;
                    }
                }

                if (DEF_BIT_IS_SET(mon_res, OS_MON_RES_STOP_EVAL) == DEF_YES) {
                    break;
                }

                p_tcb = p_tcb_next;
            }
        }
    }

    CPU_CRITICAL_EXIT();

    if (sched == DEF_YES) {
        OSSched();                                              /* Find the next highest priority task ready to run     */
    }

    if (DEF_BIT_IS_SET(op_res, OS_MON_RES_BLOCK) == DEF_YES) {
        CPU_CRITICAL_ENTER();
        switch (OSTCBCurPtr->PendStatus) {
            case OS_STATUS_PEND_OK:                             /* We got the monitor                                   */
                *p_err = OS_ERR_NONE;
                 break;

            case OS_STATUS_PEND_ABORT:                          /* Indicate that we aborted                             */
                *p_err = OS_ERR_PEND_ABORT;
                 break;

            case OS_STATUS_PEND_TIMEOUT:                        /* Indicate that we didn't get monitor within timeout   */
                *p_err = OS_ERR_TIMEOUT;
                 break;

            case OS_STATUS_PEND_DEL:                            /* Indicate that object pended on has been deleted      */
                *p_err = OS_ERR_OBJ_DEL;
                 break;

            default:
                *p_err = OS_ERR_STATUS_INVALID;
        }
        CPU_CRITICAL_EXIT();
    } else {
       *p_err = OS_ERR_NONE;
    }
}


/*
************************************************************************************************************************
*                                           CLEAR THE CONTENTS OF A MONITOR
*
* Description: This function is called by OSMonDel() to clear the contents of a monitor
*

* Argument(s): p_mon      is a pointer to the monitor to clear
*              -----
*
* Returns    : none
*
* Note(s)    : 1) This function is INTERNAL to uC/OS-III and your application MUST NOT call it.
************************************************************************************************************************
*/

void  OS_MonClr (OS_MON  *p_mon)
{
#if (OS_OBJ_TYPE_REQ == DEF_ENABLED)
    p_mon->Type       = OS_OBJ_TYPE_NONE;                       /* Mark the data structure as a NONE                    */
#endif
    p_mon->MonDataPtr = DEF_NULL;
#if (OS_CFG_DBG_EN == DEF_ENABLED)
    p_mon->NamePtr    = (CPU_CHAR *)((void *)"?MON");
#endif
    OS_PendListInit(&p_mon->PendList);                          /* Initialize the waiting list                          */
}


/*
************************************************************************************************************************
*                                        ADD/REMOVE MONITOR TO/FROM DEBUG LIST
*
* Description: These functions are called by uC/OS-III to add or remove a monitor to/from the debug list.
*
* Arguments  : p_mon     is a pointer to the monitor to add/remove
*
* Returns    : none
*
* Note(s)    : These functions are INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

#if (OS_CFG_DBG_EN == DEF_ENABLED)
void  OS_MonDbgListAdd (OS_MON  *p_mon)
{
    p_mon->DbgNamePtr               = (CPU_CHAR *)((void *)" ");
    p_mon->DbgPrevPtr               = DEF_NULL;
    if (OSMonDbgListPtr == DEF_NULL) {
        p_mon->DbgNextPtr           = DEF_NULL;
    } else {
        p_mon->DbgNextPtr           =  OSMonDbgListPtr;
        OSMonDbgListPtr->DbgPrevPtr =  p_mon;
    }
    OSMonDbgListPtr                 =  p_mon;
}


void  OS_MonDbgListRemove (OS_MON  *p_mon)
{
    OS_MON  *p_mon_next;
    OS_MON  *p_mon_prev;


    p_mon_prev = p_mon->DbgPrevPtr;
    p_mon_next = p_mon->DbgNextPtr;

    if (p_mon_prev == DEF_NULL) {
        OSMonDbgListPtr = p_mon_next;
        if (p_mon_next != DEF_NULL) {
            p_mon_next->DbgPrevPtr = DEF_NULL;
        }
        p_mon->DbgNextPtr = DEF_NULL;

    } else if (p_mon_next == DEF_NULL) {
        p_mon_prev->DbgNextPtr = DEF_NULL;
        p_mon->DbgPrevPtr      = DEF_NULL;

    } else {
        p_mon_prev->DbgNextPtr =  p_mon_next;
        p_mon_next->DbgPrevPtr =  p_mon_prev;
        p_mon->DbgNextPtr      = DEF_NULL;
        p_mon->DbgPrevPtr      = DEF_NULL;
    }
}
#endif
#endif
