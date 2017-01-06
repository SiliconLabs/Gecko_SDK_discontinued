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
File        : BUTTON_Private.h
Purpose     : BUTTON private header file
--------------------END-OF-HEADER-------------------------------------
*/

#ifndef BUTTON_PRIVATE_H
#define BUTTON_PRIVATE_H

#include "WM.h"
#include "BUTTON.h"

#if GUI_WINSUPPORT

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#ifndef   BUTTON_3D_MOVE_X
  #define BUTTON_3D_MOVE_X 1
#endif
#ifndef   BUTTON_3D_MOVE_Y
  #define BUTTON_3D_MOVE_Y 1
#endif

/*********************************************************************
*
*       Object definition
*
**********************************************************************
*/
typedef struct {
  WIDGET_DRAW_ITEM_FUNC * pfDrawSkin;
} BUTTON_SKIN_PRIVATE;

typedef struct {
  GUI_COLOR aBkColor[3];
  GUI_COLOR aTextColor[3];
  GUI_COLOR FocusColor;
  GUI_COLOR FrameColor;
  const GUI_FONT * pFont;
  BUTTON_SKIN_PRIVATE SkinPrivate;
  I16 Align;
  I16 xPosText, yPosText;
} BUTTON_PROPS;

typedef struct {
  WIDGET Widget;
  BUTTON_PROPS Props;
  WIDGET_SKIN const * pWidgetSkin;
  WM_HMEM hpText;
  WM_HMEM ahDrawObj[3];
} BUTTON_Obj;

/*********************************************************************
*
*       Macros for internal use
*
**********************************************************************
*/
#if GUI_DEBUG_LEVEL >= GUI_DEBUG_LEVEL_CHECK_ALL
  #define BUTTON_INIT_ID(p) (p->Widget.DebugId = BUTTON_ID)
#else
  #define BUTTON_INIT_ID(p)
#endif

#if GUI_DEBUG_LEVEL >= GUI_DEBUG_LEVEL_CHECK_ALL
  BUTTON_Obj * BUTTON_LockH(BUTTON_Handle h);
  #define BUTTON_LOCK_H(h)   BUTTON_LockH(h)
#else
  #define BUTTON_LOCK_H(h)   (BUTTON_Obj *)GUI_LOCK_H(h)
#endif

/*********************************************************************
*
*       Public data (internal defaults)
*
**********************************************************************
*/
extern BUTTON_PROPS BUTTON__DefaultProps;

extern const WIDGET_SKIN BUTTON__SkinClassic;
extern       WIDGET_SKIN BUTTON__Skin;

extern WIDGET_SKIN const * BUTTON__pSkinDefault;

/*********************************************************************
*
*       Private functions
*
**********************************************************************
*/
void BUTTON__SetDrawObj(BUTTON_Handle hObj, int Index, GUI_DRAW_HANDLE hDrawObj);


#endif   /* GUI_WINSUPPORT */
#endif   /* BUTTON_H */
