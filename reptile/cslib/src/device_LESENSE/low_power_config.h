/**************************************************************************//**
 * Copyright 2016 by Silicon Laboratories Inc. All rights reserved.
 *
 * http://developer.silabs.com/legal/version/v11/Silicon_Labs_Software_License_Agreement.txt
 *****************************************************************************/

#ifndef __LOW_POWER_CONFIG_H__
#define __LOW_POWER_CONFIG_H__

// Functions which must be defined with implementation-specific
// responsibilities.  These are called by LowPowerRoutines.c
void configureSensorForSleepMode(void);
void configureTimerForSleepMode(void);
void configureTimerForActiveMode(void);
void enterLowPowerState(void);
void checkTimer(void);
extern uint8_t timerTick;

#endif // __LOW_POWER_CONFIG_H__
