// Copyright 2015 Silicon Laboratories, Inc.                                  80
//------------------------------------------------------------------------------

#include PLATFORM_HEADER
#include "stack/include/ember-types.h"
#include "stack/include/event.h"
#include "hal/hal.h"
#include "hal/plugin/i2c-driver/i2c-driver.h"
#include "hal/micro/micro.h"
#include "hal/plugin/humidity-temperature-si7021/humidity.h"
#include "hal/plugin/temperature/temperature.h"

#define SI_7021_INIT_DELAY_MS                  50
#define SI_7021_USER_REG_WRITE               0xE6
#define SI_7021_USER_REG_READ                0xE7
#define SI_7021_MEASURE_RELATIVEHUMIDITY     0xF5

#define SI_7021_MEASURE_TEMPERATURE          0xF3
#define SI_7021_READ_TEMPERATURE             0xE0

#define SI_7021_RESET             0xFE

#define SI_7021_WRITE_BUFFER_COMMAND_LOCATION 0
#define SI_7021_WRITE_BUFFER_VALUE_LOCATION   1
#define SI_7021_READ_BUFFER_LOCATION_BYTE_0   0
#define SI_7021_READ_BUFFER_LOCATION_BYTE_1   1

#define ADC_BIT_12    12
#define ADC_BIT_11    11
#define ADC_BIT_10    10
#define ADC_BIT_8     8

#define ADC_BIT_SETTING_12  0 
#define ADC_BIT_SETTING_11  3
#define ADC_BIT_SETTING_10  2
#define ADC_BIT_SETTING_8   1

#define ADC_12_BIT_CONV_TIME_MS       20    
#define ADC_11_BIT_CONV_TIME_MS       15
#define ADC_10_BIT_CONV_TIME_MS       10
#define ADC_8_BIT_CONV_TIME_MS         5 

#define SI_7021_ADDR (0x40<<1)

#define READ_BYTE_SIZE_WITH_CHECKSUM  3
#define READ_BYTE_SIZE_TEMPERATURE    2 
#define READ_BYTE_SIZE_USER_REGISTER  1 
#define SEND_COMMAND_SIZE        1
#define SEND_COMMAND_WITH_VALUE_SIZE  2
#define DEFAULT_READ_BUFFER_SIZE      4
#define DEFAULT_WRITE_BUFFER_SIZE     2
#define DEFAULT_USER_SETTING       0x3A

#ifdef EMBER_AF_PLUGIN_HUMIDITY_SI7021
#define DEFAULT_MEASUREMENT_ACCURACY \
  EMBER_AF_PLUGIN_HUMIDITY_SI7021_MEASUREMENT_ACCURACY  
#endif

static uint8_t measurementReadyDelayMs = ADC_12_BIT_CONV_TIME_MS;
static bool initialized = false;
static bool measureInprogress = false;
static bool startReadBeforeInitialization = false; 
static uint8_t humidityAccuracySet(uint8_t measureAccuracy);
static void si7021Init(void);
//------------------------------------------------------------------------------
// Plugin events
EmberEventControl emberAfPluginHumiditySi7021InitEventControl;
EmberEventControl emberAfPluginHumiditySi7021ReadEventControl;
EmberEventControl emberAfPluginTemperatureSi7021InitEventControl;
EmberEventControl emberAfPluginTemperatureSi7021ReadEventControl;

//------------------------------------------------------------------------------
// Plugin private event handlers

// At this point, I2C driver is guaranteed to have initialized, so it is safe
// to call the i2c based init function
void emberAfPluginHumiditySi7021InitEventHandler(void)
{
  halHumidityInit();
  emberEventControlSetInactive(emberAfPluginHumiditySi7021InitEventControl);
}

void emberAfPluginTemperatureSi7021InitEventHandler(void)
{
  halTemperatureInit();
  emberEventControlSetInactive(emberAfPluginTemperatureSi7021InitEventControl);
}

//------------------------------------------------------------------------------
// Plugin defined callbacks

// The init callback, which will be called by the framework on init.
void emberAfPluginHumiditySi7021InitCallback(void)
{
  emberEventControlSetDelayMS(emberAfPluginHumiditySi7021InitEventControl,
                              SI_7021_INIT_DELAY_MS);
}

void emberAfPluginTemperatureSi7021InitCallback(void)
{
  emberEventControlSetDelayMS(emberAfPluginTemperatureSi7021InitEventControl,
                              SI_7021_INIT_DELAY_MS);
}

// The read event callback, which will be called after the conversion result is
// ready to be read.
void emberAfPluginTemperatureSi7021ReadEventHandler(void)
{
  uint8_t errorCode;
  uint8_t command;
  int32_t temperatureMilliC = 0;
  int32_t temperatureCalculation;
  uint8_t buffer[DEFAULT_READ_BUFFER_SIZE];

  if (startReadBeforeInitialization) {
    startReadBeforeInitialization = false;
    halTemperatureStartRead(); 
  } else {
    emberEventControlSetInactive(emberAfPluginTemperatureSi7021ReadEventControl);
    command = SI_7021_READ_TEMPERATURE;
    errorCode = halI2cWriteBytes(SI_7021_ADDR, &command, SEND_COMMAND_SIZE);
    if (errorCode != I2C_DRIVER_ERR_NONE) {
      halTemperatureReadingCompleteCallback(0, false);
    }
    errorCode = halI2cReadBytes(SI_7021_ADDR,
                                &buffer[SI_7021_READ_BUFFER_LOCATION_BYTE_0],
                                READ_BYTE_SIZE_TEMPERATURE);
    if (errorCode != I2C_DRIVER_ERR_NONE) {
      halTemperatureReadingCompleteCallback(
          SI_7021_READ_BUFFER_LOCATION_BYTE_0,
          false);
    } else {
      //temperature (milliC) = 1000 * (175.72 * ADC /65536 - 46.85)
      //                     = (1000 * ( 5624 * ADC / 8192 - 11994 )) / 256
      temperatureCalculation = HIGH_LOW_TO_INT(
                                 buffer[SI_7021_READ_BUFFER_LOCATION_BYTE_0],
                                 buffer[SI_7021_READ_BUFFER_LOCATION_BYTE_1]);
      temperatureCalculation = 5624 * temperatureCalculation;
      temperatureCalculation = temperatureCalculation >> 13;
      temperatureCalculation = temperatureCalculation - 11994;
      temperatureCalculation = (temperatureCalculation * 1000) >> 8;
      temperatureMilliC = temperatureCalculation;
    }    
    measureInprogress = false;
    halTemperatureReadingCompleteCallback(temperatureMilliC, true);
  }
}

// The read event callback, which will be called after the conversion result is
// ready to be read.
void emberAfPluginHumiditySi7021ReadEventHandler(void)
{
  uint8_t errorCode;
  uint32_t relativeHumidity;
  uint8_t buffer[DEFAULT_READ_BUFFER_SIZE];

  //we started read before initilization, so we did not send any measurement command.
  //so we redo that again here
  if (startReadBeforeInitialization) {
    startReadBeforeInitialization = false;
    halHumidityStartRead(); 
  } else {
    emberEventControlSetInactive(
        emberAfPluginHumiditySi7021ReadEventControl);

    errorCode = halI2cReadBytes(SI_7021_ADDR, 
                                &buffer[SI_7021_READ_BUFFER_LOCATION_BYTE_0],
                                READ_BYTE_SIZE_WITH_CHECKSUM);
    if (errorCode != I2C_DRIVER_ERR_NONE) {
      halHumidityReadingCompleteCallback(0, false);
      return;
    }
    
    relativeHumidity = HIGH_LOW_TO_INT(buffer[SI_7021_READ_BUFFER_LOCATION_BYTE_0],
                                       buffer[SI_7021_READ_BUFFER_LOCATION_BYTE_1]);
    //Relative Humidity in % is calculated as the following:
    //let D = data from Si7021;
    //%RH = ((125 * D) / 65536) - 6;
    //
    //to calculate the Relative Humidity in Centi Percentage:
    //Centi%RH = (((125 * D) / 65536) - 6) * 100
    //         = (((100 * 125 * D) / 65536) - 600);
    relativeHumidity = relativeHumidity * 100;
    relativeHumidity = relativeHumidity * 125;
    relativeHumidity = relativeHumidity >> 16;
    relativeHumidity = relativeHumidity - 600;
    measureInprogress = false;
    halHumidityReadingCompleteCallback((uint16_t)relativeHumidity, true);
  }
}

//------------------------------------------------------------------------------
// Plugin public functions

// All init functionality requires the I2C plugin to be initialized, which is
// not guaranteed to be the case by the time this function executes.  As a
// result, I2C based init functionality needs to execute from task context.
void halTemperatureInit(void)
{
  si7021Init();
}

void halHumidityInit(void)
{
  si7021Init();
}

static void si7021Init(void)
{
  uint8_t registerSetting;
  uint8_t resetMsg = SI_7021_RESET;

  if (initialized == false) {
    // Send the reset command to the temperature sensor
    if (halI2cWriteBytes(SI_7021_ADDR, &resetMsg, SEND_COMMAND_SIZE) 
        != I2C_DRIVER_ERR_NONE) {
      return;
    }

#ifdef EMBER_AF_PLUGIN_HUMIDITY_SI7021
    switch (DEFAULT_MEASUREMENT_ACCURACY) {
    case ADC_BIT_12:
      measurementReadyDelayMs = ADC_12_BIT_CONV_TIME_MS;
      registerSetting = ADC_BIT_SETTING_12;
      break;
    case ADC_BIT_11:
      measurementReadyDelayMs = ADC_11_BIT_CONV_TIME_MS;
      registerSetting = ADC_BIT_SETTING_11;
      break;
    case ADC_BIT_10:
      measurementReadyDelayMs = ADC_10_BIT_CONV_TIME_MS;
      registerSetting = ADC_BIT_SETTING_10;
      break;
    case ADC_BIT_8:
      measurementReadyDelayMs = ADC_8_BIT_CONV_TIME_MS;
      registerSetting = ADC_BIT_SETTING_8;
      break;
    default:
      break;
    }
    halCommonDelayMilliseconds(10); //delay 10ms after reset
    humidityAccuracySet(registerSetting);
#endif
    initialized = true;
  }
}

static uint8_t si7021StartRead(void)
{
  uint8_t errorCode,command;

  errorCode = I2C_DRIVER_ERR_NONE;
  command = SI_7021_MEASURE_RELATIVEHUMIDITY;
  if (measureInprogress == false) {
    errorCode = halI2cWriteBytes(SI_7021_ADDR, &command, SEND_COMMAND_SIZE);
    if (errorCode != I2C_DRIVER_ERR_NONE) {
    } else {
      measureInprogress = true;
    }
  }
  return errorCode;
}

//******************************************************************************
// Perform all I2C transactions of measure command to SI7021 so that it will
// start a single conversion immediately and kick off a event for humidity data 
// read event
//******************************************************************************
void halHumidityStartRead(void)
{
  if (initialized == false) {
    si7021Init();
    startReadBeforeInitialization = true;
  } else {
    si7021StartRead();
  }
  emberEventControlSetDelayMS(emberAfPluginHumiditySi7021ReadEventControl,
                              measurementReadyDelayMs);
}

//******************************************************************************
// Perform all I2C transactions of measure command to SI7021 so that it will
// start a single conversion immediately and kick off a event for temperature
// data read event
//******************************************************************************
void halTemperatureStartRead(void)
{
  if (initialized == false) {
    si7021Init();
    startReadBeforeInitialization = true;
  } else {
    si7021StartRead();
  }
  emberEventControlSetDelayMS(emberAfPluginTemperatureSi7021ReadEventControl,
                              measurementReadyDelayMs);
}

static uint8_t humidityAccuracySet(uint8_t measureAccuracy)
{
  uint8_t errorCode, currentValue;
  uint8_t writeBuffer[DEFAULT_WRITE_BUFFER_SIZE];

  currentValue = DEFAULT_USER_SETTING;
  writeBuffer[SI_7021_WRITE_BUFFER_COMMAND_LOCATION] = SI_7021_USER_REG_READ;
  
  halI2cWriteBytes(SI_7021_ADDR, writeBuffer, SEND_COMMAND_SIZE);
  errorCode = halI2cReadBytes(SI_7021_ADDR, 
                              &currentValue,
                              READ_BYTE_SIZE_USER_REGISTER);
  if (errorCode != I2C_DRIVER_ERR_NONE) {
    return errorCode;
  }
 
  writeBuffer[SI_7021_WRITE_BUFFER_COMMAND_LOCATION] = SI_7021_USER_REG_WRITE;

  // ADCbits[0]: USER REGISTER bit 0 
  // ADCbits[1]: USER REGISTER bit 7 
  // USER REGISTER (7 bit): X X X X X X X
  //                        |           |
  //                     ADCBit[1]   ADCBit[0]
  writeBuffer[SI_7021_WRITE_BUFFER_VALUE_LOCATION] = (currentValue & (0x7E)) |
              (((measureAccuracy & 0x2) << 6) | 
              ((measureAccuracy & 0x1)));
  errorCode = halI2cWriteBytes(SI_7021_ADDR,
                               writeBuffer,
                               DEFAULT_WRITE_BUFFER_SIZE);
  return errorCode;
}
