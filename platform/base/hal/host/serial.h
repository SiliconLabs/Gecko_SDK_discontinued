/** @file hal/host/serial.h
 * See @ref serial and micro specific modules for documentation.
 *
 * <!-- Copyright 2007-2010 by Ember Corporation. All rights reserved.   *80*-->
 */

/** @addtogroup serial
 * @brief This API contains the common HAL interfaces that hosts must
 * implement for the high-level serial code.
 *
 * This header describes the interface between the high-level serial APIs in
 * serial/serial.h and the low level UART implementation.
 *
 * Some functions in this file return an ::EmberStatus value. See 
 * error-def.h for definitions of all ::EmberStatus return values.
 *
 * See serial.h for source code. 
 *@{
 */


#ifndef __HAL_SERIAL_H__
#define __HAL_SERIAL_H__

#include <yfuns.h>
 

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief Assign numerical values for variables that hold Baud Rate
 * parameters. 
 */
enum SerialBaudRate
#else
#ifndef DEFINE_BAUD
#define DEFINE_BAUD(num) BAUD_##num
#endif
typedef uint8_t SerialBaudRate;
enum
#endif //DOXYGEN_SHOULD_SKIP_THIS
{ 
  DEFINE_BAUD(300) = 0,  // BAUD_300
  DEFINE_BAUD(600) = 1,  // BAUD_600
  DEFINE_BAUD(900) = 2,  // etc...
  DEFINE_BAUD(1200) = 3,
  DEFINE_BAUD(2400) = 4,
  DEFINE_BAUD(4800) = 5,
  DEFINE_BAUD(9600) = 6,
  DEFINE_BAUD(14400) = 7,
  DEFINE_BAUD(19200) = 8,
  DEFINE_BAUD(28800) = 9,
  DEFINE_BAUD(38400) = 10,
  DEFINE_BAUD(50000) = 11,
  DEFINE_BAUD(57600) = 12,
  DEFINE_BAUD(76800) = 13,
  DEFINE_BAUD(100000) = 14,
  DEFINE_BAUD(115200) = 15,
  DEFINE_BAUD(230400) = 16,
  DEFINE_BAUD(460800) = 17,
  DEFINE_BAUD(CUSTOM) = 18 
};


#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief Assign numerical values for the types of parity. 
 * Use for variables that hold Parity parameters.
 */
enum NameOfType
#else
#ifndef DEFINE_PARITY
#define DEFINE_PARITY(val) PARITY_##val
#endif
typedef uint8_t SerialParity;
enum
#endif //DOXYGEN_SHOULD_SKIP_THIS
{
  DEFINE_PARITY(NONE) = 0,  // PARITY_NONE
  DEFINE_PARITY(ODD) = 1,   // PARITY_ODD
  DEFINE_PARITY(EVEN) = 2   // PARITY_EVEN
};

/** @name Serial HAL APIs
 * These functions must be implemented by the HAL in order for the
 * serial code to operate. Only the higher-level serial code uses these 
 * functions, so they should not be called directly. The HAL should also 
 * implement the appropriate interrupt handlers to drain the TX queues and fill 
 * the RX FIFO queue, as necessary.
 *
 *@{
 */


/** @brief Initializes the UART to the given settings (same parameters 
 * as  ::emberSerialInit() ).
 * 
 * @param port     Serial port number (0 or 1).
 *  
 * @param rate     Baud rate (see  SerialBaudRate).
 * 
 * @param parity   Parity value (see  SerialParity).
 * 
 * @param stopBits Number of stop bits.
 * 
 * @return An error code if initialization failed (such as invalid baud rate),  
 * otherise EMBER_SUCCESS.
 */
EmberStatus halInternalUartInit(uint8_t port, 
                                SerialBaudRate rate,
                                SerialParity parity,
                                uint8_t stopBits);

/** @brief Returns the number bytes available in the transmit queue when
 * using the EMBER_SERIAL_USE_STDIO variant of the Ember serial library.
 *
 * @return Number of bytes available in the transmit queue.
 */
uint16_t halInternalPrintfWriteAvailable(void);

/** @brief Returns the number bytes available in the receive queue when
 * using the EMBER_SERIAL_USE_STDIO variant of the Ember serial library.
 *
 * @return Number of bytes available in the receive queue.
 */
uint16_t halInternalPrintfReadAvailable(void);

/** @brief This function enables/disables EMBER_SERIAL_USE_STDIO printing
 * behavior that is compatible with emberSerialGuaranteedPrintf() and a
 * replacement for halInternalForceWriteUartData(). (blocking, bypass queue,
 * and polling)
 */
void halInternalForcePrintf(bool onOff);


#ifndef DOXYGEN_SHOULD_SKIP_THIS
//Refer to uart.h for better documentation of fflush and stdout.

#if defined(__IAR_SYSTEMS_ICC__) && !defined(fflush)
  size_t fflush(int handle);
#endif

#if defined(__IAR_SYSTEMS_ICC__) && !defined(stdout)
  #define stdout _LLIO_STDOUT
#endif

#endif

/** @}  END of Serial HAL APIs */

#endif //__HAL_SERIAL_H__

/** @} END serial group  */
  
