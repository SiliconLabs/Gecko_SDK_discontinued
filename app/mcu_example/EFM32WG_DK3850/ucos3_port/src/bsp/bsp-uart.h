/**************************************************************************//**
 * @file
 * @brief USART1 prototypes and definitions
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

#ifndef __BSP_UART_H
#define __BSP_UART_H

#ifdef __cplusplus
extern "C" {
#endif


/***************************************************************************//**
 * @addtogroup Drivers
 * @{
 ******************************************************************************/

int  UART1_ReadChar(void);
int  UART1_WriteChar(char c);

void UART1_SerialCrLf(int on);
void UART1_SerialInit(void);

/** @} (end group Drivers) */

#ifdef __cplusplus
}
#endif

#endif
