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
*                                          Application task three
*
*                                   Silicon Labs EFM32 (EFM32GG990F1024)
*                                              with the
*                                   Silicon Labs DK3750 Development Kit
*
* @file   app_task_three.c
* @brief
* @version 4.3.0
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
*                                             DEFINITIONS
*********************************************************************************************************
*/
/* Width of LCD screen on plugin board */
#define APPDEF_LCD_TXT_SIZE  7


/*
*********************************************************************************************************
*                                         APP_TaskThree()
* @brief      The Three task.
*
* @param[in]  p_arg       Argument passed to 'APP_TaskThree()' by 'OSTaskCreate()'.
* @exception  none
* @return     none.
*/
/* Notes      :(1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                   used.  The compiler should not generate any code for this statement.
*
*
*********************************************************************************************************
*/
void APP_TaskThree(void *p_arg)
{
  static char taskStringBuffer[APPDEF_LCD_TXT_SIZE+1U] = {'u','C','/','O','S','-','2','\0'};
  char indxChar;
  int  msgContent;

  (void)p_arg; /* Note(1) */

  while (1)
  { /* Task body, always written as an infinite loop  */

    msgContent = (int)OSMboxAccept(pSerialMsgObj);
    if ((void*)0 != (void*)msgContent)
    {
      /* Shift left the whole string by one... */
      for (indxChar = 0; indxChar < APPDEF_LCD_TXT_SIZE; indxChar++)
      {
        taskStringBuffer[indxChar] = taskStringBuffer[indxChar+1];
      }

      /* ...and concatenate the new character to the end */
      taskStringBuffer[APPDEF_LCD_TXT_SIZE-1] = (char)msgContent;

      /* Write the string on display */
      printf("\rBuffer: %s", taskStringBuffer);

    }

    OSTimeDlyHMSM(0, 0, 0, 10);

    /* Delay task for 1 system tick (uC/OS-II suspends this task and executes
     * the next most important task) */
    OSTimeDly(1);
  }
}
