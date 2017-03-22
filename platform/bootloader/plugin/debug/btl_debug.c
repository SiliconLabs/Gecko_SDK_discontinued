/***************************************************************************//**
 * @file btl_debug.c
 * @brief Debug plugin for Silicon Labs Bootloader.
 * @author Silicon Labs
 * @version 1.0.0
 *******************************************************************************
 * @section License
 * <b>Copyright 2016 Silicon Laboratories, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/
#include "btl_debug.h"
#include <stdbool.h>

#if defined(BTL_PLUGIN_DEBUG_ASSERT_VERBOSE)
SL_NORETURN void btl_assert(const char* file, int line)
{
  // Reset here
#ifdef BTL_PLUGIN_DEBUG_PRINT
  btl_debugWriteString("Assert: (");
  btl_debugWriteString(file);
  btl_debugWriteString(": ");
  btl_debugWriteInt(line);
  btl_debugWriteChar(')');
  btl_debugWriteString(BTL_DEBUG_NEWLINE);
#else
  (void)file;
  (void)line;
#endif
  while (true) {
  }
}
#else
SL_NORETURN void btl_assert(void)
{
  // Reset here
#ifdef BTL_PLUGIN_DEBUG_PRINT
  btl_debugWriteLine("Assert!");
#endif
  while (true) {
  }
}
#endif

#if defined (BTL_PLUGIN_DEBUG_PRINT)

#include <string.h>

static void btl_debugWriteNibbleHex(uint8_t nibble)
{
  nibble = nibble > 9u ? nibble + 0x37u : nibble + 0x30u;
  BTL_DEBUG_PRINTC((char)nibble);
}

void btl_debugWriteCharHex(uint8_t number)
{
  btl_debugWriteNibbleHex((number & 0xF0u) >> 4);
  btl_debugWriteNibbleHex((number & 0x0Fu) >> 0);
}

void btl_debugWriteShortHex(uint16_t number)
{
  btl_debugWriteCharHex((uint8_t)((number & 0xFF00u) >> 8));
  btl_debugWriteCharHex((uint8_t)(number & 0x00FFu));
}

void btl_debugWriteWordHex(uint32_t number)
{
  btl_debugWriteShortHex((uint16_t)((number & 0xFFFF0000UL) >> 16));
  btl_debugWriteShortHex((uint16_t)(number & 0x0000FFFFUL));
}

void btl_debugWriteInt(int number)
{
  char buffer[10] = {0};
  size_t position = 0;
  uint32_t digit;
  uint32_t unsignedNumber;
  bool negative = false;

  if (number < 0) {
    negative = true;
    number = 0L - number;
  }
  unsignedNumber = (uint32_t)number;

  do {
    digit = unsignedNumber;
    unsignedNumber /= 10u;
    digit -= unsignedNumber * 10u;
    buffer[position] = (char)digit;
    position++;
  } while (unsignedNumber != 0u);

  if (negative) {
    btl_debugWriteChar('-');
  }

  do {
    position--;
    btl_debugWriteChar(buffer[position] + 30u);
  } while (position != 0u);
}

void btl_debugWriteString(const char * s)
{
  size_t position = 0;
  while (s[position] != (char)0u) {
    btl_debugWriteChar(s[position]);
    position++;
  }
}

void btl_debugWriteLine(const char * s)
{
  btl_debugWriteString(s);
  btl_debugWriteString(BTL_DEBUG_NEWLINE);
}

void btl_debugWriteNewline(void)
{
  btl_debugWriteString(BTL_DEBUG_NEWLINE);
}

#endif // BTL_PLUGIN_DEBUG_PRINT
