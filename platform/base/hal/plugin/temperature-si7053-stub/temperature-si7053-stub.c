//   Copyright 2015 Silicon Laboratories, Inc.                              *80*

#include PLATFORM_HEADER
#include "stack/include/ember-types.h"
#include "hal/plugin/temperature/temperature.h"

EmberEventControl emberAfPluginTemperatureSi7053InitEventControl;
EmberEventControl emberAfPluginTemperatureSi7053ReadEventControl;

void emberAfPluginTemperatureSi7053InitEventHandler(void)
{
}

void emberAfPluginTemperatureSi7053ReadEventHandler(void)
{
}

int32_t halTemperatureGetMaxMeasureableTemperatureMilliC(void)
{
  return 327670;
}

int32_t halTemperatureGetMinMeasureableTemperatureMilliC(void)
{
  return -327670;
}

void emberAfPluginTemperatureSi7053InitCallback(void)
{
}

void halTemperatureStartRead(void)
{
  halTemperatureReadingCompleteCallback(2700, true);
}

void halTemperatureInit(void)
{
}
