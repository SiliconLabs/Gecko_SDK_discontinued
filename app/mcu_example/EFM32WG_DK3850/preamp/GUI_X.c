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
File        : GUI_X.C
Purpose     : Config / System dependent externals for GUI
---------------------------END-OF-HEADER------------------------------
*/

#include "GUI.h"
#include "WM.h"
#include <stdbool.h>
#include <stdint.h>
#include "em_device.h"
#include "em_cmu.h"
#include "bsp.h"
#include "plot.h"

/*********************************************************************
*
*       Local data
*/
static bool aemMode = false;     /* Flag to see if display is in AEM mode */

int GUI_X_GetTime(void) { return 0; }

/***************************************************************************//**
*     @brief
*        is used to stop code execution for specified time
*     @param[in] ms
*        contains number of miliseconds to suspend program. Maximum allowed
*        value is 10000 (10 seconds).
*     @details
*        This routine could enter into EM1 or EM2 mode to reduce power
*        consumption. If touch panel is not pressed EM2 is executed, otherwise
*        due to fact that ADC requires HF clock, only EM1 is enabled. This
*        function is also used to handle joystick state and move cursor
*        according to it. In addition it could also reinitialize LCD if
*        previously Advanced Energy Monitor screen was active.
 ******************************************************************************/
void GUI_X_Delay(int ms)
{
  volatile uint32_t now;
  uint32_t startTime, waitTime;

  if ( BSP_RegisterRead( &BC_REGISTER->UIF_AEM) != BC_UIF_AEM_EFM)
  {
    /* Switched to Advanced Energy Monitor, LCD will need to be reinitialized */
    aemMode = true;
  }
  else if( aemMode )
  {
    /* Switched back from Advanced Energy Monitor, reinitialize LCD  */
    aemMode = false;
    PLOT_DisplayInit();
  }

  if ( ms > 0 )
  {
    waitTime = ms * (CMU_ClockFreqGet(cmuClock_CORE) / 1000);

    /* Enable DWT and make sure CYCCNT is running. */
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CTRL        |= 1;

    startTime = DWT->CYCCNT;
    do
    {
      now = DWT->CYCCNT;
    } while ( ( now - startTime ) < waitTime );
  }
}

/*********************************************************************
*
*       GUI_X_Init()
*
* Note:
*     @brief is called from GUI_Init is a possibility to init
*     some hardware which needs to be up and running before the GUI.
*     If not required, leave this routine blank.
*/

void GUI_X_Init(void) {}

/*********************************************************************
*
*       GUI_X_ExecIdle
*
* Note:
*  @brief Called if WM is in idle state
*/

void GUI_X_ExecIdle(void) {}

/*********************************************************************
*
*      Logging: OS dependent

Note:
  Logging is used in higher debug levels only. The typical target
  build does not use logging and does therefor not require any of
  the logging routines below. For a release build without logging
  the routines below may be eliminated to save some space.
  (If the linker is not function aware and eliminates unreferenced
  functions automatically)

*/

void GUI_X_Log     (const char *s) { GUI_USE_PARA(s); }
void GUI_X_Warn    (const char *s) { GUI_USE_PARA(s); }
void GUI_X_ErrorOut(const char *s) { GUI_USE_PARA(s); }

/*************************** End of file ****************************/
