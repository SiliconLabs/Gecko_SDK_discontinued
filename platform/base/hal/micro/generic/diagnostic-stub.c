/**
 * File: hal/micro/generic/diagnostic-stub.c
 * Description: stubs for HAL diagnostic functions.
 *
 * Copyright 2014 Silicon Laboratories, Inc.
 */

#include PLATFORM_HEADER

void halPrintCrashData(uint8_t port)
{
}

void halPrintCrashDetails(uint8_t port)
{
}

void halPrintCrashSummary(uint8_t port)
{
}

void halStartPCDiagnostics(void)
{
}

void halStopPCDiagnostics(void)
{
}

uint16_t halGetPCDiagnostics(void)
{
  return 0;
}
