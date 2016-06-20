/**************************************************************************//**
* @file
* @brief Trace buffer for debug
* @version 4.1.0

******************************************************************************
* @section License
* <b>(C) Copyright 2014 Silicon Labs, http://www.silabs.com</b>
*******************************************************************************
*
* This file is licensed under the Silabs License Agreement. See the file
* "Silabs_License_Agreement.txt" for details. Before using this software for
* any purpose, you must agree to the terms of that agreement.
*
******************************************************************************/

#include <stdint.h>
#include "em_device.h"
#include "em_msc.h"
#include "em_int.h"
#include "Trace.h"


#if TRACE_INCLUDE == 1

uint8_t *pTrace;
uint8_t *pTraceLimit;
uint8_t TraceBuff[TRACE_BUFF_SIZE];

#define TRACE_FLASH_BASE  (uint32_t*)0x10000
#define TRACE_FLASH_LIMIT TRACE_FLASH_BASE+TRACE_BUFF_SIZE/4


/**************************************************************************//**
 * @brief  TraceInit initializes the debug buffer
 *****************************************************************************/
void TraceInit(void)
{
  pTraceLimit = &TraceBuff[TRACE_BUFF_SIZE];

  for (pTrace = TraceBuff; pTrace < pTraceLimit; pTrace++)
    *pTrace = 0;

  pTrace = TraceBuff;
}


/**************************************************************************//**
 * @brief  TraceSave
 *****************************************************************************/
void TraceSave(void)
{
  uint32_t *pFlash;
   
  INT_Disable();
  MSC_Init();

  for ( pFlash=TRACE_FLASH_BASE; pFlash<TRACE_FLASH_LIMIT; pFlash+=FLASH_PAGE_SIZE/4 )
  {
    MSC_ErasePage( pFlash );
  }

  MSC_WriteWord( TRACE_FLASH_BASE, TraceBuff, TRACE_BUFF_SIZE );
  MSC_Deinit();
  INT_Enable();
}


/**************************************************************************//**
 * @brief  TraceByte puts a byte in the debug buffer
 *****************************************************************************/
void TraceByte(uint8_t Byte)
{
  *pTrace++ = Byte;

  if (pTrace == pTraceLimit)
    pTrace = TraceBuff;
}


/**************************************************************************//**
 * @brief  TraceWord puts a word in the debug buffer
 *****************************************************************************/
void TraceWord(uint16_t Word)
{
  TraceByte((Word >> 8) & 0xFF);
  TraceByte(Word & 0xFF);
}


/**************************************************************************//**
 * @brief  TraceDWord puts a 32 byte value in the debug buffer
 *****************************************************************************/
void TraceDWord(uint32_t DWord)
{
  TraceWord((DWord >> 16) & 0xFFFF);
  TraceWord(DWord & 0xFFFF);
}
#endif


