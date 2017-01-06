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
File        : ANIMATION_Basics.c
Purpose     : Sample showing the basics of using the animation object.
Requirements: WindowManager - ( )
              MemoryDevices - ( )
              AntiAliasing  - ( )
              VNC-Server    - ( )
              PNG-Library   - ( )
              TrueTypeFonts - ( )
----------------------------------------------------------------------
*/

#include <stddef.h>

#include "GUI.h"

/*********************************************************************
*
*       Types
*
**********************************************************************
*/
typedef struct {
  int xSize, ySize;
  int xPart, yPart;
  int xPos, yPos;
  int Dir;
  int Size;
  int ObjectSize;
} ANIM_DATA;

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _CalcPosition
*
*  Purpose:
*    Optional application defined position calculation in dependence
*    of the point of time within the animations timeline.
*/
static I32 _CalcPosition(GUI_TIMER_TIME ts, GUI_TIMER_TIME te, GUI_TIMER_TIME tNow) {
  I32 Result;
  int Diff, Pos, Modulation;

  Diff = te - ts;
  //
  // Linear calculation
  //
  if (Diff) {
    Result = (tNow * GUI_ANIM_RANGE) / (te - ts);
  } else {
    Result = GUI_ANIM_RANGE;
  }
  //
  // Modulation
  //
  Modulation = GUI_ANIM_RANGE >> 3;
  Pos = Result / Modulation;
  Result -= Modulation * Pos;
  if (Pos & 1) {
    Result  = Modulation - Result;
    Result  = (Result * Result) / Modulation;
    Result  = Modulation - Result;
  } else {
    Result  = (Result * Result) / Modulation;
  }
  Result += Modulation * Pos;
  return Result;
}

/*********************************************************************
*
*       _SliceInfo
*
*  Purpose:
*    Called before the first and after the last animation item of one
*    'slice' is drawn. A 'slice' means one or more animation item
*    which need to be drawn at a determined point in time.
*
*    That mechanism is used here to avoid flickering.
*/
static void _SliceInfo(int State, void * pVoid) {
  GUI_USE_PARA(pVoid);
  switch (State) {
  case GUI_ANIM_START:
    GUI_MULTIBUF_Begin();
    break;
  case GUI_ANIM_END:
    GUI_MULTIBUF_End();
    break;
  }
}

/*********************************************************************
*
*       _PrepareDrawing
*
*  Parameters:
*    pInfo   - Animation information passed by emWin to the application
*    pVoid   - Application defined void pointer
*    ppData  - Data (pointer) pointer to application data
*    xPosOld - Position to be used for clearing the display
*    Index   - Index of animation
*/
static void _PrepareDrawing(GUI_ANIM_INFO * pInfo, void * pVoid, ANIM_DATA ** ppData, int xPosOld, int Index) {
  ANIM_DATA * pData;

  //
  // Use custom void pointer for pointing to application defined data structure
  //
  pData = *ppData = (ANIM_DATA *)pVoid;
  //
  // Calculate x-position in dependence of current animation value
  //
  switch (pData->Dir) {
  case +1:
    pData->xPos = pData->xPart + ((pData->xSize - pData->xPart * 2) * pInfo->Pos) / GUI_ANIM_RANGE;
    break;
  case -1:
    pData->xPos = pData->xSize - pData->xPart - 1 - ((pData->xSize - pData->xPart * 2) * pInfo->Pos) / GUI_ANIM_RANGE;
    break;
  }
  //
  // Calculate object size in dependence of position
  //
  pData->ObjectSize = (pData->Size * pInfo->Pos) / GUI_ANIM_RANGE;
  //
  // Calculate y-position in dependence of animation index
  //
  pData->yPos = pData->yPart * (2 * Index + 1);
  //
  // Clears the area of the previous drawing
  //
  if (xPosOld) {
    GUI_ClearRect(xPosOld - pData->yPart, pData->yPos - pData->yPart, xPosOld + pData->yPart, pData->yPos + pData->yPart - 1);
  }
  //
  // Set item color in dependence of animation state
  //
  switch (pInfo->State) {
  case GUI_ANIM_START:
  case GUI_ANIM_END:
    GUI_SetColor(GUI_RED);
    break;
  case GUI_ANIM_RUNNING:
    GUI_SetColor(GUI_GREEN);
    break;
  }
}

/*********************************************************************
*
*       _AnimDrawCircle
*/
static void _AnimDrawCircle(GUI_ANIM_INFO * pInfo, void * pVoid) {
  ANIM_DATA * pData;
  static int xPosOld;

  _PrepareDrawing(pInfo, pVoid, &pData, xPosOld, 0);
  GUI_DrawCircle(pData->xPos, pData->yPos, pData->ObjectSize);
  xPosOld = pData->xPos;
}

/*********************************************************************
*
*       _AnimDrawRect
*/
static void _AnimDrawRect(GUI_ANIM_INFO * pInfo, void * pVoid) {
  ANIM_DATA * pData;
  static int xPosOld;

  _PrepareDrawing(pInfo, pVoid, &pData, xPosOld, 2);
  GUI_DrawRect(pData->xPos - pData->ObjectSize, pData->yPos - pData->ObjectSize, pData->xPos + pData->ObjectSize, pData->yPos + pData->ObjectSize);
  xPosOld = pData->xPos;
}

/*********************************************************************
*
*       _AnimFillCircle
*/
static void _AnimFillCircle(GUI_ANIM_INFO * pInfo, void * pVoid) {
  ANIM_DATA * pData;
  static int xPosOld;

  _PrepareDrawing(pInfo, pVoid, &pData, xPosOld, 1);
  GUI_FillCircle(pData->xPos, pData->yPos, pData->ObjectSize);
  xPosOld = pData->xPos;
}

/*********************************************************************
*
*       _AnimFillRect
*/
static void _AnimFillRect(GUI_ANIM_INFO * pInfo, void * pVoid) {
  ANIM_DATA * pData;
  static int xPosOld;

  _PrepareDrawing(pInfo, pVoid, &pData, xPosOld, 3);
  GUI_FillRect(pData->xPos - pData->ObjectSize, pData->yPos - pData->ObjectSize, pData->xPos + pData->ObjectSize, pData->yPos + pData->ObjectSize);
  xPosOld = pData->xPos;
}

/*********************************************************************
*
*       _AnimDrawCross
*/
static void _AnimDrawCross(GUI_ANIM_INFO * pInfo, void * pVoid) {
  ANIM_DATA * pData;
  static int xPosOld;

  _PrepareDrawing(pInfo, pVoid, &pData, xPosOld, 4);
  GUI_DrawHLine(pData->yPos, pData->xPos - pData->ObjectSize, pData->xPos + pData->ObjectSize);
  GUI_DrawVLine(pData->xPos, pData->yPos - pData->ObjectSize, pData->yPos + pData->ObjectSize);
  xPosOld = pData->xPos;
}

/*********************************************************************
*
*       _AnimDemo
*/
static void _AnimDemo(ANIM_DATA * pData) {
  GUI_ANIM_HANDLE hAnim;

  //
  // Create animation object
  //
  //   Remark: The min time/frame here is 100 to be able to notice
  //           the colors. In a real application this value should
  //           be significantly smaller to ensure a smooth motion.
  //
  //   Slice callback routine --------------+
  //   Custom *void pointer ---------+      |
  //   Minimum time per frame --+    |      |
  //   Duration ----------+     |    |      |
  //                      |     |    |      |
  hAnim = GUI_ANIM_Create(4000, 50, pData, _SliceInfo);
  //
  // Add animation items
  //
  //   Animation routine to be called ------------------------+
  //   Custom *void pointer ---------------------------+      |
  //   Method of position calculation +                |      |
  //   End on timeline ---------+     |                |      |
  //   Start on timeline -+     |     |                |      |
  //                      |     |     |                |      |
  GUI_ANIM_AddItem(hAnim,    0, 2000, ANIM_ACCEL,      pData, _AnimDrawCircle);
  GUI_ANIM_AddItem(hAnim,  500, 2500, ANIM_DECEL,      pData, _AnimDrawRect);
  GUI_ANIM_AddItem(hAnim, 1000, 3000, ANIM_ACCELDECEL, pData, _AnimFillCircle);
  GUI_ANIM_AddItem(hAnim, 1500, 3500, ANIM_LINEAR,     pData, _AnimFillRect);
  //
  // Animation item with custom defined position calculation
  //
  GUI_ANIM_AddItem(hAnim, 2000, 4000, _CalcPosition,   pData, _AnimDrawCross);
  //
  // Start animation
  //
  GUI_ANIM_Start(hAnim);
  //
  // Execute animation until end is reached
  //
  while (GUI_ANIM_Exec(hAnim) == 0) {
    //
    // Idle time for other tasks
    //
    GUI_X_Delay(5);
  }
  //
  // Delete animation object
  //
  GUI_ANIM_Delete(hAnim);
}

/*********************************************************************
*
*       _SetupAnimationData
*/
static void _SetupAnimationData(ANIM_DATA * pData) {
  //
  // Get display size
  //
  pData->xSize = LCD_GetXSize();
  pData->ySize = LCD_GetYSize();
  pData->xPart = pData->xSize / 10;
  pData->yPart = pData->ySize / 10;
  pData->Size  = (pData->yPart * 4) / 5;
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
  ANIM_DATA Data;

  GUI_Init();
  _SetupAnimationData(&Data);
  while (1) {
    for (Data.Dir = 1; Data.Dir >= -1; Data.Dir -= 2) {
      _AnimDemo(&Data);
    }
  }
}

/*************************** End of file ****************************/
