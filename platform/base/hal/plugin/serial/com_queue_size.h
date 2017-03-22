/***************************************************************************//**
 * @file com_queue_size.h
 * @brief Default definition for COM queue sizes
 * @version 0.1.0
 *******************************************************************************
 * @section License
 * <b>Copyright 2016 Silicon Laboratories, Inc, http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#if !defined(COM_QUEUE_SIZE_H)
#define COM_QUEUE_SIZE_H

// --------------------------------
// Legacy queue size defines

#if defined(COM_0_RX_QUEUE_SIZE)
  #define COM_VCP_RX_QUEUE_SIZE COM_0_RX_QUEUE_SIZE
#endif

#if defined(COM_0_TX_QUEUE_SIZE)
  #define COM_VCP_TX_QUEUE_SIZE COM_0_TX_QUEUE_SIZE
#endif

#if defined(COM_1_RX_QUEUE_SIZE)
  #define COM_USART0_RX_QUEUE_SIZE COM_1_RX_QUEUE_SIZE
#endif

#if defined(COM_1_TX_QUEUE_SIZE)
  #define COM_USART0_TX_QUEUE_SIZE COM_1_TX_QUEUE_SIZE
#endif

#if defined(COM_2_RX_QUEUE_SIZE)
  #define COM_USART1_RX_QUEUE_SIZE COM_2_RX_QUEUE_SIZE
#endif

#if defined(COM_2_TX_QUEUE_SIZE)
  #define COM_USART1_TX_QUEUE_SIZE COM_2_TX_QUEUE_SIZE
#endif

#if defined(COM_3_RX_QUEUE_SIZE)
  #define COM_USB_RX_QUEUE_SIZE COM_3_RX_QUEUE_SIZE
#endif

#if defined(COM_3_TX_QUEUE_SIZE)
  #define COM_USB_TX_QUEUE_SIZE COM_3_TX_QUEUE_SIZE
#endif

#if defined(COM_4_RX_QUEUE_SIZE)
  #define COM_LEUART0_RX_QUEUE_SIZE COM_4_RX_QUEUE_SIZE
#endif

#if defined(COM_4_TX_QUEUE_SIZE)
  #define COM_LEUART0_TX_QUEUE_SIZE COM_4_TX_QUEUE_SIZE
#endif

#if defined(COM_5_RX_QUEUE_SIZE)
  #define COM_LEUART1_RX_QUEUE_SIZE COM_5_RX_QUEUE_SIZE
#endif

#if defined(COM_5_TX_QUEUE_SIZE)
  #define COM_LEUART1_TX_QUEUE_SIZE COM_5_TX_QUEUE_SIZE
#endif

// --------------------------------
// Default queue sizes

#if !defined(COM_VCP_RX_QUEUE_SIZE)
  #define COM_VCP_RX_QUEUE_SIZE 64
#endif

#if !defined(COM_VCP_TX_QUEUE_SIZE)
  #define COM_VCP_TX_QUEUE_SIZE 128
#endif

#if !defined(COM_USART0_RX_QUEUE_SIZE)
  #define COM_USART0_RX_QUEUE_SIZE 64
#endif

#if !defined(COM_USART0_TX_QUEUE_SIZE)
  #define COM_USART0_TX_QUEUE_SIZE 128
#endif

#if !defined(COM_USART1_RX_QUEUE_SIZE)
  #define COM_USART1_RX_QUEUE_SIZE 64
#endif

#if !defined(COM_USART1_TX_QUEUE_SIZE)
  #define COM_USART1_TX_QUEUE_SIZE 128
#endif

#if !defined(COM_USART2_RX_QUEUE_SIZE)
  #define COM_USART2_RX_QUEUE_SIZE 64
#endif

#if !defined(COM_USART2_TX_QUEUE_SIZE)
  #define COM_USART2_TX_QUEUE_SIZE 128
#endif

#if !defined(COM_USB_RX_QUEUE_SIZE)
  #define COM_USB_RX_QUEUE_SIZE 64
#endif

#if !defined(COM_USB_TX_QUEUE_SIZE)
  #define COM_USB_TX_QUEUE_SIZE 128
#endif

#if !defined(COM_LEUART0_RX_QUEUE_SIZE)
  #define COM_LEUART0_RX_QUEUE_SIZE 64
#endif

#if !defined(COM_LEUART0_TX_QUEUE_SIZE)
  #define COM_LEUART0_TX_QUEUE_SIZE 128
#endif

#if !defined(COM_LEUART1_RX_QUEUE_SIZE)
  #define COM_LEUART1_RX_QUEUE_SIZE 64
#endif

#if !defined(COM_LEUART1_TX_QUEUE_SIZE)
  #define COM_LEUART1_TX_QUEUE_SIZE 128
#endif

#endif // COM_QUEUE_SIZE_H
