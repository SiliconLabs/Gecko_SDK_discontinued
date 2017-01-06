/**************************************************************************//**
 * Copyright 2016 by Silicon Laboratories Inc. All rights reserved.
 *
 * http://developer.silabs.com/legal/version/v11/Silicon_Labs_Software_License_Agreement.txt
 *****************************************************************************/


#include <stdio.h>
#include "em_device.h"
#include "em_cmu.h"
#include "em_core.h"
#include "em_gpio.h"
#include "retargetvirtual.h"

/***************************************************************************//**
 * @addtogroup RetargetIo
 * @{
 ******************************************************************************/

#if defined(RETARGET_USART)
#include "em_usart.h"
#endif

#if defined(RETARGET_LEUART)
#include "em_leuart.h"
#endif

// Receive buffer
#define RXBUFSIZE    8                          /**< Buffer size for RX */
static volatile int     rxReadIndex  = 0;       /**< Index in buffer to be read */
static volatile int     rxWriteIndex = 0;       /**< Index in buffer to be written to */
static volatile int     rxCount      = 0;       /**< Keeps track of how much data which are stored in the buffer */
static volatile uint8_t rxBuffer[RXBUFSIZE];    /**< Buffer to store data */
static uint8_t          LFtoCRLF    = 0;        /**< LF to CRLF conversion disabled */
static bool             initialized = false;    /**< Initialize UART/LEUART */

/**************************************************************************//**
 * @brief UART/LEUART IRQ Handler
 *****************************************************************************/
void RETARGET_IRQ_NAME(void)
{
#if defined(RETARGET_USART)
  if (RETARGET_UART->STATUS & USART_STATUS_RXDATAV)
  {
#else
  if (RETARGET_UART->IF & LEUART_IF_RXDATAV)
  {
#endif

    // Store Data
    rxBuffer[rxWriteIndex] = RETARGET_RX(RETARGET_UART);
    rxWriteIndex++;
    rxCount++;
    if (rxWriteIndex == RXBUFSIZE)
    {
      rxWriteIndex = 0;
    }
    // Check for overflow - flush buffer
    if (rxCount > RXBUFSIZE)
    {
      rxWriteIndex = 0;
      rxCount      = 0;
      rxReadIndex  = 0;
    }
  }
}

/** @} (end group RetargetIo) */

/**************************************************************************//**
 * @brief UART/LEUART toggle LF to CRLF conversion
 * @param on If non-zero, automatic LF to CRLF conversion will be enabled
 *****************************************************************************/
void RETARGET_SerialCrLf(int on)
{
  if (on)
    LFtoCRLF = 1;
  else
    LFtoCRLF = 0;
}

void RETARGET_BlockOnTX()
{
#if defined(RETARGET_USART)
	while (!(RETARGET_UART->STATUS & USART_STATUS_TXC))
		;
#else
	while(!(RETARGET_UART->STATUS & LEUART_STATUS_TXC));

#endif

}

/**************************************************************************//**
 * @brief Intializes UART/LEUART
 *****************************************************************************/
void RETARGET_SerialInit(void)
{
  // Configure GPIO pins
  CMU_ClockEnable(cmuClock_GPIO, true);
  // To avoid false start, configure output as high
  GPIO_PinModeSet(RETARGET_TXPORT, RETARGET_TXPIN, gpioModePushPull, 1);
  GPIO_PinModeSet(RETARGET_RXPORT, RETARGET_RXPIN, gpioModeInput, 0);
  // Enable STK virtual COM part
  GPIO_PinModeSet(RETARGET_ENPORT, RETARGET_ENPIN, gpioModePushPull, 1);

#if defined(RETARGET_USART)
  USART_TypeDef           *usart = RETARGET_UART;
  USART_InitAsync_TypeDef init   = USART_INITASYNC_DEFAULT;

  // Enable peripheral clocks
  CMU_ClockEnable(cmuClock_HFPER, true);
  CMU_ClockEnable(RETARGET_CLK, true);

  // Configure USART for basic async operation
  init.enable = usartDisable;
  USART_InitAsync(usart, &init);

  // Enable pins at correct UART/USART location.
  #if defined( USART_ROUTEPEN_RXPEN )
  usart->ROUTEPEN = USART_ROUTEPEN_RXPEN | USART_ROUTEPEN_TXPEN;
  usart->ROUTELOC0 = ( usart->ROUTELOC0 &
                       ~( _USART_ROUTELOC0_TXLOC_MASK
                          | _USART_ROUTELOC0_RXLOC_MASK ) )
                     | ( RETARGET_TX_LOCATION << _USART_ROUTELOC0_TXLOC_SHIFT )
                     | ( RETARGET_RX_LOCATION << _USART_ROUTELOC0_RXLOC_SHIFT );
  #else
  usart->ROUTE = USART_ROUTE_RXPEN | USART_ROUTE_TXPEN | RETARGET_LOCATION;
  #endif

  // Clear previous RX interrupts
  USART_IntClear(RETARGET_UART, USART_IF_RXDATAV);
  NVIC_ClearPendingIRQ(RETARGET_IRQn);

  // Enable RX interrupts
  USART_IntEnable(RETARGET_UART, USART_IF_RXDATAV);
  NVIC_EnableIRQ(RETARGET_IRQn);

  // Finally enable it
  USART_Enable(usart, usartEnable);

#else
  LEUART_TypeDef      *leuart = RETARGET_UART;
  LEUART_Init_TypeDef init    = LEUART_INIT_DEFAULT;

  // Enable CORE LE clock in order to access LE modules
  CMU_ClockEnable(cmuClock_CORELE, true);

  // Select HFLE/2 for LEUARTs
  CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_CORELEDIV2);

  CMU_ClockEnable(RETARGET_CLK, true);

  // Do not prescale clock
  CMU_ClockDivSet(RETARGET_CLK, cmuClkDiv_1);

  // Configure LEUART
  init.enable = leuartDisable;
  // Virtual COM port fix at 115200
  init.baudrate = 115200;
  LEUART_Init(leuart, &init);
  // Enable pins at default location
  leuart->ROUTE = LEUART_ROUTE_RXPEN | LEUART_ROUTE_TXPEN | RETARGET_LOCATION;

  // Clear previous RX interrupts
  LEUART_IntClear(RETARGET_UART, LEUART_IF_RXDATAV);
  NVIC_ClearPendingIRQ(RETARGET_IRQn);

  // Enable RX interrupts
  LEUART_IntEnable(RETARGET_UART, LEUART_IF_RXDATAV);
  NVIC_EnableIRQ(RETARGET_IRQn);

  // Finally enable it
  LEUART_Enable(leuart, leuartEnable);
#endif

#if !defined(__CROSSWORKS_ARM) && defined(__GNUC__)
  // Set unbuffered mode for stdout (newlib)
  setvbuf(stdout, NULL, _IONBF, 0);
#endif

  initialized = true;
}


/**************************************************************************//**
 * @brief Receive a byte from USART/LEUART and put into global buffer
 * @return -1 on failure, or positive character integer on sucesss
 *****************************************************************************/
int RETARGET_ReadChar(void)
{
  int c = -1;
  CORE_DECLARE_IRQ_STATE;

  if (initialized == false)
  {
    RETARGET_SerialInit();
  }

  CORE_ENTER_ATOMIC();
  if (rxCount > 0)
  {
    c = rxBuffer[rxReadIndex];
    rxReadIndex++;
    if (rxReadIndex == RXBUFSIZE)
    {
      rxReadIndex = 0;
    }
    rxCount--;
  }
  CORE_EXIT_ATOMIC();

  return c;
}


/**************************************************************************//**
 * @brief Transmit single byte to USART/LEUART
 * @param c Character to transmit
 * @return Transmitted character
 *****************************************************************************/
int RETARGET_WriteChar(char c)
{
  if (initialized == false)
  {
    RETARGET_SerialInit();
  }

  // Add CR or LF to CRLF if enabled
  if (LFtoCRLF && (c == '\n'))
  {
    RETARGET_TX(RETARGET_UART, '\r');
  }
  RETARGET_TX(RETARGET_UART, c);

  if (LFtoCRLF && (c == '\r'))
  {
    RETARGET_TX(RETARGET_UART, '\n');
  }

  return c;
}
