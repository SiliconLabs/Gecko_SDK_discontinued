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
File        : DIALOG_Count.c
Purpose     : Shows a dialog which is continously counting
Requirements: WindowManager - (x)
              MemoryDevices - (x)
              AntiAliasing  - ( )
              VNC-Server    - ( )
              PNG-Library   - ( )
              TrueTypeFonts - ( )
---------------------------END-OF-HEADER------------------------------
*/

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
#define RECOMMENDED_MEMORY (1024L * 25)

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { FRAMEWIN_CreateIndirect, "Counting...",     0,      30,  90, 260, 145, FRAMEWIN_CF_MOVEABLE },
  { TEXT_CreateIndirect,     "00",     GUI_ID_TEXT0,    10,  10, 120,  80 },
  { RADIO_CreateIndirect,    "",       GUI_ID_RADIO0,  150,  10, 100,  80, 0, 4 },
  { BUTTON_CreateIndirect,   "OK",     GUI_ID_OK,       10, 100,  60,  18 },
  { BUTTON_CreateIndirect,   "Cancel", GUI_ID_CANCEL,  180, 100,  60,  18 },
};

static const char * _apLabel[] = {
  "GUI_FontFD32",
  "GUI_FontFD48",
  "GUI_FontFD64",
  "GUI_FontFD80",
};

static const GUI_FONT * _apFont[] = {
  &GUI_FontD32,
  &GUI_FontD48,
  &GUI_FontD64,
  &GUI_FontD80
};

static const char * _asExplain[] = {
  "Please use the RADIO buttons to select",
  "the big digit font used for counting."
};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _SetFont
*/
static void _SetFont(WM_HWIN hDlg) {
  WM_HWIN hItem;
  int Index;
  hItem = WM_GetDialogItem(hDlg, GUI_ID_RADIO0);
  Index = RADIO_GetValue(hItem);
  hItem = WM_GetDialogItem(hDlg, GUI_ID_TEXT0);
  TEXT_SetFont(hItem, _apFont[Index]);
}

/*********************************************************************
*
*       _cbBkWindow
*/
static void _cbBkWindow(WM_MESSAGE * pMsg) {
  unsigned i;
  switch (pMsg->MsgId) {
  case WM_PAINT:
    GUI_SetBkColor(GUI_BLUE);
    GUI_Clear();
    GUI_SetColor(GUI_WHITE);
    GUI_SetFont(&GUI_Font24_ASCII);
    GUI_DispStringHCenterAt("Counting Sample", 160, 5);
    GUI_SetFont(&GUI_Font8x16);
    for (i = 0; i < GUI_COUNTOF(_asExplain); i++) {
      GUI_DispStringAt(_asExplain[i], 5, 40 + i * 16);
    }
  default:
    WM_DefaultProc(pMsg);
  }
}

/*********************************************************************
*
*       _cbCallback
*/
static void _cbCallback(WM_MESSAGE * pMsg) {
  unsigned i;
  int NCode, Id;
  WM_HWIN hDlg, hItem;
  hDlg = pMsg->hWin;
  switch (pMsg->MsgId) {
    case WM_INIT_DIALOG:
      hItem = WM_GetDialogItem(hDlg, GUI_ID_RADIO0);
      for (i = 0; i < GUI_COUNTOF(_apLabel); i++) {
        RADIO_SetText(hItem, _apLabel[i], i);
      }
      _SetFont(hDlg);
      break;
    case WM_NOTIFY_PARENT:
      Id    = WM_GetId(pMsg->hWinSrc);    /* Id of widget */
      NCode = pMsg->Data.v;               /* Notification code */
      switch (NCode) {
      case WM_NOTIFICATION_VALUE_CHANGED: /* Value has changed */
        _SetFont(hDlg);
        break;
      case WM_NOTIFICATION_RELEASED:      /* React only if released */
        if (Id == GUI_ID_OK) {            /* OK Button */
          GUI_EndDialog(hDlg, 0);
        }
        if (Id == GUI_ID_CANCEL) {        /* Cancel Button */
          GUI_EndDialog(hDlg, 1);
        }
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
  int Value = 0;
  WM_HWIN hDlgFrame;
  
  WM_SetCreateFlags(WM_CF_MEMDEV);  /* Use memory devices on all windows to avoid flicker */
  GUI_Init();
  //
  // Check if recommended memory for the sample is available
  //
  if (GUI_ALLOC_GetNumFreeBytes() < RECOMMENDED_MEMORY) {
    GUI_ErrorOut("Not enough memory available."); 
    return;
  }
  WM_SetCallback(WM_HBKWIN, _cbBkWindow);  
  hDlgFrame = 0;
  while(1) {
    WM_HWIN hDlg, hText;
    char acText[3] = {0};
    GUI_Delay(150);
    if (!WM_IsWindow(hDlgFrame)) {
      hDlgFrame = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), &_cbCallback, 0, 0, 0);
    }
    Value = (Value + 1) % 100;
    acText[0] = '0' + Value / 10;
    acText[1] = '0' + Value % 10;
    hDlg = WM_GetClientWindow(hDlgFrame);
    hText = WM_GetDialogItem(hDlg, GUI_ID_TEXT0);
    TEXT_SetText(hText, acText);
  }
}

/*************************** End of file ****************************/

