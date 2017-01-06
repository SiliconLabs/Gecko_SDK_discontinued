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
File        : WIDGET_Spinbox.c
Purpose     : Shows how to use the SPINBOX widget.
Requirements: WindowManager - (x)
              MemoryDevices - ( )
              AntiAliasing  - ( )
              VNC-Server    - ( )
              PNG-Library   - ( )
              TrueTypeFonts - ( )
---------------------------END-OF-HEADER------------------------------
*/

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
static const GUI_WIDGET_CREATE_INFO _aDialogSpinbox[] = {
  { FRAMEWIN_CreateIndirect, "Spinbox",           0,                 0,  0, 260, 160, 0, 0, 0 },
  { TEXT_CreateIndirect,     "Step value",        GUI_ID_TEXT0,     20, 15, 100,  21, 0, 0, 0 },
  { TEXT_CreateIndirect,     "Editmode \"Step\"", GUI_ID_TEXT1,     20, 42, 100,  21, 0, 0, 0 },
  { TEXT_CreateIndirect,     "Editmode \"Edit\"", GUI_ID_TEXT2,     20, 75, 100,  21, 0, 0, 0 },
  { SPINBOX_CreateIndirect,  NULL,                GUI_ID_SPINBOX0, 130, 15,  60,  21, 0, 0, 0 },
  { SPINBOX_CreateIndirect,  NULL,                GUI_ID_SPINBOX1, 130, 42,  60,  21, 0, 0, 0 },
  { SPINBOX_CreateIndirect,  NULL,                GUI_ID_SPINBOX2, 130, 75,  60,  21, 0, 0, 0 },
};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _cbBk
*/
static void _cbBk(WM_MESSAGE * pMsg) {
  int xSize;
  int ySize;

  switch (pMsg->MsgId) {
  case WM_PAINT:
    xSize = LCD_GetXSize();
    ySize = LCD_GetYSize();
    GUI_DrawGradientV(0, 0, xSize, ySize, GUI_BLUE, GUI_BLACK);
    GUI_SetColor(GUI_WHITE);
    GUI_SetFont(&GUI_Font24_ASCII);
    GUI_SetTextMode(GUI_TM_TRANS);
    GUI_DispStringHCenterAt("WIDGET_Spinbox - Sample", xSize / 2, 5);
    break;
  default:
    WM_DefaultProc(pMsg);
  }
}

/*********************************************************************
*
*       _cbClient
*/
static void _cbClient(WM_MESSAGE * pMsg) {
  EDIT_Handle hEdit;
  WM_HWIN     hItem;
  int         Value;
  int         NCode;
  int         Id;

  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:
    FRAMEWIN_SetFont(pMsg->hWin, GUI_FONT_16B_ASCII);
    FRAMEWIN_SetSkin(pMsg->hWin, FRAMEWIN_SKIN_FLEX);
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT0);
    TEXT_SetTextAlign(hItem, GUI_TA_VCENTER | GUI_TA_RIGHT);
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT1);
    TEXT_SetTextAlign(hItem, GUI_TA_VCENTER | GUI_TA_RIGHT);
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT2);
    TEXT_SetTextAlign(hItem, GUI_TA_VCENTER | GUI_TA_RIGHT);
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_SPINBOX0);
    SPINBOX_SetSkin(hItem, SPINBOX_SKIN_FLEX);
    hEdit = SPINBOX_GetEditHandle(hItem);
    EDIT_SetDecMode(hEdit, 1, 1, 10, 0, 0);
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_SPINBOX1);
    SPINBOX_SetSkin(hItem, SPINBOX_SKIN_FLEX);
    hEdit = SPINBOX_GetEditHandle(hItem);
    EDIT_SetDecMode(hEdit, 1, 0, 99999, 0, 0);
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_SPINBOX2);
    SPINBOX_SetSkin(hItem, SPINBOX_SKIN_FLEX);
    SPINBOX_SetEditMode(hItem, SPINBOX_EM_EDIT);
    SPINBOX_SetRange(hItem, 0, 99999);
    break;
  case WM_NOTIFY_PARENT:
    NCode = pMsg->Data.v;
    switch (NCode) {
    case WM_NOTIFICATION_VALUE_CHANGED:
      Id = WM_GetId(pMsg->hWinSrc);
      if (Id == GUI_ID_SPINBOX0) {
        Value = SPINBOX_GetValue(pMsg->hWinSrc);
        hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_SPINBOX1);
        SPINBOX_SetStep(hItem, Value);
      }
      break;
    default:
      WM_DefaultProc(pMsg);
    }
    break;
  case WM_PAINT:
    GUI_SetBkColor(0xAE9E8D);
    GUI_Clear();
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
  GUI_Init();
  //
  // Check if recommended memory for the sample is available
  //
  if (GUI_ALLOC_GetNumFreeBytes() < RECOMMENDED_MEMORY) {
    GUI_ErrorOut("Not enough memory available."); 
    return;
  }
  WM_SetCallback(WM_HBKWIN, _cbBk);
  TEXT_SetDefaultTextColor(GUI_WHITE);
  GUI_CreateDialogBox(_aDialogSpinbox, GUI_COUNTOF(_aDialogSpinbox), _cbClient,  WM_HBKWIN,  30,  60);
  while (1) {
    GUI_Delay(100);
  }
}

/*************************** End of file ****************************/
