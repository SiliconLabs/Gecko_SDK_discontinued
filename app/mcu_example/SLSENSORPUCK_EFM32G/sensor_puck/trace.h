/**************************************************************************//**
* @file
* @brief Trace buffer for debug
* @version 5.1.2

******************************************************************************
* @section License
* <b>Copyright 2015 Silicon Labs, Inc. http://www.silabs.com</b>
*******************************************************************************
*
* This file is licensed under the Silabs License Agreement. See the file
* "Silabs_License_Agreement.txt" for details. Before using this software for
* any purpose, you must agree to the terms of that agreement.
*
******************************************************************************/

#ifndef TRACE_H_
#define TRACE_H_

/* Set to 0 to remove the trace debugging capability */
#define TRACE_INCLUDE    1

#if TRACE_INCLUDE == 1

#define TRACE_BUFF_SIZE    1008

void TraceInit(void);
void TraceByte(uint8_t Byte);
void TraceWord(uint16_t Word);
void TraceDWord(uint32_t DWord);

#else

#define TRACE_BUFF_SIZE    0

#define TraceInit()
#define TraceByte(Byte)
#define TraceWord(Word)
#define TraceDWord(DWord)
#endif /* TRACE_INCLUDE */
#endif /* TRACE_H_ */
