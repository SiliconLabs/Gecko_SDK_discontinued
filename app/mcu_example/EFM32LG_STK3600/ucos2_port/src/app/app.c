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
*                                       Main application function
*
*                                   Silicon Labs EFM32 (EFM32LG990F256)
*                                              with the
*                               Silicon Labs EFM32LG990F256-STK Starter Kit
*
* @file   app.c
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

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
#include <includes.h>

/*
*********************************************************************************************************
*                                           MACRO DEFINITIONS
*********************************************************************************************************
*/
/* Message queue size */
#define MSG_Q_SIZE  10U

/*
*********************************************************************************************************
*                                         LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/
static OS_STK App_TaskStartStk[APP_CFG_TASK_START_STK_SIZE];
static OS_STK App_TaskOneStk[APP_CFG_TASK_ONE_STK_SIZE];
static OS_STK App_TaskTwoStk[APP_CFG_TASK_TWO_STK_SIZE];
static OS_STK App_TaskThreeStk[APP_CFG_TASK_THREE_STK_SIZE];

/* Message queue */
static void * AppMsgQueue[MSG_Q_SIZE];

/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/
static void App_TaskStart(void *p_arg);
static void App_TaskCreate(void);
static void App_MailboxCreate(void);

/*
*********************************************************************************************************
*                                      EXTERNAL GLOBAL VARIABLES
*********************************************************************************************************
*/
/* Definition of global mailbox object for inter-task communication
 * extern declaration in includes.h */
OS_EVENT * pSerialQueObj;


/*
*********************************************************************************************************
*                                         FUNCTION DEFINITIONS
*********************************************************************************************************
*/
/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Argument(s) : none.
*
* Return(s)   : none.
*********************************************************************************************************
*/
int main(void)
{
#if (OS_TASK_NAME_EN > 0)
  CPU_INT08U  err;
#endif

  /* Disable all interrupts until we are ready to accept
   * them.                                                */
  CPU_IntDis();

  /* Chip errata */
  CHIP_Init();

  /* setup SW0 for energyAware Profiler */
  BSP_TraceSwoSetup();

  /* Initialize serial port                               */
  RETARGET_SerialInit();
  RETARGET_SerialCrLf(1);

  /* Initialize "uC/OS-II, The Real-Time Kernel".         */
  OSInit();

  /* Create the start task                                */
  OSTaskCreateExt((void (*)(void *)) App_TaskStart,
                  (void           *) 0,
                  (OS_STK         *)&App_TaskStartStk[APP_CFG_TASK_START_STK_SIZE - 1],
                  (INT8U           ) APP_CFG_TASK_START_PRIO,
                  (INT16U          ) APP_CFG_TASK_START_PRIO,
                  (OS_STK         *)&App_TaskStartStk[0],
                  (INT32U          ) APP_CFG_TASK_START_STK_SIZE,
                  (void           *) 0,
                  (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));

#if (OS_TASK_NAME_EN > 0)
  OSTaskNameSet(APP_CFG_TASK_START_PRIO, (INT8U *)"Start", &err);
#endif

  /* Start multitasking (i.e. give control to uC/OS-II).  */
  OSStart();

  /* OSStart() never returns, serious error had occured if
   * code execution reached this point                    */
  while(1) ;
}


/*
*********************************************************************************************************
*                                          AppTaskStart()
*
* Description : The startup task.  The uC/OS-II ticker should only be initialize once multitasking starts.
*
* Argument(s) : p_arg       Argument passed to 'AppTaskStart()' by 'OSTaskCreate()'.
*
* Return(s)   : none.
*
* Note(s)     : (1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                   used.  The compiler should not generate any code for this statement.
*
*               (2) Interrupts are enabled once the task starts because the I-bit of the CCR register was
*                   set to 0 by 'OSTaskCreate()'.
*********************************************************************************************************
*/
static void App_TaskStart(void *p_arg)
{
  (void)p_arg; /* Note(1) */
  uint16_t osVersion1, osVersion2, osVersion3;

  /* Initialize BSP functions                             */
  BSPOS_Init();

  /* Initialize the uC/OS-II ticker                       */
  OS_CPU_SysTickInit(CMU_ClockFreqGet(cmuClock_HFPER)/OS_TICKS_PER_SEC);

#if (OS_TASK_STAT_EN > 0)
  /* Determine CPU capacity                               */
  OSStatInit();
#endif

  /* Create application mailboxes                         */
  App_MailboxCreate();

  /* Initialize LCD                                       */
  SegmentLCD_Init(true);

  /* Turn gecko symbol ON                                 */
  SegmentLCD_Symbol(LCD_SYMBOL_GECKO, 1);

  /* Turn EFM32 symbol ON                                 */
  SegmentLCD_Symbol(LCD_SYMBOL_EFM32, 1);

  /* Write welcome message on LCD                         */
  SegmentLCD_Write("uC/OS-2");

  osVersion3 = OSVersion();
  osVersion1 = osVersion3 / 10000;
  osVersion3 -= osVersion1 * 10000;
  osVersion2 = osVersion3 / 100;
  osVersion3 -= osVersion2 * 100;
  osVersion3 %= 100;

  /* Write welcome message on serial              */
  printf("\n*****************************************************************************");
  printf("\n                uC/OS-II v%d.%02d.%02d on Silicon Labs EFM32LG STK             ",
         osVersion1, osVersion2, osVersion3 );
  printf("\n                               Demo Application                              \n");
  printf("\n                                   uC/OS-II                                  ");
  printf("\n                           \"The real time kernel\"                            ");
  printf("\n                               www.micrium.com                               ");
  printf("\n\n                                is running on                              ");
  printf("\n\n                             Silicon Labs EFM32                              ");
  printf("\n            \"The world's most energy friendly microcontrollers\"              ");
  printf("\n                              www.silabs.com                                 \n");
  printf("\nDescription:");
  printf("\nTask1: LED blink task");
  printf("\nTask2: Receives characters from serial and posts message to Task3");
  printf("\nTask3: Receives message from Task2 and writes it on LCD and serial");
  printf("\n*****************************************************************************\n");
  printf("\nStart typing...\n");

  /* Create application tasks                             */
  App_TaskCreate();

  /* Suspend this task as it is only used once in one Reset cycle */
  OSTaskSuspend(APP_CFG_TASK_START_PRIO);

  while (1)
  {/* endless loop of Start task                          */
    ;
  }
}

/*
*********************************************************************************************************
*                                      App_MailboxCreate()
*
* Description : Create the application Mailboxes
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Note(s)     : none.
*********************************************************************************************************
*/
static void App_MailboxCreate(void)
{
  /* Create message queue for messaging received serial data between tasks */
  pSerialQueObj = OSQCreate(AppMsgQueue, MSG_Q_SIZE);

  /* Error check */
  if (pSerialQueObj == (OS_EVENT *)0)
  {
    /* Error during message queue initialization */
    while (1U);
  }
}


/*
*********************************************************************************************************
*                                      App_TaskCreate()
*
* Description : Create the application tasks.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Note(s)     : none.
*********************************************************************************************************
*/
static void App_TaskCreate(void)
{
#if (OS_TASK_NAME_EN > 0)
  CPU_INT08U  err;
#endif

  /* Create the One task                         */
  OSTaskCreateExt((void (*)(void *)) APP_TaskOne,
                  (void           *) 0,
                  (OS_STK         *)&App_TaskOneStk[APP_CFG_TASK_ONE_STK_SIZE - 1],
                  (INT8U           ) APP_CFG_TASK_ONE_PRIO,
                  (INT16U          ) APP_CFG_TASK_ONE_PRIO,
                  (OS_STK         *)&App_TaskOneStk[0],
                  (INT32U          ) APP_CFG_TASK_ONE_STK_SIZE,
                  (void           *) 0,
                  (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));

  /* Create the Two task                         */
  OSTaskCreateExt((void (*)(void *)) APP_TaskTwo,
                  (void           *) 0,
                  (OS_STK         *)&App_TaskTwoStk[APP_CFG_TASK_TWO_STK_SIZE - 1],
                  (INT8U           ) APP_CFG_TASK_TWO_PRIO,
                  (INT16U          ) APP_CFG_TASK_TWO_PRIO,
                  (OS_STK         *)&App_TaskTwoStk[0],
                  (INT32U          ) APP_CFG_TASK_TWO_STK_SIZE,
                  (void           *) 0,
                  (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));

  /* Create the Three task                     */
  OSTaskCreateExt((void (*)(void *)) APP_TaskThree,
                  (void           *) 0,
                  (OS_STK         *)&App_TaskThreeStk[APP_CFG_TASK_THREE_STK_SIZE - 1],
                  (INT8U           ) APP_CFG_TASK_THREE_PRIO,
                  (INT16U          ) APP_CFG_TASK_THREE_PRIO,
                  (OS_STK         *)&App_TaskThreeStk[0],
                  (INT32U          ) APP_CFG_TASK_THREE_STK_SIZE,
                  (void           *) 0,
                  (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));

#if (OS_TASK_NAME_EN > 0)
  OSTaskNameSet(APP_CFG_TASK_ONE_PRIO, (INT8U *)"One", &err);
  OSTaskNameSet(APP_CFG_TASK_TWO_PRIO, (INT8U *)"Two", &err);
  OSTaskNameSet(APP_CFG_TASK_TWO_PRIO, (INT8U *)"Three", &err);
#endif
}
