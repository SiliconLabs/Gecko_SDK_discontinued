/***************************************************************************//**
 * @file btl_driver_delay.c
 * @brief Hardware driver layer for simple delay on EXX32.
 * @author Silicon Labs
 * @version 1.0.0
 *******************************************************************************
 * @section License
 * <b>Copyright 2016 Silicon Laboratories, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include "btl_driver_delay.h"

#include "em_timer.h"

static const uint32_t iterationsPerMicrosecond = 3;

static uint16_t delayTarget = 0;
static bool expectOverflow;
static uint32_t ticksPerMillisecond = 0;

void delay_microseconds(uint32_t usecs)
{
  volatile uint64_t iterations = iterationsPerMicrosecond * usecs;

  while(iterations--);
}

void delay_init(void)
{
  // Enable clocks to TIMER0
  CMU->CTRL |= CMU_CTRL_HFPERCLKEN;
  CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_TIMER0;

  // Calculate the length of a tick
  ticksPerMillisecond = SystemHFClockGet() / 1000 / 1024;

  // Initialize timer
  TIMER_Init_TypeDef init = TIMER_INIT_DEFAULT;
  init.prescale = timerPrescale1024;
  TIMER_Init(TIMER0, &init);
}

void delay_milliseconds(uint32_t msecs, bool blocking)
{
  // TODO: Assert msecs within a single TIMER0 overflow (approx. 3300 ms)

  uint16_t count = TIMER0->CNT;
  delayTarget = count + (msecs * ticksPerMillisecond);
  expectOverflow = (delayTarget < count);

  if (blocking) {
    while (TIMER0->CNT != delayTarget);
  }
  TIMER0->IFC = TIMER_IFC_OF;
  TIMER0->IEN = TIMER_IEN_OF;
}

bool delay_expired(void)
{
  // Expecting overflow, but it hasn't happened yet
  if (expectOverflow && !(TIMER0->IF & TIMER_IF_OF)) {
    return false;
  }
  // Not expecting overflow, but it still happened
  if (!expectOverflow && (TIMER0->IF & TIMER_IF_OF)) {
    return true;
  }

  // Return true if CNT has passed the target
  return TIMER0->CNT >= delayTarget;
}
