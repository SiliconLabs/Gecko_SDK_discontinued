/**
 * File: hal/micro/generic/led-stub.c
 * Description: stub implementation of LED functions.
 *
 * Copyright 2005 by Ember Corporation. All rights reserved.
 */

#include PLATFORM_HEADER
#include "stack/include/ember-types.h"
#include "hal/hal.h"

void halInternalInitLed(void) {}

void halToggleLed (HalBoardLed led) {}
  
void halSetLed (HalBoardLed led) {}

void halClearLed (HalBoardLed led) {}

void halStackIndicateActivity(bool turnOn) {}
