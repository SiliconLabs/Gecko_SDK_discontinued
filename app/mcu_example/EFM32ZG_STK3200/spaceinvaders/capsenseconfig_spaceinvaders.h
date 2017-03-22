/***************************************************************************//**
 * @file
 * @brief capsense configuration parameters.
 * @version 5.1.2
 *******************************************************************************
 * @section License
 * <b>Copyright 2015 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#ifndef __CAPSENSCONFIG_SPACEINVADERS_H
#define __CAPSENSCONFIG_SPACEINVADERS_H
#ifdef __cplusplus
extern "C" {
#endif

#define ACMP_CAPSENSE                           ACMP0
#define ACMP_CAPSENSE_CLKEN                     CMU_HFPERCLKEN0_ACMP0
#define PRS_CH_CAPSENSE                         (1)
#define TIMER_CC_PRS_CH_CAPSENSE                TIMER_CC_CTRL_PRSSEL_PRSCH1
#define PRS_CH_CTRL_SOURCESEL_ACMP_CAPSENSE     PRS_CH_CTRL_SOURCESEL_ACMP0
#define PRS_CH_CTRL_SIGSEL_ACMPOUT_CAPSENSE     PRS_CH_CTRL_SIGSEL_ACMP0OUT

#define ACMP_CHANNELS           5             /**< Number of channels for the Analog Comparator */

#define BUTTON0_CHANNEL         4             /**< Button 0 channel */
#define BUTTON1_CHANNEL         3             /**< Button 1 channel */

#define CAPSENSE_CH_IN_USE  { false, false, false, true, true }

#ifdef __cplusplus
}
#endif
#endif
