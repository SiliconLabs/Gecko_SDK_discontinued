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
File        : MainTask.c
Purpose     : Main program, called from after main after initialisation
Requirements: WindowManager - (x)
              MemoryDevices - (x)
              AntiAliasing  - ( )
              VNC-Server    - ( )
              PNG-Library   - ( )
              TrueTypeFonts - ( )
---------------------------END-OF-HEADER------------------------------
*/

#include <stddef.h>
#include "GUI.h"
#include "DIALOG.h"
#include "Main.h"
#include "LCDConf.h"

/*********************************************************************
*
*       Dialog IDs
*/
#define ID_BUTTON_SETUP          1
#define ID_BUTTON_CALIBRATION    2
#define ID_BUTTON_ABOUT          3

//
// Recommended memory to run the sample with adequate performance
//
#define RECOMMENDED_MEMORY (1024L * 20)

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
* This table conatins the info required to create the dialog.
* It has been created manually, but could also be created by a GUI-builder.
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { FRAMEWIN_CreateIndirect, "Main Screen", 0,                       0,   0, 320, 240, 0},
  { BUTTON_CreateIndirect,   "Setup",       ID_BUTTON_SETUP,       230,  80,  60,  20 },
  { BUTTON_CreateIndirect,   "Calibration", ID_BUTTON_CALIBRATION, 230, 110,  60,  20 },
  { BUTTON_CreateIndirect,   "About",       ID_BUTTON_ABOUT,       230, 140,  60,  20 },
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
  WM_HWIN hWinSrc;
  int Id;
  int NCode;

  hWinSrc = pMsg->hWinSrc;
  hDlg = pMsg->hWin;
  switch (pMsg->MsgId) {
  case WM_PAINT:
    GUI_SetColor(GUI_BLACK);
    GUI_DrawBitmap(&bmLogoBitmap, 30, 80);
    GUI_SetFont(&GUI_Font24B_ASCII);
    GUI_SetFont(&GUI_Font16B_ASCII);
    GUI_DispStringHCenterAt("www.segger.com", 30 + bmLogoBitmap.XSize / 2, 80 + bmLogoBitmap.YSize);
    GUI_DispStringHCenterAt("Virtual screen sample", 160, 20);
    break;
  case WM_INIT_DIALOG:
    FRAMEWIN_SetFont(hDlg, &GUI_Font24B_ASCII);
    FRAMEWIN_SetTextAlign(hDlg, GUI_TA_HCENTER);
    break;
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(hWinSrc);      /* Id of widget */
    NCode = pMsg->Data.v;           /* Notification code */
    switch (NCode) {
    case WM_NOTIFICATION_RELEASED:
      switch (Id) {
      case ID_BUTTON_SETUP:
        ExecSetup();
        break;
      case ID_BUTTON_CALIBRATION:
        ExecCalibration();
        break;
      case ID_BUTTON_ABOUT:
        ExecAbout();
        break;
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
void MainTask(void);
void MainTask(void) {
  int XSize;
  int YSize;

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
  XSize = LCD_GetXSize();
  YSize = LCD_GetYSize();
  GUI_DrawBitmap(&bmLogoBitmap, (XSize - bmLogoBitmap.XSize) / 2, (YSize - bmLogoBitmap.YSize) / 3);
  GUI_SetFont(&GUI_Font24B_ASCII);
  GUI_DispStringHCenterAt("www.segger.com", XSize / 2, (YSize - bmLogoBitmap.YSize) / 3 + bmLogoBitmap.YSize + 10);
  GUI_Delay(1000);
  WM_SetDesktopColor(GUI_BLACK); /* Not required since desktop is not visible */
  GUI_ExecDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), &_cbCallback, 0, 0, 0);
}

/*************************** End of file ****************************/

