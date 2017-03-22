//   Copyright 2015 Silicon Laboratories, Inc.                              *80*

#include PLATFORM_HEADER
#include "stack/include/ember-types.h"
#include "hal/plugin/temperature/temperature.h"

EmberEventControl emberAfPluginTemperatureSi7021InitEventControl;
EmberEventControl emberAfPluginTemperatureSi7021ReadEventControl;

void emberAfPluginTemperatureSi7021InitEventHandler(void)
{
}

void emberAfPluginTemperatureSi7021ReadEventHandler(void)
{
}

void emberAfPluginTemperatureSi7021InitCallback(void)
{
}

void halTemperatureStartRead(void)
{
  halTemperatureReadingCompleteCallback(0xCAFE, false);
}

void halTemperatureInit(void)
{
}
