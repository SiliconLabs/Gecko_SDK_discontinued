/** @file hal/micro/micro.h
 * 
 * @brief Full HAL functions common across all microcontroller-specific files.
 * See @ref micro for documentation.
 *
 * Some functions in this file return an ::EmberStatus value. 
 * See error-def.h for definitions of all ::EmberStatus return values.
 *
 * <!-- Copyright 2004-2011 by Ember Corporation. All rights reserved.   *80*-->
 */
 
/** @addtogroup micro
 * Many of the supplied example applications use these microcontroller functions.
 * See hal/micro/micro.h for source code.
 *
 * @note The term SFD refers to the Start Frame Delimiter.
 *@{
 */

#ifndef __MICRO_H__
#define __MICRO_H__

#include "hal/micro/generic/em2xx-reset-defs.h"
#include "hal/micro/micro-types.h"

#ifndef DOXYGEN_SHOULD_SKIP_THIS

// Make sure that a proper plat/micro combination was selected if we aren't
// building for a host processor
#if ((! defined(EZSP_HOST)) && (! defined(UNIX_HOST)))

#ifndef PLAT
  #error no platform defined, or unsupported
#endif
#ifndef MICRO
  #error no micro defined, or unsupported
#endif
#ifndef PHY
  #error no phy defined, or unsupported
#endif

#endif // ((! defined(EZSP_HOST)) && (! defined(UNIX_HOST)))

#endif // DOXYGEN_SHOULD_SKIP_THIS

/** @brief Called from emberInit and provides a means for the HAL
 * to increment a boot counter, most commonly in non-volatile memory.
 *
 * This is useful while debugging to determine the number of resets that might
 * be seen over a period of time.  Exposing this functionality allows the
 * application to disable or alter processing of the boot counter if, for
 * example, the application is expecting a lot of resets that could wear
 * out non-volatile storage or some
 *
 * @stackusage Called from emberInit only as helpful debugging information.
 * This should be left enabled by default, but this function can also be
 * reduced to a simple return statement if boot counting is not desired.
 */
void halStackProcessBootCount(void);

/** @brief Gets information about what caused the microcontroller to reset. 
 *
 * @return A code identifying the cause of the reset.
 */
uint8_t halGetResetInfo(void);

/** @brief Calls ::halGetResetInfo() and supplies a string describing it.
 *
 * @appusage Useful for diagnostic printing of text just after program 
 * initialization.
 *
 * @return A pointer to a program space string.
 */
PGM_P halGetResetString(void);

/** @brief Calls ::halGetExtendedResetInfo() and translates the EM35x or COBRA
 *  reset code to the corresponding value used by the EM2XX HAL. Any reset codes 
 * not present in the EM2XX are returned after being OR'ed with 0x80.
 *
 * @appusage Used by the EZSP host as a platform-independent NCP reset code.
 *
 * @return The EM2XX-compatible reset code. If not supported by the EM2XX,
 *         return the platform-specific code with B7 set.
 */
#if defined(CORTEXM3) || defined(EMBER_STACK_COBRA)
  uint8_t halGetEm2xxResetInfo(void);
#else
  #define halGetEm2xxResetInfo() halGetResetInfo()
#endif

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "micro-common.h"

#if defined( EMBER_TEST )
  #include "hal/micro/unix/simulation/micro.h"
  #include "hal/micro/unix/simulation/bootloader.h"
#elif defined(CORTEXM3_EMBER_MICRO)
  #include "cortexm3/micro.h"
#elif defined(CORTEXM3_EFM32_MICRO)
  #include "cortexm3/efm32/micro.h"
#elif defined(C8051)
  #include "c8051/micro.h"
#elif defined(FFD)
//  #include "ffd/micro.h"
#elif ((defined(EZSP_HOST) || defined(UNIX_HOST)))
  #include "hal/micro/unix/host/micro.h"
#else
  #error no platform or micro defined
#endif

// the number of ticks (as returned from halCommonGetInt32uMillisecondTick)
// that represent an actual second. This can vary on different platforms.
// It must be defined by the host system.
#ifndef MILLISECOND_TICKS_PER_SECOND
  #define MILLISECOND_TICKS_PER_SECOND 1024UL
// See bug 10232
//  #error "MILLISECOND_TICKS_PER_SECOND is not defined in micro.h!"
#endif

#ifndef MILLISECOND_TICKS_PER_DECISECOND
  #define MILLISECOND_TICKS_PER_DECISECOND (MILLISECOND_TICKS_PER_SECOND / 10)
#endif

#ifndef MILLISECOND_TICKS_PER_QUARTERSECOND
  #define MILLISECOND_TICKS_PER_QUARTERSECOND (MILLISECOND_TICKS_PER_SECOND >> 2)
#endif

#ifndef MILLISECOND_TICKS_PER_MINUTE
  #define MILLISECOND_TICKS_PER_MINUTE (60UL * MILLISECOND_TICKS_PER_SECOND)
#endif

#ifndef MILLISECOND_TICKS_PER_HOUR
  #define MILLISECOND_TICKS_PER_HOUR (60UL * MILLISECOND_TICKS_PER_MINUTE)
#endif

#ifndef MILLISECOND_TICKS_PER_DAY
  #define MILLISECOND_TICKS_PER_DAY (24UL * MILLISECOND_TICKS_PER_HOUR)
#endif

#endif // DOXYGEN_SHOULD_SKIP_THIS

#endif //__MICRO_H__

/** @} END micro group  */
  

