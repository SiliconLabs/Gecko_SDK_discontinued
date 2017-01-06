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

#include "GUI.h"

#include "string.h"
#include "bsp.h"
#include "em_cmu.h"
#include "em_ebi.h"
#include "bsp_trace.h"
#include "touch.h"
#include "LCDConf.h"
#include "tftdirect.h"
#include "GUIDRV_Lin.h"

/* Application specific defines */

/* Physical display size */
#define LCD_WIDTH   320
#define LCD_HEIGHT  240

/* 16-bit color per pixel */
#define BYTES_PER_PIXEL 2

/* The size in bytes of a full frame buffer */
#define FRAME_BUFFER_SIZE (LCD_WIDTH * LCD_HEIGHT * BYTES_PER_PIXEL)

/* Start address of video memory points to beginning of PSRAM block */
#define VRAM_ADDR_START  EBI_BankAddress(EBI_BANK2)

/* Use triple buffering */
#define NUM_BUFFERS  3

/*
 * Index of a pending (ready to be shown) frame. When no frame is
 * pending this value will be -1
 */
volatile int pendingFrameIndex = -1;

/*********************************************************************
* Configure the Direct Drive Controller
**********************************************************************
*/
static const EBI_TFTInit_TypeDef tftInit =
{
  ebiTFTBank2,                  /* Select EBI Bank 2 */
  ebiTFTWidthHalfWord,          /* Select 2-byte (16-bit RGB565) increments */
  ebiTFTColorSrcMem,            /* Use memory as source for mask/blending */
  ebiTFTInterleaveOnePerDClk,   /* Allow 1 interleaved access per EBI_DCLK period */
  ebiTFTFrameBufTriggerVSync,   /* VSYNC as frame buffer update trigger */
  false,                        /* Drive DCLK from negative edge of internal clock */
  ebiTFTMBDisabled,             /* No masking and alpha blending enabled */
  ebiTFTDDModeExternal,         /* Drive from external memory */
  ebiActiveLow,                 /* CS Active Low polarity */
  ebiActiveHigh,                /* DCLK Active High polarity */
  ebiActiveLow,                 /* DATAEN Active Low polarity */
  ebiActiveLow,                 /* HSYNC Active Low polarity */
  ebiActiveLow,                 /* VSYNC Active Low polarity */
  320,                          /* Horizontal size in pixels */
  1,                            /* Horizontal Front Porch */
  30,                           /* Horizontal Back Porch */
  2,                            /* Horizontal Synchronization Pulse Width */
  240,                          /* Vertical size in pixels */
  1,                            /* Vertical Front Porch */
  4,                            /* Vertical Back Porch */
  2,                            /* Vertical Synchronization Pulse Width */
  0x0000,                       /* Frame Address pointer offset to EBI memory base */
  4,                            /* DCLK Period */
  0,                            /* DCLK Start cycles */
  0,                            /* DCLK Setup cycles */
  0,                            /* DCLK Hold cycles */
};



/* VSYNC interrupt handler. This performs the actual flipping
 * of the buffers.
 */
void EBI_IRQHandler(void) {
  uint32_t flags;

   flags = EBI_IntGet();
  EBI_IntClear(flags);

  if ( flags & EBI_IF_VSYNC ) {
    if ( pendingFrameIndex >= 0 ) {

      /* Set Direct Drive frame buffer address to the new frame */
      EBI_TFTFrameBaseSet(FRAME_BUFFER_SIZE * pendingFrameIndex);

      /* Send a confirmation to emWin that the old back buffer is visible now */
      GUI_MULTIBUF_Confirm(pendingFrameIndex);

      /* Reset the pending flag */
      pendingFrameIndex = -1;
    }
  }
}

static void copyBuffer(int layerIndex, int srcFrameIndex, int dstFrameIndex)
{
  (void) layerIndex;
  (void) srcFrameIndex;
  (void) dstFrameIndex;

  /* This implementation is empty to skip the copy operation on frame buffer
   * flip. The default operation is a memcpy operation, which both
   * requires much CPU and bus time.
   */
}

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

  /* Enable triple buffering */
  GUI_MULTIBUF_Config(NUM_BUFFERS);

  /* Set display driver and color conversion */
  GUI_DEVICE_CreateAndLink(GUIDRV_LIN_16, GUICC_M565, 0, 0);

  /* Set custom callback function for frame copy operation */
  LCD_SetDevFunc(0, LCD_DEVFUNC_COPYBUFFER, (void (*)())copyBuffer);

  /* Display driver configuration, required for Lin-driver */
  if (LCD_GetSwapXY()) {
    LCD_SetSizeEx (0, LCD_HEIGHT, LCD_WIDTH);
    LCD_SetVSizeEx(0, LCD_HEIGHT, LCD_WIDTH);
  } else {
    LCD_SetSizeEx (0, LCD_WIDTH, LCD_HEIGHT);
    LCD_SetVSizeEx(0, LCD_WIDTH, LCD_HEIGHT);
  }

  /* Point video memory to start of PSRAM, drawing will start at frame 0 */
  LCD_SetVRAMAddrEx(0, (void *)VRAM_ADDR_START);
}

void LCD_X_InitDriver(void)
{
  /* Set up Direct Drive and configure display controller */
  TFT_DirectInit(&tftInit);

  /* Enable the interrupt on VSYNC, generated from Direct Drive */
  /* Disable all EBI interrupts */
  EBI_IntDisable(_EBI_IF_MASK);

  /* Clear previous interrupts */
  EBI_IntClear(_EBI_IF_MASK);

  /* Enable VSYNC interrupt */
  EBI_IntEnable(EBI_IF_VSYNC);

  /* Enable EBI interrupts in core */
  NVIC_ClearPendingIRQ(EBI_IRQn);
  NVIC_EnableIRQ(EBI_IRQn);
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
int LCD_X_DisplayDriver(unsigned LayerIndex, unsigned Cmd, void * pData)
{
  (void) LayerIndex;

  int r;

  switch (Cmd) {

  /*
   * Called during the initialization process in order to set up the
   * display controller and put it into operation.
   */
  case LCD_X_INITCONTROLLER:
  {

    LCD_X_InitDriver();
    return 0;
  }

  /* This command is received every time the GUI is done drawing a frame */
  case LCD_X_SHOWBUFFER:
  {
    /* Get the data object */
    LCD_X_SHOWBUFFER_INFO * p;
    p = (LCD_X_SHOWBUFFER_INFO *)pData;

    /* Save the pending buffer index. The interrupt routine will flip buffer on next VSYNC. */
    pendingFrameIndex = p->Index;

  }
  default:
    r = -1;
  }
  return r;
}

/*************************** End of file ****************************/
