/***************************************************************************//**
 * @file bootloader-uart.c
 * @brief Bootloader serial interface functions for a uart
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
#include PLATFORM_HEADER  // should be iar.h
#include "bootloader-common.h"
#include "bootloader-serial.h"
#include "stack/include/ember-types.h"
#include "hal.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_usart.h"
#ifdef COM_RETARGET_SERIAL
  #include "retargetserial.h"
  #define BTL_UART RETARGET_UART
  #define BTL_UART_CLK RETARGET_CLK

  #ifdef _USART_ROUTELOC0_MASK
    #define UART_TX_LOCATION RETARGET_TX_LOCATION
    #define UART_RX_LOCATION RETARGET_RX_LOCATION
  #else
    #define UART_ROUTE_LOCATION RETARGET_LOCATION
  #endif

  #define UART_TX_PORT RETARGET_TXPORT
  #define UART_TX_PIN  RETARGET_TXPIN
  #define UART_RX_PORT RETARGET_RXPORT
  #define UART_RX_PIN  RETARGET_RXPIN
#endif /*COM_RETARGET_SERIAL*/

#ifndef BTL_UART
  #define BTL_UART USART0
#endif
#ifndef BTL_UART_CLK
  #define BTL_UART_CLK cmuClock_USART0
#endif
#ifdef _USART_ROUTELOC0_MASK
  #ifndef UART_TX_LOCATION
    #define UART_TX_LOCATION _USART_ROUTELOC0_TXLOC_LOC0
  #endif
  #ifndef UART_RX_LOCATION
    #define UART_RX_LOCATION _USART_ROUTELOC0_RXLOC_LOC0
  #endif 
#else //defined( _USART_ROUTELOC0_MASK )
  #ifndef UART_ROUTE_LOCATION
    #define UART_ROUTE_LOCATION _USART_ROUTE_LOCATION_LOC1
  #endif
#endif

#ifndef UART_TX_PORT
  #define UART_TX_PORT gpioPortA
#endif
#ifndef UART_TX_PIN
  #define UART_TX_PIN 0
#endif
#ifndef UART_RX_PORT
  #define UART_RX_PORT gpioPortA
#endif
#ifndef UART_RX_PIN
  #define UART_RX_PIN 1
#endif

// Function Name: serInit
// Description:   This function configures the UART. These are default
//                settings and can be modified for custom applications.
// Parameters:    none
// Returns:       none
//
bool initState;
void serInit(void)
{
  if (initState)
  {
    return;
  }
    /* Configure GPIO pins */
  CMU_ClockEnable(cmuClock_GPIO, true);
  /* To avoid false start, configure output as high */
  GPIO_PinModeSet(UART_TX_PORT, UART_TX_PIN, gpioModePushPull, 1);
  GPIO_PinModeSet(UART_RX_PORT, UART_RX_PIN, gpioModeInput, 0);

  USART_TypeDef           *usart = BTL_UART;
  USART_InitAsync_TypeDef init   = USART_INITASYNC_DEFAULT;

  /* Enable peripheral clocks */
  CMU_ClockEnable(cmuClock_HFPER, true);
  CMU_ClockEnable(BTL_UART_CLK, true);

  /* Configure USART for basic async operation */
  init.enable = usartDisable;
  USART_InitAsync(usart, &init);

  /* Enable pins at correct UART/USART location. */
  #if defined( USART_ROUTEPEN_RXPEN )
  usart->ROUTEPEN = USART_ROUTEPEN_RXPEN | USART_ROUTEPEN_TXPEN;
  usart->ROUTELOC0 = ( usart->ROUTELOC0 &
                       ~( _USART_ROUTELOC0_TXLOC_MASK
                          | _USART_ROUTELOC0_RXLOC_MASK ) )
                     | ( UART_TX_LOCATION << _USART_ROUTELOC0_TXLOC_SHIFT )
                     | ( UART_RX_LOCATION << _USART_ROUTELOC0_RXLOC_SHIFT );
  #else
  usart->ROUTE = USART_ROUTE_RXPEN | USART_ROUTE_TXPEN | UART_ROUTE_LOCATION;
  #endif
  /* Finally enable it */
  USART_Enable(usart, usartEnable);

  #ifndef ENABLE_EXP_UART
    halEnableVCOM();
  #endif
  initState = true;
}


void serPutFlush(void)
{
#ifdef _USART_STATUS_TXIDLE_MASK
  /*wait for txidle*/
  while ( !(USART_StatusGet(BTL_UART) & _USART_STATUS_TXIDLE_MASK) ) ;
#else
  /*wait for tx buffer to clear*/
  while ( (USART_StatusGet(BTL_UART) & _USART_STATUS_TXBL_SHIFT) ) ;
#endif
}

// wait for transmit free then send char
void serPutChar(uint8_t ch)
{
  USART_Tx(BTL_UART,ch);
}

void serPutStr(const char *str)
{
  while (*str) {
    serPutChar(*str);
    str++;
  }
}

void serPutBuf(const uint8_t buf[], uint8_t size)
{
  uint16_t i;

  for (i=0; i<size; i++) {
    serPutChar(buf[i]);
  }
}

void serPutDecimal(uint16_t val)
{
  char outStr[] = {'0','0','0','0','0','\0'};
  int8_t i = sizeof(outStr)/sizeof(char) - 1;
  uint8_t remainder, lastDigit = i-1;

  // Convert the integer into a string.
  while(--i >= 0) {
    remainder = val%10;
    val /= 10;
    outStr[i] = remainder + '0';
    if(remainder != 0) {
      lastDigit = i;
    }
  }

  // Print the final string
  serPutStr(outStr+lastDigit);
}

void serPutHex(uint8_t byte)
{
  uint8_t val;
  val = ((byte & 0xF0) >> 4);
  serPutChar((val>9)?(val-10+'A'):(val+'0') );
  val = (byte & 0x0F);
  serPutChar((val>9)?(val-10+'A'):(val+'0') );
}

void serPutHexInt(uint16_t word)
{
  serPutHex(HIGH_BYTE(word));
  serPutHex(LOW_BYTE(word));
}

bool serCharAvailable(void)
{
  if(USART_StatusGet(BTL_UART) & _USART_STATUS_RXDATAV_MASK) {
    return true;
  } else {
    return false;
  }
}

// get char if available, else return error
BL_Status serGetChar(uint8_t* ch)
{
  if(serCharAvailable()) {
    *ch = USART_Rx(BTL_UART);
    return BL_SUCCESS;
  } else {
    return BL_ERR;
  }
}

// get chars until rx buffer is empty
void serGetFlush(void)
{
  uint8_t status = BL_SUCCESS;
  uint8_t tmp;
  do {
    status = serGetChar(&tmp);
  } while(status == BL_SUCCESS);
}
