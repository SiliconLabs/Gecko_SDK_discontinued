/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                          (c) Copyright 2003-2010; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                         uC/OS-III example code
*                                          Application task one
*
*                                   Silicon Labs EFM32 (EFM32WG990F256)
*                                              with the
*                               Silicon Labs EFM32WG990F256-STK Starter Kit
*
* @file   app_task_one.c
* @brief
* @version 5.1.2
******************************************************************************
* @section License
* <b>Copyright 2015 Silicon Labs, Inc. http://www.silabs.com</b>
*******************************************************************************
*
* This file is licensed under the Silabs License Agreement. See the file
* "Silabs_License_Agreement.txt" for details. Before using this software for
* any purpose, you must agree to the terms of that agreement.
*
******************************************************************************/
#include <includes.h>


/*
*********************************************************************************************************
*                                         APP_TaskOne()
* @brief      The One task.
*
* @param[in]  p_arg       Argument passed to 'APP_TaskOne()' by 'OSTaskCreate()'.
* @exception  none
* @return     none.
*/
/* Notes      :(1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                   used.  The compiler should not generate any code for this statement.
*
*
*********************************************************************************************************
*/
void APP_TaskOne(void *p_arg)
{
  OS_ERR err = OS_ERR_NONE;
  static CPU_INT32U ledPos = 0U; /* LED position variable  */


  (void)p_arg; /* Note(1) */

  while (1U)
  { /* Task body, always written as an infinite loop  */

    /* Delay with 100msec                             */
    OSTimeDlyHMSM(0U, 0U, 0U, 100U, (OS_OPT_TIME_DLY | OS_OPT_TIME_HMSM_STRICT), &err);

    /* Animate LEDs */
    BSP_LedToggle(ledPos);

    /* Modify LED position variable                   */
    if (4u == ++ledPos)
    {
      ledPos = 0; /* 3bit overflow */
    }

    /* Delay task for 1 system tick (uC/OS-III suspends this task and executes
     * the next most important task) */
    OSTimeDly(1U, OS_OPT_TIME_DLY, &err);
  }
}
