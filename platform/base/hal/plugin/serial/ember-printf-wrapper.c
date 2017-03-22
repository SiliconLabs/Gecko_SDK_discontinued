/**
 * File: ember-printf-wrapper.c
 * Description: A wrapper for the Ember minimalistic implementation of printf()
 *   to call standard printf() format specifiers.  This saves space if 
 *   the application wants to use the standard printf(), since there is no
 *   point in including the Ember printf implementation. 
 *
 *
 * Copyright 2014 by Silicon Labs.  All rights reserved.                   *80*
 */

#include PLATFORM_HEADER
#include "stack/include/ember-types.h"
#include "stack/include/error.h"

#include "hal/hal.h"
#include "serial/serial.h"

#include <stdarg.h>
#include <stdio.h>

//=============================================================================
// Globals

#if defined(EMBER_AF_PLUGIN_STANDARD_PRINTF_SUPPORT)
  #define MAX_STRING_REWRITE_SIZE EMBER_AF_PLUGIN_STANDARD_PRINTF_SUPPORT_MAX_BUFFER_LENGTH
#else
  #define MAX_STRING_REWRITE_SIZE 80
#endif

#if !defined(APP_SERIAL)
 #define APP_SERIAL serialPort
#endif

//=============================================================================

EmberStatus emberSerialWriteHex(uint8_t port, uint8_t dataByte)
{
  printf("%02x", dataByte);
  return EMBER_SUCCESS;
}

EmberStatus emberSerialPrintCarriageReturn(uint8_t port)
{
  printf("\n");
  return EMBER_SUCCESS;
}

// Convert Ember Printf format specifiers to stardard specifiers
//   %x  => %02x
//   %2x => %04x
//   %4x => %08x
//   %p  => %s

EmberStatus emberSerialPrintfVarArg(uint8_t port, PGM_P formatString, va_list ap)
{
  char newFormatString[MAX_STRING_REWRITE_SIZE + 1];  // +1 to allow for '\0' at end
  const char* ptr = formatString;
  uint16_t length = strlen(formatString);
  if (length > MAX_STRING_REWRITE_SIZE) {
    length = MAX_STRING_REWRITE_SIZE;
  }
  uint16_t newFormatIndex = 0;
  newFormatString[MAX_STRING_REWRITE_SIZE] = '\0';
  bool previousWasPercentSymbol = false;
  uint8_t byteToWrite;
  while (*ptr != '\0'
         && newFormatIndex < MAX_STRING_REWRITE_SIZE) {
    byteToWrite = *ptr;
    if (!previousWasPercentSymbol && *ptr == '%') {
      previousWasPercentSymbol = true;
    } else if (previousWasPercentSymbol) {
      previousWasPercentSymbol = false;
      switch(byteToWrite) {
        case 'p':
          byteToWrite = 's';
          break;

        case 'x':
        case 'X':  
          newFormatString[newFormatIndex++] = '0';
          newFormatString[newFormatIndex++] = '2';
          break;

        case '2':
        case '4':
          if (*(ptr+1) == 'x'
              || *(ptr+1) == 'X') {
            newFormatString[newFormatIndex++] = '0';
            byteToWrite = (byteToWrite == '2'
                           ? '4'
                           : '8');
          }
          break;

        default:
          break;
      }
    }
    newFormatString[newFormatIndex++] = byteToWrite;
    ptr++;
  }
  newFormatString[newFormatIndex] = '\0';
  vprintf(newFormatString, ap);
  return EMBER_SUCCESS;
}

EmberStatus emberSerialPrintf(uint8_t port, PGM_P formatString, ...)
{
  EmberStatus status;
  va_list ap;
  va_start (ap, formatString);
  status = emberSerialPrintfVarArg(port, formatString, ap);
  va_end (ap);
  return status;
}

EmberStatus emberSerialPrintfLine(uint8_t port, PGM_P formatString, ...)
{
  EmberStatus status;
  va_list ap;
  va_start (ap, formatString);
  status = emberSerialPrintfVarArg(port, formatString, ap);
  va_end (ap);
  printf("\n");
  return status;
}

EmberStatus emberSerialGuaranteedPrintf(uint8_t port, PGM_P formatString, ...)
{
  EmberStatus status;
  va_list ap;
  va_start (ap, formatString);
  status = emberSerialPrintfVarArg(port, formatString, ap);
  va_end (ap);
  printf("\n");
  return status;
}

// The low-level output routine for IAR's printf().  

size_t __write(int handle, const unsigned char * buffer, size_t size)
{
  // A little white lie.  We don't know how many bytes were actually written,
  // so we pretend it was 0.  I don't know of code that handles anything
  // other than expectedSize != actualSize, in other words to handle recovery
  // of partial writes.
  return (EMBER_SUCCESS == emberSerialWriteData(APP_SERIAL,
                                                (uint8_t*)buffer,
                                                size)
          ? size
          : 0);
}

