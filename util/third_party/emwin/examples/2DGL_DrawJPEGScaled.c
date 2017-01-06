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
File        : 2DGL_DrawJPEGScaled.c
Purpose     : Sample for scaled drawing JPEG files from external memory.
Requirements: WindowManager - ( )
              MemoryDevices - ( )
              AntiAliasing  - ( )
              VNC-Server    - ( )
              PNG-Library   - ( )
              TrueTypeFonts - ( )

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
#define FRAMED_SIZE_Y 20
#define TITLE_HEIGHT  40
#define BORDER_SIZE   5
#define YPOS_IMAGE    73

//
// Recommended memory to run the sample with adequate performance
//
#define RECOMMENDED_MEMORY (1024L * 200)

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
static U8 _acBuffer[0x200];

/*********************************************************************
*
*       Static functions
*
**********************************************************************
*/
/*********************************************************************
*
*       _GetData
*
* Function description
*   This routine is called by GUI_JPEG_DrawEx(). The routine is responsible
*   for setting the data pointer to a valid data location with at least
*   one valid byte.
*
* Parameters:
*   p           - Pointer to application defined data.
*   NumBytesReq - Number of bytes requested.
*   ppData      - Pointer to data pointer. This pointer should be set to
*                 a valid location.
*   StartOfFile - If this flag is 1, the data pointer should be set to the
*                 beginning of the data stream.
*
* Return value:
*   Number of data bytes available.
*/
static int _GetData(void * p, const U8 ** ppData, unsigned NumBytesReq, U32 Off) {
  HANDLE * phFile;
  DWORD    NumBytesRead;

  phFile = (HANDLE *)p;
  //
  // Check buffer size
  //
  if (NumBytesReq > sizeof(_acBuffer)) {
    NumBytesReq = sizeof(_acBuffer);
  }
  //
  // Set file pointer to the required position
  //
  SetFilePointer(*phFile, Off, 0, FILE_BEGIN);
  //
  // Read data into buffer
  //
  ReadFile(*phFile, _acBuffer, NumBytesReq, &NumBytesRead, NULL);
  //
  // Set data pointer to the beginning of the buffer
  //
  *ppData = _acBuffer;
  //
  // Return number of available bytes
  //
  return NumBytesRead;
}

/*********************************************************************
*
*       _DrawJPEGs
*
* Function description
*   Draws the given JPEG image.
*/
static void _DrawJPEGs(const char * sFileName) {
  const char    acError[] = "There is possibly not enough memory to display this JPEG image.\n\nPlease assign more memory to emWin in GUIConf.c.";
  GUI_JPEG_INFO Info;
  GUI_RECT      Rect;
  HANDLE        hFile;
  int           xSizeScreen, ySizeScreen;
  int           xSizeScale,  ySizeScale;
  int           xSize,       ySize;
  int           xPos,        yPos;
  int           xNum,        yNum;
  int           Num;
  int           i;
  int           r;

  xSizeScreen = LCD_GetXSize();
  ySizeScreen = LCD_GetYSize();
  //
  // Display file name.
  //
  Rect.x0 = BORDER_SIZE;
  Rect.y0 = TITLE_HEIGHT;
  Rect.x1 = xSizeScreen - BORDER_SIZE - 1;
  Rect.y1 = YPOS_IMAGE - 1;
  GUI_ClearRectEx(&Rect);
  GUI_SetFont(&GUI_Font8x16);
  GUI_DispStringInRectWrap(sFileName, &Rect, GUI_TA_HCENTER | GUI_TA_VCENTER, GUI_WRAPMODE_CHAR);
  GUI_SetTextMode(GUI_TM_TRANS);
  GUI_SetFont(&GUI_Font20F_ASCII);
  //
  // Display each image scaled to 3 different sizes.
  //
  hFile = CreateFile(sFileName, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
  GUI_JPEG_GetInfoEx(_GetData, &hFile, &Info);
  for (i = 0; i < 3; i++) {
    //
    // Clear the area in which the JPEG files are displayed and set clipping rectangle.
    //
    Rect.x0 = BORDER_SIZE;
    Rect.y0 = YPOS_IMAGE;
    Rect.x1 = xSizeScreen - BORDER_SIZE - 1;
    Rect.y1 = ySizeScreen - BORDER_SIZE - 1;
    GUI_ClearRectEx(&Rect);
    GUI_SetClipRect(&Rect);
    //
    // Set scaling sizes.
    //
    switch (i) {
    case 0:
      xSizeScale = (xSizeScreen - BORDER_SIZE * 2)          / 2;
      ySizeScale = (ySizeScreen - BORDER_SIZE + YPOS_IMAGE) / 2;
      break;
    case 2:
      xSizeScale = (xSizeScreen - BORDER_SIZE * 2)          * 2;
      ySizeScale = (ySizeScreen - BORDER_SIZE + YPOS_IMAGE) * 2;
      break;
    default:
      xSizeScale = xSizeScreen - BORDER_SIZE * 2;
      ySizeScale = ySizeScreen - BORDER_SIZE + YPOS_IMAGE;
      break;
    }
    //
    // Calculate numerator and denominator for scaling and draw the image.
    //
    xNum = xSizeScale * 1000 / Info.XSize;
    yNum = ySizeScale * 1000 / Info.YSize;
    if (xNum < yNum) {
      Num = xNum;
    } else {
      Num = yNum;
    }
    xSize = Info.XSize * Num / 1000;
    ySize = Info.YSize * Num / 1000;
    xPos  = BORDER_SIZE + (xSizeScreen - BORDER_SIZE * 2 - xSize) / 2;
    yPos  = YPOS_IMAGE  + (ySizeScreen - YPOS_IMAGE      - ySize) / 2;
    r = GUI_JPEG_DrawScaledEx(_GetData, &hFile, xPos, yPos, Num, 1000);
    if (r) {
      //
      // The image could not be displayed successfully. Show an error message.
      //
      GUI_DispStringInRectWrap(acError, &Rect, GUI_TA_HCENTER | GUI_TA_VCENTER, GUI_WRAPMODE_WORD);
      GUI_Delay(2000);
      break;
    }
    //
    // Provide scaling information to the user.
    //
    GUI_SetClipRect(NULL);
    Rect.y0 = Rect.y1 - FRAMED_SIZE_Y * 2;
    switch (i) {
    case 0:
      GUI_DispStringInRectWrap("Scaled to 1/4 of the display area.",  &Rect, GUI_TA_BOTTOM | GUI_TA_LEFT, GUI_WRAPMODE_WORD);
      break;
    case 1:
      GUI_DispStringInRectWrap("Scaled to fit the display area.",     &Rect, GUI_TA_BOTTOM | GUI_TA_LEFT, GUI_WRAPMODE_WORD);
      break;
    case 2:
      GUI_DispStringInRectWrap("Scaled to 4 times the display area.", &Rect, GUI_TA_BOTTOM | GUI_TA_LEFT, GUI_WRAPMODE_WORD);
      break;
    }
    GUI_Delay(2000);
  }
  GUI_SetTextMode(GUI_TM_NORMAL);
  GUI_SetFont(&GUI_Font8x16);
  CloseHandle(hFile);
}

/*********************************************************************
*
*       _GetFirstBitmapDirectory
*
* Function description
*   Returns the first directory which contains one or more JPG files.
*/
static int _GetFirstBitmapDirectory(char * pDir, char * pBuffer) {
  WIN32_FIND_DATA Context;
  HANDLE          hFind;
  char            acMask[_MAX_PATH];
  char            acPath[_MAX_PATH];

  sprintf(acMask, "%s\\*.jpg", pDir);
  hFind = FindFirstFile(acMask, &Context);
  if (hFind != INVALID_HANDLE_VALUE) {
    sprintf(pBuffer, "%s\\", pDir);
    return 1;
  }
  sprintf(acMask, "%s\\*.", pDir);
  hFind = FindFirstFile(acMask, &Context);
  if (hFind != INVALID_HANDLE_VALUE) {
    do {
      if ((strcmp(Context.cFileName, ".") != 0) && (strcmp(Context.cFileName, "..") != 0)) {
        if (Context.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
          sprintf(acPath, "%s\\%s", pDir, Context.cFileName);
          if (_GetFirstBitmapDirectory(acPath, pBuffer)) {
            return 1;
          }
        }
      }
    } while (FindNextFile(hFind, &Context));
  }
  return 0;
}

/*******************************************************************
*
*       _DrawJPEGsFromWindowsDir
*
* Function description
*   Iterates over all JPEG files in a Windows sub folder.
*/
static void _DrawJPEGsFromWindowsDir(void) {
  WIN32_FIND_DATA Context;
  HANDLE          hFind;
  char            acPath[_MAX_PATH];
  char            acMask[_MAX_PATH];
  char            acFile[_MAX_PATH];
  char            acBuffer[_MAX_PATH];
  int             xSize;

  xSize = LCD_GetXSize();
  GUI_SetBkColor(GUI_BLACK);
  GUI_Clear();
  GUI_SetColor(GUI_WHITE);
  GUI_SetFont(&GUI_Font24_ASCII);
  GUI_DispStringHCenterAt("Draw JPEG Scaled - Sample", xSize / 2, 5);
  GetWindowsDirectory(acBuffer, sizeof(acBuffer));
  _GetFirstBitmapDirectory(acBuffer, acPath);
  sprintf(acMask, "%s*.jp*", acPath);
  hFind = FindFirstFile(acMask, &Context);
  if (hFind != INVALID_HANDLE_VALUE) {
    do {
      sprintf(acFile, "%s%s", acPath, Context.cFileName);
      _DrawJPEGs(acFile);
    } while (FindNextFile(hFind, &Context));
  } else {
    GUI_DispStringHCenterAt("No JPEG files found!", 160, 60);
    GUI_Delay(2000);
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
  while (1) {
    _DrawJPEGsFromWindowsDir();
  }
}

#endif

/*************************** End of file ****************************/
