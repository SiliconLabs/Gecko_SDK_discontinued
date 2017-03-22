/***************************************************************************//**
 * @file bootloader-serial.h
 * @brief Common bootloader serial definitions.
 * See @ref cbhc_serial for detailed documentation.
 * @version 3.20.2
 *******************************************************************************
 * @section License
 * <b>(C) Copyright 2015 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Silicon Labs has no
 * obligation to support this Software. Silicon Labs is providing the
 * Software "AS IS", with no express or implied warranties of any kind,
 * including, but not limited to, any implied warranties of merchantability
 * or fitness for any particular purpose or warranties against infringement
 * of any proprietary rights of a third party.
 *
 * Silicon Labs will not be liable for any consequential, incidental, or
 * special damages, or any other relief, or for any claim by any third party,
 * arising from your use of this Software.
 *
 ******************************************************************************/
/** @addtogroup cbhc_serial
 * @brief Common bootloader serial definitions.
 *
 * See bootloader-serial.h for source code.
 *@{
 */

#ifndef __BOOTLOADER_SERIAL_H__
#define __BOOTLOADER_SERIAL_H__


#ifndef MSD_BOOTLOADER
/** @brief Initialize serial port.
 */
void serInit(void);

/** @brief Flush the transmiter.
 */
void serPutFlush(void);
/** @brief Transmit a character.
 * @param ch A character.
 */
void serPutChar(uint8_t ch);
/** @brief  Transmit a string.
 * @param str A string.
 */
void serPutStr(const char *str);
/** @brief Transmit a buffer.
 * @param buf A buffer.
 * @param size Length of buffer.
 */
void serPutBuf(const uint8_t buf[], uint8_t size);
/** @brief Transmit a 16bit value in decimal.
 * @param val The data to print.
 */
void serPutDecimal(uint16_t val);
/** @brief Transmit a byte as hex.
 * @param byte A byte.
 */
void serPutHex(uint8_t byte);
/** @brief  Transmit a 16bit integer as hex.
 * @param word A 16bit integer.
 */
void serPutHexInt(uint16_t word);

/** @brief Determine if a character is available.
 * @return true if a character is available, false otherwise.
 */
bool serCharAvailable(void);
/** @brief Get a character if available, otherwise return an error.
 * @param ch Pointer to a location where the received byte will be placed.
 * @return ::BL_SUCCESS if a character was obtained, ::BL_ERR otherwise.
 */
uint8_t   serGetChar(uint8_t* ch);

/** @brief Flush the receiver.
 */
void    serGetFlush(void);

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#ifdef BTL_HAS_EZSP_SPI
extern uint8_t preSpiComBufIndex;
#define serResetCharInput() preSpiComBufIndex = 0;
#endif
#endif

#else //MSD_BOOTLOADER

// clear serial calls
#define serInit(x) {}
#define serPutFlush(x) 
#define serPutChar(x)
#define serPutStr(x)
#define serPutBuf(x)
#define serPutDecimal(x)
#define serPutHex(x) x
#define serPutHexInt(x)
#define serCharAvailable(x)
#define serGetChar(x)
#define serGetFlush(x)

#endif //MSD_BOOTLOADER

#endif //__BOOTLOADER_SERIAL_H__

/**@} end of group*/

