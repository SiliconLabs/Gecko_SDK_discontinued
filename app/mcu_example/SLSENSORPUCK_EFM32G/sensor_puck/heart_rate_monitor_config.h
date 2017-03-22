/**************************************************************************//**
 * @file
 * @brief Heart Rate peripheral config
 * @version 5.1.2
 ******************************************************************************
 * @section License
 * <b>Copyright 2015 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#ifndef __HEARTRATEMONITOR_CFG_H
#define __HEARTRATEMONITOR_CFG_H

#define HRM_INCLUDE_SI1147PS

// select timers to use for 100us timestamp
#define HRM_TIMER1 TIMER0
#define HRM_TIMER2 TIMER1
#define CLK_HRM_TIMER1  cmuClock_TIMER0
#define CLK_HRM_TIMER2  cmuClock_TIMER1



#endif /* __HEARTRATEMONITOR_CFG_H */
