/**************************************************************************//**
 * @file task_cfg.h
 * @brief Task configuration file
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

#ifndef  TASK_CFG_H
#define  TASK_CFG_H

/* Start task configurations */
#define  TASK_CFG_START_PRIO                  3u
#define  TASK_CFG_START_STK_SIZE            256u

/* Toggle task configuration */
#define  TASK_CFG_TOGGLE_PRIO                 4u
#define  TASK_CFG_TOGGLE_STK_SIZE           100u

/* Display task configuration */
#define  TASK_CFG_DISPLAY_PRIO                6u
#define  TASK_CFG_DISPLAY_STK_SIZE          256u

/* Image task configuration */
#define  TASK_CFG_IMAGE_PRIO                  5u
#define  TASK_CFG_IMAGE_STK_SIZE            256u

/*
*********************************************************************************************************
*                                     TRACE / DEBUG CONFIGURATION
*********************************************************************************************************
*/
#ifndef  TRACE_LEVEL_OFF
#define  TRACE_LEVEL_OFF                        0u
#endif

#ifndef  TRACE_LEVEL_INFO
#define  TRACE_LEVEL_INFO                       1u
#endif

#ifndef  TRACE_LEVEL_DBG
#define  TRACE_LEVEL_DBG                        2u
#endif

#define  APP_TRACE_LEVEL                        TRACE_LEVEL_OFF
#define  APP_TRACE                              printf

#define  APP_TRACE_INFO(x)               ((APP_TRACE_LEVEL >= TRACE_LEVEL_INFO)  ? (void)(APP_TRACE x) : (void)0)
#define  APP_TRACE_DBG(x)                ((APP_TRACE_LEVEL >= TRACE_LEVEL_DBG)   ? (void)(APP_TRACE x) : (void)0)

#endif
