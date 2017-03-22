/***************************************************************************//**
 * @file btl_crc32.c
 * @brief CRC32 functionality for Silicon Labs bootloader
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
#include "btl_crc32.h"

#define CRC32_POLYNOMIAL              (0xEDB88320UL)

uint32_t btl_crc32(const uint8_t newByte, uint32_t prevResult)
{
  uint8_t jj;
  uint32_t previous;
  uint32_t oper;

  previous = (prevResult >> 8u) & 0x00FFFFFFUL;
  oper = (prevResult ^ newByte) & 0xFFu;
  for (jj = 0; jj < 8u; jj++) {
    oper = ((oper & 0x01u) != 0u
                ? ((oper >> 1u) ^ CRC32_POLYNOMIAL)
                : (oper >> 1u));
  }

  return (previous ^ oper);
}

uint32_t btl_crc32Stream(const uint8_t* buffer, size_t length, uint32_t prevResult)
{
  size_t position = 0;
  for(;position < length; position++) {
    prevResult = btl_crc32(buffer[position], prevResult);
  }

  return prevResult;
}
