/***************************************************************************//**
 * @file com_device.h
 * @brief Device specific COM settings.
 * @version 0.01.0
 *******************************************************************************
 * @section License
 * <b>(C) Copyright 2014 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *

 ******************************************************************************/
#ifndef __COM_DEVICE_H__
#define __COM_DEVICE_H__

// Toggle COM ports to optimize code size
#define COM_USART0_ENABLE
//#define COM_USART1_ENABLE
//#define COM_USART2_ENABLE
#define COM_VCP_ENABLE

// Define the number of com ports. This should never change
#define COM_PORTS 4
#define COM_UART_PORTS 3
#define COM_USB_PORTS 0
#define COM_VCP_PORTS 1

#if (COM_UART_PORTS + COM_USB_PORTS + COM_VCP_PORTS) != COM_PORTS
  #error COM ports not defined properly
#endif

#endif //__COM_DEVICE_H__
