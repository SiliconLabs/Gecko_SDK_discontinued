/***************************************************************************//**
 * @file rangetestconfig.c
 * @brief Range Test static configuration source.
 *
 * This file contains the different application configurations for each
 * separate radio cards.
 *
 * @version 5.1.2
 *******************************************************************************
 * @section License
 * <b>Copyright 2015 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Silicon Labs has no
 * obligation to support this Software. Silicon Labs is providing the
 * Software "AS IS", with no express or implied warranties of any kind,
 * including, but not limited to, any implied warranties of merchantability
 * or fitness for any particular purpose or warranties against infringement
 * of any proprietary rights of a third party.
 *
 * Silicon Labs will not be liable for any consequential, incidental, or
 * special damages, or any other relief, or for any claim by any third party,
 * arising from your use of this Software.
 *
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>

#include "radio-configuration.h"
#include "app-configuration.h"
#include "rangetestconfigtypes.h"

const rangeTestChannelConfig_t rangeTestChannelConf[] =
  RANGETEST_CHANNEL_DEFINITIONS;

const rangeTestStaticConfig_t rangeTestConf =
{
#if defined(RADIO_CONFIGURATION_DATA_RADIO_XO_FREQ)
    RADIO_CONFIGURATION_DATA_RADIO_XO_FREQ,
#elif defined(RADIO_CONFIGURATION_DATA_CRYSTAL_FREQUENCY)
    RADIO_CONFIGURATION_DATA_CRYSTAL_FREQUENCY,
#else
    RANGETEST_RADIO_XTAL_FREQ,
#endif
  { RANGETEST_RADIO_DATA_RATE },
  { RANGETEST_RADIO_MODULATION },
  { RANGETEST_RADIO_DEVIATION },
  { RANGETEST_RADIO_OUTPUT_POWER },
  { RANGETEST_RADIO_RSSI_MIN_VALUE },
  { RANGETEST_RADIO_RSSI_MID_VALUE },
  { RANGETEST_RADIO_RSSI_MAX_VALUE },
};

const uint8_t radio_config[] = RADIO_CONFIGURATION_DATA_ARRAY;

rangeTestFieldConfig_t rangeTestFieldConf = RANGETEST_FIELD_CONFIG;
