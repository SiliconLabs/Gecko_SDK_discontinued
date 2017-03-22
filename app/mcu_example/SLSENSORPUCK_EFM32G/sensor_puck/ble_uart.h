/**************************************************************************//**
* @file
* @brief Bluetooth Low Energy UART interface driver
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

#ifndef BLE_UART_H_
#define BLE_UART_H_


void BLE_UART_Init(void);
void BLE_UART_Send(uint8_t *pMsg, int MsgLength);
#endif /* BLE_UART_H_ */
