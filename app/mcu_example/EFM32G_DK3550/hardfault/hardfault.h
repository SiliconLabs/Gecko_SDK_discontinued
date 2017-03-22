/**************************************************************************//**
 * @file
 * @brief Hardfault handler for Cortex-M3, prototypes and definitions
 * @author Joseph Yiu, Frank Van Hooft, Silicon Labs
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

#ifndef __HARDFAULT_H
#define __HARDFAULT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void HardFault_TrapDivByZero(void);
void HardFault_TrapUnaligned(void);
void HardFault_HandlerC(uint32_t *stack_pointer);

#ifdef __cplusplus
}
#endif

#endif
