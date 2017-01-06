/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2016  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.34 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with the following terms:

The  software has  been licensed  to Silicon Labs Norway, a subsidiary
of Silicon Labs Inc. whose registered office is 400 West Cesar Chavez,
Austin,  TX 78701, USA solely for  the purposes of creating  libraries 
for its  ARM Cortex-M3, M4F  processor-based devices,  sublicensed and 
distributed  under the  terms and conditions  of the  End User License  
Agreement supplied by Silicon Labs.
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
Licensing information

Licensor:                 SEGGER Software GmbH
Licensed to:              Silicon Laboratories Norway
Licensed SEGGER software: emWin
License number:           GUI-00140
License model:            See Agreement, dated 20th April 2012
Licensed product:         - 
Licensed platform:        Cortex M3, Cortex M4F
Licensed number of seats: -
----------------------------------------------------------------------
File        : MT_MultiTasking.c
Purpose     : Example demonstrates MultiTasking capabilities of emWin
Requirements: WindowManager - (x)
              MemoryDevices - (x)
              AntiAliasing  - ( )
              VNC-Server    - ( )
              PNG-Library   - ( )
              TrueTypeFonts - ( )

              Requires either a MS Windows environment or embOS.
              If not using embOS you have to change the hardware
              dependent macros to work with your OS.
----------------------------------------------------------------------
*/

#ifndef SKIP_TEST
#include <stddef.h>
#include "GUI.h"
#include "FRAMEWIN.h"

#if GUI_OS == 0
  #error Multitasking sample requires task awareness (#define GUI_OS 1)
#endif

/*******************************************************************
*
*       Define how to create a task and start multitasking
*
********************************************************************
*/
#ifndef WIN32 
  #include "RTOS.h"    /* Definitions for embOS */
  #define CREATE_TASK(pTCB, pName, pFunc, Priority, pStack)  OS_CREATETASK(pTCB, pName, pFunc, Priority, pStack)
  #define START_MT()  OS_Terminate(0)
  #define Delay(t)    OS_Delay(t)
#else
  #include "SIM.h"     /* Definitions for the Win32 simulation */
  #define CREATE_TASK(pTCB, pName, pFunc, Priority, pStack)   SIM_CreateTask(pName, pFunc)
  #define START_MT()  SIM_Start()
  #define Delay(t)    SIM_Delay(t)
#endif

//
// Recommended memory to run the sample with adequate performance
//
#define RECOMMENDED_MEMORY (1024L * 20)

/*******************************************************************
*
*       Static data
*
********************************************************************
*/
#ifndef WIN32
  //
  // Stacks
  //
  static OS_STACKPTR int Stack_0[600];
  static OS_STACKPTR int Stack_1[600];
  static OS_STACKPTR int Stack_2[600];
  static OS_TASK aTCB[3];               // Task control blocks
#endif

/*******************************************************************
*
*       Static code
*
********************************************************************
*/
/*******************************************************************
*
*       _cbCallbackT0
*/
static int XPos;
static const char aText[] = "Moving text...";

static void _cbCallbackT0(WM_MESSAGE * pMsg) {
  switch (pMsg->MsgId) {
  case WM_PAINT:
    //
    // Handle the paint message
    //
    GUI_SetBkColor(0x4040FF);
    GUI_SetColor(GUI_BLACK);
    GUI_SetFont(&GUI_FontComic24B_ASCII);
    GUI_Clear();
    GUI_DispStringAt(aText, XPos, 0);
    break;
  default:
    WM_DefaultProc(pMsg);
  }
}

/*******************************************************************
*
*       _cbCallbackT1
*/
static void _cbCallbackT1(WM_MESSAGE * pMsg) {
  WM_HWIN hWin = (FRAMEWIN_Handle)(pMsg->hWin);
  switch (pMsg->MsgId) {
  case WM_PAINT:
    //
    // Handle the paint message
    //
    GUI_SetBkColor(0xA02020);
    GUI_SetColor(GUI_WHITE);
    GUI_SetFont(&GUI_FontComic24B_ASCII);
    GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_VCENTER);
    GUI_Clear();
    GUI_DispStringHCenterAt("Moving window...", 
                            WM_GetWindowSizeX(hWin) / 2, 
                            WM_GetWindowSizeY(hWin) / 2);
    break;
  default:
    WM_DefaultProc(pMsg);
  }
}

/*******************************************************************
*
*       _cbBackgroundWin
*/
static void _cbBackgroundWin(WM_MESSAGE* pMsg) {
  switch (pMsg->MsgId) {
  case WM_PAINT:
    //
    // Handle only the paint message
    //
    GUI_SetBkColor(0x00CC00);
    GUI_Clear();
    GUI_SetFont(&GUI_Font24_ASCII);
    GUI_DispStringHCenterAt("emWin - multitasking demo\n", 160, 5);
    GUI_SetFont(&GUI_Font13_1);
    GUI_DispStringAt("Scrolling text and moving windows without flickering", 5, 35);
  default:
    WM_DefaultProc(pMsg);
  }
}

/*******************************************************************
*
*       _Task_0
*/
static void _Task_0(void) {
  FRAMEWIN_Handle hFrameWin;
  WM_HWIN         hChildWin;
  //
  // Create frame window
  //
  hFrameWin = FRAMEWIN_Create("Task 0",  NULL, WM_CF_SHOW | WM_CF_STAYONTOP,
                              0, 70, 200, 50);
  //
  // Create child window
  //
  hChildWin = WM_CreateWindowAsChild(0, 0, 0, 0, WM_GetClientWindow(hFrameWin),
                                     WM_CF_SHOW | WM_CF_MEMDEV,
                                     _cbCallbackT0, 0);
  FRAMEWIN_SetActive(hFrameWin, 0);
  //
  // Make sure the right window is active...
  //
  WM_SelectWindow(hChildWin);
  //
  // ...and the right font is selected
  //
  GUI_SetFont(&GUI_FontComic24B_ASCII);
  while(1) {
    GUI_RECT Rect;
    int XLen;
    
    XLen = GUI_GetStringDistX(aText);       // Get the length of the string
    WM_GetClientRect(&Rect);                // Get the size of the window
    //
    // Show moving text
    //
    for (XPos = 0; XPos < (Rect.x1 - Rect.x0) - XLen; XPos++) {
      WM_InvalidateWindow(hChildWin);
      Delay(50);
    }
    for (; XPos >= 0; XPos--) {
      WM_InvalidateWindow(hChildWin);
      Delay(100);
    }
  }
}

/*******************************************************************
*
*       _Task_1
*/
static void _Task_1(void) {
  FRAMEWIN_Handle hFrameWin;
  WM_HWIN         hChildWin;

  //
  // Create frame window
  //
  hFrameWin = FRAMEWIN_Create("Task 1", NULL, WM_CF_SHOW | WM_CF_STAYONTOP,
                              20, 170, 200, 50);
  //
  // Create child window
  //
  hChildWin = WM_CreateWindowAsChild(0, 0, 0, 0, WM_GetClientWindow(hFrameWin),
                                     WM_CF_SHOW | WM_CF_MEMDEV,
                                     _cbCallbackT1, 0);
  FRAMEWIN_SetActive(hFrameWin, 0);
  while(1) {
    int i;
    int nx = 80;
    int ny = 90;
    //
    // Move window continously
    //
    for (i = 0; i < ny; i++) {
      WM_MoveWindow(hFrameWin, 0, -2);
      Delay(50);
    }
    for (i = 0; i < nx; i++) {
      WM_MoveWindow(hFrameWin, 2, 0);
      Delay(50);
    }
    for (i = 0; i < ny; i++) {
      WM_MoveWindow(hFrameWin, 0, 2);
      Delay(50);
    }
    for (i = 0; i < nx; i++) {
      WM_MoveWindow(hFrameWin, -2, 0);
      Delay(50);
    }
  }
}

/*******************************************************************
*
*       _GUI_Task
*
* This task does the background processing.
* The MainTask job is to update invalid windows, but other things such as
* evaluating mouse or touch input may also be done.
*/
static void _GUI_Task(void) {
  while(1) {
    GUI_Exec();           // Do the background work ... Update windows etc.)
    GUI_X_ExecIdle();     // Nothing left to do for the moment ... Idle processing
  }
}


/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       MainTask
*/
void MainTask(void) {
  //
  // Init GUI
  //
  WM_SetCreateFlags(WM_CF_MEMDEV);              // Use memory devices on all windows to avoid flicker
  GUI_Init();
  //
  // Check if recommended memory for the sample is available
  //
  if (GUI_ALLOC_GetNumFreeBytes() < RECOMMENDED_MEMORY) {
    GUI_ErrorOut("Not enough memory available."); 
    return;
  }
  WM_SetCallback(WM_HBKWIN, _cbBackgroundWin);  // Set callback for background window
  //
  // Create tasks
  //
  CREATE_TASK(&aTCB[0], "Task_0",   _Task_0,   80, Stack_0);
  CREATE_TASK(&aTCB[1], "Task_1",   _Task_1,   60, Stack_1);
  CREATE_TASK(&aTCB[2], "GUI_TASK", _GUI_Task,  1, Stack_2);
  //
  // Start multitasking
  //
  START_MT();
}

#endif

/*************************** End of file ****************************/
