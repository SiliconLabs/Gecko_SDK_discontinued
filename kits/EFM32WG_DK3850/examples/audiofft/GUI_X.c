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
File        : GUI_X.C
Purpose     : Config / System dependent externals for GUI
---------------------------END-OF-HEADER------------------------------
*/

#include "GUI.h"
#include <stdbool.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_emu.h"
#include "em_msc.h"
#include "em_gpio.h"
#include "rtcdrv.h"
#include "bsp.h"
#include "LCDConf.h"

/*********************************************************************
*
*       Global data
*/
volatile int OS_TimeMS;   /* System millisecond counter */
bool aemMode = false;     /* Flag to see if display is in AEM mode */
volatile bool rtcFlag;    /* Flag used by the RTC timing routines */


/**************************************************************************//**
 * @brief Callback used for the RTC.
 * @details
 *   Because GPIO interrupts can alswo wake up the CM3 from sleep it is
 *   necessary for the correct timing to make sure that
 *   the wake-up source is the RTC.
 *****************************************************************************/
void RTC_TimeOutHandler(void)
{
  rtcFlag = false;
}


/*********************************************************************
*
*      Timing:
*                 GUI_X_GetTime()
*                 GUI_X_Delay(int)

  Some timing dependent routines require a GetTime
  and delay function. Default time unit (tick), normally is
  1 ms.
*/


/**************************************************************************//**
 * @brief Interrupt Service Routine for system tick counter
 *****************************************************************************/
void SysTick_Handler(void)
{
  OS_TimeMS++;       /* increment counter necessary in Delay()*/
}

/*********************************************************************
*
*       GUI_X_GetTime()
*
* Note:
*     @brief returns system time in milisecond unit.
*     @details Depending on configuration time is returned with 1ms or 10ms resolution.
*/
int GUI_X_GetTime(void) {
  return OS_TimeMS;
}

/*********************************************************************
*
*       GUI_X_Delay()
*
* Note:
*     @brief is used to stop code execution for specified time
*     @param contains number of miliseconds to suspend program.
*     @details This routine could enter into EM1 mode to reduce power consumption.
*/
void GUI_X_Delay(int ms) {
  if(BSP_RegisterRead(&BC_REGISTER->UIF_AEM) != BC_UIF_AEM_EFM) {

    /* Switched to Advanced Energy Monitor, LCD will need to be reinitialized */
    aemMode = true;
  } else if( aemMode) {

    /* Switched back from Advanced Energy Monitor, reinitialize LCD  */
    aemMode = false;
    LCD_X_InitDriver();
  }

  /* Wake us up after msec */
  NVIC_DisableIRQ(LCD_IRQn);

  /* Start RTC counter */
  rtcFlag = true;
  RTCDRV_Trigger(ms, RTC_TimeOutHandler);

  /* The rtcFlag variable is set in the RTC interrupt routine using the callback
   * RTC_TimeOutHandler. This makes sure that the elapsed time is correct. */
  while (rtcFlag)
  {
    EMU_EnterEM1();
  }

  NVIC_EnableIRQ(LCD_IRQn);
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

void GUI_X_Init(void) { }


/*********************************************************************
*
*       GUI_X_ExecIdle
*
* Note:
*  @brief Called if WM is in idle state
*/

void GUI_X_ExecIdle(void) { GUI_X_Delay(10); }

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
