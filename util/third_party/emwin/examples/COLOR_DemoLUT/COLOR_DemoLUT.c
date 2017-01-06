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
File        : COLOR_DemoLUT.c
Purpose     : Example demonstrating the use of LUT-functions
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

//
// Recommended memory to run the sample with adequate performance
//
#define RECOMMENDED_MEMORY (1024L * 5)

/*******************************************************************
*
*       Static code
*
********************************************************************
*/
/*******************************************************************
*
*       _ShowColorBar
*/
static void _ShowColorBar(void) {
  int x0;
  int y0;
  int yStep;
  int i;
  int xsize;
  U16 cs;
  U16 x;

  //
  // Initialize parameters
  //
  x0        = 60;
  y0        = 40;
  yStep     = 15;
  xsize     = LCD_GetDevCap(LCD_DEVCAP_XSIZE) - x0;
  //
  // Draw labels
  //
  GUI_SetFont(&GUI_Font8x16);
  GUI_SetColor(GUI_WHITE);
  GUI_SetBkColor(GUI_BLACK); 
  GUI_DispStringAt("Red",     0, y0 +      yStep);
  GUI_DispStringAt("Green",   0, y0 +  3 * yStep);
  GUI_DispStringAt("Blue",    0, y0 +  5 * yStep);
  GUI_DispStringAt("Grey",    0, y0 +  6 * yStep);
  GUI_DispStringAt("Yellow",  0, y0 +  8 * yStep);
  GUI_DispStringAt("Cyan",    0, y0 + 10 * yStep);
  GUI_DispStringAt("Magenta", 0, y0 + 12 * yStep);
  //
  // Draw color bars
  //
  for (i = 0; i < xsize; i++) {
    cs = (0xFF * (U32)i) / xsize;
    x = x0 + i;;
    //
    // Red
    //
    GUI_SetColor(cs);
    GUI_DrawVLine(x, y0             , y0 +     yStep - 1);
    GUI_SetColor(0xff + (255 - cs) * 0x10100L);
    GUI_DrawVLine(x, y0 +      yStep, y0 + 2 * yStep - 1);
    //
    // Green
    //
    GUI_SetColor(cs<<8);
    GUI_DrawVLine(x, y0 +  2 * yStep, y0 + 3 * yStep - 1);
    GUI_SetColor(0xff00 + (255 - cs) * 0x10001L);
    GUI_DrawVLine(x, y0 +  3 * yStep, y0 + 4 * yStep - 1);
    //
    // Blue
    //
    GUI_SetColor(cs * 0x10000L);
    GUI_DrawVLine(x, y0 +  4 * yStep, y0 + 5 * yStep - 1);
    GUI_SetColor(0xff0000 + (255 - cs) * 0x101L);
    GUI_DrawVLine(x, y0 +  5 * yStep, y0 + 6 * yStep - 1);
    //
    // Gray
    //
    GUI_SetColor((U32)cs * 0x10101L);
    GUI_DrawVLine(x, y0 +  6 * yStep, y0 + 7 * yStep - 1);
    //
    // Yellow
    //
    GUI_SetColor(cs * 0x101);
    GUI_DrawVLine(x, y0 +  7 * yStep, y0 + 8 * yStep - 1);
    GUI_SetColor(0xffff + (255 - cs) * 0x10000L);
    GUI_DrawVLine(x, y0 +  8 * yStep, y0 + 9 * yStep - 1);
    //
    // Cyan
    //
    GUI_SetColor(cs * 0x10100L);
    GUI_DrawVLine(x, y0 +  9 * yStep, y0 + 10 * yStep - 1);
    GUI_SetColor(0xffff00 + (255 - cs) * 0x1L);
    GUI_DrawVLine(x, y0 + 10 * yStep, y0 + 11 * yStep - 1);
    //
    // Magenta
    //
    GUI_SetColor(cs * 0x10001);
    GUI_DrawVLine(x, y0 + 11 * yStep, y0 + 12 * yStep - 1);
    GUI_SetColor(0xff00ff + (255 - cs) * 0x100L);
    GUI_DrawVLine(x, y0 + 12 * yStep, y0 + 13 * yStep - 1);
  }
}

/*******************************************************************
*
*       _ModifyLUT
*
* Function description
*   Modify the RGB-shares of all colors
*/
static void _ModifyLUT(int RFaktor, int GFaktor, int BFaktor) {
  U32 Color;
  U32 R;
  U32 G;
  U32 B;
  int i;
  int NumColors;

  NumColors = LCD_GetDevCap(LCD_DEVCAP_NUMCOLORS);
  for (i = 0; i < NumColors; i++) {
    //
    // Get color
    //
    Color = LCD_Index2Color(i);
    R =  Color        & 0xff;
    G = (Color >>  8) & 0xff;
    B = (Color >> 16) & 0xff;
    //
    // Modify color
    //
    R = (R * RFaktor) / 100; 
    G = (G * GFaktor) / 100; 
    B = (B * BFaktor) / 100; 
    Color = R | (G << 8)| (B << 16);
    //
    // Write modified color into lookup table
    //
    LCD_SetLUTEntryEx(0, i, Color);    
  }
}

/*******************************************************************
*
*       _DemoLUT
*
* Purpose:
*   Modify the RGB-shares of all colors
*/
static void _DemoLUT(void) {
  int i;

  //
  // First draw color bars
  //
  _ShowColorBar();
  while (1) {
    //
    // Reduce the red share
    //
    for (i = 0; i <= 100; i += 10) {
      _ModifyLUT(100 - i, 100, 100);
      GUI_Delay(100);
    }
    GUI_Delay(500);
    //
    // Reduce the green share
    //
    for (i = 0; i <= 100; i += 10) {
      _ModifyLUT(0, 100 - i, 100);
      GUI_Delay(100);
    }
    GUI_Delay(500);
    //
    // Reduce the blue share
    //
    for (i = 0; i <= 100; i += 10) {
      _ModifyLUT(0, 0, 100 - i);
      GUI_Delay(100);
    }
    GUI_Delay(500);
    //
    // Restore initial values
    //
    LCD_InitLUT();
    GUI_Delay(500);
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
  GUI_DispStringHCenterAt("COLOR_DemoLUT - Sample", 160, 5);
  _DemoLUT();
}

/*************************** End of file ****************************/
