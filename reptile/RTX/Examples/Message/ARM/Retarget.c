/*----------------------------------------------------------------------------
 * Name:    Retarget.c
 * Purpose: 'Retarget' layer for target-dependent low level functions
 * Note(s):
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2011 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <rt_sys.h>
#include "..\Serial.h"


#pragma import(__use_no_semihosting_swi)


#ifdef __MICROLIB

int fputc(int c, FILE *f) {
  return (SER_PutChar(c));
}

int fgetc(FILE *f) {
  return (SER_GetChar());
}

#else

/* Standard IO device handles. */
#define STDIN   0x8001
#define STDOUT  0x8002
#define STDERR  0x8003

/* Standard IO device name defines. */
const char __stdin_name[]  = "STDIN";
const char __stdout_name[] = "STDOUT";
const char __stderr_name[] = "STDERR";

void _ttywrch (int ch) {
  SER_PutChar(ch);
}

FILEHANDLE _sys_open (const char *name, int openmode) {
  /* Register standard Input Output devices. */
  if (strcmp(name, "STDIN") == 0) {
    return (STDIN);
  }
  if (strcmp(name, "STDOUT") == 0) {
    return (STDOUT);
  }
  if (strcmp(name, "STDERR") == 0) {
    return (STDERR);
  }
  return (-1);
}

int _sys_close (FILEHANDLE fh) {
  if (fh > 0x8000) {
    return (0);
  }
  return (-1);
}

int _sys_write (FILEHANDLE fh, const unsigned char *buf, unsigned len, int mode) {
  if (fh == STDOUT) {
    /* Standard Output device. */
    for (; len; len--) {
      SER_PutChar (*buf++);
    }
    return (0);
  }
  return (-1);
}

int _sys_read (FILEHANDLE fh, unsigned char *buf, unsigned len, int mode) {
  char c;

  if (fh == STDIN) {
    /* Standard Input device. */
    for (; len; len--) {
      c = SER_GetChar();
      if (c == '\r') break;
      *buf++ = c;
      SER_PutChar(c);
    }
    return (len);
  }
  return (-1);
}

int _sys_seek (FILEHANDLE fh, long pos) {
  if (fh > 0x8000) {
    return (-1);
  }
  return (-1);
}

int _sys_ensure (FILEHANDLE fh) {
  if (fh > 0x8000) {
    return (-1);
  }
  return (-1);
}

long _sys_flen (FILEHANDLE fh) {
  if (fh > 0x8000) {
    return (0);
  }
  return (0);
}

int _sys_istty (FILEHANDLE fh) {
  if (fh > 0x8000) {
    return (1);
  }
  return (0);
}

void _sys_exit (int return_code) {
  /* Endless loop. */
  while (1);
}

#endif
