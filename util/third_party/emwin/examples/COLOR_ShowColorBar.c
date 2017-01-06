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
File        : COLOR_ShowColorBar.c
Purpose     : Example draws a color bar
Requirements: WindowManager - ( )
              MemoryDevices - ( )
              AntiAliasing  - ( )
              VNC-Server    - ( )
              PNG-Library   - ( )
              TrueTypeFonts - ( )
----------------------------------------------------------------------
*/

#include "GUI.h"
#include "LCDConf.h"

/*******************************************************************
*
*       Defines
*
********************************************************************
*/
#define X_START 60
#define Y_START 40

//
// Recommended memory to run the sample with adequate performance
//
#define RECOMMENDED_MEMORY (1024L * 5)

/*******************************************************************
*
*       Types
*
********************************************************************
*/
typedef struct {
  int NumBars;

  GUI_COLOR Color;
  const char * s;
} BAR_DATA;

/*******************************************************************
*
*       Static data
*
********************************************************************
*/
static const BAR_DATA _aBarData[] = {
  { 2, GUI_RED    , "Red" },
  { 2, GUI_GREEN  , "Green" },
  { 2, GUI_BLUE   , "Blue" },
  { 1, GUI_WHITE  , "Grey" },
  { 2, GUI_YELLOW , "Yellow" },
  { 2, GUI_CYAN   , "Cyan" },
  { 2, GUI_MAGENTA, "Magenta" },
};

static const GUI_COLOR _aColorStart[] = { GUI_BLACK, GUI_WHITE };

/*******************************************************************
*
*       Static code
*
********************************************************************
*/
/*******************************************************************
*
*       _DemoShowColorBar
*/
static void _DemoShowColorBar(void) {
  GUI_RECT Rect;
  int      yStep;
  int      i;
  int      j;
  int      xSize;
  int      ySize;
  int      NumBars;
  int      NumColors;

  //
  // Get display size
  //
  xSize = LCD_GetXSize();
  ySize = LCD_GetYSize();
  //
  // Get number of bars
  //
  NumColors = GUI_COUNTOF(_aBarData);
  for (i = NumBars = 0, NumBars = 0; i < NumColors; i++) {
    NumBars += _aBarData[i].NumBars;
  }
  yStep = (ySize - Y_START) / NumBars;
  //
  // Draw text
  //
  Rect.x0 = 0;
  Rect.x1 = X_START - 1;
  Rect.y0 = Y_START;
	GUI_SetFont(&GUI_Font8x16);
  for (i = 0; i < NumColors; i++) {
    Rect.y1 = Rect.y0 + yStep * _aBarData[i].NumBars - 1;
    GUI_DispStringInRect(_aBarData[i].s, &Rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    Rect.y0 = Rect.y1 + 1;
  }
  //
  // Draw colors
  //
  Rect.x0 = X_START;
  Rect.x1 = xSize - 1;
  Rect.y0 = Y_START;
  for (i = 0; i < NumColors; i++) {
    for (j = 0; j < _aBarData[i].NumBars; j++) {
      Rect.y1 = Rect.y0 + yStep - 1;
      GUI_DrawGradientH(Rect.x0, Rect.y0, Rect.x1, Rect.y1, _aColorStart[j], _aBarData[i].Color);
      Rect.y0 = Rect.y1 + 1;
    }
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
  GUI_SetBkColor(GUI_BLACK);
  GUI_Clear();
  GUI_SetColor(GUI_WHITE);
  GUI_SetFont(&GUI_Font24_ASCII);
  GUI_DispStringHCenterAt("COLOR_ShowColorBar - Sample", 160, 5);
  _DemoShowColorBar();
  while(1)
    GUI_Delay(100);
}

/*************************** End of file ****************************/
