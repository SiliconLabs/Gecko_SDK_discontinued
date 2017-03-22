/*
 * File: hal/micro/generic/random.c
 * Description: Generic source for random number functions.
 *
 * Author(s): Lee Taylor, lee@ember.com,
 *            Jeff Mathews, jm@ember.com
 *
 * Copyright 2007 by Ember Corporation. All rights reserved.                *80*
 */

#include PLATFORM_HEADER
#include "hal/micro/random.h"

////////////////////////
uint16_t seed0 = 0xbeef;
uint16_t seed1 = 0xface;
#ifdef RNG_SELF_TEST
uint16_t lastval = 0xffff;
#endif

void halStackSeedRandom(uint32_t seed) 
{
  seed0 = (uint16_t) seed;
  if (seed0 == 0)
    seed0 = 0xbeef;
  seed1 = (uint16_t) (seed >> 16);
  if (seed1 == 0)
    seed1 = 0xface;
}

/*
  the stdlib rand() takes about 2.25ms @3.68MHz on the AVR
  hell- you can run an software implimentation AES cipher block in 1.5ms!
  obviously an LFSR method is much faster (12us on a 4MHz 128AVR actually).
  in the FPGA we did the first poly you see here but on a 32bit seed, 
  taking only the msb word, but we quickly noticed a sequential nature 
  in the output. so at each random number generation we actually ran the 
  LFSR 16 times. we can avoid that but still remove the sequential nature 
  and get a good looking spread by running two LFSRs and XORing them.
*/

// ** NOTE: we might consider replacing the pseudo rng generation by
// LFSR with something like SCG, LCG, or ACG - if the time trade off
// is acceptable.

static uint16_t shift(uint16_t *val, uint16_t taps)
{
  uint16_t newVal = *val;

  if (newVal & 0x8000)
    newVal ^= taps;
  *val = newVal << 1;
  return newVal;
}

uint16_t halCommonGetRandom(void)
{
  uint16_t val;
  val = (shift(&seed0, 0x0062) ^ shift(&seed1, 0x100B));

  // See Task EFM32ESS-869
  #ifdef RNG_SELF_TEST
  while (lastval == val)
  {
    val = (shift(&seed0, 0x0062) ^ shift(&seed1, 0x100B));
  }
  lastval = val;
  #endif

  return val;
}

