/**************************************************************************//**
 * Copyright 2016 by Silicon Laboratories Inc. All rights reserved.
 *
 * http://developer.silabs.com/legal/version/v11/Silicon_Labs_Software_License_Agreement.txt
 *****************************************************************************/

#include "cslib_config.h"
#include "cslib.h"

// Struct defined in sensor_node_routines.h.  Contains all state data for a
// sensor.
SensorStruct_t CSLIB_node[DEF_NUM_SENSORS];
uint8_t CSLIB_numSensors = DEF_NUM_SENSORS;
uint8_t CSLIB_sensorBufferSize = DEF_SENSOR_BUFFER_SIZE;
uint8_t CSLIB_buttonDebounce = DEF_BUTTON_DEBOUNCE;
uint16_t CSLIB_activeModePeriod = DEF_ACTIVE_MODE_PERIOD;
uint16_t CSLIB_sleepModePeriod = DEF_SLEEP_MODE_PERIOD;
uint16_t CSLIB_countsBeforeSleep = DEF_COUNTS_BEFORE_SLEEP;
uint8_t CSLIB_freeRunSetting = DEF_FREE_RUN_SETTING;
uint8_t CSLIB_sleepModeEnable = DEF_SLEEP_MODE_ENABLE;

