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
File        : FONT_ShiftJIS.c
Purpose     : Example demonstrating ShiftJIS capabilities of emWin
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
*              Definition of ShiftJIS font
*
********************************************************************
*/
//
// LATIN CAPITAL LETTER A
//
static const unsigned char acFontSJIS13_0041[ 13] = { // Code 0041
  ________,
  ________,
  ___X____,
  ___X____,
  __X_X___,
  __X_X___,
  __X_X___,
  _XXXXX__,
  _X___X__,
  _X___X__,
  XXX_XXX_,
  ________,
  ________};

//
// LATIN CAPITAL LETTER B
//
static const unsigned char acFontSJIS13_0042[ 13] = { // Code 0042
  ________,
  ________,
  XXXXX___,
  _X___X__,
  _X___X__,
  _X___X__,
  _XXXX___,
  _X___X__,
  _X___X__,
  _X___X__,
  XXXXX___,
  ________,
  ________};

//
// LATIN CAPITAL LETTER C
//
static const unsigned char acFontSJIS13_0043[ 13] = { // Code 0043
  ________,
  ________,
  __XX_X__,
  _X__XX__,
  X____X__,
  X_______,
  X_______,
  X_______,
  X_______,
  _X___X__,
  __XXX___,
  ________,
  ________};

//
// KATAKANA LETTER KE
//
static const unsigned char acFontSJIS13_8350[ 26] = { // Code 8350
  __XX____,________,
  ___X____,________,
  ___X____,________,
  ___XXXXX,XXXX____,
  __X____X,________,
  _X_____X,________,
  X______X,________,
  ______X_,________,
  ______X_,________,
  _____X__,________,
  ____X___,________,
  __XX____,________,
  ________,________};

//
// KATAKANA LETTER GE
//
static const unsigned char acFontSJIS13_8351[ 26] = { // Code 8351
  __XX____,X_X_____,
  ___X____,_X_X____,
  ___X____,________,
  __XXXXXX,XXX_____,
  __X____X,________,
  _X_____X,________,
  X______X,________,
  ______X_,________,
  ______X_,________,
  _____X__,________,
  ____X___,________,
  __XX____,________,
  ________,________};

//
// KATAKANA LETTER KO
//
static const unsigned char acFontSJIS13_8352[ 26] = { // Code 8352
  ________,________,
  ________,________,
  __XXXXXX,XX______,
  ________,_X______,
  ________,_X______,
  ________,_X______,
  ________,_X______,
  ________,_X______,
  ________,_X______,
  _XXXXXXX,XXXX____,
  ________,________,
  ________,________,
  ________,________};

static const GUI_CHARINFO GUI_FontSJIS13_CharInfo[6] = {
   {   7,   7,  1, acFontSJIS13_0041 } // Code 0041
  ,{   7,   7,  1, acFontSJIS13_0042 } // Code 0042
  ,{   7,   7,  1, acFontSJIS13_0043 } // Code 0043
  ,{  14,  14,  2, acFontSJIS13_8350 } // Code 8350
  ,{  14,  14,  2, acFontSJIS13_8351 } // Code 8351
  ,{  14,  14,  2, acFontSJIS13_8352 } // Code 8352
};

static const GUI_FONT_PROP GUI_FontSJIS13_Prop2 = {
   0x8350                        // First character
  ,0x8352                        // Last character 
  ,&GUI_FontSJIS13_CharInfo[  3] // Address of first character
  ,0                             // Pointer to next GUI_FONT_PROP
};

static const GUI_FONT_PROP GUI_FontSJIS13_Prop1 = {
   0x0041                        // First character
  ,0x0043                        // Last character 
  ,&GUI_FontSJIS13_CharInfo[  0] // Address of first character
  ,&GUI_FontSJIS13_Prop2 // Pointer to next GUI_FONT_PROP
};

static const GUI_FONT GUI_FontSJIS13 = {
   GUI_FONTTYPE_PROP_SJIS // Type of font   
  ,13                     // Height of font 
  ,13                     // Space of font y
  ,1                      // Magnification x
  ,1                      // Magnification y
  ,{&GUI_FontSJIS13_Prop1}
};

/*******************************************************************
*
*  Definition of string containing ASCII and ShiftJIS characters
*
********************************************************************
*/
static const char aSJIS[] = {
  "ABC\x83\x50\x83\x51\x83\x52\x0"
};

/*******************************************************************
*
*        Demonstrates output of ShiftJIS characters
*
********************************************************************
*/
static void _DemoShiftJIS(void) {
  GUI_SetFont(&GUI_Font24_ASCII);
  GUI_DispStringHCenterAt("FONT_ShiftJIS - Sample", 160, 5);
  //
  // Set ShiftJIS font
  //
  GUI_SetFont(&GUI_FontSJIS13);
  //
  // Display string
  //
  GUI_DispStringHCenterAt(aSJIS, 160, 80);
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
  _DemoShiftJIS();
  while (1) {
    GUI_Delay(100);
  }
}

/*************************** End of file ****************************/
