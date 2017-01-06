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
#include "em_acmp.h"
#include "em_assert.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_lesense.h"
#include "em_rtc.h"

uint8_t timerTick;

void CAPSENSE_Init(void);
void rtcSetup(uint16_t frequency);

/**************************************************************************//**
 * Configure sensor for sleep mode
 *
 * Configures sensor peripheral(s) for sleep mode scanning
 *
 *****************************************************************************/
void configureSensorForSleepMode(void)
{
   CAPSENSE_Init();
}


/**************************************************************************//**
 * Ready system for entrance into sleep mode
 *
 * Function configures hardware for optimal low power in sleep mode
 *
 *****************************************************************************/
void readyRegistersForSleep(void)
{
   // stub function called in sleep mode state machine
}

/**************************************************************************//**
 * Restore registers from sleep
 *
 * Re-enable and get system ready for active mode
 *
 *****************************************************************************/
void restoreRegistersFromSleep(void)
{
   // stub function called in sleep mode state machine
}


/**************************************************************************//**
 * Configure to sleep mode
 *
 * Re-enable and get system ready for active mode
 *
 *****************************************************************************/
void enterLowPowerState(void)
{
   EMU_EnterEM2(true);
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
#if (defined(_EFM32_PEARL_FAMILY))
	rtccSetup(1000/DEF_ACTIVE_MODE_PERIOD);
#else
	rtcSetup(1000/DEF_ACTIVE_MODE_PERIOD);
#endif
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
#if (defined(_EFM32_PEARL_FAMILY))
	rtccSetup(1000/DEF_SLEEP_MODE_PERIOD);
#else
	rtcSetup(1000/DEF_SLEEP_MODE_PERIOD);
#endif
}

/**************************************************************************//**
 * Check timer
 *
 *****************************************************************************/
void checkTimer(void)
{
   // Stub function because EFM32 code sets flag inside RTC ISR
}


