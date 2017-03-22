/*******************************************************************************
 * Trace Recorder Library for Tracealyzer v3.0.7
 * Percepio AB, www.percepio.com
 *
 * trcKernelPort.h
 *
 * The kernel-specific definitions for FreeRTOS.
 *
 * Terms of Use
 * This software (the "Tracealyzer Recorder Library") is the intellectual
 * property of Percepio AB and may not be sold or in other ways commercially
 * redistributed without explicit written permission by Percepio AB.
 *
 * Separate conditions applies for the SEGGER branded source code included.
 *
 * The recorder library is free for use together with Percepio products.
 * You may distribute the recorder library in its original form, but public
 * distribution of modified versions require approval by Percepio AB.
 *
 * Disclaimer
 * The trace tool and recorder library is being delivered to you AS IS and
 * Percepio AB makes no warranty as to its use or performance. Percepio AB does
 * not and cannot warrant the performance or results you may obtain by using the
 * software or documentation. Percepio AB make no warranties, express or
 * implied, as to noninfringement of third party rights, merchantability, or
 * fitness for any particular purpose. In no event will Percepio AB, its
 * technology partners, or distributors be liable to you for any consequential,
 * incidental or special damages, including any lost profits or lost savings,
 * even if a representative of Percepio AB has been advised of the possibility
 * of such damages, or for any claim by any third party. Some jurisdictions do
 * not allow the exclusion or limitation of incidental, consequential or special
 * damages, or the exclusion of implied warranties or limitations on how long an
 * implied warranty may last, so the above limitations may not apply to you.
 *
 * Tabs are used for indent in this file (1 tab = 4 spaces)
 *
 * Copyright Percepio AB, 2016.
 * www.percepio.com
 ******************************************************************************/


#ifndef TRCKERNELPORT_H
#define TRCKERNELPORT_H

#include <stdlib.h>

#include <os.h>
#include <os_cfg.h>                                             /* Defines OS_CFG_TRACE_EN (1: Enabled).               */
#include <os_cfg_app.h>	                                        /* Defines OS_CFG_TICK_RATE_HZ.                        */

extern int uiInEventGroupSetBitsFromISR;

#if OS_VERSION >= 30600u
#if (defined(OS_CFG_TRACE_EN) && (OS_CFG_TRACE_EN > 0u))
  #define USE_TRACEALYZER_RECORDER   1
#else
  #define USE_TRACEALYZER_RECORDER   0
#endif
#else /* OS_VERSION >= 30600u */
#error "This version of TraceAlyzer requires uC/OS-III V3.06.00 or newer."
#if (defined(TRACE_CFG_EN) && (TRACE_CFG_EN > 0u))
  #define USE_TRACEALYZER_RECORDER   1
#else
  #define USE_TRACEALYZER_RECORDER   0
#endif
#endif /* OS_VERSION >= 30600u */

#if (USE_TRACEALYZER_RECORDER == 1)

#include "trcHardwarePort.h"
#include "trcConfig.h"

/*******************************************************************************
 * Trace_Init
 *
 * The main initalization routine for the Trace recorder. Configures RTT and
 * activates the TzCtrl task.
 * Also sets up the diagnostic User Event channels used by TzCtrl task.
 ******************************************************************************/
void Trace_Init(void);

/*******************************************************************************
 * vTraceOnTraceBegin
 *
 * Called on trace begin.
 ******************************************************************************/
void vTraceOnTraceBegin(void);

/*******************************************************************************
 * vTraceOnTraceEnd
 *
 * Called on trace end.
 ******************************************************************************/
void vTraceOnTraceEnd(void);

#include "trcRecorder.h"
#include <bsp.h>

/* Defines that must be set for the recorder to work properly */
#define KERNEL_ID 0x2AA2
#define TRACE_TICK_RATE_HZ  OS_CFG_TICK_RATE_HZ            /* OS tick rate in Hertz.           */
#define TRACE_CPU_CLOCK_HZ  BSP_CPU_ClkFreq()              /* Peripheral clock speed in Hertz. */

/* Critical sections definition */
#define TRACE_ALLOC_CRITICAL_SECTION()    CPU_SR_ALLOC();
#define TRACE_ENTER_CRITICAL_SECTION()    CPU_CRITICAL_ENTER();
#define TRACE_EXIT_CRITICAL_SECTION()     CPU_CRITICAL_EXIT();

void* prvTraceGetCurrentTaskHandle(void);
uint32_t prvIsNewTCB(void* pNewTCB);

#define OS_IS_SWITCH_FROM_INT_REQUIRED() 0
#define TRACE_GET_CURRENT_TASK() prvTraceGetCurrentTaskHandle()

/*************************************************************************/
/* KERNEL SPECIFIC OBJECT CONFIGURATION									 */
/*************************************************************************/

/*******************************************************************************
 * The event codes - should match the offline config file.
 *
 * Some sections below are encoded to allow for constructions like:
 *
 * vTraceStoreKernelCall(EVENTGROUP_CREATE + objectclass, ...
 *
 * The object class ID is given by the three LSB bits, in such cases. Since each
 * object class has a separate object property table, the class ID is needed to
 * know what section in the object table to use for getting an object name from
 * an object handle.
 ******************************************************************************/

#define PSF_EVENT_NULL_EVENT								0x00

/* PSF event codes */
#define PSF_EVENT_TRACE_START								0x01
#define PSF_EVENT_TS_CONFIG									0x02
#define PSF_EVENT_OBJ_NAME									0x03
#define PSF_EVENT_TASK_PRIORITY								0x04
#define PSF_EVENT_TASK_PRIO_INHERIT							0x05
#define PSF_EVENT_TASK_PRIO_DISINHERIT						0x06
#define PSF_EVENT_DEFINE_ISR								0x07

#define PSF_EVENT_TASK_CREATE								0x10
#define PSF_EVENT_TASK_SEM_CREATE							0x11
#define PSF_EVENT_TASK_MSG_Q_CREATE							0x12
#define PSF_EVENT_QUEUE_CREATE								0x13
#define PSF_EVENT_SEMAPHORE_CREATE					        0x14
#define PSF_EVENT_MUTEX_CREATE								0x15
#define PSF_EVENT_FLAG_CREATE							    0x16
#define PSF_EVENT_MEM_CREATE							    0x17
#define PSF_EVENT_TMR_CREATE							    0x18

#define PSF_EVENT_TASK_DELETE								0x20
#define PSF_EVENT_QUEUE_DELETE								0x21
#define PSF_EVENT_SEMAPHORE_DELETE							0x22
#define PSF_EVENT_MUTEX_DELETE								0x23
#define PSF_EVENT_FLAG_DELETE					   		    0x24
#define PSF_EVENT_MEM_DELETE								0x25
#define PSF_EVENT_TMR_DELETE								0x26

#define PSF_EVENT_TASK_READY								0x30
#define PSF_EVENT_NEW_TIME									0x31
#define PSF_EVENT_NEW_TIME_SCHEDULER_SUSPENDED				0x32
#define PSF_EVENT_ISR_BEGIN									0x33
#define PSF_EVENT_ISR_RESUME								0x34
#define PSF_EVENT_TS_BEGIN									0x35
#define PSF_EVENT_TS_RESUME									0x36
#define PSF_EVENT_TASK_ACTIVATE								0x37

#define PSF_EVENT_MALLOC									0x38
#define PSF_EVENT_FREE										0x39

#define PSF_EVENT_LOWPOWER_BEGIN							0x3A
#define PSF_EVENT_LOWPOWER_END								0x3B

#define PSF_EVENT_IFE_NEXT									0x3C
#define PSF_EVENT_IFE_DIRECT								0x3D

#define PSF_EVENT_TASK_CREATE_FAILED						0x40
#define PSF_EVENT_QUEUE_CREATE_FAILED						0x41
#define PSF_EVENT_SEMAPHORE_CREATE_FAILED			        0x42
#define PSF_EVENT_MUTEX_CREATE_FAILED						0x43
#define PSF_EVENT_FLAG_CREATE_FAILED						0x44
#define PSF_EVENT_MEM_CREATE_FAILED					        0x45
#define PSF_EVENT_TMR_CREATE_FAILED			                0x46

#define PSF_EVENT_TASK_MSG_Q_SEND		    				0x50
#define PSF_EVENT_TASK_SEM_SEND		    					0x51
#define PSF_EVENT_SEM_SEND				        			0x52
#define PSF_EVENT_Q_SEND		    						0x53
#define PSF_EVENT_MUTEX_SEND								0x54
#define PSF_EVENT_FLAG_SEND						    		0x55
#define PSF_EVENT_MEM_SEND						    		0x56

#define PSF_EVENT_TASK_MSG_Q_SEND_FAILED		    		0x57
#define PSF_EVENT_TASK_SEM_SEND_FAILED		    			0x58
#define PSF_EVENT_SEM_SEND_FAILED				        	0x59
#define PSF_EVENT_Q_SEND_FAILED		    					0x5A
#define PSF_EVENT_MUTEX_SEND_FAILED					    	0x5B
#define PSF_EVENT_FLAG_SEND_FAILED					   		0x5C
#define PSF_EVENT_MEM_SEND_FAILED					   		0x5D

#define PSF_EVENT_TASK_MSG_Q_RECEIVE		   				0x60
#define PSF_EVENT_TASK_SEM_RECEIVE		    				0x61
#define PSF_EVENT_SEM_RECEIVE				       			0x62
#define PSF_EVENT_Q_RECEIVE		    						0x63
#define PSF_EVENT_MUTEX_RECEIVE								0x64
#define PSF_EVENT_FLAG_RECEIVE					    		0x65
#define PSF_EVENT_MEM_RECEIVE					    		0x66

#define PSF_EVENT_TASK_MSG_Q_RECEIVE_FAILED		   			0x70
#define PSF_EVENT_TASK_SEM_RECEIVE_FAILED		    		0x71
#define PSF_EVENT_SEM_RECEIVE_FAILED		      			0x72
#define PSF_EVENT_Q_RECEIVE_FAILED		    				0x73
#define PSF_EVENT_MUTEX_RECEIVE_FAILED						0x74
#define PSF_EVENT_FLAG_RECEIVE_FAILED			    		0x75
#define PSF_EVENT_MEM_RECEIVE_FAILED			    		0x76

#define PSF_EVENT_TASK_MSG_Q_RECEIVE_BLOCK		    		0x80
#define PSF_EVENT_TASK_SEM_RECEIVE_BLOCK		    		0x81
#define PSF_EVENT_SEM_RECEIVE_BLOCK				        	0x82
#define PSF_EVENT_Q_RECEIVE_BLOCK		    				0x83
#define PSF_EVENT_MUTEX_RECEIVE_BLOCK						0x84
#define PSF_EVENT_FLAG_RECEIVE_BLOCK					  	0x85
#define PSF_EVENT_MEM_RECEIVE_BLOCK				    		0x86

#define PSF_EVENT_QUEUE_SEND_FROMISR						0x90
#define PSF_EVENT_SEMAPHORE_GIVE_FROMISR					0x91

#define PSF_EVENT_QUEUE_SEND_FROMISR_FAILED					0x92
#define PSF_EVENT_SEMAPHORE_GIVE_FROMISR_FAILED				0x93

#define PSF_EVENT_QUEUE_RECEIVE_FROMISR						0x94
#define PSF_EVENT_SEMAPHORE_TAKE_FROMISR					0x95

#define PSF_EVENT_QUEUE_RECEIVE_FROMISR_FAILED				0x96
#define PSF_EVENT_SEMAPHORE_TAKE_FROMISR_FAILED				0x97

#define PSF_EVENT_QUEUE_PEEK								0xA0
#define PSF_EVENT_SEMAPHORE_PEEK							0xA1	/* Will never be used */
#define PSF_EVENT_MUTEX_PEEK								0xA2	/* Will never be used */

#define PSF_EVENT_QUEUE_PEEK_FAILED							0xA3
#define PSF_EVENT_SEMAPHORE_PEEK_FAILED						0xA4	/* Will never be used */
#define PSF_EVENT_MUTEX_PEEK_FAILED							0xA5	/* Will never be used */

#define PSF_EVENT_QUEUE_PEEK_BLOCK							0xA6
#define PSF_EVENT_SEMAPHORE_PEEK_BLOCK						0xA7	/* Will never be used */
#define PSF_EVENT_MUTEX_PEEK_BLOCK							0xA8	/* Will never be used */

#define PSF_EVENT_TASK_DELAY_UNTIL							0xA9
#define PSF_EVENT_TASK_DELAY								0xAA
#define PSF_EVENT_TASK_SUSPEND								0xAB
#define PSF_EVENT_TASK_RESUME								0xAC
#define PSF_EVENT_TASK_RESUME_FROMISR						0xAD

#define PSF_EVENT_TIMER_PENDFUNCCALL						0xB0
#define PSF_EVENT_TIMER_PENDFUNCCALL_FROMISR				0xB1
#define PSF_EVENT_TIMER_PENDFUNCCALL_FAILED					0xB2
#define PSF_EVENT_TIMER_PENDFUNCCALL_FROMISR_FAILED			0xB3

#define PSF_EVENT_USER_EVENT								0xB4

#define PSF_EVENT_TIMER_START								0xC0
#define PSF_EVENT_TIMER_RESET								0xC1
#define PSF_EVENT_TIMER_STOP								0xC2
#define PSF_EVENT_TIMER_CHANGEPERIOD						0xC3
#define PSF_EVENT_TIMER_START_FROMISR						0xC4
#define PSF_EVENT_TIMER_RESET_FROMISR						0xC5
#define PSF_EVENT_TIMER_STOP_FROMISR						0xC6
#define PSF_EVENT_TIMER_CHANGEPERIOD_FROMISR				0xC7
#define PSF_EVENT_TIMER_START_FAILED						0xC8
#define PSF_EVENT_TIMER_RESET_FAILED						0xC9
#define PSF_EVENT_TIMER_STOP_FAILED							0xCA
#define PSF_EVENT_TIMER_CHANGEPERIOD_FAILED					0xCB
#define PSF_EVENT_TIMER_START_FROMISR_FAILED				0xCC
#define PSF_EVENT_TIMER_RESET_FROMISR_FAILED				0xCD
#define PSF_EVENT_TIMER_STOP_FROMISR_FAILED					0xCE
#define PSF_EVENT_TIMER_CHANGEPERIOD_FROMISR_FAILED			0xCF

#define PSF_EVENT_EVENTGROUP_SYNC							0xD0
#define PSF_EVENT_EVENTGROUP_WAITBITS						0xD1
#define PSF_EVENT_EVENTGROUP_CLEARBITS						0xD2
#define PSF_EVENT_EVENTGROUP_CLEARBITS_FROMISR				0xD3
#define PSF_EVENT_EVENTGROUP_SETBITS						0xD4
#define PSF_EVENT_EVENTGROUP_SETBITS_FROMISR				0xD5
#define PSF_EVENT_EVENTGROUP_SYNC_BLOCK						0xD6
#define PSF_EVENT_EVENTGROUP_WAITBITS_BLOCK					0xD7
#define PSF_EVENT_EVENTGROUP_SYNC_FAILED					0xD8
#define PSF_EVENT_EVENTGROUP_WAITBITS_FAILED				0xD9

#define PSF_EVENT_QUEUE_SEND_FRONT							0xE0
#define PSF_EVENT_QUEUE_SEND_FRONT_FAILED					0xE1
#define PSF_EVENT_QUEUE_SEND_FRONT_BLOCK					0xE2
#define PSF_EVENT_QUEUE_SEND_FRONT_FROMISR					0xE3
#define PSF_EVENT_QUEUE_SEND_FRONT_FROMISR_FAILED			0xE4
#define PSF_EVENT_MUTEX_GIVE_RECURSIVE						0xE5
#define PSF_EVENT_MUTEX_GIVE_RECURSIVE_FAILED				0xE6
#define PSF_EVENT_MUTEX_TAKE_RECURSIVE						0xE7
#define PSF_EVENT_MUTEX_TAKE_RECURSIVE_FAILED				0xE8

#define ALLOCATE_EVENT(_type, _ptr, _size) _type tmpEvt; _type* _ptr = &tmpEvt;
#define COMMIT_EVENT(_ptr, _size) SEGGER_RTT_Write(TRC_RTT_UP_BUFFER_INDEX,(const char*)_ptr, _size);

#define TRACE_GET_OS_TICKS() (uiTraceTickCount)

/************************************************************************/
/* KERNEL SPECIFIC DATA AND FUNCTIONS NEEDED TO PROVIDE THE				*/
/* FUNCTIONALITY REQUESTED BY THE TRACE RECORDER						*/
/************************************************************************/

#if (configUSE_TIMERS == 1)
#undef INCLUDE_xTimerGetTimerDaemonTaskHandle
#define INCLUDE_xTimerGetTimerDaemonTaskHandle 1
#endif

/************************************************************************/
/* KERNEL SPECIFIC MACROS USED BY THE TRACE RECORDER					*/
/************************************************************************/

#define TRACE_MALLOC(size) pvPortMalloc(size)

/************************************************************************/
/* KERNEL SPECIFIC WRAPPERS THAT SHOULD BE CALLED BY THE KERNEL		 */
/************************************************************************/

#if (TRACE_CFG_USE_TICKLESS_IDLE != 0)

/* No need to protect critical section. */
#undef traceLOW_POWER_IDLE_BEGIN
#define traceLOW_POWER_IDLE_BEGIN() \
	{ \
		vTraceStoreEvent0(PSF_EVENT_LOWPOWER_BEGIN); \
	}

/* No need to protect critical section. */
#undef traceLOW_POWER_IDLE_END
#define traceLOW_POWER_IDLE_END() \
	{ \
		vTraceStoreEvent0(PSF_EVENT_LOWPOWER_END); \
	}

#endif

/* A macro that will update the tick count when returning from tickless idle */
#undef traceINCREASE_TICK_COUNT
/* Note: This can handle time adjustments of max 2^32 ticks, i.e., 35 seconds at 120 MHz. Thus, tick-less idle periods longer than 2^32 ticks will appear "compressed" on the time line.*/
#define traceINCREASE_TICK_COUNT( OSTickCtr ) 

/* Called for each task that becomes ready */
/* No need to protect critical section. */
#undef traceMOVED_TASK_TO_READY_STATE
#define traceMOVED_TASK_TO_READY_STATE( p_tcb ) \
	vTraceStoreEvent1(PSF_EVENT_TASK_READY, (uint32_t)p_tcb);

/* Called on each OS tick. Will call uiPortGetTimestamp to make sure it is called at least once every OS tick. */
/* No need to protect critical section. */
#undef traceTASK_INCREMENT_TICK
#define traceTASK_INCREMENT_TICK( OSTickCtr ) \
	{ extern uint32_t uiTraceTickCount; uiTraceTickCount = OSTickCtr + 1; vTraceStoreEvent1(PSF_EVENT_NEW_TIME, OSTickCtr + 1); }

/* Called on each task-switch */
/* No need to protect critical section. */
#undef traceTASK_SWITCHED_IN
#define traceTASK_SWITCHED_IN(p_tcb) \
	if (prvIsNewTCB(p_tcb)) \
	{ \
		vTraceStoreEvent2(PSF_EVENT_TASK_ACTIVATE, (uint32_t)p_tcb, p_tcb->Prio); \
	}

/* Called on vTaskSuspend */
/* No need to protect critical section. */
#undef traceTASK_SUSPEND
#define traceTASK_SUSPEND( p_tcb ) \
	vTraceStoreEvent1(PSF_EVENT_TASK_SUSPEND, (uint32_t)p_tcb);

/* Called on vTaskDelay - note the use of FreeRTOS variable xTicksToDelay */
/* No need to protect critical section. */
#undef traceTASK_DELAY
#define traceTASK_DELAY(dly_ticks) \
	vTraceStoreEvent1(PSF_EVENT_TASK_DELAY, dly_ticks);

/* Called on vTaskDelayUntil - note the use of FreeRTOS variable xTimeToWake */
/* No need to protect critical section. */
#undef traceTASK_DELAY_UNTIL
#define traceTASK_DELAY_UNTIL(dly_ticks_abs) \
	vTraceStoreEvent1(PSF_EVENT_TASK_DELAY_UNTIL, dly_ticks_abs);

/* Called on OSTaskDel */
/* No need to protect critical section. */
#undef traceTASK_DELETE
#define traceTASK_DELETE(p_tcb) \
			vTraceStoreEvent2(PSF_EVENT_TASK_DELETE, (uint32_t)p_tcb, (p_tcb != NULL) ? (p_tcb->Prio) : 0);

/* Called on OSQDel */
/* No need to protect critical section. */
#undef traceQ_DELETE
#define traceQ_DELETE( p_q ) \
			vTraceStoreEvent2(PSF_EVENT_QUEUE_DELETE, (uint32_t)p_q, 0); \

/* Called on OSMutexDel */
/* No need to protect critical section. */
#undef traceMUTEX_DELETE
#define traceMUTEX_DELETE( p_mutex ) \
			vTraceStoreEvent2(PSF_EVENT_MUTEX_DELETE, (uint32_t)p_mutex, 0);

/* Called on OSSemDel */
/* No need to protect critical section. */
#undef traceSEM_DELETE
#define traceSEM_DELETE( p_sem ) \
			vTraceStoreEvent2(PSF_EVENT_SEMAPHORE_DELETE, (uint32_t)p_sem, 0);

/* Called on OSFlagDel */
/* No need to protect critical section. */
#undef traceFLAG_DELETE
#define traceFLAG_DELETE( p_grp ) \
			vTraceStoreEvent2(PSF_EVENT_FLAG_DELETE, (uint32_t)p_grp, 0);

/* Called on OSMemDel */
/* No need to protect critical section. */
#undef traceMEM_DELETE
#define traceMEM_DELETE( p_mem ) \
			vTraceStoreEvent2(PSF_EVENT_MEM_DELETE, (uint32_t)p_mem, 0);


/* Called in OSTaskCreate */
/* Need to protect critical section without reallocating the SR.                     */
#undef traceTASK_CREATE
#define traceTASK_CREATE(p_tcb) \
	TRACE_ENTER_CRITICAL_SECTION(); \
    if (p_tcb != NULL) \
	{ \
		vTraceSaveSymbol(p_tcb, (const char*)p_tcb->NamePtr); \
		vTraceSaveObjectData(p_tcb, p_tcb->Prio); \
		vTraceStoreStringEvent(1, PSF_EVENT_OBJ_NAME, p_tcb->NamePtr, p_tcb); \
		vTraceStoreEvent2(PSF_EVENT_TASK_CREATE, (uint32_t)p_tcb, p_tcb->Prio); \
	} \
	TRACE_EXIT_CRITICAL_SECTION();

/* Called in OSTaskCreate, if it fails (typically if the stack can not be allocated) */
/* No need to protect critical section. */
#undef traceTASK_CREATE_FAILED
#define traceTASK_CREATE_FAILED(p_tcb) \
	vTraceStoreEvent0(PSF_EVENT_TASK_CREATE_FAILED);

/* Called in OSTaskCreate, while it creates the built-in message queue */
/* Need to protect critical section without reallocating the SR.       */
#undef traceTASK_MSG_Q_CREATE
#define traceTASK_MSG_Q_CREATE(p_msg_q, p_name) \
	TRACE_ENTER_CRITICAL_SECTION(); \
    if (p_msg_q != NULL) \
	{ \
        vTraceStoreEvent2(PSF_EVENT_TASK_MSG_Q_CREATE, (uint32_t)p_msg_q, (uint32_t)((OS_MSG_Q *)(p_msg_q)->NbrEntries)); \
        vTraceStoreKernelObjectName(p_msg_q, (const char*)p_name); \
	} \
	TRACE_EXIT_CRITICAL_SECTION();


/* Called in OSTaskCreate, while it creates the built-in semaphore */
/* Need to protect critical section without reallocating the SR.       */
#undef traceTASK_SEM_CREATE
#define traceTASK_SEM_CREATE(p_tcb, p_name) \
	TRACE_ENTER_CRITICAL_SECTION(); \
    if (p_tcb) \
	{ \
        vTraceStoreEvent2(PSF_EVENT_TASK_SEM_CREATE, (uint32_t)(&p_tcb->SemCtr), p_tcb->SemCtr); \
        vTraceStoreKernelObjectName((void *)(&p_tcb->SemCtr), (const char*)p_name); \
	} \
	TRACE_EXIT_CRITICAL_SECTION();

/* Called from os_q.c. */
/* No need to protect critical section. */
#undef traceQ_CREATE
#define traceQ_CREATE(p_q, p_name ) \
		vTraceStoreEvent2(PSF_EVENT_QUEUE_CREATE, (uint32_t)p_q, p_q->MsgQ.NbrEntriesSize); \
        vTraceStoreKernelObjectName(p_q, (const char*)p_name);

/* Called from OSSemCreate. */
/* No need to protect critical section. */
#undef traceSEM_CREATE
#define traceSEM_CREATE( p_sem, p_name) \
	vTraceStoreEvent2(PSF_EVENT_SEMAPHORE_CREATE, (uint32_t)p_sem, p_sem->Ctr); \
    vTraceStoreKernelObjectName(p_sem, (const char*)p_name);

/* Called from OSMutexCreate. */
/* No need to protect critical section. */
#undef traceMUTEX_CREATE
#define traceMUTEX_CREATE( p_mutex, p_name ) \
	vTraceStoreEvent1(PSF_EVENT_MUTEX_CREATE, (uint32_t)p_mutex); \
    vTraceStoreKernelObjectName(p_mutex, (const char*)p_name);

/* Called from OSMemCreate. */
/* No need to protect critical section. */
#undef traceMEM_CREATE
#define traceMEM_CREATE( p_mem, p_name ) \
	vTraceStoreEvent1(PSF_EVENT_MEM_CREATE, (uint32_t)p_mem); \
    vTraceStoreKernelObjectName(p_mem, (const char*)p_name);

/* Called from OSFlagCreate. */
/* No need to protect critical section. */
#undef traceFLAG_CREATE
#define traceFLAG_CREATE( p_grp, p_name ) \
	vTraceStoreEvent1(PSF_EVENT_FLAG_CREATE, (uint32_t)p_grp); \
    vTraceStoreKernelObjectName(p_grp, (const char*)p_name);

/* Called when a message is sent to a task's queue */
/* No need to protect critical section. */
#undef traceTASK_MSG_Q_SEND
#define traceTASK_MSG_Q_SEND(p_msg_q) \
	vTraceStoreEvent2(PSF_EVENT_TASK_MSG_Q_SEND, (uint32_t)p_msg_q, (uint32_t)((OS_MSG_Q *)(p_msg_q)->NbrEntries));

/* Called when a message is sent to a task's semaphore */
/* No need to protect critical section. */
#undef traceTASK_SEM_SEND
#define traceTASK_SEM_SEND(p_tcb) \
	vTraceStoreEvent2(PSF_EVENT_TASK_SEM_SEND, (uint32_t)(&p_tcb->SemCtr), (uint8_t)(p_tcb->SemCtr));

/* Called when a signal is sent to a semaphore */
/* No need to protect critical section. */
#undef traceSEM_SEND
#define traceSEM_SEND(p_sem) \
	vTraceStoreEvent2(PSF_EVENT_SEM_SEND, (uint32_t)p_sem, (uint8_t)(p_sem->Ctr));

/* Called when a signal is sent to a queue */
/* No need to protect critical section. */
#undef traceQ_SEND
#define traceQ_SEND(p_q) \
	vTraceStoreEvent2(PSF_EVENT_Q_SEND, (uint32_t)p_q, (uint8_t)(p_q->PendList.NbrEntries));

/* Called when a signal is sent to a mutex */
/* No need to protect critical section. */
#undef traceMUTEX_SEND
#define traceMUTEX_SEND(p_mutex) \
	vTraceStoreEvent1(PSF_EVENT_MUTEX_SEND, (uint32_t)p_mutex);

/* Called when a signal is sent to a flag group */
/* No need to protect critical section. */
#undef traceFLAG_SEND
#define traceFLAG_SEND(p_grp) \
	vTraceStoreEvent1(PSF_EVENT_FLAG_SEND, (uint32_t)p_grp);

/* Called when a signal is sent to a memory partition */
/* No need to protect critical section. */
#undef traceMEM_SEND
#define traceMEM_SEND(p_mem) \
	vTraceStoreEvent2(PSF_EVENT_MEM_SEND, (uint32_t)p_mem, (uint8_t)(p_mem->NbrFree));


/* Called when a message failed to be sent to a task's queue (timeout) */
/* No need to protect critical section. */
#undef traceTASK_MSG_Q_SEND_FAILED
#define traceTASK_MSG_Q_SEND_FAILED(p_msg_q) \
	vTraceStoreEvent2(PSF_EVENT_TASK_MSG_Q_SEND_FAILED, (uint32_t)p_msg_q, (uint32_t)((OS_MSG_Q *)(p_msg_q)->NbrEntries));

/* Called when a signal failed to be sent to a task's semaphore (timeout) */
/* No need to protect critical section. */
#undef traceTASK_SEM_SEND_FAILED
#define traceTASK_SEM_SEND_FAILED(p_tcb) \
	vTraceStoreEvent2(PSF_EVENT_TASK_SEM_SEND_FAILED, (uint32_t)(&p_tcb->SemCtr), (uint8_t)(p_tcb->SemCtr));

/* Called when a signal failed to be sent to a semaphore (timeout) */
/* No need to protect critical section. */
#undef traceSEM_SEND_FAILED
#define traceSEM_SEND_FAILED(p_tcb) \
	vTraceStoreEvent2(PSF_EVENT_SEM_SEND_FAILED, (uint32_t)p_sem, (uint8_t)(p_sem->Ctr));

/* Called when a message failed to be sent to a queue (timeout) */
/* No need to protect critical section. */
#undef traceQ_SEND_FAILED
#define traceQ_SEND_FAILED(p_q) \
	vTraceStoreEvent2(PSF_EVENT_Q_SEND_FAILED, (uint32_t)p_q, (uint8_t)(p_q->PendList.NbrEntries));

/* Called when a message failed to be sent to a mutex (timeout) */
/* No need to protect critical section. */
#undef traceMUTEX_SEND_FAILED
#define traceMUTEX_SEND_FAILED(p_mutex) \
	vTraceStoreEvent1(PSF_EVENT_MUTEX_SEND_FAILED, (uint32_t)p_mutex);

/* Called when a message failed to be sent to a flag group (timeout) */
/* No need to protect critical section. */
#undef traceFLAG_SEND_FAILED
#define traceFLAG_SEND_FAILED(p_grp) \
	vTraceStoreEvent1(PSF_EVENT_FLAG_SEND_FAILED, (uint32_t)p_grp);

/* Called when a message failed to be sent to a memory partition (timeout) */
/* No need to protect critical section. */
#undef traceMEM_SEND_FAILED
#define traceMEM_SEND_FAILED(p_mem) \
	vTraceStoreEvent1(PSF_EVENT_MEM_SEND_FAILED, (uint32_t)p_mem);


/* Called when a message is received from a task message queue */
/* No need to protect critical section. */
#undef traceTASK_MSG_Q_RECEIVE
#define traceTASK_MSG_Q_RECEIVE(p_msg_q) \
	vTraceStoreEvent2(PSF_EVENT_TASK_MSG_Q_RECEIVE, (uint32_t)p_msg_q, (uint8_t)(p_msg_q->NbrEntries));

/* Called when a message is received from a task message queue */
/* No need to protect critical section. */
#undef traceTASK_MSG_Q_RECEIVE_FAILED
#define traceTASK_MSG_Q_RECEIVE_FAILED(p_msg_q) \
	vTraceStoreEvent2(PSF_EVENT_TASK_MSG_Q_RECEIVE_FAILED, (uint32_t)p_msg_q, (uint8_t)(p_msg_q->NbrEntries));

/* Called when a message is received from a task message queue */
/* No need to protect critical section. */
#undef traceTASK_MSG_Q_RECEIVE_BLOCK
#define traceTASK_MSG_Q_RECEIVE_BLOCK(p_msg_q) \
	vTraceStoreEvent2(PSF_EVENT_TASK_MSG_Q_RECEIVE_BLOCK, (uint32_t)p_msg_q, (uint8_t)(p_msg_q->NbrEntries));


/* Called when a message is received from a task semaphore */
/* No need to protect critical section. */
#undef traceTASK_SEM_RECEIVE
#define traceTASK_SEM_RECEIVE(p_tcb) \
	vTraceStoreEvent2(PSF_EVENT_TASK_SEM_RECEIVE, (uint32_t)(&p_tcb->SemCtr), (uint8_t)(p_tcb->SemCtr));

/* Called when a message is received from a task semaphore */
/* No need to protect critical section. */
#undef traceTASK_SEM_RECEIVE_FAILED
#define traceTASK_SEM_RECEIVE_FAILED(p_tcb) \
	vTraceStoreEvent2(PSF_EVENT_TASK_SEM_RECEIVE_FAILED, (uint32_t)(&p_tcb->SemCtr), (uint8_t)(p_tcb->SemCtr));

/* Called when a message is received from a task semaphore */
/* No need to protect critical section. */
#undef traceTASK_SEM_RECEIVE_BLOCK
#define traceTASK_SEM_RECEIVE_BLOCK(p_tcb) \
	vTraceStoreEvent2(PSF_EVENT_TASK_SEM_RECEIVE_BLOCK, (uint32_t)(&p_tcb->SemCtr), (uint8_t)(p_tcb->SemCtr));


/* Called when a message is received from a semaphore */
/* No need to protect critical section. */
#undef traceSEM_RECEIVE
#define traceSEM_RECEIVE(p_sem) \
	vTraceStoreEvent2(PSF_EVENT_SEM_RECEIVE, (uint32_t)p_sem, (uint8_t)(p_sem->Ctr));

/* Called when a message is received from a semaphore */
/* No need to protect critical section. */
#undef traceSEM_RECEIVE_FAILED
#define traceSEM_RECEIVE_FAILED(p_sem) \
	vTraceStoreEvent2(PSF_EVENT_SEM_RECEIVE_FAILED, (uint32_t)p_sem, (uint8_t)(p_sem->Ctr));

/* Called when a message is received from a semaphore */
/* No need to protect critical section. */
#undef traceSEM_RECEIVE_BLOCK
#define traceSEM_RECEIVE_BLOCK(p_sem) \
	vTraceStoreEvent2(PSF_EVENT_SEM_RECEIVE_BLOCK, (uint32_t)p_sem, (uint8_t)(p_sem->Ctr));


/* Called when a message is received from a queue */
/* No need to protect critical section. */
#undef traceQ_RECEIVE
#define traceQ_RECEIVE(p_q) \
	vTraceStoreEvent2(PSF_EVENT_Q_RECEIVE, (uint32_t)p_q, (uint8_t)(p_q->PendList.NbrEntries));

/* Called when a message is received from a queue */
/* No need to protect critical section. */
#undef traceQ_RECEIVE_FAILED
#define traceQ_RECEIVE_FAILED(p_q) \
	vTraceStoreEvent2(PSF_EVENT_Q_RECEIVE_FAILED, (uint32_t)p_q, (uint8_t)(p_q->PendList.NbrEntries));

/* Called when a message is received from a queue */
/* No need to protect critical section. */
#undef traceQ_RECEIVE_BLOCK
#define traceQ_RECEIVE_BLOCK(p_q) \
	vTraceStoreEvent2(PSF_EVENT_Q_RECEIVE_BLOCK, (uint32_t)p_q, (uint8_t)(p_q->PendList.NbrEntries));


/* Called when a signal is received from a mutex */
/* No need to protect critical section. */
#undef traceMUTEX_RECEIVE
#define traceMUTEX_RECEIVE(p_mutex) \
	vTraceStoreEvent1(PSF_EVENT_MUTEX_RECEIVE, (uint32_t)p_mutex);

/* Called when a signal is received from a mutex */
/* No need to protect critical section. */
#undef traceMUTEX_RECEIVE_FAILED
#define traceMUTEX_RECEIVE_FAILED(p_mutex) \
	vTraceStoreEvent1(PSF_EVENT_MUTEX_RECEIVE_FAILED, (uint32_t)p_mutex);

/* Called when a signal is received from a mutex */
/* No need to protect critical section. */
#undef traceMUTEX_RECEIVE_BLOCK
#define traceMUTEX_RECEIVE_BLOCK(p_mutex) \
	vTraceStoreEvent1(PSF_EVENT_MUTEX_RECEIVE_BLOCK, (uint32_t)p_mutex);


/* Called when a signal is received from a flag group */
/* No need to protect critical section. */
#undef traceFLAG_RECEIVE
#define traceFLAG_RECEIVE(p_grp) \
	vTraceStoreEvent1(PSF_EVENT_FLAG_RECEIVE, (uint32_t)p_grp);

/* Called when a signal is received from a flag group */
/* No need to protect critical section. */
#undef traceFLAG_RECEIVE_FAILED
#define traceFLAG_RECEIVE_FAILED(p_grp) \
	vTraceStoreEvent1(PSF_EVENT_FLAG_RECEIVE_FAILED, (uint32_t)p_grp);

/* Called when a signal is received from a flag group */
/* No need to protect critical section. */
#undef traceFLAG_RECEIVE_BLOCK
#define traceFLAG_RECEIVE_BLOCK(p_grp) \
	vTraceStoreEvent1(PSF_EVENT_FLAG_RECEIVE_BLOCK, (uint32_t)p_grp);


/* Called when a message is received from a memory partition */
/* No need to protect critical section. */
#undef traceMEM_RECEIVE
#define traceMEM_RECEIVE(p_mem) \
	vTraceStoreEvent2(PSF_EVENT_MEM_RECEIVE, (uint32_t)p_mem, (uint8_t)(p_mem->NbrFree));

/* Called when a message is received from a memory partition */
/* No need to protect critical section. */
#undef traceMEM_RECEIVE_FAILED
#define traceMEM_RECEIVE_FAILED(p_mem) \
	vTraceStoreEvent2(PSF_EVENT_MEM_RECEIVE_FAILED, (uint32_t)p_mem, (uint8_t)(p_mem->NbrFree));

/* Called when a message is received from a memory partition */
/* No need to protect critical section. */
#undef traceMEM_RECEIVE_BLOCK
#define traceMEM_RECEIVE_BLOCK(p_mem) \
	vTraceStoreEvent2(PSF_EVENT_MEM_RECEIVE_BLOCK, (uint32_t)p_mem, (uint8_t)(p_mem->NbrFree));

/* Called in OSTaskChangePrio */
#undef traceTASK_PRIORITY_SET
#define traceTASK_PRIORITY_SET( p_tcb, prio ) \
	vTraceStoreEvent2(PSF_EVENT_TASK_PRIORITY, (uint32_t)p_tcb, prio);
	
/* Called in by mutex operations, when the task's priority is changed to the one from the mutex's owner */
/* No need to protect critical section. */
#undef traceTASK_PRIORITY_INHERIT
#define traceTASK_PRIORITY_INHERIT( p_tcb, prio ) \
	vTraceStoreEvent2(PSF_EVENT_TASK_PRIO_INHERIT, (uint32_t)p_tcb, prio);

/* Called in by mutex operations, when the task's priority is changed back to its original */
/* No need to protect critical section. */
#undef traceTASK_PRIORITY_DISINHERIT
#define traceTASK_PRIORITY_DISINHERIT( p_tcb, prio ) \
	vTraceStoreEvent2(PSF_EVENT_TASK_PRIO_DISINHERIT, (uint32_t)p_tcb, prio);

/* Called in OSTaskResume */
/* No need to protect critical section. */
#undef traceTASK_RESUME
#define traceTASK_RESUME( p_tcb ) \
	vTraceStoreEvent1(PSF_EVENT_TASK_RESUME, (uint32_t)p_tcb);

/* Not used. */
#undef traceMALLOC
#define traceMALLOC( pvAddress, uiSize ) \
	vTraceStoreEvent2(PSF_EVENT_MALLOC, (uint32_t)pvAddress, (int32_t)uiSize);

/* Not used. */
#undef traceFREE
#define traceFREE( pvAddress, uiSize ) \
	vTraceStoreEvent2(PSF_EVENT_FREE, (uint32_t)pvAddress, (int32_t)(-uiSize));

#undef traceISR_REGISTER
#define traceISR_REGISTER(isr_id, isr_prio) \
		vTraceSetISRProperties(isr_id, isr_prio);

#undef traceISR_BEGIN
#define traceISR_BEGIN(isr_id) \
		vTraceStoreISRBegin(isr_id);

#undef traceISR_END
#define traceISR_END() \
		vTraceStoreISREnd();

/************************************************************************/
/* KERNEL SPECIFIC MACROS TO NAME OBJECTS, IF NECESSARY				 */
/************************************************************************/
#define vTraceSetQueueName(object, name) \
vTraceStoreKernelObjectName(object, name);

#define vTraceSetSemaphoreName(object, name) \
vTraceStoreKernelObjectName(object, name);

#define vTraceSetMutexName(object, name) \
vTraceStoreKernelObjectName(object, name);

#define vTraceSetEventGroupName(object, name) \
vTraceStoreKernelObjectName(object, name);

#else /*(USE_TRACEALYZER_RECORDER == 1)*/

#define vTraceSetQueueName(object, name)

#define vTraceSetSemaphoreName(object, name)

#define vTraceSetMutexName(object, name)

#define vTraceSetEventGroupName(object, name)

#define vTraceInitTraceData() 
#define uiTraceStart() 

#endif /*(USE_TRACEALYZER_RECORDER == 1)*/

#endif /* TRCKERNELPORT_H */
