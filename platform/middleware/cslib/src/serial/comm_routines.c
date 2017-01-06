/**************************************************************************//**
 * Copyright 2016 by Silicon Laboratories Inc. All rights reserved.
 *
 * http://developer.silabs.com/legal/version/v11/Silicon_Labs_Software_License_Agreement.txt
 *****************************************************************************/

#include "comm_routines.h"

#include <stdio.h>
#include <stdlib.h>

#include "em_device.h"
#include "retargetvirtual.h"


/***************************************************************************//**
 * @brief
 *   Hardware init of serial port
 *
 *   This routine fully initializes the serial port for the device.
 *   It also retargets printf output to this interface, which is then
 *   used by the virtual COMM port on STKs in code examples.
 ******************************************************************************/
void CSLIB_commInit(void)
{
  // Initialize USART and map LF to CRLF
  RETARGET_SerialInit();
  RETARGET_SerialCrLf(1);
}

/***************************************************************************//**
 * @brief
 *   Causes serial interface to block until transfer completes
 *
 *   This routine forces the serial interface output to be a blocking
 *   operation.  This is necessary because otherwise the system may go
 *   into a low power state before all bytes have transmitted.
 *
 ******************************************************************************/
void BlockWhileTX(void)
{
  RETARGET_BlockOnTX();
}

/***************************************************************************//**
 * @brief
 *   Prints header line in interface for one data type
 *
 *   The capacitive sensing profiler output begins with a line that describes
 *   the data types being transmitted and the number of elements of each data
 *   type.  This function outputs information about one data type per execution.
 *
 ******************************************************************************/
void outputHeaderCount(HeaderStruct_t headerEntry)
{
  uint8_t index;
  // If more than one element is defined for this data type,
  // then output '_XX' after the name, where 'XX' increments
  // sequentially from 0 for the datatype
  if(headerEntry.instances > 1) {
    for(index = 0; index < headerEntry.instances; index++) {
      printf("%s_%d ", headerEntry.header,index);
    }
  }
  // If only one element exists for this data type, only output the
  // data type name and leave off the '_'
  else {
    printf("%s ", headerEntry.header);
  }
}

/***************************************************************************//**
 * @brief
 *   Prints header line in interface for one data type
 *
 *   The capacitive sensing profiler output begins with a line that describes
 *   the data types being transmitted and the number of elements of each data
 *   type.  This function outputs information about one data type per execution.
 *
 ******************************************************************************/
void outputBreak(void)
{
  printf(" | ");
}

/***************************************************************************//**
 * @brief
 *   Outputs | between columns when needed between data types in header
 *
 *   The serial interfaces header line needs '|' separators between
 *   each defined data type.  This function outputs those separators.
 *
 ******************************************************************************/
void outputBeginHeader(void)
{
  printf("*HEADER ");
}
/***************************************************************************//**
 * @brief
 *   Outputs a carriage return
 *
 *   This function is called in a number of places in serial interface output.
 *   Included here to condense all calls to a single printf() call in Flash.
 *
 ******************************************************************************/
void outputNewLine(void)
{
  printf("\n");
}
