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
File        : AA_HiResPixels.c
Purpose     : Demonstrates high resolution pixels
Requirements: WindowManager - (x)
              MemoryDevices - (x)
              AntiAliasing  - (x)
              VNC-Server    - ( )
              PNG-Library   - ( )
              TrueTypeFonts - ( )
----------------------------------------------------------------------
*/

#include "GUI.h"
#include "WM.h"

/*******************************************************************
*
*       Defines
*
********************************************************************
*/
#define AA_FACTOR    4
#define POLY_SIZE   19
#define POLY_POINTS  3

#define COORD_0(Plus, AA_Factor) (I16)(((I32)((Plus ? POLY_SIZE : -POLY_SIZE) * AA_Factor * 0.7071f * 10000)) / 10000)
#define COORD_1(Plus, AA_Factor) (I16)(((I32)((Plus ? POLY_SIZE : -POLY_SIZE) * AA_Factor * 1.2247f * 10000)) / 10000)
#define COORD_2(Plus, AA_Factor) (I16)(((I32)((Plus ? POLY_SIZE : -POLY_SIZE) * AA_Factor * 1.4142f * 10000)) / 10000)

//
// Recommended memory to run the sample with adequate performance
//
#define RECOMMENDED_MEMORY (1024L * 15)

/*******************************************************************
*
*       Static variables
*
********************************************************************
*/
static int _pos_x1         = 30;
static int _pos_y1         = 30;
static int _pos_x2         = 125;
static int _pos_y2         = 30;
static int _pos_x3         = 220 * AA_FACTOR;
static int _pos_y3         = 30  * AA_FACTOR;
static int _color_d        = -1;
static GUI_COLOR _color_a  = 0xFF00FE;
static GUI_COLOR _color_b  = 0x00FEFF;
static GUI_COLOR _color_c  = 0xFEFFFE;

static const GUI_POINT _aPolygon_src[] = {
  {             0, COORD_2(0, 1) },
  { COORD_1(1, 1), COORD_0(1, 1) },
  { COORD_1(0, 1), COORD_0(1, 1) }
};

static const GUI_POINT _aPolygonHiRes_src[] = {
  {                     0, COORD_2(0, AA_FACTOR) },
  { COORD_1(1, AA_FACTOR), COORD_0(1, AA_FACTOR) },
  { COORD_1(0, AA_FACTOR), COORD_0(1, AA_FACTOR) }
};

static GUI_POINT _aPolygon[POLY_POINTS];
static GUI_POINT _aPolygonHiRes[POLY_POINTS];

/*******************************************************************
*
*       Static code
*
********************************************************************
*/
/*******************************************************************
*
*       _cbWindow
*
* Function description
*   This is the callback for the window. A callback was used
*   for memory devices.
*/
static void _cbWindow(WM_MESSAGE * pMsg) {
  switch (pMsg->MsgId) {
  case WM_PAINT:
    GUI_SetBkColor(_color_a);
    GUI_ClearRect( 0, 0, 250, 14);
    GUI_SetBkColor(_color_b);
    GUI_ClearRect( 0, 15, 250, 29);
    GUI_SetBkColor(GUI_BLACK);
    GUI_ClearRect( 0, 30, 250, 60);
    GUI_SetColor(_color_c);
    GUI_FillPolygon(_aPolygon, POLY_POINTS, _pos_x1, _pos_y1);
    GUI_AA_FillPolygon(_aPolygon, POLY_POINTS, _pos_x2, _pos_y2);
    GUI_AA_EnableHiRes();
    GUI_AA_FillPolygon(_aPolygonHiRes, POLY_POINTS, _pos_x3, _pos_y3);
    GUI_AA_DisableHiRes();
    break;
  default:
    WM_DefaultProc(pMsg);
  }
}

/*******************************************************************
*
*       _CalcColor
*
* Function description
*   Calculates the color-fading.
*/
static void _CalcColor(void) {
  _color_a += 0x000002 * _color_d;
  _color_b += 0x000200 * _color_d;
  _color_c += 0x020002 * _color_d;
  if (_color_c == 0xFEFFFE || _color_c == 0x00FF00) {
    _color_d = -_color_d;
  }
}

/*******************************************************************
*
*       _ShowHiResPixels
*
* Function description
*   This is frame-function for the callback. It creates the window
*   and handles the rotation of polygons and colors.
*/
static void _ShowHiResPixels(void) {
  const GUI_FONT * FontOld;
  WM_HWIN          hWindow;
  float            pi;
  float            Step;
  float            Angle;
  int              i;

  pi   = 3.1415926f;
  Step = pi / 180;
  GUI_SetBkColor(GUI_BLACK);
  GUI_Clear();
  GUI_SetColor(GUI_WHITE);
  GUI_SetTextAlign(GUI_TA_HCENTER);
  FontOld = GUI_SetFont(&GUI_Font24_ASCII);
  GUI_DispStringAt("AA_HiResPixels - Sample", 160, 5);
  GUI_SetFont(FontOld);
  GUI_SetColor(GUI_RED);
  GUI_DispStringHCenterAt("not\nantialised", 65, 100);
  GUI_SetColor(GUI_GREEN);
  GUI_DispStringHCenterAt("antialised", 160, 100);
  GUI_SetColor(GUI_BLUE);
  GUI_DispStringHCenterAt("antialised\nwith high\nresolution", 255, 100);
  hWindow = WM_CreateWindow(35, 140, 250, 60, WM_CF_SHOW | WM_CF_MEMDEV, _cbWindow, 0);
  WM_SelectWindow(hWindow);
  GUI_AA_SetFactor(AA_FACTOR);
  while (1) {
    for (i = 0, Angle = 0; i < 360; i++) {
      Angle += Step;
      GUI_RotatePolygon(_aPolygonHiRes, _aPolygonHiRes_src, POLY_POINTS, Angle);
      GUI_RotatePolygon(_aPolygon,      _aPolygon_src,      POLY_POINTS, Angle);
      _CalcColor();
      WM_InvalidateWindow(hWindow);
      GUI_Delay(50);
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
  _ShowHiResPixels();
}

/*************************** End of file ****************************/

