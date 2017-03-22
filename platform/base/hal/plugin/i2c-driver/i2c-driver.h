// Copyright 2014 Silicon Laboratories, Inc.
#ifndef __I2C_DRIVER_H__
#define __I2C_DRIVER_H__

// Return codes for read/write transaction error handling
#define I2C_DRIVER_ERR_NONE         0x00
#define I2C_DRIVER_ERR_TIMEOUT      0x01
#define I2C_DRIVER_ERR_ADDR_NAK     0x02
#define I2C_DRIVER_ERR_DATA_NAK     0x03
#define I2C_DRIVER_ERR_ARB_LOST     0x04
#define I2C_DRIVER_ERR_USAGE_FAULT  0x05
#define I2C_DRIVER_ERR_SW_FAULT     0x06
#define I2C_DRIVER_ERR_UNKOWN       0xFF

/** @brief Initializes the I2C plugin. The application framework will generally
 * initialize this plugin automatically. Customers who do not use the framework
 * must ensure the plugin is initialized by calling this function.
 */
void halI2cInitialize(void);

/** @brief Write bytes
 *
 * This function writes a set of bytes to an I2C slave, returning immediately
 * after the last write is done.
 *
 * @param address Address of the I2C slave
 *
 * @param buffer  Pointer to the buffer holding the data
 *
 * @param count   Number of bytes to be written
 *
 * @return 0 if I2C operation was successful. Error code if unsuccesful
 */
uint8_t halI2cWriteBytes(uint8_t address,
                       const uint8_t *buffer,
                       uint8_t count);

/** @brief Write bytes with delay
 *
 * This function writes a set of bytes to an I2C slave, returning after a delay
 * after the last write is done.
 *
 * @param address Address of the I2C slave
 *
 * @param buffer  Pointer to the buffer holding the data
 *
 * @param count   Number of bytes to be written
 *
 * @param delay   Number of ms to wait before returning after last write
 *
 * @return 0 if I2C operation was successful. Error code if unsuccesful
 */
uint8_t halI2cWriteBytesDelay(uint8_t address,
                         const uint8_t *buffer,
                         uint8_t count,
                         uint8_t delay);

/** @brief Read bytes
 *
 * This function reads a set of bytes from an I2C slave, returning immediately
 * after the last read is done.
 *
 * @param address Address of the I2C slave
 *
 * @param buffer  Pointer to the buffer to hold the incoming data
 *
 * @param count   Number of bytes to be read
 *
 * @return 0 if I2C operation was successful. Error code if unsuccesful
 */
uint8_t halI2cReadBytes(uint8_t address,
                      uint8_t *buffer,
                      uint8_t count);

#endif // __I2C_DRIVER_H__
