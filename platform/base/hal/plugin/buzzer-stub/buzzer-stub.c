/**
 * File: hal/plugin/buzzer-stub/buzzer-stub.c
 * Description: stub implementation of buzzer functions.
 *
 * Copyright 2005 by Ember Corporation. All rights reserved.
 */

#include PLATFORM_HEADER
#include "stack/include/ember-types.h"
#include "hal/hal.h"

bool tuneDone = true;
void halPlayTune_P(uint8_t PGM *tune, bool bkg) {}
void halStackIndicatePresence(void) {}
