/***************************************************************************//**
 * @file
 * @brief Provide stdio retargeting to USART/UART or LEUART.
 * @version 5.1.2
 *******************************************************************************
 * @section License
 * <b>Copyright 2015 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include <stdio.h>
#include "em_device.h"
#include "em_cmu.h"
#include "em_core.h"
#include "em_gpio.h"
#include "em_usart.h"
#include "bsp.h"
#include "bsp-uart.h"

/***************************************************************************//**
 * @addtogroup Drivers
 * @{
 ******************************************************************************/


/* Receive buffer */
#define RXBUFSIZE    8
static volatile int     rxReadIndex  = 0;
static volatile int     rxWriteIndex = 0;
static volatile int     rxCount      = 0;
static volatile uint8_t rxBuffer[RXBUFSIZE];
static uint8_t          LFtoCRLF    = 0;
static bool             initialized = false;

/**************************************************************************//**
 * @brief UART/LEUART IRQ Handler
 *****************************************************************************/
void UART1_RX_IRQHandler(void)
{
  if (UART1->STATUS & USART_STATUS_RXDATAV)
  {
    /* Store Data */
    rxBuffer[rxWriteIndex] = USART_Rx(UART1);
    rxWriteIndex++;
    rxCount++;
    if (rxWriteIndex == RXBUFSIZE)
    {
      rxWriteIndex = 0;
    }
    /* Check for overflow - flush buffer */
    if (rxCount > RXBUFSIZE)
    {
      rxWriteIndex = 0;
      rxCount      = 0;
      rxReadIndex  = 0;
    }
  }
}


/**************************************************************************//**
 * @brief UART/LEUART toggle LF to CRLF conversion
 * @param on If non-zero, automatic LF to CRLF conversion will be enabled
 *****************************************************************************/
void UART1_SerialCrLf(int on)
{
  if (on)
    LFtoCRLF = 1;
  else
    LFtoCRLF = 0;
}


/**************************************************************************//**
 * @brief Intializes UART/LEUART
 *****************************************************************************/
void UART1_SerialInit(void)
{
  /* Configure GPIO pins */
  CMU_ClockEnable(cmuClock_GPIO, true);
  /* To avoid false start, configure output as high */
  GPIO_PinModeSet(gpioPortB, 9, gpioModePushPull, 1);
  GPIO_PinModeSet(gpioPortB, 10, gpioModeInput, 0);

  USART_TypeDef           *usart = UART1;
  USART_InitAsync_TypeDef init   = USART_INITASYNC_DEFAULT;

  /* Enable EFM32WG_DK3850 RS232/UART switch */
  BSP_PeripheralAccess(BSP_RS232_UART, true);

  /* Enable peripheral clocks */
  CMU_ClockEnable(cmuClock_HFPER, true);
  CMU_ClockEnable(cmuClock_UART1, true);

  /* Configure USART for basic async operation */
  init.enable = usartDisable;
  USART_InitAsync(usart, &init);

  /* Enable pins at UART1 location #2 */
  usart->ROUTE = USART_ROUTE_RXPEN | USART_ROUTE_TXPEN | USART_ROUTE_LOCATION_LOC2;

  /* Clear previous RX interrupts */
  USART_IntClear(UART1, USART_IF_RXDATAV);
  NVIC_ClearPendingIRQ(UART1_RX_IRQn);

  /* Enable RX interrupts */
  USART_IntEnable(UART1, USART_IF_RXDATAV);
  NVIC_EnableIRQ(UART1_RX_IRQn);

  /* Finally enable it */
  USART_Enable(usart, usartEnable);

#if !defined(__CROSSWORKS_ARM) && defined(__GNUC__)
  setvbuf(stdout, NULL, _IONBF, 0);   /*Set unbuffered mode for stdout (newlib)*/
#endif

  initialized = true;
}


/**************************************************************************//**
 * @brief Receive a byte from USART/LEUART and put into global buffer
 * @return -1 on failure, or positive character integer on sucesss
 *****************************************************************************/
int UART1_ReadChar(void)
{
  int c = -1;
  CORE_DECLARE_IRQ_STATE;

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
 * @param data Character to transmit
 *****************************************************************************/
int UART1WriteChar(char c)
{
  if (initialized == false)
  {
    UART1_SerialInit();
  }

  /* Add CR or LF to CRLF if enabled */
  if (LFtoCRLF && (c == '\n'))
  {
    USART_Tx(UART1, '\r');
  }
  USART_Tx(UART1, c);

  if (LFtoCRLF && (c == '\r'))
  {
    USART_Tx(UART1, '\n');
  }

  return c;
}

/** @} (end group Drivers) */
