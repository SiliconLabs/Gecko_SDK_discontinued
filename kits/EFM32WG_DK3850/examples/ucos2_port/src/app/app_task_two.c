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
*                                   Silicon Labs EFM32 (EFM32WG990F256)
*                                              with the
*                                   Silicon Labs DK3850 Development Kit
*
* @file   app_task_two.c
* @brief
* @version 4.1.0
******************************************************************************
* @section License
* <b>(C) Copyright 2013 Energy Micro AS, http://www.energymicro.com</b>
*******************************************************************************
*
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
*
* 1. The origin of this software must not be misrepresented; you must not
*    claim that you wrote the original software.
* 2. Altered source versions must be plainly marked as such, and must not be
*    misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
* 4. The source and compiled code may only be used on Energy Micro "EFM32"
*    microcontrollers and "EFR4" radios.
*
* DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Energy Micro AS has no
* obligation to support this Software. Energy Micro AS is providing the
* Software "AS IS", with no express or implied warranties of any kind,
* including, but not limited to, any implied warranties of merchantability
* or fitness for any particular purpose or warranties against infringement
* of any proprietary rights of a third party.
*
* Energy Micro AS will not be liable for any consequential, incidental, or
* special damages, or any other relief, or for any claim by any third party,
* arising from your use of this Software.
*
*********************************************************************************************************
*/
#include <includes.h>

#define CHAR_BUFFER_SIZE 10
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
  static char taskMsg[CHAR_BUFFER_SIZE]; /* buffer for received characters to be passed to TaskThree */
  signed int  taskCharBuffer = -1; /* Character buffer for receiving */
  unsigned char bufIndex = 0;

  (void)p_arg;  /* Note(1) */

  while (1)
  { /* Task body, always written as an infinite loop  */

    /* Load character received on serial to character buffer */
    taskCharBuffer = UART1_ReadChar();

    /* If the character in the buffer is valid... */
    if ( (taskCharBuffer >= 32) && (taskCharBuffer < 128) )
    {
      /* ...cast and copy it to message buffer variable... */
      taskMsg[bufIndex] = (char)taskCharBuffer;

      /* ...and post the message to the mailbox */
      if(OS_ERR_NONE != OSMboxPost(pSerialMsgObj, &taskMsg[bufIndex]))
      {
        /* Error has occured, handle can be done here */
      }

      bufIndex++;
      if(bufIndex >= CHAR_BUFFER_SIZE)
        bufIndex = 0;
    }

    /* Delay task for 1 system tick (uC/OS-II suspends this task and executes
     * the next most important task) */
    OSTimeDly(1);
  }
}
