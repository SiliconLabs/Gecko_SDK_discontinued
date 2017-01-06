/*----------------------------------------------------------------------------
 * Name:    Getline.c
 * Purpose: Line Edited Character Input
 * Notes:
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2009 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include "Serial.h"

#define CNTLQ      0x11
#define CNTLS      0x13
#define DEL        0x7F
#define BACKSPACE  0x08
#define CR         0x0D
#define LF         0x0A

 
/*----------------------------------------------------------------------------
  Line Editor
 *----------------------------------------------------------------------------*/
void getline (char *line, int n) {
  int  cnt = 0;
  char c;

  do {
    if ((c = SER_GetChar()) == CR) c = LF; /* read character                 */
    if (c == BACKSPACE  ||  c == DEL) {    /* process backspace              */
      if (cnt != 0)  {            
        cnt--;                             /* decrement count                */
        line--;                            /* and line pointer               */
        putchar (BACKSPACE);               /* echo backspace                 */
        putchar (' ');
        putchar (BACKSPACE);
      }
    }
    else if (c != CNTLQ && c != CNTLS) {   /* ignore Control S/Q             */
      putchar (*line = c);                 /* echo and store character       */
      line++;                              /* increment line pointer         */
      cnt++;                               /* and count                      */
    }
    #ifdef __USE_FFLUSH
    fflush (stdout);
    #endif
  } while (cnt < n - 1  &&  c != LF);      /* check limit and line feed      */
  *(line - 1) = 0;                         /* mark end of string             */
}
