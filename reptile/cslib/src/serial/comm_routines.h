/**************************************************************************//**
 * Copyright 2016 by Silicon Laboratories Inc. All rights reserved.
 *
 * http://developer.silabs.com/legal/version/v11/Silicon_Labs_Software_License_Agreement.txt
 *****************************************************************************/

#ifndef __COMM_ROUTINES_H__
#define __COMM_ROUTINES_H__
#include <stdint.h>
/** Saves string describing data types and number of data elements */
typedef struct{
   char* header;
   uint8_t instances;
}HeaderStruct_t;

/** Helper function for outputting a string */
uint8_t OutputString(uint8_t* buffer);

/** Hardware init of serial port */
void CSLIB_commInit(void);

/** Outputs | between columns when needed in ASCII serial protocol */
void outputBreak(void);

/** Outputs Profiler header with column information */
void outputBeginHeader(void);

/** Carriage return function */
void outputNewLine(void);

/** Causes serial interface to block until transfer completes */
void BlockWhileTX(void);

/** Top level function for serial interface tick, called in main() */
void CSLIB_commUpdate(void);

/** Prints header line in interface for one data type */
void outputHeaderCount(HeaderStruct_t headerEntry);

#endif // __COMM_ROUTINES_H__
