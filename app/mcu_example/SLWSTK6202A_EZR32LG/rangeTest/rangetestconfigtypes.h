/***************************************************************************//**
 * @file rangetestconfigtypes.h
 * @brief Prototype definitions for Range Test application and radio
 * configuration structs.
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

#ifndef RANGETESTSTATICCONFIG_H_
#define RANGETESTSTATICCONFIG_H_

typedef struct rangeTestFieldConfig
{
  bool sendCrc;
  bool calcCrc;
  bool checkCrc;
  bool whEnable;
  uint16_t length;
} rangeTestFieldConfig_t;

typedef struct rangeTestChannelConfig
{
  uint16_t chStart;
  uint16_t chEnd;
  uint32_t chSpacing;
  uint32_t baseFreq;
} rangeTestChannelConfig_t;

typedef struct rangeTestStaticConfig
{
  uint32_t radioXTALFreq;
  char     radioDataRate[20u];
  char     radioModulation[20u];
  char     radioDeviation[20u];
  char     radioOutputPower[20u];
  char     rssiMinValue[5u];
  char     rssiMidValue[5u];
  char     rssiMaxValue[5u];
} rangeTestStaticConfig_t;

#endif /* RANGETESTSTATICCONFIG_H_ */
