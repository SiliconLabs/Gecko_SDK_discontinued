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
File        : 2DGL_DrawPolygon.c
Purpose     : Example for drawing polygons
Requirements: WindowManager - ( )
              MemoryDevices - ( )
              AntiAliasing  - ( )
              VNC-Server    - ( )
              PNG-Library   - ( )
              TrueTypeFonts - ( )
----------------------------------------------------------------------
*/

#include "GUI.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
//
// Recommended memory to run the sample with adequate performance
//
#define RECOMMENDED_MEMORY (1024L * 5)

/*******************************************************************
*
*       Static variables
*
********************************************************************
*/
static const GUI_POINT _aPointArrow[] = {
  {  0,   0 },
  {-40, -30 },
  {-10, -20 },
  {-10, -70 },
  { 10, -70 },
  { 10, -20 },
  { 40, -30 },
};

static const GUI_POINT _aPointStar[] = {
  {  0, -36 },
  {  8,  -8 },
  { 36,   0 },
  {  8,   8 },
  {  0,  36 },
  { -8,   8 },
  {-36,   0 },
  { -8,  -8 }
};

static const GUI_POINT _aPointHexagon[] = {
  {  0, -30 },
  { 26, -15 },
  { 26,  15 },
  {  0,  30 },
  {-26,  15 },
  {-26, -15 },
};

/*******************************************************************
*
*       Static code
*
********************************************************************
*/
/*******************************************************************
*
*       _DrawPolygons
*
* Function description
*   Draws polygons of different shapes and colors
*/
static void _DrawPolygons(void) {
  int y;

  y = 90;
  //
  // clear display
  //
  GUI_SetBkColor(GUI_BLACK);
  GUI_Clear();
  //
  // display text
  //
  GUI_SetColor(GUI_WHITE);
  GUI_SetFont(&GUI_Font24_ASCII);
  GUI_SetTextAlign(GUI_TA_HCENTER);
  GUI_DispStringAt("DrawPolygon - Sample", 160, 5);
  GUI_SetFont(&GUI_Font8x16);
  GUI_DispStringAt("using", 5, 40);
  GUI_DispStringAt("GUI_FillPolygon", 5, 55);
  GUI_SetTextAlign(GUI_TA_HCENTER);
  GUI_DispStringAt("Polygons of arbitrary shape\nin any color", 160, y + 90);
  GUI_Delay(500);
  //
  // draw filled polygons
  //
  while (1) {
    GUI_ClearRect(100, y, 220, y + 85);
    GUI_SetColor(GUI_BLUE);
    GUI_FillPolygon (&_aPointArrow[0], 7, 160, y + 80);
    GUI_Delay(1000);
    GUI_ClearRect(100, y, 220, y + 85);
    GUI_SetColor(GUI_RED);
    GUI_FillPolygon (&_aPointStar[0], 8, 160, y + 45);
    GUI_Delay(1000);
    GUI_ClearRect(100, y, 220, y + 85);
    GUI_SetColor(GUI_GREEN);
    GUI_FillPolygon(&_aPointHexagon[0], 6, 160, y + 45);
    GUI_Delay(1000);
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
  _DrawPolygons();
  while (1);
}

/*************************** End of file ****************************/

