/*----------------------------------------------------------------------------
 *      RL-ARM - RTX
 *----------------------------------------------------------------------------
 *      Name:    TRAFFIC.C
 *      Purpose: Traffic Light Controller example program
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

#include <stdio.h>                    /* standard I/O .h-file                */
#include <ctype.h>                    /* character functions                 */
#include <string.h>                   /* string and memory functions         */
#include <stdbool.h>

#include "cmsis_os.h"

#include "LPC17xx.H"                  /* LPC17xx definitions                 */
#include "GLCD.h"
#include "Serial.h"
#include "LED.h"
#include "KBD.h"

#define __FI    1                     /* Font index 16x24                    */

#define RED     0                     /* I/O Pin:  RED    lamp output        */
#define YELLOW  1                     /* I/O Pin:  YELLOW lamp output        */
#define GREEN   2                     /* I/O Pin:  GREEN  lamp output        */
#define STOP    4                     /* I/O Pin:  STOP   lamp output        */
#define WALK    5                     /* I/O Pin:  WALK   lamp output        */

const char menu[] = 
  "\r\n"
  "+********* TRAFFIC LIGHT CONTROLLER using RTX kernel *********+\r\n"
  "| This program is a simple Traffic Light Controller. Between  |\r\n"
  "| start time and end time the system controls a traffic light |\r\n"
  "| with pedestrian self-service.  Outside of this time range   |\r\n"
  "| the YELLOW caution lamp is blinking.                        |\r\n"
  "+ command -+ syntax -----+ function --------------------------+\r\n"
  "| Display  | D           | display times                      |\r\n"
  "| Time     | T hh:mm:ss  | set clock time                     |\r\n"
  "| Start    | S hh:mm:ss  | set start time                     |\r\n"
  "| End      | E hh:mm:ss  | set end time                       |\r\n"
  "+----------+-------------+------------------------------------+\r\n";


extern void getline (char *, int);    /* external function:  input line      */

osThreadId tid_command;               /* Thread id of thread: command        */
osThreadId tid_blinking;              /* Thread id of thread: blinking       */
osThreadId tid_lights;                /* Thread id of thread: lights         */
osThreadId tid_keyread;               /* Thread id of thread: keyread        */
osThreadId tid_getesc;                /* Thread id of thread: getesc         */
osThreadId tid_lcd;                   /* Thread id of thread: lcd            */
  
osMutexDef(mut_GLCD);
osMutexId  mut_GLCD;                  /* Mutex to controll GLCD access       */

struct time  {                        /* structure of the time record        */
  uint8_t hour;                       /* hour                                */
  uint8_t min;                        /* minute                              */
  uint8_t sec;                        /* second                              */
};

struct time ctime = { 12,  0,  0 };   /* storage for clock time values       */
struct time stime = {  7, 30,  0 };   /* storage for start time values       */
struct time etime = { 18, 30,  0 };   /* storage for end   time values       */

char cmdline[16];                     /* storage for command input line      */
struct time rtime;                    /* temporary storage for entry time    */
bool display_time = false;            /* flag signal cmd state display_time  */
bool escape;                          /* flag: mark ESCAPE character enteRED */

#define ESC  0x1B                     /* ESCAPE character code               */

void init (void const *argument);     /* Function prototypes                 */
void clock (void const *argument) ;
void get_escape (void const *argument);
void command (void const *argument);
void blinking (void const *argument);
void lights (void const *argument);
void keyread (void const *argument);
void lcd (void const *argument);

osThreadDef(command,    osPriorityNormal, 1, 2000);
osThreadDef(blinking,   osPriorityNormal, 1, 0);
osThreadDef(lights,     osPriorityNormal, 1, 0);
osThreadDef(keyread,    osPriorityNormal, 1, 0);
osThreadDef(get_escape, osPriorityNormal, 1, 0);
osThreadDef(lcd,        osPriorityNormal, 1, 0);

osTimerDef(clock1s, clock);
osTimerId  clock1s;


/*---------------------------------------------------------------------------
  Set Lights function
  controls LEDs and LCD display
 *---------------------------------------------------------------------------*/
void SetLights (uint32_t light, uint32_t on) {

  osMutexWait(mut_GLCD, osWaitForever);
  GLCD_SetBackColor(White);
  if (light == RED) {
    GLCD_SetTextColor(Red);
    if (on == 0) {
       GLCD_DisplayChar(4, 15, __FI, 0x80+0);
    }
    else {
       GLCD_DisplayChar(4, 15, __FI, 0x80+1);
    }
  }
  if (light == YELLOW) {
    GLCD_SetTextColor(Yellow);
    if (on == 0) {
      GLCD_DisplayChar(5, 15, __FI, 0x80+0);
    }
    else {
      GLCD_DisplayChar(5, 15, __FI, 0x80+1);
    }
  }
  if (light == GREEN) {
    GLCD_SetTextColor(Green);
    if (on == 0) {
      GLCD_DisplayChar(6, 15, __FI, 0x80+0);
    }
    else {
      GLCD_DisplayChar(6, 15, __FI, 0x80+1);
    }
  }  
  if (light == STOP) {
    GLCD_SetTextColor(Red);
    if (on == 0) {
      GLCD_DisplayChar(5, 13, __FI, 0x80+12);
    }
    else {
      GLCD_DisplayChar(5, 13, __FI, 0x80+13);
    }
  }
  if (light == WALK) {
    GLCD_SetTextColor(Green);
    if (on == 0) {
      GLCD_DisplayChar(6, 13, __FI, 0x80+14);
    }
    else {
      GLCD_DisplayChar(6, 13, __FI, 0x80+15);
    }
  }
  osMutexRelease(mut_GLCD);

  if (on == 0) {
    LED_Off (light);
  }
  else {
    LED_On (light);
  }
}


/*----------------------------------------------------------------------------
  Main Thread 'main'
 *---------------------------------------------------------------------------*/
int main (void) {                         /* program execution starts here   */

  GLCD_Init();                            /* initialize GLCD                 */
  SER_Init ();                            /* initialize serial interface     */
  LED_Init ();                            /* initialize LEDs                 */
  KBD_Init ();                            /* initialize Push Button          */

  mut_GLCD = osMutexCreate(osMutex(mut_GLCD));
                                          /* create and start clock timer    */
  clock1s = osTimerCreate(osTimer(clock1s), osTimerPeriodic, NULL);                                          
  if (clock1s) osTimerStart(clock1s, 1000);                                          
                                          /* start thread lcd                */
  tid_lcd = osThreadCreate(osThread(lcd), NULL);

  osDelay(500);
                                          /* start command thread            */
  tid_command = osThreadCreate(osThread(command), NULL);
                                          /* start lights thread             */
  tid_lights  = osThreadCreate(osThread(lights), NULL);
                                          /* start keyread thread            */
  tid_keyread = osThreadCreate(osThread(keyread), NULL);

  osDelay(osWaitForever);
  while(1);
}

/*----------------------------------------------------------------------------
  clock: Periodic 1s Timer Callback
 *---------------------------------------------------------------------------*/
void clock (void const *argument) {

  if (++ctime.sec == 60) {            /* calculate the second                */
    ctime.sec = 0;
    if (++ctime.min == 60) {          /* calculate the minute                */
      ctime.min = 0;
      if (++ctime.hour == 24) {       /* calculate the hour                  */
        ctime.hour = 0;
      }
    }
  }
  if (display_time) {                 /* if command_status == display_time   */
                                      /* signal time changed to command      */
    osSignalSet(tid_command, 0x0001);
  }
}

/*----------------------------------------------------------------------------
  readtime: convert line input to time values & store in rtime
 *---------------------------------------------------------------------------*/
char readtime (char *buffer) {
  int args;                                    /* number of arguments        */
  int hour,min,sec;

  rtime.sec = 0;                               /* preset second              */
  args = sscanf (buffer, "%d:%d:%d",           /* scan input line for        */
                         &hour, &min, &sec);   /* hour, minute and second    */
  
  if (hour > 23  ||  min > 59  ||              /* check for valid inputs     */
      sec > 59   ||  args < 2        ) {
     printf ("\r\n*** ERROR: INVALID TIME FORMAT\r\n");
     return (0);
  }
  rtime.hour = hour;                           /* setting a new time: hour   */
  rtime.min  = min;                            /* setting a new time: min    */
  rtime.sec  = sec;                            /* setting a new time: sec    */
  return (1);
}

/*----------------------------------------------------------------------------
  Thread 5 'get_escape': check if ESC (escape character) was entered
 *---------------------------------------------------------------------------*/
void get_escape (void const *argument) {
  while (1)  {                                /* endless loop                */
    if (SER_GetChar () == ESC) {              /* If ESC entered, set flag    */ 
      escape = true;                          /* 'escape', set event flag of */
      osSignalSet(tid_command, 0x0002);       /* thread 'command'            */
    }
  }
}

/*----------------------------------------------------------------------------
  Thread 1 'command': command processor
 *---------------------------------------------------------------------------*/
void command (void const *argument) {
  int i;

  printf (menu);                              /* display command menu        */
  while (1) {                                 /* endless loop                */
    printf ("\r\nCommand: ");                 /* display prompt              */
    #ifdef __USE_FFLUSH
    fflush (stdout); 
    #endif
    getline (cmdline, sizeof (cmdline));      /* get command line input      */

    for (i = 0; cmdline[i] != 0; i++) {       /* convert to uppercase        */
      cmdline[i] = (char) toupper((int)cmdline[i]);
    }

    for (i = 0; cmdline[i] == ' '; i++);      /* skip blanks                 */

    switch (cmdline[i]) {                     /* proceed to command function */
      case 'D':                               /* Display Time Command        */
        printf ("Start Time: %02d:%02d:%02d    "
                "End Time: %02d:%02d:%02d\r\n",
                 stime.hour, stime.min, stime.sec,
                 etime.hour, etime.min, etime.sec);
        printf ("                        type ESC to abort\r");
        #ifdef __USE_FFLUSH
        fflush (stdout);
        #endif

        tid_getesc = osThreadCreate(osThread(get_escape), NULL);
                                              /* ESC check in display loop   */
        escape = false;                       /* clear escape flag           */
        display_time = true;                  /* set display time flag       */
        osSignalClear(tid_command, 0x0003);
                                              /* clear pending signals       */
        while (!escape) {                     /* while no ESC enteRED        */
          printf ("Clock Time: %02d:%02d:%02d\r",   /* display time          */
                   ctime.hour, ctime.min, ctime.sec);
          #ifdef __USE_FFLUSH
          fflush (stdout);
          #endif
          osSignalWait(0, osWaitForever);     /* wait for time change or ESC */
        }

        osThreadTerminate(tid_getesc);        /* terminate 'get_escape'      */
        display_time = false;                 /* clear display time flag     */
        printf ("\r\n\r\n");
        break;

      case 'T':                               /* Set Time Command            */
        if (readtime (&cmdline[i+1])) {       /* read time input and         */
          ctime.hour = rtime.hour;            /* store in 'ctime'            */
          ctime.min  = rtime.min;
          ctime.sec  = rtime.sec;
        }
        break;

      case 'E':                               /* Set End Time Command        */
        if (readtime (&cmdline[i+1]))  {      /* read time input and         */
          etime.hour = rtime.hour;              /* store in 'end'              */
          etime.min  = rtime.min;
          etime.sec  = rtime.sec;
        }
        break;

      case 'S':                               /* Set Start Time Command      */
        if (readtime (&cmdline[i+1]))  {      /* read time input and         */
          stime.hour = rtime.hour;            /* store in 'start'            */
          stime.min  = rtime.min;
          stime.sec  = rtime.sec;
        }
        break;

      default:                                /* Error Handling              */
        printf (menu);                        /* display command menu        */
        break;
    }   
  }
}

/*----------------------------------------------------------------------------
  signalon: check if clock time is between start and end
 *---------------------------------------------------------------------------*/
char signalon (void) {
  if (memcmp (&stime, &etime, sizeof (struct time)) < 0)  {
    if (memcmp (&stime, &ctime, sizeof (struct time)) < 0  &&
        memcmp (&ctime, &etime, sizeof (struct time)) < 0) {
      return (1);
    }
  }
  else { 
    if (memcmp (&etime, &ctime, sizeof (stime)) > 0  &&
        memcmp (&ctime, &stime, sizeof (stime)) > 0) {
      return (1);
    }
  }
  return (0);                                 /* signal off, blinking on     */
}

/*----------------------------------------------------------------------------
  Thread 2 'blinking': runs if current time is outside start & end time 
 *---------------------------------------------------------------------------*/
void blinking (void const *argument) {        /* blink YELLOW light          */
  SetLights (RED, 0);                         /* all lights off              */
  SetLights (YELLOW, 0);
  SetLights (GREEN, 0);
  SetLights (STOP, 0);
  SetLights (WALK, 0);
  while (1) {                                 /* endless loop                */
    SetLights (YELLOW, 1);                    /* YELLOW light on             */
    osDelay(300);                             /* wait for timeout: 300ms     */
    SetLights (YELLOW, 0);                    /* YELLOW light off            */
    osDelay(300);                             /* wait for timeout: 300ms     */
    if (signalon ()) {                        /* if blinking time over       */
      tid_lights = osThreadCreate(osThread(lights), NULL);
                                              /* start lights                */
      return;                                 /* and STOP blinking           */
    }
  }
}

/*----------------------------------------------------------------------------
  Thread 3 'lights': executes if current time is between start & end time
 *---------------------------------------------------------------------------*/
void lights (void const *argument) {          /* traffic light operation     */
  SetLights (RED, 1);                         /* RED & STOP lights on        */
  SetLights (STOP, 1);
  SetLights (YELLOW, 0);
  SetLights (GREEN, 0);
  SetLights (WALK, 0);
  while (1) {                                 /* endless loop                */
    osDelay(500);                             /* wait for timeout: 500ms     */
    if (!signalon ()) {                       /* if traffic signal time over */
      tid_blinking = osThreadCreate(osThread(blinking), NULL);
                                              /* start blinking              */
      return;                                 /* STOP lights                 */
    }
    SetLights (YELLOW, 1);
    osDelay(500);                             /* wait for timeout: 500ms     */
    SetLights (RED, 0);                       /* GREEN light for cars        */
    SetLights (YELLOW, 0);
    SetLights (GREEN, 1);
    osSignalClear(tid_lights, 0x0010);
    osDelay(500);                             /* wait for timeout: 500ms     */
    osSignalWait(0x0010, 7500);               /* wait for event with timeout */
    SetLights (YELLOW, 1);                    /* timeout value: 7.5s         */
    SetLights (GREEN, 0);
    osDelay(500);                             /* wait for timeout: 500ms     */
    SetLights (RED, 1);                       /* RED light for cars          */
    SetLights (YELLOW, 0);
    osDelay(500);                             /* wait for timeout: 500ms     */
    SetLights (STOP, 0);                      /* GREEN light for WALKers     */    
    SetLights (WALK, 1);
    osDelay(2500);                            /* wait for timeout: 2.5s      */
    SetLights (STOP, 1);                      /* RED light for WALKers       */        
    SetLights (WALK, 0);
  }
}

/*----------------------------------------------------------------------------
  Thread 4 'keyread': process key stroke from pedestrian push button
 *---------------------------------------------------------------------------*/
void keyread (void const *argument) {
  while (1) {                                 /* endless loop                */
    if (INT0_Get() == 0) {                    /* if key pressed              */
      osSignalSet(tid_lights, 0x0010);        /* set signal to lights thread */
    }
    osDelay(50);                              /* wait for timeout: 50ms      */
  }
}

/*----------------------------------------------------------------------------
  Thread 6 'lcd': LCD Control Thread
 *---------------------------------------------------------------------------*/
void lcd (void const *argument) {

  GLCD_Clear(White);                                /* Clear the LCD         */
  for (;;) {
    osMutexWait(mut_GLCD, osWaitForever);
    GLCD_SetBackColor(Blue);                        /* Set the Back Color    */
    GLCD_SetTextColor(White);                       /* Set the Text Color    */
    GLCD_DisplayString(0, 0, __FI, "    MCB1700 Demo    ");
    GLCD_DisplayString(1, 0, __FI, "    RTX Traffic     ");
    GLCD_DisplayString(2, 0, __FI, "    www.keil.com    ");
    osMutexRelease(mut_GLCD);
    osDelay(4000);

    osMutexWait(mut_GLCD, osWaitForever);
    GLCD_SetBackColor(Blue);                        /* Set the Back Color    */
    GLCD_SetTextColor(Red);                         /* Set the Text Color    */
    GLCD_DisplayString(0, 0, __FI, "    MCB1700 Demo    ");
    GLCD_DisplayString(1, 0, __FI, "    RTX Traffic     ");
    GLCD_DisplayString(2, 0, __FI, "    www.keil.com    ");
    osMutexRelease(mut_GLCD);
    osDelay(4000);
  }
}

/*----------------------------------------------------------------------------
  end of file
 *---------------------------------------------------------------------------*/
