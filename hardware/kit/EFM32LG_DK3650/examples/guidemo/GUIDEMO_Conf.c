/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2012  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.14 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with the following terms:

The software has been licensed to Energy Micro AS whose registered office
is situated at  Sandakerveien 118, N-0484 Oslo, NORWAY solely
for  the  purposes  of  creating  libraries  for Energy Micros ARM Cortex-M3, M4F
processor-based  devices,  sublicensed  and distributed  under the terms and
conditions  of  the   End  User  License Agreement supplied by Energy Micro AS. 
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : GUIDEMO_Conf.c
Purpose     : Runtime configurability of the GUIDEMO
----------------------------------------------------------------------
*/
/*********************************************************************
*
*       Includes
*
**********************************************************************
*/
#include "GUI.h"
#include "GUIDEMO.h"
#if GUIDEMO_USE_VNC
  #include "GUI_VNC.h"
#endif

/*********************************************************************
*
*       Routine table
*/
static void (* _apfTest[])(void) = {
#if (SHOW_GUIDEMO_SPEEDOMETER && GUI_SUPPORT_MEMDEV)
  GUIDEMO_Speedometer,
#endif
#if (SHOW_GUIDEMO_ZOOMANDROTATE && GUI_WINSUPPORT && GUI_SUPPORT_MEMDEV)
  GUIDEMO_ZoomAndRotate,
#endif
#if (SHOW_GUIDEMO_SKINNING && GUI_WINSUPPORT && GUI_SUPPORT_MEMDEV)
  GUIDEMO_Skinning,
#endif
#if (SHOW_GUIDEMO_BARGRAPH && GUI_SUPPORT_MEMDEV)
  GUIDEMO_BarGraph,
#endif
#if (SHOW_GUIDEMO_FADING && GUI_SUPPORT_MEMDEV)
  GUIDEMO_Fading,
#endif
#if (SHOW_GUIDEMO_TRANSPARENTDIALOG && GUI_WINSUPPORT && GUI_SUPPORT_MEMDEV)
  GUIDEMO_TransparentDialog,
#endif
#if (SHOW_GUIDEMO_WASHINGMACHINE && GUI_WINSUPPORT && GUI_SUPPORT_MEMDEV)
  GUIDEMO_WashingMachine,
#endif
#if (SHOW_GUIDEMO_ICONVIEW && GUI_WINSUPPORT && GUI_SUPPORT_MEMDEV)
  GUIDEMO_IconView,
#endif
#if (SHOW_GUIDEMO_IMAGEFLOW && GUI_WINSUPPORT && GUI_SUPPORT_MEMDEV)
  GUIDEMO_ImageFlow,
#endif
#if (SHOW_GUIDEMO_TREEVIEW && GUI_WINSUPPORT)
  GUIDEMO_Treeview,
#endif
#if (SHOW_GUIDEMO_LISTVIEW && GUI_WINSUPPORT)
  GUIDEMO_Listview,
#endif
#if SHOW_GUIDEMO_VSCREEN
  GUIDEMO_VScreen,
#endif
#if (SHOW_GUIDEMO_GRAPH && GUI_SUPPORT_MEMDEV)
  GUIDEMO_Graph,
#endif
#if SHOW_GUIDEMO_SPEED
  GUIDEMO_Speed,
#endif
#if SHOW_GUIDEMO_BITMAP
  GUIDEMO_Bitmap,
#endif
#if (SHOW_GUIDEMO_CURSOR && GUI_SUPPORT_CURSOR && GUI_SUPPORT_TOUCH)
  GUIDEMO_Cursor,
#endif
#if SHOW_GUIDEMO_COLORBAR
  GUIDEMO_ColorBar,
#endif
#if (SHOW_GUIDEMO_AUTOMOTIVE && GUI_SUPPORT_MEMDEV)
  GUIDEMO_Automotive,
#endif
  0
};

/*********************************************************************
*
*       GUIDEMO_Config
*/
void GUIDEMO_Config(GUIDEMO_CONFIG * pConfig) {
  pConfig->apFunc   = _apfTest;
  pConfig->NumDemos = GUI_COUNTOF(_apfTest);
  pConfig->Flags    = GUIDEMO_CF_SHOW_SPRITES | GUIDEMO_CF_USE_VNC | GUIDEMO_CF_USE_AUTO_BK;
  #if GUIDEMO_USE_VNC
    pConfig->pGUI_VNC_X_StartServer = GUI_VNC_X_StartServer;
  #endif
}

/*************************** End of file ****************************/
