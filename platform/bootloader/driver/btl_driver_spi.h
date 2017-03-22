/***************************************************************************//**
 * @file btl_driver_spi.h
 * @brief Hardware driver layer for simple SPI transactions.
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

#ifndef BTL_DRIVER_SPI_H
#define BTL_DRIVER_SPI_H

#include "em_usart.h"

/**
 * @addtogroup Driver
 * @{
 * @addtogroup Spi SPI
 * @brief Basic Serial Peripheral Interface driver
 * @details Simple, blocking SPI master implementation for communication with
 *          external devices.
 * @{
 */

/**
 * Initialize a USART peripheral for SPI
 */
void spi_init(void);

/**
 * Write a single byte, discarding the received byte
 *
 * @param[in] data The byte to send
 */
void spi_writeByte(uint8_t data);

/**
 * Write two bytes, discarding the received bytes
 *
 * @param[in] data The bytes to send, most significant byte first
 */
void spi_writeHalfword(uint16_t data);

/**
 * Write three bytes, discarding the received bytes
 *
 * @param[in] data The bytes to send, most significant byte first
 */
void spi_write3Byte(uint32_t data);

/**
 * Read a byte by sending a 0xFF byte
 *
 * @return The received byte
 */
uint8_t spi_readByte(void);

/**
 * Read two bytes by sending two 0xFF bytes
 *
 * @return The received bytes, most significant byte first
 */
uint16_t spi_readHalfword(void);

/**
 * Assert the slave select line. Polarity is configured by @ref spi_init.
 */
void spi_setCsActive(void);

/**
 * Dessert the slave select line. Polarity is configured by @ref spi_init.
 */
void spi_setCsInactive(void);

/**
 * @} // addtogroup Spi
 * @} // addtogroup Driver
 */

#endif // BTL_DRIVER_SPI_H
