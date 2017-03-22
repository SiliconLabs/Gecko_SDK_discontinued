/***************************************************************************//**
 * @file btl_debug.h
 * @brief Debug plugin for Silicon Labs Bootloader.
 * @author Silicon Labs
 * @version 1.0.0
 *******************************************************************************
 * # License
 * <b>Copyright 2016 Silicon Laboratories, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#ifndef BTL_DEBUG_H
#define BTL_DEBUG_H

#include "config/btl_config.h"
#include "stdint.h"
#include "core/btl_util.h"

MISRAC_DISABLE
#include "em_common.h"
MISRAC_ENABLE

/***************************************************************************//**
 * @addtogroup Plugin
 * @{
 * @addtogroup Debug
 * @{
 * @brief Debug Plugin
 * @details
 *   This plugin provides the bootloader with support for debugging functions.
 *   The plugin implements two types of debugging functionality:
 *   * Defining BTL_PLUGIN_DEBUG_ASSERT enables assertions on compile-time
 *     configurable parameters in the bootloader
 *   * Defining BTL_PLUGIN_DEBUG_PRINT enables debug prints at strategic
 *     points in the code.
 ******************************************************************************/

#if defined(BTL_PLUGIN_DEBUG_ASSERT)
#if defined(BTL_PLUGIN_DEBUG_ASSERT_VERBOSE)
SL_NORETURN void btl_assert(const char* file, int line);
#define BTL_ASSERT(exp)    ((exp) ? ((void)0) : btl_assert(__FILE__, __LINE__))
#else
SL_NORETURN void btl_assert(void);
#define BTL_ASSERT(exp)    ((exp) ? ((void)0) : btl_assert())
#endif
#else
/// Assertion in bootloader
#define BTL_ASSERT(exp)      ((void)(exp))
#endif

// Print debug information throughout the bootloader
#if defined(BTL_PLUGIN_DEBUG_PRINT)

#ifndef BTL_DEBUG_NEWLINE
#define BTL_DEBUG_NEWLINE "\n"
#endif

void btl_debugInit(void);
void btl_debugWriteChar(char c);
void btl_debugWriteString(const char * s);
void btl_debugWriteLine(const char * s);

void btl_debugWriteCharHex(uint8_t number);
void btl_debugWriteShortHex(uint16_t number);
void btl_debugWriteWordHex(uint32_t number);
void btl_debugWriteInt(int number);
void btl_debugWriteNewline(void);

#define BTL_DEBUG_INIT()        (btl_debugInit())
#define BTL_DEBUG_PRINT(str)    (btl_debugWriteString(str))
#define BTL_DEBUG_PRINTLN(str)  (btl_debugWriteLine(str))
#define BTL_DEBUG_PRINTC(chr)   (btl_debugWriteChar(chr))

#define BTL_DEBUG_PRINT_CHAR_HEX(number)  (btl_debugWriteCharHex(number))
#define BTL_DEBUG_PRINT_SHORT_HEX(number) (btl_debugWriteShortHex(number))
#define BTL_DEBUG_PRINT_WORD_HEX(number)  (btl_debugWriteWordHex(number))
#define BTL_DEBUG_PRINT_LF()    (btl_debugWriteNewline())

#else // No debug prints

/// Initialize debug output
#define BTL_DEBUG_INIT()                  do {} while (0)
/// Print a string to debug out
#define BTL_DEBUG_PRINT(str)              do {} while (0)
/// Print a string followed by a newline to debug out
#define BTL_DEBUG_PRINTLN(str)            do {} while (0)
/// Print a character to debut out
#define BTL_DEBUG_PRINTC(chr)             do {} while (0)
/// Print a single hex byte
#define BTL_DEBUG_PRINT_CHAR_HEX(number)  do {} while (0)
/// Print two hex bytes
#define BTL_DEBUG_PRINT_SHORT_HEX(number) do {} while (0)
/// Print a hex word
#define BTL_DEBUG_PRINT_WORD_HEX(number)  do {} while (0)
/// Print a newline
#define BTL_DEBUG_PRINT_LF()              do {} while (0)

#endif


/**
 * @} // addtogroup Debug
 * @} // addtogroup Plugin
 */

#endif // BTL_DEBUG_H
