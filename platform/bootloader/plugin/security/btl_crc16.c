/***************************************************************************//**
 * @file btl_crc16.c
 * @brief CRC16 functionality for Silicon Labs bootloader
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
#include "btl_crc16.h"

uint16_t btl_crc16(const uint8_t newByte, uint16_t prevResult)
{
  prevResult = (prevResult >> 8) | (prevResult << 8);
  prevResult ^= newByte;
  prevResult ^= (prevResult & 0xff) >> 4;
  prevResult ^= (prevResult << 8) << 4;

  //[[ What I wanted is the following function:
  // prevResult ^= ((prevResult & 0xff) << 4) << 1;
  // Unfortunately the compiler does this in 46 cycles.  The next line of code
  // does the same thing, but the compiler uses only 10 cycles to implement it.
  //]]
  prevResult ^= ((uint8_t) ( (uint8_t) ( (uint8_t) (prevResult & 0xff) ) << 5))
              | ((uint16_t) ( (uint8_t) ( (uint8_t) (prevResult & 0xff)) >> 3) << 8);

  return prevResult;
}

uint16_t btl_crc16Stream(const uint8_t* buffer, size_t length, uint16_t prevResult)
{
  size_t position = 0;
  for(;position < length; position++) {
    prevResult = btl_crc16(buffer[position], prevResult);
  }

  return prevResult;
}
