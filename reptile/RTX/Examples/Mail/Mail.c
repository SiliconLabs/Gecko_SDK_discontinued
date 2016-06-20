/*----------------------------------------------------------------------------
 *      RL-ARM - RTX
 *----------------------------------------------------------------------------
 *      Name:    MAIL.C
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

typedef struct {                      /* Mail object structure               */
  float    voltage;                   /* AD result of measured voltage       */
  float    current;                   /* AD result of measured current       */
  uint32_t counter;                   /* A counter value                     */
} T_MEAS;

osMailQDef(mail, 16, T_MEAS);         /* Define mail queue                   */
osMailQId  mail;

/* Forward reference */
void send_thread (void const *argument);
void recv_thread (void const *argument);

/* Thread definitions */
osThreadDef(send_thread, osPriorityNormal, 1, 0);
osThreadDef(recv_thread, osPriorityNormal, 1, 2000);


/*----------------------------------------------------------------------------
 *  Thread 1: Send thread
 *---------------------------------------------------------------------------*/
void send_thread (void const *argument) {
  T_MEAS *mptr;

  mptr = osMailAlloc(mail, osWaitForever);           /* Allocate memory      */
  mptr->voltage = 223.72;             /* Set the mail content                */
  mptr->current = 17.54;
  mptr->counter = 120786;
  osMailPut(mail, mptr);              /* Send Mail                           */
  osDelay(100);

  mptr = osMailAlloc(mail, osWaitForever);           /* Allocate memory      */
  mptr->voltage = 227.23;             /* Prepare 2nd mail                    */
  mptr->current = 12.41;
  mptr->counter = 170823;
  osMailPut(mail, mptr);              /* Send Mail                           */
  osThreadYield();                    /* Cooperative multitasking            */
  osDelay(100);

  mptr = osMailAlloc(mail, osWaitForever);           /* Allocate memory      */
  mptr->voltage = 229.44;             /* Prepare 3rd mail                    */
  mptr->current = 11.89;
  mptr->counter = 237178;
  osMailPut(mail, mptr);              /* Send Mail                           */
  osDelay(100);
                                      /* We are done here, exit this thread  */
}

/*----------------------------------------------------------------------------
 *  Thread 2: Receive thread
 *---------------------------------------------------------------------------*/
void recv_thread (void const *argument) {
  T_MEAS  *rptr;
  osEvent  evt;

  for (;;) {
    evt = osMailGet(mail, osWaitForever);  /* wait for mail                  */
    if (evt.status == osEventMail) {
      rptr = evt.value.p;
      printf ("\nVoltage: %.2f V\n",rptr->voltage);
      printf ("Current: %.2f A\n",rptr->current);
      printf ("Number of cycles: %d\n",(int)rptr->counter);
      #ifdef __USE_FFLUSH
      fflush (stdout);
      #endif
      osMailFree(mail, rptr);         /* free memory allocated for mail      */
    }
  }
}

/*----------------------------------------------------------------------------
 *   Main:
 *---------------------------------------------------------------------------*/
int main (void) {                     /* program execution starts here       */

  mail = osMailCreate(osMailQ(mail), NULL);  /* create mail queue            */

  tid_thread1 = osThreadCreate(osThread(send_thread), NULL);
  tid_thread2 = osThreadCreate(osThread(recv_thread), NULL);

  osDelay(osWaitForever);
  for (;;);
}

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
