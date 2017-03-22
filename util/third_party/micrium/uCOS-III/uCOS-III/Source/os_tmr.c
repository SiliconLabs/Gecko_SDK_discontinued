/*
************************************************************************************************************************
*                                                      uC/OS-III
*                                                 The Real-Time Kernel
*
*                                  (c) Copyright 2009-2016; Micrium, Inc.; Weston, FL
*                           All rights reserved.  Protected by international copyright laws.
*
*                                                   TIMER MANAGEMENT
*
* File    : OS_TMR.C
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
const  CPU_CHAR  *os_tmr__c = "$Id: $";
#endif


#if (OS_CFG_TMR_EN == DEF_ENABLED)
/*
************************************************************************************************************************
*                                               LOCAL FUNCTION PROTOTYPES
************************************************************************************************************************
*/

static  void  OS_TmrLock   (void);
static  void  OS_TmrUnlock (void);


/*
************************************************************************************************************************
*                                                   CREATE A TIMER
*
* Description: This function is called by your application code to create a timer.
*
* Arguments  : p_tmr           Is a pointer to a timer control block
*
*              p_name          Is a pointer to an ASCII string that is used to name the timer.  Names are useful for
*                              debugging.
*
*              dly             Initial delay.
*                              If the timer is configured for ONE-SHOT mode, this is the timeout used
*                              If the timer is configured for PERIODIC mode, this is the first timeout to wait for
*                              before the timer starts entering periodic mode
*
*              period          The 'period' being repeated for the timer.
*                              If you specified 'OS_OPT_TMR_PERIODIC' as an option, when the timer expires, it will
*                              automatically restart with the same period.
*
*              opt             Specifies either:
*
*                                  OS_OPT_TMR_ONE_SHOT       The timer counts down only once
*                                  OS_OPT_TMR_PERIODIC       The timer counts down and then reloads itself
*
*              p_callback      Is a pointer to a callback function that will be called when the timer expires.  The
*                              callback function must be declared as follows:
*
*                                  void  MyCallback (OS_TMR *p_tmr, void *p_arg);
*
*              p_callback_arg  Is an argument (a pointer) that is passed to the callback function when it is called.
*
*              p_err           Is a pointer to an error code.  '*p_err' will contain one of the following:
*
*                                 OS_ERR_NONE                    The call succeeded
*                                 OS_ERR_ILLEGAL_CREATE_RUN_TIME If you are trying to create the timer after you called
*                                                                  OSSafetyCriticalStart()
*                                 OS_ERR_OBJ_PTR_NULL            Is 'p_tmr' is a NULL pointer
*                                 OS_ERR_OPT_INVALID             You specified an invalid option
*                                 OS_ERR_TMR_INVALID_CALLBACK    You specified an invalid callback for a periodic timer
*                                 OS_ERR_TMR_INVALID_DLY         You specified an invalid delay
*                                 OS_ERR_TMR_INVALID_PERIOD      You specified an invalid period
*                                 OS_ERR_TMR_ISR                 If the call was made from an ISR
*
* Returns    : none
*
* Note(s)    : 1) This function only creates the timer.  In other words, the timer is not started when created.  To
*                 start the timer, call OSTmrStart().
************************************************************************************************************************
*/

void  OSTmrCreate (OS_TMR               *p_tmr,
                   CPU_CHAR             *p_name,
                   OS_TICK               dly,
                   OS_TICK               period,
                   OS_OPT                opt,
                   OS_TMR_CALLBACK_PTR   p_callback,
                   void                 *p_callback_arg,
                   OS_ERR               *p_err)
{
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
    if (OSIntNestingCtr > 0u) {                                 /* See if trying to call from an ISR                    */
       *p_err = OS_ERR_TMR_ISR;
        return;
    }
#endif

#if (OS_CFG_ARG_CHK_EN == DEF_ENABLED)
    if (p_tmr == DEF_NULL) {                                    /* Validate 'p_tmr'                                     */
       *p_err = OS_ERR_OBJ_PTR_NULL;
        return;
    }

    switch (opt) {
        case OS_OPT_TMR_PERIODIC:
             if (period == 0u) {
                *p_err = OS_ERR_TMR_INVALID_PERIOD;
                 return;
             }

             if (p_callback == DEF_NULL) {                      /* No point in a periodic timer without a callback      */
                *p_err = OS_ERR_TMR_INVALID_CALLBACK;
                 return;
             }
             break;

        case OS_OPT_TMR_ONE_SHOT:
             if (dly == 0u) {
                *p_err = OS_ERR_TMR_INVALID_DLY;
                 return;
             }
             break;

        default:
            *p_err = OS_ERR_OPT_INVALID;
             return;
    }
#endif

    if (OSRunning == OS_STATE_OS_RUNNING) {                     /* Only lock when the kernel is running                 */
        OS_TmrLock();
    }

    p_tmr->State          = OS_TMR_STATE_STOPPED;               /* Initialize the timer fields                          */
#if (OS_OBJ_TYPE_REQ == DEF_ENABLED)
    p_tmr->Type           = OS_OBJ_TYPE_TMR;
#endif
#if (OS_CFG_DBG_EN == DEF_ENABLED)
    p_tmr->NamePtr        = p_name;
#else
    (void)p_name;
#endif
    p_tmr->Dly            = dly;
    p_tmr->Remain         = 0u;
    p_tmr->Period         = period;
    p_tmr->Opt            = opt;
    p_tmr->CallbackPtr    = p_callback;
    p_tmr->CallbackPtrArg = p_callback_arg;
    p_tmr->NextPtr        = DEF_NULL;
    p_tmr->PrevPtr        = DEF_NULL;

#if (OS_CFG_DBG_EN == DEF_ENABLED)
    OS_TmrDbgListAdd(p_tmr);
#endif
#if (OS_CFG_DBG_EN == DEF_ENABLED)
    OSTmrQty++;                                                 /* Keep track of the number of timers created           */
#endif

    if (OSRunning == OS_STATE_OS_RUNNING) {
        OS_TmrUnlock();
    }

   *p_err = OS_ERR_NONE;
}


/*
************************************************************************************************************************
*                                                   DELETE A TIMER
*
* Description: This function is called by your application code to delete a timer.
*
* Arguments  : p_tmr          Is a pointer to the timer to stop and delete.
*
*              p_err          Is a pointer to an error code.  '*p_err' will contain one of the following:
*
*                                 OS_ERR_NONE                    The call succeeded
*                                 OS_ERR_ILLEGAL_DEL_RUN_TIME    If you are trying to delete the timer after you called
*                                                                  OSStart()
*                                 OS_ERR_OBJ_TYPE                If 'p_tmr' is not pointing to a timer
*                                 OS_ERR_OS_NOT_RUNNING          If uC/OS-III is not running yet
*                                 OS_ERR_TMR_INACTIVE            If the timer was not created
*                                 OS_ERR_TMR_INVALID             If 'p_tmr' is a NULL pointer
*                                 OS_ERR_TMR_INVALID_STATE       The timer is in an invalid state
*                                 OS_ERR_TMR_ISR                 If the function was called from an ISR
*
* Returns    : DEF_TRUE   if the timer was deleted
*              DEF_FALSE  if not or upon an error
*
* Note(s)    : none
************************************************************************************************************************
*/

#if (OS_CFG_TMR_DEL_EN == DEF_ENABLED)
CPU_BOOLEAN  OSTmrDel (OS_TMR  *p_tmr,
                       OS_ERR  *p_err)
{
    CPU_BOOLEAN  success;



#ifdef OS_SAFETY_CRITICAL
    if (p_err == DEF_NULL) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return (DEF_FALSE);
    }
#endif

#ifdef OS_SAFETY_CRITICAL_IEC61508
    if (OSSafetyCriticalStartFlag == DEF_TRUE) {
       *p_err = OS_ERR_ILLEGAL_DEL_RUN_TIME;
        return (DEF_FALSE);
    }
#endif

#if (OS_CFG_CALLED_FROM_ISR_CHK_EN == DEF_ENABLED)
    if (OSIntNestingCtr > 0u) {                                 /* See if trying to call from an ISR                    */
       *p_err  = OS_ERR_TMR_ISR;
        return (DEF_FALSE);
    }
#endif

#if (OS_CFG_INVALID_OS_CALLS_CHK_EN == DEF_ENABLED)             /* Is the kernel running?                               */
    if (OSRunning != OS_STATE_OS_RUNNING) {
       *p_err = OS_ERR_OS_NOT_RUNNING;
        return (DEF_FALSE);
    }
#endif

#if (OS_CFG_ARG_CHK_EN == DEF_ENABLED)
    if (p_tmr == DEF_NULL) {
       *p_err = OS_ERR_TMR_INVALID;
        return (DEF_FALSE);
    }
#endif

#if (OS_CFG_OBJ_TYPE_CHK_EN == DEF_ENABLED)
    if (p_tmr->Type != OS_OBJ_TYPE_TMR) {                       /* Make sure timer was created                          */
       *p_err = OS_ERR_OBJ_TYPE;
        return (DEF_FALSE);
    }
#endif

    OS_TmrLock();

#if (OS_CFG_DBG_EN == DEF_ENABLED)
    OS_TmrDbgListRemove(p_tmr);
#endif

    switch (p_tmr->State) {
        case OS_TMR_STATE_RUNNING:
             OS_TmrUnlink(p_tmr);                               /* Remove from the list                                 */
             OS_TmrClr(p_tmr);
#if (OS_CFG_DBG_EN == DEF_ENABLED)
             OSTmrQty--;                                        /* One less timer                                       */
#endif
            *p_err   = OS_ERR_NONE;
             success = DEF_TRUE;
             break;

        case OS_TMR_STATE_STOPPED:                              /* Timer has not started or ...                         */
        case OS_TMR_STATE_COMPLETED:                            /* ... timer has completed the ONE-SHOT time            */
             OS_TmrClr(p_tmr);                                  /* Clear timer fields                                   */
#if (OS_CFG_DBG_EN == DEF_ENABLED)
             OSTmrQty--;                                        /* One less timer                                       */
#endif
            *p_err   = OS_ERR_NONE;
             success = DEF_TRUE;
             break;

        case OS_TMR_STATE_UNUSED:                               /* Already deleted                                      */
            *p_err   = OS_ERR_TMR_INACTIVE;
             success = DEF_FALSE;
             break;

        default:
            *p_err   = OS_ERR_TMR_INVALID_STATE;
             success = DEF_FALSE;
             break;
    }

    OS_TmrUnlock();

    return (success);
}
#endif


/*
************************************************************************************************************************
*                                    GET HOW MUCH TIME IS LEFT BEFORE A TIMER EXPIRES
*
* Description: This function is called to get the number of ticks before a timer times out.
*
* Arguments  : p_tmr    Is a pointer to the timer to obtain the remaining time from.
*
*              p_err    Is a pointer to an error code.  '*p_err' will contain one of the following:
*
*                           OS_ERR_NONE               The call succeeded
*                           OS_ERR_OBJ_TYPE           If 'p_tmr' is not pointing to a timer
*                           OS_ERR_OS_NOT_RUNNING     If uC/OS-III is not running yet
*                           OS_ERR_TMR_INACTIVE       If 'p_tmr' points to a timer that is not active
*                           OS_ERR_TMR_INVALID        If 'p_tmr' is a NULL pointer
*                           OS_ERR_TMR_INVALID_STATE  The timer is in an invalid state
*                           OS_ERR_TMR_ISR            If the call was made from an ISR
*
* Returns    : The time remaining for the timer to expire.  The time represents 'timer' increments.  In other words, if
*              OS_TmrTask() is signaled every 1/10 of a second then the returned value represents the number of 1/10 of
*              a second remaining before the timer expires.
*
* Note(s)    : none
************************************************************************************************************************
*/

OS_TICK  OSTmrRemainGet (OS_TMR  *p_tmr,
                         OS_ERR  *p_err)
{
    OS_TICK  remain;


#ifdef OS_SAFETY_CRITICAL
    if (p_err == DEF_NULL) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return (0u);
    }
#endif

#if (OS_CFG_CALLED_FROM_ISR_CHK_EN == DEF_ENABLED)
    if (OSIntNestingCtr > 0u) {                                 /* See if trying to call from an ISR                    */
       *p_err = OS_ERR_TMR_ISR;
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
    if (p_tmr == DEF_NULL) {
       *p_err = OS_ERR_TMR_INVALID;
        return (0u);
    }
#endif

#if (OS_CFG_OBJ_TYPE_CHK_EN == DEF_ENABLED)
    if (p_tmr->Type != OS_OBJ_TYPE_TMR) {                       /* Make sure timer was created                          */
       *p_err = OS_ERR_OBJ_TYPE;
        return (0u);
    }
#endif

    OS_TmrLock();

    switch (p_tmr->State) {
        case OS_TMR_STATE_RUNNING:
             remain = p_tmr->Remain;
            *p_err  = OS_ERR_NONE;
             break;

        case OS_TMR_STATE_STOPPED:                              /* It's assumed that the timer has not started yet      */
             if (p_tmr->Opt == OS_OPT_TMR_PERIODIC) {
                 if (p_tmr->Dly == 0u) {
                     remain = p_tmr->Period;
                 } else {
                     remain = p_tmr->Dly;
                 }
             } else {
                 remain = p_tmr->Dly;
             }
            *p_err = OS_ERR_NONE;
             break;

        case OS_TMR_STATE_COMPLETED:                            /* Only ONE-SHOT that timed out can be in this state    */
            *p_err  = OS_ERR_NONE;
             remain = 0u;
             break;

        case OS_TMR_STATE_UNUSED:
            *p_err  = OS_ERR_TMR_INACTIVE;
             remain = 0u;
             break;

        default:
            *p_err = OS_ERR_TMR_INVALID_STATE;
             remain = 0u;
             break;
    }

    OS_TmrUnlock();

    return (remain);
}


/*
************************************************************************************************************************
*                                                    SET A TIMER
*
* Description: This function is called by your application code to set a timer.
*
* Arguments  : p_tmr           Is a pointer to a timer control block
*
*              dly             Initial delay.
*                              If the timer is configured for ONE-SHOT mode, this is the timeout used
*                              If the timer is configured for PERIODIC mode, this is the first timeout to wait for
*                              before the timer starts entering periodic mode
*
*              period          The 'period' being repeated for the timer.
*                              If you specified 'OS_OPT_TMR_PERIODIC' as an option, when the timer expires, it will
*                              automatically restart with the same period.
*
*              p_callback      Is a pointer to a callback function that will be called when the timer expires.  The
*                              callback function must be declared as follows:
*
*                                  void  MyCallback (OS_TMR *p_tmr, void *p_arg);
*
*              p_callback_arg  Is an argument (a pointer) that is passed to the callback function when it is called.
*
*              p_err           Is a pointer to an error code.  '*p_err' will contain one of the following:
*
*                                 OS_ERR_NONE                    The timer was configured as expected
*                                 OS_ERR_OBJ_TYPE                If the object type is invalid
*                                 OS_ERR_OS_NOT_RUNNING          If uC/OS-III is not running yet
*                                 OS_ERR_TMR_INVALID             If 'p_tmr' is a NULL pointer or invalid option
*                                 OS_ERR_TMR_INVALID_CALLBACK    you specified an invalid callback for a periodic timer
*                                 OS_ERR_TMR_INVALID_DLY         You specified an invalid delay
*                                 OS_ERR_TMR_INVALID_PERIOD      You specified an invalid period
*                                 OS_ERR_TMR_ISR                 If the call was made from an ISR
*
* Returns    : none
*
* Note(s)    : 1) This function can be called on a running timer. The change to the delay and period will only
*                 take effect after the current period or delay has passed. Change to the callback will take
*                 effect immediately.
************************************************************************************************************************
*/

void  OSTmrSet (OS_TMR               *p_tmr,
                OS_TICK               dly,
                OS_TICK               period,
                OS_TMR_CALLBACK_PTR   p_callback,
                void                 *p_callback_arg,
                OS_ERR               *p_err)
{
#ifdef OS_SAFETY_CRITICAL
    if (p_err == DEF_NULL) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return;
    }
#endif

#if (OS_CFG_CALLED_FROM_ISR_CHK_EN == DEF_ENABLED)
    if (OSIntNestingCtr > 0u) {                                 /* See if trying to call from an ISR                    */
       *p_err = OS_ERR_TMR_ISR;
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
    if (p_tmr == DEF_NULL) {                                    /* Validate 'p_tmr'                                     */
       *p_err = OS_ERR_TMR_INVALID;
        return;
    }
#endif

#if (OS_CFG_OBJ_TYPE_CHK_EN == DEF_ENABLED)
    if (p_tmr->Type != OS_OBJ_TYPE_TMR) {                       /* Make sure timer was created                          */
       *p_err = OS_ERR_OBJ_TYPE;
        return;
    }
#endif

#if (OS_CFG_ARG_CHK_EN == DEF_ENABLED)
    switch (p_tmr->Opt) {
        case OS_OPT_TMR_PERIODIC:
             if (period == 0u) {
                *p_err = OS_ERR_TMR_INVALID_PERIOD;
                 return;
             }

             if (p_callback == DEF_NULL) {                      /* No point in a periodic timer without a callback      */
                *p_err = OS_ERR_TMR_INVALID_CALLBACK;
                 return;
             }
             break;

        case OS_OPT_TMR_ONE_SHOT:
             if (dly == 0u) {
                *p_err = OS_ERR_TMR_INVALID_DLY;
                 return;
             }
             break;

        default:
            *p_err = OS_ERR_TMR_INVALID;
             return;
    }
#endif

    OS_TmrLock();

    p_tmr->Dly            = dly;
    p_tmr->Period         = period;
    p_tmr->CallbackPtr    = p_callback;
    p_tmr->CallbackPtrArg = p_callback_arg;

   *p_err = OS_ERR_NONE;

    OS_TmrUnlock();

}


/*
************************************************************************************************************************
*                                                   START A TIMER
*
* Description: This function is called by your application code to start a timer.
*
* Arguments  : p_tmr    Is a pointer to an OS_TMR
*
*              p_err    Is a pointer to an error code.  '*p_err' will contain one of the following:
*
*                           OS_ERR_NONE                The timer was started
*                           OS_ERR_OBJ_TYPE            If 'p_tmr' is not pointing to a timer
*                           OS_ERR_OS_NOT_RUNNING      If uC/OS-III is not running yet
*                           OS_ERR_TMR_INACTIVE        If the timer was not created
*                           OS_ERR_TMR_INVALID         If 'p_tmr' is a NULL pointer
*                           OS_ERR_TMR_INVALID_STATE   The timer is in an invalid state
*                           OS_ERR_TMR_ISR             If the call was made from an ISR
*
* Returns    : DEF_TRUE      is the timer was started
*              DEF_FALSE     if not or upon an error
*
* Note(s)    : 1) When starting/restarting a timer, regardless if it is in PERIODIC or ONE-SHOT mode, the timer is
*                 linked to the timer list with the OS_OPT_LINK_DLY option. This option sets the initial expiration
*                 time for the timer. For timers in PERIODIC mode, subsequent expiration times are handled by
*                 the OS_TmrTask().
************************************************************************************************************************
*/

CPU_BOOLEAN  OSTmrStart (OS_TMR  *p_tmr,
                         OS_ERR  *p_err)
{
    OS_TMR      *p_next;
    CPU_BOOLEAN  success;



#ifdef OS_SAFETY_CRITICAL
    if (p_err == DEF_NULL) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return (DEF_FALSE);
    }
#endif

#if (OS_CFG_CALLED_FROM_ISR_CHK_EN == DEF_ENABLED)
    if (OSIntNestingCtr > 0u) {                                 /* See if trying to call from an ISR                    */
       *p_err = OS_ERR_TMR_ISR;
        return (DEF_FALSE);
    }
#endif

#if (OS_CFG_INVALID_OS_CALLS_CHK_EN == DEF_ENABLED)             /* Is the kernel running?                               */
    if (OSRunning != OS_STATE_OS_RUNNING) {
       *p_err = OS_ERR_OS_NOT_RUNNING;
        return (DEF_FALSE);
    }
#endif

#if (OS_CFG_ARG_CHK_EN == DEF_ENABLED)
    if (p_tmr == DEF_NULL) {
       *p_err = OS_ERR_TMR_INVALID;
        return (DEF_FALSE);
    }
#endif

#if (OS_CFG_OBJ_TYPE_CHK_EN == DEF_ENABLED)
    if (p_tmr->Type != OS_OBJ_TYPE_TMR) {                       /* Make sure timer was created                          */
       *p_err = OS_ERR_OBJ_TYPE;
        return (DEF_FALSE);
    }
#endif

    OS_TmrLock();

    switch (p_tmr->State) {
        case OS_TMR_STATE_RUNNING:                              /* Restart the timer                                    */
             if (p_tmr->Dly == 0u) {
                 p_tmr->Remain = p_tmr->Period;
             } else {
                 p_tmr->Remain = p_tmr->Dly;
             }
            *p_err         = OS_ERR_NONE;
             success       = DEF_TRUE;
             break;

        case OS_TMR_STATE_STOPPED:                              /* Start the timer                                      */
        case OS_TMR_STATE_COMPLETED:
             p_tmr->State  = OS_TMR_STATE_RUNNING;
             if (p_tmr->Dly == 0u) {
                 p_tmr->Remain = p_tmr->Period;
             } else {
                 p_tmr->Remain = p_tmr->Dly;
             }
             if (OSTmrListPtr ==  DEF_NULL) {                   /* Link into timer list                                 */
                 p_tmr->NextPtr   = DEF_NULL;                   /* This is the first timer in the list                  */
                 p_tmr->PrevPtr   = DEF_NULL;
                 OSTmrListPtr     = p_tmr;
#if (OS_CFG_DBG_EN == DEF_ENABLED)
                 OSTmrListEntries = 1u;
#endif
             } else {
                 p_next           = OSTmrListPtr;               /* Insert at the beginning of the list                  */
                 p_tmr->NextPtr   = OSTmrListPtr;
                 p_tmr->PrevPtr   = DEF_NULL;
                 p_next->PrevPtr  = p_tmr;
                 OSTmrListPtr     = p_tmr;
#if (OS_CFG_DBG_EN == DEF_ENABLED)
                 OSTmrListEntries++;
#endif
             }
            *p_err   = OS_ERR_NONE;
             success = DEF_TRUE;
             break;

        case OS_TMR_STATE_UNUSED:                               /* Timer not created                                    */
            *p_err   = OS_ERR_TMR_INACTIVE;
             success = DEF_FALSE;
             break;

        default:
            *p_err = OS_ERR_TMR_INVALID_STATE;
             success = DEF_FALSE;
             break;
    }

    OS_TmrUnlock();

    return (success);
}


/*
************************************************************************************************************************
*                                           FIND OUT WHAT STATE A TIMER IS IN
*
* Description: This function is called to determine what state the timer is in:
*
*                  OS_TMR_STATE_UNUSED     the timer has not been created
*                  OS_TMR_STATE_STOPPED    the timer has been created but has not been started or has been stopped
*                  OS_TMR_STATE_COMPLETED  the timer is in ONE-SHOT mode and has completed it's timeout
*                  OS_TMR_SATE_RUNNING     the timer is currently running
*
* Arguments  : p_tmr    Is a pointer to the desired timer
*
*              p_err    Is a pointer to an error code.  '*p_err' will contain one of the following:
*
*                           OS_ERR_NONE               The return value reflects the state of the timer
*                           OS_ERR_OBJ_TYPE           If 'p_tmr' is not pointing to a timer
*                           OS_ERR_OS_NOT_RUNNING     If uC/OS-III is not running yet
*                           OS_ERR_TMR_INVALID        If 'p_tmr' is a NULL pointer
*                           OS_ERR_TMR_INVALID_STATE  If the timer is not in a valid state
*                           OS_ERR_TMR_ISR            If the call was made from an ISR
*
* Returns    : The current state of the timer (see description).
*
* Note(s)    : none
************************************************************************************************************************
*/

OS_STATE  OSTmrStateGet (OS_TMR  *p_tmr,
                         OS_ERR  *p_err)
{
    OS_STATE  state;



#ifdef OS_SAFETY_CRITICAL
    if (p_err == DEF_NULL) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return (OS_TMR_STATE_UNUSED);
    }
#endif

#if (OS_CFG_CALLED_FROM_ISR_CHK_EN == DEF_ENABLED)
    if (OSIntNestingCtr > 0u) {                                 /* See if trying to call from an ISR                    */
       *p_err = OS_ERR_TMR_ISR;
        return (OS_TMR_STATE_UNUSED);
    }
#endif

#if (OS_CFG_INVALID_OS_CALLS_CHK_EN == DEF_ENABLED)             /* Is the kernel running?                               */
    if (OSRunning != OS_STATE_OS_RUNNING) {
       *p_err = OS_ERR_OS_NOT_RUNNING;
        return (OS_TMR_STATE_UNUSED);
    }
#endif

#if (OS_CFG_ARG_CHK_EN == DEF_ENABLED)
    if (p_tmr == DEF_NULL) {
       *p_err = OS_ERR_TMR_INVALID;
        return (OS_TMR_STATE_UNUSED);
    }
#endif

#if (OS_CFG_OBJ_TYPE_CHK_EN == DEF_ENABLED)
    if (p_tmr->Type != OS_OBJ_TYPE_TMR) {                       /* Make sure timer was created                          */
       *p_err = OS_ERR_OBJ_TYPE;
        return (OS_TMR_STATE_UNUSED);
    }
#endif

    OS_TmrLock();

    state = p_tmr->State;
    switch (state) {
        case OS_TMR_STATE_UNUSED:
        case OS_TMR_STATE_STOPPED:
        case OS_TMR_STATE_COMPLETED:
        case OS_TMR_STATE_RUNNING:
            *p_err = OS_ERR_NONE;
             break;

        default:
            *p_err = OS_ERR_TMR_INVALID_STATE;
             break;
    }

    OS_TmrUnlock();

    return (state);
}


/*
************************************************************************************************************************
*                                                    STOP A TIMER
*
* Description: This function is called by your application code to stop a timer.
*
* Arguments  : p_tmr          Is a pointer to the timer to stop.
*
*              opt            Allows you to specify an option to this functions which can be:
*
*                               OS_OPT_TMR_NONE            Do nothing special but stop the timer
*                               OS_OPT_TMR_CALLBACK        Execute the callback function, pass it the callback argument
*                                                          specified when the timer was created.
*                               OS_OPT_TMR_CALLBACK_ARG    Execute the callback function, pass it the callback argument
*                                                          specified in THIS function call
*
*              callback_arg   Is a pointer to a 'new' callback argument that can be passed to the callback function
*                               instead of the timer's callback argument.  In other words, use 'callback_arg' passed in
*                               THIS function INSTEAD of p_tmr->OSTmrCallbackArg
*
*              p_err          Is a pointer to an error code.  '*p_err' will contain one of the following:
*
*                               OS_ERR_NONE                The timer has stopped
*                               OS_ERR_OBJ_TYPE            If 'p_tmr' is not pointing to a timer
*                               OS_ERR_OPT_INVALID         If you specified an invalid option for 'opt'
*                               OS_ERR_OS_NOT_RUNNING      If uC/OS-III is not running yet
*                               OS_ERR_TMR_INACTIVE        If the timer was not created
*                               OS_ERR_TMR_INVALID         If 'p_tmr' is a NULL pointer
*                               OS_ERR_TMR_INVALID_STATE   The timer is in an invalid state
*                               OS_ERR_TMR_ISR             If the function was called from an ISR
*                               OS_ERR_TMR_NO_CALLBACK     If the timer does not have a callback function defined
*                               OS_ERR_TMR_STOPPED         If the timer was already stopped
*
* Returns    : DEF_TRUE       If we stopped the timer (if the timer is already stopped, we also return DEF_TRUE)
*              DEF_FALSE      If not
*
* Note(s)    : none
************************************************************************************************************************
*/

CPU_BOOLEAN  OSTmrStop (OS_TMR  *p_tmr,
                        OS_OPT   opt,
                        void    *p_callback_arg,
                        OS_ERR  *p_err)
{
    OS_TMR_CALLBACK_PTR  p_fnct;
    CPU_BOOLEAN          success;



#ifdef OS_SAFETY_CRITICAL
    if (p_err == DEF_NULL) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return (DEF_FALSE);
    }
#endif

#if (OS_CFG_CALLED_FROM_ISR_CHK_EN == DEF_ENABLED)
    if (OSIntNestingCtr > 0u) {                                 /* See if trying to call from an ISR                    */
       *p_err = OS_ERR_TMR_ISR;
        return (DEF_FALSE);
    }
#endif

#if (OS_CFG_INVALID_OS_CALLS_CHK_EN == DEF_ENABLED)             /* Is the kernel running?                               */
    if (OSRunning != OS_STATE_OS_RUNNING) {
       *p_err = OS_ERR_OS_NOT_RUNNING;
        return (DEF_FALSE);
    }
#endif

#if (OS_CFG_ARG_CHK_EN == DEF_ENABLED)
    if (p_tmr == DEF_NULL) {
       *p_err = OS_ERR_TMR_INVALID;
        return (DEF_FALSE);
    }
#endif

#if (OS_CFG_OBJ_TYPE_CHK_EN == DEF_ENABLED)
    if (p_tmr->Type != OS_OBJ_TYPE_TMR) {                       /* Make sure timer was created                          */
       *p_err = OS_ERR_OBJ_TYPE;
        return (DEF_FALSE);
    }
#endif

    OS_TmrLock();

    switch (p_tmr->State) {
        case OS_TMR_STATE_RUNNING:
            *p_err = OS_ERR_NONE;
             switch (opt) {
                 case OS_OPT_TMR_CALLBACK:
                      OS_TmrUnlink(p_tmr);                      /* Remove from timer list                               */
                      p_fnct = p_tmr->CallbackPtr;              /* Execute callback function ...                        */
                      if (p_fnct != DEF_NULL) {                 /* ... if available                                     */
                        (*p_fnct)(p_tmr, p_tmr->CallbackPtrArg);/* Use callback arg when timer was created     */
                      } else {
                         *p_err = OS_ERR_TMR_NO_CALLBACK;
                      }
                      break;

                 case OS_OPT_TMR_CALLBACK_ARG:
                      OS_TmrUnlink(p_tmr);                      /* Remove from timer list                               */
                      p_fnct = p_tmr->CallbackPtr;              /* Execute callback function if available ...           */
                      if (p_fnct != DEF_NULL) {
                        (*p_fnct)(p_tmr, p_callback_arg);       /* .. using the 'callback_arg' provided in call       */
                      } else {
                         *p_err = OS_ERR_TMR_NO_CALLBACK;
                      }
                      break;

                 case OS_OPT_TMR_NONE:
                      OS_TmrUnlink(p_tmr);                      /* Remove from timer list                               */
                      break;

                 default:
                     OS_TmrUnlock();
                    *p_err = OS_ERR_OPT_INVALID;
                     return (DEF_FALSE);
             }
             success = DEF_TRUE;
             break;

        case OS_TMR_STATE_COMPLETED:                            /* Timer has already completed the ONE-SHOT or          */
        case OS_TMR_STATE_STOPPED:                              /* ... timer has not started yet.                       */
            *p_err   = OS_ERR_TMR_STOPPED;
             success = DEF_TRUE;
             break;

        case OS_TMR_STATE_UNUSED:                               /* Timer was not created                                */
            *p_err   = OS_ERR_TMR_INACTIVE;
             success = DEF_FALSE;
             break;

        default:
            *p_err   = OS_ERR_TMR_INVALID_STATE;
             success = DEF_FALSE;
             break;
    }

    OS_TmrUnlock();

    return (success);
}


/*
************************************************************************************************************************
*                                                 CLEAR TIMER FIELDS
*
* Description: This function is called to clear all timer fields.
*
* Argument(s): p_tmr    Is a pointer to the timer to clear
*              -----
*
* Returns    : none
*
* Note(s)    : 1) This function is INTERNAL to uC/OS-III and your application MUST NOT call it.
************************************************************************************************************************
*/

void  OS_TmrClr (OS_TMR  *p_tmr)
{
    p_tmr->State          = OS_TMR_STATE_UNUSED;                /* Clear timer fields                                   */
#if (OS_OBJ_TYPE_REQ == DEF_ENABLED)
    p_tmr->Type           = OS_OBJ_TYPE_NONE;
#endif
#if (OS_CFG_DBG_EN == DEF_ENABLED)
    p_tmr->NamePtr        = (CPU_CHAR *)((void *)"?TMR");
#endif
    p_tmr->Dly            = 0u;
    p_tmr->Remain         = 0u;
    p_tmr->Period         = 0u;
    p_tmr->Opt            = 0u;
    p_tmr->CallbackPtr    = DEF_NULL;
    p_tmr->CallbackPtrArg = DEF_NULL;
    p_tmr->NextPtr        = DEF_NULL;
    p_tmr->PrevPtr        = DEF_NULL;
}


/*
************************************************************************************************************************
*                                         ADD/REMOVE TIMER TO/FROM DEBUG TABLE
*
* Description: These functions are called by uC/OS-III to add or remove a timer to/from a timer debug table.
*
* Arguments  : p_tmr     is a pointer to the timer to add/remove
*
* Returns    : none
*
* Note(s)    : These functions are INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/


#if (OS_CFG_DBG_EN == DEF_ENABLED)
void  OS_TmrDbgListAdd (OS_TMR  *p_tmr)
{
    p_tmr->DbgPrevPtr               = DEF_NULL;
    if (OSTmrDbgListPtr == DEF_NULL) {
        p_tmr->DbgNextPtr           = DEF_NULL;
    } else {
        p_tmr->DbgNextPtr           =  OSTmrDbgListPtr;
        OSTmrDbgListPtr->DbgPrevPtr =  p_tmr;
    }
    OSTmrDbgListPtr                 =  p_tmr;
}



void  OS_TmrDbgListRemove (OS_TMR  *p_tmr)
{
    OS_TMR  *p_tmr_next;
    OS_TMR  *p_tmr_prev;


    p_tmr_prev = p_tmr->DbgPrevPtr;
    p_tmr_next = p_tmr->DbgNextPtr;

    if (p_tmr_prev == DEF_NULL) {
        OSTmrDbgListPtr = p_tmr_next;
        if (p_tmr_next != DEF_NULL) {
            p_tmr_next->DbgPrevPtr = DEF_NULL;
        }
        p_tmr->DbgNextPtr = DEF_NULL;

    } else if (p_tmr_next == DEF_NULL) {
        p_tmr_prev->DbgNextPtr = DEF_NULL;
        p_tmr->DbgPrevPtr      = DEF_NULL;

    } else {
        p_tmr_prev->DbgNextPtr = p_tmr_next;
        p_tmr_next->DbgPrevPtr = p_tmr_prev;
        p_tmr->DbgNextPtr      = DEF_NULL;
        p_tmr->DbgPrevPtr      = DEF_NULL;
    }
}
#endif


/*
************************************************************************************************************************
*                                             INITIALIZE THE TIMER MANAGER
*
* Description: This function is called by OSInit() to initialize the timer manager module.
*
* Argument(s): p_err    is a pointer to a variable that will contain an error code returned by this function.
*
*                           OS_ERR_NONE
*                           OS_ERR_TMR_STK_INVALID       if you didn't specify a stack for the timer task
*                           OS_ERR_TMR_STK_SIZE_INVALID  if you didn't allocate enough space for the timer stack
*                           OS_ERR_PRIO_INVALID          if you specified the same priority as the idle task
*                           OS_ERR_xxx                   any error code returned by OSTaskCreate()
*
* Returns    : none
*
* Note(s)    : 1) This function is INTERNAL to uC/OS-III and your application MUST NOT call it.
************************************************************************************************************************
*/

void  OS_TmrInit (OS_ERR  *p_err)
{
#if (OS_CFG_DBG_EN == DEF_ENABLED)
    OSTmrDbgListPtr = DEF_NULL;
#endif

    OSTmrListPtr        = DEF_NULL;                             /* Create an empty timer list                           */
#if (OS_CFG_DBG_EN == DEF_ENABLED)
    OSTmrListEntries    = 0u;
#endif

    if (OSCfg_TmrTaskRate_Hz > 0u) {
        OSTmrUpdateCnt  = OSCfg_TickRate_Hz / OSCfg_TmrTaskRate_Hz;
    } else {
        OSTmrUpdateCnt  = OSCfg_TickRate_Hz / 10u;
    }
    OSTmrUpdateCtr      = OSTmrUpdateCnt;

    OSTmrTickCtr        = 0u;

#if (OS_CFG_TS_EN == DEF_ENABLED)
    OSTmrTaskTimeMax    = 0u;
#endif

#if (OS_CFG_MUTEX_EN == DEF_ENABLED)
    OSMutexCreate(&OSTmrMutex,                                  /* Use a mutex to protect the timers                    */
                  (CPU_CHAR *)"OS Tmr Mutex",
                  p_err);
    if (*p_err != OS_ERR_NONE) {
        return;
    }
#endif

                                                                /* -------------- CREATE THE TIMER TASK --------------- */
    if (OSCfg_TmrTaskStkBasePtr == DEF_NULL) {
       *p_err = OS_ERR_TMR_STK_INVALID;
        return;
    }

    if (OSCfg_TmrTaskStkSize < OSCfg_StkSizeMin) {
       *p_err = OS_ERR_TMR_STK_SIZE_INVALID;
        return;
    }

    if (OSCfg_TmrTaskPrio >= (OS_CFG_PRIO_MAX - 1u)) {
       *p_err = OS_ERR_TMR_PRIO_INVALID;
        return;
    }

    OSTaskCreate(&OSTmrTaskTCB,
                 (CPU_CHAR   *)"uC/OS-III Timer Task",
                 OS_TmrTask,
                 DEF_NULL,
                 OSCfg_TmrTaskPrio,
                 OSCfg_TmrTaskStkBasePtr,
                 OSCfg_TmrTaskStkLimit,
                 OSCfg_TmrTaskStkSize,
                 0u,
                 0u,
                 DEF_NULL,
                 (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_NO_TLS),
                 p_err);
}


/*
************************************************************************************************************************
*                                         REMOVE A TIMER FROM THE TIMER LIST
*
* Description: This function is called to remove the timer from the timer list.
*
* Arguments  : p_tmr          Is a pointer to the timer to remove.
*              -----
*
* Returns    : none
*
* Note(s)    : 1) This function is INTERNAL to uC/OS-III and your application MUST NOT call it.
************************************************************************************************************************
*/

void  OS_TmrUnlink (OS_TMR  *p_tmr)
{
    OS_TMR  *p_tmr1;
    OS_TMR  *p_tmr2;


    if (OSTmrListPtr == p_tmr) {                                /* See if timer to remove is at the beginning of list   */
        p_tmr1       = p_tmr->NextPtr;
        OSTmrListPtr = p_tmr1;
        if (p_tmr1 != DEF_NULL) {
            p_tmr1->PrevPtr = DEF_NULL;
        }
    } else {
        p_tmr1          = p_tmr->PrevPtr;                       /* Remove timer from somewhere in the list              */
        p_tmr2          = p_tmr->NextPtr;
        p_tmr1->NextPtr = p_tmr2;
        if (p_tmr2 != DEF_NULL) {
            p_tmr2->PrevPtr = p_tmr1;
        }
    }
    p_tmr->State   = OS_TMR_STATE_STOPPED;
    p_tmr->NextPtr = DEF_NULL;
    p_tmr->PrevPtr = DEF_NULL;
#if (OS_CFG_DBG_EN == DEF_ENABLED)
    OSTmrListEntries--;
#endif
}


/*
************************************************************************************************************************
*                                                 TIMER MANAGEMENT TASK
*
* Description: This task is created by OS_TmrInit().
*
* Arguments  : none
*
* Returns    : none
*
* Note(s)    : 1) This function is INTERNAL to uC/OS-III and your application MUST NOT call it.
************************************************************************************************************************
*/

void  OS_TmrTask (void  *p_arg)
{
    OS_ERR               err;
    OS_TMR_CALLBACK_PTR  p_fnct;
    OS_TMR              *p_tmr;
    OS_TMR              *p_tmr_next;
#if (OS_CFG_DYN_TICK_EN != DEF_ENABLED)
    CPU_TS               ts;
#endif
#if (OS_CFG_TS_EN == DEF_ENABLED)
    CPU_TS               ts_start;
    CPU_TS               ts_delta;
#endif



    (void)p_arg;                                                /* Not using 'p_arg', prevent compiler warning          */
    for (;;) {
#if (OS_CFG_DYN_TICK_EN != DEF_ENABLED)
        (void)OSTaskSemPend( 0u,                                /* Wait for signal indicating time to update tmrs       */
                             OS_OPT_PEND_BLOCKING,
                            &ts,
                            &err);
        (void)err;
#else
        OSTimeDly(OSTmrUpdateCnt, OS_OPT_TIME_DLY, &err);
        (void)err;
#endif


        OS_TmrLock();
#if (OS_CFG_TS_EN == DEF_ENABLED)
        ts_start = OS_TS_GET();
#endif
        OSTmrTickCtr++;                                         /* Increment the current time                           */
        p_tmr    = OSTmrListPtr;
        while (p_tmr != DEF_NULL) {                             /* Update all the timers in the list                    */
            OSSchedLock(&err);
            (void)err;
            p_tmr_next = p_tmr->NextPtr;
            p_tmr->Remain--;
            if (p_tmr->Remain == 0u) {
                if (p_tmr->Opt == OS_OPT_TMR_PERIODIC) {
                    p_tmr->Remain = p_tmr->Period;              /* Reload the time remaining                            */
                } else {
                    OS_TmrUnlink(p_tmr);                        /* Remove from list                                     */
                    p_tmr->State = OS_TMR_STATE_COMPLETED;      /* Indicate that the timer has completed                */
                }
                p_fnct = p_tmr->CallbackPtr;                    /* Execute callback function if available               */
                if (p_fnct != (OS_TMR_CALLBACK_PTR)0u) {
                    (*p_fnct)(p_tmr, p_tmr->CallbackPtrArg);
                }
            }
            p_tmr = p_tmr_next;
            OSSchedUnlock(&err);
            (void)err;
        }

#if (OS_CFG_TS_EN == DEF_ENABLED)
        ts_delta = OS_TS_GET() - ts_start;                      /* Measure execution time of timer task                 */
        if (OSTmrTaskTimeMax < ts_delta) {
            OSTmrTaskTimeMax = ts_delta;
        }
#endif

        OS_TmrUnlock();
    }
}


/*
************************************************************************************************************************
*                                          TIMER MANAGEMENT LOCKING MECHANISM
*
* Description: These functions are use to handle timer critical sections.  The preferred method is to use a mutex in
*              order to avoid locking the scheduler and also, to avoid calling callback functions while the scheduler is
*              locked.
*
* Arguments  : none
*
* Returns    : none
*
* Note(s)    : 1) These function is INTERNAL to uC/OS-III and your application MUST NOT call it.
************************************************************************************************************************
*/

static  void  OS_TmrLock (void)
{
    OS_ERR  err;
#if (OS_CFG_MUTEX_EN == DEF_ENABLED)
    CPU_TS  ts;


    OSMutexPend(&OSTmrMutex,                                    /* Use a mutex to protect the timers                    */
                0u,
                OS_OPT_PEND_BLOCKING,
                &ts,
                &err);
#else
    OSSchedLock(&err);                                          /* Lock the scheduler to protect the timers             */
#endif
    (void)err;
}




static  void  OS_TmrUnlock (void)
{
    OS_ERR  err;


#if (OS_CFG_MUTEX_EN == DEF_ENABLED)
    OSMutexPost(&OSTmrMutex,                                    /* Use a mutex to protect the timers                    */
                 OS_OPT_POST_NONE,
                &err);
#else
    OSSchedUnlock(&err);                                        /* Lock the scheduler to protect the timers             */
#endif
    (void)err;
}

#endif
