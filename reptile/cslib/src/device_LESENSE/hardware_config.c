/**************************************************************************//**
 * Copyright 2016 by Silicon Laboratories Inc. All rights reserved.
 *
 * http://developer.silabs.com/legal/version/v11/Silicon_Labs_Software_License_Agreement.txt
 *****************************************************************************/

#include "cslib_hwconfig.h"
#include "hardware_routines.h"
#include "cslib_config.h"

/// @brief Derive thresholds at init based on config profile settings
uint16_t active_thresh[DEF_NUM_SENSORS];
uint16_t inactive_thresh[DEF_NUM_SENSORS];

const GPIO_Port_TypeDef CSLIB_ports[DEF_NUM_SENSORS] =
{
   CSLIB_CHANNEL_PORT
};

const unsigned long CSLIB_pins[DEF_NUM_SENSORS] =
{
   CSLIB_CHANNEL_PIN
};

const uint8_t CSLIB_activeThreshold[DEF_NUM_SENSORS] =
{
  ACTIVE_THRESHOLD_ARRAY
};
const uint8_t CSLIB_inactiveThreshold[DEF_NUM_SENSORS] =
{
  INACTIVE_THRESHOLD_ARRAY
};

const uint8_t CSLIB_averageTouchDelta[DEF_NUM_SENSORS] =
{
  AVERAGE_TOUCH_DELTA_ARRAY
};
