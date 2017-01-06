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
File        : FONT_ShowAllTTFs.c
Purpose     : Demonstrates the use of TTF font files
Requirements: WindowManager - ( )
              MemoryDevices - ( )
              AntiAliasing  - ( )
              VNC-Server    - ( )
              PNG-Library   - ( )
              TrueTypeFonts - (x)

              Can be used in a MS Windows environment only!
----------------------------------------------------------------------
*/

#ifndef SKIP_TEST

#include <windows.h>
#include <stdio.h>
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

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
static unsigned     _aHeight  [] = {16, 20, 32, 48};   // Array of heights used to show text
static GUI_TTF_CS   _aCS      [GUI_COUNTOF(_aHeight)]; // Each GUI font needs its own GUI_TTF_CS structure
static GUI_FONT     _aFont    [GUI_COUNTOF(_aHeight)]; // Array of GUI fonts
static char         _acFamily [200];
static char         _acStyle  [200];

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _ShowText
*/
static void _ShowText(void) {
  int i;

  GUI_Clear();
  i = 0;
  GUI_TTF_GetFamilyName(&_aFont[i], _acFamily, sizeof(_acFamily));
  GUI_TTF_GetStyleName(&_aFont[i],  _acStyle,  sizeof(_acStyle));
  GUI_SetFont(&GUI_Font20_1);
  GUI_DispString(_acFamily);
  GUI_DispString(", ");
  GUI_DispString(_acStyle);
  GUI_DispNextLine();
  GUI_DrawHLine(GUI_GetDispPosY(), 0, 0xfff);
  GUI_SetFont(&_aFont[i]);
  GUI_DispString("abcdefghijklmnopqrstuvwxyz\n");
  GUI_DispString("ABCDEFGHIJKLMNOPQRSTUVWXYZ\n");
  GUI_DispString("123456789.:,;(:*!?')\n");
  GUI_DrawHLine(GUI_GetDispPosY(), 0, 0xfff);
  for (i = 0; i < GUI_COUNTOF(_aHeight); i++) {
    GUI_SetFont(&_aFont[i]);
    GUI_DispString("The quick brown fox jumps over the lazy dog. 1234567890\n");
  }
  GUI_Delay(1000);
}

/*********************************************************************
*
*       _CreateFonts
*/
static int _CreateFonts(const U8 * pData, U32 NumBytes) {
  int i;
  GUI_TTF_DATA TTF_Data;        // Only one GUI_TTF_DATA structure is used per font face

  TTF_Data.pData    = pData;    // Set pointer to file data
  TTF_Data.NumBytes = NumBytes; // Set size of file
  for (i = 0; i < GUI_COUNTOF(_aHeight); i++) {
    //
    // Initialize GUI_TTF_CS members
    //
    _aCS[i].PixelHeight = _aHeight[i];
    _aCS[i].pTTF        = &TTF_Data;
    //
    // Create GUI font
    //
    if (GUI_TTF_CreateFontAA(&_aFont[i], &_aCS[i])) {
      return 1;
    }
  }
  return 0;
}

/*********************************************************************
*
*       _cbFontDemo
*
* Function description
*   The function uses the given pointer to a true type font for creating
*   a couple of GUI fonts and showing the outline of the TTF font.
*
* Parameters:
*   pData    - Location of font file
*   NumBytes - Size of font file
*/
static void _cbFontDemo(const U8 * pData, U32 NumBytes) {
  _CreateFonts(pData, NumBytes); // Create fonts
  _ShowText();                   // Show some text
  GUI_TTF_DestroyCache();        // Clear the TTF cache
}

/*********************************************************************
*
*       _IterateOverAllFiles
*
* Function description
*   The function iterates over all files of the given folder and the
*   given mask, reads the contents of the file and calls the function
*   pfDoSomething() with location and size of file data.
*   Can be used under Win32 environment only.
*/
static void _IterateOverAllFiles(const char * sFolder, const char * sMask, void (* pfDoSomething)(const U8 * pData, U32 NumBytes)) {
  GUI_RECT Rect = {0, 10, 1000, 1000};
  char              acMask[_MAX_PATH];
  char              acFile[_MAX_PATH];
  WIN32_FIND_DATA   Context;
  HANDLE            hFind;
  HANDLE            hFile;
  U8              * pData;
  DWORD             NumBytes;
  DWORD             NumBytesRead;

  sprintf(acMask, "%s\\%s", sFolder, sMask);
  hFind = FindFirstFile(acMask, &Context);
  if (hFind != INVALID_HANDLE_VALUE) {
    do {
      sprintf(acFile, "%s\\%s", sFolder, Context.cFileName);
      hFile = CreateFile(acFile, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
      if (hFile != INVALID_HANDLE_VALUE) {
        NumBytes = GetFileSize(hFile, NULL);
        pData = (U8 *)malloc(NumBytes);
        ReadFile(hFile, pData, NumBytes, &NumBytesRead, NULL);
        pfDoSomething(pData, NumBytes);
        free(pData);
      }
      CloseHandle(hFile);
    } while (FindNextFile(hFind, &Context));
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
  char acPath[200];

  GUI_Init();
  //
  // Check if recommended memory for the sample is available
  //
  if (GUI_ALLOC_GetNumFreeBytes() < RECOMMENDED_MEMORY) {
    GUI_ErrorOut("Not enough memory available."); 
    return;
  }
  //
  // Set colors and text mode
  //
  GUI_SetBkColor(GUI_WHITE);
  GUI_SetColor(GUI_BLACK);
  GUI_SetTextMode(GUI_TM_TRANS);
  //
  // Get windows system directory and extend it with '\Font'
  //
  GetWindowsDirectory(acPath, sizeof(acPath));
  strcat(acPath, "\\Fonts");
  //
  // Iterate over files and call _cbFontDemo for each file
  //
  while (1) {
    _IterateOverAllFiles(acPath, "*.ttf", _cbFontDemo);
  }
}

#endif

/*************************** End of file ****************************/
