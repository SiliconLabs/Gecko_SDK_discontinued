/**************************************************************************//**
 * Copyright 2016 by Silicon Laboratories Inc. All rights reserved.
 *
 * http://developer.silabs.com/legal/version/v11/Silicon_Labs_Software_License_Agreement.txt
 *****************************************************************************/

#include "cslib_config.h"
#include <stdio.h>
#include <stdint.h>
#include "cslib_sensor_descriptors.h"

/// @brief Contains string names for each defined sensor
const char* sensorDescriptors[DEF_NUM_SENSORS] =
{
 SENSOR_DESCRIPTOR_LIST
};

/***************************************************************************//**
 * @brief
 *   Outputs a line to Capacitive Sensing Profiler naming each sensor
 *
 ******************************************************************************/
void outputsensorDescriptors(void)
{
  uint8_t index;
  // Output control word showing Profiler that line contains sensor names
  printf("*SENSOR_DESCRIPTORS ");

  for(index = 0; index < DEF_NUM_SENSORS; index++)
  {
    printf("%s | ",sensorDescriptors[index]);
  }
  printf("\n");
}

