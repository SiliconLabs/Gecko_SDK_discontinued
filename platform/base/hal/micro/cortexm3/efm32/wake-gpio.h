/** @file hal/micro/cortexm3/efm32/wake-gpio.h
 *
 * <!-- Copyright 2016 Silicon Laboratories, Inc.                        *80*-->
 */

#ifndef WAKE_GPIO_H
#define WAKE_GPIO_H

// create some handy macros for referring to whole ports
// A monolithic constant for all GPIO wake sources
#define EMBER_WAKE_GPIO_BITMASK_ARRAY { \
  0, \
  0, \
  0, \
  0, \
  0, \
  0, \
}

// TODO: Extend for Jumbo, which will have 12 ports

#endif // WAKE_GPIO_H
