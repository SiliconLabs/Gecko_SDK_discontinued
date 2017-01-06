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
File        : LCDConf.c
Purpose     : Display controller configuration (single layer)
---------------------------END-OF-HEADER------------------------------
*/

#include <stddef.h>

#include "GUI.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
//
// Color conversion for memory devices to be used
//
#if (GUI_USE_ARGB)
  #define GUICC GUICC_M8888I
#else
  #define GUICC GUICC_8888
#endif

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       LCD_X_Config
*
* Purpose:
*   Called during the initialization process in order to set up the
*   display driver configuration.
*   
*/
void LCD_X_Config(void) {
  GUI_DEVICE * pDevice;
  struct INIT_LAYER {
    const GUI_DEVICE_API * pDriver;
    const LCD_API_COLOR_CONV * pColorConv;
    int xSize, ySize;
    int xPos, yPos;
    int NumScreens;
  } aInitLayer[] = {
    {
      GUIDRV_WIN32, // DisplayDriver
      GUICC,        // ColorMode
      800, 480,     // LayerSize
      0,   0,       // LayerPosition
      1
    },
    {
      GUIDRV_WIN32, // DisplayDriver
      GUICC,        // ColorMode
      300, 100,     // LayerSize
      250, 190,     // LayerPosition
      1
    },
    {
      GUIDRV_WIN32, // DisplayDriver
      GUICC,        // ColorMode
      240, 320,     // LayerSize
      280,  80,     // LayerPosition
      1
    },
  };
  int i;
  
  for (i = 0; i < GUI_COUNTOF(aInitLayer); i++) {
    pDevice = GUI_DEVICE_CreateAndLink(aInitLayer[i].pDriver, aInitLayer[i].pColorConv, 0, i);
    LCD_SetSizeEx (i, aInitLayer[i].xSize, aInitLayer[i].ySize);
    LCD_SetVSizeEx(i, aInitLayer[i].xSize, aInitLayer[i].ySize * aInitLayer[i].NumScreens);
    LCD_SetPosEx(i, aInitLayer[i].xPos, aInitLayer[i].yPos);
    LCD_SetVisEx(i, 1);
  }
}

/*********************************************************************
*
*       LCD_X_DisplayDriver
*
* Purpose:
*   This function is called by the display driver for several purposes.
*   To support the according task the routine needs to be adapted to
*   the display controller. Please note that the commands marked with
*   'optional' are not cogently required and should only be adapted if 
*   the display controller supports these features.
*
* Parameter:
*   LayerIndex - Index of layer to be configured
*   Cmd        - Please refer to the details in the switch statement below
*   pData      - Pointer to a LCD_X_DATA structure
*
* Return Value:
*   < -1 - Error
*     -1 - Command not handled
*      0 - OK
*/
int LCD_X_DisplayDriver(unsigned LayerIndex, unsigned Cmd, void * pData) {
  int r;

  switch (Cmd) {
  //
  // Required
  //
  case LCD_X_INITCONTROLLER: {
    //
    // Called during the initialization process in order to set up the
    // display controller and put it into operation. If the display
    // controller is not initialized by any external routine this needs
    // to be adapted by the customer...
    //
    return 0;
  }
  default:
    r = -1;
  }
  return r;
}

/*************************** End of file ****************************/
