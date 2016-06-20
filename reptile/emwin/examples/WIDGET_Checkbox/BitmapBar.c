/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2015  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.28 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with the following terms:

The  software has  been licensed  to Energy  Micro AS whose registered
office is  situated at  Sandakerveien 118,  N-0484 Oslo, NORWAY solely
for  the  purposes  of  creating  libraries  for   Energy  Micros  ARM
Cortex-M3,  M4Fb processor-based devices,  sublicensed and distributed
under  the  terms  and conditions of  the End  User License  Agreement
supplied by Energy Micro AS. 
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : BitmapBar.c
Purpose     : Sample bitmap for checkbox 11x11
----------------------------------------------------------------------
*/

#include "GUI.h"
#include "Main.h"

/* Colors */
static const GUI_COLOR _aColorDisabled[] = {0x808080, 0x101010};
static const GUI_COLOR _aColorEnabled[]  = {GUI_WHITE, GUI_BLACK};

/* Palettes */
static const GUI_LOGPALETTE _PalCheckDisabled = {
  2,	/* number of entries */
  1, 	/* transparency flag */
  _aColorDisabled
};

static const GUI_LOGPALETTE _PalCheckEnabled = {
  2,	/* number of entries */
  1, 	/* transparency flag */
  _aColorEnabled
};

/* Pixel data */
static const unsigned char _acBar[] = {
  ________, ________,
  ________, ________,
  ________, ________,
  ________, ________,
  ________, ________,
  __XXXXXX, X_______,
  __XXXXXX, X_______,
  ________, ________,
  ________, ________,
  ________, ________,
  ________, ________,
};

/* Bitmaps */
const GUI_BITMAP _abmBar[2] = {
  { 11, 11, 2, 1, _acBar, &_PalCheckDisabled },
  { 11, 11, 2, 1, _acBar, &_PalCheckEnabled },
};

/*************************** End of file ****************************/
