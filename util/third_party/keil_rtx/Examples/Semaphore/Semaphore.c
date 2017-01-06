/*----------------------------------------------------------------------------
 *      RL-ARM - RTX
 *----------------------------------------------------------------------------
 *      Name:    Semaphore.c
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

#include <stdio.h>
#include "cmsis_os.h"

/* Thread IDs */
osThreadId tid_thread1;               /* assigned ID for thread 1            */
osThreadId tid_thread2;               /* assigned ID for thread 2            */

/* Semaphore ID */
osSemaphoreId semaphore;

/*----------------------------------------------------------------------------
 *   Thread 1 - High Priority - Active every 3ms
 *---------------------------------------------------------------------------*/
void thread1 (void const *argument) {
  int32_t val;

  while (1) {
    /* Pass control to other tasks for 3ms */
    osDelay(3);
    /* Wait 1ms for the free semaphore */
    val = osSemaphoreWait (semaphore, 1);
    if (val > 0) {
      /* If there was no time-out the semaphore was acquired */
      printf ("Thread 1\n");
      /* Return a token back to a semaphore */
      osSemaphoreRelease (semaphore);
    }
  }
}

/*----------------------------------------------------------------------------
 *   Thread 2 - Normal Priority - looks for a free semaphore and uses
 *                                the resource whenever it is available
 *---------------------------------------------------------------------------*/
void thread2 (void const *argument) {
  while (1) {
    /* Wait indefinetly for a free semaphore */
    osSemaphoreWait (semaphore, osWaitForever);
    /* OK, the serial interface is free now, use it. */
    printf ("Thread 2\n");
    /* Return a token back to a semaphore. */
    osSemaphoreRelease (semaphore);
  }
}

/* Thread definitions */
osThreadDef(thread1, osPriorityHigh,   1, 0);
osThreadDef(thread2, osPriorityNormal, 1, 0);

/* Semaphore definition */
osSemaphoreDef(semaphore);

/*----------------------------------------------------------------------------
 *   Main:
 *---------------------------------------------------------------------------*/
int main (void) {                     /* program execution starts here       */

  semaphore = osSemaphoreCreate(osSemaphore(semaphore), 1);

  tid_thread1 = osThreadCreate(osThread(thread1), NULL);
  tid_thread2 = osThreadCreate(osThread(thread2), NULL);

  osDelay(osWaitForever);
  for (;;);
}

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
