/*
 * File: antenna.c
 * Description: HAL functions to control antenna mode
 *
 *
 * <!-- Copyright 2015 Silicon Laboratories, Inc.                        *80*-->
 */
//[[ Author(s): Micah Evans ]]


#include PLATFORM_HEADER
#include "stack/include/ember.h"
#include "include/error.h"

#include "hal/hal.h"

#ifdef RTOS
  #include "rtos/rtos.h"
#endif

#ifdef ANTENNA_SELECT_GPIO

static HalAntennaMode antennaMode = HAL_ANTENNA_MODE_DEFAULT;
static uint8_t antennaSelection;

static void selectAntenna(uint8_t antenna)
{
  antennaSelection = antenna;

  if(antennaSelection == 0) {
    halGpioSet(ANTENNA_SELECT_GPIO);
#ifdef ANTENNA_nSELECT_GPIO
    halGpioClear(ANTENNA_nSELECT_GPIO);
#endif
  } else if(antennaSelection == 1) {
    halGpioClear(ANTENNA_SELECT_GPIO);
#ifdef ANTENNA_nSELECT_GPIO
    halGpioSet(ANTENNA_nSELECT_GPIO);
#endif
  }
}

HalAntennaMode halGetAntennaMode(void)
{
  return antennaMode;
}

EmberStatus halSetAntennaMode(HalAntennaMode mode)
{
  EmberStatus status = EMBER_SUCCESS;

  switch(mode) {
  case HAL_ANTENNA_MODE_ENABLE1:
    selectAntenna(0);
    break;
  case HAL_ANTENNA_MODE_ENABLE2:
    selectAntenna(1);
    break;
  case HAL_ANTENNA_MODE_DIVERSITY:
    break;
  default:
    status = EMBER_BAD_ARGUMENT;
    break;
  }

  if(status == EMBER_SUCCESS) {
    antennaMode = mode;
  }

  return status;
}

EmberStatus halToggleAntenna(void)
{
  if(antennaMode == HAL_ANTENNA_MODE_DIVERSITY) {
    selectAntenna(halGpioRead(ANTENNA_SELECT_GPIO));
   return EMBER_SUCCESS;
  }

  return EMBER_ERR_FATAL;
}

#else //ANTENNA_SELECT_GPIO
#ifndef JAMBUILD
#warning "ANTENNA_SELECT_GPIO is undefined.  Define ANTENNA_SELECT_GPIO to enable antenna diversity"
#endif//JAMBUILD

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

#endif//ANTENNA_SELECT_GPIO
