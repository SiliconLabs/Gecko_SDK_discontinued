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
Purpose     : Display controller configuration
---------------------------END-OF-HEADER------------------------------
*/

#include <stddef.h>

#include "GUI.h"
#include "GUIDRV_Lin.h"

/*********************************************************************
*
*       Layer configuration
*
* Purpose:
*   Contains the definitions of size, color conversion, driver and optional
*   the virtual size and the position of each layer.
*
*   Can be easily enhanced for further layers.
*
**********************************************************************
*/
//
// Physical display size
//
#define _XSIZE_PHYS_0 320
#define _YSIZE_PHYS_0 240
#define _XSIZE_PHYS_1 320
#define _YSIZE_PHYS_1 240

//
// Color conversion
//
#define _COLORCONV_0 GUICC_8666
#define _COLORCONV_1 GUICC_8888

//
// Display driver
//
#define _DRIVER_0 GUIDRV_LIN_8
#define _DRIVER_1 GUIDRV_LIN_32

/*********************************************************************
*
*       Generic configuration and checking
*
* Purpose:
*   Checks if the layer configuration is complete and defines the
*   required macros for the layer configuration table.
*
*   Can be easily enhanced for further layers.
*
**********************************************************************
*/
/*********************************************************************
*
*       Layer 0
*/
#if    defined(_XSIZE_PHYS_0)
  #if !defined(_YSIZE_PHYS_0)  \
   || !defined(_COLORCONV_0)   \
   || !defined(_DRIVER_0)
    #error Incomplete layer configuration!
  #else
    #ifndef   _VXSIZE_PHYS_0
      #define _VXSIZE_PHYS_0 _XSIZE_PHYS_0
    #endif
    #ifndef   _VYSIZE_PHYS_0
      #define _VYSIZE_PHYS_0 _YSIZE_PHYS_0
    #endif
    #ifndef   _XPOS_0
      #define _XPOS_0 0
    #endif
    #ifndef   _YPOS_0
      #define _YPOS_0 0
    #endif
    #ifndef   _PALETTE_0
      #define _PALETTE_0 NULL
    #endif
    #define _SCREENSIZE_0   _XSIZE_PHYS_0,  _YSIZE_PHYS_0
    #define _VIRTUALSIZE_0  _VXSIZE_PHYS_0, _VYSIZE_PHYS_0
    #define _CONFIGURATON_0 _SCREENSIZE_0,  _VIRTUALSIZE_0, _XPOS_0, _YPOS_0, _COLORCONV_0
  #endif
#endif

/*********************************************************************
*
*       Layer 1
*/
#if    defined(_XSIZE_PHYS_1)
  #if !defined(_YSIZE_PHYS_1)  \
   || !defined(_COLORCONV_1)   \
   || !defined(_DRIVER_1)
    #error Incomplete layer configuration!
  #else
    #ifndef   _VXSIZE_PHYS_1
      #define _VXSIZE_PHYS_1 _XSIZE_PHYS_1
    #endif
    #ifndef   _VYSIZE_PHYS_1
      #define _VYSIZE_PHYS_1 _YSIZE_PHYS_1
    #endif
    #ifndef   _XPOS_1
      #define _XPOS_1 0
    #endif
    #ifndef   _YPOS_1
      #define _YPOS_1 0
    #endif
    #ifndef   _PALETTE_1
      #define _PALETTE_1 NULL
    #endif
    #define _SCREENSIZE_1   _XSIZE_PHYS_1,  _YSIZE_PHYS_1
    #define _VIRTUALSIZE_1  _VXSIZE_PHYS_1, _VYSIZE_PHYS_1
    #define _CONFIGURATON_1 _SCREENSIZE_1,  _VIRTUALSIZE_1, _XPOS_1, _YPOS_1, _COLORCONV_1
  #endif
#endif

/*********************************************************************
*
*       Types
*
**********************************************************************
*/
/*********************************************************************
*
*       _CONFIG_PARA
*/
typedef struct {
  int xSize, ySize, vxSize, vySize, xPos, yPos;
  const LCD_API_COLOR_CONV * pColorConv;
  const GUI_DEVICE_API     * pDisplayDriver;
  const LCD_PHYSPALETTE    * pPalette;
} _CONFIG_PARA;

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
/*********************************************************************
*
*       Static data: Layer initialization structure
*
**********************************************************************
*/
/*********************************************************************
*
*       _aConfigPara
*
* Purpose:
*   This table contains the complete layer/display configuration and
*   is used by LCD_X_Config().
*
*   Can be easily enhanced for further layers.
*/
static const _CONFIG_PARA _aConfigPara[] = {
  #if defined(_XSIZE_PHYS_0)
    { _CONFIGURATON_0, _DRIVER_0, _PALETTE_0},
  #endif
  #if defined(_XSIZE_PHYS_1)
    { _CONFIGURATON_1, _DRIVER_1, _PALETTE_1},
  #endif
};

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
  int i, BitsPerPixel;
  U32 Addr;

  //
  // Video RAM start address.
  //
  // This address is only required for display drivers with memory
  // mapped video RAM like the LIN_xxx driver. It does not need to
  // be adapted for display controllers with a simple bus interface.
  //
  Addr = 0x12345678;
  //
  // Display driver configuration
  //
  for (i = 0; i < GUI_COUNTOF(_aConfigPara); i++) {
    //
    // Set display driver and color conversion for 1st layer
    //
    GUI_DEVICE_CreateAndLink(_aConfigPara[i].pDisplayDriver, _aConfigPara[i].pColorConv, 0, i);
    //
    // Display driver configuration
    //
    LCD_SetPosEx     (i, _aConfigPara[i].xPos,   _aConfigPara[i].yPos);
    LCD_SetSizeEx    (i, _aConfigPara[i].xSize,  _aConfigPara[i].ySize);
    LCD_SetVSizeEx   (i, _aConfigPara[i].vxSize, _aConfigPara[i].vySize);
    LCD_SetVRAMAddrEx(i, (void *)Addr);
    LCD_SetVisEx     (i, 1);
    //
    // Set user palette data (only required if no fixed palette is used)
    //
    if (_aConfigPara[i].pPalette) {
      LCD_SetLUTEx(i, _aConfigPara[i].pPalette);
    }
    //
    // Calculate video RAM address for the following layer
    //
    BitsPerPixel = LCD_GetBitsPerPixelEx(i);
    Addr += ((_aConfigPara[i].vxSize * BitsPerPixel) * _aConfigPara[i].vySize) / 8;
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
*      0 - Ok
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
    // ...
    return 0;
  }
  case LCD_X_SETVRAMADDR: {
    //
    // Required for setting the address of the video RAM for drivers
    // with memory mapped video RAM which is passed in the 'pVRAM' element of p
    //
    LCD_X_SETVRAMADDR_INFO * p;
    p = (LCD_X_SETVRAMADDR_INFO *)pData;
    //...
    return 0;
  }
  case LCD_X_SETORG: {
    //
    // Required for setting the display origin which is passed in the 'xPos' and 'yPos' element of p
    //
    LCD_X_SETORG_INFO * p;
    p = (LCD_X_SETORG_INFO *)pData;
    //...
    return 0;
  }
  case LCD_X_SETLUTENTRY: {
    //
    // Required for setting a lookup table entry which is passed in the 'Pos' and 'Color' element of p
    //
    LCD_X_SETLUTENTRY_INFO * p;
    p = (LCD_X_SETLUTENTRY_INFO *)pData;
    //...
    return 0;
  }
  case LCD_X_ON: {
    //
    // Required if the display controller should support switching on and off
    //
    return 0;
  }
  case LCD_X_OFF: {
    //
    // Required if the display controller should support switching on and off
    //
    // ...
    return 0;
  }
  default:
    r = -1;
  }
  return r;
}

/*************************** End of file ****************************/
