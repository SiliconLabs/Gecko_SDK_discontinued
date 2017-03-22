/*******************************************************************************
 * Tracealyzer v3.0.7 Recorder Library
 * Percepio AB, www.percepio.com
 *
 * trcKernelPortMicrium.c
 *
 * Kernel-specific functionality for uC/OS-III, used by the recorder library.
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

#include "trcKernelPort.h"

#if (USE_TRACEALYZER_RECORDER == 1)

#include <stdint.h>

#include <os.h>


objectHandleType prvTraceGetTaskNumber(void* handle)
{
	OS_TCB *os_tcb;

	if (handle != NULL)
	{
		os_tcb = (OS_TCB *)handle;
		return (os_tcb->TaskID);
	}

	return NULL;
}

char * prvTraceAppend(char *p_name, char *suffix)
{
	static char buf[64];
	char *name_ptr;

	name_ptr = (char *)p_name;
	
	Str_Copy_N(&buf[0], name_ptr, 64);
	Str_Cat_N(buf, " ", 64);
	Str_Cat_N(buf, suffix, 64);

	return (&buf[0]);
}

unsigned char prvTraceIsSchedulerActive()
{
	return OSRunning == OS_STATE_OS_RUNNING;				/* OSRunning is a global variable declared by uC/OS-III */
															/* ???? && OSSchedLockNestingCtr == (OS_NESTING_CTR)0 */
}

unsigned char prvTraceIsSchedulerSuspended()
{
	return OSRunning == OS_STATE_OS_STOPPED;				/* OSRunning is a global variable declared by uC/OS-III */
															/* ???? && OSSchedLockNestingCtr > (OS_NESTING_CTR)0 */
}

unsigned char prvTraceIsSchedulerStarted()
{
	return OSRunning == OS_STATE_OS_RUNNING;				/* OSRunning is a global variable declared by uC/OS-III */
}

void* prvTraceGetCurrentTaskHandle()
{
	return OSTCBCurPtr;										/* OSTCBCurPtr is a pointer to currently running TCB */
}

void* prvTraceGetNextTaskHandle()
{
	return OSTCBHighRdyPtr;									/* OSTCBHighRdyPtr is a pointer to the next TCB to switch */
}

/* Initialization of the object property table */
void vTraceInitObjectPropertyTable()
{
	RecorderDataPtr->ObjectPropertyTable.NumberOfObjectClasses = TRACE_NCLASSES;
	RecorderDataPtr->ObjectPropertyTable.NumberOfObjectsPerClass[0] = NQueue;
	RecorderDataPtr->ObjectPropertyTable.NumberOfObjectsPerClass[1] = NSemaphore;
	RecorderDataPtr->ObjectPropertyTable.NumberOfObjectsPerClass[2] = NMutex;
	RecorderDataPtr->ObjectPropertyTable.NumberOfObjectsPerClass[3] = NFlag;
	RecorderDataPtr->ObjectPropertyTable.NumberOfObjectsPerClass[4] = NMem;
	RecorderDataPtr->ObjectPropertyTable.NumberOfObjectsPerClass[5] = NTaskSem;
	RecorderDataPtr->ObjectPropertyTable.NumberOfObjectsPerClass[6] = NTaskQ;
	RecorderDataPtr->ObjectPropertyTable.NumberOfObjectsPerClass[7] = NTask;
	RecorderDataPtr->ObjectPropertyTable.NumberOfObjectsPerClass[8] = NISR;
	RecorderDataPtr->ObjectPropertyTable.NameLengthPerClass[0] = NameLenQueue;
	RecorderDataPtr->ObjectPropertyTable.NameLengthPerClass[1] = NameLenSemaphore;
	RecorderDataPtr->ObjectPropertyTable.NameLengthPerClass[2] = NameLenMutex;
	RecorderDataPtr->ObjectPropertyTable.NameLengthPerClass[3] = NameLenFlag;
	RecorderDataPtr->ObjectPropertyTable.NameLengthPerClass[4] = NameLenMem;
	RecorderDataPtr->ObjectPropertyTable.NameLengthPerClass[5] = NameLenTaskSem;
	RecorderDataPtr->ObjectPropertyTable.NameLengthPerClass[6] = NameLenTaskQ;
	RecorderDataPtr->ObjectPropertyTable.NameLengthPerClass[7] = NameLenTask;
	RecorderDataPtr->ObjectPropertyTable.NameLengthPerClass[8] = NameLenISR;
	RecorderDataPtr->ObjectPropertyTable.TotalPropertyBytesPerClass[0] = PropertyTableSizeQueue;
	RecorderDataPtr->ObjectPropertyTable.TotalPropertyBytesPerClass[1] = PropertyTableSizeSemaphore;
	RecorderDataPtr->ObjectPropertyTable.TotalPropertyBytesPerClass[2] = PropertyTableSizeMutex;
	RecorderDataPtr->ObjectPropertyTable.TotalPropertyBytesPerClass[3] = PropertyTableSizeFlag;
	RecorderDataPtr->ObjectPropertyTable.TotalPropertyBytesPerClass[4] = PropertyTableSizeMem;
	RecorderDataPtr->ObjectPropertyTable.TotalPropertyBytesPerClass[5] = PropertyTableSizeTaskSem;
	RecorderDataPtr->ObjectPropertyTable.TotalPropertyBytesPerClass[6] = PropertyTableSizeTaskQ;
	RecorderDataPtr->ObjectPropertyTable.TotalPropertyBytesPerClass[7] = PropertyTableSizeTask;
	RecorderDataPtr->ObjectPropertyTable.TotalPropertyBytesPerClass[8] = PropertyTableSizeISR;
	RecorderDataPtr->ObjectPropertyTable.StartIndexOfClass[0] = StartIndexQueue;
	RecorderDataPtr->ObjectPropertyTable.StartIndexOfClass[1] = StartIndexSemaphore;
	RecorderDataPtr->ObjectPropertyTable.StartIndexOfClass[2] = StartIndexMutex;
	RecorderDataPtr->ObjectPropertyTable.StartIndexOfClass[3] = StartIndexFlag;
	RecorderDataPtr->ObjectPropertyTable.StartIndexOfClass[4] = StartIndexMem;
	RecorderDataPtr->ObjectPropertyTable.StartIndexOfClass[5] = StartIndexTaskSem;
	RecorderDataPtr->ObjectPropertyTable.StartIndexOfClass[6] = StartIndexTaskQ;
	RecorderDataPtr->ObjectPropertyTable.StartIndexOfClass[7] = StartIndexTask;
	RecorderDataPtr->ObjectPropertyTable.StartIndexOfClass[8] = StartIndexISR;
	RecorderDataPtr->ObjectPropertyTable.ObjectPropertyTableSizeInBytes = TRACE_OBJECT_TABLE_SIZE;
}

/* Initialization of the handle mechanism, see e.g, xTraceGetObjectHandle */
void vTraceInitObjectHandleStack()
{
	objectHandleStacks.indexOfNextAvailableHandle[0] = 0;
	objectHandleStacks.indexOfNextAvailableHandle[1] = NQueue;
	objectHandleStacks.indexOfNextAvailableHandle[2] = NQueue + NSemaphore;
	objectHandleStacks.indexOfNextAvailableHandle[3] = NQueue + NSemaphore + NMutex;
	objectHandleStacks.indexOfNextAvailableHandle[4] = NQueue + NSemaphore + NMutex + NFlag;
	objectHandleStacks.indexOfNextAvailableHandle[5] = NQueue + NSemaphore + NMutex + NFlag + NMem;
	objectHandleStacks.indexOfNextAvailableHandle[6] = NQueue + NSemaphore + NMutex + NFlag + NMem + NTaskSem;
	objectHandleStacks.indexOfNextAvailableHandle[7] = NQueue + NSemaphore + NMutex + NFlag + NMem + NTaskSem + NTaskQ;
	objectHandleStacks.indexOfNextAvailableHandle[8] = NQueue + NSemaphore + NMutex + NFlag + NMem + NTaskSem + NTaskQ + NTask;

	objectHandleStacks.lowestIndexOfClass[0] = 0;
	objectHandleStacks.lowestIndexOfClass[1] = NQueue;
	objectHandleStacks.lowestIndexOfClass[2] = NQueue + NSemaphore;
	objectHandleStacks.lowestIndexOfClass[3] = NQueue + NSemaphore + NMutex;
	objectHandleStacks.lowestIndexOfClass[4] = NQueue + NSemaphore + NMutex + NFlag;
	objectHandleStacks.lowestIndexOfClass[5] = NQueue + NSemaphore + NMutex + NFlag + NMem;
	objectHandleStacks.lowestIndexOfClass[6] = NQueue + NSemaphore + NMutex + NFlag + NMem + NTaskSem;
	objectHandleStacks.lowestIndexOfClass[7] = NQueue + NSemaphore + NMutex + NFlag + NMem + NTaskSem + NTaskQ;
	objectHandleStacks.lowestIndexOfClass[8] = NQueue + NSemaphore + NMutex + NFlag + NMem + NTaskSem + NTaskQ + NTask;

	objectHandleStacks.highestIndexOfClass[0] = NQueue - 1;
	objectHandleStacks.highestIndexOfClass[1] = NQueue + NSemaphore - 1;
	objectHandleStacks.highestIndexOfClass[2] = NQueue + NSemaphore + NMutex - 1;
	objectHandleStacks.highestIndexOfClass[3] = NQueue + NSemaphore + NMutex + NFlag - 1;
	objectHandleStacks.highestIndexOfClass[4] = NQueue + NSemaphore + NMutex + NFlag + NMem - 1;
	objectHandleStacks.highestIndexOfClass[5] = NQueue + NSemaphore + NMutex + NFlag + NMem + NTaskSem - 1;
	objectHandleStacks.highestIndexOfClass[6] = NQueue + NSemaphore + NMutex + NFlag + NMem + NTaskSem + NTaskQ - 1;
	objectHandleStacks.highestIndexOfClass[7] = NQueue + NSemaphore + NMutex + NFlag + NMem + NTaskSem + NTaskQ + NTask - 1;
	objectHandleStacks.highestIndexOfClass[8] = NQueue + NSemaphore + NMutex + NFlag + NMem + NTaskSem + NTaskQ + NTask + NISR - 1;
}

/* Returns the "Not enough handles" error message for this object class */
const char* pszTraceGetErrorNotEnoughHandles(traceObjectClass objectclass)
{
	switch(objectclass)
	{
	case TRACE_CLASS_QUEUE:
		return "Not enough QUEUE handles - increase NQueue in trcConfig.h";
	case TRACE_CLASS_SEMAPHORE:
		return "Not enough SEMAPHORE handles - increase NSemaphore in trcConfig.h";
	case TRACE_CLASS_MUTEX:
		return "Not enough MUTEX handles - increase NMutex in trcConfig.h";
	case TRACE_CLASS_FLAG:
		return "Not enough EVENT FLAG handles - increase NFlag in trcConfig.h";
	case TRACE_CLASS_MEM:
		return "Not enough MEMORY PARTITION handles - increase NMem in trcConfig.h";
	case TRACE_CLASS_TASK_SEM:
		return "Not enough TASK handles - increase NTask in trcConfig.h";
	case TRACE_CLASS_TASK_Q:
		return "Not enough TASK handles - increase NTask in trcConfig.h";
	case TRACE_CLASS_TASK:
		return "Not enough TASK handles - increase NTask in trcConfig.h";
	case TRACE_CLASS_ISR:
		return "Not enough ISR handles - increase NISR in trcConfig.h";
	default:
		return "pszTraceGetErrorHandles: Invalid objectclass!";
	}
}

/* Returns the exclude state of the object */
uint8_t uiTraceIsObjectExcluded(traceObjectClass objectclass, objectHandleType handle)
{
	TRACE_ASSERT(objectclass < TRACE_NCLASSES, "prvTraceIsObjectExcluded: objectclass >= TRACE_NCLASSES", 1);
	TRACE_ASSERT(handle <= RecorderDataPtr->ObjectPropertyTable.NumberOfObjectsPerClass[objectclass], "uiTraceIsObjectExcluded: Invalid value for handle", 1);
	
	switch(objectclass)
	{
	case TRACE_CLASS_QUEUE:
		return TRACE_GET_QUEUE_FLAG_ISEXCLUDED(handle);
	case TRACE_CLASS_SEMAPHORE:
		return TRACE_GET_SEMAPHORE_FLAG_ISEXCLUDED(handle);
	case TRACE_CLASS_MUTEX:
		return TRACE_GET_MUTEX_FLAG_ISEXCLUDED(handle);
	case TRACE_CLASS_FLAG:
		return TRACE_GET_FLAG_FLAG_ISEXCLUDED(handle);
	case TRACE_CLASS_MEM:
		return TRACE_GET_MEM_FLAG_ISEXCLUDED(handle);
	case TRACE_CLASS_TASK_SEM:
		return TRACE_GET_TASK_SEM_FLAG_ISEXCLUDED(handle);
	case TRACE_CLASS_TASK_Q:
		return TRACE_GET_TASK_Q_FLAG_ISEXCLUDED(handle);
	case TRACE_CLASS_TASK:
		return TRACE_GET_TASK_FLAG_ISEXCLUDED(handle);
	}
	
	/* Must never reach */
	return 1;
}

#endif
