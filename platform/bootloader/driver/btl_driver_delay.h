/***************************************************************************//**
 * @file btl_driver_delay.h
 * @brief Hardware driver layer for simple delay.
 * @author Silicon Labs
 * @version 1.0.0
 *******************************************************************************
 * # License
 * <b>Copyright 2016 Silicon Laboratories, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#ifndef BTL_DRIVER_DELAY_H
#define BTL_DRIVER_DELAY_H

#include <stdbool.h>
#include <stdint.h>

/**
 * @addtogroup Driver
 * @brief Hardware drivers for bootloader
 * @{
 * @addtogroup Delay
 * @brief Basic delay functionality
 * @details Simple delay routines for use with plugins that require small delays.
 * @{
 */

/**
 * Delay for a number of microseconds
 *
 * @note This function can be used without calling delay_init() first.
 *
 * @param usecs Number of microseconds to delay
 */
void delay_microseconds(uint32_t usecs);


/**
 * Initialize the delay driver's millisecond counter
 */
void delay_init(void);

/**
 * Delay for a number of milliseconds
 *
 * @param msecs    Number of milliseconds to delay
 * @param blocking Whether to block until the delay has expired. If false, the
 *                 @ref delay_expired() function can be called to check whether
 *                 the delay has expired.
 */
void delay_milliseconds(uint32_t msecs, bool blocking);

/**
 * Check whether the current delay has expired
 *
 * @return True if the delay has expired
 */
bool delay_expired(void);

/**
 * @} // addtogroup Delay
 * @} // addtogroup Driver
 */

#endif // BTL_DRIVER_DELAY_H
