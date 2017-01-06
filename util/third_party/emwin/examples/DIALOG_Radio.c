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
File        : DIALOG_Radio.c
Purpose     : Example demonstrating the use of a RADIO widget
Requirements: WindowManager - (x)
              MemoryDevices - (x)
              AntiAliasing  - ( )
              VNC-Server    - ( )
              PNG-Library   - ( )
              TrueTypeFonts - ( )
----------------------------------------------------------------------
*/

#include <stddef.h>
#include "GUI.h"
#include "DIALOG.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
//
// Recommended memory to run the sample with adequate performance
//
#define RECOMMENDED_MEMORY (1024L * 10)

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
/*********************************************************************
*
*       Dialog resource
*
* Function description
*   This table conatins the info required to create the dialog.
*   It has been created manually, but could also be created by a GUI-builder.
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { FRAMEWIN_CreateIndirect, "Radio button sample", 0,        30,  70, 260, 100, FRAMEWIN_CF_MOVEABLE },
  { RADIO_CreateIndirect,     NULL,           GUI_ID_RADIO0,   5,  10,   0,   0,   0,  3 },
  { TEXT_CreateIndirect,     "Suspend",       GUI_ID_TEXT0,   25,  10,  70,  20, TEXT_CF_LEFT },
  { TEXT_CreateIndirect,     "Shut down",     GUI_ID_TEXT1,   25,  30,  70,  20, TEXT_CF_LEFT },
  { TEXT_CreateIndirect,     "Restart after", GUI_ID_TEXT2,   25,  50,  70,  20, TEXT_CF_LEFT },
  { TEXT_CreateIndirect,     "seconds",       GUI_ID_TEXT3,  130,  50,  70,  20, TEXT_CF_LEFT },
  { EDIT_CreateIndirect,     "200",           GUI_ID_EDIT0,   95,  47,  30,  19, 0, 3},
  { BUTTON_CreateIndirect,   "OK",            GUI_ID_OK,     180,  10,  60,  20 },
  { BUTTON_CreateIndirect,   "Cancel",        GUI_ID_CANCEL, 180,  40,  60,  20 }
};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _cbBkWindow
*/
static void _cbBkWindow(WM_MESSAGE* pMsg) {
  switch (pMsg->MsgId) {
  case WM_PAINT:
    GUI_SetBkColor(GUI_RED);
    GUI_Clear();
    GUI_SetColor(GUI_WHITE);
    GUI_SetFont(&GUI_Font24_ASCII);
    GUI_DispStringHCenterAt("DIALOG_Radio - Sample", 160, 5);
    break;
  default:
    WM_DefaultProc(pMsg);
  }
}

/*********************************************************************
*
*       _cbCallback
*/
static void _cbCallback(WM_MESSAGE * pMsg) {
  WM_HWIN hDlg;
  WM_HWIN hItem;
  int     Sel;
  int     NCode;
  int     Id;

  GUI_PID_STATE * pState;
  hDlg = pMsg->hWin;
  switch (pMsg->MsgId) {
    case WM_INIT_DIALOG:
      hItem = WM_GetDialogItem(hDlg, GUI_ID_EDIT0);
      EDIT_SetDecMode(hItem, 30,   0, 999, 0, 0);    // Select decimal mode
      WM_DisableWindow(hItem);
      break;
    case WM_KEY:
      switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) {
        case GUI_KEY_ESCAPE:
          GUI_EndDialog(hDlg, 1);
          break;
        case GUI_KEY_ENTER:
          GUI_EndDialog(hDlg, 0);
          break;
      }
      break;
    case WM_TOUCH_CHILD:
      Id = WM_GetId(pMsg->hWinSrc);      // Id of widget
      switch (Id) {
        case GUI_ID_TEXT0:
        case GUI_ID_TEXT1:
        case GUI_ID_TEXT2:
          pState = (GUI_PID_STATE *)((WM_MESSAGE *)pMsg->Data.p)->Data.p;
          if (pState) {
            if (pState->Pressed) {
              WM_HWIN hRadio = WM_GetDialogItem(hDlg, GUI_ID_RADIO0);
              RADIO_SetValue(hRadio, Id - GUI_ID_TEXT0);    // Use the text beside the radio button to 
                                                            // set the value of the radio button
            }
          }
          break;
      }
      break;
    case WM_NOTIFY_PARENT:
      Id    = WM_GetId(pMsg->hWinSrc);      // Id of widget
      NCode = pMsg->Data.v;                 // Notification code
      switch (NCode) {
        case WM_NOTIFICATION_RELEASED:      // React only if released
          switch (Id) {
            case GUI_ID_OK:
              GUI_EndDialog(hDlg, 0);
              break;
            case GUI_ID_CANCEL:
              GUI_EndDialog(hDlg, 1);
              break;
          }
          break;
        case WM_NOTIFICATION_VALUE_CHANGED:
          hItem = WM_GetDialogItem(hDlg, GUI_ID_RADIO0);
          Sel   = RADIO_GetValue(hItem);
          hItem = WM_GetDialogItem(hDlg, GUI_ID_EDIT0);
          WM_SetEnableState(hItem, Sel == 2);      
          break;
      }
      break;
    default:
      WM_DefaultProc(pMsg);
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
  WM_SetCreateFlags(WM_CF_MEMDEV);  // Use memory devices on all windows to avoid flicker
  GUI_Init();
  //
  // Check if recommended memory for the sample is available
  //
  if (GUI_ALLOC_GetNumFreeBytes() < RECOMMENDED_MEMORY) {
    GUI_ErrorOut("Not enough memory available."); 
    return;
  }
  WM_SetCallback(WM_HBKWIN, &_cbBkWindow);
  while (1) {
    GUI_ExecDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), &_cbCallback, 0, 0, 0);
    GUI_Delay(1000);
  }
}

/*************************** End of file ****************************/

