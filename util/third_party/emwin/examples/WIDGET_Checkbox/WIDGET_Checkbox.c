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
File        : WIDGET_CheckBox.c
Purpose     : Example demonstrating the use of a CHECKBOX widget
Requirements: WindowManager - (x)
              MemoryDevices - ( )
              AntiAliasing  - ( )
              VNC-Server    - ( )
              PNG-Library   - ( )
              TrueTypeFonts - ( )
----------------------------------------------------------------------
*/

#include <stddef.h>
#include "GUI.h"
#include "DIALOG.h"
#include "Main.h"

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

//
// Dialog resource
//
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { FRAMEWIN_CreateIndirect, "Check box sample",   0,   10,  10, 300, 220, FRAMEWIN_CF_MOVEABLE},
  { TEXT_CreateIndirect,     "Enabled:",           0,    5,  10, 120,   0 },
  { CHECKBOX_CreateIndirect, 0,        GUI_ID_CHECK0,    5,  30, 120,   0 },
  { CHECKBOX_CreateIndirect, 0,        GUI_ID_CHECK1,    5,  60, 120,   0 },
  { CHECKBOX_CreateIndirect, 0,        GUI_ID_CHECK2,    5,  90, 120,   20 },
  { CHECKBOX_CreateIndirect, 0,        GUI_ID_CHECK3,    5, 125, 120,   26 },
  { TEXT_CreateIndirect,     "Disabled:",          0,  150,  10, 120,   0 },
  { CHECKBOX_CreateIndirect, 0,        GUI_ID_CHECK4,  150,  30, 120,   0 },
  { CHECKBOX_CreateIndirect, 0,        GUI_ID_CHECK5,  150,  60, 120,   0 },
  { CHECKBOX_CreateIndirect, 0,        GUI_ID_CHECK6,  150,  90, 120,   26 },
  { CHECKBOX_CreateIndirect, 0,        GUI_ID_CHECK7,  150, 125, 120,   26 },
  { BUTTON_CreateIndirect,   "OK",         GUI_ID_OK,   10, 170,  60,  20 },
  { BUTTON_CreateIndirect,   "Cancel", GUI_ID_CANCEL,  225, 170,  60,  20 },
};

//
// Check box text
//
static const char * _apLabel[] = {
  "Default",
  "3 States",
  "Box XL",
  "Box XXL"
};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _cbCallback
*/
static void _cbCallback(WM_MESSAGE * pMsg) {
  WM_HWIN hDlg;
  WM_HWIN hItem;
  int     i;
  int     NCode;
  int     Id;

  hDlg = pMsg->hWin;
  switch (pMsg->MsgId) {
    case WM_INIT_DIALOG:
      WM_GetDialogItem(hDlg, GUI_ID_CHECK0);
      for (i = 0; i < 8; i++) {
        int Index = i % 4;
        hItem = WM_GetDialogItem(hDlg, GUI_ID_CHECK0 + i); // Get the handle of the desired checkbox
        CHECKBOX_SetText(hItem, _apLabel[Index]);          // Set the check box text
        switch (Index) {
        case 1:
          //
          // Set the number of possible states to 3
          //
          CHECKBOX_SetNumStates(hItem, 3);
          //
          // Use user defined bitmaps for the third state
          //
          CHECKBOX_SetImage(hItem, &_abmBar[0], CHECKBOX_BI_INACTIV_3STATE);  // Used if widget is disabled
          CHECKBOX_SetImage(hItem, &_abmBar[1], CHECKBOX_BI_ACTIV_3STATE);    // Used if widget is enabled
          CHECKBOX_SetState(hItem, 2);
          break;
        case 2:
          CHECKBOX_SetState(hItem, 1);
          //
          // Use user defined bitmaps for the 'checked' state
          //
          CHECKBOX_SetImage(hItem, &_abmXL[0], CHECKBOX_BI_INACTIV_CHECKED);  // Used if widget is disabled
          CHECKBOX_SetImage(hItem, &_abmXL[1], CHECKBOX_BI_ACTIV_CHECKED);    // Used if widget is enabled 
          CHECKBOX_SetFont(hItem, &GUI_FontComic18B_ASCII);
          break;
        case 3:
          CHECKBOX_SetState(hItem, 1);
          //
          // Use user defined bitmaps for the 'checked' state
          //
          CHECKBOX_SetImage(hItem, &_abmXXL[0], CHECKBOX_BI_INACTIV_CHECKED); // Used if widget is disabled
          CHECKBOX_SetImage(hItem, &_abmXXL[1], CHECKBOX_BI_ACTIV_CHECKED);   // Used if widget is enabled 
          CHECKBOX_SetFont(hItem, &GUI_FontComic24B_ASCII);
          break;
        }
        //
        // Disable all check boxes shown on the right side of the dialog
        //
        if (i >= 4) {
          WM_DisableWindow(hItem);
        }
      }
      break;
    case WM_NOTIFY_PARENT:
      Id    = WM_GetId(pMsg->hWinSrc);      // Id of widget
      NCode = pMsg->Data.v;                 // Notification code
      switch (NCode) {
        case WM_NOTIFICATION_RELEASED:      // React only if released
          if (Id == GUI_ID_OK) {            // OK Button
            GUI_EndDialog(hDlg, 0);
          }
          if (Id == GUI_ID_CANCEL) {        // Cancel Button
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
  #if GUI_SUPPORT_MEMDEV
    WM_SetCreateFlags(WM_CF_MEMDEV);
  #endif
  GUI_Init();
  //
  // Check if recommended memory for the sample is available
  //
  if (GUI_ALLOC_GetNumFreeBytes() < RECOMMENDED_MEMORY) {
    GUI_ErrorOut("Not enough memory available."); 
    return;
  }
  WM_SetDesktopColor(GUI_BLACK);
  while (1) {
    GUI_ExecDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), &_cbCallback, 0, 0, 0);
    GUI_Delay(500);
  }
}

/*************************** End of file ****************************/

