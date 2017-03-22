/**
 * File: hal/micro/generic/micro-stub.c
 * Description: stub implementation of HAL
 *
 * Copyright 2007 by Ember Corporation. All rights reserved.
 */

#include PLATFORM_HEADER
#include "stack/include/ember-types.h"
#include "hal/hal.h"

void halInit(void) {}

void halReboot(void)
{ 
  fprintf(stderr, "[rebooting]\n");
  exit(1);
}

void halPowerDown(void) {}

void halPowerUp(void) {}

void halSleep(SleepModes sleepMode) {}

PGM_P halGetResetString(void)
{
  static PGM_P resetString = "UNKWN";
  return (resetString);
}

uint8_t halGetResetInfo(void)
{
   return 0;
}
