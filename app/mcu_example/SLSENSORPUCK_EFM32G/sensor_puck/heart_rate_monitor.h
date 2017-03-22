/**************************************************************************//**
* @file
* @brief Heart Rate and SpO2 state machine
* @version 5.1.2

******************************************************************************
* @section License
* <b>Copyright 2015 Silicon Labs, Inc. http://www.silabs.com</b>
*******************************************************************************
*
* This file is licensensed under the Silabs License Agreement. See the file
* "Silabs_License_Agreement.txt" for details. Before using this software for
* any purpose, you must agree to the terms of that agreement.
*
******************************************************************************/

#ifndef __HEARTRATEMONITOR_H
#define __HEARTRATEMONITOR_H
#include "si114x_sys_out_puck.h"

typedef enum HRMSpO2State
{
  HRM_STATE_IDLE,
  HRM_STATE_NOSIGNAL,
  HRM_STATE_ACQUIRING,
  HRM_STATE_ACTIVE,
  HRM_STATE_INVALID,
  HRM_STATE_ERROR
} HRMSpO2State_t;

void HeartRateMonitor_Init(Si114xPortConfig_t* i2c);
bool HeartRateMonitor_Loop(bool forceStop, bool checkSkinContact);
void HeartRateMonitor_TimerEventHandler(void);
void HeartRateMonitor_Interrupt(void);
bool HeartRateMonitor_SamplesPending(void);
void HeartRateMonitor_GetVersion(char *hrmVersion);
#endif /* __HEARTRATEMONITOR_H */
