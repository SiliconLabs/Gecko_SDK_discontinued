/**
 * File: hal/plugin/antenna-stub/antenna-stub.c
 * Description: stub implementation of antenna control HAL
 *
 * <!-- Copyright 2015 Silicon Laboratories, Inc.                        *80*-->
 */

#include PLATFORM_HEADER
#include "stack/include/ember-types.h"
#include "hal/hal.h"

#ifdef EMBER_STACK_CONNECT
#include "stack/include/error.h"
#endif

// Stubs in case someone insists on referencing them

HalAntennaMode halGetAntennaMode(void)
{
  return HAL_ANTENNA_MODE_ENABLE1;
}

EmberStatus halSetAntennaMode(HalAntennaMode mode)
{
  return ((mode == HAL_ANTENNA_MODE_ENABLE1) ? EMBER_SUCCESS : EMBER_BAD_ARGUMENT);
}

EmberStatus halToggleAntenna(void)
{
  return EMBER_ERR_FATAL;
}
