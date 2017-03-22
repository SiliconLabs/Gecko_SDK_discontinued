//   Copyright 2015 Silicon Laboratories, Inc.                              *80*

#include PLATFORM_HEADER
#include "stack/include/ember-types.h"
#include "hal/plugin/humidity-temperature-si7021/humidity.h"

EmberEventControl emberAfPluginHumiditySi7021InitEventControl;
EmberEventControl emberAfPluginHumiditySi7021ReadEventControl;

void emberAfPluginHumiditySi7021InitEventHandler(void)
{
}

void emberAfPluginHumiditySi7021ReadEventHandler(void)
{
}

void emberAfPluginHumiditySi7021InitCallback(void)
{
}

void halHumidityStartRead(void)
{
  halHumidityReadingCompleteCallback(4321, 1);
}

uint16_t halHumidityGetMaxMeasureableHumidityDeciPercent(void)
{
  return 9999;
}

uint16_t halHumidityGetMinMeasureableHumidityDeciPercent(void)
{
  return 0001;
}

void halHumidityInit(void)
{
}
