/** @file hal/plugin/buzzer/buzzer-efr32.c
 *  @brief  Sample API functions for using a timer to play tunes on a buzzer.
 * 
 * <!-- Author(s): Areeya Vimayangkoon -->
 * <!-- Copyright 2007 by Ember Corporation. All rights reserved.       *80*-->   
 */

#include PLATFORM_HEADER
#include "stack/include/ember.h"
#include "hal/hal.h"

bool tuneDone = true;

void halPlayTune_P(uint8_t PGM *tune, bool bkg)
{
}

void halTimer1Isr(void)
{
}

void halStackIndicatePresence(void)
{
}
