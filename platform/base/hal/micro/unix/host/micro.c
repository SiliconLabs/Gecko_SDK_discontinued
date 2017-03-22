/**
 * File: hal/micro/unix/host/micro.c
 * Description: PC host files for the HAL.
 *
 * Copyright 2003 by Ember Corporation. All rights reserved.
 */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include PLATFORM_HEADER
#include "stack/include/ember-types.h"
#include "hal/hal.h"
#include "include/error.h"

#if defined(EZSP_HOST) && !defined(EZSP_ASH) && !defined(EZSP_USB)
  #include "spi-protocol-common.h"
  #define initSpiNcp() halNcpSerialInit()
#else
  #define initSpiNcp()
#endif

static void (*microRebootHandler)(void) = NULL;

void halInit(void)
{
  initSpiNcp();
}

void setMicroRebootHandler(void (*handler)(void))
{
  microRebootHandler = handler;
}

void halReboot(void)
{
  if (microRebootHandler) {
    microRebootHandler();
  }
  printf("\nReboot not supported.  Exiting instead.\n");
  exit(0);
}

uint8_t halGetResetInfo(void)
{
   return RESET_SOFTWARE;
}

PGM_P halGetResetString(void)
{
  static PGM_P resetString = "SOFTWARE";
  return (resetString);
}

// Ideally this should not be necessary, but the serial code references
// this for all platforms.
void simulatedSerialTimePasses(void) 
{
}

void halPowerDown(void)
{
}

void halPowerUp(void)
{
}

void halSleep(SleepModes sleepMode)
{
}

// Implement this to catch incorrect HAL calls for this platform.
void halCommonSetSystemTime(uint32_t time)
{
  fprintf(stderr, "FATAL:  halCommonSetSystemTime() not supported!\n");
  assert(0);
}

void halInternalResetWatchDog(void)
{
}

void halCommonDelayMicroseconds(uint16_t usec)
{
  if (usec == 0)
    return;

  struct timeval timeout;
  timeout.tv_sec = 0;
  timeout.tv_usec = usec;
  assert(select(0, NULL, NULL, NULL, &timeout) >= 0);
}

void halCommonDelayMilliseconds(uint16_t msec)
{
  uint16_t cnt = msec;

  if (msec == 0)
    return;

  while(cnt-- > 0)
    halCommonDelayMicroseconds(1000);
}

