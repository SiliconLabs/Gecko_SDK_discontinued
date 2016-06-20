/*----------------------------------------------------------------------------
 *      RL-ARM - RTX
 *----------------------------------------------------------------------------
 *      Name:    RTX_EX2.C
 *      Purpose: RTX example program
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

#include "cmsis_os.h"

volatile uint16_t counter;            /* counter for main thread             */
volatile uint16_t counter1;           /* counter for thread 1                */
volatile uint16_t counter2;           /* counter for thread 2                */
volatile uint16_t counter3;           /* counter for thread 3                */

/* Thread IDs */
osThreadId thread1_id;
osThreadId thread2_id;
osThreadId thread3_id;

/* Forward reference */
void job1 (void const *argument);
void job2 (void const *argument);
void job3 (void const *argument);

/* Thread definitions */
osThreadDef(job1, osPriorityAboveNormal, 1, 0);
osThreadDef(job2, osPriorityNormal, 1, 0);
osThreadDef(job3, osPriorityNormal, 1, 0);

/*----------------------------------------------------------------------------
 *   Thread 1: 'job1'
 *---------------------------------------------------------------------------*/
void job1 (void const *argument) {    /* higher priority to preempt job2     */
  while (1) {                         /* endless loop                        */
    counter1++;                       /* increment counter 1                 */
    osDelay(10);                      /* wait for timeout: 10ms              */
  }
}

/*----------------------------------------------------------------------------
 *   Thread 2: 'job2'
 *---------------------------------------------------------------------------*/
void job2 (void const *argument) {
  while (1)  {                        /* endless loop                        */
    counter2++;                       /* increment counter 2                 */
    if (counter2 == 0) {              /* signal overflow of counter 2        */
      osSignalSet(thread3_id, 0x0001);/* to thread 3                         */
      osThreadYield();
    }
  }
}

/*----------------------------------------------------------------------------
 *   Thread 3: 'job3'
 *---------------------------------------------------------------------------*/
void job3 (void const *argument) {
  while (1) {                         /* endless loop                        */
    osSignalWait(0x0001, osWaitForever);  /* wait for signal event           */
    counter3++;                       /* process overflow from counter 2     */
  }
}

/*----------------------------------------------------------------------------
 *   Main Thread
 *---------------------------------------------------------------------------*/
int main (void) {                     /* program execution starts here       */

  /* Set higher priority of main thread to preempt job2                      */
  osThreadSetPriority(osThreadGetId(), osPriorityAboveNormal);

  thread1_id = osThreadCreate(osThread(job1),NULL);  /* create thread1       */
  thread2_id = osThreadCreate(osThread(job2),NULL);  /* create thread2       */
  thread3_id = osThreadCreate(osThread(job3),NULL);  /* create thread3       */

  while (1) {                         /* endless loop                        */
    counter++;                        /* increment counter                   */
    osDelay(5);                       /* wait for timeout: 5ms               */
  }
}

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
