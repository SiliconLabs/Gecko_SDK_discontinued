/**
 * File: hal/micro/generic/button-stub.c
 * Description: stub implementation of button functions.
 *
 * Copyright 2015 Silicon Laboratories, Inc.
 */

#include PLATFORM_HEADER
#include "stack/include/ember-types.h"
#include "hal/hal.h"

void halInternalInitButton(void)
{
}

uint8_t halButtonState(uint8_t button)
{
  return BUTTON_RELEASED;
}

uint8_t halButtonPinState(uint8_t button)
{
  return BUTTON_RELEASED;
}

uint16_t halGpioIsr(uint16_t interrupt, uint16_t pcbContext)
{
  return 0;
}

uint16_t halTimerIsr(uint16_t interrupt, uint16_t pcbContext)
{
  return 0;
}

void simulatedButtonIsr(uint8_t button, bool isPress)
{
}
