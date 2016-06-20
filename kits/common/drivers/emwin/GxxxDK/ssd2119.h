/***************************************************************************//**
 * @file
 * @brief SSD2119 LCD controller driver
 * @version 4.0.0
 *******************************************************************************
 * @section License
 * <b>(C) Copyright 2012 Energy Micro AS, http://www.energymicro.com</b>
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
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Energy Micro AS has no
 * obligation to support this Software. Energy Micro AS is providing the
 * Software "AS IS", with no express or implied warranties of any kind,
 * including, but not limited to, any implied warranties of merchantability
 * or fitness for any particular purpose or warranties against infringement
 * of any proprietary rights of a third party.
 *
 * Energy Micro AS will not be liable for any consequential, incidental, or
 * special damages, or any other relief, or for any claim by any third party,
 * arising from your use of this Software.
 *
 ******************************************************************************/
#ifndef __SSD2119_H
#define __SSD2119_H

#include <stdbool.h>
#include "em_device.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

#define notSSD2119_REGISTER_ACCESS_HOOKS
/** if you define this value, driver include functions allowing registering
 * your own TFT controller functions. This will allow changing way of accessing
 * controller to SPI for example but will also slow graphics.
*/

#define notLCD_DEVFUNC_CONTRADDR     0x35

/** if you define below value, direct access to TFT controller will be replaced
 * with access to addresses defined by separate function. This will allow
 * dynamic TFT controller address change but will also slow graphics.
*/

/** Performance verification (Release version compiled with IAR):
  *  NO FUNCTIONS HOOKS & NO VARIABLE CONTROLLER ADDRESS: 800 kpix/s
  *  NO FUNCTIONS HOOKS & VARIABLE CONTROLLER ADDRESS:    623 kpix/s
  *  FUNCTIONS HOOKS & NO VARIABLE CONTROLLER ADDRESS:    618 kpix/s
  *  FUNCTIONS HOOKS & VARIABLE CONTROLLER ADDRESS:       537 kpix/s
*/

#ifdef SSD2119_REGISTER_ACCESS_HOOKS
  #define LCD_DEVFUNC_WRITEREGISTER 0x30
  #define LCD_DEVFUNC_SETREGISTER   0x31
  #define LCD_DEVFUNC_WRITEDATA     0x32
  #define LCD_DEVFUNC_READDATA      0x33
#endif

#define LCD_DEVFUNC_INITIALIZE    0x34

#ifndef LCD_DEVFUNC_CONTRADDR
#define SSD2119_REGISTER_ADDRESS 0x88000000
#define SSD2119_DATA_ADDRESS     0x88000002
#endif

/** @endcond */

#ifdef __cplusplus
}
#endif

#endif /* __SSD2119_H */
