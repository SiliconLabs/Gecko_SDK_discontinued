// File: ashV3-stubs.c
//
// Description: Stubs for ASHv3 for images that don't need it.
//
// Copyright 2013 by Silicon Laboratories. All rights reserved.             *80*

#include PLATFORM_HEADER
#include "stack/core/ember-stack.h"
#include "hal/hal.h"
#include "hal/micro/serial.h"
#include "hal/micro/uart-link.h"

bool uartTxSpaceAvailable(void)
{
  return true;
}

void uartFlushTx(void)
{
}

void uartLinkReset(void)
{
  halHostSerialInit();
}

extern bool halHostTxIsIdle(void);

bool emAshPreparingForPowerDown(void)
{
  // if the HAL TX is idle, then we return false
  // (we're not preparing to power down anymore)
  return ! halHostTxIsIdle();
}

void emAshNotifyTxComplete(void)
{
}

void emAshConfigUart(uint8_t dropPercentage, uint8_t corruptPercentage)
{
}
