/***************************************************************************//**
 * @file btl_driver_uart.h
 * @brief Universal UART driver for the Silicon Labs Bootloader.
 * @author Silicon Labs
 * @version 1.0.0
 *******************************************************************************
 * # License
 * <b>Copyright 2016 Silicon Laboratories, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#ifndef BTL_DRIVER_UART_H
#define BTL_DRIVER_UART_H

#include "em_device.h"
#include <stdbool.h>
#include <stddef.h>
#include "api/btl_errorcode.h"

/***************************************************************************//**
 * @addtogroup Driver
 * @{
 * @addtogroup Uart UART
 * @brief Serial UART Interface driver
 * @details Flexible UART driver implementation for communication with
 *          external devices.
 *
 *    This driver will support both blocking and non-blocking operation,
 *    with LDMA backing the background transfers to support nonblocking.
 *    Additionally, support for hardware flow control is included.
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * Initialize the configured USART peripheral for UART operation. Also sets up
 *  GPIO settings for TX, RX, and (if configured) flow control.
 ******************************************************************************/
void uart_init(void);

/***************************************************************************//**
 * Write a data buffer to the uart
 *
 * @param[in] buffer   The data buffer to send
 * @param[in] length   Number of bytes in the buffer to send
 * @param[in] blocking Indicate whether we can offload this transfer to LDMA
 *                     and return, or we should wait on completion before
 *                     returning.
 *
 * @return BOOTLOADER_OK if successful, error code otherwise
 ******************************************************************************/
int32_t uart_sendBuffer(uint8_t* buffer, size_t length, bool blocking);

/***************************************************************************//**
 * Write one byte to the uart in a blocking fashion.
 *
 * @param[in] byte The byte to send
 *
 * @return BOOTLOADER_OK if succesful, error code otherwise
 ******************************************************************************/
int32_t uart_sendByte(uint8_t byte);

/***************************************************************************//**
 * Figure out whether the UART can accept more data to send
 *
 * @return true if the uart is not currently transmitting
 ******************************************************************************/
bool    uart_isTxIdle(void);

/***************************************************************************//**
 * Get the number of bytes ready for reading.
 *
 * @return Number of bytes in the receive buffer available for reading with
 *   @ref uart_receiveBuffer
 ******************************************************************************/
size_t  uart_getRxAvailableBytes(void);

/***************************************************************************//**
 * Read from the UART into a data buffer
 *
 * @param[out] buffer The data buffer to receive into
 * @param[in] requestedLength Number of bytes we'd like to read
 * @param[out] receivedLength Number of bytes read
 * @param[in] blocking Indicate whether we should wait for requestedLength
 *   bytes to be available and read before returning, or we can read out
 *   whatever is currently in the buffer and return.
 * @param[in] timeout Number of milliseconds to wait for data in blocking mode
 *
 * @return BOOTLOADER_OK if succesful, error code otherwise
 ******************************************************************************/
int32_t uart_receiveBuffer(uint8_t* buffer,
                           size_t requestedLength,
                           size_t* receivedLength,
                           bool blocking,
                           uint32_t timeout);

/***************************************************************************//**
 * Get one byte from the uart in a blocking fashion.
 *
 * @param[out] byte The byte to send
 *
 * @return BOOTLOADER_OK if succesful, error code otherwise
 ******************************************************************************/
int32_t uart_receiveByte(uint8_t* byte);

/***************************************************************************//**
 * Flush one or both UART buffers.
 *
 * @param[in] flushTx Flush the transmit buffer when true
 * @param[in] flushRx Flush the receive buffer when true
 *
 * @return BOOTLOADER_OK
 ******************************************************************************/
int32_t uart_flush(bool flushTx, bool flushRx);

/**
 * @} // addtogroup Uart
 * @} // addtogroup Driver
 */

#endif // BTL_DRIVER_UART_H
