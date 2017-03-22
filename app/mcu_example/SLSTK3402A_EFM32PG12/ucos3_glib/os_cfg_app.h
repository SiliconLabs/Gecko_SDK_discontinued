/**************************************************************************//**
 * @file main.c
 * @brief uC/OS-III Application Configuration file
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

#ifndef OS_CFG_APP_H
#define OS_CFG_APP_H

/*
************************************************************************************************************************
*                                                      CONSTANTS
************************************************************************************************************************
*/
                                                                /* ------------------ MISCELLANEOUS ------------------- */
#define  OS_CFG_ISR_STK_SIZE                         256u       /* Stack size of ISR stack (number of CPU_STK elements) */

#define  OS_CFG_MSG_POOL_SIZE                         32u       /* Maximum number of messages                           */

#define  OS_CFG_TASK_STK_LIMIT_PCT_EMPTY              10u       /* Stack limit position in percentage to empty          */


                                                                /* -------------------- IDLE TASK --------------------- */
#define  OS_CFG_IDLE_TASK_STK_SIZE                   128u       /* Stack size (number of CPU_STK elements)              */


                                                                /* ----------------- ISR HANDLER TASK ----------------- */
#define  OS_CFG_INT_Q_SIZE                            10u       /* Size of ISR handler task queue                       */
#define  OS_CFG_INT_Q_TASK_STK_SIZE                  100u       /* Stack size (number of CPU_STK elements)              */


                                                                /* ------------------ STATISTIC TASK ------------------ */
#define  OS_CFG_STAT_TASK_PRIO       (OS_CFG_PRIO_MAX-2u)       /* Priority                                             */
#define  OS_CFG_STAT_TASK_RATE_HZ                     10u       /* Rate of execution (1 to 10 Hz)                       */
#define  OS_CFG_STAT_TASK_STK_SIZE                   128u       /* Stack size (number of CPU_STK elements)              */


                                                                /* ---------------------- TICKS ----------------------- */
#define  OS_CFG_TICK_RATE_HZ                        1000u       /* Tick rate in Hertz (10 to 1000 Hz)                   */
#define  OS_CFG_TICK_TASK_PRIO                         1u       /* Priority                                             */
#define  OS_CFG_TICK_TASK_STK_SIZE                   128u       /* Stack size (number of CPU_STK elements)              */

                                                                /* --------------------- TIMERS ----------------------- */
#define  OS_CFG_TMR_TASK_PRIO        (OS_CFG_PRIO_MAX-3u)       /* Priority of 'Timer Task'                             */
#define  OS_CFG_TMR_TASK_RATE_HZ                      10u       /* Rate for timers (10 Hz Typ.)                         */
#define  OS_CFG_TMR_TASK_STK_SIZE                    128u       /* Stack size (number of CPU_STK elements)              */

#endif
