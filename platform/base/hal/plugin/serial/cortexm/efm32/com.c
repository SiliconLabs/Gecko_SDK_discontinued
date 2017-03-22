/***************************************************************************//**
 * @file com.c
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

#include PLATFORM_HEADER
#include "stack/include/ember-types.h"
#include "stack/include/error.h"

#include "hal/hal.h"

#include <stdarg.h>

#ifdef CORTEXM3_EFM32_MICRO
#include "em_device.h"
#include "em_usart.h"
#include "em_core.h"
#endif

#include "serial/com.h"
#include "serial/ember-printf.h"

// Set COM_UART_ENABLE if at least one UART is enabled
#if (defined(COM_USART0_ENABLE)     \
     || defined(COM_USART1_ENABLE)  \
     || defined(COM_USART2_ENABLE)  \
     || defined(COM_LEUART0_ENABLE) \
     || defined(COM_LEUART1_ENABLE))
#define COM_UART_ENABLE
#endif

// --------------------------------
// Static declarations
static inline bool checkValidPort(COM_Port_t port);
static void dequeueFifoBuffer(COM_Handle_t comhandle, uint16_t count);
static COM_Handle_t getComHandleFromPort(COM_Port_t port);
static inline bool getOutputFifoSpace(COM_Handle_t comhandle, uint8_t extraByteCount);
static inline bool getInputFifoSpace(COM_Handle_t comhandle, uint8_t extraByteCount);
static void rxGpioIntEnable(void);
static void rxGpioIntDisable(void);
static void rxGpioIntInit(void);
static Ecode_t setComHandleQueues(COM_Port_t port);
static void txBuffer(COM_Port_t port, uint8_t *data, uint16_t length);
static void updateBuffer(COM_FifoQueue_t *q, uint16_t xferred, uint16_t size);

#if defined(COM_VCP_ENABLE)
static inline bool checkValidVcpPort(COM_Port_t port);
#endif

#if defined(COM_UART_ENABLE)
static inline bool checkValidUartPort(COM_Port_t port);
static void enableRxIrq(COM_Port_t port, bool enable);
static UARTDRV_Handle_t getUartHandleFromPort(COM_Port_t port);
static void rxCallback(UARTDRV_Handle_t handle,
                       Ecode_t transferStatus,
                       uint8_t *data,
                       UARTDRV_Count_t transferCount);
static void rxNextBuffer(COM_Handle_t handle);
static Ecode_t setUartBufferQueues(COM_Port_t port, COM_Init_t * init);
static void txCatchUp(COM_Handle_t comhandle);
static void txCallback(UARTDRV_Handle_t handle,
                       Ecode_t transferStatus,
                       uint8_t *data,
                       UARTDRV_Count_t transferCount);
#endif
/* Initialization data and buffer queues */

// -------------------------------------------------------------------------
// The macro DEFINE_BUF_QUEUE() can only be used for static definition of 
// buffer queues, which includes a struct typedef. Since it is always used
// in a standalone fashion, this expansion will not interfere with any other
// logic and does not require enclosing parentheses 
//cstat -MISRAC2012-Rule-20.7
// -------------------------------------------------------------------------
#ifdef COM_VCP_ENABLE
  //add VCP support
  #include "stack/platform/micro/debug-channel.h"
  /* FIFO Buffers */
  DEFINE_FIFO_QUEUE(COM_VCP_RX_QUEUE_SIZE, comFifoQueueRxVcp)
  DEFINE_FIFO_QUEUE(COM_VCP_TX_QUEUE_SIZE, comFifoQueueTxVcp)
#endif //COM_VCP_ENABLE

#ifdef COM_USART0_ENABLE
  /* UARTDRV buffer queues */
  DEFINE_BUF_QUEUE(EMDRV_UARTDRV_MAX_CONCURRENT_RX_BUFS, comBufferQueueRxUsart0);
  DEFINE_BUF_QUEUE(EMDRV_UARTDRV_MAX_CONCURRENT_TX_BUFS, comBufferQueueTxUsart0);
  /* COM FIFO Buffers */
  DEFINE_FIFO_QUEUE(COM_USART0_RX_QUEUE_SIZE,comFifoQueueRxUsart0)
  DEFINE_FIFO_QUEUE(COM_USART0_TX_QUEUE_SIZE,comFifoQueueTxUsart0)
#endif

#ifdef COM_USART1_ENABLE
  /* UARTDRV buffer queues */
  DEFINE_BUF_QUEUE(EMDRV_UARTDRV_MAX_CONCURRENT_RX_BUFS, comBufferQueueRxUsart1);
  DEFINE_BUF_QUEUE(EMDRV_UARTDRV_MAX_CONCURRENT_TX_BUFS, comBufferQueueTxUsart1);
  /* COM FIFO Buffers */
  DEFINE_FIFO_QUEUE(COM_USART1_RX_QUEUE_SIZE,comFifoQueueRxUsart1)
  DEFINE_FIFO_QUEUE(COM_USART1_TX_QUEUE_SIZE,comFifoQueueTxUsart1)
#endif

#ifdef COM_USART2_ENABLE
  /* UARTDRV buffer queues */
  DEFINE_BUF_QUEUE(EMDRV_UARTDRV_MAX_CONCURRENT_RX_BUFS, comBufferQueueRxUsart2);
  DEFINE_BUF_QUEUE(EMDRV_UARTDRV_MAX_CONCURRENT_TX_BUFS, comBufferQueueTxUsart2);
  /* COM FIFO Buffers */
  DEFINE_FIFO_QUEUE(COM_USART2_RX_QUEUE_SIZE,comFifoQueueRxUsart2)
  DEFINE_FIFO_QUEUE(COM_USART2_TX_QUEUE_SIZE,comFifoQueueTxUsart2)
#endif

#ifdef COM_USB_ENABLE
  /* COM FIFO Buffers */
  DEFINE_FIFO_QUEUE(COM_USB_RX_QUEUE_SIZE, comFifoQueueRxUsb)
  DEFINE_FIFO_QUEUE(COM_USB_TX_QUEUE_SIZE, comFifoQueueTxUsb)
#endif

#ifdef COM_LEUART0_ENABLE
  /* UARTDRV buffer queues */
  DEFINE_BUF_QUEUE(EMDRV_UARTDRV_MAX_CONCURRENT_RX_BUFS, comBufferQueueRxLeuart0);
  DEFINE_BUF_QUEUE(EMDRV_UARTDRV_MAX_CONCURRENT_TX_BUFS, comBufferQueueTxLeuart0);
  /* COM FIFO Buffers */
  DEFINE_FIFO_QUEUE(COM_LEUART0_RX_QUEUE_SIZE,comFifoQueueRxLeuart0)
  DEFINE_FIFO_QUEUE(COM_LEUART0_TX_QUEUE_SIZE,comFifoQueueTxLeuart0)
#endif

#ifdef COM_LEUART1_ENABLE
  /* UARTDRV buffer queues */
  DEFINE_BUF_QUEUE(EMDRV_UARTDRV_MAX_CONCURRENT_RX_BUFS, comBufferQueueRxLeuart1);
  DEFINE_BUF_QUEUE(EMDRV_UARTDRV_MAX_CONCURRENT_TX_BUFS, comBufferQueueTxLeuart1);
  /* COM FIFO Buffers */
  DEFINE_FIFO_QUEUE(COM_LEUART1_RX_QUEUE_SIZE,comFifoQueueRxLeuart1)
  DEFINE_FIFO_QUEUE(COM_LEUART1_TX_QUEUE_SIZE,comFifoQueueTxLeuart1)
#endif
// -------------------------------------------------------------------------
// Re-enable enclosing parentheses CSTAT rule
//cstat +MISRAC2012-Rule-20.7
// -------------------------------------------------------------------------
// --------------------------------
// COM handle array indexes
typedef enum COM_HandleIndex
{
#ifdef COM_VCP_ENABLE
  comHandleIndexVcp,
#endif
#ifdef COM_USART0_ENABLE
  comHandleIndexUsart0,
#endif
#ifdef COM_USART1_ENABLE
  comHandleIndexUsart1,
#endif
#ifdef COM_USART2_ENABLE
  comHandleIndexUsart2,
#endif
#ifdef COM_USB_ENABLE
  comHandleIndexUsb,
#endif
#ifdef COM_LEUART0_ENABLE
  comHandleIndexLeuart0,
#endif
#ifdef COM_LEUART1_ENABLE
  comHandleIndexLeuart1,
#endif
  comHandleIndexSize
} COM_HandleIndex_t;
// COM handles
static COM_HandleData_t comhandledata[comHandleIndexSize];

// --------------------------------
// UART handle array indexes
#if defined(COM_UART_ENABLE)
typedef enum COM_UartHandleIndex
{
#ifdef COM_USART0_ENABLE
  comUartHandleIndexUsart0,
#endif
#ifdef COM_USART1_ENABLE
  comUartHandleIndexUsart1,
#endif
#ifdef COM_USART2_ENABLE
  comUartHandleIndexUsart2,
#endif
#ifdef COM_LEUART0_ENABLE
  comUartHandleIndexLeuart0,
#endif
#ifdef COM_LEUART1_ENABLE
  comUartHandleIndexLeuart1,
#endif
  comUartHandleIndexSize
} COM_UartHandleIndex_t;
// UART handles
static UARTDRV_HandleData_t uarthandledata[comUartHandleIndexSize];
#endif

// DMA interrupt state
uint32_t dma_IEN;

static inline bool checkValidPort(COM_Port_t port)
{
  switch (port) {
#ifdef COM_VCP_ENABLE
    case COM_VCP:
    case comPortVcp:
        return true;
#endif
#ifdef COM_USART0_ENABLE
    case COM_USART0:
    case comPortUsart0:
        return true;
#endif
#ifdef COM_USART1_ENABLE
    case COM_USART1:
    case comPortUsart1:
        return true;
#endif
#ifdef COM_USART2_ENABLE
    case COM_USART2:
    case comPortUsart2:
        return true;
#endif
#ifdef COM_USB_ENABLE
    case COM_USB:
    case comPortUsb:
        return true;
#endif
#ifdef COM_LEUART0_ENABLE
    case COM_LEUART0:
    case comPortLeuart0:
        return true;
#endif
#ifdef COM_LEUART1_ENABLE
    case COM_LEUART1:
    case comPortLeuart1:
        return true;
#endif
    default:
      return false;
  }
}

static void dequeueFifoBuffer(COM_Handle_t comhandle, uint16_t count)
{
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_ATOMIC();
  FIFO_DEQUEUE_MULTIPLE(comhandle->txQueue,comhandle->txsize,count);
  CORE_EXIT_ATOMIC();
}

/**
 * @brief Convert a COM_Port_t into a pointer to COM handle
 *
 * @param[in] port COM port number
 *
 * @return Pointer to COM handle
 */
static COM_Handle_t getComHandleFromPort(COM_Port_t port)
{
  size_t index;

  switch(port)
  {
#ifdef COM_VCP_ENABLE
    case COM_VCP:
    case comPortVcp:
      index = comHandleIndexVcp;
      break;
#endif
#ifdef COM_USART0_ENABLE
    case COM_USART0:
    case comPortUsart0:
      index = comHandleIndexUsart0;
      break;
#endif
#ifdef COM_USART1_ENABLE
    case COM_USART1:
    case comPortUsart1:
      index = comHandleIndexUsart1;
      break;
#endif
#ifdef COM_USART2_ENABLE
    case COM_USART2:
    case comPortUsart2:
      index = comHandleIndexUsart2;
      break;
#endif
#ifdef COM_USB_ENABLE
    case COM_USB:
    case comPortUsb:
      index = comHandleIndexUsart0;
      break;
#endif
#ifdef COM_LEUART0_ENABLE
    case COM_LEUART0:
    case comPortLeuart0:
      index = comHandleIndexLeuart0;
      break;
#endif
#ifdef COM_LEUART1_ENABLE
    case COM_LEUART1:
    case comPortLeuart1:
      index = comHandleIndexLeuart1;
      break;
#endif
    default:
      assert(false);
      // Will never reach this line
      return comhandledata;
  }

  return &comhandledata[index];
}

static inline bool getOutputFifoSpace(COM_Handle_t comhandle, uint8_t extraByteCount)
{
  return (comhandle->txQueue->used < comhandle->txsize - extraByteCount);
}

static inline bool getInputFifoSpace(COM_Handle_t comhandle, uint8_t extraByteCount)
{
  return (comhandle->rxQueue->used < comhandle->rxsize - extraByteCount);
}

static void pumpRx(COM_Port_t port)
{
  if (checkValidPort(port) == false)
  {
    return;
  }
  // UART
#if defined(COM_UART_ENABLE)
  if (checkValidUartPort(port))
  {
    COM_Handle_t comhandle = getComHandleFromPort(port);
    uint8_t *buf;
    UARTDRV_Count_t xferred, remaining;
    CORE_ATOMIC_SECTION(
    UARTDRV_GetReceiveStatus(comhandle->uarthandle, &buf, &xferred, &remaining);
    updateBuffer(comhandle->rxQueue, xferred, comhandle->rxsize);
    )
    // Enable flow control if appropriate after updating buffer
    if ( (UARTDRV_FlowControlGetSelfStatus(comhandle->uarthandle) == uartdrvFlowControlOff) &&
         ( (comhandle->rxsize - comhandle->rxQueue->used) >=
           comhandle->rxStart) )
    {
      // Only enable flow control if DMAs are set up when using HW flow control
      if ( ( (comhandle->uarthandle->fcType == uartdrvFlowControlHw) &&
             (UARTDRV_GetReceiveDepth(comhandle->uarthandle)>=2) ) ||
           (comhandle->uarthandle->fcType == uartdrvFlowControlSw) )
      {
        UARTDRV_FlowControlSet(comhandle->uarthandle, uartdrvFlowControlOn);
      }
    }
  }
#endif
#ifdef COM_VCP_ENABLE
  if (port == COM_VCP)
  {
    emDebugReceiveData();
    return;
  }
#endif
}

/* Enable interrupt on the UART RX pin, in order to be able to wakeup on rx
 * activity in energy modes where the UART is not available. */
static void rxGpioIntEnable(void)
{
#ifdef CORTEXM3_EFM32_MICRO
  GPIO_IntClear(1 << UART_RX_INT_PIN);
  GPIO_IntEnable(1 << UART_RX_INT_PIN);
#endif
}

/* Disable interrupt on the UART RX pin */
static void rxGpioIntDisable(void)
{
#ifdef CORTEXM3_EFM32_MICRO
  GPIO_IntDisable(1 << UART_RX_INT_PIN);
  GPIO_IntClear(1 << UART_RX_INT_PIN);
#endif
}

/* Initialize the UART RX pin as a GPIO Interrupt. GPIO Interrupt is used
 * in order to be able to wakeup on rx activity in energy modes when the
 * UART peripheral is not available. */
static void rxGpioIntInit(void)
{
#ifdef CORTEXM3_EFM32_MICRO
  GPIO_ExtIntConfig(UART_RX_INT_PORT, UART_RX_INT_PIN, UART_RX_INT_PIN, false, true, false);
#endif
}

/**
 * Set RX/TX queues in COM handle based on given COM port
 */
static Ecode_t setComHandleQueues(COM_Port_t port)
{
  COM_Handle_t comhandle = getComHandleFromPort(port);
  switch(port)
  {
#ifdef COM_VCP_ENABLE
    case COM_VCP:
    case comPortVcp:
      comhandle->rxQueue = (COM_FifoQueue_t *)&comFifoQueueRxVcp;
      comhandle->txQueue = (COM_FifoQueue_t *)&comFifoQueueTxVcp;
      comhandle->rxsize = COM_VCP_RX_QUEUE_SIZE;
      comhandle->txsize = COM_VCP_TX_QUEUE_SIZE;
      break;
#endif
#ifdef COM_USART0_ENABLE
    case COM_USART0:
    case comPortUsart0:
      comhandle->rxQueue = (COM_FifoQueue_t *)&comFifoQueueRxUsart0;
      comhandle->txQueue = (COM_FifoQueue_t *)&comFifoQueueTxUsart0;
      comhandle->rxsize = COM_USART0_RX_QUEUE_SIZE;
      comhandle->txsize = COM_USART0_TX_QUEUE_SIZE;
      break;
#endif
#ifdef COM_USART1_ENABLE
    case COM_USART1:
    case comPortUsart1:
      comhandle->rxQueue = (COM_FifoQueue_t *)&comFifoQueueRxUsart1;
      comhandle->txQueue = (COM_FifoQueue_t *)&comFifoQueueTxUsart1;
      comhandle->rxsize = COM_USART1_RX_QUEUE_SIZE;
      comhandle->txsize = COM_USART1_TX_QUEUE_SIZE;
      break;
#endif
#ifdef COM_USART2_ENABLE
    case COM_USART2:
    case comPortUsart2:
      comhandle->rxQueue = (COM_FifoQueue_t *)&comFifoQueueRxUsart2;
      comhandle->txQueue = (COM_FifoQueue_t *)&comFifoQueueTxUsart2;
      comhandle->rxsize = COM_USART2_RX_QUEUE_SIZE;
      comhandle->txsize = COM_USART2_TX_QUEUE_SIZE;
      break;
#endif
#ifdef COM_LEUART0_ENABLE
    case COM_LEUART0:
    case comPortLeuart0:
      comhandle->rxQueue = (COM_FifoQueue_t *)&comFifoQueueRxLeuart0;
      comhandle->txQueue = (COM_FifoQueue_t *)&comFifoQueueTxLeuart0;
      comhandle->rxsize = COM_LEUART0_RX_QUEUE_SIZE;
      comhandle->txsize = COM_LEUART0_TX_QUEUE_SIZE;
      break;
#endif
#ifdef COM_LEUART1_ENABLE
    case COM_LEUART1:
    case comPortLeuart1:
      comhandle->rxQueue = (COM_FifoQueue_t *)&comFifoQueueRxLeuart1;
      comhandle->txQueue = (COM_FifoQueue_t *)&comFifoQueueTxLeuart1;
      comhandle->rxsize = COM_LEUART1_RX_QUEUE_SIZE;
      comhandle->txsize = COM_LEUART1_TX_QUEUE_SIZE;
      break;
#endif
    default:
      return EMBER_ERR_FATAL;
      break;
  }

  return EMBER_SUCCESS;
}

/* reload buffer callbacks */
static void updateBuffer(COM_FifoQueue_t *q, uint16_t xferred, uint16_t size)
{
  // Update tail with additional xferred. Data should already be there
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_ATOMIC();
    if (xferred > q->pumped)
    {
    q->head = ((q->head - q->pumped + xferred) % size);
    q->used += xferred - q->pumped;
    q->pumped = xferred;
    }
  CORE_EXIT_ATOMIC();
}

static void txBuffer(COM_Port_t port, uint8_t *data, uint16_t length)
{
#if defined(COM_UART_ENABLE)
  if (checkValidUartPort(port))
  {
    COM_Handle_t comhandle = getComHandleFromPort(port);
    // Skip transmit if paused
    if (comhandle->txPaused)
    {
      comhandle->txCatchUp = true;
      return;
    }
    // Catch up from skipped transmit(s) from before
    if (comhandle->txCatchUp)
    {
      txCatchUp(comhandle);
    }
    else if (UARTDRV_Transmit(comhandle->uarthandle, data, length, txCallback) != EMBER_SUCCESS)
    {
      while(UARTDRV_Transmit(comhandle->uarthandle, data, length, txCallback) != EMBER_SUCCESS);
    }
  }
#endif
  return;
}

/**
 * VCP-only static functions
 */
#if defined(COM_VCP_ENABLE)
static inline bool checkValidVcpPort(COM_Port_t port)
{
  switch (port) {
#ifdef COM_VCP_ENABLE
    case COM_VCP:
    case comPortVcp:
        return true;
#endif
    default:
      return false;
  }
}
#endif

/**
 * UART-only static functions
 */
#if defined(COM_UART_ENABLE)
static inline bool checkValidLeuartPort(COM_Port_t port)
{
  switch (port) {
#ifdef COM_LEUART0_ENABLE
    case COM_LEUART0:
    case comPortLeuart0:
        return true;
#endif
#ifdef COM_LEUART1_ENABLE
    case COM_LEUART1:
    case comPortLeuart1:
        return true;
#endif
    default:
      return false;
  }
}

static inline bool checkValidUartPort(COM_Port_t port)
{
  switch (port) {
#ifdef COM_USART0_ENABLE
    case COM_USART0:
    case comPortUsart0:
        return true;
#endif
#ifdef COM_USART1_ENABLE
    case COM_USART1:
    case comPortUsart1:
        return true;
#endif
#ifdef COM_USART2_ENABLE
    case COM_USART2:
    case comPortUsart2:
        return true;
#endif
#ifdef COM_LEUART0_ENABLE
    case COM_LEUART0:
    case comPortLeuart0:
        return true;
#endif
#ifdef COM_LEUART1_ENABLE
    case COM_LEUART1:
    case comPortLeuart1:
        return true;
#endif
    default:
      return false;
  }
}

static void enableRxIrq(COM_Port_t port, bool enable)
{
  COM_Handle_t comhandle = getComHandleFromPort(port);
  IRQn_Type irqn;
  switch (port) {
  #ifdef COM_USART0_ENABLE
    case COM_USART0:
      irqn = USART0_RX_IRQn;
      break;
  #endif
  #ifdef COM_USART1_ENABLE
    case COM_USART1:
      irqn = USART1_RX_IRQn;
      break;
  #endif
  #ifdef COM_USART2_ENABLE
    case COM_USART2:
      irqn = USART2_RX_IRQn;
      break;
  #endif
  #ifdef COM_LEUART0_ENABLE
    case COM_LEUART0:
      irqn = LEUART0_IRQn;
      break;
  #endif
  #ifdef COM_LEUART1_ENABLE
    case COM_LEUART1:
      irqn = LEUART1_IRQn;
      break;
  #endif
    default:
      return;
  }
  if (comhandle->uarthandle->type == uartdrvUartTypeUart)
  {
    /* Clear previous RX interrupts */
    USART_IntClear(comhandle->uarthandle->peripheral.uart, USART_IF_RXDATAV);
    NVIC_ClearPendingIRQ(irqn);

    if (enable)
    {
      /* Enable RX interrupts */
      USART_IntEnable(comhandle->uarthandle->peripheral.uart, USART_IF_RXDATAV);
      NVIC_EnableIRQ(irqn);

      USART_Enable(comhandle->uarthandle->peripheral.uart, usartEnable);
    }
    else
    {
      /* Disable RX interrupts */
      USART_IntDisable(comhandle->uarthandle->peripheral.uart, USART_IF_RXDATAV);
      NVIC_DisableIRQ(irqn);

      USART_Enable(comhandle->uarthandle->peripheral.uart, usartDisable);
    }
  }
  else
  {
    /* Clear previous RX interrupts */
    LEUART_IntClear(comhandle->uarthandle->peripheral.leuart, LEUART_IF_RXDATAV);
    NVIC_ClearPendingIRQ(irqn);

    if (enable)
    {
      /* Enable RX interrupts */
      LEUART_IntEnable(comhandle->uarthandle->peripheral.leuart, LEUART_IF_RXDATAV);
      NVIC_EnableIRQ(irqn);

      LEUART_Enable(comhandle->uarthandle->peripheral.leuart, leuartEnable);
    }
    else
    {
      /* Disable RX interrupts */
      LEUART_IntDisable(comhandle->uarthandle->peripheral.leuart, LEUART_IF_RXDATAV);
      NVIC_DisableIRQ(irqn);

      LEUART_Enable(comhandle->uarthandle->peripheral.leuart, leuartDisable);
    }
  }
}

/**
 * @brief Convert a COM_Port_t into a pointer to UART handle
 *
 * @param[in] port COM port number
 *
 * @return Pointer to UART handle
 */
static UARTDRV_Handle_t getUartHandleFromPort(COM_Port_t port)
{
  size_t index;

  switch(port)
  {
#ifdef COM_USART0_ENABLE
    case COM_USART0:
    case comPortUsart0:
      index = comUartHandleIndexUsart0;
      break;
#endif
#ifdef COM_USART1_ENABLE
    case COM_USART1:
    case comPortUsart1:
      index = comUartHandleIndexUsart1;
      break;
#endif
#ifdef COM_USART2_ENABLE
    case COM_USART2:
    case comPortUsart2:
      index = comUartHandleIndexUsart2;
      break;
#endif
#ifdef COM_LEUART0_ENABLE
    case COM_LEUART0:
    case comPortLeuart0:
      index = comUartHandleIndexLeuart0;
      break;
#endif
#ifdef COM_LEUART1_ENABLE
    case COM_LEUART1:
    case comPortLeuart1:
      index = comUartHandleIndexLeuart1;
      break;
#endif
    default:
      return NULL;
  }

  return &uarthandledata[index];
}

static void rxCallback(UARTDRV_Handle_t handle,
                       Ecode_t transferStatus,
                       uint8_t *data,
                       UARTDRV_Count_t transferCount)
{
  uint16_t nextRx;
  COM_Handle_t comhandle;

  // Don't process callback if the transfer was aborted
  if (transferStatus == ECODE_EMDRV_UARTDRV_ABORTED)
  {
    return;
  }

  // Figure out which com port this is
  for (uint8_t i = 0; i < comHandleIndexSize; i++)
  {
    comhandle = &comhandledata[i];
    if (comhandle->uarthandle == handle)
    {
      // update fifo
      updateBuffer(comhandle->rxQueue, transferCount, comhandle->rxsize);
      // reset pumped bytes (already in interrupt context)
      comhandle->rxQueue->pumped = 0;
      // set up next receive operation
      // will return current number of rx operations (including current one)
      // Should set up buffers until there will be 2 after this exits
      while (UARTDRV_GetReceiveDepth(handle)<3)
      {
        nextRx = comhandle->bufferIndex * comhandle->rxStop;
        if (comhandle->rxQueue->used > 0)
        {
          // Check to see if there are used bytes in the next buffer by
          // comparing head and tail positions with the buffer start/stop
          if ( ( (comhandle->rxQueue->tail < (nextRx+comhandle->rxStop))
                 && (nextRx < comhandle->rxQueue->head))
               || ( (comhandle->rxQueue->head < comhandle->rxQueue->tail)
                    && ( ((nextRx+comhandle->rxStop) > comhandle->rxQueue->tail)
                         || (nextRx < comhandle->rxQueue->head) ) ) )
          {
            // Apply flow control since next buffer couldn't be allocated
            UARTDRV_FlowControlSet(handle, uartdrvFlowControlOff);
            break;
          }
        }
        rxNextBuffer(comhandle);
      }

    }
  }
}

/* Set up next RX operation (not used for SW flow control) */
static void rxNextBuffer(COM_Handle_t handle)
{
  UARTDRV_Receive(handle->uarthandle,
                  &handle->rxQueue->fifo[handle->bufferIndex * handle->rxStop],
                  handle->rxStop,
                  rxCallback);
  // manually ensure flow control is set after receive
  if (UARTDRV_GetReceiveDepth(handle->uarthandle)>=2)
  {
    UARTDRV_FlowControlSet(handle->uarthandle, uartdrvFlowControlOn);
  }
  handle->bufferIndex = (handle->bufferIndex + 1) % handle->bufferLimit;
}

/**
 * Set correct UARTDRV FifoQueues into init struct based on port
 */
static Ecode_t setUartBufferQueues(COM_Port_t port, COM_Init_t * init)
{
  switch(port)
  {
#ifdef COM_USART0_ENABLE
    case COM_USART0:
    case comPortUsart0:
      init->uartdrvinit.uartinit.rxQueue = (UARTDRV_Buffer_FifoQueue_t *)&comBufferQueueRxUsart0;
      init->uartdrvinit.uartinit.txQueue = (UARTDRV_Buffer_FifoQueue_t *)&comBufferQueueTxUsart0;
      break;
#endif
#ifdef COM_USART1_ENABLE
    case COM_USART1:
    case comPortUsart1:
      init->uartdrvinit.uartinit.rxQueue = (UARTDRV_Buffer_FifoQueue_t *)&comBufferQueueRxUsart1;
      init->uartdrvinit.uartinit.txQueue = (UARTDRV_Buffer_FifoQueue_t *)&comBufferQueueTxUsart1;
      break;
#endif
#ifdef COM_USART2_ENABLE
    case COM_USART2:
    case comPortUsart2:
      init->uartdrvinit.uartinit.rxQueue = (UARTDRV_Buffer_FifoQueue_t *)&comBufferQueueRxUsart2;
      init->uartdrvinit.uartinit.txQueue = (UARTDRV_Buffer_FifoQueue_t *)&comBufferQueueTxUsart2;
      break;
#endif
#ifdef COM_LEUART0_ENABLE
    case COM_LEUART0:
    case comPortLeuart0:
      init->uartdrvinit.leuartinit.rxQueue = (UARTDRV_Buffer_FifoQueue_t *)&comBufferQueueRxLeuart0;
      init->uartdrvinit.leuartinit.txQueue = (UARTDRV_Buffer_FifoQueue_t *)&comBufferQueueTxLeuart0;
      break;
#endif
#ifdef COM_LEUART1_ENABLE
    case COM_LEUART1:
    case comPortLeuart1:
      init->uartdrvinit.leuartinit.rxQueue = (UARTDRV_Buffer_FifoQueue_t *)&comBufferQueueRxLeuart1;
      init->uartdrvinit.leuartinit.txQueue = (UARTDRV_Buffer_FifoQueue_t *)&comBufferQueueTxLeuart1;
      break;
#endif
    default:
      return EMBER_ERR_FATAL;
  }
  return EMBER_SUCCESS;
}

static void txCatchUp(COM_Handle_t comhandle)
{
  if (!comhandle->txPaused)
  {
    uint8_t *fifotail;
    uint8_t length;
    uint8_t *buffer;
    uint32_t sent;
    uint32_t remaining;
    // Get current FIFO head
    fifotail= &comhandle->txQueue->fifo[comhandle->txQueue->tail];
    // Check for active transmit
    UARTDRV_GetTransmitStatus(comhandle->uarthandle, &buffer, &sent, &remaining);
    // Determine bytes to "catch up"
    length = comhandle->txQueue->used - remaining - sent;
    // wrap
    if (comhandle->txQueue->head < comhandle->txQueue->tail)
    {
      if(UARTDRV_Transmit(comhandle->uarthandle,
                             fifotail,
                             comhandle->txsize - comhandle->txQueue->tail,
                             txCallback)
            != EMBER_SUCCESS)
      {
        return;
      }
      length -= comhandle->txsize - comhandle->txQueue->tail;
      fifotail = comhandle->txQueue->fifo;
    }
    if (UARTDRV_Transmit(comhandle->uarthandle, fifotail, length, txCallback)
          == EMBER_SUCCESS)
    {
      comhandle->txCatchUp = false;
    }
  }
}

static void txCallback(UARTDRV_Handle_t handle,
                       Ecode_t transferStatus,
                       uint8_t *data,
                       UARTDRV_Count_t transferCount)
{
  COM_Handle_t comhandle;
  // Figure out which com port this is
  // 
  for (uint8_t i = 0; i < comHandleIndexSize; i++)
  {
    comhandle = &comhandledata[i];
    if (comhandle->uarthandle == handle)
    {
      dequeueFifoBuffer(comhandle, 
                        (transferCount > comhandle->txQueue->used) ?
                          comhandle->txQueue->used :
                          transferCount);
    }
  }
}
#endif //defined(COM_UART_ENABLE)

void COM_RX_IRQHandler(COM_Port_t port, uint8_t byte)
{
  UARTDRV_FlowControlState_t fcState;
  uint16_t freeSpace;
  COM_Handle_t comhandle = getComHandleFromPort(port);

  // Intercept and handle flow control bytes
  if (byte == UARTDRV_FC_SW_XON)
  {
    UARTDRV_FlowControlSetPeerStatus(comhandle->uarthandle, uartdrvFlowControlOn);
    comhandle->txPaused = false;
    // Transmit queued data in the TX FIFO queue
    txCatchUp(comhandle);
  }
  else if (byte == UARTDRV_FC_SW_XOFF)
  {
    UARTDRV_FlowControlSetPeerStatus(comhandle->uarthandle, uartdrvFlowControlOff);
    comhandle->txPaused = true;
  }
  else
  {
    // store byte in RX FIFO
    FIFO_ENQUEUE(comhandle->rxQueue, byte, comhandle->rxsize);
    // Send flow control byte if threshold exceeded
    fcState = UARTDRV_FlowControlGetSelfStatus(comhandle->uarthandle);
    freeSpace = comhandle->rxsize - comhandle->rxQueue->used;
    if ( (fcState == uartdrvFlowControlOn) && (freeSpace <= comhandle->rxStop) )
    {
      UARTDRV_FlowControlSet(comhandle->uarthandle, uartdrvFlowControlOff);
    }
    else if ( (fcState == uartdrvFlowControlOff) && (freeSpace >= comhandle->rxStart) )
    {
      UARTDRV_FlowControlSet(comhandle->uarthandle, uartdrvFlowControlOn);
    }
  }
}

#ifdef COM_USART0_ENABLE
void USART0_RX_IRQHandler(void)
{
  if (USART0->STATUS & USART_STATUS_RXDATAV)
  {
    COM_RX_IRQHandler(COM_USART0, USART_Rx(USART0));
  }
}
#endif

#ifdef COM_USART1_ENABLE
void USART1_RX_IRQHandler(void)
{
  if (USART1->STATUS & USART_STATUS_RXDATAV)
  {
    COM_RX_IRQHandler(COM_USART1, USART_Rx(USART1));
  }
}
#endif

#ifdef COM_USART2_ENABLE
void USART2_RX_IRQHandler(void)
{
  if (USART2->STATUS & USART_STATUS_RXDATAV)
  {
    COM_RX_IRQHandler(COM_USART2, USART_Rx(USART2));
  }
}
#endif

/* "power down" COM by switching from DMA to UART byte interrupts */
void COM_InternalPowerDown()
{
#if (LDMA_COUNT > 0)
  if (LDMA->IEN!=0)
  {
    dma_IEN= LDMA->IEN;
    LDMA->IEN = 0;
  }
#else
  if (DMA->IEN!=0)
  {
    dma_IEN= DMA->IEN;
    DMA->IEN = 0;
  }
#endif
  #ifdef COM_USART0_ENABLE
    NVIC_ClearPendingIRQ( USART0_RX_IRQn );
    NVIC_EnableIRQ( USART0_RX_IRQn );
    USART_IntEnable(USART0, USART_IF_RXDATAV);
  #endif
  #ifdef COM_USART1_ENABLE
    NVIC_ClearPendingIRQ( USART1_RX_IRQn );
    NVIC_EnableIRQ( USART1_RX_IRQn );
    USART_IntEnable(USART1, USART_IF_RXDATAV);
  #endif
  #ifdef COM_USART2_ENABLE
    NVIC_ClearPendingIRQ( USART2_RX_IRQn );
    NVIC_EnableIRQ( USART2_RX_IRQn );
    USART_IntEnable(USART2, USART_IF_RXDATAV);
  #endif
  rxGpioIntEnable();
}

/* "power up" COM by switching back to DMA interrupts */
void COM_InternalPowerUp()
{
  #ifdef COM_USART0_ENABLE
    USART_IntClear(USART0, USART_IF_RXDATAV);
    USART_IntDisable(USART0, USART_IF_RXDATAV);
    NVIC_ClearPendingIRQ( USART0_RX_IRQn );
    NVIC_DisableIRQ( USART0_RX_IRQn );
  #endif
  #ifdef COM_USART1_ENABLE
    USART_IntClear(USART1, USART_IF_RXDATAV);
    USART_IntDisable(USART1, USART_IF_RXDATAV);
    NVIC_ClearPendingIRQ( USART1_RX_IRQn );
    NVIC_DisableIRQ( USART1_RX_IRQn );
  #endif
  #ifdef COM_USART2_ENABLE
    USART_IntClear(USART2, USART_IF_RXDATAV);
    USART_IntDisable(USART2, USART_IF_RXDATAV);
    NVIC_ClearPendingIRQ( USART2_RX_IRQn );
    NVIC_DisableIRQ( USART2_RX_IRQn );
  #endif
  if (dma_IEN!=0)
  {
#if (LDMA_COUNT > 0)
    LDMA->IEN = dma_IEN;
#else
    DMA->IEN = dma_IEN;
#endif
  }
  rxGpioIntDisable();
}

/* Inject data into the RX FIFO */
Ecode_t COM_InternalReceiveData(COM_Port_t port, uint8_t *data, uint32_t length)
{
  COM_Handle_t comhandle = getComHandleFromPort(port);
  // Check for space for this message in the FIFO
  if (!getInputFifoSpace(comhandle, length))
  {
    return EMBER_ERR_FATAL;
  }

  for (uint32_t i = 0; i < length; i++)
  {
    CORE_DECLARE_IRQ_STATE;
    CORE_ENTER_ATOMIC();
    FIFO_ENQUEUE(comhandle->rxQueue, *data, comhandle->rxsize);
    CORE_EXIT_ATOMIC();
    data++;
  }

  return EMBER_SUCCESS;
}

bool COM_InternalRxIsPaused(COM_Port_t port)
{
  // UART
#if defined(COM_UART_ENABLE)
  if (checkValidUartPort(port))
  {
    COM_Handle_t comhandle = getComHandleFromPort(port);
    return (UARTDRV_FlowControlGetSelfStatus(comhandle->uarthandle) == uartdrvFlowControlOff);
  }
#endif
  return false;
}

bool COM_InternalTxIsIdle(COM_Port_t port)
{
  // UART
#if defined(COM_UART_ENABLE)
  if (checkValidUartPort(port))
  {
    COM_Handle_t comhandle = getComHandleFromPort(port);
    return (UARTDRV_GetPeripheralStatus(comhandle->uarthandle) & UARTDRV_STATUS_TXIDLE) ? true : false;
  }
#endif
  return false;
}

bool COM_InternalTxIsPaused(COM_Port_t port)
{
  COM_Handle_t comhandle = getComHandleFromPort(port);
  return comhandle->txPaused;
}

//------------------------------------------------------
// Serial initialization

Ecode_t COM_Init(COM_Port_t port, COM_Init_t *init)
{
  if (checkValidPort(port)==false)
  {
    return EMBER_ERR_FATAL;
  }
  if (!COM_Unused(port))
  {
    // Port already configured; reinitialize
    COM_DeInit(port);
  }

  COM_Handle_t comhandle = getComHandleFromPort(port);

  if (setComHandleQueues(port) != EMBER_SUCCESS)
  {
    return EMBER_ERR_FATAL;
  }

  // VCP
#if (COM_VCP_PORTS > 0) && defined(COM_VCP_ENABLE)
  if (checkValidVcpPort(port))
  {
    return emDebugInit();
  }
#endif

  // UART
#if defined(COM_UART_ENABLE)
  if (checkValidUartPort(port))
  {
    Ecode_t status;

    comhandle->uarthandle = getUartHandleFromPort(port);

    // add rx/tx buffer queue to initdata
    if (setUartBufferQueues(port, init) != EMBER_SUCCESS)
    {
      return EMBER_ERR_FATAL;
    }

    // COM will issue an XOFF when free buffer space <= rxStop bytes and an XON
    // when fres buffer space >= rxStart bytes.
    // Stop threshold must be lower than the start threshold or else it's
    // unclear whether or not to start/stop when buffer is in between.
    if (init->rxStart < init->rxStop)
    {
      // Reset COM handle
      memset (comhandle, 0, sizeof(COM_HandleData_t));
      return EMBER_BAD_ARGUMENT;
    }
    // store RX flow control thresholds
    comhandle->rxStop = init->rxStop;
    comhandle->rxStart = init->rxStart;

    // iniitalize hardware
    if (checkValidLeuartPort(port))
    {
      status = UARTDRV_InitLeuart(comhandle->uarthandle, &init->uartdrvinit.leuartinit);
    }
    else //USART
    {
      status = UARTDRV_InitUart(comhandle->uarthandle, &init->uartdrvinit.uartinit);
    }
    if (status != EMBER_SUCCESS)
    {
      return status;
    }
    GPIO_PinModeSet(comhandle->uarthandle->rxPort,
                comhandle->uarthandle->rxPin,
                gpioModeInputPull,
                1);

    if ((checkValidLeuartPort(port) &&
         (init->uartdrvinit.leuartinit.fcType == uartdrvFlowControlSw) ) ||
        (!checkValidLeuartPort(port) &&
         (init->uartdrvinit.uartinit.fcType == uartdrvFlowControlSw) ) )
    {
      enableRxIrq(port, true);
      // begin by sending XON
      UARTDRV_FlowControlSet(comhandle->uarthandle, uartdrvFlowControlOn);
    }
    else
    {
      comhandle->bufferIndex = 0;
      comhandle->bufferLimit = comhandle->rxsize/comhandle->rxStop;

      // start ping pong buffers for FIFO
      rxNextBuffer(comhandle);
      rxNextBuffer(comhandle);
    }

    #ifndef ENABLE_EXP_UART
      halEnableVCOM();
    #endif
  }
#endif // COM_UART_ENABLE
  rxGpioIntInit();

  return EMBER_SUCCESS;
}

Ecode_t COM_DeInit(COM_Port_t port)
{
  COM_Handle_t comhandle;
  Ecode_t status;

  if (checkValidPort(port)==false)
  {
    return EMBER_ERR_FATAL;
  }
  if (COM_Unused(port))
  {
    return EMBER_SUCCESS;
  }

  COM_FlushRx(port);

  comhandle = getComHandleFromPort(port);

#if defined(COM_UART_ENABLE)
  if (checkValidUartPort(port))
  {
    status = UARTDRV_DeInit(comhandle->uarthandle);
    if (status != ECODE_EMDRV_UARTDRV_OK)
    {
      return status;
    }

    if (comhandle->uarthandle->fcType == uartdrvFlowControlSw)
    {
      enableRxIrq(port, false);
    }
    rxGpioIntDisable();
  }
#endif
  memset (comhandle, 0, sizeof(COM_HandleData_t));
  return EMBER_SUCCESS;
}

//------------------------------------------------------
// Serial Input

// returns # bytes available for reading
uint16_t COM_ReadAvailable(COM_Port_t port)
{
  COM_Handle_t comhandle = getComHandleFromPort(port);
  // make sure rx buffer is updated
  pumpRx(port);
  return comhandle->rxQueue->used;
}

Ecode_t COM_ReadByte(COM_Port_t port, uint8_t *dataByte)
{
  uint16_t nextRx;
  if (checkValidPort(port)==false)
  {
    return EMBER_ERR_FATAL;
  }
  COM_Handle_t comhandle = getComHandleFromPort(port);
  // make sure rx buffer is updated
  pumpRx(port);
  if (comhandle->rxQueue->used > 0) {
    CORE_DECLARE_IRQ_STATE;
    CORE_ENTER_ATOMIC();
      *dataByte = FIFO_DEQUEUE(comhandle->rxQueue, comhandle->rxsize);

      #ifdef COM_UART_ENABLE
      if (checkValidUartPort(port) &&
          comhandle->uarthandle->fcType != uartdrvFlowControlSw)
      {
        while (UARTDRV_GetReceiveDepth(comhandle->uarthandle)<2)
        {
          nextRx = comhandle->bufferIndex * comhandle->rxStop;
          if ( ( (comhandle->rxQueue->tail < (nextRx+comhandle->rxStop))
                 && (nextRx < comhandle->rxQueue->head))
               || ( (comhandle->rxQueue->head < comhandle->rxQueue->tail)
                    && ( ((nextRx+comhandle->rxStop) > comhandle->rxQueue->tail)
                         || (nextRx < comhandle->rxQueue->head) ) ) )
          {
            break;
          }
          rxNextBuffer(comhandle);
        }
      }
      #endif //COM_UART_ENABLE
    CORE_EXIT_ATOMIC();
    return EMBER_SUCCESS;
  }
  return EMBER_SERIAL_RX_EMPTY;
}

Ecode_t COM_ReadData(COM_Port_t port,
                                uint8_t *data,
                                uint16_t length,
                                uint16_t *bytesRead)
{
  if (checkValidPort(port)==false)
  {
    return EMBER_ERR_FATAL;
  }
  uint16_t bytesReadInternal = 0;
  Ecode_t status;

  while (bytesReadInternal < length) {
    status = COM_ReadByte(port, data);

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
Ecode_t COM_ReadDataTimeout(COM_Port_t port,
                                       uint8_t *data,
                                       uint16_t length,
                                       uint16_t *bytesRead,
                                       uint16_t firstByteTimeout,
                                       uint16_t subsequentByteTimeout)
{
  if (checkValidPort(port)==false)
  {
    return EMBER_ERR_FATAL;
  }
  uint16_t bytesReadInternal = 0;
  Ecode_t status;
  uint16_t timeout = firstByteTimeout;
  uint16_t startTime = halCommonGetInt16uMillisecondTick();

  // loop until we read the max number of bytes or the timeout elapses
  while (bytesReadInternal < length
         && elapsedTimeInt16u(startTime, halCommonGetInt16uMillisecondTick()) < timeout) {
    status = COM_ReadByte(port, data);

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

Ecode_t COM_ReadPartialLine(COM_Port_t port, char *data, uint8_t max, uint8_t * index)
{
  if (checkValidPort(port)==false)
  {
    return EMBER_ERR_FATAL;
  }
  Ecode_t err;
  uint8_t ch;

  if (((*index) == 0) || ((*index) >= max))
    data[0] = '\0';

  for (;;) {
    err = COM_ReadByte(port, &ch);

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
        COM_WriteString(port, "\b \b");
      }
      // don't add or process this character
      continue;
    }

    //if the string is about to overflow, fake in a CR
    if ( (*index) + 2 > max ) {
      ch = '\r';
    }

    COM_WriteByte(port, ch); // term char echo

    //upcase that char
    if ( ch>='a' && ch<='z') ch = ch - ('a'-'A');

    // build a string until we press enter
    if ( ( ch == '\r' ) || ( ch == '\n' ) ) {
      data[*index] = '\0';

      if (ch == '\r') {
        COM_WriteByte(port, '\n'); // "append" LF
        *index = 0;                       // Reset for next line; \n next
      } else {
        COM_WriteByte(port, '\r'); // "append" CR
        *index = max;                     // Reset for next line; \r,\0 next
      }

      return EMBER_SUCCESS;
    }

    data[(*index)++] = ch;
  }
}

Ecode_t COM_ReadLine(COM_Port_t port, char *data, uint8_t max)
{
  if (checkValidPort(port)==false)
  {
    return EMBER_ERR_FATAL;
  }
  uint8_t index=0;

  while(COM_ReadPartialLine(port, data, max, &index) != EMBER_SUCCESS) {
    halResetWatchdog();
  }
  return EMBER_SUCCESS;
}

//------------------------------------------------------
// Serial Output

// returns # bytes (if fifo mode)/messages (if buffer mode) that can be written
uint16_t COM_WriteAvailable(COM_Port_t port)
{
  COM_Handle_t comhandle = getComHandleFromPort(port);
  return comhandle->txsize - comhandle->txQueue->used;
}

uint16_t COM_WriteUsed(COM_Port_t port)
{
  COM_Handle_t comhandle = getComHandleFromPort(port);
  return comhandle->txQueue->used;
}

Ecode_t COM_WriteByte(COM_Port_t port, uint8_t dataByte)
{
  if (checkValidPort(port)==false)
  {
    return EMBER_ERR_FATAL;
  }
  return COM_WriteData(port, &dataByte, 1);
}

Ecode_t COM_WriteHex(COM_Port_t port, uint8_t dataByte)
{
  if (checkValidPort(port)==false)
  {
    return EMBER_ERR_FATAL;
  }
  uint8_t hex[2];
  emWriteHexInternal(hex, dataByte, 2);
  return COM_WriteData(port, hex, 2);
}

Ecode_t COM_WriteString(COM_Port_t port, PGM_P string)
{
  if (checkValidPort(port)==false)
  {
    return EMBER_ERR_FATAL;
  }
#ifdef COM_VCP_ENABLE
  if (checkValidVcpPort(port))
  {
    emDebugSendVuartMessage((uint8_t*)string, strlen(string));
    return EMBER_SUCCESS;
  }
#endif //COM_VCP_ENABLE
  COM_Handle_t comhandle = getComHandleFromPort(port);
  uint8_t *fifohead= &comhandle->txQueue->fifo[comhandle->txQueue->head];
  uint8_t length = 0;
  uint8_t wraplength = 0;
  uint8_t txlength = 0;
  while(*string != '\0') {
    while (! getOutputFifoSpace(comhandle, 0)) 
    {
      if (comhandle->txPaused)
      {
        return EMBER_SERIAL_TX_OVERFLOW;
      }
    };
    CORE_DECLARE_IRQ_STATE;
    CORE_ENTER_ATOMIC();
    FIFO_ENQUEUE(comhandle->txQueue,*string,comhandle->txsize);
    CORE_EXIT_ATOMIC();
    string++;
    length++;
    // queue just wrapped
    if (comhandle->txQueue->head == 0)
    {
      // store first transmit length
      txlength = length - wraplength;
      // transmit chunk
      txBuffer(port, fifohead, txlength);
      wraplength += txlength;
      // move fifohead back to start
      fifohead = comhandle->txQueue->fifo;
    }
  }

  if ( length > wraplength)
  {
    txBuffer(port, fifohead, length - wraplength);
  }
  return EMBER_SUCCESS;
}

Ecode_t COM_PrintCarriageReturn(COM_Port_t port)
{
  if (checkValidPort(port)==false)
  {
    return EMBER_ERR_FATAL;
  }
  return COM_Printf(port, "\r\n");
}

Ecode_t COM_PrintfVarArg(COM_Port_t port, PGM_P formatString, va_list ap)
{
  if (checkValidPort(port)==false)
  {
    return EMBER_ERR_FATAL;
  }
  Ecode_t stat = EMBER_SUCCESS;
  if(!emPrintfInternal(COM_WriteData, port, formatString, ap))
    stat = EMBER_ERR_FATAL;
  return stat;
}

Ecode_t COM_Printf(COM_Port_t port, PGM_P formatString, ...)
{
  if (checkValidPort(port)==false)
  {
    return EMBER_ERR_FATAL;
  }
  Ecode_t stat;
  va_list ap;
  va_start (ap, formatString);
  stat = COM_PrintfVarArg(port, formatString, ap);
  va_end (ap);
  return stat;
}

Ecode_t COM_PrintfLine(COM_Port_t port, PGM_P formatString, ...)
{
  if (checkValidPort(port)==false)
  {
    return EMBER_ERR_FATAL;
  }
  Ecode_t stat;
  va_list ap;
  va_start (ap, formatString);
  stat = COM_PrintfVarArg(port, formatString, ap);
  va_end (ap);
  COM_PrintCarriageReturn(port);
  return stat;
}

Ecode_t COM_WriteData(COM_Port_t port, uint8_t *data, uint8_t length)
{
  if (checkValidPort(port)==false)
  {
    return EMBER_ERR_FATAL;
  }
#ifdef COM_VCP_ENABLE
  if (checkValidVcpPort(port))
  {
    emDebugSendVuartMessage(data, length);
    return EMBER_SUCCESS;
  }
#endif //COM_VCP_ENABLE
  COM_Handle_t comhandle = getComHandleFromPort(port);

  uint8_t *fifohead= &comhandle->txQueue->fifo[comhandle->txQueue->head];
  uint8_t wraplength = 0;
  uint8_t txlength = 0;
  bool wrap = false;
  for (uint8_t i =0; i<length; i++) {
    while (! getOutputFifoSpace(comhandle, 0)) 
    {
      if (comhandle->txPaused)
      {
        return EMBER_SERIAL_TX_OVERFLOW;
      }
    };
    CORE_DECLARE_IRQ_STATE;
    CORE_ENTER_ATOMIC();
    FIFO_ENQUEUE(comhandle->txQueue,*data,comhandle->txsize);
    wrap = comhandle->txQueue->head == 0 ? true : false;
    CORE_EXIT_ATOMIC();
    data++;
    // queue just wrapped
    if (wrap)
    {
      // store first transmit length
      txlength = i + 1 - wraplength;
      // transmit chunk
      txBuffer(port, fifohead, txlength);
      wraplength+=txlength;
      // move fifohead back to start
      fifohead = comhandle->txQueue->fifo;
    }
  }
  if (length>wraplength)
  {
    txBuffer(port, fifohead, length - wraplength);
  }
  return EMBER_SUCCESS;
}

Ecode_t COM_ForceWriteData(COM_Port_t port, uint8_t *data, uint8_t length)
{
  Ecode_t status = EMBER_ERR_FATAL;
  if (checkValidPort(port)==false)
  {
    return status;
  }
#ifdef COM_VCP_ENABLE
  if (checkValidVcpPort(port))
  {
    emDebugSendVuartMessage(data, length);
    status = EMBER_SUCCESS;
  }
#endif //COM_VCP_ENABLE
#if defined(COM_UART_ENABLE)
  COM_Handle_t comhandle = getComHandleFromPort(port);
  if (checkValidUartPort(port))
  {
    status = UARTDRV_ForceTransmit(comhandle->uarthandle, data, length);
  }
#endif
  return status;
}

Ecode_t COM_GuaranteedPrintf(COM_Port_t port, PGM_P formatString, ...)
{
  if (checkValidPort(port)==false)
  {
    return EMBER_ERR_FATAL;
  }
  Ecode_t stat = EMBER_SUCCESS;
  va_list ap;
  va_start (ap, formatString);
  if(!emPrintfInternal(COM_ForceWriteData, port, formatString, ap))
    stat = EMBER_ERR_FATAL;
  va_end (ap);
  return stat;
}

Ecode_t COM_WaitSend(COM_Port_t port)
{
  if (checkValidPort(port)==false)
  {
    return EMBER_ERR_FATAL;
  }
  COM_Handle_t comhandle = getComHandleFromPort(port);

#if defined(COM_VCP_ENABLE)
  if (checkValidVcpPort(port))
  {
    while (comhandle->txQueue->used>0);
  }
#endif
#if defined(COM_UART_ENABLE)
  if (checkValidUartPort(port))
  {
    while ( (comhandle->txQueue->used > 0)
            || (UARTDRV_GetTransmitDepth(comhandle->uarthandle) > 0)
            || !( (UARTDRV_GetPeripheralStatus(comhandle->uarthandle) & UARTDRV_STATUS_TXC)
                  && (UARTDRV_GetPeripheralStatus(comhandle->uarthandle) & UARTDRV_STATUS_TXIDLE)
                ) );
  }
#endif

  return EMBER_SUCCESS;
}

//------------------------------------------------------
// Serial buffer maintenance
void COM_FlushRx(COM_Port_t port)
{
  COM_Handle_t comhandle = getComHandleFromPort(port);
  COM_FifoQueue_t *q = comhandle->rxQueue;

  if (comhandle->uarthandle->fcType != uartdrvFlowControlSw)
  {
    UARTDRV_Abort(comhandle->uarthandle, uartdrvAbortReceive);

    // reset buffer index
    comhandle->bufferIndex = 0;

    // restart buffers for FIFO
    rxNextBuffer(comhandle);
    rxNextBuffer(comhandle);
  }

  ATOMIC_LITE(
     q->used = 0;
     q->head = 0;
     q->tail = 0;
     q->pumped = 0;
   )
}

bool COM_Unused(uint8_t port)
{
  if(checkValidPort(port) == false)
  {
    return true; 
  }

  COM_Handle_t comhandle = getComHandleFromPort((COM_Port_t) port);
  if (comhandle == NULL)
  {
    return true;
  }
  // use rxQueue as a proxy for a COM port being initialized. Will be a null 
  // pointer if uninitialized
  return !((bool) comhandle->rxQueue);
}
