/** @file hal/micro/cortexm3/em35x/em3592/wake-gpio.h
 *
 * @brief
 * Condense WAKE_ON_P* macros into port-specific and monolithic macros.
 *
 * THIS IS A GENERATED FILE.  DO NOT EDIT.
 *
 * <!-- Copyright 2016 Silicon Laboratories, Inc.                        *80*-->
 */

#ifndef __WAKE_GPIO_H__
#define __WAKE_GPIO_H__

// create some handy macros for referring to whole ports
#define EMBER_WAKE_PORT_A    \
  ( (WAKE_ON_PA0 << PA0_BIT) \
  | (WAKE_ON_PA1 << PA1_BIT) \
  | (WAKE_ON_PA2 << PA2_BIT) \
  | (WAKE_ON_PA3 << PA3_BIT) \
  | (WAKE_ON_PA4 << PA4_BIT) \
  | (WAKE_ON_PA5 << PA5_BIT) \
  | (WAKE_ON_PA6 << PA6_BIT) \
  | (WAKE_ON_PA7 << PA7_BIT) \
  )

#define EMBER_WAKE_PORT_B    \
  ( (WAKE_ON_PB0 << PB0_BIT) \
  | (WAKE_ON_PB1 << PB1_BIT) \
  | (WAKE_ON_PB2 << PB2_BIT) \
  | (WAKE_ON_PB3 << PB3_BIT) \
  | (WAKE_ON_PB4 << PB4_BIT) \
  | (WAKE_ON_PB5 << PB5_BIT) \
  | (WAKE_ON_PB6 << PB6_BIT) \
  | (WAKE_ON_PB7 << PB7_BIT) \
  )

#define EMBER_WAKE_PORT_C    \
  ( (WAKE_ON_PC0 << PC0_BIT) \
  | (WAKE_ON_PC1 << PC1_BIT) \
  | (WAKE_ON_PC2 << PC2_BIT) \
  | (WAKE_ON_PC3 << PC3_BIT) \
  | (WAKE_ON_PC4 << PC4_BIT) \
  | (WAKE_ON_PC5 << PC5_BIT) \
  | (WAKE_ON_PC6 << PC6_BIT) \
  | (WAKE_ON_PC7 << PC7_BIT) \
  )

#define EMBER_WAKE_PORT_D    \
  ( (WAKE_ON_PD1 << PD1_BIT) \
  | (WAKE_ON_PD2 << PD2_BIT) \
  | (WAKE_ON_PD3 << PD3_BIT) \
  | (WAKE_ON_PD4 << PD4_BIT) \
  )

#define EMBER_WAKE_PORT_E    \
  ( (WAKE_ON_PE0 << PE0_BIT) \
  | (WAKE_ON_PE1 << PE1_BIT) \
  | (WAKE_ON_PE2 << PE2_BIT) \
  | (WAKE_ON_PE3 << PE3_BIT) \
  )

// A monolithic constant for all GPIO wake sources
#define EMBER_WAKE_GPIO_BITMASK_ARRAY { \
  EMBER_WAKE_PORT_A, \
  EMBER_WAKE_PORT_B, \
  EMBER_WAKE_PORT_C, \
  EMBER_WAKE_PORT_D, \
  EMBER_WAKE_PORT_E, \
  0, \
}


#endif // __WAKE_GPIO_H__
