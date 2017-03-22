/**
 * File: hal/micro/generic/system-timer.c
 * Description: simulation files for the system timer part of the HAL
 *
 * Copyright 2008 by Ember Corporation. All rights reserved.
 */

#include <sys/time.h>

#include PLATFORM_HEADER

uint32_t halCommonGetInt32uMillisecondTick(void)
{
  struct timeval tv;
  uint32_t now;  

  gettimeofday(&tv, NULL);
  now = (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
  return now;
}

uint16_t halCommonGetInt16uMillisecondTick(void)
{
  return (uint16_t)halCommonGetInt32uMillisecondTick();
}

uint16_t halCommonGetInt16uQuarterSecondTick(void)
{
  return (uint16_t)(halCommonGetInt32uMillisecondTick() >> 8);
}
