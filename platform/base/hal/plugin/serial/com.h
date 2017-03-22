/***************************************************************************//**
 * @file com.h
 * @brief COM Layer.
 * @version 0.01.0
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
#ifndef __COM_H__
#define __COM_H__

#include "em_device.h"
#include "em_gpio.h"
#include "uartdrv.h"
#include "hal/micro/cortexm3/usb/em_usb.h"
#include "com-serial.h"
#include "stack/include/ember-debug.h"

#ifdef CORTEXM3_EFM32_MICRO
  #include "com_device.h"
  #include "com_queue_size.h"
  #include "com_config.h"
#endif

#if DEBUG_LEVEL == NO_DEBUG
  #undef COM_VCP_ENABLE
#endif

// Enable individual com ports
typedef enum COM_Port
{
  // Legacy COM port defines
  COM_VCP = 0,
  COM_USART0 = 1,
  COM_USART1 = 2,
  COM_USART2 = 3,
  COM_USB = 4,
  COM_LEUART0 = 5,
  COM_LEUART1 = 6,

  // VCP
  comPortVcp = 0x10,

  // USARTs
  comPortUsart0 = 0x20,
  comPortUsart1 = 0x21,
  comPortUsart2 = 0x22,

  // UARTs

  // LEUARTs
  comPortLeuart0 = 0x40,
  comPortLeuart1 = 0x41,

  // USB
  comPortUsb = 0x50,
} COM_Port_t;
#ifdef __CC_ARM
#pragma anon_unions
#endif
typedef struct
{
  union uartdrvinit {
    UARTDRV_InitUart_t uartinit;
    UARTDRV_InitLeuart_t leuartinit;
  } uartdrvinit;
  uint16_t rxStop;
  uint16_t rxStart;
} COM_Init_t;

typedef struct {
  /** Index of next byte to send.*/
  uint16_t head;
  /** Index of where to enqueue next message.*/
  uint16_t tail;
  /** Number of bytes queued.*/
  volatile uint16_t used;
  /*! Number of bytes pumped */
  uint16_t pumped;
  /** FIFO of queue data.*/
  uint8_t fifo[];
} COM_FifoQueue_t;

typedef struct COM_HandleData
{
  COM_FifoQueue_t *rxQueue;
  COM_FifoQueue_t *txQueue;
  uint16_t rxsize;
  uint16_t txsize;
  bool txPaused;
  bool txCatchUp;
  uint16_t rxStop;
  uint16_t rxStart;
  uint16_t bufferIndex;
  uint16_t bufferLimit;
  UARTDRV_Handle_t uarthandle;
} COM_HandleData_t;

/// Handle pointer
typedef COM_HandleData_t * COM_Handle_t;

//------------------------------------------------------
// Memory allocations for Queue data structures

//Macros to define fifo and buffer queues, can't use a typedef becuase the size
// of the fifo array in the queues can change
#define DEFINE_FIFO_QUEUE(qSize, qName)             \
  static struct {                                   \
    /*! Indexes of next byte to send*/              \
    uint16_t head;                                  \
    /*! Index of where to enqueue next message*/    \
    uint16_t tail;                                  \
    /*! Number of bytes queued*/                    \
    volatile uint16_t used;                         \
    /*! Number of bytes pumped */                   \
    uint16_t pumped;                                \
    /*! FIFO of queue data*/                        \
    uint8_t fifo[qSize];                            \
  } qName;

#undef  FIFO_ENQUEUE // Avoid possible warning, replace other definition
#define FIFO_ENQUEUE(queue,data,size)               \
  do {                                              \
    (queue)->fifo[(queue)->head] = (data);          \
    (queue)->head = (((queue)->head + 1) % (size)); \
    (queue)->used++;                                \
  } while(0)
#undef  FIFO_DEQUEUE // Avoid possible warning, replace other definition
#define FIFO_DEQUEUE(queue,size)                    \
  (queue)->fifo[(queue)->tail];                     \
  (queue)->tail = (((queue)->tail + 1) % (size));   \
  (queue)->used--
#define FIFO_DEQUEUE_MULTIPLE(queue,size,num) \
  do {(queue)->tail = (((queue)->tail + (num)) % (size));  \
  (queue)->used -= (num);} while(0)

#ifndef halEnableVCOM
  #define halEnableVCOM()
#endif


#define COM_INITUART(initdata) ((COM_Init_t) initdata)

void COM_InternalPowerDown();
void COM_InternalPowerUp();
bool COM_InternalTxIsIdle(COM_Port_t port);
bool COM_InternalRxIsPaused(COM_Port_t port);
bool COM_InternalTxIsPaused(COM_Port_t port);
Ecode_t COM_InternalReceiveData(COM_Port_t port, uint8_t *data, uint32_t length);

Ecode_t COM_Init(COM_Port_t port, COM_Init_t *init);
Ecode_t COM_DeInit(COM_Port_t port);

uint16_t COM_ReadAvailable(COM_Port_t port);
Ecode_t COM_ReadByte(COM_Port_t port, uint8_t *dataByte);
Ecode_t COM_ReadData(COM_Port_t port,
                                uint8_t *data,
                                uint16_t length,
                                uint16_t *bytesRead);
Ecode_t COM_ReadDataTimeout(COM_Port_t port,
                                       uint8_t *data,
                                       uint16_t length,
                                       uint16_t *bytesRead,
                                       uint16_t firstByteTimeout,
                                       uint16_t subsequentByteTimeout);
Ecode_t COM_ReadPartialLine(COM_Port_t port, char *data, uint8_t max, uint8_t * index);
Ecode_t COM_ReadLine(COM_Port_t port, char *data, uint8_t max);

Ecode_t COM_ForceWriteData(COM_Port_t port, uint8_t *data, uint8_t length);
uint16_t COM_WriteAvailable(COM_Port_t port);
uint16_t COM_WriteUsed(COM_Port_t port);
Ecode_t COM_WriteByte(COM_Port_t port, uint8_t dataByte);
Ecode_t COM_WriteHex(COM_Port_t port, uint8_t dataByte);
Ecode_t COM_WriteString(COM_Port_t port, PGM_P string);
Ecode_t COM_PrintCarriageReturn(COM_Port_t port);
Ecode_t COM_Printf(COM_Port_t port, PGM_P formatString, ...);
Ecode_t COM_PrintfVarArg(COM_Port_t port, PGM_P formatString, va_list ap);
Ecode_t COM_PrintfLine(COM_Port_t port, PGM_P formatString, ...);
Ecode_t COM_WriteData(COM_Port_t port, uint8_t *data, uint8_t length);
Ecode_t COM_GuaranteedPrintf(COM_Port_t port, PGM_P formatString, ...);
Ecode_t COM_WaitSend(COM_Port_t port);
void COM_FlushRx(COM_Port_t port);
bool COM_Unused(uint8_t port);

#endif //__COM_H__
