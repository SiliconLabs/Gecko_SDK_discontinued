//  Copyright 2015 Silicon Laboratories, Inc.                              *80*
//
#include PLATFORM_HEADER
#include "stack/include/ember-types.h"
#include "stack/include/event.h"
#include "hal/hal.h"
#include "hal/plugin/i2c-driver/i2c-driver.h"
#include "hal/micro/micro.h"
#include "hal/plugin/illuminance-si1141/illuminance.h"
#include "hal/plugin/illuminance-si1141/illuminance-si1141.h"

// Shorter macros for plugin options
#define MEASUREMENT_SAMPLE_SIZE \
  (EMBER_AF_PLUGIN_ILLUMINANCE_SI1141_NUMBER_OF_SAMPLES)

#define LOG_TABLE_SIZE 90

//log table of 89 entries from 10000*LOG(1.1) to 10000*LOG(9.9)
static const uint16_t logTable[LOG_TABLE_SIZE] =
{
  0,    414,  792,  1139, 1461, 1761, 2041, 2304, 2553, 2788, 3010, 3222, 3424,
  3617, 3802, 3979, 4150, 4314, 4472, 4624, 4771, 4914, 5051, 5185, 5315, 5441,
  5563, 5682, 5798, 5911, 6021, 6128, 6232, 6335, 6435, 6532, 6628, 6721, 6812,
  6902, 6990, 7076, 7160, 7243, 7324, 7404, 7482, 7559, 7634, 7709, 7782, 7853,
  7924, 7993, 8062, 8129, 8195, 8261, 8325, 8388, 8451, 8513, 8573, 8633, 8692,
  8751, 8808, 8865, 8921, 8976, 9031, 9085, 9138, 9191, 9243, 9294, 9345, 9395,
  9445, 9494, 9542, 9590, 9638, 9685, 9731, 9777, 9823, 9868, 9912, 9956
};
static uint8_t operationMode = HAL_ILLUMINANCE_SI1141_LEVEL_3;
static uint32_t illumSummation;
static uint8_t sampleIndex;
static uint8_t luxMultiplier = HAL_ILLUMINANCE_SI1141_DEFAULT_MULTIPLIER;
typedef struct
{
  uint8_t current[3];
  uint8_t taskList;
  uint8_t measRateMultiplier;
  uint8_t adcGain;
  uint8_t adcMisc;
  uint8_t adcMux[3];
} Si1141Param_t;
//------------------------------------------------------------------------------
// Plugin events
EmberEventControl emberAfPluginIlluminanceSi1141InitEventControl;
EmberEventControl emberAfPluginIlluminanceSi1141ReadEventControl;

//------------------------------------------------------------------------------
//private status functions

//pause Proximity and Ambient Light Sensing operation
static uint8_t si1141PsAlsPause(uint8_t deviceAddr);

//single force Ambient Light Sensing operation
static uint8_t si114xAlsForce(uint8_t deviceAddr);

//pause all operations
static uint8_t si114xPauseAll(uint8_t deviceAddr);

//software reset the si114x senor
static uint8_t si114xReset(uint8_t deviceAddr);

//initilization procedure of si114x
static uint8_t si114xInitialize(uint8_t deviceAddr,
                                Si1141Param_t *pSi1141Param);

//confiugration routine for si114x
static uint8_t si114xConfigure(uint8_t deviceAddr,
                               Si1141Param_t *pSi1141Param);

//write to register
static uint8_t si114xWriteToRegister(uint8_t deviceAddr,
                                     uint8_t address,
                                     uint8_t value);

//read from register
static uint8_t si114xReadFromRegister(uint8_t deviceAddr,
                                      uint8_t address,
                                      uint8_t * pvalue);

//write a block of data to si114x
static uint8_t si114xBlockWrite(uint8_t deviceAddr,
                                uint8_t address,
                                uint8_t length,
                                uint8_t *values);

//parameter ram setup for si114x
static uint8_t parameterDataSet(uint8_t deviceAddr,
                                uint8_t address,
                                uint8_t value);

//change the gain factor of si114x, which include software reset of the sensor
static void si114xChangeGain(uint8_t deviceAddr, uint8_t modeTobeChanged);

//this routine waits for the sensor to finish processing and go abck to sleep
//mode, it might block the processor for 10ms under abnormal situation, i.e.
//the processing time of the sensor is longer than normal.
static uint8_t waitUntilIdle(uint8_t deviceAddr);

//this routine send a command to si114x and make sure it's properly responded
static uint8_t sendCommand(uint8_t deviceAddr, uint8_t command);

//------------------------------------------------------------------------------
// Plugin private event handlers

void emberAfPluginIlluminanceSi1141InitEventHandler(void)
{
  halIlluminanceInit();
  emberEventControlSetInactive(emberAfPluginIlluminanceSi1141InitEventControl);
}

void emberAfPluginIlluminanceSi1141ReadEventHandler(void)
{
  uint8_t dataLow, dataHigh;
  uint16_t logLux, rawData, logTableScale, residue;
  uint32_t lux, combinedData, compareStep, adder, divisor;
  uint8_t errorCode;

  if (sampleIndex == 0) {
    si114xAlsForce(HAL_ILLUMINANCE_SI1141_I2C_ADDR);
    sampleIndex++;
    emberEventControlSetDelayMS(
                        emberAfPluginIlluminanceSi1141ReadEventControl,
                        HAL_ILLUMINANCE_SI1141_MAX_CONVERSION_TIME_MS);
  } else if (sampleIndex <= MEASUREMENT_SAMPLE_SIZE) {
    si114xReadFromRegister(HAL_ILLUMINANCE_SI1141_I2C_ADDR,
                           HAL_ILLUMINANCE_SI1141_REG_ALS_VIS_DATA0,
                           &dataLow);
    si114xReadFromRegister(HAL_ILLUMINANCE_SI1141_I2C_ADDR,
                           HAL_ILLUMINANCE_SI1141_REG_ALS_VIS_DATA1,
                           &dataHigh);
    rawData = HIGH_LOW_TO_INT(dataHigh, dataLow);
    illumSummation += rawData;
    sampleIndex++;
    si114xAlsForce(HAL_ILLUMINANCE_SI1141_I2C_ADDR);
    emberEventControlSetDelayMS(emberAfPluginIlluminanceSi1141ReadEventControl,
                                HAL_ILLUMINANCE_SI1141_MAX_CONVERSION_TIME_MS);
  } else {
    combinedData = illumSummation / MEASUREMENT_SAMPLE_SIZE;

    //anything lower than the ADC_OFFSET specified by the chip spec, should be
    //considered as 0 reading, anything larger than the offset, should substract
    // the offset from the raw adc reading
    if (combinedData <= HAL_ILLUMINANCE_SI1141_ADC_OFFSET) {
      combinedData  = 0;
    } else {
      combinedData = combinedData - HAL_ILLUMINANCE_SI1141_ADC_OFFSET;
    }

    //operationMode is the accuracy setting of the sensor, the adc reading
    //scaling equals to the 2^operationMode
    //luxMultiplier is the parameter from calibration of cli command, which
    //should reflect the gain of the diffuser meterial when taking other lux
    //meter as reference
    lux = ((combinedData) * luxMultiplier ) >> operationMode;
    logLux = 0;

    if (lux != 0) {
      //our log table is a hundred point table from 1-10,
      //the scale is 10 time the integer lux value
      logTableScale = lux * 10;
      adder = 0;
      divisor = 1;

      //
      //for 10 < lux < 100
      //10000 * log(x) =
      //10000 * log(10) + 10000 * log(x / 10)
      //
      //for 100 < lux < 1000
      //10000 * log(x) =
      //10000 * log(100) + 10000 * log(x / 100)
      //
      //for 1000 < lux < 10000
      //10000 * log(x) =
      //10000 * log(1000) + 10000 * log(x / 1000)
      //
      //each step is one decade in term of log10, we start from 10
      compareStep = 10;
      while (lux > compareStep) {
        //each step is one decade in term of log10
        compareStep = compareStep * 10;
        adder = adder + 10000;
        divisor = divisor * 10;
      }

      //our logtable acutally starts from 1.0, not 0.0, so the index needed to
      //be shift by 10
      logLux = adder + logTable[logTableScale / divisor - 10];

      //the value lookup above gives us a floored value in the log table
      //use linear intepolation for any value between two table entries.
      //
      residue = lux % divisor;
      residue = (logTable[(logTableScale / divisor - 10) + 1]
                 - logTable[logTableScale / divisor - 10])
                 * residue
                 / divisor;
      logLux = residue + logLux;
    }

    //Si1141's accuracy is proportional to the gain, to measure higher
    //lux, we need lower gain and we thus have less accuracy, to measure
    //lower lux we need higher gain and we have higher accuracy but easier
    //to saturate. so we have a dynamic gain setting mechanism here to
    //swtich different gain level according to different lux we got.
    switch (operationMode ) {
    case HAL_ILLUMINANCE_SI1141_LEVEL_3://highest precision, gain 7
      if (combinedData > HAL_ILLUMINANCE_SI1141_LEVEL3_UPPER_BOUND) {
        halIlluminanceStartRead(luxMultiplier);
        si114xChangeGain(HAL_ILLUMINANCE_SI1141_I2C_ADDR,
                         HAL_ILLUMINANCE_SI1141_LEVEL_2);
        return;
      }
      break;
    case HAL_ILLUMINANCE_SI1141_LEVEL_2://middle level gain 3
      if (combinedData > HAL_ILLUMINANCE_SI1141_LEVEL2_UPPER_BOUND) {
        si114xChangeGain(HAL_ILLUMINANCE_SI1141_I2C_ADDR,
                         HAL_ILLUMINANCE_SI1141_LEVEL_1);
        halIlluminanceStartRead(luxMultiplier);
        return;
      } else if (combinedData < HAL_ILLUMINANCE_SI1141_LEVEL2_LOWER_BOUND) {
        si114xChangeGain(HAL_ILLUMINANCE_SI1141_I2C_ADDR,
                         HAL_ILLUMINANCE_SI1141_LEVEL_3);
        halIlluminanceStartRead(luxMultiplier);
        return;
      }
      break;
    case HAL_ILLUMINANCE_SI1141_LEVEL_1://lowest precision gain 1
      if (combinedData < HAL_ILLUMINANCE_SI1141_LEVEL1_LOWER_BOUND) {
        si114xChangeGain(HAL_ILLUMINANCE_SI1141_I2C_ADDR,
                         HAL_ILLUMINANCE_SI1141_LEVEL_2);
        halIlluminanceStartRead(luxMultiplier);
        return;
      }
      break;
    default:
      break;
    }
    halIlluminanceReadingCompleteCallback(logLux);
    errorCode = si1141PsAlsPause(HAL_ILLUMINANCE_SI1141_I2C_ADDR);
    //it should not return error, something wrong here if it does
    //then we have to reset everything again.
    if (errorCode != HAL_ILLUMINANCE_SI1141_ERR_NONE) {
      emberEventControlSetActive(
        emberAfPluginIlluminanceSi1141InitEventControl);
    }
    emberEventControlSetInactive(
        emberAfPluginIlluminanceSi1141ReadEventControl);
  }
}

//------------------------------------------------------------------------------
// Plugin defined callbacks

// The init callback, which will be called by the framework on init.
void emberAfPluginIlluminanceSi1141InitCallback(void)
{
  emberEventControlSetDelayMS(emberAfPluginIlluminanceSi1141InitEventControl,
                              HAL_ILLUMINANCE_SI1141_INIT_DELAY_MS);
}

//------------------------------------------------------------------------------
// Plugin public functions

//Initialize the hardware
void halIlluminanceInit(void)
{
  Si1141Param_t parameterData;
  si114xInitialize(HAL_ILLUMINANCE_SI1141_I2C_ADDR, &parameterData);
}

uint8_t halIlluminanceCalibrate(uint32_t extLux)
{
  uint8_t dataLow, dataHigh, i;
  uint32_t rawData, combinedData, summation;

  summation = 0;
  for (i = 0; i < MEASUREMENT_SAMPLE_SIZE; i++) {
    si114xAlsForce(HAL_ILLUMINANCE_SI1141_I2C_ADDR);
    halCommonDelayMilliseconds(HAL_ILLUMINANCE_SI1141_MAX_CONVERSION_TIME_MS);
    si114xReadFromRegister(HAL_ILLUMINANCE_SI1141_I2C_ADDR,
                           HAL_ILLUMINANCE_SI1141_REG_ALS_VIS_DATA0,
                           &dataLow);
    si114xReadFromRegister(HAL_ILLUMINANCE_SI1141_I2C_ADDR,
                           HAL_ILLUMINANCE_SI1141_REG_ALS_VIS_DATA1,
                           &dataHigh);
    rawData = HIGH_LOW_TO_INT(dataHigh, dataLow);
    summation += rawData;
    halCommonDelayMilliseconds(HAL_ILLUMINANCE_SI1141_MAX_CONVERSION_TIME_MS);
  }
  combinedData = summation / MEASUREMENT_SAMPLE_SIZE;
  if (combinedData <= HAL_ILLUMINANCE_SI1141_ADC_OFFSET) {
    combinedData = 0;
  } else {
    combinedData = combinedData - HAL_ILLUMINANCE_SI1141_ADC_OFFSET;
  }
  luxMultiplier = (uint8_t)(((uint32_t)extLux << operationMode) / combinedData);

  return luxMultiplier;
}

uint16_t halIlluminanceStartRead(uint8_t multiplier)
{
  uint8_t value;

  if (multiplier != 0) {
    luxMultiplier = multiplier;
  }
  illumSummation = 0;
  sampleIndex = 0;

  si114xReadFromRegister(HAL_ILLUMINANCE_SI1141_I2C_ADDR,
                         HAL_ILLUMINANCE_SI1141_REG_CHIP_STAT,
                         &value);
  emberEventControlSetDelayMS(emberAfPluginIlluminanceSi1141ReadEventControl,
                              HAL_ILLUMINANCE_SI1141_MAX_CONVERSION_TIME_MS);
  return 0;
}

//-----------------------------------------------------------------------------
//static local funcitons
static uint8_t si114xWriteToRegister(uint8_t deviceAddr,
                                     uint8_t address,
                                     uint8_t value)
{
  uint8_t writeBuffer[2];

  writeBuffer[0] = address;
  writeBuffer[1] = value;

  if (I2C_DRIVER_ERR_NONE !=
      halI2cWriteBytes(deviceAddr, writeBuffer, sizeof(writeBuffer))) {
    return HAL_ILLUMINANCE_SI1141_ERR_COMMMUICATION;
  }
  return HAL_ILLUMINANCE_SI1141_ERR_NONE;
}

static uint8_t si114xReadFromRegister(uint8_t deviceAddr,
                                      uint8_t address,
                                      uint8_t * readBuffer)
{
  uint8_t localAddress;

  localAddress = address;

  if (I2C_DRIVER_ERR_NONE !=
      halI2cWriteBytes(deviceAddr, &localAddress, sizeof(localAddress))) {
    return HAL_ILLUMINANCE_SI1141_ERR_COMMMUICATION;
  }
  if (I2C_DRIVER_ERR_NONE !=
      halI2cReadBytes(deviceAddr, readBuffer, sizeof(*readBuffer))) {
    return HAL_ILLUMINANCE_SI1141_ERR_COMMMUICATION;
  }
  return HAL_ILLUMINANCE_SI1141_ERR_NONE ;
}

static uint8_t si114xBlockWrite(uint8_t deviceAddr,
                                uint8_t address,
                                uint8_t length,
                                uint8_t *values)
{
  uint8_t errorCode, counter;

  for ( counter = 0; counter < length; counter++) {
    errorCode = si114xWriteToRegister(deviceAddr,
                                      address+counter,
                                      values[counter]);
  }

  return errorCode;
}

// This loops until the Si114x is known to be in its sleep state
// or if an i2c error occurs
static uint8_t waitUntilIdle(uint8_t deviceAddr)
{
  uint8_t errorCode = 0;
  uint8_t value = 0;
  errorCode = si114xReadFromRegister(deviceAddr,
                                     HAL_ILLUMINANCE_SI1141_REG_CHIP_STAT,
                                     &value);
  value &= HAL_ILLUMINANCE_SI1141_CHIP_STAT_MASK;
  if ((errorCode != HAL_ILLUMINANCE_SI1141_ERR_NONE)
      || ((value & HAL_ILLUMINANCE_SI1141_CHIP_IDLE) == 0)) {
    //try again after 10ms, it shouldn't be happening in normal situation
    halCommonDelayMilliseconds(10);
    errorCode = si114xReadFromRegister(deviceAddr,
                                       HAL_ILLUMINANCE_SI1141_REG_CHIP_STAT,
                                       &value);
    value &= HAL_ILLUMINANCE_SI1141_CHIP_STAT_MASK;
  }
  return errorCode;
}

static uint8_t sendCommand(uint8_t deviceAddr, uint8_t command)
{
  uint8_t response = 0;
  uint8_t response1 =0;
  uint8_t errorCode = HAL_ILLUMINANCE_SI1141_ERR_NONE;

  // Get the response register contents
  si114xReadFromRegister(deviceAddr,
                         HAL_ILLUMINANCE_SI1141_REG_RESPONSE,
                         &response);

  //Warning!! this operation might block the processor for 10ms
  if (errorCode = (waitUntilIdle(deviceAddr))
                  != HAL_ILLUMINANCE_SI1141_ERR_NONE ) {
    return errorCode;
  }

  // Double-check the response register is consistent if it not NOP command
  if (command != HAL_ILLUMINANCE_SI1141_COMMAND_NOP) {
    errorCode = si114xReadFromRegister(deviceAddr,
                                       HAL_ILLUMINANCE_SI1141_REG_RESPONSE,
                                       &response1);
    if (errorCode != HAL_ILLUMINANCE_SI1141_ERR_NONE) {
      return HAL_ILLUMINANCE_SI1141_ERR_COMMMUICATION;
    }
    if (response1 != response) {
      return HAL_ILLUMINANCE_SI1141_ERR_DEVICE_BUSY;
    }
  }

  // Send the Command
  errorCode = si114xWriteToRegister(deviceAddr,
                                    HAL_ILLUMINANCE_SI1141_REG_COMMAND,
                                    command);
  if (errorCode != HAL_ILLUMINANCE_SI1141_ERR_NONE) {
    return errorCode;
  }

  // Expect a change in the response register
  if (command == HAL_ILLUMINANCE_SI1141_COMMAND_NOP) {
    return errorCode; // Skip if the command is NOP
  }

  errorCode = si114xReadFromRegister(deviceAddr,
                                     HAL_ILLUMINANCE_SI1141_REG_RESPONSE,
                                     &response);
  if (errorCode != HAL_ILLUMINANCE_SI1141_ERR_NONE) {
    return HAL_ILLUMINANCE_SI1141_ERR_COMMMUICATION;
  }
  return errorCode;
}

static uint8_t si114xNop(uint8_t deviceAddr)
{
 return sendCommand(deviceAddr, HAL_ILLUMINANCE_SI1141_COMMAND_NOP);
}

static uint8_t si114xAlsForce (uint8_t deviceAddr)
{
 return sendCommand(deviceAddr, HAL_ILLUMINANCE_SI1141_COMMAND_ALS_FORCE);
}

static uint8_t si1141PsAlsPause (uint8_t deviceAddr)
{
  return sendCommand(deviceAddr, HAL_ILLUMINANCE_SI1141_COMMAND_PSALS_PAUSE);
}

static uint8_t si114xPauseAll(uint8_t deviceAddr)
{
  uint8_t value, errorCode;

  //  After a RESET, if the Si114x receives a command (including NOP) before
  //  the Si114x has gone to sleep, the chip hangs. This first while loop
  //  avoids this. The reading of the REG_CHIPSTAT does not disturb the
  //  internal MCU.
  if (errorCode = (waitUntilIdle(deviceAddr))
                  != HAL_ILLUMINANCE_SI1141_ERR_NONE ) {
    return errorCode;
  }
  si114xNop(deviceAddr);
  errorCode = si1141PsAlsPause(deviceAddr);
  if (errorCode != HAL_ILLUMINANCE_SI1141_ERR_NONE) {
    return errorCode;
  }
  halCommonDelayMilliseconds(5);

  // the repsonse should be the command counter, si1141PsAlsPause should be our
  // first commnad. so if the response is good, we expect it to be a '1'.
  errorCode = si114xReadFromRegister(deviceAddr,
                                     HAL_ILLUMINANCE_SI1141_REG_RESPONSE,
                                     &value);
  if (value != HAL_ILLUMINANCE_SI1141_RESET_RESPOND) { // no properly reset.
    return HAL_ILLUMINANCE_SI1141_ERR_DEVICE_BUSY;
  } else {
    return HAL_ILLUMINANCE_SI1141_ERR_NONE;
  }
}

static uint8_t parameterDataSet(uint8_t deviceAddr,
                                uint8_t address,
                                uint8_t value)
{
  uint8_t errorCode;
  uint8_t buffer[2];
  uint8_t responseStored;
  uint8_t response;

  //Warning!! this operation might block the processor for 10ms
  if (errorCode = (waitUntilIdle(deviceAddr))
                  != HAL_ILLUMINANCE_SI1141_ERR_NONE) {
    return errorCode;
  }

  errorCode = si114xReadFromRegister(deviceAddr,
                                     HAL_ILLUMINANCE_SI1141_REG_RESPONSE,
                                     &responseStored);

  buffer[0] = value;
  buffer[1] = HAL_ILLUMINANCE_SI1141_COMMAND_PARAM_SET
              + (address & HAL_ILLUMINANCE_SI1141_PARAM_RAM_OFFSET_MASK);

  errorCode = si114xBlockWrite(deviceAddr,
                               HAL_ILLUMINANCE_SI1141_REG_PARAM_WR,
                               sizeof(buffer),
                               (uint8_t*) buffer);
  if (errorCode != HAL_ILLUMINANCE_SI1141_ERR_NONE) {
    return errorCode;
  }

  // Wait for command to finish
  errorCode = si114xReadFromRegister(deviceAddr,
                                     HAL_ILLUMINANCE_SI1141_REG_RESPONSE,
                                     &response);
  // we expect the response register is changed here, try again
  if ((response == responseStored )
      || (errorCode != HAL_ILLUMINANCE_SI1141_ERR_NONE)) {
    halCommonDelayMilliseconds(10);
    errorCode = si114xReadFromRegister(deviceAddr,
                                       HAL_ILLUMINANCE_SI1141_REG_RESPONSE,
                                       &response);
    if (response == responseStored) {
      return HAL_ILLUMINANCE_SI1141_ERR_DEVICE_BUSY;
    }
    if (errorCode != HAL_ILLUMINANCE_SI1141_ERR_NONE) {
      return HAL_ILLUMINANCE_SI1141_ERR_COMMMUICATION;
    }
  }
  return errorCode;
}

static uint8_t si114xReset(uint8_t deviceAddr)
{
  uint8_t errorCode;

  errorCode = HAL_ILLUMINANCE_SI1141_ERR_NONE; //this is actually 0
  errorCode = si114xWriteToRegister(deviceAddr,
                                    HAL_ILLUMINANCE_SI1141_REG_MEAS_RATE,
                                    0x00);
  if (errorCode != HAL_ILLUMINANCE_SI1141_ERR_NONE) {
    return errorCode;
  }

  errorCode = si114xWriteToRegister(deviceAddr,
                                    HAL_ILLUMINANCE_SI1141_REG_ALS_RATE,
                                    0x00);
  if (errorCode != HAL_ILLUMINANCE_SI1141_ERR_NONE) {
    return errorCode;
  }

  errorCode = si114xPauseAll(deviceAddr);
  if (errorCode != HAL_ILLUMINANCE_SI1141_ERR_NONE) {
    return errorCode;
  }

  // The clearing of the registers could be redundant, but it is okay.
  // This is to make sure that these registers are cleared.
  errorCode = si114xWriteToRegister(deviceAddr,
                                    HAL_ILLUMINANCE_SI1141_REG_MEAS_RATE,
                                    0x00);
  if (errorCode != HAL_ILLUMINANCE_SI1141_ERR_NONE) {
    return HAL_ILLUMINANCE_SI1141_ERR_COMMMUICATION;
  }

  errorCode = si114xWriteToRegister(deviceAddr,
                                    HAL_ILLUMINANCE_SI1141_REG_IRQ_ENABLE,
                                    0x00);
  if (errorCode != HAL_ILLUMINANCE_SI1141_ERR_NONE) {
    return HAL_ILLUMINANCE_SI1141_ERR_COMMMUICATION;
  }

  errorCode = si114xWriteToRegister(deviceAddr,
                                    HAL_ILLUMINANCE_SI1141_REG_IRQ_MODE1,
                                    0x00);
  if (errorCode != HAL_ILLUMINANCE_SI1141_ERR_NONE) {
    return HAL_ILLUMINANCE_SI1141_ERR_COMMMUICATION;
  }

  errorCode = si114xWriteToRegister(deviceAddr,
                                    HAL_ILLUMINANCE_SI1141_REG_IRQ_MODE2,
                                    0x00);
  if (errorCode != HAL_ILLUMINANCE_SI1141_ERR_NONE) {
    return HAL_ILLUMINANCE_SI1141_ERR_COMMMUICATION;
  }

  errorCode = si114xWriteToRegister(deviceAddr,
                                    HAL_ILLUMINANCE_SI1141_REG_INT_CFG,
                                    0x00);
  if (errorCode != HAL_ILLUMINANCE_SI1141_ERR_NONE) {
    return HAL_ILLUMINANCE_SI1141_ERR_COMMMUICATION;
  }

  errorCode = si114xWriteToRegister(deviceAddr,
                                    HAL_ILLUMINANCE_SI1141_REG_IRQ_STATUS,
                                    0xFF);
  if (errorCode != HAL_ILLUMINANCE_SI1141_ERR_NONE) {
    return HAL_ILLUMINANCE_SI1141_ERR_COMMMUICATION;
  }

  // Perform the Reset Command
  errorCode = si114xWriteToRegister(deviceAddr,
                                    HAL_ILLUMINANCE_SI1141_REG_COMMAND,
                                    HAL_ILLUMINANCE_SI1141_COMMAND_RESET);
  if (errorCode != HAL_ILLUMINANCE_SI1141_ERR_NONE) {
    return HAL_ILLUMINANCE_SI1141_ERR_COMMMUICATION;
  }

  // Delay for 10 ms. This delay is needed to allow the Si114x
  // to perform internal reset sequence.
  halCommonDelayMilliseconds(10);

  // Write Hardware Key
  errorCode = si114xWriteToRegister(deviceAddr,
                                    HAL_ILLUMINANCE_SI1141_REG_HW_KEY,
                                    HAL_ILLUMINANCE_SI1141_HW_KEY_VAL0);
  return errorCode;
}

static uint8_t si114xConfigure(uint8_t deviceAddr, Si1141Param_t * pSi1141Param)
{
  uint8_t errorCode, count;

  errorCode = HAL_ILLUMINANCE_SI1141_ERR_NONE; //this is actually 0

  // Note that the Si114xReset() actually performs the following functions:
  //     1. Pauses all prior measurements
  //     2. Clear  i2c registers that need to be cleared
  //     3. Clears irq status to make sure INT* is negated
  //     4. Delays 10 ms
  //     5. Sends HW Key
  count = 0;
  while (count < HAL_ILLUMINANCE_SI1141_RETRY_COUNT) {
    errorCode = si114xReset(deviceAddr);
    if (errorCode == HAL_ILLUMINANCE_SI1141_ERR_NONE) {
      break;
    }
    count++;
  }
  if (count == HAL_ILLUMINANCE_SI1141_RETRY_COUNT) {
      return errorCode;
  }

  // clearing the LED driving registers to 0
  errorCode = si114xWriteToRegister(deviceAddr,
                                    HAL_ILLUMINANCE_SI1141_REG_PS_LED21,
                                    0x0);
  if (errorCode != HAL_ILLUMINANCE_SI1141_ERR_NONE) {
    return HAL_ILLUMINANCE_SI1141_ERR_COMMMUICATION;
  }

  errorCode = si114xWriteToRegister(deviceAddr,
                                    HAL_ILLUMINANCE_SI1141_REG_PS_LED3,
                                    0x0);
  if (errorCode != HAL_ILLUMINANCE_SI1141_ERR_NONE) {
    return HAL_ILLUMINANCE_SI1141_ERR_COMMMUICATION;
  }

  // Initialize CHLIST Parameter from caller to enable measurement
  // Valid Tasks are: HAL_ILLUMINANCE_SI1141_ALS_VIS_TASK, ALS_IR_TASK, PS1_TASK
  //                  PS2_TASK, PS3_TASK and AUX_TASK
  errorCode = parameterDataSet(deviceAddr,
                               HAL_ILLUMINANCE_SI1141_PARAM_CH_LIST,
                               pSi1141Param->taskList);
  if (errorCode != HAL_ILLUMINANCE_SI1141_ERR_NONE) {
    return errorCode;
  }

  // clear IRQ Modes disable it and write INT CFG to interrupt on every sample
  errorCode = si114xWriteToRegister(deviceAddr,
                                    HAL_ILLUMINANCE_SI1141_REG_INT_CFG,
                                    HAL_ILLUMINANCE_SI1141_ICG_INTOE);
  if (errorCode != HAL_ILLUMINANCE_SI1141_ERR_NONE) {
    return HAL_ILLUMINANCE_SI1141_ERR_COMMMUICATION;
  }

  errorCode = si114xWriteToRegister(deviceAddr,
                                    HAL_ILLUMINANCE_SI1141_REG_IRQ_ENABLE,
                                    0);
  if (errorCode != HAL_ILLUMINANCE_SI1141_ERR_NONE) {
    return HAL_ILLUMINANCE_SI1141_ERR_COMMMUICATION;
  }

  errorCode = si114xWriteToRegister(deviceAddr,
                                    HAL_ILLUMINANCE_SI1141_REG_IRQ_MODE1,
                                    0);
  if (errorCode != HAL_ILLUMINANCE_SI1141_ERR_NONE) {
    return HAL_ILLUMINANCE_SI1141_ERR_COMMMUICATION;
  }

  errorCode = si114xWriteToRegister(deviceAddr,
                                    HAL_ILLUMINANCE_SI1141_REG_IRQ_MODE2,
                                    0);
  if (errorCode != HAL_ILLUMINANCE_SI1141_ERR_NONE) {
    return HAL_ILLUMINANCE_SI1141_ERR_COMMMUICATION ;
  }

  // Configure the ALS VIS channel mgain
  errorCode = parameterDataSet(deviceAddr,
                               HAL_ILLUMINANCE_SI1141_PARAM_ALSVIS_ADC_GAIN,
                               pSi1141Param->adcGain);
  if (errorCode != HAL_ILLUMINANCE_SI1141_ERR_NONE) {
    return errorCode;
  }

  // Configure the ALS VIS channel recovery time
  errorCode = parameterDataSet(deviceAddr,
                               HAL_ILLUMINANCE_SI1141_PARAM_ALSVIS_ADC_COUNTER,
                               (~(pSi1141Param->adcGain)) << 4);
  if (errorCode != HAL_ILLUMINANCE_SI1141_ERR_NONE) {
    return errorCode;
  }

  // clear the misc register, which is used for LED driving. we don't need it
  // at this point.
  errorCode = parameterDataSet(deviceAddr,
                               HAL_ILLUMINANCE_SI1141_PARAM_ALSVIS_ADC_MISC,
                               0);
  if (errorCode != HAL_ILLUMINANCE_SI1141_ERR_NONE) {
    return errorCode;
  }

  // If the caller passes a negative measRateMultiplier, it means that it does
  // not want to start autonomous measurements.
  if (pSi1141Param->measRateMultiplier > 0) {
    errorCode = si114xWriteToRegister(deviceAddr,
                                      HAL_ILLUMINANCE_SI1141_REG_MEAS_RATE,
                                      pSi1141Param->measRateMultiplier);
    if (errorCode != HAL_ILLUMINANCE_SI1141_ERR_NONE) {
      return HAL_ILLUMINANCE_SI1141_ERR_COMMMUICATION;
    }

    errorCode = si114xWriteToRegister(deviceAddr,
                                      HAL_ILLUMINANCE_SI1141_REG_PS_RATE,
                                      0x00);
    if (errorCode != HAL_ILLUMINANCE_SI1141_ERR_NONE) {
      return HAL_ILLUMINANCE_SI1141_ERR_COMMMUICATION;
    }

    // if 0x08, VIS, IR, AUX Measurements every time device wakes up.
    errorCode = si114xWriteToRegister(deviceAddr,
                                      HAL_ILLUMINANCE_SI1141_REG_ALS_RATE,
                                      0x08);
    if (errorCode != HAL_ILLUMINANCE_SI1141_ERR_NONE) {
      return HAL_ILLUMINANCE_SI1141_ERR_COMMMUICATION;
    }
  }
  return errorCode;
}

static uint8_t si114xInitialize(uint8_t deviceAddr, Si1141Param_t *pSi1141Param)
{
  uint8_t errorCode = HAL_ILLUMINANCE_SI1141_ERR_NONE ;
  uint8_t chipId = 0;

  pSi1141Param->current[0] = 0x0;
  pSi1141Param->current[1] = 0x0;
  pSi1141Param->current[2] = 0x0;

  // we set up for ambient light measurement only
  pSi1141Param->taskList = HAL_ILLUMINANCE_SI1141_ALS_VIS_TASK;
  pSi1141Param->measRateMultiplier = 0;// we use single measurement each time.
  // 0xa0 every 30.0 ms
  // 0x94 every 20.0 ms
  // 0x84 every 10.4 ms
  // 0x74 every  5.2 ms
  // 0x70 every  4.2 ms
  // 0x60 every  3.2 ms
  // 0x21 every  158 us

  pSi1141Param->adcGain = operationMode;// PS_ADC_GAIN to div by 128

  // PS_ADC_MISC to default Normal Signal Range
  pSi1141Param->adcMisc = HAL_ILLUMINANCE_SI1141_ADC_NORM;

  si114xReadFromRegister(deviceAddr,
                         HAL_ILLUMINANCE_SI1141_REG_PART_ID,
                         &chipId);
  errorCode = si114xConfigure(deviceAddr, pSi1141Param);
  if (errorCode != HAL_ILLUMINANCE_SI1141_ERR_NONE) {
    return errorCode;
  }
  errorCode = si1141PsAlsPause(deviceAddr);
  if (errorCode != HAL_ILLUMINANCE_SI1141_ERR_NONE) {
    return errorCode;
  }
  return errorCode;
}

static void si114xChangeGain(uint8_t deviceAddr, uint8_t modeTobeChanged)
{
  Si1141Param_t parameterData;

  operationMode = modeTobeChanged;
  si114xInitialize(HAL_ILLUMINANCE_SI1141_I2C_ADDR, &parameterData);
}
