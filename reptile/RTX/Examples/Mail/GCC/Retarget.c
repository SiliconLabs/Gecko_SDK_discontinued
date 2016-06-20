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

#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>

#include "..\Serial.h"


int _isatty (int file) {
  return (1);
}

int _open (const char * path, int flags, ...) {
  return (-1);
}

int _close (int file) {
  return (-1);
}

int _fstat (int file, struct stat * st) {
  st->st_mode = S_IFCHR;
  return 0;
}

int _lseek (int file, int ptr, int dir) {
  return (0);
}

int _read (int file, char * ptr, int len) {
  char c;
  int  i;

  for (i = 0; i < len; i++) {
    c = SER_GetChar();
    if (c == '\r') break;
    *ptr++ = c;
    SER_PutChar(c);
  }
  return (len - i);
}

int _write (int file, char * ptr, int len) {
  int i;

  for (i = 0; i < len; i++) SER_PutChar (*ptr++);
  return len;
}
