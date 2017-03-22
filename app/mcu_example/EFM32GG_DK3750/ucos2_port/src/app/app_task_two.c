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
*                                         uC/OS-II example code
*                                          Application task two
*
*                                   Silicon Labs EFM32 (EFM32GG990F1024)
*                                              with the
*                                   Silicon Labs DK3750 Development Kit
*
* @file   app_task_two.c
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
*                                         APP_TaskTwo()
* @brief      The Two task.
*
* @param[in]  p_arg       Argument passed to 'APP_TaskTwo()' by 'OSTaskCreate()'.
* @exception  none
* @return     none.
*/
/* Notes      :(1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                   used.  The compiler should not generate any code for this statement.
*
*
*********************************************************************************************************
*/
void APP_TaskTwo(void *p_arg)
{
  int taskChar;                   // Character received.

  (void)p_arg;  /* Note(1) */

  while (1)
  { /* Task body, always written as an infinite loop  */

    /* Load character received on serial to character buffer */
    taskChar = UART1_ReadChar();

    /* If the character in the buffer is valid... */
    if ( (taskChar >= 32) && (taskChar < 128) )
    {
      /* Post the message to the mailbox */
      if (OS_ERR_NONE != OSQPost(pSerialQueObj, (void*)taskChar))
      {
        /* Error can be handled here */
      }
    }

    /* Delay task for 1 system tick (uC/OS-II suspends this task and executes
     * the next most important task) */
    OSTimeDly(1);
  }
}
