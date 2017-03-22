/**
 * File: hal/micro/cortexm3/ezr32/diagnostic-stub.c
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

void halInternalAssertFailed(PGM_P filename, int linenumber)
{
}

uint16_t halInternalCrashHandler(void)
{
  return 0;
}

// Stub for the EnergyMicro assert handler
void assertEFM(const char *file, int line)
{
}
