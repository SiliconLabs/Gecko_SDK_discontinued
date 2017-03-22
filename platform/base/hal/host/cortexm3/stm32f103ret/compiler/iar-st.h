/** @file hal/host/cortexm3/stm32f103ret/compiler/iar-st.h
 * See @ref platform_common and @ref stm32f103ret_iar_config for documentation.
 *
 * <!-- Copyright 2010 by Ember Corporation. All rights reserved.        *80*-->
 */

/** @addtogroup stm32f103ret_iar_config
 * @brief Compiler and Platform specific definitions and typedefs for the
 *  STM32F103RET Host built with the IAR ARM C compiler.
 *
 * @note iar-st.h should be included first in all source files by setting the
 *  preprocessor macro PLATFORM_HEADER to point to it.  iar-st.h automatically
 *  includes platform-common.h.
 *
 * See @ref platform_common for common documentation.
 *
 * See iar-st.h for source code.
 *@{
 */

#ifndef __IAR_ST_H__
#define __IAR_ST_H__

#ifndef __ICCARM__
  #error Improper PLATFORM_HEADER
#endif

#if (__VER__ < 6040002)
  #error Only IAR EWARM versions greater than 6.40.2 are supported
#endif // __VER__

//Pull in the registers, Library, and other critical/useful ST code.
#include "stm32f10x.h"
#include <stddef.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS
//The Cortex-M3 does not have zero-page memory
#define XAP2B_PAGEZERO_ON
#define XAP2B_PAGEZERO_OFF
#endif

/** \name Master Variable Types
 * These are a set of typedefs to make the size of all variable declarations
 * explicitly known.  Since the IAR host code links against the ST Standard
 * peripheral library, we need to map Ember's variable types to ST's
 * variable types.
 * @note ST uses IAR's variable types, found in stdint.h.
 */
//@{
/**
 * @brief A typedef to make the size of the variable explicitly known.
 */
typedef bool boolean; /*To ease adoption of bool instead of boolean.*/
typedef uint8_t  int8u;
typedef int8_t   int8s;
typedef uint16_t int16u;
typedef int16_t  int16s;
typedef uint32_t int32u;
typedef int32_t  int32s;
typedef uint32_t PointerType;
//@} \\END MASTER VARIABLE TYPES


/**
 * @brief Internal function to reset the watchdog timer.
 * @note Be very very careful when using this as you can easily get
 * into an infinite loop if you are not careful.
 */
void halInternalResetWatchDog(void);

/**
 * @brief Macro to reset the watchdog timer.
 * @note Be very very careful when using this as you can easily get
 * into an infinite loop if you are not careful.
 */
#define halResetWatchdog()  halInternalResetWatchDog()


/**
 * @brief Some platforms need to cast enum values that have the high bit set.
 */
#define SIGNED_ENUM

/**
 * @brief Stub for code not running in simulation.
 */
#define simulatedSerialTimePasses()


/**
 * @brief Use the Divide and Modulus Operations from platform-common.h
 */
#define _HAL_USE_COMMON_DIVMOD_


/**
 * @brief Use the Master Program Memory Declarations from platform-common.h
 */
#define _HAL_USE_COMMON_PGM_


////////////////////////////////////////////////////////////////////////////////
/** \name Miscellaneous Macros
 */
////////////////////////////////////////////////////////////////////////////////
//@{

/**
 * @brief A convenient method for code to know what endiannes processor
 * it is running on.  For the Cortex-M3, we are little endian.
 */
#define BIGENDIAN_CPU  false

/**
 * @brief Define the parameters to main(), and for those functions that
 *   are passed the arguments from main().
 */
#define MAIN_FUNCTION_PARAMETERS void
#define MAIN_FUNCTION_ARGUMENTS  


#ifndef __SOURCEFILE__
  /**
   * @brief The __SOURCEFILE__ macro is used by asserts to list the
   * filename if it isn't otherwise defined, set it to the compiler intrinsic
   * which specifies the whole filename and path of the sourcefile
   */
  #define __SOURCEFILE__ __FILE__
#endif


#undef assert
#if !defined(SIMPLER_ASSERT_REBOOT) || defined(DOXYGEN_SHOULD_SKIP_THIS)
  /**
   * @brief A prototype definition for use by the assert macro.
   */
  void halInternalAssertFailed(const char * filename, int linenumber);
  
  /**
   * @brief A custom implementation of the C language assert macro.
   * This macro implements the conditional evaluation and calls the function
   * halInternalAssertFailed().
   */
  #define assert(condition)                                    \
        do {                                                   \
          if (! (condition)) {                                 \
            halInternalAssertFailed(__SOURCEFILE__, __LINE__); \
          }                                                    \
        } while(0)
#else
  #define assert(condition) \
            do { if( !(condition) ) while(1){} } while(0)
#endif


/**
 * @brief Stub for code not running in simulation.
 */
#define simulatedTimePasses()
/**
 * @brief Stub for code not running in simulation.
 */
#define simulatedTimePassesMs(x)
/**
 * @brief Stub for code not running in simulation.
 */
#define simulatedSerialTimePasses()

/**
 * @brief Convinience macro for turning a token into a string
 */
#define STRINGIZE(X) #X

/**
 * @brief Provide a portable way to align data.
 */
#define ALIGNMENT(X) \
  _Pragma( STRINGIZE( data_alignment=##X## ) )

////////////////////////////////////////////////////////////////////////////////
//@}  // end of Miscellaneous Macros
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
/** \name Global Interrupt Manipulation Macros
 */
////////////////////////////////////////////////////////////////////////////////
//@{

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    //The concept of LITE atomic handling isn't implemented on this platform,
    //so just redirect to the normal atomic handling.
    #define ATOMIC_LITE(blah)             ATOMIC(blah)
    #define DECLARE_INTERRUPT_STATE_LITE  DECLARE_INTERRUPT_STATE
    #define DISABLE_INTERRUPTS_LITE()     DISABLE_INTERRUPTS()
    #define RESTORE_INTERRUPTS_LITE()     RESTORE_INTERRUPTS()
    
    /**
     * @brief This macro should be called in the local variable
     * declarations section of any function which calls DISABLE_INTERRUPTS()
     * or RESTORE_INTERRUPTS().
     */
    #define DECLARE_INTERRUPT_STATE uint32_t _emIsrState
#endif  // DOXYGEN_SHOULD_SKIP_THIS

//The core Global Interrupt Manipulation Macros start here.

/**
 * @brief Disable interrupts, saving the previous state so it can be
 * later restored with RESTORE_INTERRUPTS().
 * \note Do not fail to call RESTORE_INTERRUPTS().
 * \note It is safe to nest this call.
 */
#define DISABLE_INTERRUPTS()       \
  do {                             \
    _emIsrState = __get_PRIMASK(); \
    __set_PRIMASK(1);              \
  } while(0)


/** 
 * @brief Restore the global interrupt state previously saved by
 * DISABLE_INTERRUPTS()
 * \note Do not call without having first called DISABLE_INTERRUPTS()
 * to have saved the state.
 * \note It is safe to nest this call.
 */
#define RESTORE_INTERRUPTS()    \
  do {                          \
    __set_PRIMASK(_emIsrState); \
  } while(0)


/**
 * @brief Enable global interrupts without regard to the current or
 * previous state.
 */
#define INTERRUPTS_ON() \
  do {                  \
    __set_PRIMASK(0);   \
  } while(0)


/**
 * @brief Disable global interrupts without regard to the current or
 * previous state.
 */
#define INTERRUPTS_OFF() \
  do {                   \
    __set_PRIMASK(1);    \
  } while(0)


/**
 * @returns true if global interrupts are disabled.
 */
#define INTERRUPTS_ARE_OFF() (__get_PRIMASK() != 0)


/**
 * @returns true if global interrupt flag was enabled when 
 * ::DISABLE_INTERRUPTS() was called.
 */
#define INTERRUPTS_WERE_ON() (_emIsrState == 0)


/**
 * @brief A block of code may be made atomic by wrapping it with this
 * macro.  Something which is atomic cannot be interrupted by interrupts.
 */
#define ATOMIC(blah)       \
{                          \
  DECLARE_INTERRUPT_STATE; \
  DISABLE_INTERRUPTS();    \
  { blah }                 \
  RESTORE_INTERRUPTS();    \
}


/**
 * @brief Allows any pending interrupts to be executed. Usually this
 * would be called at a safe point while interrupts are disabled (such as
 * within an ISR).
 * 
 * Takes no action if interrupts are already enabled.
 */
#define HANDLE_PENDING_INTERRUPTS() \
  do {                              \
    if (INTERRUPTS_ARE_OFF()) {     \
      INTERRUPTS_ON();              \
      INTERRUPTS_OFF();             \
    }                               \
 } while (0)

////////////////////////////////////////////////////////////////////////////////
//@}  // end of Global Interrupt Manipulation Macros
////////////////////////////////////////////////////////////////////////////////


/**
 * @name Generic Types
 *@{
 */
//true and FLASE are defined in ST's HAL Library

#ifndef NULL
/**
 * @brief The null pointer.
 */
#define NULL ((void *)0)
#endif

//@} \\END Generic Types


/**
 * Include string.h for the C Standard Library memory routines used in
 * platform-common.
 */
#include <string.h>


/**
 * @brief Include platform-common.h last to pick up defaults and common definitions.
 */
#define PLATCOMMONOKTOINCLUDE
  #include "hal/host/generic/compiler/platform-common.h"
#undef PLATCOMMONOKTOINCLUDE

#endif // __IAR_ST_H__

/**@} //END addtogroup 
 */

