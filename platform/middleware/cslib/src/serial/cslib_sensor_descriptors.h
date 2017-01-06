/**************************************************************************//**
 * Copyright 2016 by Silicon Laboratories Inc. All rights reserved.
 *
 * http://developer.silabs.com/legal/version/v11/Silicon_Labs_Software_License_Agreement.txt
 *****************************************************************************/

#ifndef CSLIB_SENSOR_DESCRIPTORS_H
#define CSLIB_SENSOR_DESCRIPTORS_H

void outputsensorDescriptors(void);

#define HAS_SENSOR_DESCRIPTORS

#if (DEF_NUM_SENSORS == 2)
#define SENSOR_DESCRIPTOR_LIST "B0", "B1"
#elif (DEF_NUM_SENSORS == 4)
#define SENSOR_DESCRIPTOR_LIST "B0", "B1", "B2", "B3"
#else
#error "Unsupported CSLIB sensor count."
#endif

#endif // CSLIB_SENSOR_DESCRIPTORS_H
