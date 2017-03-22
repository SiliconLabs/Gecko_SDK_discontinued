/**************************************************************************//**
 * @file
 * @brief Heart Rate peripheral config
 * @version 5.1.2
 ******************************************************************************
 * @section License
 * <b>Copyright 2015 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#ifndef __HEARTRATEMONITOR_CFG_H
#define __HEARTRATEMONITOR_CFG_H

// select timers to use for 100us timestamp
#define HRM_TIMER1      TIMER2
#define HRM_TIMER2      TIMER3
#define CLK_HRM_TIMER1  cmuClock_TIMER2
#define CLK_HRM_TIMER2  cmuClock_TIMER3

#define enableGreenLED() do { \
     GPIO_PinOutClear(gpioPortC, 3); \
     GPIO_PinOutSet(gpioPortE, 2); \
     GPIO_PinOutSet(gpioPortE, 3); \
     } while (0)

#define disableGreenLED() do { \
     GPIO_PinOutSet(gpioPortC, 3); \
     } while (0)

#define enableRedLED() do { \
     GPIO_PinOutClear(gpioPortC, 0); \
     GPIO_PinOutSet(gpioPortE, 2); \
     GPIO_PinOutSet(gpioPortE, 3); \
     } while (0)

#define disableRedLED() do { \
     GPIO_PinOutSet(gpioPortC, 0); \
     GPIO_PinOutClear(gpioPortE, 2); \
     GPIO_PinOutClear(gpioPortE, 3); \
     } while (0)

#endif /* __HEARTRATEMONITOR_CFG_H */
