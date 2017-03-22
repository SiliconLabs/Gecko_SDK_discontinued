// *****************************************************************************
// * temperature-si7053.h
// *
// * API for reading temperature data from an SI7053 temperature sensor
// *
// * It implements the hardware support for a temperature measurement on our
// * development kit board.  Note:  this can easily be modified to change the
// * ADC value, as well as the curve to translate from ADC voltage to the
// * temperature value.
// *
// * Copyright 2015 Silicon Laboratories, Inc.                              *80*
// *****************************************************************************

#include PLATFORM_HEADER
#include "stack/include/ember-types.h"
#include "stack/include/event.h"
#include "hal/hal.h"
#include "hal/plugin/i2c-driver/i2c-driver.h"
#include "hal/micro/micro.h"
#include "hal/plugin/temperature/temperature.h"

#include "app/framework/include/af.h"

//------------------------------------------------------------------------------
// Plugin private macros

// Si7503 I2C protocol based macros
#define SI7503_I2C_ADDR                    0x80
#define SI7503_I2C_RESET_MSG               0xfe
#define SI7503_I2C_RESET_MSG_LEN           1
#define SI7503_I2C_READ_CFG_MSG            0xf3
#define SI7503_I2C_TEMP_REG                0x40
#define SI7503_I2C_TEMP_RESPONSE_LEN       2
// This is the maximum amount of time between sending a write command and the
// temperature data being ready.  This is found in table 2 of the data sheet,
// and can be set to a lower value if 11-13 bit conversions are used instead of
// the default 14 bit values.
#define SI7503_I2C_MAX_CONVERSION_TIME_MS  12

//------------------------------------------------------------------------------
// Plugin events
EmberEventControl emberAfPluginTemperatureSi7053InitEventControl;
EmberEventControl emberAfPluginTemperatureSi7053ReadEventControl;

//******************************************************************************
// Initialize the sensor
//   Send the reset command over the I2C bus
//   Execute an I2C read for initial temperature value
//******************************************************************************

//------------------------------------------------------------------------------
// Plugin private event handlers

// At this point, I2C driver is guaranteed to have initialized, so it is safe
// to call the i2c based init function
void emberAfPluginTemperatureSi7053InitEventHandler(void)
{
  uint8_t resetMsg = SI7503_I2C_RESET_MSG;
  uint8_t resetMsgLen = SI7503_I2C_RESET_MSG_LEN;

  // Send the reset command to the temperature sensor
  halI2cWriteBytes(SI7503_I2C_ADDR, &resetMsg, resetMsgLen);

  emberEventControlSetInactive(emberAfPluginTemperatureSi7053InitEventControl);
}

void emberAfPluginTemperatureSi7053ReadEventHandler(void)
{
  uint8_t status;
  uint8_t tempResponse[SI7503_I2C_TEMP_RESPONSE_LEN];
  uint16_t rawTemperature;
  int32_t tempMilliDegreesC;

  emberEventControlSetInactive(emberAfPluginTemperatureSi7053ReadEventControl);

  // perform the I2C read to get a new data value
  status  = halI2cReadBytes(SI7503_I2C_ADDR,
                            tempResponse,
                            SI7503_I2C_TEMP_RESPONSE_LEN);

  // Verify the read was successful
  if (status) {
    halTemperatureReadingCompleteCallback(0, false);
  }

  // Assemble the I2C response into a 16 bit value
  rawTemperature = ((uint16_t)tempResponse[0] << 8) |  (uint16_t)tempResponse[1];

  // Convert temperature to milli degrees C
  tempMilliDegreesC = ((rawTemperature * 21965L) >> 13) - 46850;

  halTemperatureReadingCompleteCallback(tempMilliDegreesC, true);
}


//------------------------------------------------------------------------------
// Plugin defined callbacks

// The init callback, which will be called by the framework on init.
void emberAfPluginTemperatureSi7053InitCallback(void)
{
  halTemperatureInit();
}

//------------------------------------------------------------------------------
// Plugin public functions

// All init functionality requires the I2C plugin to be initialized, which is
// not guaranteed to be the case by the time this function executes.  As a
// result, I2C based init functionality needs to execute from task context.
void halTemperatureInit(void)
{
  emberEventControlSetActive(emberAfPluginTemperatureSi7053InitEventControl);

}

//******************************************************************************
// Perform all I2C transactions necessary to read the temperature from the
// SI7503, convert it to milliDegrees Celsius, and return it.
//******************************************************************************
void halTemperatureStartRead(void)
{
  uint8_t cfgMsg = SI7503_I2C_READ_CFG_MSG;

  // Write the configuration message to the I2C device to set it to single temp
  // measurement with no hold master, then perform an i2c  read of the
  // temperature register
  halI2cWriteBytes(SI7503_I2C_ADDR, &cfgMsg, 1);

  // It will take a maximum of SI7503_I2C_MAX_CONVERSION_TIME_MS milliseconds
  // for the sensor to finish generating a new value.  Delay that amount of time
  // to ensure that we're not burning power waiting for the generation to finish
  emberEventControlSetDelayMS(emberAfPluginTemperatureSi7053ReadEventControl,
                              SI7503_I2C_MAX_CONVERSION_TIME_MS);
}
