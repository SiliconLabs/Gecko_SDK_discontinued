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
File        : GUI_FontIntern.h
Purpose     : Internal decalrations used in font files
---------------------------END-OF-HEADER------------------------------

Attention : Do not modify this file ! If you do, you will not
            be able do update to a later GUI version !

*/


#ifndef  GUI_FONTINTERN_H    /* Guard against multiple inclusion */
#define  GUI_FONTINTERN_H

#include "GUI.h"

#if defined(__cplusplus)
extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

extern GUI_CONST_STORAGE GUI_FONT_PROP GUI_Font8ASCII_Prop;
extern GUI_CONST_STORAGE GUI_FONT_PROP GUI_FontF8x13_ASCII_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP GUI_FontF8x15B_ASCII_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP GUI_Font10S_ASCII_FontProp1;
extern GUI_CONST_STORAGE GUI_FONT_PROP GUI_Font10ASCIIProp1;
extern GUI_CONST_STORAGE GUI_FONT_PROP GUI_Font13ASCII_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP GUI_Font13B_ASCII_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP GUI_Font13H_ASCII_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP GUI_Font13HB_ASCII_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP GUI_Font16_1_FontProp1;
extern GUI_CONST_STORAGE GUI_FONT_PROP GUI_Font16ASCIIProp1;
extern GUI_CONST_STORAGE GUI_FONT_PROP GUI_Font16B_ASCII_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP GUI_Font20_ASCII_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP GUI_Font20B_ASCII_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP GUI_Font24_ASCII_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP GUI_Font24B_ASCII_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP GUI_Font32_ASCII_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP GUI_Font32B_ASCII_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP GUI_FontComic18B_ASCII_Prop1;
extern GUI_CONST_STORAGE GUI_FONT_PROP GUI_FontComic24B_ASCII_Prop1;

extern GUI_CONST_STORAGE GUI_CHARINFO GUI_Font16_HK_CharInfo[169];

#if defined(__cplusplus)
  }
#endif


#endif   /* Guard against multiple inclusion */

/*************************** End of file ****************************/
