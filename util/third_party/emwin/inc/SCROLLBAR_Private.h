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
File        : SCROLLBAR_Private.h
Purpose     : SCROLLBAR internal header file
---------------------------END-OF-HEADER------------------------------
*/

#ifndef SCROLLBAR_PRIVATE_H
#define SCROLLBAR_PRIVATE_H

#include "SCROLLBAR.h"
#include "WIDGET.h"
#include "GUI_Debug.h"

#if GUI_WINSUPPORT

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define PRESSED_STATE_NONE  0
#define PRESSED_STATE_RIGHT 1
#define PRESSED_STATE_LEFT  2
#define PRESSED_STATE_THUMB 3

/*********************************************************************
*
*       Private config defaults
*
**********************************************************************
*/

/* Define colors */
#ifndef   SCROLLBAR_COLOR_SHAFT_DEFAULT
  #define SCROLLBAR_COLOR_SHAFT_DEFAULT GUI_GRAY
#endif

#ifndef   SCROLLBAR_COLOR_ARROW_DEFAULT
  #define SCROLLBAR_COLOR_ARROW_DEFAULT GUI_BLACK
#endif

#ifndef   SCROLLBAR_COLOR_THUMB_DEFAULT
  #define SCROLLBAR_COLOR_THUMB_DEFAULT GUI_GRAY_C0
#endif

#ifndef   SCROLLBAR_THUMB_SIZE_MIN_DEFAULT
  #define SCROLLBAR_THUMB_SIZE_MIN_DEFAULT 4
#endif

#ifndef SCROLLBAR_DEFAULT_WIDTH
  #if   WIDGET_USE_SCHEME_SMALL
    #define SCROLLBAR_DEFAULT_WIDTH 11
  #elif WIDGET_USE_SCHEME_MEDIUM
    #define SCROLLBAR_DEFAULT_WIDTH 16
  #elif WIDGET_USE_SCHEME_LARGE
    #define SCROLLBAR_DEFAULT_WIDTH 22
  #endif
#endif

#define SCROLLBAR_TIMER_ID 1234

/*********************************************************************
*
*       Module internal data
*
**********************************************************************
*/
extern GUI_COLOR  SCROLLBAR__aDefaultBkColor[2];
extern GUI_COLOR  SCROLLBAR__aDefaultColor[2];
extern I16        SCROLLBAR__DefaultWidth;
extern I16        SCROLLBAR__ThumbSizeMin;

/*********************************************************************
*
*       Object definition
*
**********************************************************************
*/
typedef struct {
  WIDGET_DRAW_ITEM_FUNC * pfDrawSkin;
} SCROLLBAR_SKIN_PRIVATE;

typedef struct {
  GUI_COLOR aColor[3];
  SCROLLBAR_SKIN_PRIVATE SkinPrivate;
} SCROLLBAR_PROPS;

typedef struct {
  int x0_LeftArrow;
  int x1_LeftArrow;
  int x0_Thumb;
  int x1_Thumb;
  int x0_RightArrow;
  int x1_RightArrow;
  int x1;
  int xSizeMoveable;
  int ThumbSize;
} SCROLLBAR_POSITIONS;

typedef struct SCROLLBAR_OBJ SCROLLBAR_OBJ;

struct SCROLLBAR_OBJ {
  WIDGET Widget;
  SCROLLBAR_PROPS Props;
  WIDGET_SKIN const * pWidgetSkin;
  void (* pfCalcPositions)(SCROLLBAR_Handle hObj, SCROLLBAR_POSITIONS * pPos);
  int NumItems, v, PageSize;
  int State;
  int TimerStep;
  int TouchPos;
  WM_HMEM hTimer;
};

/*********************************************************************
*
*       Private macros
*
**********************************************************************
*/
#if GUI_DEBUG_LEVEL >= GUI_DEBUG_LEVEL_CHECK_ALL
  #define SCROLLBAR_INIT_ID(p) (p->Widget.DebugId = SCROLLBAR_ID)
#else
  #define SCROLLBAR_INIT_ID(p)
#endif

#if GUI_DEBUG_LEVEL >= GUI_DEBUG_LEVEL_CHECK_ALL
  SCROLLBAR_OBJ * SCROLLBAR_LockH(SCROLLBAR_Handle h);
  #define SCROLLBAR_LOCK_H(h)   SCROLLBAR_LockH(h)
#else
  #define SCROLLBAR_LOCK_H(h)   (SCROLLBAR_OBJ *)GUI_LOCK_H(h)
#endif

/*********************************************************************
*
*       Private functions
*
**********************************************************************
*/
void SCROLLBAR__InvalidatePartner(SCROLLBAR_Handle hObj);
void SCROLLBAR__Rect2VRect       (const WIDGET * pWidget, GUI_RECT * pRect);

/*********************************************************************
*
*       Private data
*
**********************************************************************
*/
extern SCROLLBAR_PROPS SCROLLBAR__DefaultProps;

extern const WIDGET_SKIN SCROLLBAR__SkinClassic;
extern       WIDGET_SKIN SCROLLBAR__Skin;

extern WIDGET_SKIN const * SCROLLBAR__pSkinDefault;

#endif        /* GUI_WINSUPPORT */
#endif        /* Avoid multiple inclusion */
