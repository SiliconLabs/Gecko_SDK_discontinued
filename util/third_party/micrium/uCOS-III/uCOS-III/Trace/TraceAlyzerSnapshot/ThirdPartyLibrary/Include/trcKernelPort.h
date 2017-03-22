/*******************************************************************************
 * Tracealyzer v3.0.7 Recorder Library
 * Percepio AB, www.percepio.com
 *
 * trcKernelPort.h
 *
 * Kernel-specific functionality for Micrium µC/OS-III, used by the recorder.
 * 
 * Terms of Use
 * This software is copyright Percepio AB. The recorder library is free for
 * use together with Percepio products. You may distribute the recorder library
 * in its original form, including modifications in trcHardwarePort.c/.h
 * given that these modification are clearly marked as your own modifications
 * and documented in the initial comment section of these source files. 
 * This software is the intellectual property of Percepio AB and may not be 
 * sold or in other ways commercially redistributed without explicit written 
 * permission by Percepio AB.
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

#include <os.h>
#include <os_cfg.h>	                                        /* Defines OS_CFG_TRACE_EN (1: Enabled).               */
#include <os_cfg_app.h>	                                    /* Defines OS_CFG_TICK_RATE_HZ.                        */
#include <bsp.h>

#include "trcHardwarePort.h"

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

/* Defines that must be set for the recorder to work properly */
#define TRACE_KERNEL_VERSION 0x2AA2
#define TRACE_CPU_CLOCK_HZ           BSP_CPU_ClkFreq()        /* Peripheral clock speed in Hertz. */
#define TRACE_PERIPHERAL_CLOCK_HZ    BSP_CPU_ClkFreq()        /* Peripheral clock speed in Hertz. */
#define TRACE_TICK_RATE_HZ           OS_CFG_TICK_RATE_HZ      /* OS tick rate in Hertz.           */
#define TRACE_CPU_CLOCKS_PER_TICK    1

#define TRACE_SR_ALLOC_CRITICAL_SECTION() CPU_SR_ALLOC();
#define TRACE_ENTER_CRITICAL_SECTION() CPU_CRITICAL_ENTER();
#define TRACE_EXIT_CRITICAL_SECTION() CPU_CRITICAL_EXIT();

#ifndef TRACE_ENTER_CRITICAL_SECTION
	#error "This port has no valid definition for critical sections! See http://percepio.com/2014/10/27/how-to-define-critical-sections-for-the-recorder/"
#endif

/************************************************************************/
/* KERNEL SPECIFIC OBJECT CONFIGURATION                                 */
/************************************************************************/
#define TRACE_NCLASSES        9
#define TRACE_CLASS_QUEUE     ((traceObjectClass)0)
#define TRACE_CLASS_SEMAPHORE ((traceObjectClass)1)
#define TRACE_CLASS_MUTEX     ((traceObjectClass)2)
#define TRACE_CLASS_FLAG      ((traceObjectClass)3)
#define TRACE_CLASS_MEM       ((traceObjectClass)4)
#define TRACE_CLASS_TASK_SEM  ((traceObjectClass)5)
#define TRACE_CLASS_TASK_Q    ((traceObjectClass)6)
#define TRACE_CLASS_TASK      ((traceObjectClass)7)
#define TRACE_CLASS_ISR       ((traceObjectClass)8)

#define TRACE_KERNEL_OBJECT_COUNT (NQueue + NSemaphore + NMutex + NFlag + NMem + NTaskSem + NTaskQ + NTask + NISR)

/* The size of the Object Property Table entries, in bytes, per object */

/* Queue properties (except name):     current number of message in queue */
#define PropertyTableSizeQueue         (NameLenQueue + 1)      

/* Semaphore properties (except name): state (signaled = 1, cleared = 0) */
#define PropertyTableSizeSemaphore     (NameLenSemaphore + 1) 

/* Mutex properties (except name):     owner (task handle, 0 = free) */
#define PropertyTableSizeMutex         (NameLenMutex + 1)         

/* Flags properties (except name): state (signaled = 1, cleared = 0) */
#define PropertyTableSizeFlag          (NameLenFlag + 1)

/* Memory Partitions properties (except name): state (signaled = 1, cleared = 0) */
#define PropertyTableSizeMem           (NameLenMem + 1)

/* Task Semaphore properties (except name): state (signaled = 1, cleared = 0) */
#define PropertyTableSizeTaskSem       (NameLenTaskSem + 1)

/* Task Q properties (except name): state (signaled = 1, cleared = 0) */
#define PropertyTableSizeTaskQ         (NameLenTaskQ + 1)
   
/* Task properties (except name):      Byte 0: Current priority
                                       Byte 1: state (if already active) 
                                       Byte 2: InstanceFinishEvent_ServiceCode
                                       Byte 3: InstanceFinishEvent_ObjHandle */
#define PropertyTableSizeTask         (NameLenTask + 4)

/* ISR properties:                     Byte 0: priority
                                       Byte 1: state (if already active) */
#define PropertyTableSizeISR          (NameLenISR + 2)



/* The layout of the byte array representing the Object Property Table */
#define StartIndexQueue            0
#define StartIndexSemaphore        StartIndexQueue     + NQueue     * PropertyTableSizeQueue
#define StartIndexMutex            StartIndexSemaphore + NSemaphore * PropertyTableSizeSemaphore
#define StartIndexFlag             StartIndexMutex     + NMutex     * PropertyTableSizeMutex
#define StartIndexMem              StartIndexFlag      + NFlag      * PropertyTableSizeFlag
#define StartIndexTaskSem          StartIndexMem       + NMem       * PropertyTableSizeMem
#define StartIndexTaskQ            StartIndexTaskSem   + NTaskSem   * PropertyTableSizeTaskSem
#define StartIndexTask             StartIndexTaskQ     + NTaskQ     * PropertyTableSizeTaskQ
#define StartIndexISR              StartIndexTask      + NTask      * PropertyTableSizeTask


/* Number of bytes used by the object table */
#define TRACE_OBJECT_TABLE_SIZE    StartIndexISR       + NISR       * PropertyTableSizeISR

/* Includes */
#include "trcTypes.h"
#include "trcConfig.h"
#include "trcKernelHooks.h"
#include "trcBase.h"
#include "trcKernel.h"
#include "trcUser.h"

/* Initialization of the object property table */
void vTraceInitObjectPropertyTable(void);

/* Initialization of the handle mechanism, see e.g, xTraceGetObjectHandle */
void vTraceInitObjectHandleStack(void);

/* Returns the "Not enough handles" error message for the specified object class */
const char* pszTraceGetErrorNotEnoughHandles(traceObjectClass objectclass);

/*******************************************************************************
 * The event codes - should match the offline config file.
 * 
 * Some sections below are encoded to allow for constructions like:
 *
 *  vTraceStoreKernelCall(EVENTGROUP_CREATE + objectclass, ...
 *
 * The object class ID is given by the three LSB bits, in such cases. Since each 
 * object class has a separate object property table, the class ID is needed to 
 * know what section in the object table to use for getting an object name from
 * an object handle. 
 ******************************************************************************/

#define NULL_EVENT                   (0x00)  /* Ignored in the analysis*/

/*******************************************************************************
 * EVENTGROUP_DIV
 *
 * Miscellaneous events.
 ******************************************************************************/
#define EVENTGROUP_DIV               (NULL_EVENT + 1)                   /*0x01*/
#define DIV_XPS                      (EVENTGROUP_DIV + 0)               /*0x01*/
#define DIV_TASK_READY               (EVENTGROUP_DIV + 1)               /*0x02*/
#define DIV_NEW_TIME                 (EVENTGROUP_DIV + 2)               /*0x03*/

/*******************************************************************************
 * EVENTGROUP_TS
 *
 * Events for storing task-switches and interrupts. The RESUME events are 
 * generated if the task/interrupt is already marked active.
 ******************************************************************************/
#define EVENTGROUP_TS                (EVENTGROUP_DIV + 3)               /*0x04*/
#define TS_ISR_BEGIN                 (EVENTGROUP_TS + 0)                /*0x04*/
#define TS_ISR_RESUME                (EVENTGROUP_TS + 1)                /*0x05*/
#define TS_TASK_BEGIN                (EVENTGROUP_TS + 2)                /*0x06*/
#define TS_TASK_RESUME               (EVENTGROUP_TS + 3)                /*0x07*/

/*******************************************************************************
 * EVENTGROUP_OBJCLOSE_NAME
 * 
 * About Close Events
 * When an object is evicted from the object property table (object close), two 
 * internal events are stored (EVENTGROUP_OBJCLOSE_NAME and 
 * EVENTGROUP_OBJCLOSE_PROP), containing the handle-name mapping and object 
 * properties valid up to this point.
 ******************************************************************************/
#define EVENTGROUP_OBJCLOSE_NAME     (EVENTGROUP_TS + 4)                /*0x08*/

/*******************************************************************************
 * EVENTGROUP_OBJCLOSE_PROP
 * 
 * The internal event carrying properties of deleted objects
 * The handle and object class of the closed object is not stored in this event, 
 * but is assumed to be the same as in the preceding CLOSE event. Thus, these 
 * two events must be generated from within a critical section. 
 * When queues are closed, arg1 is the "state" property (i.e., number of 
 * buffered messages/signals).
 * When actors are closed, arg1 is priority, arg2 is handle of the "instance 
 * finish" event, and arg3 is event code of the "instance finish" event. 
 * In this case, the lower three bits is the object class of the instance finish 
 * handle. The lower three bits are not used (always zero) when queues are 
 * closed since the queue type is given in the previous OBJCLOSE_NAME event.
 ******************************************************************************/
#define EVENTGROUP_OBJCLOSE_PROP              (EVENTGROUP_OBJCLOSE_NAME + 10)              /*0x12*/

/*******************************************************************************
 * EVENTGROUP_CREATE
 * 
 * The events in this group are used to log Kernel object creations.
 * The lower three bits in the event code gives the object class, i.e., type of
 * create operation (task, queue, semaphore, etc).
 ******************************************************************************/
#define EVENTGROUP_CREATE_SUCCESS             (EVENTGROUP_OBJCLOSE_PROP + 10)              /*0x1C*/

/*******************************************************************************
 * EVENTGROUP_SEND
 * 
 * The events in this group are used to log Send/Give events on queues, 
 * semaphores and mutexes The lower three bits in the event code gives the 
 * object class, i.e., what type of object that is operated on (queue, semaphore 
 * or mutex).
 ******************************************************************************/
#define EVENTGROUP_SEND_SUCCESS               (EVENTGROUP_CREATE_SUCCESS + 10)             /*0x26*/

/*******************************************************************************
 * EVENTGROUP_RECEIVE
 * 
 * The events in this group are used to log Receive/Take events on queues, 
 * semaphores and mutexes. The lower three bits in the event code gives the 
 * object class, i.e., what type of object that is operated on (queue, semaphore
 * or mutex).
 ******************************************************************************/
#define EVENTGROUP_RECEIVE_SUCCESS            (EVENTGROUP_SEND_SUCCESS + 10)               /*0x30*/

/* Send/Give operations, to a task */
#define EVENTGROUP_SEND_TO_TASK_SUCCESS       (EVENTGROUP_RECEIVE_SUCCESS + 10)            /*0x3A*/

/* Receive/Take operations, from a task */
#define EVENTGROUP_RECEIVE_FROM_TASK_SUCCESS  (EVENTGROUP_SEND_TO_TASK_SUCCESS + 10)       /*0x44*/

/* "Failed" event type versions of above (timeout, failed allocation, etc) */
#define EVENTGROUP_KSE_FAILED                 (EVENTGROUP_RECEIVE_FROM_TASK_SUCCESS + 10)  /*0x4E*/

/* Failed create calls - memory allocation failed */
#define EVENTGROUP_CREATE_FAILED              (EVENTGROUP_KSE_FAILED)                      /*0x4E*/

/* Failed send/give - timeout! */
#define EVENTGROUP_SEND_FAILED                (EVENTGROUP_CREATE_FAILED + 10)              /*0x58*/

/* Failed receive/take - timeout! */
#define EVENTGROUP_RECEIVE_FAILED             (EVENTGROUP_SEND_FAILED + 10)                /*0x62*/

/* Failed non-blocking send/give - queue full */
#define EVENTGROUP_SEND_TO_TASK_FAILED        (EVENTGROUP_RECEIVE_FAILED + 10)             /*0x6C*/

/* Failed non-blocking receive/take - queue empty */
#define EVENTGROUP_RECEIVE_FROM_TASK_FAILED   (EVENTGROUP_SEND_TO_TASK_FAILED + 10)        /*0x76*/

/* Events when blocking on receive/take */
#define EVENTGROUP_RECEIVE_BLOCK              (EVENTGROUP_RECEIVE_FROM_TASK_FAILED + 10)   /*0x80*/

/* Events when blocking on send/give */
#define EVENTGROUP_SEND_BLOCK                 (EVENTGROUP_RECEIVE_BLOCK + 10)              /*0x8A*/

/* Events on queue peek (receive) */
#define EVENTGROUP_PEEK_SUCCESS               (EVENTGROUP_SEND_BLOCK + 10)                 /*0x94*/

/* Events on object delete (vTaskDelete or vQueueDelete) */
#define EVENTGROUP_DELETE_SUCCESS             (EVENTGROUP_PEEK_SUCCESS + 10)               /*0x9E*/

/* Other events - object class is implied: TASK */
#define EVENTGROUP_OTHERS                     (EVENTGROUP_DELETE_SUCCESS + 10)             /*0xA8*/
#define TASK_DELAY_UNTIL                      (EVENTGROUP_OTHERS + 0)                      /*0xA8*/
#define TASK_DELAY                            (EVENTGROUP_OTHERS + 1)                      /*0xA9*/
#define TASK_SUSPEND                          (EVENTGROUP_OTHERS + 2)                      /*0xAA*/
#define TASK_RESUME                           (EVENTGROUP_OTHERS + 3)                      /*0xAB*/
#define TASK_RESUME_FROM_ISR                  (EVENTGROUP_OTHERS + 4)                      /*0xAC*/
#define TASK_PRIORITY_SET                     (EVENTGROUP_OTHERS + 5)                      /*0xAD*/
#define TASK_PRIORITY_INHERIT                 (EVENTGROUP_OTHERS + 6)                      /*0xAE*/
#define TASK_PRIORITY_DISINHERIT              (EVENTGROUP_OTHERS + 7)                      /*0xAF*/

/* Not yet used */
#define EVENTGROUP_FTRACE_PLACEHOLDER         (EVENTGROUP_OTHERS + 8)                      /*0xB0*/

/* User events */
#define EVENTGROUP_USEREVENT                  (EVENTGROUP_FTRACE_PLACEHOLDER + 10)         /*0xBA*/
#define USER_EVENT                            (EVENTGROUP_USEREVENT + 0)

/* Allow for 0-15 arguments (the number of args is added to event code) */
#define USER_EVENT_LAST                       (EVENTGROUP_USEREVENT + 15)                  /*0xC9*/

/*******************************************************************************
 * XTS Event - eXtended TimeStamp events
 * The timestamps used in the recorder are "differential timestamps" (DTS), i.e.
 * the time since the last stored event. The DTS fields are either 1 or 2 bytes 
 * in the other events, depending on the bytes available in the event struct. 
 * If the time since the last event (the DTS) is larger than allowed for by 
 * the DTS field of the current event, an XTS event is inserted immediately 
 * before the original event. The XTS event contains up to 3 additional bytes 
 * of the DTS value - the higher bytes of the true DTS value. The lower 1-2 
 * bytes are stored in the normal DTS field. 
 * There are two types of XTS events, XTS8 and XTS16. An XTS8 event is stored 
 * when there is only room for 1 byte (8 bit) DTS data in the original event, 
 * which means a limit of 0xFF (255). The XTS16 is used when the original event 
 * has a 16 bit DTS field and thereby can handle values up to 0xFFFF (65535).
 * 
 * Using a very high frequency time base can result in many XTS events. 
 * Preferably, the time between two OS ticks should fit in 16 bits, i.e.,
 * at most 65535. If your time base has a higher frequency, you can define
 * the TRACE
 ******************************************************************************/

#define EVENTGROUP_SYS                        (EVENTGROUP_USEREVENT + 16)                  /*0xCA*/
#define XTS8                                  (EVENTGROUP_SYS + 0)                         /*0xCA*/
#define XTS16                                 (EVENTGROUP_SYS + 1)                         /*0xCB*/
#define EVENT_BEING_WRITTEN                   (EVENTGROUP_SYS + 2)                         /*0xCC*/
#define RESERVED_DUMMY_CODE                   (EVENTGROUP_SYS + 3)                         /*0xCD*/
#define LOW_POWER_BEGIN                       (EVENTGROUP_SYS + 4)                         /*0xCE*/
#define LOW_POWER_END                         (EVENTGROUP_SYS + 5)                         /*0xCF*/
#define XID                                   (EVENTGROUP_SYS + 6)                         /*0xD0*/
#define XTS16L                                (EVENTGROUP_SYS + 7)                         /*0xD1*/
#define TASK_INSTANCE_FINISHED_NEXT_KSE       (EVENTGROUP_SYS + 8)                         /*0xD2*/
#define TASK_INSTANCE_FINISHED_DIRECT         (EVENTGROUP_SYS + 9)                         /*0xD3*/

/************************************************************************/
/* KERNEL SPECIFIC DATA AND FUNCTIONS NEEDED TO PROVIDE THE             */
/* FUNCTIONALITY REQUESTED BY THE TRACE RECORDER                        */
/************************************************************************/

/* These functions are implemented in the .c file since certain header 
files must not be included in this one */
unsigned char prvTraceGetObjectType(void* handle);
objectHandleType prvTraceGetTaskNumber(void* handle);
unsigned char prvTraceIsSchedulerActive(void);
unsigned char prvTraceIsSchedulerSuspended(void);
unsigned char prvTraceIsSchedulerStarted(void);
void prvTraceEnterCritical(void);
void prvTraceExitCritical(void);
void* prvTraceGetCurrentTaskHandle(void);
char* prvTraceAppend(char *, char *);


/************************************************************************/
/* KERNEL SPECIFIC MACROS USED BY THE TRACE RECORDER                    */
/************************************************************************/

#define TRACE_MALLOC(size) pvPortMalloc(size)

#define TRACE_IS_SCHEDULER_ACTIVE() prvTraceIsSchedulerActive()
#define TRACE_IS_SCHEDULER_STARTED() prvTraceIsSchedulerStarted()
#define TRACE_IS_SCHEDULER_SUSPENDED() prvTraceIsSchedulerSuspended()
#define TRACE_GET_CURRENT_TASK() prvTraceGetCurrentTaskHandle()
#define TRACE_GET_NEXT_TASK() prvTraceGetNextTaskHandle()

#define TRACE_GET_TASK_PRIORITY(pxTCB) ((uint8_t)pxTCB->Prio)              /* Prio is a field in OS_TCB (0: highest priority).     */
#define TRACE_GET_TASK_NAME(pxTCB) ((char*)pxTCB->NamePtr)                 /* NamePtr is a field in OS_TCB.                        */
#define TRACE_GET_TASK_NUMBER(pxTCB) (prvTraceGetTaskNumber(pxTCB))
#define TRACE_SET_TASK_NUMBER(pxTCB) pxTCB->TaskID = xTraceGetObjectHandle(TRACE_CLASS_TASK);

#define TRACE_GET_OBJECT_TRACE_CLASS(CLASS, pxObject) TRACE_GET_##CLASS##_TRACE_CLASS()

#define TRACE_GET_OBJECT_NUMBER(CLASS, pxObject) TRACE_GET_##CLASS##_NUMBER(pxObject)
#define TRACE_SET_OBJECT_NUMBER(CLASS, pxObject) TRACE_SET_##CLASS##_NUMBER(pxObject) 

#define TRACE_GET_CLASS_EVENT_CODE(SERVICE, RESULT, CLASS, kernelClass) (uint8_t)(EVENTGROUP_##SERVICE##_##RESULT + TRACE_GET_##CLASS##_TRACE_CLASS())
#define TRACE_GET_OBJECT_EVENT_CODE(SERVICE, RESULT, CLASS, pxObject) (uint8_t)(EVENTGROUP_##SERVICE##_##RESULT + TRACE_GET_##CLASS##_TRACE_CLASS())
#define TRACE_GET_TASK_EVENT_CODE(SERVICE, RESULT, CLASS, pxTCB) (EVENTGROUP_##SERVICE##_##RESULT + TRACE_CLASS_TASK)

/**************** The helper macros, one pair for each of Micrium's kernel object types. Note the use of MICRIUM_TASK_MSG_Q in the macro name. */
#define TRACE_GET_MICRIUM_TASK_MSG_Q_NUMBER(p_msg_q) (((OS_MSG_Q *)p_msg_q)->MsgQID)
#define TRACE_SET_MICRIUM_TASK_MSG_Q_NUMBER(p_msg_q) (((OS_MSG_Q *)p_msg_q)->MsgQID = xTraceGetObjectHandle(TRACE_CLASS_TASK_Q))

#define TRACE_GET_MICRIUM_TASK_SEM_NUMBER(p_tcb) (((OS_TCB *)p_tcb)->SemID)
#define TRACE_SET_MICRIUM_TASK_SEM_NUMBER(p_tcb) (((OS_TCB *)p_tcb)->SemID = xTraceGetObjectHandle(TRACE_CLASS_TASK_SEM))

#define TRACE_GET_MICRIUM_MUTEX_NUMBER(p_mutex) (((OS_MUTEX *)p_mutex)->MutexID)
#define TRACE_SET_MICRIUM_MUTEX_NUMBER(p_mutex) (((OS_MUTEX *)p_mutex)->MutexID = xTraceGetObjectHandle(TRACE_CLASS_MUTEX))

#define TRACE_GET_MICRIUM_SEM_NUMBER(p_sem) (((OS_SEM *)p_sem)->SemID)
#define TRACE_SET_MICRIUM_SEM_NUMBER(p_sem) (((OS_SEM *)p_sem)->SemID = xTraceGetObjectHandle(TRACE_CLASS_SEMAPHORE))

#define TRACE_GET_MICRIUM_Q_NUMBER(p_q) (((OS_Q *)p_q)->MsgQ.MsgQID)
#define TRACE_SET_MICRIUM_Q_NUMBER(p_q) (((OS_Q *)p_q)->MsgQ.MsgQID = xTraceGetObjectHandle(TRACE_CLASS_QUEUE))

#define TRACE_GET_MICRIUM_FLAG_NUMBER(p_grp) (((OS_FLAG_GRP *)p_grp)->FlagID)
#define TRACE_SET_MICRIUM_FLAG_NUMBER(p_grp) (((OS_FLAG_GRP *)p_grp)->FlagID = xTraceGetObjectHandle(TRACE_CLASS_FLAG))

#define TRACE_GET_MICRIUM_MEM_NUMBER(p_mem) (((OS_MEM *)p_mem)->MemID)
#define TRACE_SET_MICRIUM_MEM_NUMBER(p_mem) (((OS_MEM *)p_mem)->MemID = xTraceGetObjectHandle(TRACE_CLASS_MEM))

// this replaces the lookup table, create one for each Micrium kernel object
#define TRACE_GET_MICRIUM_Q_TRACE_CLASS()          TRACE_CLASS_QUEUE
#define TRACE_GET_MICRIUM_SEM_TRACE_CLASS()        TRACE_CLASS_SEMAPHORE
#define TRACE_GET_MICRIUM_MUTEX_TRACE_CLASS()      TRACE_CLASS_MUTEX
#define TRACE_GET_MICRIUM_FLAG_TRACE_CLASS()       TRACE_CLASS_FLAG
#define TRACE_GET_MICRIUM_MEM_TRACE_CLASS()        TRACE_CLASS_MEM
#define TRACE_GET_MICRIUM_TASK_SEM_TRACE_CLASS()   TRACE_CLASS_TASK_SEM
#define TRACE_GET_MICRIUM_TASK_MSG_Q_TRACE_CLASS() TRACE_CLASS_TASK_Q


/************************************************************************/
/* KERNEL SPECIFIC WRAPPERS THAT SHOULD BE CALLED BY THE KERNEL         */
/************************************************************************/

/* Called for each task that becomes ready */
/* No critical section needed */
#undef traceMOVED_TASK_TO_READY_STATE
#define traceMOVED_TASK_TO_READY_STATE(p_tcb) \
	trcKERNEL_HOOKS_MOVED_TASK_TO_READY_STATE(p_tcb);

/* Called on each OS tick. Will call uiPortGetTimestamp to make sure it is called at least once every OS tick. */
/* No critical section needed */
#undef traceTASK_INCREMENT_TICK
#define traceTASK_INCREMENT_TICK(OSTickCtr) \
	if (OSRunning == 1) { trcKERNEL_HOOKS_NEW_TIME(DIV_NEW_TIME, OSTickCtr + 1); } \
    else if (OSRunning == 0) { trcKERNEL_HOOKS_INCREMENT_TICK(); }

/* Called on each task-switch-in */
/* No critical section needed */
#undef traceTASK_SWITCHED_IN
#define traceTASK_SWITCHED_IN(p_tcb) \
	trcKERNEL_HOOKS_TASK_SWITCH(p_tcb);

/* Called in OSTaskSuspend() */
/* Critical section needed. Also SR_ALLOC needed. */
#undef traceTASK_SUSPEND
#define traceTASK_SUSPEND(p_tcb) \
	{ \
		TRACE_SR_ALLOC_CRITICAL_SECTION(); \
		TRACE_ENTER_CRITICAL_SECTION(); \
		trcKERNEL_HOOKS_TASK_SUSPEND(TASK_SUSPEND, p_tcb); \
		TRACE_EXIT_CRITICAL_SECTION(); \
	}

/* Called on any of the task delay functions */
/* No critical section needed */
#undef traceTASK_DELAY
#define traceTASK_DELAY(dly_ticks) \
	trcKERNEL_HOOKS_TASK_DELAY(TASK_DELAY, OSTCBCurPtr, dly_ticks); \
	trcKERNEL_HOOKS_SET_TASK_INSTANCE_FINISHED();

#if (INCLUDE_OBJECT_DELETE == 1)
/* Called each time a task gets deleted */
/* No critical section needed */
#undef traceTASK_DELETE
#define traceTASK_DELETE(p_tcb) \
	trcKERNEL_HOOKS_TASK_DELETE(DELETE, p_tcb);

/* Called on OSSemDel */
/* No critical section needed */
#undef traceSEM_DELETE
#define traceSEM_DELETE(CLASS, p_sem) \
	trcKERNEL_HOOKS_OBJECT_DELETE(DELETE, CLASS, p_sem);

/* Called on OSQDel */
/* No critical section needed */
#undef traceQ_DELETE
#define traceQ_DELETE(CLASS, p_q) \
	trcKERNEL_HOOKS_OBJECT_DELETE(DELETE, CLASS, p_q);

/* Called on OSMutexDel */
/* No critical section needed */
#undef traceMUTEX_DELETE
#define traceMUTEX_DELETE(CLASS, p_mutex) \
	trcKERNEL_HOOKS_OBJECT_DELETE(DELETE, CLASS, p_mutex);

/* Called on OSFlagDel */
/* No critical section needed */
#undef traceFLAG_DELETE
#define traceFLAG_DELETE(CLASS, p_grp) \
	trcKERNEL_HOOKS_OBJECT_DELETE(DELETE, CLASS, p_grp);

#endif

/* Called each time a task is created */
/* Critical section needed */
#undef traceTASK_CREATE
#define traceTASK_CREATE(p_tcb) \
	TRACE_ENTER_CRITICAL_SECTION(); \
	if (p_tcb != NULL) \
	{ \
		trcKERNEL_HOOKS_TASK_CREATE(CREATE, UNUSED, p_tcb); \
	} \
	TRACE_EXIT_CRITICAL_SECTION();

/* Called in OSTaskCreate, if it fails (typically if the stack can not be allocated) */
/* Need to protect critical section without reallocating the SR.                     */
#undef traceTASK_CREATE_FAILED
#define traceTASK_CREATE_FAILED(p_tcb) \
	TRACE_ENTER_CRITICAL_SECTION(); \
	trcKERNEL_HOOKS_TASK_CREATE_FAILED(CREATE, UNUSED); \
	TRACE_EXIT_CRITICAL_SECTION();

/* Called in OSTaskCreate, while it creates the built-in message queue */
/* Need to protect critical section without reallocating the SR.       */
#undef traceTASK_MSG_Q_CREATE
#define traceTASK_MSG_Q_CREATE(CLASS, p_msg_q, p_name )\
	TRACE_ENTER_CRITICAL_SECTION(); \
	trcKERNEL_HOOKS_OBJECT_CREATE(CREATE, CLASS, p_msg_q); \
    vTraceSetObjectName(TRACE_GET_OBJECT_TRACE_CLASS(CLASS, p_msg_q), TRACE_GET_OBJECT_NUMBER(CLASS, p_msg_q), prvTraceAppend(p_name, "Q")); \
	TRACE_EXIT_CRITICAL_SECTION();

/* Called in OSTaskCreate, while it creates the built-in message queue */
/* Need to protect critical section without reallocating the SR.       */
#undef traceTASK_SEM_CREATE
#define traceTASK_SEM_CREATE(CLASS, p_tcb, p_name )\
	TRACE_ENTER_CRITICAL_SECTION(); \
	trcKERNEL_HOOKS_OBJECT_CREATE(CREATE, CLASS, p_tcb); \
    vTraceSetObjectName(TRACE_GET_OBJECT_TRACE_CLASS(CLASS, p_tcb), TRACE_GET_OBJECT_NUMBER(CLASS, p_tcb), prvTraceAppend(p_name, "Sem")); \
	TRACE_EXIT_CRITICAL_SECTION();

/* No critical section needed */
#undef traceMUTEX_CREATE
#define traceMUTEX_CREATE(CLASS, p_mutex, p_name )\
	trcKERNEL_HOOKS_OBJECT_CREATE(CREATE, CLASS, p_mutex); \
	vTraceSetObjectName(TRACE_GET_OBJECT_TRACE_CLASS(CLASS, p_mutex), TRACE_GET_OBJECT_NUMBER(CLASS, p_mutex), p_name);
	
/* No critical section needed */
#undef traceSEM_CREATE
#define traceSEM_CREATE(CLASS, p_sem, p_name )\
	trcKERNEL_HOOKS_OBJECT_CREATE(CREATE, CLASS, p_sem); \
    vTraceSetObjectName(TRACE_GET_OBJECT_TRACE_CLASS(CLASS, p_sem), TRACE_GET_OBJECT_NUMBER(CLASS, p_sem), p_name);

/* No critical section needed */
#undef traceQ_CREATE
#define traceQ_CREATE(CLASS, p_q, p_name )\
	trcKERNEL_HOOKS_OBJECT_CREATE(CREATE, CLASS, p_q); \
	vTraceSetObjectName(TRACE_GET_OBJECT_TRACE_CLASS(CLASS, p_q), TRACE_GET_OBJECT_NUMBER(CLASS, p_q), p_name);

/* No critical section needed */
#undef traceFLAG_CREATE
#define traceFLAG_CREATE(CLASS, p_grp, p_name )\
	trcKERNEL_HOOKS_OBJECT_CREATE(CREATE, CLASS, p_grp); \
	vTraceSetObjectName(TRACE_GET_OBJECT_TRACE_CLASS(CLASS, p_grp), TRACE_GET_OBJECT_NUMBER(CLASS, p_grp), p_name);

/* No critical section needed */
#undef traceMEM_CREATE
#define traceMEM_CREATE(CLASS, p_mem, p_name )\
	trcKERNEL_HOOKS_OBJECT_CREATE(CREATE, CLASS, p_mem); \
	vTraceSetObjectName(TRACE_GET_OBJECT_TRACE_CLASS(CLASS, p_mem), TRACE_GET_OBJECT_NUMBER(CLASS, p_mem), p_name);

/* Called when a message is sent to a queue */
/* Critical section needed. Also SR_ALLOC needed. */
#undef traceTASK_MSG_Q_SEND
#define traceTASK_MSG_Q_SEND(CLASS, p_msg_q) \
	{ \
		TRACE_SR_ALLOC_CRITICAL_SECTION(); \
		TRACE_ENTER_CRITICAL_SECTION(); \
		trcKERNEL_HOOKS_KERNEL_SERVICE(SEND, SUCCESS, CLASS, p_msg_q); \
		trcKERNEL_HOOKS_SET_OBJECT_STATE(CLASS, p_msg_q, (uint32_t)((OS_MSG_Q *)(p_msg_q)->NbrEntries)); \
		TRACE_EXIT_CRITICAL_SECTION(); \
	}

/* No critical section needed */
#undef traceTASK_MSG_Q_SEND_FAILED
#define traceTASK_MSG_Q_SEND_FAILED(CLASS, p_msg_q) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(SEND, FAILED, CLASS, p_msg_q);	
	
/* Called when a signal is posted to a task semaphore (Post) */
/* Critical section needed. Also SR_ALLOC needed. */
#undef traceTASK_SEM_SEND
#define traceTASK_SEM_SEND(CLASS, p_tcb) \
  { \
		TRACE_SR_ALLOC_CRITICAL_SECTION(); \
		TRACE_ENTER_CRITICAL_SECTION(); \
		trcKERNEL_HOOKS_KERNEL_SERVICE(SEND, SUCCESS, CLASS, p_tcb); \
		trcKERNEL_HOOKS_SET_OBJECT_STATE(CLASS, p_tcb, (uint8_t)(p_tcb->SemCtr)); \
		TRACE_EXIT_CRITICAL_SECTION(); \
	}

/* No critical section needed */
#undef traceTASK_SEM_SEND_FAILED
#define traceTASK_SEM_SEND_FAILED(CLASS, p_tcb) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(SEND, FAILED, CLASS, p_tcb);

/* Called when a signal is posted to a semaphore (Post) */
/* Critical section needed. Also SR_ALLOC needed. */
#undef traceSEM_SEND
#define traceSEM_SEND(CLASS, p_sem) \
  { \
		TRACE_SR_ALLOC_CRITICAL_SECTION(); \
		TRACE_ENTER_CRITICAL_SECTION(); \
	trcKERNEL_HOOKS_KERNEL_SERVICE(SEND, SUCCESS, CLASS, p_sem); \
		trcKERNEL_HOOKS_SET_OBJECT_STATE(CLASS, p_sem, (uint8_t)(p_sem->Ctr)); \
		TRACE_EXIT_CRITICAL_SECTION(); \
	}

/* No critical section needed */
#undef traceSEM_SEND_FAILED
#define traceSEM_SEND_FAILED(CLASS, p_sem) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(SEND, FAILED, CLASS, p_sem);

/* Called when a signal is posted to a semaphore (Post) */
/* Critical section needed. Also SR_ALLOC needed. */
#undef traceQ_SEND
#define traceQ_SEND(CLASS, p_q) \
  { \
		TRACE_SR_ALLOC_CRITICAL_SECTION(); \
		TRACE_ENTER_CRITICAL_SECTION(); \
		trcKERNEL_HOOKS_KERNEL_SERVICE(SEND, SUCCESS, CLASS, p_q); \
		trcKERNEL_HOOKS_SET_OBJECT_STATE(CLASS, p_q, (uint8_t)(p_q->PendList.NbrEntries)); \
		TRACE_EXIT_CRITICAL_SECTION(); \
	}

/* No critical section needed */
#undef traceQ_SEND_FAILED
#define traceQ_SEND_FAILED(CLASS, p_q) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(SEND, FAILED, CLASS, p_q);
	
/* Called when a signal is posted to a mutex (Post) */
/* No critical section needed */
#undef traceMUTEX_SEND
#define traceMUTEX_SEND(CLASS, p_mutex) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(SEND, SUCCESS, CLASS, p_mutex);
	
/* No critical section needed */
#undef traceMUTEX_SEND_FAILED
#define traceMUTEX_SEND_FAILED(CLASS, p_mutex) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(SEND, FAILED, CLASS, p_mutex);

/* Called when a signal is posted to a event flag group (Post) */
/* No critical section needed */
#undef traceFLAG_SEND
#define traceFLAG_SEND(CLASS, p_grp) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(SEND, SUCCESS, CLASS, p_grp);

/* No critical section needed */
#undef traceFLAG_SEND_FAILED
#define traceFLAG_SEND_FAILED(CLASS, p_grp) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(SEND, FAILED, CLASS, p_grp);

/* Called when a task or ISR returns a memory partition (put) */
/* Critical section needed */
#undef traceMEM_SEND
#define traceMEM_SEND(CLASS, p_mem) \
	TRACE_ENTER_CRITICAL_SECTION(); \
	trcKERNEL_HOOKS_KERNEL_SERVICE(SEND, SUCCESS, CLASS, p_mem); \
	trcKERNEL_HOOKS_SET_OBJECT_STATE(CLASS, p_mem, (uint8_t)(p_mem->NbrFree)); \
	TRACE_EXIT_CRITICAL_SECTION();

/* No critical section needed */
#undef traceMEM_SEND_FAILED
#define traceMEM_SEND_FAILED(CLASS, p_mem) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(SEND, FAILED, CLASS, p_mem);

/* Called when a message is received from a task message queue */
/* Critical section needed */
#undef traceTASK_MSG_Q_RECEIVE
#define traceTASK_MSG_Q_RECEIVE(CLASS, p_msg_q) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(RECEIVE, SUCCESS, CLASS, p_msg_q); \
	trcKERNEL_HOOKS_SET_OBJECT_STATE(CLASS, p_msg_q, (uint8_t)(p_msg_q->NbrEntries));

/* No critical section needed */
#undef traceTASK_MSG_Q_RECEIVE_FAILED
#define traceTASK_MSG_Q_RECEIVE_FAILED(CLASS, p_msg_q) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(RECEIVE, FAILED, CLASS, p_msg_q);

/* Called when a message is received from a task message queue */
/* No critical section needed */
#undef traceTASK_MSG_Q_RECEIVE_BLOCK
#define traceTASK_MSG_Q_RECEIVE_BLOCK(CLASS, p_msg_q) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(RECEIVE, BLOCK, CLASS, p_msg_q); \
	trcKERNEL_HOOKS_SET_TASK_INSTANCE_FINISHED();
//	trcKERNEL_HOOKS_SET_OBJECT_STATE(CLASS, p_msg_q, (uint8_t)(p_msg_q->NbrEntries)); \

/* Called when a task pends on a task semaphore (Pend)*/
/* No critical section needed */
#undef traceTASK_SEM_RECEIVE
#define traceTASK_SEM_RECEIVE(CLASS, p_tcb) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(RECEIVE, SUCCESS, CLASS, p_tcb); \
	trcKERNEL_HOOKS_SET_OBJECT_STATE(CLASS, p_tcb, (uint8_t)(p_tcb->SemCtr));

/* No critical section needed */
#undef traceTASK_SEM_RECEIVE_FAILED
#define traceTASK_SEM_RECEIVE_FAILED(CLASS, p_tcb) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(RECEIVE, FAILED, CLASS, p_tcb);

/* Called when a task pends and blocks on a task semaphore (Pend)*/
/* No critical section needed */
#undef traceTASK_SEM_RECEIVE_BLOCK
#define traceTASK_SEM_RECEIVE_BLOCK(CLASS, p_tcb) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(RECEIVE, BLOCK, CLASS, p_tcb); \
	trcKERNEL_HOOKS_SET_TASK_INSTANCE_FINISHED();
//	trcKERNEL_HOOKS_SET_OBJECT_STATE(CLASS, p_tcb, (uint8_t)(p_tcb->SemCtr)); \

/* Called when a task pends on a semaphore (Pend)*/
/* No critical section needed */
#undef traceSEM_RECEIVE
#define traceSEM_RECEIVE(CLASS, p_sem) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(RECEIVE, SUCCESS, CLASS, p_sem); \
	trcKERNEL_HOOKS_SET_OBJECT_STATE(CLASS, p_sem, (uint8_t)(p_sem->Ctr));
	
/* No critical section needed */
#undef traceSEM_RECEIVE_FAILED
#define traceSEM_RECEIVE_FAILED(CLASS, p_sem) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(RECEIVE, FAILED, CLASS, p_sem);

/* Called when a task pends on a semaphore (Pend)*/
/* No critical section needed */
#undef traceSEM_RECEIVE_BLOCK
#define traceSEM_RECEIVE_BLOCK(CLASS, p_sem) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(RECEIVE, BLOCK, CLASS, p_sem); \
	trcKERNEL_HOOKS_SET_TASK_INSTANCE_FINISHED();
	//trcKERNEL_HOOKS_SET_OBJECT_STATE(CLASS, p_sem, (uint8_t)(p_sem->Ctr)); \

/* Called when a message is received from a message queue */
/* Critical section needed */
#undef traceQ_RECEIVE
#define traceQ_RECEIVE(CLASS, p_q) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(RECEIVE, SUCCESS, CLASS, p_q); \
	trcKERNEL_HOOKS_SET_OBJECT_STATE(CLASS, p_q, (uint8_t)(p_q->PendList.NbrEntries));

/* No critical section needed */
#undef traceQ_RECEIVE_FAILED
#define traceQ_RECEIVE_FAILED(CLASS, p_q) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(RECEIVE, FAILED, CLASS, p_q);

/* Called when a message is received from a message queue */
/* No critical section needed */
#undef traceQ_RECEIVE_BLOCK
#define traceQ_RECEIVE_BLOCK(CLASS, p_q) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(RECEIVE, BLOCK, CLASS, p_q); \
	trcKERNEL_HOOKS_SET_TASK_INSTANCE_FINISHED();
//	trcKERNEL_HOOKS_SET_OBJECT_STATE(CLASS, p_q, (uint8_t)(p_q->PendList.NbrEntries)); \

/* Called when a task pends on a mutex (Pend)*/
/* No critical section needed */
#undef traceMUTEX_RECEIVE
#define traceMUTEX_RECEIVE(CLASS, p_mutex) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(RECEIVE, SUCCESS, CLASS, p_mutex);

/* No critical section needed */
#undef traceMUTEX_RECEIVE_FAILED
#define traceMUTEX_RECEIVE_FAILED(CLASS, p_mutex) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(RECEIVE, FAILED, CLASS, p_mutex);
	
/* Called when a task pends on a mutex (Pend)*/
/* No critical section needed */
#undef traceMUTEX_RECEIVE_BLOCK
#define traceMUTEX_RECEIVE_BLOCK(CLASS, p_mutex) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(RECEIVE, BLOCK, CLASS, p_mutex);

/* Called when a task pends on an event flag group (Pend)*/
/* No critical section needed */
#undef traceFLAG_RECEIVE
#define traceFLAG_RECEIVE(CLASS, p_grp) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(RECEIVE, SUCCESS, CLASS, p_grp);

/* No critical section needed */
#undef traceFLAG_RECEIVE_FAILED
#define traceFLAG_RECEIVE_FAILED(CLASS, p_grp) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(RECEIVE, FAILED, CLASS, p_grp);

/* Called when a task pends on an event flag group (Pend)*/
/* No critical section needed */
#undef traceFLAG_RECEIVE_BLOCK
#define traceFLAG_RECEIVE_BLOCK(CLASS, p_grp) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(RECEIVE, BLOCK, CLASS, p_grp); \
	trcKERNEL_HOOKS_SET_TASK_INSTANCE_FINISHED();

/* Called when a task or ISR gets a memory partition */
/* Critical section needed */
#undef traceMEM_RECEIVE
#define traceMEM_RECEIVE(CLASS, p_mem) \
	TRACE_ENTER_CRITICAL_SECTION(); \
	trcKERNEL_HOOKS_KERNEL_SERVICE(RECEIVE, SUCCESS, CLASS, p_mem); \
	trcKERNEL_HOOKS_SET_OBJECT_STATE(CLASS, p_mem, (uint8_t)(p_mem->NbrFree)); \
	TRACE_EXIT_CRITICAL_SECTION();

/* No critical section needed */
#undef traceMEM_RECEIVE_FAILED
#define traceMEM_RECEIVE_FAILED(CLASS, p_mem) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(RECEIVE, FAILED, CLASS, p_mem);

/* Called in OSTaskChangePrio */
/* No critical section needed */
#undef traceTASK_PRIORITY_SET
#define traceTASK_PRIORITY_SET( pxTask, uxNewPriority ) \
	trcKERNEL_HOOKS_TASK_PRIORITY_CHANGE(TASK_PRIORITY_SET, pxTask, uxNewPriority);

/* Called in by mutex operations, when the task's priority is changed to the one from the mutex's owner */
/* No critical section needed */
#undef traceTASK_PRIORITY_INHERIT
#define traceTASK_PRIORITY_INHERIT(p_tcb, prio) \
	trcKERNEL_HOOKS_TASK_PRIORITY_CHANGE(TASK_PRIORITY_INHERIT, p_tcb, prio);

/* Called in by mutex operations, when the task's priority is changed back to its original */
/* No critical section needed */
#undef traceTASK_PRIORITY_DISINHERIT
#define traceTASK_PRIORITY_DISINHERIT(p_tcb, prio) \
	trcKERNEL_HOOKS_TASK_PRIORITY_CHANGE(TASK_PRIORITY_DISINHERIT, p_tcb, prio);

/* Called in vTaskResume */
/* No critical section needed */
#undef traceTASK_RESUME
#define traceTASK_RESUME(p_tcb) \
	trcKERNEL_HOOKS_TASK_RESUME(TASK_RESUME, p_tcb);

#undef traceISR_REGISTER
#define traceISR_REGISTER(isr_id, isr_name, isr_prio) \
        vTraceSetISRProperties(isr_id, isr_name, isr_prio);

#undef traceISR_BEGIN
#define traceISR_BEGIN(isr_id) \
        vTraceStoreISRBegin(isr_id);

#undef traceISR_END
#define traceISR_END() \
        vTraceStoreISREnd();

/************************************************************************/
/* KERNEL SPECIFIC MACROS TO EXCLUDE OR INCLUDE THINGS IN TRACE         */
/************************************************************************/

/* Returns the exclude state of the object */
uint8_t uiTraceIsObjectExcluded(traceObjectClass objectclass, objectHandleType handle);

#define TRACE_SET_QUEUE_FLAG_ISEXCLUDED(queueIndex) TRACE_SET_FLAG_ISEXCLUDED(excludedObjects, queueIndex)
#define TRACE_CLEAR_QUEUE_FLAG_ISEXCLUDED(queueIndex) TRACE_CLEAR_FLAG_ISEXCLUDED(excludedObjects, queueIndex)
#define TRACE_GET_QUEUE_FLAG_ISEXCLUDED(queueIndex) TRACE_GET_FLAG_ISEXCLUDED(excludedObjects, queueIndex)

#define TRACE_SET_SEMAPHORE_FLAG_ISEXCLUDED(semaphoreIndex) TRACE_SET_FLAG_ISEXCLUDED(excludedObjects, NQueue+1+semaphoreIndex)
#define TRACE_CLEAR_SEMAPHORE_FLAG_ISEXCLUDED(semaphoreIndex) TRACE_CLEAR_FLAG_ISEXCLUDED(excludedObjects, NQueue+1+semaphoreIndex)
#define TRACE_GET_SEMAPHORE_FLAG_ISEXCLUDED(semaphoreIndex) TRACE_GET_FLAG_ISEXCLUDED(excludedObjects, NQueue+1+semaphoreIndex)

#define TRACE_SET_MUTEX_FLAG_ISEXCLUDED(mutexIndex) TRACE_SET_FLAG_ISEXCLUDED(excludedObjects, NQueue+1+NSemaphore+1+mutexIndex)
#define TRACE_CLEAR_MUTEX_FLAG_ISEXCLUDED(mutexIndex) TRACE_CLEAR_FLAG_ISEXCLUDED(excludedObjects, NQueue+1+NSemaphore+1+mutexIndex)
#define TRACE_GET_MUTEX_FLAG_ISEXCLUDED(mutexIndex) TRACE_GET_FLAG_ISEXCLUDED(excludedObjects, NQueue+1+NSemaphore+1+mutexIndex)

#define TRACE_SET_FLAG_FLAG_ISEXCLUDED(evtIndex) TRACE_SET_FLAG_ISEXCLUDED(excludedObjects, NQueue+1+NSemaphore+1+NMutex+1+evtIndex)
#define TRACE_CLEAR_FLAG_FLAG_ISEXCLUDED(evtIndex) TRACE_CLEAR_FLAG_ISEXCLUDED(excludedObjects, NQueue+1+NSemaphore+1+NMutex+1+evtIndex)
#define TRACE_GET_FLAG_FLAG_ISEXCLUDED(evtIndex) TRACE_GET_FLAG_ISEXCLUDED(excludedObjects, NQueue+1+NSemaphore+1+NMutex+1+evtIndex)

#define TRACE_SET_MEM_FLAG_ISEXCLUDED(memIndex)   TRACE_SET_FLAG_ISEXCLUDED(excludedObjects,   NQueue+1+NSemaphore+1+NMutex+1+NFlag+1+memIndex)
#define TRACE_CLEAR_MEM_FLAG_ISEXCLUDED(memIndex) TRACE_CLEAR_FLAG_ISEXCLUDED(excludedObjects, NQueue+1+NSemaphore+1+NMutex+1+NFlag+1+memIndex)
#define TRACE_GET_MEM_FLAG_ISEXCLUDED(memIndex)   TRACE_GET_FLAG_ISEXCLUDED(excludedObjects,   NQueue+1+NSemaphore+1+NMutex+1+NFlag+1+memIndex)

#define TRACE_SET_TASK_SEM_FLAG_ISEXCLUDED(taskSemIndex)   TRACE_SET_FLAG_ISEXCLUDED(excludedObjects,   NQueue+1+NSemaphore+1+NMutex+1+NFlag+1+NMem+1+taskSemIndex)
#define TRACE_CLEAR_TASK_SEM_FLAG_ISEXCLUDED(taskSemIndex) TRACE_CLEAR_FLAG_ISEXCLUDED(excludedObjects, NQueue+1+NSemaphore+1+NMutex+1+NFlag+1+NMem+1+taskSemIndex)
#define TRACE_GET_TASK_SEM_FLAG_ISEXCLUDED(taskSemIndex)   TRACE_GET_FLAG_ISEXCLUDED(excludedObjects,   NQueue+1+NSemaphore+1+NMutex+1+NFlag+1+NMem+1+taskSemIndex)

#define TRACE_SET_TASK_Q_FLAG_ISEXCLUDED(taskQIndex)   TRACE_SET_FLAG_ISEXCLUDED(excludedObjects,   NQueue+1+NSemaphore+1+NMutex+1+NFlag+1+NMem+1+NTaskSem+1+taskQIndex)
#define TRACE_CLEAR_TASK_Q_FLAG_ISEXCLUDED(taskQIndex) TRACE_CLEAR_FLAG_ISEXCLUDED(excludedObjects, NQueue+1+NSemaphore+1+NMutex+1+NFlag+1+NMem+1+NTaskSem+1+taskQIndex)
#define TRACE_GET_TASK_Q_FLAG_ISEXCLUDED(taskQIndex)   TRACE_GET_FLAG_ISEXCLUDED(excludedObjects,   NQueue+1+NSemaphore+1+NMutex+1+NFlag+1+NMem+1+NTaskSem+1+taskQIndex)

#define TRACE_SET_TASK_FLAG_ISEXCLUDED(taskIndex)   TRACE_SET_FLAG_ISEXCLUDED(excludedObjects,   NQueue+1+NSemaphore+1+NMutex+1+NFlag+1+NMem+1+NTaskSem+1+NTaskQ+1+taskIndex)
#define TRACE_CLEAR_TASK_FLAG_ISEXCLUDED(taskIndex) TRACE_CLEAR_FLAG_ISEXCLUDED(excludedObjects, NQueue+1+NSemaphore+1+NMutex+1+NFlag+1+NMem+1+NTaskSem+1+NTaskQ+1+taskIndex)
#define TRACE_GET_TASK_FLAG_ISEXCLUDED(taskIndex)   TRACE_GET_FLAG_ISEXCLUDED(excludedObjects,   NQueue+1+NSemaphore+1+NMutex+1+NFlag+1+NMem+1+NTaskSem+1+NTaskQ+1+taskIndex)


#define TRACE_CLEAR_OBJECT_FLAG_ISEXCLUDED(objectclass, handle) \
switch (objectclass) \
{ \
case TRACE_CLASS_QUEUE: \
	TRACE_CLEAR_QUEUE_FLAG_ISEXCLUDED(handle); \
	break; \
case TRACE_CLASS_SEMAPHORE: \
	TRACE_CLEAR_SEMAPHORE_FLAG_ISEXCLUDED(handle); \
	break; \
case TRACE_CLASS_MUTEX: \
	TRACE_CLEAR_MUTEX_FLAG_ISEXCLUDED(handle); \
	break; \
case TRACE_CLASS_FLAG: \
	TRACE_CLEAR_FLAG_FLAG_ISEXCLUDED(handle); \
	break; \
case TRACE_CLASS_MEM: \
	TRACE_CLEAR_MEM_FLAG_ISEXCLUDED(handle); \
	break; \
case TRACE_CLASS_TASK_SEM: \
	TRACE_CLEAR_TASK_SEM_FLAG_ISEXCLUDED(handle); \
	break; \
case TRACE_CLASS_TASK_Q: \
	TRACE_CLEAR_TASK_Q_FLAG_ISEXCLUDED(handle); \
	break; \
case TRACE_CLASS_TASK: \
	TRACE_CLEAR_TASK_FLAG_ISEXCLUDED(handle); \
	break; \
}

#define TRACE_SET_OBJECT_FLAG_ISEXCLUDED(objectclass, handle) \
switch (objectclass) \
{ \
case TRACE_CLASS_QUEUE: \
	TRACE_SET_QUEUE_FLAG_ISEXCLUDED(handle); \
	break; \
case TRACE_CLASS_SEMAPHORE: \
	TRACE_SET_SEMAPHORE_FLAG_ISEXCLUDED(handle); \
	break; \
case TRACE_CLASS_MUTEX: \
	TRACE_SET_MUTEX_FLAG_ISEXCLUDED(handle); \
	break; \
case TRACE_CLASS_FLAG: \
	TRACE_SET_FLAG_FLAG_ISEXCLUDED(handle); \
	break; \
case TRACE_CLASS_MEM: \
	TRACE_SET_MEM_FLAG_ISEXCLUDED(handle); \
	break; \
case TRACE_CLASS_TASK_SEM: \
	TRACE_SET_TASK_SEM_FLAG_ISEXCLUDED(handle); \
	break; \
case TRACE_CLASS_TASK_Q: \
	TRACE_SET_TASK_Q_FLAG_ISEXCLUDED(handle); \
	break; \
case TRACE_CLASS_TASK: \
	TRACE_SET_TASK_FLAG_ISEXCLUDED(handle); \
	break; \
}

/* Task */
#define vTraceExcludeTaskFromTrace(handle) \
TRACE_SET_TASK_FLAG_ISEXCLUDED(TRACE_GET_TASK_NUMBER(handle));

#define vTraceIncludeTaskInTrace(handle) \
TRACE_CLEAR_TASK_FLAG_ISEXCLUDED(TRACE_GET_TASK_NUMBER(handle));


/* Queue */
#define vTraceExcludeQueueFromTrace(handle) \
TRACE_SET_QUEUE_FLAG_ISEXCLUDED(TRACE_GET_OBJECT_NUMBER(MICRIUM_Q, handle));

#define vTraceIncludeQueueInTrace(handle) \
TRACE_CLEAR_QUEUE_FLAG_ISEXCLUDED(TRACE_GET_OBJECT_NUMBER(MICRIUM_Q, handle));


/* Semaphore */
#define vTraceExcludeSemaphoreFromTrace(handle) \
TRACE_SET_SEMAPHORE_FLAG_ISEXCLUDED(TRACE_GET_OBJECT_NUMBER(MICRIUM_SEM, handle));

#define vTraceIncludeSemaphoreInTrace(handle) \
TRACE_CLEAR_SEMAPHORE_FLAG_ISEXCLUDED(TRACE_GET_OBJECT_NUMBER(MICRIUM_SEM, handle));


/* Mutex */
#define vTraceExcludeMutexFromTrace(handle) \
TRACE_SET_MUTEX_FLAG_ISEXCLUDED(TRACE_GET_OBJECT_NUMBER(MICRIUM_MUTEX, handle));

#define vTraceIncludeMutexInTrace(handle) \
TRACE_CLEAR_MUTEX_FLAG_ISEXCLUDED(TRACE_GET_OBJECT_NUMBER(MICRIUM_MUTEX, handle));


/* Task Q */
#define vTraceExcludeTaskQFromTrace(handle) \
TRACE_SET_TASK_Q_FLAG_ISEXCLUDED(TRACE_GET_OBJECT_NUMBER(MICRIUM_TASK_MSG_Q, handle));

#define vTraceIncludeTaskQInTrace(handle) \
TRACE_CLEAR_TASK_Q_FLAG_ISEXCLUDED(TRACE_GET_OBJECT_NUMBER(MICRIUM_TASK_MSG_Q, handle));


/* Task Sem */
#define vTraceExcludeTaskSemFromTrace(handle) \
TRACE_SET_TASK_SEM_FLAG_ISEXCLUDED(TRACE_GET_OBJECT_NUMBER(MICRIUM_TASK_SEM, handle));

#define vTraceIncludeTaskSemInTrace(handle) \
TRACE_CLEAR_TASK_SEM_FLAG_ISEXCLUDED(TRACE_GET_OBJECT_NUMBER(MICRIUM_TASK_SEM, handle));


/* Flag */
#define vTraceExcludeFlagFromTrace(handle) \
TRACE_SET_FLAG_FLAG_ISEXCLUDED(TRACE_GET_OBJECT_NUMBER(MICRIUM_FLAG, handle));

#define vTraceIncludeFlagInTrace(handle) \
TRACE_CLEAR_FLAG_FLAG_ISEXCLUDED(TRACE_GET_OBJECT_NUMBER(MICRIUM_FLAG, handle));


/* Mem */
#define vTraceExcludeMemFromTrace(handle) \
TRACE_SET_MEM_FLAG_ISEXCLUDED(TRACE_GET_OBJECT_NUMBER(MICRIUM_MEM, handle));

#define vTraceIncludeMemInTrace(handle) \
TRACE_CLEAR_MEM_FLAG_ISEXCLUDED(TRACE_GET_OBJECT_NUMBER(MICRIUM_MEM, handle));


/* Kernel Services */
#define vTraceExcludeKernelServiceDelayFromTrace() \
TRACE_SET_EVENT_CODE_FLAG_ISEXCLUDED(TASK_DELAY); \
TRACE_SET_EVENT_CODE_FLAG_ISEXCLUDED(TASK_DELAY_UNTIL);

#define vTraceIncludeKernelServiceDelayInTrace() \
TRACE_CLEAR_EVENT_CODE_FLAG_ISEXCLUDED(TASK_DELAY); \
TRACE_CLEAR_EVENT_CODE_FLAG_ISEXCLUDED(TASK_DELAY_UNTIL);

/* HELPER MACROS FOR KERNEL SERVICES FOR OBJECTS */
#define vTraceExcludeKernelServiceSendFromTrace_HELPER(class) \
TRACE_SET_EVENT_CODE_FLAG_ISEXCLUDED(EVENTGROUP_SEND_SUCCESS + class); \
TRACE_SET_EVENT_CODE_FLAG_ISEXCLUDED(EVENTGROUP_SEND_BLOCK + class); \
TRACE_SET_EVENT_CODE_FLAG_ISEXCLUDED(EVENTGROUP_SEND_FAILED + class);

#define vTraceIncludeKernelServiceSendInTrace_HELPER(class) \
TRACE_CLEAR_EVENT_CODE_FLAG_ISEXCLUDED(EVENTGROUP_SEND_SUCCESS + class); \
TRACE_CLEAR_EVENT_CODE_FLAG_ISEXCLUDED(EVENTGROUP_SEND_BLOCK + class); \
TRACE_CLEAR_EVENT_CODE_FLAG_ISEXCLUDED(EVENTGROUP_SEND_FAILED + class);

#define vTraceExcludeKernelServiceReceiveFromTrace_HELPER(class) \
TRACE_SET_EVENT_CODE_FLAG_ISEXCLUDED(EVENTGROUP_RECEIVE_SUCCESS + class); \
TRACE_SET_EVENT_CODE_FLAG_ISEXCLUDED(EVENTGROUP_RECEIVE_BLOCK + class); \
TRACE_SET_EVENT_CODE_FLAG_ISEXCLUDED(EVENTGROUP_RECEIVE_FAILED + class);

#define vTraceIncludeKernelServiceReceiveInTrace_HELPER(class) \
TRACE_CLEAR_EVENT_CODE_FLAG_ISEXCLUDED(EVENTGROUP_RECEIVE_SUCCESS + class); \
TRACE_CLEAR_EVENT_CODE_FLAG_ISEXCLUDED(EVENTGROUP_RECEIVE_BLOCK + class); \
TRACE_CLEAR_EVENT_CODE_FLAG_ISEXCLUDED(EVENTGROUP_RECEIVE_FAILED + class);

/* EXCLUDE AND INCLUDE FOR QUEUE */
#define vTraceExcludeKernelServiceQueuePostFromTrace() \
vTraceExcludeKernelServiceSendFromTrace_HELPER(TRACE_CLASS_QUEUE);

#define vTraceIncludeKernelServiceQueuePostInTrace() \
vTraceIncludeKernelServiceSendInTrace_HELPER(TRACE_CLASS_QUEUE);

#define vTraceExcludeKernelServiceQueuePendFromTrace() \
vTraceExcludeKernelServiceReceiveFromTrace_HELPER(TRACE_CLASS_QUEUE);

#define vTraceIncludeKernelServiceQueuePendInTrace() \
vTraceIncludeKernelServiceReceiveInTrace_HELPER(TRACE_CLASS_QUEUE);

/* EXCLUDE AND INCLUDE FOR SEMAPHORE */
#define vTraceExcludeKernelServiceSemaphorePostFromTrace() \
vTraceExcludeKernelServiceSendFromTrace_HELPER(TRACE_CLASS_SEMAPHORE);

#define vTraceIncludeKernelServiceSemaphorePostInTrace() \
vTraceIncludeKernelServiceSendInTrace_HELPER(TRACE_CLASS_SEMAPHORE);

#define vTraceExcludeKernelServiceSemaphorePendFromTrace() \
vTraceExcludeKernelServiceReceiveFromTrace_HELPER(TRACE_CLASS_SEMAPHORE);

#define vTraceIncludeKernelServiceSemaphorePendInTrace() \
vTraceIncludeKernelServiceReceiveInTrace_HELPER(TRACE_CLASS_SEMAPHORE);

/* EXCLUDE AND INCLUDE FOR MUTEX */
#define vTraceExcludeKernelServiceMutexPostFromTrace() \
vTraceExcludeKernelServiceSendFromTrace_HELPER(TRACE_CLASS_MUTEX);

#define vTraceIncludeKernelServiceMutexPostInTrace() \
vTraceIncludeKernelServiceSendInTrace_HELPER(TRACE_CLASS_MUTEX);

#define vTraceExcludeKernelServiceMutexPendFromTrace() \
vTraceExcludeKernelServiceReceiveFromTrace_HELPER(TRACE_CLASS_MUTEX);

#define vTraceIncludeKernelServiceMutexPendInTrace() \
vTraceIncludeKernelServiceReceiveInTrace_HELPER(TRACE_CLASS_MUTEX);

/* EXCLUDE AND INCLUDE FOR FLAG */
#define vTraceExcludeKernelServiceFlagPostFromTrace() \
vTraceExcludeKernelServiceSendFromTrace_HELPER(TRACE_CLASS_FLAG);

#define vTraceIncludeKernelServiceFlagPostInTrace() \
vTraceIncludeKernelServiceSendInTrace_HELPER(TRACE_CLASS_FLAG);

#define vTraceExcludeKernelServiceFlagPendFromTrace() \
vTraceExcludeKernelServiceReceiveFromTrace_HELPER(TRACE_CLASS_FLAG);

#define vTraceIncludeKernelServiceFlagPendInTrace() \
vTraceIncludeKernelServiceReceiveInTrace_HELPER(TRACE_CLASS_FLAG);

/* EXCLUDE AND INCLUDE FOR MEM */
#define vTraceExcludeKernelServiceMemPostFromTrace() \
vTraceExcludeKernelServiceSendFromTrace_HELPER(TRACE_CLASS_MEM);

#define vTraceIncludeKernelServiceMemPostInTrace() \
vTraceIncludeKernelServiceSendInTrace_HELPER(TRACE_CLASS_MEM);

#define vTraceExcludeKernelServiceMemPendFromTrace() \
vTraceExcludeKernelServiceReceiveFromTrace_HELPER(TRACE_CLASS_MEM);

#define vTraceIncludeKernelServiceMemPendInTrace() \
vTraceIncludeKernelServiceReceiveInTrace_HELPER(TRACE_CLASS_MEM);

/* EXCLUDE AND INCLUDE FOR TASK Q */
#define vTraceExcludeKernelServiceTaskQPostFromTrace() \
vTraceExcludeKernelServiceSendFromTrace_HELPER(TRACE_CLASS_TASK_Q);

#define vTraceIncludeKernelServiceTaskQPostInTrace() \
vTraceIncludeKernelServiceSendInTrace_HELPER(TRACE_CLASS_TASK_Q);

#define vTraceExcludeKernelServiceTaskQPendFromTrace() \
vTraceExcludeKernelServiceReceiveFromTrace_HELPER(TRACE_CLASS_TASK_Q);

#define vTraceIncludeKernelServiceTaskQPendInTrace() \
vTraceIncludeKernelServiceReceiveInTrace_HELPER(TRACE_CLASS_TASK_Q);

/* EXCLUDE AND INCLUDE FOR TASK SEM */
#define vTraceExcludeKernelServiceTaskSemPostFromTrace() \
vTraceExcludeKernelServiceSendFromTrace_HELPER(TRACE_CLASS_TASK_SEM);

#define vTraceIncludeKernelServiceTaskSemPostInTrace() \
vTraceIncludeKernelServiceSendInTrace_HELPER(TRACE_CLASS_TASK_SEM);

#define vTraceExcludeKernelServiceTaskSemPendFromTrace() \
vTraceExcludeKernelServiceReceiveFromTrace_HELPER(TRACE_CLASS_TASK_SEM);

#define vTraceIncludeKernelServiceTaskSemPendInTrace() \
vTraceIncludeKernelServiceReceiveInTrace_HELPER(TRACE_CLASS_TASK_SEM);

/************************************************************************/
/* KERNEL SPECIFIC MACROS TO NAME OBJECTS, IF NECESSARY                 */
/************************************************************************/
// Not necessary

#endif

#endif /* TRCKERNELPORT_H */
