/**************************************************************************//**
 * @file os_cfg.h
 * @brief uC/OS-III configuration file
 * @version 5.1.2
 *
 ******************************************************************************
 * @section License
 * <b>Copyright 2016 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#ifndef OS_CFG_H
#define OS_CFG_H
                                                           /* --------------------------- MISCELLANEOUS --------------------------- */
#define OS_CFG_APP_HOOKS_EN             DEF_ENABLED        /* Enable (DEF_ENABLED) application specific hooks                       */
#define OS_CFG_ARG_CHK_EN               DEF_ENABLED        /* Enable (DEF_ENABLED) argument checking                                */
#define OS_CFG_CALLED_FROM_ISR_CHK_EN   DEF_ENABLED        /* Enable (DEF_ENABLED) check for called from ISR                        */
#define OS_CFG_DBG_EN                   DEF_ENABLED        /* Enable (DEF_ENABLED) debug code/variables                             */
#define OS_CFG_DYN_TICK_EN              DEF_DISABLED       /* Enable (DEF_ENABLED) the Dynamic Tick                                 */
#define OS_CFG_INVALID_OS_CALLS_CHK_EN  DEF_DISABLED       /* Enable (DEF_ENABLED) checks for invalid kernel calls                  */
#define OS_CFG_OBJ_TYPE_CHK_EN          DEF_DISABLED       /* Enable (DEF_ENABLED) object type checking                             */
#define OS_CFG_TS_EN                    DEF_DISABLED       /* Enable (DEF_ENABLED) time stamping                                    */

#define OS_CFG_PRIO_MAX                 32u                /* Defines the maximum number of task priorities (see OS_PRIO data type) */

#define OS_CFG_SCHED_LOCK_TIME_MEAS_EN  DEF_DISABLED       /* Include (DEF_ENABLED) code to measure scheduler lock time             */
#define OS_CFG_SCHED_ROUND_ROBIN_EN     DEF_DISABLED       /* Include (DEF_ENABLED) code for Round-Robin scheduling                 */

#define OS_CFG_STK_SIZE_MIN             64u                /* Minimum allowable task stack size                                     */


                                                           /* --------------------------- EVENT FLAGS ----------------------------- */
#define OS_CFG_FLAG_EN                  DEF_ENABLED        /* Enable (DEF_ENABLED) code generation for EVENT FLAGS                  */
#define OS_CFG_FLAG_DEL_EN              DEF_DISABLED       /*     Include (DEF_ENABLED) code for OSFlagDel()                        */
#define OS_CFG_FLAG_MODE_CLR_EN         DEF_DISABLED       /*     Include (DEF_ENABLED) code for Wait on Clear EVENT FLAGS          */
#define OS_CFG_FLAG_PEND_ABORT_EN       DEF_DISABLED       /*     Include (DEF_ENABLED) code for OSFlagPendAbort()                  */


                                                           /* ------------------------ MEMORY MANAGEMENT -------------------------  */
#define OS_CFG_MEM_EN                   DEF_ENABLED        /* Enable (DEF_ENABLED) code generation for the MEMORY MANAGER           */


                                                           /* ------------------- MUTUAL EXCLUSION SEMAPHORES --------------------  */
#define OS_CFG_MUTEX_EN                 DEF_ENABLED        /* Enable (DEF_ENABLED) code generation for MUTEX                        */
#define OS_CFG_MUTEX_DEL_EN             DEF_DISABLED       /*     Include (DEF_ENABLED) code for OSMutexDel()                       */
#define OS_CFG_MUTEX_PEND_ABORT_EN      DEF_DISABLED       /*     Include (DEF_ENABLED) code for OSMutexPendAbort()                 */


                                                           /* -------------------------- MESSAGE QUEUES --------------------------  */
#define OS_CFG_Q_EN                     DEF_ENABLED        /* Enable (DEF_ENABLED) code generation for QUEUES                       */
#define OS_CFG_Q_DEL_EN                 DEF_DISABLED       /*     Include (DEF_ENABLED) code for OSQDel()                           */
#define OS_CFG_Q_FLUSH_EN               DEF_DISABLED       /*     Include (DEF_ENABLED) code for OSQFlush()                         */
#define OS_CFG_Q_PEND_ABORT_EN          DEF_ENABLED        /*     Include (DEF_ENABLED) code for OSQPendAbort()                     */


                                                           /* ---------------------------- SEMAPHORES ----------------------------- */
#define OS_CFG_SEM_EN                   DEF_ENABLED        /* Enable (DEF_ENABLED) code generation for SEMAPHORES                   */
#define OS_CFG_SEM_DEL_EN               DEF_DISABLED       /*     Include (DEF_ENABLED) code for OSSemDel()                         */
#define OS_CFG_SEM_PEND_ABORT_EN        DEF_ENABLED        /*     Include (DEF_ENABLED) code for OSSemPendAbort()                   */
#define OS_CFG_SEM_SET_EN               DEF_ENABLED        /*     Include (DEF_ENABLED) code for OSSemSet()                         */


                                                           /* ----------------------------- MONITORS ------------------------------ */
#define OS_CFG_MON_EN                   DEF_ENABLED        /* Enable (DEF_ENABLED) code generation for MONITORS                     */
#define OS_CFG_MON_DEL_EN               DEF_DISABLED       /*     Include (DEF_ENABLED) code for OSMonDel()                         */

                                                           /* -------------------------- TASK MANAGEMENT -------------------------- */
#define OS_CFG_STAT_TASK_EN             DEF_ENABLED        /* Enable (DEF_ENABLED) the statistics task                              */
#define OS_CFG_STAT_TASK_STK_CHK_EN     DEF_ENABLED        /*     Check task stacks (DEF_ENABLED) from the statistic task           */

#define OS_CFG_TASK_CHANGE_PRIO_EN      DEF_ENABLED        /* Include (DEF_ENABLED) code for OSTaskChangePrio()                     */
#define OS_CFG_TASK_DEL_EN              DEF_DISABLED       /* Include (DEF_ENABLED) code for OSTaskDel()                            */
#define OS_CFG_TASK_IDLE_EN             DEF_ENABLED        /* Include (DEF_ENABLED) the idle task                                   */
#define OS_CFG_TASK_PROFILE_EN          DEF_ENABLED        /* Include (DEF_ENABLED) variables in OS_TCB for profiling               */
#define OS_CFG_TASK_Q_EN                DEF_ENABLED        /* Include (DEF_ENABLED) code for OSTaskQXXXX()                          */
#define OS_CFG_TASK_Q_PEND_ABORT_EN     DEF_DISABLED       /* Include (DEF_ENABLED) code for OSTaskQPendAbort()                     */
#define OS_CFG_TASK_REG_TBL_SIZE        1u                 /* Number of task specific registers                                     */
#define OS_CFG_TASK_STK_REDZONE_EN      DEF_ENABLED        /* Enable (DEF_ENABLED) stack redzone                                    */
#define OS_CFG_TASK_STK_REDZONE_DEPTH   8u                 /*     Depth of the stack redzone                                        */
#define OS_CFG_TASK_SEM_PEND_ABORT_EN   DEF_ENABLED        /* Include (DEF_ENABLED) code for OSTaskSemPendAbort()                   */
#define OS_CFG_TASK_SUSPEND_EN          DEF_ENABLED        /* Include (DEF_ENABLED) code for OSTaskSuspend() and OSTaskResume()     */
#define OS_CFG_TASK_TICK_EN             DEF_ENABLED        /* Include (DEF_ENABLED) the kernel tick task                            */

                                                           /* ------------------ TASK LOCAL STORAGE MANAGEMENT -------------------  */
#define OS_CFG_TLS_TBL_SIZE             0u                 /* Include (DEF_ENABLED) code for Task Local Storage (TLS) registers     */

                                                           /* ------------------------- TIME MANAGEMENT --------------------------  */
#define OS_CFG_TIME_DLY_HMSM_EN         DEF_DISABLED       /* Include (DEF_ENABLED) code for OSTimeDlyHMSM()                        */
#define OS_CFG_TIME_DLY_RESUME_EN       DEF_DISABLED       /* Include (DEF_ENABLED) code for OSTimeDlyResume()                      */

                                                           /* ------------------------- TIMER MANAGEMENT -------------------------- */
#define OS_CFG_TMR_EN                   DEF_DISABLED       /* Enable (DEF_ENABLED) code generation for TIMERS                       */
#define OS_CFG_TMR_DEL_EN               DEF_DISABLED       /* Enable (DEF_ENABLED) code generation for OSTmrDel()                   */

                                                           /* ------------------------- TRACE RECORDER ---------------------------- */
#define OS_CFG_TRACE_EN                 DEF_DISABLED       /* Enable (DEF_ENABLED) uC/OS-III Trace instrumentation                  */
#define OS_CFG_TRACE_API_ENTER_EN       DEF_DISABLED       /* Enable (DEF_ENABLED) uC/OS-III Trace API enter instrumentation        */
#define OS_CFG_TRACE_API_EXIT_EN        DEF_DISABLED       /* Enable (DEF_ENABLED) uC/OS-III Trace API exit  instrumentation        */

#endif
