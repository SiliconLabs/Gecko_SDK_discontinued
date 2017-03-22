/*
************************************************************************************************************************
*                                                    uC/OS-III Trace
*                                                The RTOS Event Analyzer
*
*                                  (c) Copyright 2015-2016; Micrium, Inc.; Weston, FL
*                           All rights reserved.  Protected by international copyright laws.
*
*                                                uC/OS-III TRACE EVENTS
*                                                 Percepio TraceAlyzer
*                                                  (Snapshot Recorder)
*
*                                        (c) Copyright 2015-2016; Percepio AB
*                           All rights reserved.  Protected by international copyright laws.
*
* File    : OS_TRACE_EVENTS.H
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


/*
************************************************************************************************************************
*                                                    INCLUDE FILES
************************************************************************************************************************
*/

#include <trcKernelPort.h>


/*
************************************************************************************************************************
*                                               uC/OS-III Trace Macros
************************************************************************************************************************
*/

#if (defined(OS_CFG_TRACE_EN) && (OS_CFG_TRACE_EN > 0u))

#define  OS_TRACE_INIT()                                      vTraceInitTraceData()
#define  OS_TRACE_START()                                     vTraceStart()
#define  OS_TRACE_STOP()                                      vTraceStop()
#define  OS_TRACE_CLEAR()                                     vTraceClear()

#else

#define  OS_TRACE_INIT()
#define  OS_TRACE_START()
#define  OS_TRACE_STOP()
#define  OS_TRACE_CLEAR()

#endif


/*
************************************************************************************************************************
*                                              uC/Trace Kernel-Related Macros
************************************************************************************************************************
*/

#if (defined(OS_CFG_TRACE_EN) && (OS_CFG_TRACE_EN > 0u))

#define  OS_TRACE_TICK_INCREMENT(OSTickCtr)                 traceTASK_INCREMENT_TICK(OSTickCtr)

#define  OS_TRACE_TASK_CREATE(p_tcb)                        traceTASK_CREATE(p_tcb)
#define  OS_TRACE_TASK_CREATE_FAILED(p_tcb)                 traceTASK_CREATE_FAILED(p_tcb)
#define  OS_TRACE_TASK_DEL(p_tcb)                           traceTASK_DELETE(p_tcb)
#define  OS_TRACE_TASK_READY(p_tcb)                         traceMOVED_TASK_TO_READY_STATE(p_tcb)
#define  OS_TRACE_TASK_SWITCHED_IN(p_tcb)                   traceTASK_SWITCHED_IN(p_tcb)
#define  OS_TRACE_TASK_DLY(dly_ticks)                       traceTASK_DELAY(dly_ticks)
#define  OS_TRACE_TASK_SUSPEND(p_tcb)                       traceTASK_SUSPEND(p_tcb)
#define  OS_TRACE_TASK_SUSPENDED(p_tcb)
#define  OS_TRACE_TASK_RESUME(p_tcb)                        traceTASK_RESUME(p_tcb)
#define  OS_TRACE_TASK_PRIO_CHANGE(p_tcb, prio)             traceTASK_PRIORITY_SET(p_tcb, prio)

#define  OS_TRACE_ISR_REGISTER(isr_id, isr_name, isr_prio)  traceISR_REGISTER(isr_id, isr_name, isr_prio)
#define  OS_TRACE_ISR_BEGIN(isr_id)                         traceISR_BEGIN(isr_id)
#define  OS_TRACE_ISR_END()                                 traceISR_END()

#define  OS_TRACE_ISR_ENTER()
#define  OS_TRACE_ISR_EXIT()
#define  OS_TRACE_ISR_EXIT_TO_SCHEDULER()

#define  OS_TRACE_TASK_MSG_Q_CREATE(p_msg_q, p_name)        traceTASK_MSG_Q_CREATE(MICRIUM_TASK_MSG_Q, p_msg_q, p_name)
#define  OS_TRACE_TASK_MSG_Q_POST(p_msg_q)                  traceTASK_MSG_Q_SEND(MICRIUM_TASK_MSG_Q, p_msg_q)
#define  OS_TRACE_TASK_MSG_Q_POST_FAILED(p_msg_q)           traceTASK_MSG_Q_SEND_FAILED(MICRIUM_TASK_MSG_Q, p_msg_q)
#define  OS_TRACE_TASK_MSG_Q_PEND(p_msg_q)                  traceTASK_MSG_Q_RECEIVE(MICRIUM_TASK_MSG_Q, p_msg_q)
#define  OS_TRACE_TASK_MSG_Q_PEND_FAILED(p_msg_q)           traceTASK_MSG_Q_RECEIVE_FAILED(MICRIUM_TASK_MSG_Q, p_msg_q)
#define  OS_TRACE_TASK_MSG_Q_PEND_BLOCK(p_msg_q)            traceTASK_MSG_Q_RECEIVE_BLOCK(MICRIUM_TASK_MSG_Q, p_msg_q)

#define  OS_TRACE_TASK_SEM_CREATE(p_tcb, p_name)            traceTASK_SEM_CREATE(MICRIUM_TASK_SEM, p_tcb, p_name)
#define  OS_TRACE_TASK_SEM_POST(p_tcb)                      traceTASK_SEM_SEND(MICRIUM_TASK_SEM, p_tcb)
#define  OS_TRACE_TASK_SEM_POST_FAILED(p_tcb)               traceTASK_SEM_SEND_FAILED(MICRIUM_TASK_SEM, p_tcb)
#define  OS_TRACE_TASK_SEM_PEND(p_tcb)                      traceTASK_SEM_RECEIVE(MICRIUM_TASK_SEM, p_tcb)
#define  OS_TRACE_TASK_SEM_PEND_FAILED(p_tcb)               traceTASK_SEM_RECEIVE_FAILED(MICRIUM_TASK_SEM, p_tcb)
#define  OS_TRACE_TASK_SEM_PEND_BLOCK(p_tcb)                traceTASK_SEM_RECEIVE_BLOCK(MICRIUM_TASK_SEM, p_tcb)

#define  OS_TRACE_MUTEX_CREATE(p_mutex, p_name)             traceMUTEX_CREATE(MICRIUM_MUTEX, p_mutex, p_name)
#define  OS_TRACE_MUTEX_DEL(p_mutex)                        traceMUTEX_DELETE(MICRIUM_MUTEX, p_mutex)
#define  OS_TRACE_MUTEX_POST(p_mutex)                       traceMUTEX_SEND(MICRIUM_MUTEX, p_mutex)
#define  OS_TRACE_MUTEX_POST_FAILED(p_mutex)                traceMUTEX_SEND_FAILED(MICRIUM_MUTEX, p_mutex)
#define  OS_TRACE_MUTEX_PEND(p_mutex)                       traceMUTEX_RECEIVE(MICRIUM_MUTEX, p_mutex)
#define  OS_TRACE_MUTEX_PEND_FAILED(p_mutex)                traceMUTEX_RECEIVE_FAILED(MICRIUM_MUTEX, p_mutex)
#define  OS_TRACE_MUTEX_PEND_BLOCK(p_mutex)                 traceMUTEX_RECEIVE_BLOCK(MICRIUM_MUTEX, p_mutex)

#define  OS_TRACE_MUTEX_TASK_PRIO_INHERIT(p_tcb, prio)      traceTASK_PRIORITY_INHERIT(p_tcb, prio)
#define  OS_TRACE_MUTEX_TASK_PRIO_DISINHERIT(p_tcb, prio)   traceTASK_PRIORITY_DISINHERIT(p_tcb, prio)

#define  OS_TRACE_SEM_CREATE(p_sem, p_name)                 traceSEM_CREATE(MICRIUM_SEM, p_sem, p_name)
#define  OS_TRACE_SEM_DEL(p_sem)                            traceSEM_DELETE(MICRIUM_SEM, p_sem)
#define  OS_TRACE_SEM_POST(p_sem)                           traceSEM_SEND(MICRIUM_SEM, p_sem)
#define  OS_TRACE_SEM_POST_FAILED(p_sem)                    traceSEM_SEND_FAILED(MICRIUM_SEM, p_sem)
#define  OS_TRACE_SEM_PEND(p_sem)                           traceSEM_RECEIVE(MICRIUM_SEM, p_sem)
#define  OS_TRACE_SEM_PEND_FAILED(p_sem)                    traceSEM_RECEIVE_FAILED(MICRIUM_SEM, p_sem)
#define  OS_TRACE_SEM_PEND_BLOCK(p_sem)                     traceSEM_RECEIVE_BLOCK(MICRIUM_SEM, p_sem)

#define  OS_TRACE_Q_CREATE(p_q, p_name)                     traceQ_CREATE(MICRIUM_Q, p_q, p_name)
#define  OS_TRACE_Q_DEL(p_q)                                traceQ_DELETE(MICRIUM_Q, p_q)
#define  OS_TRACE_Q_POST(p_q)                               traceQ_SEND(MICRIUM_Q, p_q)
#define  OS_TRACE_Q_POST_FAILED(p_q)                        traceQ_SEND_FAILED(MICRIUM_Q, p_q)
#define  OS_TRACE_Q_PEND(p_q)                               traceQ_RECEIVE(MICRIUM_Q, p_q)
#define  OS_TRACE_Q_PEND_FAILED(p_q)                        traceQ_RECEIVE_FAILED(MICRIUM_Q, p_q)
#define  OS_TRACE_Q_PEND_BLOCK(p_q)                         traceQ_RECEIVE_BLOCK(MICRIUM_Q, p_q)

#define  OS_TRACE_FLAG_CREATE(p_grp, p_name)                traceFLAG_CREATE(MICRIUM_FLAG, p_grp, p_name)
#define  OS_TRACE_FLAG_DEL(p_grp)                           traceFLAG_DELETE(MICRIUM_FLAG, p_grp)
#define  OS_TRACE_FLAG_POST(p_grp)                          traceFLAG_SEND(MICRIUM_FLAG, p_grp)
#define  OS_TRACE_FLAG_POST_FAILED(p_grp)                   traceFLAG_SEND_FAILED(MICRIUM_FLAG, p_grp)
#define  OS_TRACE_FLAG_PEND(p_grp)                          traceFLAG_RECEIVE(MICRIUM_FLAG, p_grp)
#define  OS_TRACE_FLAG_PEND_FAILED(p_grp)                   traceFLAG_RECEIVE_FAILED(MICRIUM_FLAG, p_grp)
#define  OS_TRACE_FLAG_PEND_BLOCK(p_grp)                    traceFLAG_RECEIVE_BLOCK(MICRIUM_FLAG, p_grp)

#define  OS_TRACE_MEM_CREATE(p_mem, p_name)                 traceMEM_CREATE(MICRIUM_MEM, p_mem, p_name)
#define  OS_TRACE_MEM_PUT(p_mem)                            traceMEM_SEND(MICRIUM_MEM, p_mem)
#define  OS_TRACE_MEM_PUT_FAILED(p_mem)                     traceMEM_SEND_FAILED(MICRIUM_MEM, p_mem)
#define  OS_TRACE_MEM_GET(p_mem)                            traceMEM_RECEIVE(MICRIUM_MEM, p_mem)
#define  OS_TRACE_MEM_GET_FAILED(p_mem)                     traceMEM_RECEIVE_FAILED(MICRIUM_MEM, p_mem)

#else

#define  OS_TRACE_TICK_INCREMENT(OSTickCtr)

#define  OS_TRACE_TASK_CREATE(p_tcb)
#define  OS_TRACE_TASK_CREATE_FAILED(p_tcb)
#define  OS_TRACE_TASK_DEL(p_tcb)
#define  OS_TRACE_TASK_READY(p_tcb)
#define  OS_TRACE_TASK_SWITCHED_IN(p_tcb)
#define  OS_TRACE_TASK_DLY(dly_ticks)
#define  OS_TRACE_TASK_SUSPEND(p_tcb)
#define  OS_TRACE_TASK_SUSPENDED(p_tcb)
#define  OS_TRACE_TASK_RESUME(p_tcb)
#define  OS_TRACE_TASK_PRIO_CHANGE(p_tcb, prio)

#define  OS_TRACE_ISR_REGISTER(isr_id, isr_name, isr_prio)
#define  OS_TRACE_ISR_BEGIN(isr_id)
#define  OS_TRACE_ISR_END()

#define  OS_TRACE_ISR_ENTER()
#define  OS_TRACE_ISR_EXIT()
#define  OS_TRACE_ISR_EXIT_TO_SCHEDULER()

#define  OS_TRACE_TASK_MSG_Q_CREATE(p_msg_q, p_name)
#define  OS_TRACE_TASK_MSG_Q_POST(p_msg_q)
#define  OS_TRACE_TASK_MSG_Q_POST_FAILED(p_msg_q)
#define  OS_TRACE_TASK_MSG_Q_PEND(p_msg_q)
#define  OS_TRACE_TASK_MSG_Q_PEND_FAILED(p_msg_q)
#define  OS_TRACE_TASK_MSG_Q_PEND_BLOCK(p_msg_q)

#define  OS_TRACE_TASK_SEM_CREATE(p_tcb, p_name)
#define  OS_TRACE_TASK_SEM_POST(p_tcb)
#define  OS_TRACE_TASK_SEM_POST_FAILED(p_tcb)
#define  OS_TRACE_TASK_SEM_PEND(p_tcb)
#define  OS_TRACE_TASK_SEM_PEND_FAILED(p_tcb)
#define  OS_TRACE_TASK_SEM_PEND_BLOCK(p_tcb)

#define  OS_TRACE_MUTEX_CREATE(p_mutex, p_name)
#define  OS_TRACE_MUTEX_DEL(p_mutex)
#define  OS_TRACE_MUTEX_POST(p_mutex)
#define  OS_TRACE_MUTEX_POST_FAILED(p_mutex)
#define  OS_TRACE_MUTEX_PEND(p_mutex)
#define  OS_TRACE_MUTEX_PEND_FAILED(p_mutex)
#define  OS_TRACE_MUTEX_PEND_BLOCK(p_mutex)

#define  OS_TRACE_MUTEX_TASK_PRIO_INHERIT(p_tcb, prio)
#define  OS_TRACE_MUTEX_TASK_PRIO_DISINHERIT(p_tcb, prio)

#define  OS_TRACE_SEM_CREATE(p_sem, p_name)
#define  OS_TRACE_SEM_DEL(p_sem)
#define  OS_TRACE_SEM_POST(p_sem)
#define  OS_TRACE_SEM_POST_FAILED(p_sem)
#define  OS_TRACE_SEM_PEND(p_sem)
#define  OS_TRACE_SEM_PEND_FAILED(p_sem)
#define  OS_TRACE_SEM_PEND_BLOCK(p_sem)

#define  OS_TRACE_Q_CREATE(p_q, p_name)
#define  OS_TRACE_Q_DEL(p_q)
#define  OS_TRACE_Q_POST(p_q)
#define  OS_TRACE_Q_POST_FAILED(p_q)
#define  OS_TRACE_Q_PEND(p_q)
#define  OS_TRACE_Q_PEND_FAILED(p_q)
#define  OS_TRACE_Q_PEND_BLOCK(p_q)

#define  OS_TRACE_FLAG_CREATE(p_grp, p_name)
#define  OS_TRACE_FLAG_DEL(p_grp)
#define  OS_TRACE_FLAG_POST(p_grp)
#define  OS_TRACE_FLAG_POST_FAILED(p_grp)
#define  OS_TRACE_FLAG_PEND(p_grp)
#define  OS_TRACE_FLAG_PEND_FAILED(p_grp)
#define  OS_TRACE_FLAG_PEND_BLOCK(p_grp)

#define  OS_TRACE_MEM_CREATE(p_mem, p_name)
#define  OS_TRACE_MEM_PUT(p_mem)
#define  OS_TRACE_MEM_PUT_FAILED(p_mem)
#define  OS_TRACE_MEM_GET(p_mem)
#define  OS_TRACE_MEM_GET_FAILED(p_mem)

#endif


/*
************************************************************************************************************************
*                                   uC/OS-III Trace Unused Macros (for other recorders)
************************************************************************************************************************
*/

#define  OS_TRACE_MUTEX_DEL_ENTER(p_mutex, opt)
#define  OS_TRACE_MUTEX_POST_ENTER(p_mutex, opt)
#define  OS_TRACE_MUTEX_PEND_ENTER(p_mutex, timeout, opt, p_ts)
#define  OS_TRACE_TASK_MSG_Q_POST_ENTER(p_msg_q, p_void, msg_size, opt)
#define  OS_TRACE_TASK_MSG_Q_PEND_ENTER(p_msg_q, timeout, opt, p_msg_size, p_ts)
#define  OS_TRACE_TASK_SEM_POST_ENTER(p_tcb, opt)
#define  OS_TRACE_TASK_SEM_PEND_ENTER(p_tcb, timeout, opt, p_ts)
#define  OS_TRACE_SEM_DEL_ENTER(p_sem, opt)
#define  OS_TRACE_SEM_POST_ENTER(p_sem, opt)
#define  OS_TRACE_SEM_PEND_ENTER(p_sem, timeout, opt, p_ts)
#define  OS_TRACE_Q_DEL_ENTER(p_q, opt)
#define  OS_TRACE_Q_POST_ENTER(p_q, p_void, msg_size, opt)
#define  OS_TRACE_Q_PEND_ENTER(p_q, timeout, opt, p_msg_size, p_ts)
#define  OS_TRACE_FLAG_DEL_ENTER(p_grp, opt)
#define  OS_TRACE_FLAG_POST_ENTER(p_grp, flags, opt)
#define  OS_TRACE_FLAG_PEND_ENTER(p_grp, flags, timeout, opt, p_ts)
#define  OS_TRACE_MEM_PUT_ENTER(p_mem, p_blk)
#define  OS_TRACE_MEM_GET_ENTER(p_mem)

#define  OS_TRACE_MUTEX_DEL_EXIT(RetVal)
#define  OS_TRACE_MUTEX_POST_EXIT(RetVal)
#define  OS_TRACE_MUTEX_PEND_EXIT(RetVal)
#define  OS_TRACE_TASK_MSG_Q_POST_EXIT(RetVal)
#define  OS_TRACE_TASK_MSG_Q_PEND_EXIT(RetVal)
#define  OS_TRACE_TASK_SEM_POST_EXIT(RetVal)
#define  OS_TRACE_TASK_SEM_PEND_EXIT(RetVal)
#define  OS_TRACE_SEM_DEL_EXIT(RetVal)
#define  OS_TRACE_SEM_POST_EXIT(RetVal)
#define  OS_TRACE_SEM_PEND_EXIT(RetVal)
#define  OS_TRACE_Q_DEL_EXIT(RetVal)
#define  OS_TRACE_Q_POST_EXIT(RetVal)
#define  OS_TRACE_Q_PEND_EXIT(RetVal)
#define  OS_TRACE_FLAG_DEL_EXIT(RetVal)
#define  OS_TRACE_FLAG_POST_EXIT(RetVal)
#define  OS_TRACE_FLAG_PEND_EXIT(RetVal)
#define  OS_TRACE_MEM_PUT_EXIT(RetVal)
#define  OS_TRACE_MEM_GET_EXIT(RetVal)
