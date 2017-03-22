/*******************************************************************************
 * Trace Recorder Library for Tracealyzer v3.0.7
 * Percepio AB, www.percepio.com
 *
 * trcKernelPort.c
 *
 * The kernel-specific code for integration with uC/OS-III.
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

#include "trcKernelPort.h"

#if (USE_TRACEALYZER_RECORDER == 1)

#include <stdint.h>
#include "trcRecorder.h"
#include "trcStreamPort.h"


#include <os.h>
#include <lib_str.h>

/* TzCtrl task Stack and TCB */
static  CPU_STK  TrcCtrlTaskStk[TRC_CTRL_TASK_STACK_SIZE];
static  OS_TCB   TrcCtrlTaskTCB;

/* Called by TzCtrl task periodically (every 100 ms) */
static void CheckRecorderStatus(void);

/* The TzCtrl task - receives commands from Tracealyzer (start/stop) */
static  void     TraceCtrlTask (void  *p_arg);

/* Monitored by TzCtrl task, that give warnings as User Events */
extern volatile uint32_t NoRoomForSymbol;
extern volatile uint32_t NoRoomForObjectData;
extern volatile uint32_t LongestSymbolName;
extern volatile uint32_t MaxBytesTruncated;

#define TRC_PORT_MALLOC(size) SOME_FUNCTION_THAT_ALLOCATES_DYNAMIC_MEMORY(size)
#if ((TRC_STREAM_PORT_BLOCKING_TRANSFER == 1) && (TRC_MEASURE_BLOCKING_TIME == 1))

/*** Used in blocking transfer mode, if enabled TRC_MEASURE_BLOCKING_TIME **************/

/* The highest number of cycles used by Write. */
static volatile int32_t blockingCyclesMax;

/* The number of times Write has blocked due to a full buffer. */
static volatile uint32_t blockingCount;

/* User Event Channel for giving warnings regarding blocking */
static char* trcDiagnosticsChannel;

#endif /*((TRC_STREAM_PORT_BLOCKING_TRANSFER==1) && (TRC_MEASURE_BLOCKING_TIME == 1))*/

TRC_STREAM_PORT_ALLOCATE_FIELDS()

/* User Event Channel for giving warnings regarding NoRoomForSymbol etc. */
char* trcWarningChannel;

/* Keeps track of previous values, to only react on changes. */
static uint32_t NoRoomForSymbol_last = 0;
static uint32_t NoRoomForObjectData_last = 0;
static uint32_t LongestSymbolName_last = 0;
static uint32_t MaxBytesTruncated_last = 0;

/*******************************************************************************
 * prvTraceGetCurrentTaskHandle
 *
 * Function that returns the handle to the currently executing task.
 *
 ******************************************************************************/
void* prvTraceGetCurrentTaskHandle(void)
{
	return OSTCBCurPtr;                                         /* OSTCBCurPtr is a pointer to currently running TCB    */
}

/*******************************************************************************
 * prvIsNewTCB
 *
 * Function that returns the handle to the currently executing task.
 *
 ******************************************************************************/
static void* pCurrentTCB = NULL;
uint32_t prvIsNewTCB(void* pNewTCB)
{
	if (pCurrentTCB != pNewTCB)
	{
		pCurrentTCB = pNewTCB;
		return 1;
	}
	return 0;
}
/*******************************************************************************
 * CheckRecorderStatus
 *
 * Called by TzCtrl task periodically (every 100 ms - seems reasonable).
 * Checks a number of diagnostic variables and give warnings as user events,
 * in most cases including a suggested solution.
 ******************************************************************************/
static void CheckRecorderStatus(void)
{
	if (NoRoomForSymbol > NoRoomForSymbol_last)
	{
		vTracePrintF(trcWarningChannel, "TRC_SYMBOL_TABLE_SLOTS too small. Add %d slots.",
			NoRoomForSymbol);

		NoRoomForSymbol_last = NoRoomForSymbol;
	}

	if (NoRoomForObjectData > NoRoomForObjectData_last)
	{
		vTracePrintF(trcWarningChannel, "TRC_OBJECT_DATA_SLOTS too small. Add %d slots.",
			NoRoomForObjectData);

		NoRoomForObjectData_last = NoRoomForObjectData;
	}

	if (LongestSymbolName > LongestSymbolName_last)
	{
		if (LongestSymbolName > TRC_SYMBOL_MAX_LENGTH)
		{
			vTracePrintF(trcWarningChannel, "TRC_SYMBOL_MAX_LENGTH too small. Add %d chars.",
				LongestSymbolName);
		}
		LongestSymbolName_last = LongestSymbolName;
	}

	if (MaxBytesTruncated > MaxBytesTruncated_last)
	{
		/* Some string event generated a too long string that was truncated.
		This may happen for the following functions:
		- vTracePrintF
		- vTracePrintF
		- vTraceStoreKernelObjectName
		- vTraceStoreUserEventChannelName
		- vTraceSetISRProperties

		A PSF event may store maximum 60 bytes payload, including data arguments
		and string characters. For User Events, also the User Event Channel ptr
		must be squeezed in, if a channel is specified. */

		vTracePrintF(trcWarningChannel, "String event too long, up to %d bytes truncated.",
			MaxBytesTruncated);

		MaxBytesTruncated_last = MaxBytesTruncated;
	}

#if ((TRC_STREAM_PORT_BLOCKING_TRANSFER==1) && (TRC_MEASURE_BLOCKING_TIME))
	if (blockingCount > 0)
	{
		/* At least one case of blocking since the last check and this is
		the longest case. */
		vTracePrintF(trcDiagnosticsChannel, "Longest since last: %d us",
			(uint32_t)blockingCyclesMax/(TRACE_CPU_CLOCK_HZ/1000000));

		blockingCyclesMax = 0;
	}
#endif
}

/*******************************************************************************
 * vTraceOnTraceBegin
 *
 * Called on trace begin.
 ******************************************************************************/
void vTraceOnTraceBegin(void)
{
	TRC_STREAM_PORT_ON_TRACE_BEGIN();
}

/*******************************************************************************
 * vTraceOnTraceEnd
 *
 * Called on trace end.
 ******************************************************************************/
void vTraceOnTraceEnd(void)
{
	TRC_STREAM_PORT_ON_TRACE_END();
}

/*******************************************************************************
 * TzCtrl
 *
 * Task for receiving commands from Tracealyzer and for recorder diagnostics.
 *
 ******************************************************************************/
static  void  TraceCtrlTask (void  *p_arg)
{
    OS_ERR  err;
	TracealyzerCommandType msg;
	int bytes = 0;

	while (1)
	{
		bytes = 0;
		TRC_STREAM_PORT_READ_DATA(&msg, sizeof(TracealyzerCommandType), &bytes);
	    if (bytes != 0)
		{
			if (bytes == sizeof(TracealyzerCommandType))
			{
				if (isValidCommand(&msg))
				{
					processCommand(&msg); /* Start or Stop currently... */
				}
			}
		}

		do
		{
			bytes = 0;
			TRC_STREAM_PORT_PERIODIC_SEND_DATA(&bytes);
		}
		while (bytes != 0);

		CheckRecorderStatus();
		OSTimeDly(100, OS_OPT_TIME_DLY, &err);
	}
}

/*******************************************************************************
 * Trace_Init
 *
 * The main initialization routine for the trace recorder. Configures the stream
 * and activates the TzCtrl task.
 * Also sets up the diagnostic User Event channels used by TzCtrl task.
 *
 ******************************************************************************/
void Trace_Init(void)
{
    OS_ERR err;
    
	TRC_STREAM_PORT_INIT();

//	TRC_PORT_SPECIFIC_INIT();

	trcWarningChannel = vTraceStoreUserEventChannelName("Warnings from Recorder");

#if ((TRC_STREAM_PORT_BLOCKING_TRANSFER==1) && (TRC_MEASURE_BLOCKING_TIME==1))
	trcDiagnosticsChannel = vTraceStoreUserEventChannelName("Blocking on trace buffer");
#endif

  	/* Creates the TzCtrl task - receives trace commands (start, stop, ...) */
	OSTaskCreate((OS_TCB     *)&TrcCtrlTaskTCB,             /* Create the control task                              */
                 (CPU_CHAR   *)"uC/OS-III Trace Control Task",
                 (OS_TASK_PTR ) TraceCtrlTask,
                 (void       *) 0,
                 (OS_PRIO     ) TRC_CTRL_TASK_PRIORITY,
                 (CPU_STK    *)&TrcCtrlTaskStk[0],
                 (CPU_STK_SIZE) TRC_CTRL_TASK_STACK_SIZE / 10u,
                 (CPU_STK_SIZE) TRC_CTRL_TASK_STACK_SIZE,
                 (OS_MSG_QTY  ) 0u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);

}

#endif
