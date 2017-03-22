/***************************************************************************//**
 * @file serial.c
 * @brief Serial Layer, legacy support
 *******************************************************************************
 * @section License
 * <b>(C) Copyright 2014 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *

 ******************************************************************************/

#include PLATFORM_HEADER
#include "stack/include/ember-types.h"
#include "stack/include/error.h"

//Host processors do not use Ember Message Buffers.
#ifndef EZSP_HOST
  #include "stack/include/packet-buffer.h"
#endif

#include "hal/hal.h"
#include "serial.h"
#ifdef CORTEXM3_EFM32_MICRO
  #include "com.h"
#endif
#include "ember-printf.h"

#include <stdarg.h>

#ifdef EMBER_SERIAL_USE_STDIO
#include <stdio.h>
#endif //EMBER_SERIAL_USE_STDIO

#ifdef EMBER_SERIAL_CUSTOM_STDIO
#include EMBER_SERIAL_CUSTOM_STDIO
#define EMBER_SERIAL_USE_STDIO
#endif // EMBER_SERIAL_CUSTOM_STDIO

// AppBuilder and Afv2 will define the characteristics of the Serial ports here.
#if defined(ZA_GENERATED_HEADER)
  #include ZA_GENERATED_HEADER
#endif

// Crude method of mashing together com and serial layers before all the drivers
// are properly ported over into ember world. NT 2014-09-16
#ifdef CORTEXM3_EFM32_MICRO
//------------------------------------------------------
// Serial initialization

EmberStatus emberSerialInit(uint8_t port, 
                                          SerialBaudRate rate,
                                          SerialParity parity,
                                          uint8_t stopBits)
{
  EmberStatus status = EMBER_ERR_FATAL;
  COM_Init_t initdata;
  switch (port) {
#ifdef COM_VCP_ENABLE
    case COM_VCP:
    status = COM_Init((COM_Port_t) port, NULL);
    break;
#endif
#ifdef COM_USART0_ENABLE
    case COM_USART0:
      initdata = (COM_Init_t) COM_USART0_DEFAULT;
      break;
#endif
#ifdef COM_USART1_ENABLE
    case COM_USART1:
      initdata = (COM_Init_t) COM_USART1_DEFAULT;
      break;
#endif
#ifdef COM_USART2_ENABLE
    case COM_USART2:
      initdata = (COM_Init_t) COM_USART2_DEFAULT;
      break;
#endif
#ifdef COM_LEUART0_ENABLE
    case COM_LEUART0:
      initdata = (COM_Init_t) COM_LEUART0_DEFAULT;
      break;
#endif
#ifdef COM_LEUART1_ENABLE
    case COM_LEUART1:
      initdata = (COM_Init_t) COM_LEUART1_DEFAULT;
      break;
#endif
    default:
      return status;
  }
#if defined(COM_USART0_ENABLE) || defined (COM_USART1_ENABLE) || defined (COM_USART2_ENABLE)
  if ((port==COM_USART0) || (port==COM_USART1) || (port==COM_USART2))
  {
    initdata.uartdrvinit.uartinit.baudRate=rate;
    initdata.uartdrvinit.uartinit.parity=(USART_Parity_TypeDef)parity;
    if (stopBits==1)
    {
      initdata.uartdrvinit.uartinit.stopBits=usartStopbits1;
    }
    else if (stopBits==2)
    {
      initdata.uartdrvinit.uartinit.stopBits=usartStopbits2;
    }
    status = COM_Init((COM_Port_t) port, &initdata);
  }
#endif
#if defined (COM_LEUART0_ENABLE) || defined (COM_LEUART1_ENABLE)
  if ((port==COM_LEUART0) || (port==COM_LEUART1))
  {
    initdata.uartdrvinit.leuartinit.baudRate=rate;
    initdata.uartdrvinit.leuartinit.parity=(LEUART_Parity_TypeDef)parity;
    if (stopBits==1)
    {
      initdata.uartdrvinit.leuartinit.stopBits=leuartStopbits1;
    }
    else if (stopBits==2)
    {
      initdata.uartdrvinit.leuartinit.stopBits=leuartStopbits2;
    }
    status = COM_Init((COM_Port_t) port, &initdata);
  }
#endif
  return status;
}


//------------------------------------------------------
// Serial Input

// returns # bytes available for reading
uint16_t emberSerialReadAvailable(uint8_t port)  
{
  return COM_ReadAvailable((COM_Port_t) port);
}

EmberStatus emberSerialReadByte(uint8_t port, uint8_t *dataByte)
{
  return COM_ReadByte((COM_Port_t) port, dataByte);
}

EmberStatus emberSerialReadData(uint8_t port,
                                uint8_t *data,
                                uint16_t length,
                                uint16_t *bytesRead)
{
  uint16_t bytesReadInternal = 0;
  EmberStatus status;

  while (bytesReadInternal < length) {
    status = emberSerialReadByte(port, data);

    switch (status) {
      case EMBER_SUCCESS:
        ++data;
        ++bytesReadInternal;
        break;

      case EMBER_SERIAL_RX_EMPTY:
        // empty queue is not an error for us, we just want to keep waiting
        break;

      default:
        // only store number of bytes read if the caller provided a non-NULL pointer
        if (bytesRead) {
          *bytesRead = bytesReadInternal;
        }
        return status;
    }
  }

  // only store number of bytes read if the caller provided a non-NULL pointer
  if (bytesRead) {
    *bytesRead = bytesReadInternal;
  }

  return EMBER_SUCCESS;

}

#ifndef EMBER_TEST
EmberStatus emberSerialReadDataTimeout(uint8_t port,
                                       uint8_t *data,
                                       uint16_t length,
                                       uint16_t *bytesRead,
                                       uint16_t firstByteTimeout,
                                       uint16_t subsequentByteTimeout)
{
  uint16_t bytesReadInternal = 0;
  EmberStatus status;
  uint16_t timeout = firstByteTimeout;
  uint16_t startTime = halCommonGetInt16uMillisecondTick();
  
  // loop until we read the max number of bytes or the timeout elapses
  while (bytesReadInternal < length
         && elapsedTimeInt16u(startTime,halCommonGetInt16uMillisecondTick()) < timeout) {
    status = emberSerialReadByte(port, data);

    switch (status) {
      case EMBER_SUCCESS:
        ++data;
        ++bytesReadInternal;
        // reset timer and timeout for next character
        startTime = halCommonGetInt16uMillisecondTick();
        timeout = subsequentByteTimeout;
        break;

      case EMBER_SERIAL_RX_EMPTY:
        // empty queue is not an error for us, we just want to keep waiting
        break;

      default:
        // only store number of bytes read if the caller provided a non-NULL pointer
        if (bytesRead) {
          *bytesRead = bytesReadInternal;
        }
        return status;
    }
  }

  // only store number of bytes read if the caller provided a non-NULL pointer
  if (bytesRead) {
    *bytesRead = bytesReadInternal;
  }

  return bytesReadInternal == length ? EMBER_SUCCESS : EMBER_SERIAL_RX_EMPTY;
}
#endif // EMBER_TEST

EmberStatus emberSerialReadPartialLine(uint8_t port, char *data, uint8_t max, uint8_t *index)
{
  return COM_ReadPartialLine((COM_Port_t) port, data, max, index);
}

EmberStatus emberSerialReadLine(uint8_t port, char *data, uint8_t max)
{
  uint8_t index=0;

  while(emberSerialReadPartialLine(port, data, max, &index) != EMBER_SUCCESS) {
    halResetWatchdog();
  }
  return EMBER_SUCCESS;
}

//------------------------------------------------------
// Serial Output

// returns # bytes (if fifo mode)/messages (if buffer mode) that can be written
uint16_t emberSerialWriteAvailable(uint8_t port)  
{
  return COM_WriteAvailable((COM_Port_t) port);
}

uint16_t emberSerialWriteUsed(uint8_t port)
{
  return COM_WriteUsed((COM_Port_t) port);
}

EmberStatus emberSerialWriteByte(uint8_t port, uint8_t dataByte)
{
  return COM_WriteByte((COM_Port_t) port, dataByte);
}

EmberStatus emberSerialWriteString(uint8_t port, PGM_P string)
{
  return COM_WriteString((COM_Port_t) port, string);
}

EmberStatus emberSerialWriteData(uint8_t port, uint8_t *data, uint8_t length)
{
  return COM_WriteData((COM_Port_t) port, data, length);
}

EmberStatus emberSerialWriteBuffer(uint8_t port, 
                                   EmberMessageBuffer buffer, 
                                   uint8_t start, 
                                   uint8_t length)
{
  for(;PACKET_BUFFER_SIZE <= start; start-=PACKET_BUFFER_SIZE)
    buffer = emberStackBufferLink(buffer);
  while (0 < length) {
    uint8_t remainingInBuffer = PACKET_BUFFER_SIZE - start;
    uint8_t bytes = (length < remainingInBuffer
                   ? length
                   : remainingInBuffer);
    emberSerialWriteData(port,
                         emberMessageBufferContents(buffer) + start,
                         bytes);
    length -= bytes;
    start = 0;
    buffer = emberStackBufferLink(buffer);
  }
  return EMBER_SUCCESS;
}


EmberStatus emberSerialWaitSend(uint8_t port)  // waits for all byte to be written out of a port
{
  return COM_WaitSend((COM_Port_t) port);
}

//------------------------------------------------------
// Guaranteed output

// The _only_ Guaranteed API:  The usage model for this api 
//   Does not require efficiency

#if !defined(EMBER_AF_PLUGIN_STANDARD_PRINTF_SUPPORT)

EmberStatus emberSerialGuaranteedPrintf(uint8_t port, PGM_P formatString, ...)
{
  va_list ap;
  va_start (ap, formatString);
  (void) emPrintfInternal(COM_ForceWriteData, (COM_Port_t) port, formatString, ap);
  va_end (ap);
  return EMBER_SUCCESS;
}

#endif

//------------------------------------------------------
// Serial buffer maintenance
void emberSerialFlushRx(uint8_t port) 
{
  COM_FlushRx((COM_Port_t) port);
}
void emberSerialBufferTick ( void ) { }


bool emberSerialUnused(uint8_t port)
{
#ifdef EMBER_SERIAL_USE_STDIO
  return false;
#else //EMBER_SERIAL_USE_STDIO
  return COM_Unused((COM_Port_t) port);
#endif //EMBER_SERIAL_USE_STDIO
}
#else //CORTEXM3_EFM32_MICRO

//Documentary comments:
// To conserve precious flash, there is very little validity checking
//  on the given parameters.  Be sure not to use an invalid port number
//  or a port that is unused.
// Blocking routines will always wait for room (but not buffers - if buffers
//  cannot be allocated, no part of the message will be sent)
// Non-blocking routines will never wait for room, and may cause partial
//  messages to be sent.  If a contiguous message needs to be sent, available
//  space should be checked _before_ calling the appropriate write API.

//------------------------------------------------------
// Determine if blocking code needs to be enabled
#if defined(EMBER_SERIAL0_BLOCKING) || \
    defined(EMBER_SERIAL1_BLOCKING) || \
    defined(EMBER_SERIAL2_BLOCKING) || \
    defined(EMBER_SERIAL3_BLOCKING) || \
    defined(EMBER_SERIAL4_BLOCKING)
  #define EM_ENABLE_SERIAL_BLOCKING
#endif
#ifdef EMBER_SERIAL0_BLOCKING
  #define EM_SERIAL0_BLOCKSTATE true
#else
  #define EM_SERIAL0_BLOCKSTATE false
#endif
#ifdef EMBER_SERIAL1_BLOCKING
  #define EM_SERIAL1_BLOCKSTATE true
#else
  #define EM_SERIAL1_BLOCKSTATE false
#endif
#ifdef EMBER_SERIAL2_BLOCKING
  #define EM_SERIAL2_BLOCKSTATE true
#else
  #define EM_SERIAL2_BLOCKSTATE false
#endif
#ifdef EMBER_SERIAL3_BLOCKING
  #define EM_SERIAL3_BLOCKSTATE true
#else
  #define EM_SERIAL3_BLOCKSTATE false
#endif
#ifdef EMBER_SERIAL4_BLOCKING
  #define EM_SERIAL4_BLOCKSTATE true
#else
  #define EM_SERIAL4_BLOCKSTATE false
#endif

#ifndef EMBER_SERIAL_USE_STDIO
//------------------------------------------------------
// Memory allocations for Queue data structures

//Macros to define fifo and buffer queues, can't use a typedef becuase the size
// of the fifo array in the queues can change
#define DEFINE_FIFO_QUEUE(qSize, qName)             \
  static struct {                                   \
    /*! Indexes of next byte to send*/              \
    uint16_t head;                                     \
    /*! Index of where to enqueue next message*/    \
    uint16_t tail;                                     \
    /*! Number of bytes queued*/                    \
    volatile uint16_t used;                            \
    /*! FIFO of queue data*/                        \
    uint8_t fifo[qSize];                              \
  } qName;

#define DEFINE_BUFFER_QUEUE(qSize, qName)           \
  static struct {                                   \
    /*! Indexes of next message to send*/           \
    uint8_t head;                                     \
    /*! Index of where to enqueue next message*/    \
    uint8_t tail;                                     \
    /*! Number of messages queued*/                 \
    volatile uint8_t used;                            \
    uint8_t dead;                                     \
    EmberMessageBuffer currentBuffer;               \
    uint8_t *nextByte, *lastByte;                     \
    /*! FIFO of messages*/                          \
    EmSerialBufferQueueEntry fifo[qSize];           \
  } qName;


// Allocate Appropriate TX Queue for port 0
#if EMBER_SERIAL0_MODE == EMBER_SERIAL_FIFO
  DEFINE_FIFO_QUEUE(EMBER_SERIAL0_TX_QUEUE_SIZE,emSerial0TxQueue)
  #define EM_SERIAL0_TX_QUEUE_ADDR (&emSerial0TxQueue)
#elif EMBER_SERIAL0_MODE == EMBER_SERIAL_BUFFER
  DEFINE_BUFFER_QUEUE(EMBER_SERIAL0_TX_QUEUE_SIZE,emSerial0TxQueue)
  #define EM_SERIAL0_TX_QUEUE_ADDR (&emSerial0TxQueue)
#elif EMBER_SERIAL0_MODE == EMBER_SERIAL_UNUSED || \
      EMBER_SERIAL0_MODE == EMBER_SERIAL_LOWLEVEL
  #define EM_SERIAL0_TX_QUEUE_ADDR (NULL)
  #define EMBER_SERIAL0_TX_QUEUE_SIZE 0
  #define EMBER_SERIAL0_RX_QUEUE_SIZE 0
#endif

// Allocate Appropriate TX Queue for port 1
#if EMBER_SERIAL1_MODE == EMBER_SERIAL_FIFO
  DEFINE_FIFO_QUEUE(EMBER_SERIAL1_TX_QUEUE_SIZE,emSerial1TxQueue)
  #define EM_SERIAL1_TX_QUEUE_ADDR (&emSerial1TxQueue)
#elif EMBER_SERIAL1_MODE == EMBER_SERIAL_BUFFER
  DEFINE_BUFFER_QUEUE(EMBER_SERIAL1_TX_QUEUE_SIZE,emSerial1TxQueue)
  #define EM_SERIAL1_TX_QUEUE_ADDR (&emSerial1TxQueue)
#elif EMBER_SERIAL1_MODE == EMBER_SERIAL_UNUSED || \
      EMBER_SERIAL1_MODE == EMBER_SERIAL_LOWLEVEL 
  #define EM_SERIAL1_TX_QUEUE_ADDR (NULL)
  #define EMBER_SERIAL1_TX_QUEUE_SIZE 0
  #define EMBER_SERIAL1_RX_QUEUE_SIZE 0
#endif

// Allocate Appropriate TX Queue for port 2
#if EMBER_SERIAL2_MODE == EMBER_SERIAL_FIFO
  DEFINE_FIFO_QUEUE(EMBER_SERIAL2_TX_QUEUE_SIZE,emSerial2TxQueue)
  #define EM_SERIAL2_TX_QUEUE_ADDR (&emSerial2TxQueue)
#elif EMBER_SERIAL2_MODE == EMBER_SERIAL_BUFFER
  DEFINE_BUFFER_QUEUE(EMBER_SERIAL2_TX_QUEUE_SIZE,emSerial2TxQueue)
  #define EM_SERIAL2_TX_QUEUE_ADDR (&emSerial2TxQueue)
#elif EMBER_SERIAL2_MODE == EMBER_SERIAL_UNUSED || \
      EMBER_SERIAL2_MODE == EMBER_SERIAL_LOWLEVEL 
  #define EM_SERIAL2_TX_QUEUE_ADDR (NULL)
  #define EMBER_SERIAL2_TX_QUEUE_SIZE 0
  #define EMBER_SERIAL2_RX_QUEUE_SIZE 0
#endif

// Allocate Appropriate TX Queue for port 3
#if EMBER_SERIAL3_MODE == EMBER_SERIAL_FIFO
  DEFINE_FIFO_QUEUE(EMBER_SERIAL3_TX_QUEUE_SIZE,emSerial3TxQueue)
  #define EM_SERIAL3_TX_QUEUE_ADDR (&emSerial3TxQueue)
#elif EMBER_SERIAL3_MODE == EMBER_SERIAL_BUFFER
  DEFINE_BUFFER_QUEUE(EMBER_SERIAL3_TX_QUEUE_SIZE,emSerial3TxQueue)
  #define EM_SERIAL3_TX_QUEUE_ADDR (&emSerial3TxQueue)
#elif EMBER_SERIAL3_MODE == EMBER_SERIAL_UNUSED || \
      EMBER_SERIAL3_MODE == EMBER_SERIAL_LOWLEVEL 
  #define EM_SERIAL3_TX_QUEUE_ADDR (NULL)
  #define EMBER_SERIAL3_TX_QUEUE_SIZE 0
  #define EMBER_SERIAL3_RX_QUEUE_SIZE 0
#endif

// Allocate Appropriate TX Queue for port 4
#if EMBER_SERIAL4_MODE == EMBER_SERIAL_FIFO
  DEFINE_FIFO_QUEUE(EMBER_SERIAL4_TX_QUEUE_SIZE,emSerial4TxQueue)
  #define EM_SERIAL4_TX_QUEUE_ADDR (&emSerial4TxQueue)
#elif EMBER_SERIAL4_MODE == EMBER_SERIAL_BUFFER
  DEFINE_BUFFER_QUEUE(EMBER_SERIAL4_TX_QUEUE_SIZE,emSerial4TxQueue)
  #define EM_SERIAL4_TX_QUEUE_ADDR (&emSerial4TxQueue)
#elif EMBER_SERIAL4_MODE == EMBER_SERIAL_UNUSED || \
      EMBER_SERIAL4_MODE == EMBER_SERIAL_LOWLEVEL 
  #define EM_SERIAL4_TX_QUEUE_ADDR (NULL)
  #define EMBER_SERIAL4_TX_QUEUE_SIZE 0
  #define EMBER_SERIAL4_RX_QUEUE_SIZE 0
#endif

// Allocate RX Queues (Always FIFOs)
#if EMBER_SERIAL0_MODE != EMBER_SERIAL_UNUSED
  DEFINE_FIFO_QUEUE(EMBER_SERIAL0_RX_QUEUE_SIZE,emSerial0RxQueue)
  #define EM_SERIAL0_RX_QUEUE_ADDR (&emSerial0RxQueue)
#else
  #define EM_SERIAL0_RX_QUEUE_ADDR (NULL)
#endif

#if EMBER_SERIAL1_MODE != EMBER_SERIAL_UNUSED
  DEFINE_FIFO_QUEUE(EMBER_SERIAL1_RX_QUEUE_SIZE,emSerial1RxQueue)
  #define EM_SERIAL1_RX_QUEUE_ADDR (&emSerial1RxQueue)
#else
  #define EM_SERIAL1_RX_QUEUE_ADDR (NULL)
  #define emSerial1RxQueue (NULL)
#endif

#if EMBER_SERIAL2_MODE != EMBER_SERIAL_UNUSED
  DEFINE_FIFO_QUEUE(EMBER_SERIAL2_RX_QUEUE_SIZE,emSerial2RxQueue)
  #define EM_SERIAL2_RX_QUEUE_ADDR (&emSerial2RxQueue)
#else
  #define EM_SERIAL2_RX_QUEUE_ADDR (NULL)
  #define emSerial2RxQueue (NULL)
#endif

#if EMBER_SERIAL3_MODE != EMBER_SERIAL_UNUSED
  DEFINE_FIFO_QUEUE(EMBER_SERIAL3_RX_QUEUE_SIZE,emSerial3RxQueue)
  #define EM_SERIAL3_RX_QUEUE_ADDR (&emSerial3RxQueue)
#else
  #define EM_SERIAL3_RX_QUEUE_ADDR (NULL)
  #define emSerial3RxQueue (NULL)
#endif

#if EMBER_SERIAL4_MODE != EMBER_SERIAL_UNUSED
  DEFINE_FIFO_QUEUE(EMBER_SERIAL4_RX_QUEUE_SIZE,emSerial4RxQueue)
  #define EM_SERIAL4_RX_QUEUE_ADDR (&emSerial4RxQueue)
#else
  #define EM_SERIAL4_RX_QUEUE_ADDR (NULL)
  #define emSerial4RxQueue (NULL)
#endif

//------------------------------------------------------
// Easy access to data structures for a particular port

// The FOR_EACH_PORT(CAST,PREFIX_,_SUFFIX) macro will expand in to something like:
//    CAST(PREFIX_0_SUFFIX),
//    CAST(PREFIX_1_SUFFIX)
// with a line & number for each port of EM_NUM_SERIAL_PORTS

// Data structure for referencing TX Queues
//  (allows for different modes and queue sizes)
void *emSerialTxQueues[EM_NUM_SERIAL_PORTS] = 
  { FOR_EACH_PORT( (void *),EM_SERIAL,_TX_QUEUE_ADDR ) };

uint16_t PGM emSerialTxQueueSizes[EM_NUM_SERIAL_PORTS] =
  { FOR_EACH_PORT( (uint16_t),EMBER_SERIAL,_TX_QUEUE_SIZE ) };

uint16_t PGM emSerialTxQueueMasks[EM_NUM_SERIAL_PORTS] =
  { FOR_EACH_PORT( (uint16_t),EMBER_SERIAL,_TX_QUEUE_SIZE-1 ) };

// Data structure for referencing RX Queues
//  (allows for different queue sizes)
EmSerialFifoQueue *emSerialRxQueues[EM_NUM_SERIAL_PORTS] = 
  { FOR_EACH_PORT( (EmSerialFifoQueue *),EM_SERIAL,_RX_QUEUE_ADDR ) };

uint16_t PGM emSerialRxQueueSizes[EM_NUM_SERIAL_PORTS] =
  { FOR_EACH_PORT( (uint16_t),EMBER_SERIAL,_RX_QUEUE_SIZE ) };

// In-flash data structure for determined port mode
uint8_t PGM emSerialPortModes[EM_NUM_SERIAL_PORTS] = 
  { FOR_EACH_PORT( (uint8_t),EMBER_SERIAL,_MODE ) };

//Compatibility code for the AVR Atmega
//If an AVR Atmega is used, then emSerialTx(/Rx)QueueWraps map to
//emSerialTx(/Rx)QueueMasks, otherwise they map to emSerialTx(/Rx)QueueSizes
#ifdef AVR_ATMEGA
uint8_t PGM emSerialTxQueueWraps[EM_NUM_SERIAL_PORTS] = 
  { FOR_EACH_PORT( (uint8_t), EMBER_SERIAL,_TX_QUEUE_SIZE-1 ) };
uint8_t PGM emSerialRxQueueWraps[EM_NUM_SERIAL_PORTS] = 
  { FOR_EACH_PORT( (uint8_t), EMBER_SERIAL,_RX_QUEUE_SIZE-1 ) };
#else
uint16_t PGM emSerialTxQueueWraps[EM_NUM_SERIAL_PORTS] = 
  { FOR_EACH_PORT( (uint16_t), EMBER_SERIAL,_TX_QUEUE_SIZE ) };
uint16_t PGM emSerialRxQueueWraps[EM_NUM_SERIAL_PORTS] = 
  { FOR_EACH_PORT( (uint16_t), EMBER_SERIAL,_RX_QUEUE_SIZE ) };
#endif

#ifdef EM_ENABLE_SERIAL_BLOCKING
// In-flash data structure for blocking mode
bool PGM emSerialBlocking[EM_NUM_SERIAL_PORTS] =
  { FOR_EACH_PORT( (bool),EM_SERIAL,_BLOCKSTATE ) };
#endif

uint8_t emSerialRxError[EM_NUM_SERIAL_PORTS] = {EMBER_SUCCESS,};
uint16_t emSerialRxErrorIndex[EM_NUM_SERIAL_PORTS] = {0,};
#endif //EMBER_SERIAL_USE_STDIO


//------------------------------------------------------
// Buffered Serial utility APIs

#ifdef EM_ENABLE_SERIAL_BUFFER
// always executed in interrupt context
void emSerialBufferNextMessageIsr(EmSerialBufferQueue *q) 
{
  EmSerialBufferQueueEntry *e = &q->fifo[q->tail];

  q->currentBuffer = e->buffer;
  q->nextByte = emberLinkedBufferContents(q->currentBuffer) + e->startIndex;
  if((e->length + e->startIndex) > PACKET_BUFFER_SIZE) {
    q->lastByte = q->nextByte + ((PACKET_BUFFER_SIZE-1) - e->startIndex);
    e->length -= PACKET_BUFFER_SIZE - e->startIndex;
  } else {
    q->lastByte = q->nextByte + e->length - 1;
    e->length = 0;
  }
}
#endif

#ifdef EM_ENABLE_SERIAL_BUFFER
// always executed in interrupt context
void emSerialBufferNextBlockIsr(EmSerialBufferQueue *q, uint8_t port)
{
  EmSerialBufferQueueEntry *e = &q->fifo[q->tail];
  
  if(e->length != 0) {
    q->currentBuffer = emberStackBufferLink(q->currentBuffer);
    q->nextByte = emberLinkedBufferContents(q->currentBuffer);
    if(e->length > PACKET_BUFFER_SIZE) {
      q->lastByte = q->nextByte + 31;
      e->length -= PACKET_BUFFER_SIZE;
    } else {
      q->lastByte = q->nextByte + e->length - 1;
      e->length = 0;
    }
  } else {
    #ifdef AVR_ATMEGA
      //If we are using an AVR host, non power-of-2 queue sizes are NOT
      //supported and therefore we use a mask    
      q->tail = ((q->tail+1) & emSerialTxQueueMasks[port]);
    #else // AVR_ATMEGA
      //If we are using the xap2b/cortexm3, non power-of-2 queue sizes are
      //supported and therefore we use a mod with the queue size
      q->tail = ((q->tail+1) % emSerialTxQueueSizes[port]);
    #endif // !AVR_ATMEGA
    q->dead++;
    q->used--;
    if(q->used)
      emSerialBufferNextMessageIsr(q);
    else
      q->nextByte = NULL;
  }
}
#endif

//------------------------------------------------------
// Serial initialization

EmberStatus emberSerialInit(uint8_t port, 
                            SerialBaudRate rate,
                            SerialParity parity,
                            uint8_t stopBits)
{
#ifdef EMBER_SERIAL_USE_STDIO
  //When using stdio, bypass the serial library initialization
  //and just initialize the low level UART driver.
  return halInternalUartInit(port, rate, parity, stopBits);
#else //EMBER_SERIAL_USE_STDIO

  if (emberSerialUnused(port)) return EMBER_SERIAL_INVALID_PORT;

  switch(emSerialPortModes[port]) {
#ifdef EM_ENABLE_SERIAL_FIFO
  case EMBER_SERIAL_FIFO: {
    EmSerialFifoQueue *q = (EmSerialFifoQueue *)emSerialTxQueues[port];
    ATOMIC_LITE(
      q->used = 0;
      q->head = 0;
      q->tail = 0;
    )
    break; }
#endif
#ifdef EM_ENABLE_SERIAL_BUFFER
  case EMBER_SERIAL_BUFFER: {
    EmSerialBufferQueue *q = (EmSerialBufferQueue *)emSerialTxQueues[port];
    ATOMIC_LITE(
      q->used = 0;
      q->head = 0;
      q->tail = 0;
      q->dead = 0;
      q->currentBuffer = EMBER_NULL_MESSAGE_BUFFER;
      q->nextByte = NULL;
      q->lastByte = NULL;
    )
    break; }
#endif
  default:
    return EMBER_SERIAL_INVALID_PORT;
    //break;  //statement is unreachable
  }

#if     (defined(EM_ENABLE_SERIAL_FIFO) || defined(EM_ENABLE_SERIAL_BUFFER))
  EmSerialFifoQueue *rq = emSerialRxQueues[port];
  ATOMIC_LITE(
    rq->used = 0;
    rq->head = 0;
    rq->tail = 0;
    emSerialRxError[port] = EMBER_SUCCESS;
  )

  return halInternalUartInit(port, rate, parity, stopBits);
#endif//(defined(EM_ENABLE_SERIAL_FIFO) || defined(EM_ENABLE_SERIAL_BUFFER))
#endif //EMBER_SERIAL_USE_STDIO
}

//------------------------------------------------------
// Serial Input

// returns # bytes available for reading
uint16_t emberSerialReadAvailable(uint8_t port)  
{
#ifdef EMBER_SERIAL_USE_STDIO
  return halInternalPrintfReadAvailable();
#else //EMBER_SERIAL_USE_STDIO
  halInternalUartRxPump(port);
  return emSerialRxQueues[port]->used;
#endif //EMBER_SERIAL_USE_STDIO
}

EmberStatus emberSerialReadByte(uint8_t port, uint8_t *dataByte)
{
#ifdef EMBER_SERIAL_USE_STDIO
  int ch=-1;
  ch = getchar();
  if(ch<0) {
    return EMBER_SERIAL_RX_EMPTY;
  }
  *dataByte = (uint8_t)ch;
  return EMBER_SUCCESS;
#else //EMBER_SERIAL_USE_STDIO
  uint8_t retval;
  EmSerialFifoQueue *q = emSerialRxQueues[port];

  if (emberSerialUnused(port)) {
    return EMBER_ERR_FATAL;
  }

  if(emSerialRxError[port] != EMBER_SUCCESS) {
    if(emSerialRxErrorIndex[port] == q->tail) {
      ATOMIC_LITE(
        retval = emSerialRxError[port];
        emSerialRxError[port] = EMBER_SUCCESS;
      )
      return retval;
    }
  }
  
  halInternalUartRxPump(port);
  halInternalUartFlowControl(port);
  
  if(q->used > 0) {
    ATOMIC_LITE(
      *dataByte = FIFO_DEQUEUE(q,emSerialRxQueueWraps[port]);
    )
    if(emSerialRxError[port] != EMBER_SUCCESS) {
      //This index is used when there is an error when the FIFO is full.
      if(emSerialRxErrorIndex[port] == RX_FIFO_FULL) {
        //q->tail has advanced by one, we can now mark the head as the error
        emSerialRxErrorIndex[port] = q->head;
      }
    }
    return EMBER_SUCCESS;
  } else {
    return EMBER_SERIAL_RX_EMPTY;
  }
#endif //EMBER_SERIAL_USE_STDIO
}

EmberStatus emberSerialReadData(uint8_t port,
                                uint8_t *data,
                                uint16_t length,
                                uint16_t *bytesRead)
{
  uint16_t bytesReadInternal = 0;
  EmberStatus status;

  while (bytesReadInternal < length) {
    status = emberSerialReadByte(port, data);

    switch (status) {
      case EMBER_SUCCESS:
        ++data;
        ++bytesReadInternal;
        break;

      case EMBER_SERIAL_RX_EMPTY:
        // empty queue is not an error for us, we just want to keep waiting
        break;

      default:
        // only store number of bytes read if the caller provided a non-NULL pointer
        if (bytesRead) {
          *bytesRead = bytesReadInternal;
        }
        return status;
    }
  }

  // only store number of bytes read if the caller provided a non-NULL pointer
  if (bytesRead) {
    *bytesRead = bytesReadInternal;
  }

  return EMBER_SUCCESS;

}

#ifndef EMBER_TEST
EmberStatus emberSerialReadDataTimeout(uint8_t port,
                                       uint8_t *data,
                                       uint16_t length,
                                       uint16_t *bytesRead,
                                       uint16_t firstByteTimeout,
                                       uint16_t subsequentByteTimeout)
{
  uint16_t bytesReadInternal = 0;
  EmberStatus status;
  uint16_t timeout = firstByteTimeout;
  uint16_t startTime = halCommonGetInt16uMillisecondTick();
  
  // loop until we read the max number of bytes or the timeout elapses
  while (bytesReadInternal < length
         && elapsedTimeInt16u(startTime,halCommonGetInt16uMillisecondTick()) < timeout) {
    status = emberSerialReadByte(port, data);

    switch (status) {
      case EMBER_SUCCESS:
        ++data;
        ++bytesReadInternal;
        // reset timer and timeout for next character
        startTime = halCommonGetInt16uMillisecondTick();
        timeout = subsequentByteTimeout;
        break;

      case EMBER_SERIAL_RX_EMPTY:
        // empty queue is not an error for us, we just want to keep waiting
        break;

      default:
        // only store number of bytes read if the caller provided a non-NULL pointer
        if (bytesRead) {
          *bytesRead = bytesReadInternal;
        }
        return status;
    }
  }

  // only store number of bytes read if the caller provided a non-NULL pointer
  if (bytesRead) {
    *bytesRead = bytesReadInternal;
  }

  return bytesReadInternal == length ? EMBER_SUCCESS : EMBER_SERIAL_RX_EMPTY;
}
#endif // EMBER_TEST

EmberStatus emberSerialReadPartialLine(uint8_t port, char *data, uint8_t max, uint8_t * index)
{
  EmberStatus err;
  uint8_t ch;

  if (((*index) == 0) || ((*index) >= max))
    data[0] = '\0';

  for (;;) {   
    err = emberSerialReadByte(port, &ch);

    // no new serial port char?, keep looping
    if (err) return err;

    // handle bogus characters
    if ( ch > 0x7F ) continue;

    // handle leading newline - fogBUGZ # 584
    if (((*index) == 0) &&
        ((ch == '\n') || (ch == 0))) continue;

    // Drop the CR, or NULL that is part of EOL sequence.
    if ((*index) >= max) {
      *index = 0;
      if ((ch == '\r') || (ch == 0)) continue;
    }

    // handle backspace
    if ( ch == 0x8 || ch == 0x7F ) {
      if ( (*index) > 0 ) {
        // delete the last character from our string
        (*index)--;
        data[*index] = '\0';
        // echo backspace
        emberSerialWriteString(port, "\b \b");
      }
      // don't add or process this character
      continue;
    }

    //if the string is about to overflow, fake in a CR
    if ( (*index) + 2 > max ) {
      ch = '\r';
    }

    emberSerialWriteByte(port, ch); // term char echo

    //upcase that char
    if ( ch>='a' && ch<='z') ch = ch - ('a'-'A');

    // build a string until we press enter
    if ( ( ch == '\r' ) || ( ch == '\n' ) ) {
      data[*index] = '\0';

      if (ch == '\r') {
        emberSerialWriteByte(port, '\n'); // "append" LF
        *index = 0;                       // Reset for next line; \n next
      } else {
        emberSerialWriteByte(port, '\r'); // "append" CR
        *index = max;                     // Reset for next line; \r,\0 next
      }

      return EMBER_SUCCESS;
    } 
      
    data[(*index)++] = ch;
  }
}

EmberStatus emberSerialReadLine(uint8_t port, char *data, uint8_t max)
{
  uint8_t index=0;

  while(emberSerialReadPartialLine(port, data, max, &index) != EMBER_SUCCESS) {
    halResetWatchdog();
  }
  return EMBER_SUCCESS;
}

//------------------------------------------------------
// Serial Output

// returns # bytes (if fifo mode)/messages (if buffer mode) that can be written
uint16_t emberSerialWriteAvailable(uint8_t port)  
{
#ifdef EMBER_SERIAL_USE_STDIO
  return halInternalPrintfWriteAvailable();
#else //EMBER_SERIAL_USE_STDIO
  switch(emSerialPortModes[port]) {
#ifdef EM_ENABLE_SERIAL_FIFO
  case EMBER_SERIAL_FIFO:
    return emSerialTxQueueSizes[port] - 
      ((EmSerialFifoQueue*)emSerialTxQueues[port])->used;
#endif
#ifdef EM_ENABLE_SERIAL_BUFFER
  case EMBER_SERIAL_BUFFER: {
    EmSerialBufferQueue *q = (EmSerialBufferQueue *)emSerialTxQueues[port];
    uint8_t elementsUsed;
    uint8_t elementsDead; 
    ATOMIC_LITE( // To clarify the volatile access.
           elementsUsed = q->used;
           elementsDead = q->dead;
           )
    return emSerialTxQueueSizes[port] - (elementsUsed + elementsDead);
    }
#endif
  default: {
  }
  }
  return 0;
#endif //EMBER_SERIAL_USE_STDIO
}

uint16_t emberSerialWriteUsed(uint8_t port)
{
#ifdef EMBER_SERIAL_USE_STDIO
  return 0;
#else
  return emSerialTxQueueSizes[port] - emberSerialWriteAvailable(port);
#endif //EMBER_SERIAL_USE_STDIO
}

EmberStatus emberSerialWriteByte(uint8_t port, uint8_t dataByte)
{
  return emberSerialWriteData(port, &dataByte, 1);
}

#ifndef EMBER_SERIAL_USE_STDIO
#ifdef EM_ENABLE_SERIAL_FIFO
static bool getOutputFifoSpace(EmSerialFifoQueue *q,
                                  uint8_t port,
                                  uint16_t extraByteCount)
{
  return (q->used < emSerialTxQueueSizes[port] - extraByteCount);
}
#endif
#endif //EMBER_SERIAL_USE_STDIO

EmberStatus emberSerialWriteString(uint8_t port, PGM_P string)
{
#ifdef EMBER_SERIAL_USE_STDIO
  while(*string != '\0') {
    putchar(*string);
    string++;
  }
  return EMBER_SUCCESS;
#else //EMBER_SERIAL_USE_STDIO
  switch(emSerialPortModes[port]) {
#ifdef EM_ENABLE_SERIAL_FIFO
  case EMBER_SERIAL_FIFO:
    {
      EmSerialFifoQueue *q = (EmSerialFifoQueue *)emSerialTxQueues[port];
      bool kickStartTx = false;

      while(*string != '\0') {
        while (! getOutputFifoSpace(q, port, 0)) {
          if (kickStartTx) {
            halInternalStartUartTx(port);
            kickStartTx = false;
          }
          #ifdef EM_ENABLE_SERIAL_BLOCKING
            if (emSerialBlocking[port]) {
              simulatedSerialTimePasses();
              // Pet the watchdog here?
              continue;
            }
          #endif
          return EMBER_SERIAL_TX_OVERFLOW;
        }
        ATOMIC_LITE(
          if (q->used == 0) {
            kickStartTx = true;
          }
          FIFO_ENQUEUE(q,*string,emSerialTxQueueWraps[port]);
        )
        string++;
      }
      // make sure the interrupt is enabled so it will be sent
      halInternalStartUartTx(port);
      return EMBER_SUCCESS;
    }
#endif
#ifdef EM_ENABLE_SERIAL_BUFFER
  case EMBER_SERIAL_BUFFER:
    {
      EmberMessageBuffer buff = emberAllocateStackBuffer();
      if(buff != EMBER_NULL_MESSAGE_BUFFER) {
        EmberStatus stat;
        if((stat=emberAppendPgmStringToLinkedBuffers(buff, string)) 
           == EMBER_SUCCESS) {
          stat = emberSerialWriteBuffer(port, buff, 0, emberMessageBufferLength(buff));
        }
        // Refcounts may be manipulated in ISR if DMA used
        ATOMIC( emberReleaseMessageBuffer(buff); )
        return stat;
      }
      return EMBER_NO_BUFFERS;
    }
#endif
  default:
    return EMBER_ERR_FATAL;
  }
#endif //EMBER_SERIAL_USE_STDIO
}


EmberStatus emberSerialWriteData(uint8_t port, uint8_t *data, uint8_t length)
{
#ifdef EMBER_SERIAL_USE_STDIO
  while(length--) {
    putchar(*data);
    data++;
  }
  return EMBER_SUCCESS;
#else //EMBER_SERIAL_USE_STDIO
  switch(emSerialPortModes[port]) {
#ifdef EM_ENABLE_SERIAL_FIFO
  case EMBER_SERIAL_FIFO:
    {
      EmSerialFifoQueue *q = (EmSerialFifoQueue *)emSerialTxQueues[port];
      bool kickStartTx = false;

      while(length--) {
        while (! getOutputFifoSpace(q, port, 0)) {
          if (kickStartTx) {
            halInternalStartUartTx(port);
            kickStartTx = false;
          }
          #ifdef EM_ENABLE_SERIAL_BLOCKING
            if (emSerialBlocking[port]) {
              simulatedSerialTimePasses();
              // Pet the watchdog here?
              continue;
            }
          #endif
          return EMBER_SERIAL_TX_OVERFLOW;
        }
        ATOMIC_LITE(
          if (q->used == 0) {
            kickStartTx = true;
          }
          FIFO_ENQUEUE(q,*data,emSerialTxQueueWraps[port]);
        )
        data++;
      }
      // make sure the interrupt is enabled so it will be sent
      halInternalStartUartTx(port);
      return EMBER_SUCCESS;
    }
#endif
#ifdef EM_ENABLE_SERIAL_BUFFER
  case EMBER_SERIAL_BUFFER:
    {
      // Note: We must always copy this, even in buffer mode
      //  since it is ram based data and there are no reference counts
      //  or indication of when it is actually written out the serial
      //  we cannot trust that the data won't be changed after this call
      //  but before it was actually written out.
      EmberMessageBuffer buff = emberFillLinkedBuffers(data,length);
      if(buff != EMBER_NULL_MESSAGE_BUFFER) {
        EmberStatus stat = emberSerialWriteBuffer(port, buff, 0, emberMessageBufferLength(buff));
        // Refcounts may be manipulated in ISR if DMA used
        ATOMIC( emberReleaseMessageBuffer(buff); )
        return stat;
      } else 
        return EMBER_NO_BUFFERS;
    }
#endif
  default:
    return EMBER_ERR_FATAL;
  }
#endif //EMBER_SERIAL_USE_STDIO
}

#ifdef EM_ENABLE_SERIAL_BUFFER
EmberStatus emberSerialWriteBuffer(uint8_t port, 
                                   EmberMessageBuffer buffer, 
                                   uint8_t start, 
                                   uint8_t length)
{
//Host processors do not use Ember Message Buffers.
#if defined(EZSP_HOST) || defined(EMBER_SERIAL_USE_STDIO)
  return EMBER_ERR_FATAL;  //This function is invalid.
#else// !EZSP_HOST && !EMBER_SERIAL_USE_STDIO

  if(buffer == EMBER_NULL_MESSAGE_BUFFER)
    return EMBER_ERR_FATAL;
  if(length == 0)
    return EMBER_SUCCESS;

  switch(emSerialPortModes[port]) {
#ifdef   EM_ENABLE_SERIAL_FIFO
  case EMBER_SERIAL_FIFO:
    {
      for(;PACKET_BUFFER_SIZE <= start; start-=PACKET_BUFFER_SIZE)
        buffer = emberStackBufferLink(buffer);
      
      while (0 < length) {
        uint8_t remainingInBuffer = PACKET_BUFFER_SIZE - start;
        uint8_t bytes = (length < remainingInBuffer
                       ? length
                       : remainingInBuffer);
        emberSerialWriteData(port,
                             emberMessageBufferContents(buffer) + start,
                             bytes);
        length -= bytes;
        start = 0;
        buffer = emberStackBufferLink(buffer);
      }
      // make sure the interrupt is enabled so it will be sent
      halInternalStartUartTx(port);
      break;
    }
#endif// EM_ENABLE_SERIAL_FIFO
#ifdef   EM_ENABLE_SERIAL_BUFFER
  case EMBER_SERIAL_BUFFER:
    {
      EmSerialBufferQueue *q = (EmSerialBufferQueue *)emSerialTxQueues[port];
      EmSerialBufferQueueEntry *e;
      uint8_t elementsUsed;
      uint8_t elementsDead;

      ATOMIC_LITE( // To clarify volatile access.
             elementsUsed = q->used;
             elementsDead = q->dead;
             )
      
      #ifdef   EM_ENABLE_SERIAL_BLOCKING
      if(emSerialBlocking[port]) {
        while((elementsUsed + elementsDead) >= emSerialTxQueueSizes[port]) {
          emberSerialBufferTick();
          //re-read the element counters after clocking the serial buffers
          ATOMIC_LITE( // To clarify volatile access.
                elementsUsed = q->used;
                elementsDead = q->dead;
                )
        }
      } else
      #endif// EM_ENABLE_SERIAL_BLOCKING
      if((elementsUsed + elementsDead) >= emSerialTxQueueSizes[port]) {
        if(elementsDead)
          emberSerialBufferTick();
        else
          return EMBER_SERIAL_TX_OVERFLOW;
      }

      for(;PACKET_BUFFER_SIZE <= start; start-=PACKET_BUFFER_SIZE) {
        buffer = emberStackBufferLink(buffer);
      }
      emberHoldMessageBuffer(buffer);
      
      e = &q->fifo[q->head];
      e->length = length;
      e->buffer = buffer;
      e->startIndex = start;
      #ifdef AVR_ATMEGA
        //If we are using an AVR host, non power-of-2 queue sizes are NOT
        //supported and therefore we use a mask
        q->head = ((q->head+1) & emSerialTxQueueMasks[port]);
      #else // AVR_ATMEGA
        //If we are using the xap2b/cortexm3, non power-of-2 queue sizes are
        //supported and therefore we use a mod with the queue size
        q->head = ((q->head+1) % emSerialTxQueueSizes[port]);
      #endif // !AVR_ATMEGA
      ATOMIC_LITE(
        q->used++;
      )
      halInternalStartUartTx(port);  
      break; 
    }
#endif// EM_ENABLE_SERIAL_BUFFER
  default:
    return EMBER_ERR_FATAL;
  }
  return EMBER_SUCCESS;
#endif// !EZSP_HOST && !EMBER_SERIAL_USE_STDIO
}
#endif// EM_ENABLE_SERIAL_BUFFER


EmberStatus emberSerialWaitSend(uint8_t port)  // waits for all byte to be written out of a port
{
#ifdef EMBER_SERIAL_USE_STDIO
  fflush(stdout);
#else //EMBER_SERIAL_USE_STDIO
  switch(emSerialPortModes[port]) {
#ifdef EM_ENABLE_SERIAL_FIFO
  case EMBER_SERIAL_FIFO: {
    EmSerialFifoQueue *q = (EmSerialFifoQueue *)emSerialTxQueues[port];
    while(q->used)
      simulatedSerialTimePasses();
    break; }
#endif
#ifdef EM_ENABLE_SERIAL_BUFFER
  case EMBER_SERIAL_BUFFER: {
    EmSerialBufferQueue *q = (EmSerialBufferQueue *)emSerialTxQueues[port];
    while(q->used) {
      simulatedSerialTimePasses();
    }
    break; }
#endif
  default: {
  }
  } // close switch.
  halInternalWaitUartTxComplete(port);
#endif //EMBER_SERIAL_USE_STDIO
  return EMBER_SUCCESS;
}

//------------------------------------------------------
// Guaranteed output

// The _only_ Guaranteed API:  The usage model for this api 
//   Does not require efficiency

#if !defined(EMBER_AF_PLUGIN_STANDARD_PRINTF_SUPPORT)

EmberStatus emberSerialGuaranteedPrintf(uint8_t port, PGM_P formatString, ...)
{
#ifdef EMBER_SERIAL_USE_STDIO
  //TODO: This function might have to break stdio abstraction to operate
  //      with interrupts off and guarantee a print.
  //NOTE: The following code is the same code found in emberSerialPrintf
  EmberStatus stat;
  halInternalForcePrintf(true);
  va_list ap;
  va_start (ap, formatString);
  stat = emberSerialPrintfVarArg(port, formatString, ap);
  va_end (ap);
  halInternalForcePrintf(false);
  return stat;
#else //EMBER_SERIAL_USE_STDIO
  va_list ap;
  
  // prevent interrupt driven transmission from intermixing
  halInternalStopUartTx(port);
  va_start(ap, formatString);
  emPrintfInternal(halInternalForceWriteUartData, port, formatString, ap);
  va_end(ap);

  // re-enable interrupt driven transmission if needed
  switch(emSerialPortModes[port]) {
#ifdef EM_ENABLE_SERIAL_FIFO
  case EMBER_SERIAL_FIFO: {
    EmSerialFifoQueue *q = (EmSerialFifoQueue *)emSerialTxQueues[port];
    if(q->used)
      halInternalStartUartTx(port);
    break; }
#endif
#ifdef EM_ENABLE_SERIAL_BUFFER
  case EMBER_SERIAL_BUFFER: {
    EmSerialBufferQueue *q = (EmSerialBufferQueue *)emSerialTxQueues[port];
    if(q->used)
      halInternalStartUartTx(port);
    break; }
#endif
  default: {
  }
  } //close switch.

  return EMBER_SUCCESS;
#endif //EMBER_SERIAL_USE_STDIO
}

#endif

//------------------------------------------------------
// Serial buffer maintenance
void emberSerialFlushRx(uint8_t port) 
{
#ifdef EMBER_SERIAL_USE_STDIO
  ATOMIC(
    while(halInternalPrintfReadAvailable()) {
      (int)getchar();
    }
  )
#else //EMBER_SERIAL_USE_STDIO
  EmSerialFifoQueue *q = (EmSerialFifoQueue *)emSerialRxQueues[port];

  ATOMIC_LITE(
     q->used = 0;
     q->head = 0;
     q->tail = 0;
     )
#endif //EMBER_SERIAL_USE_STDIO
}

//------------------------------------------------------
// Serial Buffer Cleanup Tick

#ifdef EM_ENABLE_SERIAL_BUFFER
//Helper function to calculate deadIndex since ifdefs cannot exist in the 
//ATOMIC_LITE block
uint8_t calculateDeadIndex(uint8_t port, uint8_t tail, uint8_t numDead) {
  uint8_t deadIndex;
  
  #ifdef AVR_ATMEGA
    //If we are using an AVR host, non power-of-2 queue sizes are NOT
    //supported and therefore we use a mask
    deadIndex = (tail - numDead) & emSerialTxQueueMasks[port];
  #else // AVR_ATMEGA
    //If we are using the xap2b/cortexm3, non power-of-2 queue sizes are
    //supported and therefore we need to use a conditional to figure
    //out the deadIndex
    if(numDead > tail) {
      //Since subtracting numDead from tail would cause deadIndex to
      //wrap, we add the tail to the queue size and then subtract
      //numDead
      deadIndex = (emSerialTxQueueSizes[port] + tail) - numDead;
    } else {
      deadIndex = tail - numDead;
    }
  #endif // !AVR_ATMEGA
  
  return deadIndex;
}
#endif //EM_ENABLE_SERIAL_BUFFER
    
void emberSerialBufferTick(void)
{
#ifdef EM_ENABLE_SERIAL_BUFFER
  uint8_t port;
  EmSerialBufferQueue *q;
  uint8_t numDead, deadIndex;

  for(port=0; port<EM_NUM_SERIAL_PORTS; port++) {
    if(emSerialPortModes[port] == EMBER_SERIAL_BUFFER) {
      q = (EmSerialBufferQueue *)emSerialTxQueues[port];

      if(q->dead) {
        ATOMIC_LITE(
          numDead = q->dead;
          q->dead = 0;
          deadIndex = calculateDeadIndex(port,q->tail,numDead);
        )
        for(;numDead;numDead--) {
          // Refcounts may be manipulated in ISR if DMA used
          ATOMIC( emberReleaseMessageBuffer(q->fifo[deadIndex].buffer); )
          #ifdef AVR_ATMEGA
            //If we are using an AVR host, non power-of-2 queue sizes are NOT
            //supported and therefore we use a mask
            deadIndex = (deadIndex + 1) & emSerialTxQueueMasks[port];
          #else // AVR_ATMEGA
            //If we are using the xap2b/cortexm3, non power-of-2 queue sizes are
            //supported and therefore we use a mod with the queue size
            deadIndex = (deadIndex + 1) % emSerialTxQueueSizes[port];
          #endif // !AVR_ATMEGA
        }
      }
    }
  }

  simulatedSerialTimePasses();
#endif
}

bool emberSerialUnused(uint8_t port)
{
#ifdef EMBER_SERIAL_USE_STDIO
  return false;
#else //EMBER_SERIAL_USE_STDIO
  return (EM_NUM_SERIAL_PORTS <= port
          || emSerialPortModes[port] == EMBER_SERIAL_UNUSED);
#endif //EMBER_SERIAL_USE_STDIO
}

#endif //CORTEXM3_EFM32_MICRO
