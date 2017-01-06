/**************************************************************************//**
 * Copyright 2016 by Silicon Laboratories Inc. All rights reserved.
 *
 * http://developer.silabs.com/legal/version/v11/Silicon_Labs_Software_License_Agreement.txt
 *****************************************************************************/

#ifndef __CSLIB_HW_CONFIG_H__
#define __CSLIB_HW_CONFIG_H__

#include "em_lesense.h"
#include "em_gpio.h"

/**************************************************************************//**
 *
 * @addtogroup cslib_HWconfig CSLIB library hardware configuration
 * @{
 *
 * @brief Hardware configuration for CSLIB
 *
 * These definitions configure the capacitive sensing hardware in
 * active and sleep mode scans.  They also configure channel-by-channel
 * thresholds and expected touchd deltas.
 *
 *****************************************************************************/


/// @brief Per channel active threshold setting
/// @note Minimum threshold used is @ref INACTIVE_THRESHOLD_ARRAY value,
/// maximum value is 100
#define ACTIVE_THRESHOLD_ARRAY 50, 50

/// @brief Per channel inactive threshold setting
/// @note Minimum threshold used is 1, maximum value is @ref ACTIVE_THRESHOLD_ARRAY
#define INACTIVE_THRESHOLD_ARRAY 30, 30

/// @brief Per-channel expected touch delta.  This value describes the difference
/// in capacitive sensing output codes between the inactive/baseline of the sensor,
/// and the output of the sensor when active(touched).
/// @note These values should be defined in terms of X/16, or X>>4, as they are stored
/// in a packed byte array
#define AVERAGE_TOUCH_DELTA_ARRAY 100>>4, 100>>4

/// @brief Per channel ACMP_CAPSENSE channel value for each enabled sensor
#define CSLIB_MUX_INPUT 3, 4

/// @brief Array stores ACMP input settings
extern const uint16_t CSLIB_muxInput[];

/** @} (end cslib_HWconfig) */
#endif // __CSLIB_HW_CONFIG_H__
