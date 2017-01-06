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
File        : WIDGET_PopupMenu.c
Purpose     : Shows how to create a popup menu
Requirements: WindowManager - (x)
              MemoryDevices - ( )
              AntiAliasing  - ( )
              VNC-Server    - ( )
              PNG-Library   - ( )
              TrueTypeFonts - ( )
----------------------------------------------------------------------
*/

#include "GUI.h"
#include "MENU.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
/* Define IDs for menu items */
#define ID_MENU_NEW       (GUI_ID_USER +  0)
#define ID_MENU_PASS      (GUI_ID_USER +  1)
#define ID_MENU_EXIT      (GUI_ID_USER +  2)

//
// Recommended memory to run the sample with adequate performance
//
#define RECOMMENDED_MEMORY (1024L * 5)

/*********************************************************************
*
*       Types
*
**********************************************************************
*/
//
// Structure for menu items
//
typedef struct {
  char * sText;
  U16 Id;
  U16 Flags;
} MENU_ITEM;

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
//
// Text for message box
//
static const char * _acMessage[] = {
  "New Game will be started...",
  "Passed...",
  "Application will be closed..."
};
//
// Explanation of sample
//
static char * _apExplain[] = {
  "This sample shows how to use a popup menu.",
  "Please touch the display at any position...",
};
//
// Array of menu items
//
static MENU_ITEM _aMenuItems[] = {
  {"New game", ID_MENU_NEW,  0},
  {"Pass",     ID_MENU_PASS, 0},
  {0,          0,            MENU_IF_SEPARATOR},
  {"Exit",     ID_MENU_EXIT, 0},
};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _AddMenuItem
*
* Function description
*   Adds one menu item to the given menu
*/
static void _AddMenuItem(MENU_Handle hMenu, MENU_Handle hSubmenu, const char* pText, U16 Id, U16 Flags) {
  MENU_ITEM_DATA Item;

  Item.pText    = pText;
  Item.hSubmenu = hSubmenu;
  Item.Flags    = Flags;
  Item.Id       = Id;
  MENU_AddItem(hMenu, &Item);
}

/*********************************************************************
*
*       _OpenPopup
*
* Function description
*   The function opens a popup menu at the given position. It returns
*   immediately after creation. On the first call it creates the menu.
*/
static void _OpenPopup(WM_HWIN hParent, MENU_ITEM * pMenuItems, int NumItems, int x, int y) {
  static MENU_Handle hMenu;
  if (!hMenu) {
    int i;

    //
    // Create the popup window only one time
    //
    hMenu = MENU_CreateEx(0, 0, 0, 0, WM_UNATTACHED, 0, MENU_CF_VERTICAL, 0);
    for (i = 0; i < NumItems; i++) {
      _AddMenuItem(hMenu, 0, pMenuItems[i].sText, pMenuItems[i].Id, pMenuItems[i].Flags);
    }
  }
  //
  // Open the popup menu. After opening the menu the function returns immediately.
  // After selecting a menu item or after touching the display outside the menu the 
  // popup menu will be closed, but not deleted.
  //
  MENU_Popup(hMenu, hParent, x, y, 0, 0, 0);
}

/*********************************************************************
*
*       _cbBkWin
*
* Function description
*   Callback function of the background window. On receiving a
*   WM_PID_STATE_CHANGED message it creates a popup menu.
*   After selecting a menu item it shows a short message box.
*/
static void _cbBkWin(WM_MESSAGE * pMsg) {
  unsigned       i;
  MENU_MSG_DATA* pData;

  switch (pMsg->MsgId) {
  case WM_PAINT:
    //
    // Paint the background window
    //
    GUI_SetBkColor(GUI_RED);
    GUI_Clear();
    GUI_SetFont(&GUI_Font24_ASCII);
    GUI_DispStringHCenterAt("WIDGET_PopupMenu - Sample", 160, 5);
    GUI_DispNextLine();
    GUI_SetFont(&GUI_Font13_ASCII);
    GUI_DispNextLine();
    for (i = 0; i < GUI_COUNTOF(_apExplain); i++) {
      GUI_DispStringHCenterAt(_apExplain[i], 160, GUI_GetDispPosY());
      GUI_DispNextLine();
    }
    break;
  case WM_MENU:
    //
    // Process the menu message
    //
    pData = (MENU_MSG_DATA*)pMsg->Data.p;
    switch (pData->MsgType) {
    case MENU_ON_ITEMSELECT:
      //
      // A menu item has been selected
      //
      switch (pData->ItemId) {
      case ID_MENU_NEW:
      case ID_MENU_PASS:
      case ID_MENU_EXIT:
        //
        // Short message box
        //
        GUI_MessageBox(_acMessage[pData->ItemId - GUI_ID_USER], "Message", GUI_MESSAGEBOX_CF_MODAL);
        break;
      }
      break;
    default:
      WM_DefaultProc(pMsg);
    }
    break;
  case WM_PID_STATE_CHANGED:
    //
    // Create a popup menu after touching the display
    //
    _OpenPopup(WM_HBKWIN, _aMenuItems, GUI_COUNTOF(_aMenuItems),
               ((WM_PID_STATE_CHANGED_INFO *)pMsg->Data.p)->x, 
               ((WM_PID_STATE_CHANGED_INFO *)pMsg->Data.p)->y);
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
*
* Function description
*   Sets only the callback function for the desktop window. The rest will
*   be done within the callback function.
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
  WM_SetCallback(WM_HBKWIN, _cbBkWin);
  while (1) {
    GUI_Delay(100);
  }
}

/*************************** End of file ****************************/

