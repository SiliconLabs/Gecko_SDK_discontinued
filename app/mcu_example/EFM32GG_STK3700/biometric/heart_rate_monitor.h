/**************************************************************************//**
 * @file
 * @brief Heart Rate and SpO2 state machine
 * @version 5.1.2
 ******************************************************************************
 * @section License
 * <b>Copyright 2015 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#ifndef __HEARTRATEMONITOR_H
#define __HEARTRATEMONITOR_H
#include "si114x_sys_out.h"
typedef enum
{
  BIOMETRIC_EXP,
  SI1143_PS,
  SI1147_PS
}HeartRateMonitor_Config_t ;

void HeartRateMonitor_Init(Si114xPortConfig_t* i2c, HeartRateMonitor_Config_t );
bool HeartRateMonitor_Loop ( bool displaySpO2, bool forceStop, bool checkSkinContact );
void HeartRateMonitor_TimerEventHandler(void);
void HeartRateMonitor_Interrupt (void);
bool HeartRateMonitor_SamplesPending (void);
void HeartRateMonitor_GetVersion (char *hrmVersion);

#endif /* __HEARTRATEMONITOR_H */
