/*
 * File: hal/micro/generic/mem-util.c
 * Description: generic memory manipulation routines.
 *
 * Author(s): Lee Taylor, lee@ember.com,
 *            Jeff Mathews, jm@ember.com
 *
 * Copyright 2004 by Ember Corporation. All rights reserved.                *80*
 */

#include PLATFORM_HEADER
#include "stack/include/ember-types.h"
#include "include/error.h"

#include "hal/hal.h"

#if !defined(XAP2B) && !defined(UNIX)
// A version of memmove. This can handle overlapping source and
// destination regions.

#ifndef EMBER_STACK_COBRA
void halCommonMemMove(void *dest, const void *src, uint16_t bytes)
{
  uint8_t *d = (uint8_t *)dest;
  uint8_t *s = (uint8_t *)src;

  if (d > s) {
    d += bytes - 1;
    s += bytes - 1;
    #ifndef _HAL_SMALL_MEMUTILS_
      while(bytes >= 4) {
        bytes -= 4;
        *d-- = *s--;
        *d-- = *s--;
        *d-- = *s--;
        *d-- = *s--;
      }
    #endif // _HAL_SMALL_MEMUTILS_
    for(; bytes; bytes--) {
      *d-- = *s--;
    }
  } else {
    #ifndef _HAL_SMALL_MEMUTILS_
      while(bytes >= 4) {
        bytes -= 4;
        *d++ = *s++;
        *d++ = *s++;
        *d++ = *s++;
        *d++ = *s++;
      }
    #endif // _HAL_SMALL_MEMUTILS_
    for(; bytes; bytes--) {
      *d++ = *s++;
    }
  }
}
#endif //#ifndef EMBER_STACK_COBRA

// Same as above except copies from Program space to RAM.
// This routine never has to worry about overlapping source and dest
void halCommonMemPGMCopy(void* dest, const void PGM_NO_CONST *source, uint16_t bytes)
{
  uint8_t *d = (uint8_t *)dest;
  PGM_PU s = (PGM_PU)source;

  #ifndef _HAL_SMALL_MEMUTILS_
    while(bytes >= 4) {
      bytes -= 4;
      *d++ = *s++;
      *d++ = *s++;
      *d++ = *s++;
      *d++ = *s++;
    }
  #endif // _HAL_SMALL_MEMUTILS_
  for(; bytes; bytes--) {
    *d++ = *s++;
  }
}

void halCommonMemSet(void *dest, uint8_t val, uint16_t bytes)
{
  uint8_t *d=(uint8_t *)dest;

  for(;bytes;bytes--) {
    *d++ = val;
  }
}

int16_t halCommonMemCompare(const void *source0, const void *source1, uint16_t bytes)
{
  uint8_t *s0 = (uint8_t *)source0;
  uint8_t *s1 = (uint8_t *)source1;

  for(; 0 < bytes; bytes--, s0++, s1++) {
    uint8_t b0 = *s0;
    uint8_t b1 = *s1;
    if (b0 != b1)
      return b0 - b1;
  }
  return 0;
}

// Test code for halCommonMemCompare().  There is no good place for unit tests
// for this file.  If you change the function you should probably rerun the
// test.
//  {
//    uint8_t s0[3] = { 0, 0, 0};
//    uint8_t s1[3] = { 0, 0, 0};
//    uint8_t i;
//    assert(halCommonMemCompare(s0, s1, 0) == 0);
//    assert(halCommonMemCompare(s0, s1, 3) == 0);
//    for (i = 0; i < 3; i++) {
//      s0[i] = 1;
//      assert(halCommonMemCompare(s0, s1, 3) > 0);
//      s1[i] = 2;
//      assert(halCommonMemCompare(s0, s1, 3) < 0);
//      s0[i] = 2;
//    }
//  }

// Same again, except that the second source is in program space.

int8_t halCommonMemPGMCompare(const void *source0, const void PGM_NO_CONST *source1, uint16_t bytes)
{
  uint8_t *s0 = (uint8_t *)source0;
  uint8_t PGM *s1 = (uint8_t PGM *)source1;

  for(; 0 < bytes; bytes--, s0++, s1++) {
    uint8_t b0 = *s0;
    uint8_t b1 = *s1;
    if (b0 != b1)
      return b0 - b1;
  }
  return 0;
}
#endif
