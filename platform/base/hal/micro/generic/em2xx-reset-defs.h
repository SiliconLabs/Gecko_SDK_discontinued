/** @file hal/micro/generic/em2xx-reset-defs.h
 * @brief Definitions of reset types compatible with EM2xx usage.
 *
 * <!-- Copyright 2009 by Ember Corporation.             All rights reserved.-->
 */

#ifndef __EM2XX_RESET_DEFS_H__
#define __EM2XX_RESET_DEFS_H__


/** @addtogroup em2xx_resets
 *@{
 */
/** @brief EM2xx-compatible reset code returned by halGetEm2xxResetInfo()
 *@{
 */
#define EM2XX_RESET_UNKNOWN               0
#define EM2XX_RESET_EXTERNAL              1   // EM2XX reports POWERON instead
#define EM2XX_RESET_POWERON               2
#define EM2XX_RESET_WATCHDOG              3
#define EM2XX_RESET_ASSERT                6
#define EM2XX_RESET_BOOTLOADER            9
#define EM2XX_RESET_SOFTWARE              11
/**@} */

/**@}  // end of CRC Functions
 */

#endif    //__EM2XX_RESET_DEFS_H__
