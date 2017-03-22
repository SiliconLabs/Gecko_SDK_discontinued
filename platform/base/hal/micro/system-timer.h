/** @file hal/micro/system-timer.h
 * See @ref system_timer for documentation.
 * 
 *
 * <!-- Copyright 2005 by Ember Corporation. All rights reserved.-->   
 */

/** @addtogroup system_timer
 * @brief Functions that provide access to the system clock.
 *
 * A single system tick (as returned by ::halCommonGetInt16uMillisecondTick() and
 * ::halCommonGetInt32uMillisecondTick() ) is approximately 1 millisecond.
 *
 * - When used with a 32.768kHz crystal, the system tick is 0.976 milliseconds.
 *
 * - When used with a 3.6864MHz crystal, the system tick is 1.111 milliseconds.
 *
 * A single quarter-second tick (as returned by
 * ::halCommonGetInt16uQuarterSecondTick() ) is approximately 0.25 seconds.
 *
 * The values used by the time support functions will wrap after an interval.
 * The length of the interval depends on the length of the tick and the number
 * of bits in the value. However, there is no issue when comparing time deltas
 * of less than half this interval with a subtraction, if all data types are the
 * same.
 *
 * See system-timer.h for source code.
 *@{
 */

#ifndef __SYSTEM_TIMER_H__
#define __SYSTEM_TIMER_H__

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#if defined( EMBER_TEST )
  #include "unix/simulation/system-timer-sim.h"
#endif

#endif // DOXYGEN_SHOULD_SKIP_THIS


/**
 * @brief Initializes the system tick.
 *
 * @return Time to update the async registers after RTC is started (units of 100 
 * microseconds).
 */
uint16_t halInternalStartSystemTimer(void);


/**
 * @brief Returns the current system time in system ticks, as a 16-bit
 * value.
 *
 * @return The least significant 16 bits of the current system time, in system
 * ticks.
 */
uint16_t halCommonGetInt16uMillisecondTick(void);

/**
 * @brief Returns the current system time in system ticks, as a 32-bit
 * value.
 *
 * @nostackusage
 *
 * @return The least significant 32 bits of the current system time, in 
 * system ticks.
 */
uint32_t halCommonGetInt32uMillisecondTick(void);

/**
 * @brief Returns the current system time in quarter second ticks, as a
 * 16-bit value.
 *
 * @nostackusage
 *
 * @return The least significant 16 bits of the current system time, in system
 * ticks multiplied by 256.
 */
uint16_t halCommonGetInt16uQuarterSecondTick(void);

/**
 * @brief Uses the system timer to enter ::SLEEPMODE_WAKETIMER for
 * approximately the specified amount of time (provided in quarter seconds).
 *
 * This function returns ::EMBER_SUCCESS and the duration parameter is
 * decremented to 0 after sleeping for the specified amount of time.  If an
 * interrupt occurs that brings the chip out of sleep, the function returns
 * ::EMBER_SLEEP_INTERRUPTED and the duration parameter reports the amount of
 * time remaining out of the original request.
 *
 * @note This routine always enables interrupts.
 *
 * @note The maximum sleep time of the hardware is limited on AVR-based
 * platforms to 8 seconds, on EM2XX-based platforms to 64 seconds, and on
 * EM35x platforms to 48.5 days.  Any sleep duration greater than this limit
 * will wake up briefly (e.g. 16 microseconds) to reenable another sleep cycle.
 *
 * The EM2xx has a 16 bit sleep timer, which normally runs at 1024Hz.  In order
 * to support long sleep durations, the chip will periodically wake up to 
 * manage a larger timer in software.   This periodic wakeup is normally 
 * triggered once every 32 seconds.  However, this period can be extended to 
 * once every 2.275 hours by building with <b>ENABLE_LONG_SLEEP_CYCLES</b>
 * defined.  This definition enables the use of a prescaler when sleeping for
 * more than 63 seconds at a time.  However, this define also imposes the
 * following limitations:
 *
 * 1. The chip may only wake up from the sleep timer.  (External GPIO wake 
 *    events may not be used)
 * 2. Each time a sleep cycle is performed, a loss of accuracy up to +/-750ms
 *    will be observed in the system timer.
 *
 * @nostackusage
 *
 * @param duration The amount of time, expressed in quarter seconds, that the
 * micro should be placed into ::SLEEPMODE_WAKETIMER.  When the function returns,
 * this parameter provides the amount of time remaining out of the original
 * sleep time request (normally the return value will be 0).
 * 
 * @return An EmberStatus value indicating the success or
 *  failure of the command.
 */
EmberStatus halSleepForQuarterSeconds(uint32_t *duration);

/**
 * @brief Uses the system timer to enter ::SLEEPMODE_WAKETIMER for
 * approximately the specified amount of time (provided in milliseconds).
 * Note that since the system timer ticks at a rate of 1024Hz, a second is 
 * comprised of 1024 milliseconds in this function.
 *
 * This function returns ::EMBER_SUCCESS and the duration parameter is
 * decremented to 0 after sleeping for the specified amount of time.  If an
 * interrupt occurs that brings the chip out of sleep, the function returns
 * ::EMBER_SLEEP_INTERRUPTED and the duration parameter reports the amount of
 * time remaining out of the original request.
 *
 * @note This routine always enables interrupts.
 *
 * @note This function is not implemented on AVR-based platforms.
 *
 * @note Sleep durations less than 3 milliseconds are not allowed on
 * on EM2XX-based platforms.  Any attempt to sleep for less than 3 milliseconds
 * on EM2XX-based platforms will cause the function to immediately exit without
 * sleeping and return ::EMBER_SLEEP_INTERRUPTED.
 *
 * @note The maximum sleep time of the hardware is limited on EM2XX-based 
 * platforms to 32 seconds.  Any sleep duration greater than this limit
 * will wake up briefly (e.g. 16 microseconds) to reenable another sleep cycle.
 * Due to this limitation, this function should not be used with durations
 * within 3 milliseconds of a multiple 32 seconds.  The short sleep cycle that
 * results from such durations is not handled reliably by the system timer on
 * EM2XX-based platforms.  If a sleep duration within 3 milliseconds of a
 * multiple of 32 seconds is desired, halSleepForQuarterSeconds should be used.
 * 
 * @nostackusage
 *
 * @param duration The amount of time, expressed in milliseconds
 * (1024 milliseconds = 1 second), that the micro should be placed into
 * ::SLEEPMODE_WAKETIMER.  When the function returns, this parameter provides
 * the amount of time remaining out of the original sleep time request
 * (normally the return value will be 0).
 * 
 * @return An EmberStatus value indicating the success or
 *  failure of the command.
 */
EmberStatus halSleepForMilliseconds(uint32_t *duration); 

/**
 * @brief Uses the system timer to enter ::SLEEPMODE_IDLE for
 * approximately the specified amount of time (provided in milliseconds).
 *
 * This function returns ::EMBER_SUCCESS and the duration parameter is
 * decremented to 0 after idling for the specified amount of time.  If an
 * interrupt occurs that brings the chip out of idle, the function returns
 * ::EMBER_SLEEP_INTERRUPTED and the duration parameter reports the amount of
 * time remaining out of the original request.
 *
 * @note This routine always enables interrupts.
 *
 * @nostackusage
 *
 * @param duration  The amount of time, expressed in milliseconds, that the
 * micro should be placed into ::SLEEPMODE_IDLE.  When the function returns,
 * this parameter provides the amount of time remaining out of the original
 * idle time request (normally the return value will be 0).
 *
 * @return An EmberStatus value indicating the success or
 *  failure of the command.
 */
EmberStatus halCommonIdleForMilliseconds(uint32_t *duration);
// Maintain the previous API for backwards compatibility
#define halIdleForMilliseconds(duration) halCommonIdleForMilliseconds((duration))

#ifdef EMBER_STACK_COBRA
EmberStatus halCobraIdleForMicroseconds(uint32_t *duration);
#endif

#endif //__SYSTEM_TIMER_H__

/**@} //END addtogroup 
 */




