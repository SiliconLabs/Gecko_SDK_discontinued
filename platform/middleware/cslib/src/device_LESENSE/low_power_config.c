/**************************************************************************//**
 * Copyright 2016 by Silicon Laboratories Inc. All rights reserved.
 *
 * http://developer.silabs.com/legal/version/v11/Silicon_Labs_Software_License_Agreement.txt
 *****************************************************************************/

#include "em_device.h"
#include "em_chip.h"

#include "cslib_hwconfig.h"
#include "cslib_config.h"
#include "cslib.h"

#include "hardware_routines.h"
#include "low_power_config.h"

// Drivers
#include "caplesense.h"
#include "em_acmp.h"
#include "em_assert.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_lesense.h"
#include "em_rtc.h"
#include "rtcdriver.h"
#include "em_vcmp.h"

uint8_t timerTick;

extern void CAPLESENSE_switchToSleep(bool);

/**************************************************************************//**
 * Configure sensor for sleep mode
 *
 * Configures sensor peripheral(s) for sleep mode scanning
 *
 *****************************************************************************/
void configureSensorForSleepMode(void)
{
	// Disable VCMP
	VCMP_Disable();

	// Disable clock to VCMP
	CMU_ClockEnable(cmuClock_VCMP, false);
	CAPLESENSE_switchToSleep(true);
}



/**************************************************************************//**
 * Configure to sleep mode
 *
 * Re-enable and get system ready for active mode
 *
 *****************************************************************************/
void enterLowPowerState(void)
{
  CAPLESENSE_Sleep();
}

/**************************************************************************//**
 * Configure timer for active mode
 *
 * This is a top-level call to configure the timer to active mode, one of the
 * two defined modes of operation in the system.  This instance of the function
 * configures the SmaRTClock to the frequency defined in cslib_config.h.
 *
 *****************************************************************************/
void configureTimerForActiveMode(void)
{
}

/**************************************************************************//**
 * Configure timer for sleep mode
 *
 * This is a top-level call to configure the timer to sleep mode, one of the
 * two defined modes of operation in the system.  This instance of the function
 * configures the SmaRTClock to the frequency defined in cslib_config.h.
 *
 *****************************************************************************/
void configureTimerForSleepMode(void)
{
}


/**************************************************************************//**
 * Check timer
 *
 *****************************************************************************/
void checkTimer(void)
{
}


