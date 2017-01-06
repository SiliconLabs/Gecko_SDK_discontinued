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
File        : TOUCH_Calibrate.c
Purpose     : Demonstrates how a touch screen can be calibrated at run time
Requirements: WindowManager - ( )
              MemoryDevices - ( )
              AntiAliasing  - ( )
              VNC-Server    - ( )
              PNG-Library   - ( )
              TrueTypeFonts - ( )
---------------------------END-OF-HEADER------------------------------
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
#define RECOMMENDED_MEMORY (1024L * 10)

/********************************************************************
*
*       Static data
*
*********************************************************************
*/

static const char * _acPos[] = {
  "(upper left position)",
  "(lower right position)"
};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _WaitForPressedState
*
* Function description
*   Waits until the touch is in the given pressed state for at least 250 ms
*/
static void _WaitForPressedState(int Pressed) {
  GUI_PID_STATE State;

  do {
    GUI_TOUCH_GetState(&State);
    GUI_Delay(1);
    if (State.Pressed == Pressed) {
      int TimeStart = GUI_GetTime();
      do {
        GUI_TOUCH_GetState(&State);
        GUI_Delay(1);
        if (State.Pressed != Pressed) {
          break;
        } else if ((GUI_GetTime() - 50) > TimeStart) {
          return;
        }
      } while (1);
    }
  } while (1);
}

/*********************************************************************
*
*       _DispStringCentered
*
* Function description
*   Shows the given text horizontally and vertically centered
*/
static void _DispStringCentered(const char * pString) {
  GUI_RECT Rect;

  Rect.x0 = Rect.y0 = 0;
  Rect.x1 = LCD_GetXSize() - 1;
  Rect.y1 = LCD_GetYSize() - 1;
  GUI_DispStringInRect(pString, &Rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
}

/*********************************************************************
*
*       _GetPhysValues
*
* Function description
*   Asks the user to press the touch screen at the given position
*   and returns the physical A/D values
*/
static void _GetPhysValues(int LogX, int LogY, int * pPhysX, int * pPhysY, const char * pString) {
  char acText[] = "Press here";
  GUI_RECT Rect;
  int FontSizeY;
  int Align;

  FontSizeY = GUI_GetFontSizeY();
  GUI_Clear();
  GUI_SetColor(GUI_BLACK);
  _DispStringCentered("Runtime calibration,\n"
                      "please touch the screen\n"
                      "at the center of the ring."); /* Ask user to press the touch */
  //
  // Calculate the rectangle for the string
  //
  Rect.y0 = LogY - FontSizeY;
  Rect.y1 = LogY + FontSizeY;
  if (LogX < LCD_GetXSize() / 2) {
    Rect.x0 = LogX + 15;
    Rect.x1 = LCD_GetXSize();
    Align = GUI_TA_LEFT;
  } else {
    Rect.x0 = 0;
    Rect.x1 = LogX - 15;
    Align = GUI_TA_RIGHT;
  }
  //
  // Show the text nearby the ring
  //
  GUI_DispStringInRect(acText, &Rect, Align | GUI_TA_TOP);
  GUI_DispStringInRect(pString, &Rect, Align | GUI_TA_BOTTOM);
  //
  // Draw the ring
  //
  GUI_FillCircle(LogX, LogY, 10);
  GUI_SetColor(GUI_WHITE);
  GUI_FillCircle(LogX, LogY, 5);
  GUI_SetColor(GUI_BLACK);
  //
  // Wait until touch is pressed
  //
  _WaitForPressedState(1);
  *pPhysX = GUI_TOUCH_GetxPhys();
  *pPhysY = GUI_TOUCH_GetyPhys();
  //
  // Wait until touch is released
  //
  _WaitForPressedState(0);
}

/********************************************************************
*
*       _Explain
*
* Funtion description
*   Shows a text to give a short explanation of the sample program
*/
static void _Explain(void) {
  _DispStringCentered("This sample shows how\n"
                      "a touch screen can be\n"
                      "calibrated at run time.\n"
                      "Please press the touch\n"
                      "screen to continue...");
  GUI_DispStringHCenterAt("TOUCH_Calibrate", LCD_GetXSize() / 2, 5);
  _WaitForPressedState(1);
  _WaitForPressedState(0);
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
  int aPhysX[2], aPhysY[2], aLogX[2], aLogY[2], i;
  GUI_Init();
  //
  // Check if recommended memory for the sample is available
  //
  if (GUI_ALLOC_GetNumFreeBytes() < RECOMMENDED_MEMORY) {
    GUI_ErrorOut("Not enough memory available."); 
    return;
  }
  GUI_SetBkColor(GUI_WHITE);
  GUI_Clear();
  GUI_SetColor(GUI_BLACK);
  GUI_SetFont(&GUI_Font13B_ASCII);
  _Explain();
  /* Set the logical values */
  aLogX[0] = 15;
  aLogY[0] = 15;
  aLogX[1] = LCD_GetXSize() - 15;
  aLogY[1] = LCD_GetYSize() - 15;
  /* Get the physical values of the AD converter for 2 positions */
  for (i = 0; i < 2; i++) {
    _GetPhysValues(aLogX[i], aLogY[i], &aPhysX[i], &aPhysY[i], _acPos[i]);
  }
  /* Use the physical values to calibrate the touch screen */
  GUI_TOUCH_Calibrate(0, aLogX[0], aLogX[1], aPhysX[0], aPhysX[1]); /* Calibrate X-axis */
  GUI_TOUCH_Calibrate(1, aLogY[0], aLogY[1], aPhysY[0], aPhysY[1]); /* Calibrate Y-axis */
  /* Display the result */
  GUI_CURSOR_Show();
  GUI_Clear();
  _DispStringCentered("Congratulation, your\n"
                      "touch screen has been\n"
                      "calibrated. Please use\n"
                      "the cursor to test\n"
                      "the calibration...");
  /* Let the user play */
  while(1) {
    GUI_PID_STATE State;

    GUI_TOUCH_GetState(&State);
    if (State.Pressed == 1) {
      GUI_FillCircle(State.x, State.y, 3);
    }
    GUI_Delay(10);
  }
}

/*************************** End of file ****************************/

