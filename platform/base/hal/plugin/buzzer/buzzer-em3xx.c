/** @file hal/plugin/buzzer/buzzer-em3xx.c
 *  @brief  Sample API functions for using a timer to play tunes on a buzzer.
 *
 * <!-- Copyright 2015 Silicon Laboratories, Inc.                        *80*-->
 */

#include PLATFORM_HEADER
#include "stack/include/ember.h"
#include "hal/hal.h"

// Tune state
uint8_t PGM *currentTune = NULL;
uint8_t tunePos = 0;
uint16_t currentDuration = 0;
bool tuneDone=true;

// Keep the defaults if not defined on the board file. Default is TMR1
#ifndef BUZZER_CLK
#define BUZZER_CLK TIM1->OR
#endif

#ifndef BUZZER_PSC
#define BUZZER_PSC TIM1->PSC
#endif

#ifndef BUZZER_UPDATE
#define BUZZER_UPDATE TIM1->EGR
#endif

#ifndef BUZZER_OUTPUT_CFG
#define BUZZER_OUTPUT_CFG TIM1->CCMR1
#endif

#ifndef BUZZER_OUTPUT_MODE
#define BUZZER_OUTPUT_MODE (0x3 << _TIM_CCMR1_TIM_OC2M_SHIFT)
#endif

#ifndef BUZZER_OUTPUT_ENABLE
#define BUZZER_OUTPUT_ENABLE TIM1->CCER
#endif

#ifndef BUZZER_OUTPUT_ENABLE_CHANNEL
#define BUZZER_OUTPUT_ENABLE_CHANNEL TIM_CCER_TIM_CC2E
#endif

#ifndef BUZZER_TOP
#define BUZZER_TOP TIM1->ARR
#endif

#ifndef BUZZER_CNT
#define BUZZER_CNT TIM1->CNT
#endif

#ifndef BUZZER_INT_MASK
#define BUZZER_INT_MASK EVENT_TIM1->CFG
#endif

#ifndef BUZZER_INT
#define BUZZER_INT EVENT_TIM1->FLAG
#endif

#ifndef BUZZER_BIT
#define BUZZER_BIT TIM_SR_UIF
#endif

#ifndef BUZZER_ENABLE
#define BUZZER_ENABLE TIM1->CR1
#endif

#ifndef BUZZER_TEMPO
#define BUZZER_TEMPO 200
#endif

// EO defaults

static void endTune(void)
{
    // Also useful for "cleaning out the timer."
    BUZZER_INT_MASK &= ~BUZZER_BIT; //disable the Timer, CNT ?= TOP interrupt
    NVIC_DisableIRQ(TIM1_IRQn); //stop the interrupts
    BUZZER_OUTPUT_ENABLE  = 0; //disable output
    BUZZER_ENABLE = 0;; //disable timer
    tuneDone = true;
    BUZZER_TOP = 0;
    BUZZER_CNT = 0;
}

static void setUpNextNoteOrStop(void)
{
  if (currentTune[tunePos + 1]) {
    if (currentTune[tunePos]) {
      // generate a note
      BUZZER_TOP = currentTune[tunePos]*13; //magical conversion
      BUZZER_CNT = 0; //force the counter back to zero to prevent missing BUZZER_TOP
      BUZZER_OUTPUT_ENABLE = BUZZER_OUTPUT_ENABLE_CHANNEL; //enable channel output
      // work some magic to determine the duration based upon the frequency
      // of the note we are currently playing.
      currentDuration = (((uint16_t)BUZZER_TEMPO
                          * (uint16_t)currentTune[tunePos + 1])
                         / (currentTune[tunePos] / 15));
    } else {
      // generate a pause
      BUZZER_TOP = 403; //simulated a note (NOTE_B4*13), but output is disabled
      BUZZER_CNT = 0; //force the counter back to zero to prevent missing BUZZER_TOP
      BUZZER_OUTPUT_ENABLE = 0; //Output waveform disabled for silence
      currentDuration = (((uint16_t)BUZZER_TEMPO
                          * (uint16_t)currentTune[tunePos + 1])
                         / (31 / 15));
    }
    tunePos += 2;
  } else {
    endTune();
  }
}

void halPlayTune_P(uint8_t PGM *tune, bool bkg)
{
  //Start from a fully disabled timer.  (Tune's cannot be chained with
  //this play tune functionality.)
  endTune();

  //According to emulator.h, buzzer is on pin 15 which is mapped
  //to channel 2 of TMR1
  BUZZER_CLK = 0; //use 12MHz clock
  BUZZER_PSC = 5; //2^5=32 -> 12MHz/32 = 375kHz = 2.6us tick
  BUZZER_UPDATE = 1; //trigger update event to load new prescaler value
  BUZZER_OUTPUT_CFG  = 0; //start from a zeroed configuration
  //Output waveform: toggle on CNT reaching TOP
  BUZZER_OUTPUT_CFG |= BUZZER_OUTPUT_MODE;

  currentTune = tune;
  tunePos = 0;
  tuneDone = false;

  ATOMIC(
    BUZZER_INT_MASK = BUZZER_BIT; //enable the Timer 1, CNT ?= TOP interrupt
    NVIC_EnableIRQ(TIM1_IRQn); //enable top level timer interrupts
    BUZZER_ENABLE |= TIM_CEN; //enable counting
    setUpNextNoteOrStop();
  )
  while (!bkg && !tuneDone) {
    halResetWatchdog();
  }
}

void halTimer1Isr(void)
{
  if (currentDuration-- == 0) {
    setUpNextNoteOrStop();
  }
  //clear interrupt
  BUZZER_INT = 0xFFFFFFFF;
}

uint8_t PGM hereIamTune[] = {
  NOTE_B4,  1,
  0,        1,
  NOTE_B4,  1,
  0,        1,
  NOTE_B4,  1,
  0,        1,
  NOTE_B5,  5,
  0,        0
};

void halStackIndicatePresence(void)
{
  halPlayTune_P(hereIamTune, true);
}
