/*----------------------------------------------------------------------------
 *      RL-ARM - RTX
 *----------------------------------------------------------------------------
 *      Name:    main.cpp
 *      Purpose: C++ ostream + STL + exception demonstration
 *----------------------------------------------------------------------------
 *
 * Copyright (c) 1999-2009 KEIL, 2009-2013 ARM Germany GmbH
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  - Neither the name of ARM  nor the names of its contributors may be used 
 *    to endorse or promote products derived from this software without 
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <stddef.h>

#include <iostream>
#include <vector>
#include <map>
#include <string>

#include "cmsis_os.h"

extern "C" int SER_Init (void);


/*
 * STL Test Code
 */

typedef std::vector<int>        IntVectorType;
typedef IntVectorType::iterator IntVectorItr;
typedef std::map<int, int>      IntIntMapType;
typedef IntIntMapType::iterator IntIntMapItr;

IntVectorType v;
IntIntMapType m;
std::string   str;

void stl_test (void) {
  int  sum = 0;
	
// vector test
  sum = v.size();
  v.push_back(1);
  v.push_back(2);
  v.push_back(3);
  sum = v.size();
	
  sum = 0;
  for (IntVectorItr itr = v.begin(); itr != v.end(); itr++) {
    sum += *itr;
  }
  printf("*** stl_test: vector sum = %d\n", sum);

// map test
  sum  = 0;
  m[1] = 1;
  m[2] = 2;
  m[3] = 3;
	
  for (IntIntMapItr itr = m.begin(); itr != m.end(); itr++) {
    sum += itr->first * itr->second;
  }
  printf("*** stl_test: map sum = %d\n", sum);

  printf("\n");

// exception test
  std::cout << "Throwing an exception:" << std::endl;
  std::cout << "----------------------" << std::endl;

  try {
    throw "Test exception";
  }
  catch (const char *szMessage) {
    str = szMessage;
  }

  if (str.length() > 0) {
    str += " - has been catched !";
  }
  std::cout << str << std::endl;
  std::cout << std::endl;
}


/* Forward reference */
void threadX (void const *argument);

/* Thread IDs */
osThreadId main_id;
osThreadId threadX_id;

/* Thread definitions */
osThreadDef(threadX, osPriorityNormal, 1, 2000);


/*
 * Thread X
 */
void threadX (void const *argument) {
  for (;;) {
    /* Wait for completion of do-this */
    osSignalWait(0x0001, osWaitForever);
    /* do-that */
    stl_test();
    /* Indicate to main thread completion of do-that */
    osSignalSet(main_id, 0x0001);
  }
}


/*
 * Main Thread
 */
int main (void) {

  SER_Init();  // Initialize Serial Interface

  std::cout << "*** Hello C++ World!" << std::endl;
  std::cout << std::endl;

  /* Get main thread ID */
  main_id = osThreadGetId();
  
  /* Create thread X */
  threadX_id = osThreadCreate(osThread(threadX), NULL);

  /* Indicate to thread X completion of do-this */
  osSignalSet(threadX_id, 0x0001);
  
  /* Wait for completion of do-that */
  osSignalWait(0x0001, osWaitForever);

  std::cout << "End of example." << std::endl;
  while (1);
} 
