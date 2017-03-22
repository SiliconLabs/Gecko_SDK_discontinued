// Copyright 2015 Silicon Laboratories, Inc.

#include PLATFORM_HEADER

void halI2cInitialize(void)
{
}

void emberAfPluginI2cDriverInitCallback(void)
{
}

uint8_t halI2cWriteBytes(uint8_t address, const uint8_t *buffer, uint8_t count)
{
  return 0;
}

uint8_t halI2cWriteBytesDelay(uint8_t address,
                              const uint8_t *buffer,
                              uint8_t count,
                              uint8_t delay)
{
  return 0;
}

uint8_t halI2cReadBytes(uint8_t address, uint8_t *buffer, uint8_t count)
{
  return 0;
}
