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
File        : DIALOG_FS_ChooseFile.c
Purpose     : Shows how to make use of the CHOOSEFILE dialog.
              It shows the dialog with and without skinning.
              The button bar is shown at the bottom and at the top
              of the window.
Requirements: WindowManager - (x)
              MemoryDevices - (x)
              AntiAliasing  - ( )
              VNC-Server    - ( )
              PNG-Library   - ( )
              TrueTypeFonts - ( )

              Requires eithar a MS Windows environment or emFile!
----------------------------------------------------------------------
*/

#ifndef SKIP_TEST

#include "DIALOG.h"
#include "MESSAGEBOX.h"

#ifdef WIN32
  #include <windows.h>
#else
  #include "BSP.h"
  #include "FS.h"
  #include "RTOS.h"
#endif

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#ifndef _MAX_PATH
  #define _MAX_PATH 256
#else
  #if (_MAX_PATH > 256)
    #undef  _MAX_PATH
    #define _MAX_PATH 256
  #endif
#endif

#ifndef _MAX_EXT
  #define _MAX_EXT  256
#endif

//
// Recommended memory to run the sample with adequate performance
//
#define RECOMMENDED_MEMORY (1024L * 900)

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
static struct {
  U32 Mask;
  char c;
} _aAttrib[] = {
#ifdef WIN32
  { FILE_ATTRIBUTE_READONLY , 'R' },
  { FILE_ATTRIBUTE_HIDDEN   , 'H' },
  { FILE_ATTRIBUTE_SYSTEM   , 'S' },
  { FILE_ATTRIBUTE_DIRECTORY, 'D' },
  { FILE_ATTRIBUTE_ARCHIVE  , 'A' }
#else
  { FS_ATTR_READ_ONLY, 'R' },
  { FS_ATTR_HIDDEN   , 'H' },
  { FS_ATTR_SYSTEM   , 'S' },
  { FS_ATTR_DIRECTORY, 'D' },
  { FS_ATTR_ARCHIVE  , 'A' },
#endif
};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _GetExt
*/
#ifndef WIN32
static void _GetExt(char * pFile, char * pExt) {
  int Len;
  int i;
  int j;

  //
  // Search beginning of extension
  //
  Len = strlen(pFile);
  for (i = Len; i > 0; i--) {
    if (*(pFile + i) == '.') {
      *(pFile + i) = '\0';     // Cut extension from filename
      break;
    }
  }
  //
  // Copy extension
  //
  j = 0;
  while (*(pFile + ++i) != '\0') {
    *(pExt + j) = *(pFile + i);
    j++;
  }
  *(pExt + j) = '\0';          // Set end of string
}
#endif

/*********************************************************************
*
*       _GetData
*/
static int _GetData(CHOOSEFILE_INFO * pInfo) {
#ifdef WIN32
  static HANDLE hFind;
  static int NewDir;
  static char acDrive [_MAX_DRIVE];
  static char acDir   [_MAX_DIR];
  static char acName  [_MAX_FNAME];
  static char acExt   [_MAX_EXT];
  static char acMask  [_MAX_PATH];
  static char acPath  [_MAX_PATH];
  static char acAttrib[10] = {0};
  WIN32_FIND_DATA Context;
  int i, r;
  char c;

  switch (pInfo->Cmd) {
  case CHOOSEFILE_FINDFIRST:
    if (hFind != 0) {
      FindClose(hFind);
    }
    //
    // Split path into drive and directory
    //
    _splitpath(pInfo->pRoot, acDrive, acDir, NULL, NULL);
    NewDir = 1;
    //
    // Do not 'break' here...
    //
  case CHOOSEFILE_FINDNEXT:
    if (NewDir) {
      _makepath(acMask, acDrive, acDir, NULL, NULL);
      strcat(acMask, pInfo->pMask);
      hFind = FindFirstFile(acMask, &Context);
      if (hFind == INVALID_HANDLE_VALUE) {
        FindClose(hFind);
        hFind = 0;
        return 1;
      }
    } else {
      r = FindNextFile(hFind, &Context);
      if (r == 0) {
        FindClose(hFind);
        hFind = 0;
        return 1;
      }
    }
    NewDir = 0;
    //
    // Generate attribute string (pInfo->pAttrib)
    //
    for (i = 0; i < GUI_COUNTOF(_aAttrib); i++) {
      c = (Context.dwFileAttributes & _aAttrib[i].Mask) ? _aAttrib[i].c : '-';
      acAttrib[i] = c;
    }
    //
    // Make name and extension (pInfo->pName, pInfo->pExt)
    //
    if ((Context.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
      _splitpath(Context.cFileName, NULL, NULL, acName, acExt);
    } else {
      strcpy(acName, Context.cFileName);
      acExt[0] = 0;
    }
    //
    // Pass data to dialog
    //
    pInfo->pAttrib = acAttrib;
    pInfo->pName   = acName;
    pInfo->pExt    = acExt;
    pInfo->SizeL   = Context.nFileSizeLow;
    pInfo->SizeH   = Context.nFileSizeHigh;
    pInfo->Flags   = (Context.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
                     ? CHOOSEFILE_FLAG_DIRECTORY : 0;
  }
  return 0;
#else
  static FS_FIND_DATA FindData;
  static char         acAttrib[10]         = { 0 };
  static char         acFile  [_MAX_PATH];
  static char         acExt   [_MAX_EXT];
  char                c;
  int                 i;
  int                 r;

  switch (pInfo->Cmd) {
  case CHOOSEFILE_FINDFIRST:
    r = FS_FindFirstFile(&FindData, pInfo->pRoot, acFile, sizeof(acFile));
    break;
  case CHOOSEFILE_FINDNEXT:
    r = FS_FindNextFile(&FindData) ^ 1;
    break;
  }
  if (r == 0) {
    pInfo->Flags = (FindData.Attributes & _aAttrib[3].Mask) ? CHOOSEFILE_FLAG_DIRECTORY : 0;
    for (i = 0; i < GUI_COUNTOF(_aAttrib); i++) {
      if (FindData.Attributes & _aAttrib[i].Mask) {
        c = _aAttrib[i].c;
      } else {
        c = '-';
      }
      acAttrib[i] = c;
    }
    _GetExt(acFile, acExt);
    pInfo->pAttrib = acAttrib;
    pInfo->pName   = acFile;
    pInfo->pExt    = acExt;
    pInfo->SizeL   = FindData.FileSize;
    pInfo->SizeH   = 0;
  }
  return r;
#endif
}

/*********************************************************************
*
*       _EnableSkinning
*/
static void _EnableSkinning(int OnOff) {
  if (OnOff) {
    FRAMEWIN_SetDefaultSkin(FRAMEWIN_SKIN_FLEX);
    BUTTON_SetDefaultSkin  (BUTTON_SKIN_FLEX);
    DROPDOWN_SetDefaultSkin(DROPDOWN_SKIN_FLEX);
    HEADER_SetDefaultSkin(HEADER_SKIN_FLEX);
    SCROLLBAR_SetDefaultSkin(SCROLLBAR_SKIN_FLEX);
  } else {
    FRAMEWIN_SetDefaultSkinClassic();
    BUTTON_SetDefaultSkinClassic();
    DROPDOWN_SetDefaultSkinClassic();
    HEADER_SetDefaultSkinClassic();
    SCROLLBAR_SetDefaultSkinClassic();
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
  const char        acMask[]            = "*.*";
  CHOOSEFILE_INFO   Info                = { 0 };
  WM_HWIN           hWin;
  int               xSize;
  int               ySize;
  int               Cnt                 = 0;
  int               r;
#ifdef WIN32
  char              acDirLocal[]        = "C:\\";
  char              acDirWin[_MAX_PATH] = "\0";
  char              acDirTmp[_MAX_PATH] = "\0";
  char              acDirSys[_MAX_PATH] = "\0";
  char            * apDrives[4]         = { acDirLocal, acDirWin, acDirTmp, acDirSys };
#else
  char              acDir[_MAX_PATH]    = "mmc:";
  char const      * apDrives[1]         = { acDir };
#endif

#ifndef WIN32
  //
  // Initialize emFile
  //
  FS_Init();
  FS_FAT_SupportLFN();
#else
  //
  // Initialize directories
  //
  GetWindowsDirectory(acDirWin, sizeof(acDirWin));
  GetTempPath        (sizeof(acDirTmp), acDirTmp);
  GetSystemDirectory (acDirSys, sizeof(acDirSys));
#endif
  //
  // Initialize emWin
  //
  WM_SetCreateFlags(WM_CF_MEMDEV);
  GUI_Init();
  //
  // Check if recommended memory for the sample is available
  //
  if (GUI_ALLOC_GetNumFreeBytes() < RECOMMENDED_MEMORY) {
    GUI_ErrorOut("Not enough memory available."); 
    return;
  }
  WM_SetDesktopColor(GUI_WHITE);
  //
  // Set defaults for FRAMEWIN widget
  //
  FRAMEWIN_SetDefaultFont(GUI_FONT_20_ASCII);
  FRAMEWIN_SetDefaultTextAlign(GUI_TA_HCENTER);
  //
  // Get display size and initialize pointers to search mask and callback routine
  //
  xSize = LCD_GetXSize();
  ySize = LCD_GetYSize();
  Info.pfGetData = _GetData;
  Info.pMask     = acMask;
  while (1) {
    _EnableSkinning(((Cnt >> 2) % 2) ^ 1);
    CHOOSEFILE_SetTopMode(Cnt % 2);
    hWin = CHOOSEFILE_Create(WM_HBKWIN, 0, 0, xSize, ySize, apDrives, GUI_COUNTOF(apDrives), 0, "File Dialog", 0, &Info);
    //
    // Show different variants of dialog
    //
    if (((Cnt >> 1) % 2) == 0) {
      CHOOSEFILE_SetButtonText(hWin, CHOOSEFILE_BI_OK    , "OK");
      CHOOSEFILE_SetButtonText(hWin, CHOOSEFILE_BI_CANCEL, "Cancel");
      if (((Cnt >> 2) % 2) == 0) {
        CHOOSEFILE_SetButtonText(hWin, CHOOSEFILE_BI_UP    , "UP");
      }
    }
    r = GUI_ExecCreatedDialog(hWin);
    if (r == 0) {
      hWin = MESSAGEBOX_Create(Info.pRoot, "File:", 0);
      GUI_Delay(2000);
      WM_DeleteWindow(hWin);
    } else {
      GUI_Delay(500);
    }
    Cnt++;
  }
}

#endif

/*************************** End of file ****************************/
