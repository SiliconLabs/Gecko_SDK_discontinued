/** @file hal/micro/cortexm3/compiler/gcc.h
 * @brief Compiler/platform-specific definitions and typedefs for the
 * GCC ARM C compiler.
 *
 * This file should be included first in all source files by
 * setting the preprocessor macro PLATFORM_HEADER to point to it.
 *
 * <!-- Copyright 2008 by Ember Corporation. All rights reserved. -->
 */
#ifndef __GCC_H__
#define __GCC_H__

#ifndef __GNUC__
  #error Improper PLATFORM_HEADER
#endif

#ifndef DOXYGEN_SHOULD_SKIP_THIS
  #include <stddef.h>
  #include <stdarg.h>
  #include <stdint.h>
  #include <stdbool.h>
  #if defined (CORTEXM3_EMBER_MICRO)
    #include "micro/cortexm3/em35x/regs.h"
    #include "micro/cortexm3/micro-features.h"
    #include "interrupts-em3xx.h"
  #elif defined (CORTEXM3_EFM32_MICRO)
    // EFR32
    #include "em_device.h"  
    #include "micro/cortexm3/efm32/regs.h"
    #include "micro/cortexm3/micro-features.h"
    #define NVIC_CONFIG "hal/micro/cortexm3/efm32/nvic-config.h"
    #include "interrupts-efm32.h"
  #else
    #error Unknown CORTEXM3 micro
  #endif
  //Provide a default NVIC configuration file.  The build process can
  //override this if it needs to.
  #ifndef NVIC_CONFIG
    #define NVIC_CONFIG "hal/micro/cortexm3/nvic-config.h"
  #endif
//[[
#ifdef  EMBER_EMU_TEST
  #ifdef  I_AM_AN_EMULATOR
    // This register is defined for both the chip and the emulator with
    // with distinct reset values.  Need to undefine to avoid preprocessor
    // collision.
    #undef DATA_EMU_REGS_BASE
    #undef DATA_EMU_REGS_END
    #undef DATA_EMU_REGS_SIZE
    #undef I_AM_AN_EMULATOR
    #undef I_AM_AN_EMULATOR_REG
    #undef I_AM_AN_EMULATOR_ADDR
    #undef I_AM_AN_EMULATOR_RESET
    #undef I_AM_AN_EMULATOR_I_AM_AN_EMULATOR
    #undef I_AM_AN_EMULATOR_I_AM_AN_EMULATOR_MASK
    #undef I_AM_AN_EMULATOR_I_AM_AN_EMULATOR_BIT
    #undef I_AM_AN_EMULATOR_I_AM_AN_EMULATOR_BITS
  #endif//I_AM_AN_EMULATOR
  #if defined (CORTEXM3_EMBER_MICRO)
    #include "micro/cortexm3/em35x/regs-emu.h"
    #include "micro/cortexm3/micro-features.h"
  #else
    #error MICRO currently not supported for emulator builds.
  #endif
#endif//EMBER_EMU_TEST
//]]
#endif  // DOXYGEN_SHOULD_SKIP_THIS


/** \name Master Variable Types
 * These are a set of typedefs to make the size of all variable declarations
 * explicitly known.
 */
//@{
/**
 * @description A typedef to make the size of the variable explicitly known.
 */
typedef bool boolean; /*To ease adoption of bool instead of boolean.*/
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


#define _HAL_USE_COMMON_PGM_


////////////////////////////////////////////////////////////////////////////////
/** \name Miscellaneous Macros
 */
////////////////////////////////////////////////////////////////////////////////
//@{

/**
 * @description A convenient method for code to know what endiannes processor
 * it is running on.  For the Cortex-M3, we are little endian.
 */
#define BIGENDIAN_CPU  false


/**
 * @description A friendlier name for the compiler's intrinsic for not
 * stripping.
 */
#define NO_STRIPPING __attribute__((used))


/**
 * @description A friendlier name for the compiler's intrinsic for eeprom
 * reference.
 */
#define EEPROM


#ifndef __SOURCEFILE__
  /**
   * @description The __SOURCEFILE__ macro is used by asserts to list the
   * filename if it isn't otherwise defined, set it to the compiler intrinsic
   * which specifies the whole filename and path of the sourcefile
   */
  #define __SOURCEFILE__ __FILE__
#endif


#undef assert
/**
 * @description A prototype definition for use by the assert macro. (see
 * hal/micro/micro.h)
 */
void halInternalAssertFailed(const char *filename, int linenumber);

/**
 * @brief A custom implementation of the C language assert macro.
 * This macro implements the conditional evaluation and calls the function
 * halInternalAssertFailed(). (see hal/micro/micro.h)
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
#define assert(condition)
#else //DOXYGEN_SHOULD_SKIP_THIS
// Don't define PUSH_REGS_BEFORE_ASSERT if it causes problems with the compiler.
// For example, in some compilers any inline assembly disables all optimization.
//
// For IAR V5.30, inline assembly apparently does not affect compiler output.
//#define PUSH_REGS_BEFORE_ASSERT
#ifdef PUSH_REGS_BEFORE_ASSERT
#define assert(condition) \
      do { if (! (condition)) { \
        asm("PUSH {R0,R1,R2,LR}"); \
        halInternalAssertFailed(__SOURCEFILE__, __LINE__); } } while(0)
#else
#define assert(condition) \
      do { if (! (condition)) { \
        halInternalAssertFailed(__SOURCEFILE__, __LINE__); } } while(0)
#endif
#endif //DOXYGEN_SHOULD_SKIP_THIS

/**
 * @brief Macro to reset the watchdog timer.  Note:  be very very
 * careful when using this as you can easily get into an infinite loop if you
 * are not careful.
 */
void halInternalResetWatchDog(void);
#ifdef RTOS
  void rtosResetWatchdog(void);
  #define halResetWatchdog()  rtosResetWatchdog()
#else
  #define halResetWatchdog()  halInternalResetWatchDog()
#endif //RTOS

/**
 * @description Declare a variable as unused to avoid a warning.  Has no effect
 * in IAR builds
 */
#define UNUSED __attribute__ ((unused))

/**
 * @brief Some platforms need to cast enum values that have the high bit set.
 */
#define SIGNED_ENUM

#define STACK_FILL_VALUE  0xCDCDCDCD
#ifdef RAMEXE
  //If the whole build is running out of RAM, as chosen by the RAMEXE build
  //define, then define RAMFUNC to nothing since it's not needed.
  #define RAMFUNC
#else //RAMEXE
  #define RAMFUNC __attribute__ ((long_call, section (".data.ramfunc")))
#endif //RAMEXE
#define asm(x) __asm__(x)
#define NO_OPERATION() __asm__("nop")

/**
 * @brief A convenience macro that makes it easy to change the field of a
 * register to any value.
 */
#define SET_REG_FIELD(reg, field, value)                      \
  do{                                                         \
    reg = ((reg & (~field##_MASK)) | (value << field##_BIT)); \
  }while(0)

/**
 * @description Stub for code not running in simulation.
 */
#define simulatedTimePasses()
/**
 * @description Stub for code not running in simulation.
 */
#define simulatedTimePassesMs(x)
/**
 * @description Stub for code not running in simulation.
 */
#define simulatedSerialTimePasses()


#define _HAL_USE_COMMON_DIVMOD_


/**
 * @brief Provide a portable way to specify the segment where a variable
 * lives.
 */
#define VAR_AT_SEGMENT(__variableDeclaration, __segmentName) \
  __variableDeclaration __attribute__ ((section (__segmentName)))

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

/**
 * @brief For compatibility with IAR ignore the __no_init attribute
 */
#define __no_init

/**
 * @brief Portable Segment names
 */
#define __NO_INIT__                         ".noinit"
#define __DEBUG_CHANNEL__                   ".debugChannel"
#define __INTVEC__                          ".intvec"
#define __CSTACK__                          ".cstack"
#define __RESETINFO__                       ".resetinfo"
#define __DATA_INIT__                       ".data_init"
#define __DATA__                            ".data"
#define __BSS__                             ".bss"
#define __APP_RAM__                         "APP_RAM"
#define __CONST__                           ".rodata"
#define __TEXT__                            ".text"
#define __TEXTRW_INIT__                     ".textrw_init"
#define __TEXTRW__                          ".textrw"
#define __AAT__                             ".aat"                // Application address table
#define __BAT_INIT__                        ".bat.init"           // Bootloader  address table
#define __BAT__                             ".bat.noinit"         // Bootloader  address table
#define __FAT__                             ".fat"                // Fixed       address table
#define __RAT__                             ".rat"                // Ramexe      address table
#define __NVM__                             ".nvm"                // Non-Volatile Memory storage
#define __SIMEE__                           ".simee"              // Simulated EEPROM storage
#define __PSSTORE__                         ".psstore"            // PS Store storage
#define __EMHEAP__                          ".emheap"             // Ember Heap region
#define __EMHEAP_OVERLAY__                  "EMHEAP_overlay"      // Heap/reset info overlay
#define __GUARD_REGION__                    ".guard_region"       // Guard page
#define __INTERNAL_STORAGE__                ".internal_storage"   // Internal storage region
#define __UNRETAINED_RAM__                  "UNRETAINED_RAM"      // RAM not retained in sleep

/**
 * @brief Linker defined variables for each region's start/end/size values.
 */
extern uint32_t __NO_INIT__begin, __NO_INIT__end, __NO_INIT__size;
extern uint32_t __DEBUG_CHANNEL__begin, __DEBUG_CHANNEL__end, __DEBUG_CHANNEL__size;
extern uint32_t __INTVEC__begin, __INTVEC__end, __INTVEC__size;
extern uint32_t __CSTACK__begin, __CSTACK__end, __CSTACK__size;
extern uint32_t __RESETINFO__begin, __RESETINFO__end, __RESETINFO__size;
extern uint32_t __DATA_INIT__begin, __DATA_INIT__end, __DATA_INIT__size;
extern uint32_t __DATA__begin, __DATA__end, __DATA__size;
extern uint32_t __BSS__begin, __BSS__end, __BSS__size;
extern uint32_t __APP_RAM__begin, __APP_RAM__end, __APP_RAM__size;
extern uint32_t __CONST__begin, __CONST__end, __CONST__size;
extern uint32_t __TEXT__begin, __TEXT__end, __TEXT__size;
extern uint32_t __TEXTRW_INIT__begin, __TEXTRW_INIT__end, __TEXTRW_INIT__size;
extern uint32_t __TEXTRW__begin, __TEXTRW__end, __TEXTRW__size;
extern uint32_t __AAT__begin, __AAT__end, __AAT__size;
extern uint32_t __BAT_INIT__begin, __BAT_INIT__end, __BAT_INIT__size;
extern uint32_t __BAT__begin, __BAT__end, __BAT__size;
extern uint32_t __FAT__begin, __FAT__end, __FAT__size;
extern uint32_t __RAT__begin, __RAT__end, __RAT__size;
extern uint32_t __NVM__begin, __NVM__end, __NVM__size;
extern uint32_t __SIMEE__begin, __SIMEE__end, __SIMEE__size;
extern uint32_t __PSSTORE__begin, __PSSTORE__end, __PSSTORE__size;
extern uint32_t __EMHEAP__begin, __EMHEAP__end, __EMHEAP__size;
extern uint32_t __EMHEAP_OVERLAY__begin, __EMHEAP_OVERLAY__end, __EMHEAP_OVERLAY__size;
extern uint32_t __GUARD_REGION__begin, __GUARD_REGION__end, __GUARD_REGION__size;
extern uint32_t __INTERNAL_STORAGE__begin, __INTERNAL_STORAGE__end, __INTERNAL_STORAGE__size;
extern uint32_t __UNRETAINED_RAM__begin, __UNRETAINED_RAM__end, __UNRETAINED_RAM__size;

/**
 * @brief Defines for each segement's begin/end/size. These are what we should
 * use in code because they're portable across the different compilers
 */
#define _NO_INIT_SEGMENT_BEGIN          (&__NO_INIT__begin)
#define _DEBUG_CHANNEL_SEGMENT_BEGIN    (&__DEBUG_CHANNEL__begin)
#define _INTVEC_SEGMENT_BEGIN           (&__INTVEC__begin)
#define _CSTACK_SEGMENT_BEGIN           (&__CSTACK__begin)
#define _RESETINFO_SEGMENT_BEGIN        (&__RESETINFO__begin)
#define _DATA_INIT_SEGMENT_BEGIN        (&__DATA_INIT__begin)
#define _DATA_SEGMENT_BEGIN             (&__DATA__begin)
#define _BSS_SEGMENT_BEGIN              (&__BSS__begin)
#define _APP_RAM_SEGMENT_BEGIN          (&__APP_RAM__begin)
#define _CONST_SEGMENT_BEGIN            (&__CONST__begin)
#define _TEXT_SEGMENT_BEGIN             (&__TEXT__begin)
#define _TEXTRW_INIT_SEGMENT_BEGIN      (&__TEXTRW_INIT__begin)
#define _TEXTRW_SEGMENT_BEGIN           (&__TEXTRW__begin)
#define _AAT_SEGMENT_BEGIN              (&__AAT__begin)
#define _BAT_INIT_SEGMENT_BEGIN         (&__BAT_INIT__begin)
#define _BAT_SEGMENT_BEGIN              (&__BAT__begin)
#define _FAT_SEGMENT_BEGIN              (&__FAT__begin)
#define _RAT_SEGMENT_BEGIN              (&__RAT__begin)
#define _NVM_SEGMENT_BEGIN              (&__NVM__begin)
#define _SIMEE_SEGMENT_BEGIN            (&__SIMEE__begin)
#define _PSSTORE_SEGMENT_BEGIN          (&__PSSTORE__begin)
#define _EMHEAP_SEGMENT_BEGIN           (&__EMHEAP__begin)
#define _EMHEAP_OVERLAY_SEGMENT_BEGIN   (&__EMHEAP_OVERLAY__begin)
#define _GUARD_REGION_SEGMENT_BEGIN     (&__GUARD_REGION__begin)
#define _INTERNAL_STORAGE_SEGMENT_BEGIN (&__INTERNAL_STORAGE__begin)
#define _UNRETAINED_RAM_SEGMENT_BEGIN   (&__UNRETAINED_RAM__begin)

#define _NO_INIT_SEGMENT_END            (&__NO_INIT__end)
#define _DEBUG_CHANNEL_SEGMENT_END      (&__DEBUG_CHANNEL__end)
#define _INTVEC_SEGMENT_END             (&__INTVEC__end)
#define _CSTACK_SEGMENT_END             (&__CSTACK__end)
#define _RESETINFO_SEGMENT_END          (&__RESETINFO__end)
#define _DATA_INIT_SEGMENT_END          (&__DATA_INIT__end)
#define _DATA_SEGMENT_END               (&__DATA__end)
#define _BSS_SEGMENT_END                (&__BSS__end)
#define _APP_RAM_SEGMENT_END            (&__APP_RAM__end)
#define _CONST_SEGMENT_END              (&__CONST__end)
#define _TEXT_SEGMENT_END               (&__TEXT__end)
#define _TEXTRW_INIT_SEGMENT_END        (&__TEXTRW_INIT__end)
#define _TEXTRW_SEGMENT_END             (&__TEXTRW__end)
#define _AAT_SEGMENT_END                (&__AAT__end)
#define _BAT_INIT_SEGMENT_END           (&__BAT_INIT__end)
#define _BAT_SEGMENT_END                (&__BAT__end)
#define _FAT_SEGMENT_END                (&__FAT__end)
#define _RAT_SEGMENT_END                (&__RAT__end)
#define _NVM_SEGMENT_END                (&__NVM__end)
#define _SIMEE_SEGMENT_END              (&__SIMEE__end)
#define _PSSTORE_SEGMENT_END            (&__PSSTORE__end)
#define _EMHEAP_SEGMENT_END             (&__EMHEAP__end)
#define _EMHEAP_OVERLAY_SEGMENT_END     (&__EMHEAP_OVERLAY__end)
#define _GUARD_REGION_SEGMENT_END       (&__GUARD_REGION__end)
#define _INTERNAL_STORAGE_SEGMENT_END   (&__INTERNAL_STORAGE__end)
#define _UNRETAINED_RAM_SEGMENT_END     (&__UNRETAINED_RAM__end)

#define _NO_INIT_SEGMENT_SIZE           ((uint32_t)&__NO_INIT__size)
#define _DEBUG_CHANNEL_SEGMENT_SIZE     ((uint32_t)&__DEBUG_CHANNEL__size)
#define _INTVEC_SEGMENT_SIZE            ((uint32_t)&__INTVEC__size)
#define _CSTACK_SEGMENT_SIZE            ((uint32_t)&__CSTACK__size)
#define _RESETINFO_SEGMENT_SIZE         ((uint32_t)&__RESETINFO__size)
#define _DATA_INIT_SEGMENT_SIZE         ((uint32_t)&__DATA_INIT__size)
#define _DATA_SEGMENT_SIZE              ((uint32_t)&__DATA__size)
#define _BSS_SEGMENT_SIZE               ((uint32_t)&__BSS__size)
#define _APP_RAM_SEGMENT_SIZE           ((uint32_t)&__APP_RAM__size)
#define _CONST_SEGMENT_SIZE             ((uint32_t)&__CONST__size)
#define _TEXT_SEGMENT_SIZE              ((uint32_t)&__TEXT__size)
#define _TEXTRW_INIT_SEGMENT_SIZE       ((uint32_t)&__TEXTRW_INIT__size)
#define _TEXTRW_SEGMENT_SIZE            ((uint32_t)&__TEXTRW__size)
#define _AAT_SEGMENT_SIZE               ((uint32_t)&__AAT__size)
#define _BAT_INIT_SEGMENT_SIZE          ((uint32_t)&__BAT_INIT__size)
#define _BAT_SEGMENT_SIZE               ((uint32_t)&__BAT__size)
#define _FAT_SEGMENT_SIZE               ((uint32_t)&__FAT__size)
#define _RAT_SEGMENT_SIZE               ((uint32_t)&__RAT__size)
#define _NVM_SEGMENT_SIZE               ((uint32_t)&__NVM__size)
#define _SIMEE_SEGMENT_SIZE             ((uint32_t)&__SIMEE__size)
#define _PSSTORE_SEGMENT_SIZE           ((uint32_t)&__PSSTORE__size)
#define _EMHEAP_SEGMENT_SIZE            ((uint32_t)&__EMHEAP__size)
#define _EMHEAP_OVERLAY_SEGMENT_SIZE    ((uint32_t)&__EMHEAP_OVERLAY__size)
#define _GUARD_REGION_SEGMENT_SIZE      ((uint32_t)&__GUARD_REGION__size)
#define _INTERNAL_STORAGE_SEGMENT_SIZE  ((uint32_t)&__INTERNAL_STORAGE__size)
#define _UNRETAINED_RAM_SEGMENT_SIZE    ((uint32_t)&__UNRETAINED_RAM__size)

//A utility function for inserting barrier instructions.  These
//instructions should be used whenever the MPU is enabled or disabled so
//that all memory/instruction accesses can complete before the MPU changes
//state.
void _executeBarrierInstructions(void);

/**
 * @brief Returns the absolute value of I (also called the magnitude of I).
 * That is, if I is negative, the result is the opposite of I, but if I is
 * nonnegative the result is I.
 *
 * @param I  An integer.
 *
 * @return A nonnegative integer.
 */
int abs(int I);

////////////////////////////////////////////////////////////////////////////////
//@}  // end of Miscellaneous Macros
////////////////////////////////////////////////////////////////////////////////

#define _HAL_USE_COMMON_MEMUTILS_

// Include platform-common last to pick up defaults and common definitions
#define PLATCOMMONOKTOINCLUDE
  #include "hal/micro/generic/compiler/platform-common.h"
#undef PLATCOMMONOKTOINCLUDE

/**
 * @brief The kind of arguments the main function takes
 */
#define MAIN_FUNCTION_PARAMETERS void
#define MAIN_FUNCTION_ARGUMENTS 

#endif // __EMBER_CONFIG_H__
