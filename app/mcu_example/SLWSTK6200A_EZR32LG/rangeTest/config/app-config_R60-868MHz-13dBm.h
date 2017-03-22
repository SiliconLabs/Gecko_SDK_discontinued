/***************************************************************************//**
 * @file app-config_R60-868MHz-13dBm.h
 * @brief Range Test application configuration for SLWSTK6200A kit.
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

#ifndef RANGETESTCONFIG_H_
#define RANGETESTCONFIG_H_


#define RANGETEST_CHANNEL_DEFINITIONS \
{ /* Start, End, Spacing, Base */ \
  {0, 16, 250000, 868000000}, \
}

#define RANGETEST_FIELD_CONFIG \
{ \
  /*sendCrc*/         true,   \
  /*calcCrc*/         true,   \
  /*checkCrc*/        true,   \
  /*whitenEnable*/    false,  \
  /*length*/          24,   \
}

//TX: 11 chars, RX: 7 chars, TRX: 7 chars
#define RANGETEST_RADIO_DATA_RATE      "9.6kbps"

//TX: 10 chars, RX: 5 chars, TRX: 5 chars
#define RANGETEST_RADIO_MODULATION     "2GFSK"

//TX: 11 chars, RX: 7 chars, TRX: 7 chars
#define RANGETEST_RADIO_DEVIATION      "4.8kHz"

//TX: 8 chars, TRX: 6 chars
#define RANGETEST_RADIO_OUTPUT_POWER   "+13dBm"

// RSSI chart Y scale values
#define RANGETEST_RADIO_RSSI_MIN_VALUE "-115"
#define RANGETEST_RADIO_RSSI_MID_VALUE "-42"
#define RANGETEST_RADIO_RSSI_MAX_VALUE "10"

#endif /* RANGETESTCONFIG_H_ */
