/***************************************************************************//**
 * @file btl_driver_spislave.h
 * @brief Universal SPI slave driver for the Silicon Labs Bootloader.
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

#ifndef BTL_DRIVER_SPISLAVE_H
#define BTL_DRIVER_SPISLAVE_H

#include "em_device.h"
#include <stdbool.h>
#include <stddef.h>
#include "api/btl_errorcode.h"

/***************************************************************************//**
 * @addtogroup Driver
 * @{
 * @addtogroup Spislave SPI Slave
 * @brief SPI Slave Interface driver
 * @details Flexible SPI Slave driver implementation for use in an NCP
 *    scenario.
 *
 *    This driver will support both blocking and non-blocking operation,
 *    with LDMA backing the background transfers to support nonblocking.
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * Initialize the configured USART peripheral for SPI slave operation.
 *  Also sets up GPIO settings for MOSI, MISO, SCLK and SS.
 *  After initialization, the SPI slave will have RX enabled.
 *
 * @return BOOTLOADER_OK if succesful, error code otherwise
 ******************************************************************************/
void spislave_init(void);

/***************************************************************************//**
 * Reset the configured USART peripheral and return MOSI, MISO, CLK and SS
 * to a Hi-Z state.
 ******************************************************************************/
void spislave_deinit(void);

/***************************************************************************//**
 * Write a data buffer to the master next time the master starts clocking SCLK.
 * This transfer will be non-blocking, and its progress can be tracked through
 * @ref spislave_getTxBytesLeft
 *
 * @param[in] buffer   The data buffer to send
 * @param[in] length   Number of bytes in the buffer to send
 *
 * @return BOOTLOADER_OK if successful, error code otherwise
 ******************************************************************************/
int32_t spislave_sendBuffer(uint8_t* buffer,
                            size_t length);

/***************************************************************************//**
 * Write one byte to the master in a blocking fashion.
 *
 * @warning If the master goes down, this will block forever.
 *
 * @param[in] byte The byte to send
 *
 * @return BOOTLOADER_OK if succesful, error code otherwise
 ******************************************************************************/
int32_t spislave_sendByte(uint8_t byte);

/***************************************************************************//**
 * Gets the amount of bytes left in the TX data buffer.
 *
 * @return Number of bytes in the transmit buffer still needing to go out
 ******************************************************************************/
size_t  spislave_getTxBytesLeft(void);

/***************************************************************************//**
 * Enable/disable MISO output
 *
 * @param[in] enable True to enable the transmitter, false to disable
 ******************************************************************************/
void spislave_enableTransmitter(bool enable);

/***************************************************************************//**
 * Enable/disable receiving bytes from the master into our internal buffer.
 * The purpose of this function is to avoid filling up the buffer with 0xFF
 *   while a master is polling for new data.
 *
 * @param[in] enable True to enable the receiver, false to disable
 ******************************************************************************/
void spislave_enableReceiver(bool enable);

/***************************************************************************//**
 * Gets the amount of bytes ready for reading.
 *
 * @return Number of bytes in the receive buffer available for reading with
 *         @ref spislave_receiveBuffer
 ******************************************************************************/
size_t  spislave_getRxAvailableBytes(void);

/***************************************************************************//**
 * Read from the RX buffer into a local buffer.
 *
 * @param[out] buffer The data buffer to receive into
 * @param[in]  requestedLength Number of bytes we'd like to read
 * @param[out] receivedLength  Number of bytes read
 * @param[in]  blocking Indicate whether we should wait for requestedLength
 *   bytes to be available and read before returning, or we can read out
 *   whatever is currently in the buffer and return.
 * @param[in] timeout Number of milliseconds to wait for data in blocking mode
 *
 * @return BOOTLOADER_OK if succesful, error code otherwise
 ******************************************************************************/
int32_t spislave_receiveBuffer(uint8_t* buffer,
                               size_t requestedLength,
                               size_t* receivedLength,
                               bool blocking,
                               uint32_t timeout);

/***************************************************************************//**
 * Get one byte from the SPI slave in a blocking fashion.
 *
 * @warning If the master never clocks in a byte, this function will block
 *          forever.
 *
 * @param[out] byte Pointer to where to put the received byte
 *
 * @return BOOTLOADER_OK if succesful, error code otherwise
 ******************************************************************************/
int32_t spislave_receiveByte(uint8_t* byte);

/***************************************************************************//**
 * Flush one or both buffers.
 *
 * @param[in] flushTx Flush the transmit buffer when true
 * @param[in] flushRx Flush the receive buffer when true
 ******************************************************************************/
void spislave_flush(bool flushTx, bool flushRx);

/**
 * @} // addtogroup Uart
 * @} // addtogroup Driver
 */

#endif // BTL_DRIVER_SPISLAVE_H
