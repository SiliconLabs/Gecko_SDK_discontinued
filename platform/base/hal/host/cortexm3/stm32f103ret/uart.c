/** @file hal/host/cortexm3/stm32f103ret/uart.c
 * @brief UART driver intended to work with IAR Standard library for
 * serial output as well as satisfy some components of Ember's serial library.
 *
 * NOTE: This driver is blocking!
 *
 * <!-- Copyright 2010 by Ember Corporation. All rights reserved.        *80*-->
 */

#include PLATFORM_HEADER
#include "stack/include/error.h"
#include "hal/hal.h"


//This state variable is used by halInternalForcePrintf() to enable
//forced writes that do not use interrupts and do not use the queue.
//This is so printf can behave like halInternalForceWriteUartData() and
//makes emberSerialGuaranteedPrintf() work without interrupts.
bool forcedWrite = false;

#define RECEIVE_QUEUE_SIZE  (128)
#define TRANSMIT_QUEUE_SIZE (128)

uint8_t rxQ[RECEIVE_QUEUE_SIZE];
uint16_t rxHead;
uint16_t rxTail;
uint16_t rxUsed;

uint8_t txQ[TRANSMIT_QUEUE_SIZE];
uint16_t txHead;
uint16_t txTail;
uint16_t txUsed;

//Connect the COM port to USART1.  While all of these definitions are not
//strictly necessary, they are useful for retargeting if a different
//serial port is desired.
#define COM_USART      USART1
#define COM_TX_PORT    GPIOA
#define COM_RX_PORT    GPIOA
#define COM_TX_PIN     GPIO_Pin_9
#define COM_RX_PIN     GPIO_Pin_10
#define COM_IRQn       USART1_IRQn
#define COM_IRQHandler USART1_IRQHandler

//////////////////////////////////////////////////////////////////////////////
// Initialization

static void uartInit(uint32_t baudRate,
                     uint16_t wordLength,
                     uint16_t parity,
                     uint16_t stopBits)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  USART_InitTypeDef USART_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  
  rxHead=0;
  rxTail=0;
  rxUsed=0;
  
  txHead=0;
  txTail=0;
  txUsed=0;
  
  
  //Configure USART TX GPIO as alternate function push-pull
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Pin = COM_TX_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(COM_TX_PORT, &GPIO_InitStructure);
  
  //Configure USART RX GPIO as input floating 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Pin = COM_RX_PIN;
  GPIO_Init(COM_RX_PORT, &GPIO_InitStructure);
  
  //Configure USART (but without enabling it)
  USART_InitStructure.USART_BaudRate = baudRate;
  USART_InitStructure.USART_WordLength = wordLength;
  USART_InitStructure.USART_Parity = parity;
  USART_InitStructure.USART_StopBits = stopBits;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(COM_USART, &USART_InitStructure);
  
  //Enable USART 
  USART_Cmd(COM_USART, ENABLE);
  
  //Enable USART (second level) interrupts
  //  TX DR empty will be enabled as needed
  //USART_ITConfig(COM_USART, USART_IT_TXE, ENABLE);
  //  TX complete will be enabled as needed
  //USART_ITConfig(COM_USART, USART_IT_TC, ENABLE);
  //  RX DR not empty
  USART_ITConfig(COM_USART, USART_IT_RXNE, ENABLE);
  //  frame, noise, or overrun receive error
  //    IAR standard library doesn't allow for any error handling in the
  //    case of rx errors so the errors are just silently dropped.
  //USART_ITConfig(COM_USART, USART_IT_ERR, ENABLE);
  
  //Enable USART (top level) NVIC interrupt and configure the priority
  NVIC_InitStructure.NVIC_IRQChannel = COM_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

EmberStatus halInternalUartInit(uint8_t port,
                                SerialBaudRate emberRate,
                                SerialParity emberParity,
                                uint8_t emberStopbits)
{
  //Translate from Ember's UART API to the host's UART API (which
  //uses different variable types for the UART parameters).
  uint32_t uartBaud;
  uint16_t uartParity;
  uint16_t uartStopbits;
  
  switch(emberRate) {
  case BAUD_19200:
    uartBaud = 19200;
    break;
  case BAUD_38400:
    uartBaud = 38400;
    break;
  case BAUD_57600:
    uartBaud = 57600;
    break;
  case BAUD_115200:
    uartBaud = 115200;
    break;
  default:
    return EMBER_SERIAL_INVALID_BAUD_RATE;
  };
  
  switch(emberParity) {
  case PARITY_EVEN:
    uartParity = USART_Parity_Even;
    break;
  case PARITY_ODD:
    uartParity = USART_Parity_Odd;
    break;
  case PARITY_NONE:
  default:
    uartParity = USART_Parity_No;
    break;
  };
  
  switch(emberStopbits) {
  case 2:
    uartStopbits = USART_StopBits_2;
    break;
  case 1:
  default:
    uartStopbits = USART_StopBits_1;
    break;
  };
  
  uartInit(uartBaud, USART_WordLength_8b, uartParity, uartStopbits);
  return EMBER_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
// Transmit

// IAR Standard library hook for serial output
size_t __write(int handle, const unsigned char * buffer, size_t size)
{
  size_t nChars = 0;
  
  /* This template only writes to "standard out" and "standard err",
   * for all other file handles it returns failure. */
  if (handle != _LLIO_STDOUT && handle != _LLIO_STDERR) {
    return _LLIO_ERROR;
  }
  
  if(forcedWrite) {
    //This is a halInternalForceWriteUartData replacement for printf.
    size_t i;
    for(i=0;i<size;i++) {
      while(USART_GetFlagStatus(COM_USART, USART_FLAG_TXE) == RESET) {}
      USART_SendData(COM_USART, (uint8_t)buffer[i]);
      nChars++;
    }
    while(USART_GetFlagStatus(COM_USART, USART_FLAG_TC) == RESET) {}
    return nChars;
  }
  
  if (buffer == 0) {
    //This means that we should flush internal buffers.
    //Spin until the TX Q is empty, the USART DR is empty, and the
    //serializer has completed.
    while((txUsed) ||
          (USART_GetFlagStatus(COM_USART, USART_FLAG_TXE) == RESET) ||
          (USART_GetFlagStatus(COM_USART, USART_FLAG_TC) == RESET)) {
      USART_ITConfig(COM_USART, USART_IT_TXE, ENABLE);
    }
    return 0;
  }
  
  //NOTE: If printing is performed from ISR/ATOMIC context, it's possible
  //      to get stuck here since this library driver is designed to
  //      be blocking.
  while(size) {
    //Add a byte onto the TX Q if there is room.  If no room, keep trying
    //until there is room to add a byte.
    if(txUsed<(TRANSMIT_QUEUE_SIZE-1)) {
      ATOMIC(
        txQ[txHead] = (uint8_t)(*buffer);
        txHead = (txHead+1) % TRANSMIT_QUEUE_SIZE;
        txUsed++;
        
        buffer++;
        size--;
        ++nChars;
      )
    }
    //Let the USART ISR begin sending data from the Q
    USART_ITConfig(COM_USART, USART_IT_TXE, ENABLE);
  }
  
  return nChars;
}

size_t fflush(int handle)
{
  return __write(_LLIO_STDOUT, NULL, 0);
}

void halInternalForcePrintf(bool onOff)
{
  forcedWrite = onOff;
}

//////////////////////////////////////////////////////////////////////////////
// Receive

// IAR Standard library hook for serial input
size_t __read(int handle, unsigned char * buffer, size_t size)
{
  int nChars = 0;
  
  /* This template only reads from "standard in", for all other file
   * handles it returns failure. */
  if (handle != _LLIO_STDIN) {
    return _LLIO_ERROR;
  }
  
  for(nChars = 0; (rxUsed>0) && (nChars < size); nChars++) {
    ATOMIC(
      *buffer++ = rxQ[rxTail];
      rxTail = (rxTail+1) % RECEIVE_QUEUE_SIZE;
      rxUsed--;
    )
  }
  
  return nChars;
}

uint16_t halInternalPrintfWriteAvailable()
{
  return ((TRANSMIT_QUEUE_SIZE-1)-txUsed);
}

uint16_t halInternalPrintfReadAvailable()
{
  return rxUsed;
}

//////////////////////////////////////////////////////////////////////////////
// Interrupts

void COM_IRQHandler(void)
{
  //RECEIVE INTERRUPT
  if(USART_GetITStatus(COM_USART, USART_IT_RXNE) == SET) {
    uint8_t incoming = USART_ReceiveData(COM_USART)&0xFF;
    //IAR standard library doesn't allow for any error handling in the
    //case of not having space in the receive queue, so the
    //error is just silently dropped.
    if(rxUsed < (RECEIVE_QUEUE_SIZE-1)) {
      rxQ[rxHead] = incoming;
      rxHead = (rxHead+1) % RECEIVE_QUEUE_SIZE;
      rxUsed++;
    }
  }
  //IAR standard library doesn't allow for any error handling in the
  //case of rx errors so the errors are just silently dropped.
  //if(USART_GetITStatus(COM_USART, USART_IT_FE) == SET) {}
  //if(USART_GetITStatus(COM_USART, USART_IT_NE) == SET) {}
  //if(USART_GetITStatus(COM_USART, USART_IT_ORE) == SET) {}
  
  
  //TRANSMIT INTERRUPT
  if(USART_GetITStatus(COM_USART, USART_IT_TXE) == SET) {
    if(txUsed) {
      //TXE means there is room in the hardware so pull a byte from the Q
      USART_SendData(COM_USART, (uint8_t)(txQ[txTail]));
      txTail = (txTail+1) % TRANSMIT_QUEUE_SIZE;
      txUsed--;
    } else {
      //There is no more data in the Q, so disable the ISR
      USART_ITConfig(COM_USART, USART_IT_TXE, DISABLE);
    }
  }
  //TXE is responsible for feeding data from the Q into the hardware
  //so TC shouldn't be necessary as an interrupt.
  //if(USART_GetITStatus(COM_USART, USART_IT_TC) == SET) {}
}

