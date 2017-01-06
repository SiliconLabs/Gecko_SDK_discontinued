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
File        : WIDGET_ListBox.c
Purpose     : Example demonstrating the LISTBOX widget
Requirements: WindowManager - (x)
              MemoryDevices - ( )
              AntiAliasing  - ( )
              VNC-Server    - ( )
              PNG-Library   - ( )
              TrueTypeFonts - ( )
----------------------------------------------------------------------
*/
#include "GUI.h"
#include "FRAMEWIN.h"
#include "LISTBOX.h"
#include <stddef.h>

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define SPEED 1000

//
// Recommended memory to run the sample with adequate performance
//
#define RECOMMENDED_MEMORY (1024L * 5)

/*******************************************************************
*
*       static variables
*
********************************************************************
*/
static const GUI_ConstString _ListBox[] = {
  "English", "Deutsch", NULL
};

/*******************************************************************
*
*       static code
*
********************************************************************
*/
/*******************************************************************
*
*       _ShowSeveralFunctions
*/
static void _ShowSeveralFunctions(LISTBOX_Handle hListBox) {
  int NumEntries;
  int i;

  //
  // Add scrollbar
  //
  GUI_DispStringAtCEOL("SCROLLBAR_CreateAttached", 5, 55);
  GUI_Delay(SPEED);
  SCROLLBAR_CreateAttached(hListBox, SCROLLBAR_CF_VERTICAL);
  GUI_Delay(SPEED * 0.75);
  //
  // Add strings
  //
  GUI_DispStringAtCEOL("LISTBOX_AddString", 5, 55);
  GUI_Delay(SPEED);
  LISTBOX_AddString(hListBox, "Français");
  GUI_Delay(SPEED / 6);
  LISTBOX_AddString(hListBox, "Japanese");
  GUI_Delay(SPEED / 6);
  LISTBOX_AddString(hListBox, "Italiano");
  GUI_Delay(SPEED / 6);
  LISTBOX_AddString(hListBox, "Español");
  GUI_Delay(SPEED / 6);
  LISTBOX_AddString(hListBox, "Other language ...");
  GUI_Delay(SPEED * 0.6);
  //
  // Set focus
  //
  GUI_DispStringAtCEOL("WM_SetFocus", 5, 55);
  GUI_Delay(SPEED * 0.9);
  WM_SetFocus(hListBox);
  GUI_Delay(SPEED * 0.7);
  //
  // Set font
  //
  GUI_DispStringAtCEOL("LISTBOX_SetFont", 5, 55);
  GUI_Delay(SPEED * 0.9);
  LISTBOX_SetFont(hListBox, &GUI_Font13B_1);
  GUI_Delay(SPEED * 0.7);
  //
  // Increment selection
  //
  GUI_DispStringAtCEOL("LISTBOX_IncSel", 5, 55);
  GUI_Delay(SPEED);
  NumEntries = LISTBOX_GetNumItems(hListBox);
  for (i = 0; i < NumEntries - 1; i++) {
    LISTBOX_IncSel(hListBox);
    GUI_Delay(SPEED / 6);
	}
  GUI_Delay(SPEED / 4);
  //
  // Show automatic scrollbar
  //
  GUI_DispStringAtCEOL("Optional automatic scrollbar", 5, 55);
  GUI_Delay(SPEED);
  LISTBOX_SetAutoScrollH(hListBox, 1);
  LISTBOX_SetAutoScrollV(hListBox, 1);
  GUI_Delay(SPEED * 0.75);
  //
  // Set font
  //
  GUI_DispStringAtCEOL("LISTBOX_SetFont", 5, 55);
  GUI_Delay(SPEED);
  LISTBOX_SetFont(hListBox, &GUI_Font16B_1);
  GUI_Delay(SPEED * 0.75);
  //
  // Decrement selection
  //
  GUI_DispStringAtCEOL("LISTBOX_DecSel", 5, 55);
  GUI_Delay(SPEED);
  for (i = 0; i < NumEntries - 1; i++) {
    LISTBOX_DecSel(hListBox);
    GUI_Delay(SPEED / 6);
	}
  GUI_Delay(SPEED / 4);
  //
  // Change width of scrollbar
  //
  GUI_DispStringAtCEOL("Change scrollbar width", 5, 55);
  GUI_Delay(SPEED * 0.7);
  {
    SCROLLBAR_Handle hScrollH = WM_GetDialogItem(hListBox, GUI_ID_HSCROLL);
    SCROLLBAR_Handle hScrollV = WM_GetDialogItem(hListBox, GUI_ID_VSCROLL);
    SCROLLBAR_SetWidth(hScrollV, 14);
    GUI_Delay(SPEED / 4);
    SCROLLBAR_SetWidth(hScrollH, 14);
    GUI_Delay(SPEED * 0.6);
  }
  //
  // Change size of listbox
  //
  GUI_DispStringAtCEOL("Change size of listbox", 5, 55);
  GUI_Delay(SPEED * 0.75);
  WM_ResizeWindow(hListBox, -15, 0);
  GUI_Delay(SPEED / 4);
  WM_ResizeWindow(hListBox, 0, -15);
  GUI_Delay(SPEED / 4);
  WM_ResizeWindow(hListBox, 15, 0);
  GUI_Delay(SPEED / 4);
  WM_ResizeWindow(hListBox, 0, 15);
  GUI_Delay(SPEED / 2);
  //
  // Disable item
  //
  GUI_DispStringAtCEOL("LISTBOX_SetItemDisabled", 5, 55);
  GUI_Delay(SPEED);
  LISTBOX_SetItemDisabled(hListBox, 4, 1);
  GUI_Delay(SPEED * 0.75);
  //
  // Set multi selection mode
  //
  GUI_DispStringAtCEOL("LISTBOX_SetMulti", 5, 55);
  GUI_Delay(SPEED);
  LISTBOX_SetMulti(hListBox, 1);
  GUI_Delay(SPEED * 0.75);
  //
  // Select item
  //
  GUI_DispStringAtCEOL("LISTBOX_SetItemSel", 5, 55);
  GUI_Delay(SPEED);
  LISTBOX_SetItemSel(hListBox, 0, 1);
  GUI_Delay(SPEED / 4);
  LISTBOX_SetItemSel(hListBox, 1, 1);
  GUI_Delay(SPEED / 4);
  LISTBOX_SetItemSel(hListBox, 2, 1);
  GUI_Delay(SPEED * 0.8);
  //
  // Delete listbox widget
  //
  GUI_DispStringAtCEOL("WM_DeleteWindow", 5, 55);
  GUI_Delay(SPEED * 1.1);
  WM_DeleteWindow(hListBox);
  GUI_Delay(SPEED * 0.75);
}

/*******************************************************************
*
*       _DemoListBox
*/
static void _DemoListBox(void) {
  LISTBOX_Handle hListBox;

  //
  // Display titel
  //
  GUI_SetBkColor(0xB00000);
  GUI_SetColor(0xFFFFFF);
  GUI_SetFont(&GUI_Font24_ASCII);
  GUI_DispStringHCenterAt("Listbox - Sample", 160, 5);
  GUI_Delay(SPEED / 2);
  //
  // Create listbox
  //
  GUI_SetFont(&GUI_Font8x16);
  GUI_SetTextAlign(GUI_TA_LEFT);
  GUI_DispStringAtCEOL("using", 5, 40);
  GUI_DispStringAtCEOL("LISTBOX_Create", 5, 55);
  GUI_Delay(SPEED * 0.9);
  hListBox = LISTBOX_Create(_ListBox, 100, 80, 120, 115, WM_CF_SHOW);
  GUI_Delay(SPEED * 0.75);
  //
  // Show serveral functions of listbox
  //
  _ShowSeveralFunctions(hListBox);
  //
  // Clear display
  //
  GUI_Clear();
  GUI_Delay(SPEED * 1.5);
}

/*******************************************************************
*
*       _DemoListBoxAsChild
*/
static void _DemoListBoxAsChild(void) {
  FRAMEWIN_Handle hFrame;
  LISTBOX_Handle  hListBox;

  //
  // Display titel
  //
  GUI_SetBkColor(0xB00000);
  GUI_SetColor(0xFFFFFF);
  GUI_SetFont(&GUI_Font24_ASCII);
  GUI_DispStringHCenterAt("Listbox as child - Sample", 160, 5);
  GUI_Delay(SPEED / 2);
  //
  // Create framewin
  //
  GUI_SetFont(&GUI_Font8x16);
  GUI_SetTextAlign(GUI_TA_LEFT);
  GUI_DispStringAtCEOL("using", 5, 40);
  GUI_DispStringAtCEOL("FRAMEWIN_Create", 5, 55);
  GUI_Delay(SPEED);
  hFrame = FRAMEWIN_Create("List box", NULL, WM_CF_SHOW, 100, 80, 120, 140);
  FRAMEWIN_SetFont(hFrame, &GUI_Font16B_ASCII);
  FRAMEWIN_SetActive(hFrame, 1);
  GUI_Delay(SPEED * 0.75);
  //
  // Create listbox
  //
  GUI_DispStringAtCEOL("LISTBOX_CreateAsChild", 5, 55);
  GUI_Delay(SPEED);
  hListBox = LISTBOX_CreateAsChild(_ListBox, WM_GetClientWindow(hFrame), 0, 0, 0, 0, WM_CF_SHOW);
  GUI_Delay(SPEED * 0.75);
  //
  // Show serveral functions of listbox
  //
  _ShowSeveralFunctions(hListBox);
  //
  // Delete framewin widget
  //
  GUI_DispStringAtCEOL("WM_DeleteWindow", 5, 55);
  GUI_Delay(SPEED);
  WM_DeleteWindow(hFrame);
  GUI_Delay(SPEED * 0.75);
  //
  // Clear display
  //
  GUI_Clear();
  GUI_Delay(SPEED * 1.5);
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
  WM_SetDesktopColor(0xB00000);
  GUI_Exec();
  while(1) {
    _DemoListBox();
    _DemoListBoxAsChild();
  }
}

/*************************** End of file ****************************/

