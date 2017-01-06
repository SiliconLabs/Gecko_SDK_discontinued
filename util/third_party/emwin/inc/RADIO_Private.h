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
File        : RADIO_Private.h
Purpose     : RADIO private header file
--------------------END-OF-HEADER-------------------------------------
*/

#ifndef RADIO_PRIVATE_H
#define RADIO_PRIVATE_H

#include "WM.h"

#if GUI_WINSUPPORT

#include "RADIO.h"
#include "WIDGET.h"
#include "GUI_ARRAY.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
/* Define default image inactiv */
#ifndef   RADIO_IMAGE0_DEFAULT
  #define RADIO_IMAGE0_DEFAULT        &RADIO__abmRadio[0]
#endif

/* Define default image activ */
#ifndef   RADIO_IMAGE1_DEFAULT
  #define RADIO_IMAGE1_DEFAULT        &RADIO__abmRadio[1]
#endif

/* Define default image check */
#ifndef   RADIO_IMAGE_CHECK_DEFAULT
  #define RADIO_IMAGE_CHECK_DEFAULT   &RADIO__bmCheck
#endif

/* Define default font */
#ifndef   RADIO_FONT_DEFAULT
  #if   WIDGET_USE_SCHEME_SMALL
    #define RADIO_SPACING_DEFAULT 20
    #define RADIO_FONT_DEFAULT &GUI_Font13_1
  #elif WIDGET_USE_SCHEME_MEDIUM
    #define RADIO_SPACING_DEFAULT 24
    #define RADIO_FONT_DEFAULT &GUI_Font16_1
  #elif WIDGET_USE_SCHEME_LARGE
    #define RADIO_SPACING_DEFAULT 30
    #define RADIO_FONT_DEFAULT &GUI_Font24_1
  #endif
#endif

/* Define vertical default spacing */
#ifndef   RADIO_SPACING_DEFAULT
  #define RADIO_SPACING_DEFAULT 20
#endif

/* Define default text color */
#ifndef   RADIO_DEFAULT_TEXT_COLOR
  #define RADIO_DEFAULT_TEXT_COLOR    GUI_BLACK
#endif

/* Define default background color */
#ifndef   RADIO_DEFAULT_BKCOLOR
  #define RADIO_DEFAULT_BKCOLOR       GUI_GRAY_C0
#endif

#ifndef   RADIO_FOCUSCOLOR_DEFAULT
  #define RADIO_FOCUSCOLOR_DEFAULT    GUI_BLACK
#endif

#define RADIO_BORDER                  2

/*********************************************************************
*
*       Object definition
*
**********************************************************************
*/
typedef struct {
  unsigned (* pfGetButtonSize)(RADIO_Handle hObj);
  WIDGET_DRAW_ITEM_FUNC * pfDrawSkin;
} RADIO_SKIN_PRIVATE;

typedef struct {
  GUI_COLOR BkColor;
  GUI_COLOR TextColor;
  GUI_COLOR FocusColor;
  const GUI_FONT * pFont;
  const GUI_BITMAP * apBmRadio[2];
  const GUI_BITMAP * pBmCheck;
  RADIO_SKIN_PRIVATE SkinPrivate;
} RADIO_PROPS;

typedef struct {
  WIDGET Widget;
  RADIO_PROPS Props;
  WIDGET_SKIN const * pWidgetSkin;
  GUI_ARRAY TextArray;
  I16 Sel;                   /* current selection */
  U16 Spacing;
  U16 NumItems;
  U8  GroupId;
} RADIO_Obj;

/*********************************************************************
*
*       Macros for internal use
*
**********************************************************************
*/
#if GUI_DEBUG_LEVEL >= GUI_DEBUG_LEVEL_CHECK_ALL
  #define RADIO_INIT_ID(p) p->Widget.DebugId = RADIO_ID
#else
  #define RADIO_INIT_ID(p)
#endif

#if GUI_DEBUG_LEVEL >= GUI_DEBUG_LEVEL_CHECK_ALL
  RADIO_Obj * RADIO_LockH(RADIO_Handle h);
  #define RADIO_LOCK_H(h)   RADIO_LockH(h)
#else
  #define RADIO_LOCK_H(h)   (RADIO_Obj *)GUI_LOCK_H(h)
#endif

/*********************************************************************
*
*       Types
*
**********************************************************************
*/

typedef void tRADIO_SetValue(RADIO_Handle hObj, int v);

/*********************************************************************
*
*       Extern data
*
**********************************************************************
*/

extern RADIO_PROPS RADIO__DefaultProps;

extern const WIDGET_SKIN RADIO__SkinClassic;
extern       WIDGET_SKIN RADIO__Skin;

extern WIDGET_SKIN const * RADIO__pSkinDefault;

extern const GUI_BITMAP             RADIO__abmRadio[2];
extern const GUI_BITMAP             RADIO__bmCheck;
extern       tRADIO_SetValue*       RADIO__pfHandleSetValue;

/*********************************************************************
*
*       public functions (internal)
*
**********************************************************************
*/
void     RADIO__SetValue     (RADIO_Handle hObj, int v);
unsigned RADIO__GetButtonSize(RADIO_Handle hObj);

#endif   /* GUI_WINSUPPORT */
#endif   /* RADIO_PRIVATE_H */

/************************* end of file ******************************/
