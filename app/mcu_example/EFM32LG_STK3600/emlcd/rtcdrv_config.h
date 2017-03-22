/***************************************************************************//**
 * @file rtcdrv_config.h
 * @brief RTCDRV configuration file.
 * @version 5.1.2
 *******************************************************************************
 * @section License
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/
#ifndef RTCDRV_CONFIG_H
#define RTCDRV_CONFIG_H

// Define how many timers RTCDRV will provide.
#define EMDRV_RTCDRV_NUM_TIMERS     (1)

// Uncomment the following line to include the wallclock functionality.
//#define EMDRV_RTCDRV_WALLCLOCK_CONFIG

// Uncomment the following line to enable integration with SLEEP driver.
//#define EMDRV_RTCDRV_SLEEPDRV_INTEGRATION

#endif /* RTCDRV_CONFIG_H */
