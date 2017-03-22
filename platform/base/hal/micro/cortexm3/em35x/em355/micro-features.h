/** @file hal/micro/cortexm3/em35x/em355/micro-features.h
 *
 * @brief
 * Constants to indicate which features the em355 has available
 *
 * THIS IS A GENERATED FILE.  DO NOT EDIT.
 *
 * <!-- Copyright 2016 Silicon Laboratories, Inc.                        *80*-->
 */

#ifndef __MICRO_FEATURES_H__
#define __MICRO_FEATURES_H__

#define CORTEXM3_EM35X_GEN3 1

// Masks of which GPIO this chip has on which ports
#define EMBER_MICRO_PORT_A_GPIO 0xBF // 7   5 4 3 2 1 0
#define EMBER_MICRO_PORT_B_GPIO 0x9F // 7     4 3 2 1 0
#define EMBER_MICRO_PORT_C_GPIO 0xFF // 7 6 5 4 3 2 1 0

// Which physical serial ports are available
#define EMBER_MICRO_HAS_SC1 1
#define EMBER_MICRO_HAS_SC2 1
#define EMBER_MICRO_SERIAL_CONTROLLERS_MASK 0x03




#endif // __MICRO_FEATURES_H__
