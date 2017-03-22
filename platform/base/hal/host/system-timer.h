/** @file hal/host/system-timer.h
 * See @ref system_timer for documentation.
 *
 * <!-- Copyright 2010 by Ember Corporation. All rights reserved.        *80*-->
 */


/** @addtogroup system_timer
 * @brief Functions that provide access to the system timer.
 *
 * A single system tick (as returned by ::halCommonGetInt16uMillisecondTick()
 * and ::halCommonGetInt32uMillisecondTick() ) is approximately 1 millisecond.
 *
 * @note The actual time of a tick is specific to each micro.
 *
 * A single quarter-second tick (as returned by
 * ::halCommonGetInt16uQuarterSecondTick() ) is approximately 0.25 seconds.
 *
 * The values used by the time support functions will wrap after an interval.
 * The length of the interval depends on the length of the tick and the number
 * of bits in the value. However, there is no issue when comparing time deltas
 * of less than half this interval with a subtraction, if all data types are
 * the same.
 *
 * See system-timer.h for source code.
 *@{
 */

#ifndef __SYSTEM_TIMER_H__
#define __SYSTEM_TIMER_H__


/**
 * @brief Initializes the system tick.
 *
 * @return Time to update the async registers after timer is
 * started (units of 100 microseconds).
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
 * @return The least significant 32 bits of the current system time, in 
 * system ticks.
 */
uint32_t halCommonGetInt32uMillisecondTick(void);

/**
 * @brief Returns the current system time in quarter second ticks, as a
 * 16-bit value.
 *
 * @return The least significant 16 bits of the current system time, in system
 * ticks multiplied by 256.
 */
uint16_t halCommonGetInt16uQuarterSecondTick(void);

/** @brief Set the current system time
 *
 * @param time  A 32 bit value, expressed in milliseconds, that will
 * become the current system time.
 */
void halCommonSetSystemTime(uint32_t time);


#endif //__SYSTEM_TIMER_H__

/**@} //END addtogroup 
 */

