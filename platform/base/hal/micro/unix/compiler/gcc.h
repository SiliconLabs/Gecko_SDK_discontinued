/** @file hal/micro/unix/compiler/gcc.h
 * See @ref platform_common and @ref unix_gcc_config for documentation.
 *
 * <!-- Copyright 2003-2010 by Ember Corporation. All rights reserved.   *80*-->
 */

/** @addtogroup unix_gcc_config
 * @brief Compiler and Platform specific definitions and typedefs for the
 *  the Unix GCC compiler.
 *
 * @note ATOMIC and interrupt manipulation macros are defined to have no
 * affect.
 *
 * @note gcc.h should be included first in all source files by setting the
 *  preprocessor macro PLATFORM_HEADER to point to it.  gcc.h automatically
 *  includes platform-common.h.
 *
 * See @ref platform_common for common documentation.
 *
 * See gcc.h for source code.
 *@{
 */

#ifndef __GCC_H__
#define __GCC_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/** \name Master Variable Types
 * These are a set of typedefs to make the size of all variable declarations
 * explicitly known.
 */
//@{
/**
 * @brief A typedef to make the size of the variable explicitly known.
 */

#ifndef EMBER_USE_WINDOWS_BOOLEAN // prevent duplicate definition of boolean
typedef bool boolean; /*To ease adoption of bool instead of boolean.*/
#endif

typedef unsigned char  int8u;
typedef signed   char  int8s;
typedef unsigned short int16u;
typedef signed   short int16s;
typedef unsigned int   int32u;
typedef signed   int   int32s;
typedef unsigned long long int64u;
typedef signed   long long int64s;
typedef unsigned long  PointerType;
//@} \\END MASTER VARIABLE TYPES


/**
 * @brief 8051 memory segments stubs.
 */
#if defined(C8051)
typedef boolean        bit;
typedef bit            BIT;

#define SEG_DATA
#define SEG_IDATA
#define SEG_XDATA
#define SEG_PDATA
#define SEG_CODE
#define SEG_BDATA

#define idata SEG_IDATA
#define xdata SEG_XDATA
#define pdata SEG_PDATA
#define code  SEG_CODE
#define bdata SEG_BDATA
#endif // C8051

/**
 * @brief Denotes that this platform supports 64-bit data-types.
 */
#define HAL_HAS_INT64

/**
 * @brief Use the Master Program Memory Declarations from platform-common.h
 */
#define _HAL_USE_COMMON_PGM_

/**
 * @brief A definition stating what the endianess of the platform is.
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
#define BIGENDIAN_CPU false
#else
  #if defined(__i386__)
    #define BIGENDIAN_CPU  false
  #elif defined(__ARM7__)
    #define BIGENDIAN_CPU  false
  #elif defined(__x86_64__)
    #define BIGENDIAN_CPU  false
  #elif defined(__arm__)
    #if defined(__BIG_ENDIAN)
      #define BIGENDIAN_CPU  true
    #else
      #define BIGENDIAN_CPU  false
    #endif
  #elif defined(__LITTLE_ENDIAN__)
    #define BIGENDIAN_CPU  false
  #elif defined(__BIG_ENDIAN__)
    #define BIGENDIAN_CPU  true
  #elif defined(__APPLE__)
    #define BIGENDIAN_CPU  true
  #elif defined(__mips__)
    #if defined(__BIG_ENDIAN)
      #define BIGENDIAN_CPU  true
    #else
      #define BIGENDIAN_CPU  false
    #endif
  #else
    #error endianess not defined
  #endif
#endif

#ifndef DOXYGEN_SHOULD_SKIP_THIS
  #define NO_STRIPPING
  #define EEPROM

  // Always include stdio.h and assert.h if running under Unix so that they
  // can be used when debugging.
  #include <stdio.h>
  #include <assert.h>
  #include <stdarg.h>

  #define NOP()
  #define DECLARE_INTERRUPT_STATE
  #define DECLARE_INTERRUPT_STATE_LITE
  #define DISABLE_INTERRUPTS() do { } while(0)
  #define DISABLE_INTERRUPTS_LITE() do { } while(0)
  #define RESTORE_INTERRUPTS() do { } while(0)
  #define RESTORE_INTERRUPTS_LITE() do { } while(0)
  #define INTERRUPTS_ON() do { } while(0)
  #define INTERRUPTS_OFF() do { } while(0)
#if defined(EMBER_TEST)
  #define INTERRUPTS_ARE_OFF() (true)
#else
  #define INTERRUPTS_ARE_OFF() (false)
#endif
  #define ATOMIC(blah) { blah }
  #define ATOMIC_LITE(blah) { blah }
  #define HANDLE_PENDING_INTERRUPTS() do { } while(0)

  #define LOG_MESSAGE_DUMP

  #define UNUSED __attribute__ ((unused))
  #define SIGNED_ENUM

  // think different
  #ifdef __APPLE__
  #define __unix__
  #endif

  #ifdef WIN32
  // undefine this here too
    // See bug EMSTACK-2808
    #if !defined(IMAGE_BUILDER)
      #define __attribute__(foo)
    #endif
  #endif

  #if defined(EMBER_TEST)
    #define MAIN_FUNCTION_PARAMETERS void
    #define MAIN_FUNCTION_ARGUMENTS

    // Called by application main loops to let the simulator simulate.
    // Not used on real hardware.
    void simulatedTimePasses(void);         // time moves forward 4ms
    void simulatedTimePassesUs(uint32_t us);  // time moves forward us microseconds

    // This moves time forward for the minimum of:
    //   - timeToNextAppEvent milliseconds
    //   - time until the next stack event fires
    //   - time until next serial character is read or written
    // This is used to allow time to pass more efficiently - if there is nothing
    // to do it can move the clock forward by a large amount.
    void simulatedTimePassesMs(uint32_t timeToNextAppEvent);

  #else
    #define MAIN_FUNCTION_PARAMETERS int argc, char* argv[]
    #define MAIN_FUNCTION_ARGUMENTS  argc, argv
    #define MAIN_FUNCTION_HAS_STANDARD_ARGUMENTS

    // Stub for code not running in simulation.
    #define simulatedTimePasses()
    #define simulatedTimePassesUs(x)
    #define simulatedTimePassesMs(x)

  #endif

  // Called by the serial code when it wants to block.
  void simulatedSerialTimePasses(void);

#endif //DOXYGEN_SHOULD_SKIP_THIS

/**
 * @brief Provide a portable way to align data.
 */
#define ALIGNMENT(__alignmentBytes) \
  __attribute__ ((aligned (__alignmentBytes)))

/**
 * @brief Provide a portable way to specify a symbol as weak
 */
#define WEAK(__symbol) \
  __attribute__ ((weak)) __symbol

/**
 * @brief Provide a portable way to specify a compile time assert
 */
#define STATIC_ASSERT(__condition,__errorstr) \
  _Static_assert(__condition,__errorstr)

/** \name Watchdog Prototypes
 * Define the watchdog macro and internal function to simply be
 * stubs to satisfy those programs that have no HAL (i.e. scripted tests)
 * and those that want to reference real HAL functions (simulation binaries
 * and Unix host applications) we define both halResetWatchdog() and
 * halInternalResetWatchdog().  The former is used by most of the scripted
 * tests while the latter is used by simulation and real host applications.
 */
//@{
/** @brief Watchdog stub prototype.
 */
void halInternalResetWatchDog(void);
#if defined(EMBER_SCRIPTED_TEST)
  #define halResetWatchdog()
#else
  #define halResetWatchdog() \
    halInternalResetWatchDog()
#endif
//@} //end of Watchdog Prototypes

/**
 * Include string.h for the C Standard Library memory routines used in
 * platform-common.
 */
#include <string.h>

/**
 * @brief Use the Divide and Modulus Operations from platform-common.h
 */
#define _HAL_USE_COMMON_DIVMOD_

/**
 * @brief Include platform-common.h last to pick up defaults and common definitions.
 */
#define PLATCOMMONOKTOINCLUDE
  #include "hal/micro/generic/compiler/platform-common.h"
#undef PLATCOMMONOKTOINCLUDE

#endif //__GCC_H__

/**@} //END addtogroup
 */

