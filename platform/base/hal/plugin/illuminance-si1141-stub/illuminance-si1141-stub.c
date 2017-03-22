//  Copyright 2015 Silicon Laboratories, Inc.                              *80*

#include PLATFORM_HEADER
#include "hal/plugin/illuminance-si1141/illuminance.h"
#include "stack/include/ember-types.h"

//------------------------------------------------------------------------------
// Plugin events
EmberEventControl emberAfPluginIlluminanceSi1141InitEventControl;
EmberEventControl emberAfPluginIlluminanceSi1141ReadEventControl;

void emberAfPluginIlluminanceSi1141InitEventHandler(void)
{
}

void emberAfPluginIlluminanceSi1141ReadEventHandler(void)
{
}

void emberAfPluginIlluminanceSi1141InitCallback(void)
{
}

void halIlluminanceInit(void)
{
}

uint8_t halIlluminanceCalibrate(uint32_t extLux)
{
  return 0;
}

uint16_t halIlluminanceStartRead(uint8_t multiplier)
{
  halIlluminanceReadingCompleteCallback(0xBEEF);
  return 0;
}

uint16_t halIlluminanceGetMaxMeasureableIlluminanceLogLux(void)
{
  return 0xFFFD;
}

uint16_t halIlluminanceGetMinMeasureableIlluminanceLogLux(void)
{
  return 0x0002;
}
