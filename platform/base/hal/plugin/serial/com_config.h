/***************************************************************************//**
 * @file com_config.h
 * @brief Default configuration for COM peripherals
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

#if !defined(COM_CONFIG_H)
#define COM_CONFIG_H

#if defined(COM_RETARGET_SERIAL)
  #include "retargetserialconfig.h"
#endif

#ifdef COM_USART0_ENABLE

  #if defined( _USART_ROUTELOC0_MASK )
  #ifdef RETARGET_USART0
    #define USART0_TX_LOCATION RETARGET_TX_LOCATION
    #define USART0_RX_LOCATION RETARGET_RX_LOCATION
    #ifdef RETARGET_CTS_PORT
      #define USART0_CTS_PORT RETARGET_CTS_PORT
      #define USART0_CTS_PIN  RETARGET_CTS_PIN
    #endif
    #ifdef RETARGET_RTS_PORT
      #define USART0_RTS_PORT RETARGET_RTS_PORT
      #define USART0_RTS_PIN  RETARGET_RTS_PIN
    #endif
  #endif /*RETARGET_USART0*/

  #ifndef USART0_TX_LOCATION
    #define USART0_TX_LOCATION _USART_ROUTELOC0_TXLOC_LOC1
  #endif
  #ifndef USART0_RX_LOCATION
    #define USART0_RX_LOCATION _USART_ROUTELOC0_RXLOC_LOC31
  #endif
  #ifndef USART0_CTS_PORT
    #define USART0_CTS_PORT gpioPortA
  #endif
  #ifndef USART0_CTS_PIN
    #define USART0_CTS_PIN 2
  #endif
  #ifndef USART0_RTS_PORT
    #define USART0_RTS_PORT gpioPortA
  #endif
  #ifndef USART0_RTS_PIN
    #define USART0_RTS_PIN 3
  #endif

  #ifndef USART0_CTS_LOCATION
    #define USART0_CTS_LOCATION _USART_ROUTELOC1_CTSLOC_LOC30
  #endif

  #ifndef USART0_RTS_LOCATION
    #define USART0_RTS_LOCATION _USART_ROUTELOC1_RTSLOC_LOC30
  #endif

  #ifdef COM_USART0_HW_UART_FC
    #define USART0_FC_MODE uartdrvFlowControlHwUart
  #elif defined(COM_USART0_HW_FC) || defined(COM_USART0_HW_GPIO_FC)
    #define USART0_FC_MODE uartdrvFlowControlHw
  #elif defined (COM_USART0_SW_FC)
    #define USART0_FC_MODE uartdrvFlowControlSw
  #else
    #define USART0_FC_MODE uartdrvFlowControlNone
  #endif

  #define USART0_INIT                                                                                         \
  {                                                                                                               \
    USART0,                                                               /* USART port                   */      \
    115200,                                                               /* Baud rate                    */      \
    USART0_TX_LOCATION,                                                   /* USART Tx pin location number */      \
    USART0_RX_LOCATION,                                                   /* USART Rx pin location number */      \
    (USART_Stopbits_TypeDef)USART_FRAME_STOPBITS_ONE,                     /* Stop bits                    */      \
    (USART_Parity_TypeDef)USART_FRAME_PARITY_NONE,                        /* Parity                       */      \
    (USART_OVS_TypeDef)USART_CTRL_OVS_X16,                                /* Oversampling mode            */      \
    false,                                                                /* Majority vote disable        */      \
    USART0_FC_MODE,                                                       /* Flow control                 */      \
    USART0_CTS_PORT,                                                      /* CTS port number              */      \
    USART0_CTS_PIN,                                                       /* CTS pin number               */      \
    USART0_RTS_PORT,                                                      /* RTS port number              */      \
    USART0_RTS_PIN,                                                       /* RTS pin number               */      \
    NULL,                                                                 /* RX operation queue           */      \
    NULL,                                                                 /* TX operation queue           */      \
    USART0_CTS_LOCATION,                                                  /* CTS pin location             */      \
    USART0_RTS_LOCATION                                                   /* RTS pin location             */      \
  }
  #else //defined( _USART_ROUTELOC0_MASK )

  #if (RETARGET_UART == USART0)
    #define USART0_ROUTE_LOCATION BSP_BCC_LOCATION
  #else /*(BSP_BCC_USART == USART0)*/
    #define USART0_ROUTE_LOCATION _USART_ROUTE_LOCATION_LOC1
  #endif /*(BSP_BCC_USART == USART0)*/

  #define USART0_INIT                                                                                         \
  {                                                                                                               \
    USART0,                                                               /* USART port                   */      \
    115200,                                                               /* Baud rate                    */      \
    USART0_ROUTE_LOCATION,                                                /* USART pins location number   */      \
    (USART_Stopbits_TypeDef)USART_FRAME_STOPBITS_ONE,                     /* Stop bits                    */      \
    (USART_Parity_TypeDef)USART_FRAME_PARITY_NONE,                        /* Parity                       */      \
    (USART_OVS_TypeDef)USART_CTRL_OVS_X16,                                /* Oversampling mode            */      \
    false,                                                                /* Majority vote disable        */      \
    USART0_FC_MODE,                                                       /* Flow control                 */      \
    (GPIO_Port_TypeDef)AF_USART0_CS_PORT(USART0_ROUTE_LOCATION),          /* CTS port number              */      \
    AF_USART0_CS_PIN(USART0_ROUTE_LOCATION),                              /* CTS pin number               */      \
    (GPIO_Port_TypeDef)AF_USART0_CLK_PORT(USART0_ROUTE_LOCATION),         /* RTS port number              */      \
    AF_USART0_CLK_PIN(USART0_ROUTE_LOCATION),                             /* RTS pin number               */      \
    NULL,                                                                 /* RX operation queue           */      \
    NULL                                                                  /* TX operation queue           */      \
  }

  #endif //defined( _USART_ROUTELOC0_MASK )

  #ifndef COM_USART0_RXSTOP
    #define COM_USART0_RXSTOP 16
  #endif
  #ifndef COM_USART0_RXSTART
    #define COM_USART0_RXSTART COM_USART0_RXSTOP
  #endif
  // Start threshold and size must be multiples of the stop threshold if using
  // hardware flow control.
  #ifdef COM_USART0_HW_FC
    #if COM_USART0_RX_QUEUE_SIZE % COM_USART0_RXSTOP != 0
      #error RX queue size must be a multiple of RX stop
    #endif
    #if COM_USART0_RXSTART % COM_USART0_RXSTOP != 0
      #error RX start must be a multiple of RX stop
    #endif
  #endif

  #define COM_USART0_DEFAULT                                                   \
  {                                                                            \
    (UARTDRV_Init_t) USART0_INIT,                  /* USART initdata        */ \
    COM_USART0_RXSTOP,                             /* RX stop threshold     */ \
    COM_USART0_RXSTART                             /* RX start threshold    */ \
  }
#endif //COM_USART0_ENABLE
#ifdef COM_USART1_ENABLE
  #if defined( _USART_ROUTELOC0_MASK )

  #ifdef RETARGET_USART1
    #define USART1_TX_LOCATION RETARGET_TX_LOCATION
    #define USART1_RX_LOCATION RETARGET_RX_LOCATION
    #ifdef RETARGET_CTS_PORT
      #define USART1_CTS_PORT RETARGET_CTS_PORT
      #define USART1_CTS_PIN  RETARGET_CTS_PIN
    #endif
    #ifdef RETARGET_RTS_PORT
      #define USART1_RTS_PORT RETARGET_RTS_PORT
      #define USART1_RTS_PIN  RETARGET_RTS_PIN
    #endif
  #endif /*RETARGET_USART1*/

  #ifndef USART1_TX_LOCATION
    #define USART1_TX_LOCATION _USART_ROUTELOC0_TXLOC_LOC3
  #endif
  #ifndef USART1_RX_LOCATION
    #define USART1_RX_LOCATION _USART_ROUTELOC0_RXLOC_LOC1
  #endif
  #ifndef USART1_CTS_PORT
    #define USART1_CTS_PORT gpioPortD
  #endif
  #ifndef USART1_CTS_PIN
    #define USART1_CTS_PIN 12
  #endif
  #ifndef USART1_RTS_PORT
    #define USART1_RTS_PORT gpioPortD
  #endif
  #ifndef USART1_RTS_PIN
    #define USART1_RTS_PIN 11
  #endif

  #ifdef COM_USART1_HW_UART_FC
    #define USART1_FC_MODE uartdrvFlowControlHwUart
  #elif defined(COM_USART1_HW_FC) || defined(COM_USART1_HW_GPIO_FC)
    #define USART1_FC_MODE uartdrvFlowControlHw
  #elif defined (COM_USART1_SW_FC)
    #define USART1_FC_MODE uartdrvFlowControlSw
  #else
    #define USART1_FC_MODE uartdrvFlowControlNone
  #endif

  #define USART1_INIT                                                                                         \
  {                                                                                                               \
    USART1,                                                               /* USART port                   */      \
    115200,                                                               /* Baud rate                    */      \
    USART1_TX_LOCATION,                                                   /* USART Tx pin location number */      \
    USART1_RX_LOCATION,                                                   /* USART Rx pin location number */      \
    (USART_Stopbits_TypeDef)USART_FRAME_STOPBITS_ONE,                     /* Stop bits                    */      \
    (USART_Parity_TypeDef)USART_FRAME_PARITY_NONE,                        /* Parity                       */      \
    (USART_OVS_TypeDef)USART_CTRL_OVS_X16,                                /* Oversampling mode            */      \
    false,                                                                /* Majority vote disable        */      \
    USART1_FC_MODE,                                                       /* Flow control                 */      \
    USART1_CTS_PORT,                                                      /* CTS port number              */      \
    USART1_CTS_PIN,                                                       /* CTS pin number               */      \
    USART1_RTS_PORT,                                                      /* RTS port number              */      \
    USART1_RTS_PIN,                                                       /* RTS pin number               */      \
    NULL,                                                                 /* RX operation queue           */      \
    NULL                                                                  /* TX operation queue           */      \
  }
  #else //defined( _USART_ROUTELOC0_MASK )

  #define USART1_INIT                                                                                         \
  {                                                                                                               \
    USART1,                                                               /* USART port                   */      \
    115200,                                                               /* Baud rate                    */      \
    _USART_ROUTE_LOCATION_LOC1,                                           /* USART pins location number   */      \
    (USART_Stopbits_TypeDef)USART_FRAME_STOPBITS_ONE,                     /* Stop bits                    */      \
    (USART_Parity_TypeDef)USART_FRAME_PARITY_NONE,                        /* Parity                       */      \
    (USART_OVS_TypeDef)USART_CTRL_OVS_X16,                                /* Oversampling mode            */      \
    false,                                                                /* Majority vote disable        */      \
    USART1_FC_MODE,                                                       /* Flow control                 */      \
    (GPIO_Port_TypeDef)AF_USART1_CS_PORT(_USART_ROUTE_LOCATION_LOC1),     /* CTS port number              */      \
    AF_USART1_CS_PIN(_USART_ROUTE_LOCATION_LOC1),                         /* CTS pin number               */      \
    (GPIO_Port_TypeDef)AF_USART1_CLK_PORT(_USART_ROUTE_LOCATION_LOC1),    /* RTS port number              */      \
    AF_USART1_CLK_PIN(_USART_ROUTE_LOCATION_LOC1),                        /* RTS pin number               */      \
    NULL,                                                                 /* RX operation queue           */      \
    NULL                                                                  /* TX operation queue           */      \
  }
  #endif //defined( _USART_ROUTELOC0_MASK )

  #ifndef COM_USART1_RXSTOP
    #define COM_USART1_RXSTOP 16
  #endif
  #ifndef COM_USART1_RXSTART
    #define COM_USART1_RXSTART COM_USART1_RXSTOP
  #endif
  // Start threshold and size must be multiples of the stop threshold if using
  // hardware flow control.
  #ifdef COM_USART1_HW_FC
    #if COM_USART1_RX_QUEUE_SIZE % COM_USART1_RXSTOP != 0
      #error RX queue size must be a multiple of RX stop
    #endif
    #if COM_USART1_RXSTART % COM_USART1_RXSTOP != 0
      #error RX start must be a multiple of RX stop
    #endif
  #endif

  #define COM_USART1_DEFAULT                                                   \
  {                                                                            \
    (UARTDRV_Init_t) USART1_INIT,                  /* USART initdata        */ \
    COM_USART1_RXSTOP,                             /* RX stop threshold     */ \
    COM_USART1_RXSTART                             /* RX start threshold    */ \
  }
#endif //COM_USART1_ENABLE

#ifdef COM_USART2_ENABLE

  #define USART2_INIT                                                                                         \
  {                                                                                                               \
    USART2,                                                               /* USART port                   */      \
    115200,                                                               /* Baud rate                    */      \
    _USART_ROUTE_LOCATION_LOC1,                                           /* USART pins location number   */      \
    (USART_Stopbits_TypeDef)USART_FRAME_STOPBITS_ONE,                     /* Stop bits                    */      \
    (USART_Parity_TypeDef)USART_FRAME_PARITY_NONE,                        /* Parity                       */      \
    (USART_OVS_TypeDef)USART_CTRL_OVS_X16,                                /* Oversampling mode            */      \
    false,                                                                /* Majority vote disable        */      \
    uartdrvFlowControlHw,                                                 /* Flow control                 */      \
    (GPIO_Port_TypeDef)AF_USART2_CS_PORT(_USART_ROUTE_LOCATION_LOC1),     /* CTS port number              */      \
    (GPIO_Port_TypeDef)AF_USART2_CS_PIN(_USART_ROUTE_LOCATION_LOC1),      /* CTS pin number               */      \
    (GPIO_Port_TypeDef)AF_USART2_CLK_PORT(_USART_ROUTE_LOCATION_LOC1),    /* RTS port number              */      \
    (GPIO_Port_TypeDef)AF_USART2_CLK_PIN(_USART_ROUTE_LOCATION_LOC1),     /* RTS pin number               */      \
    NULL,                                                                 /* RX operation queue           */      \
    NULL                                                                  /* TX operation queue           */      \
  }
  #ifndef COM_USART2_RXSTOP
    #define COM_USART2_RXSTOP 16
  #endif
  #ifndef COM_USART2_RXSTART
    #define COM_USART2_RXSTART COM_USART2_RXSTOP
  #endif
  // Start threshold and size must be multiples of the stop threshold if using
  // hardware flow control.
  #ifdef COM_USART2_HW_FC
    #if COM_USART2_RX_QUEUE_SIZE % COM_USART2_RXSTOP != 0
      #error RX queue size must be a multiple of RX stop
    #endif
    #if COM_USART2_RXSTART % COM_USART2_RXSTOP != 0
      #error RX start must be a multiple of RX stop
    #endif
  #endif

  #define COM_USART2_DEFAULT                                                   \
  {                                                                            \
    (UARTDRV_Init_t) USART2_INIT,                  /* USART initdata        */ \
    COM_USART2_RXSTOP,                             /* RX stop threshold     */ \
    COM_USART2_RXSTART                             /* RX start threshold    */ \
  }
#endif //COM_USART2_ENABLE

#ifdef COM_LEUART0_ENABLE

  #ifdef RETARGET_LEUART0
    #ifdef  _LEUART_ROUTELOC0_MASK
      #define LEUART0_TX_LOCATION RETARGET_TX_LOCATION
      #define LEUART0_RX_LOCATION RETARGET_RX_LOCATION
    #else // _LEUART_ROUTELOC0_MASK
      #define LEUART0_LOCATION RETARGET_LOCATION
    #endif // _LEUART_ROUTELOC0_MASK
    #ifdef RETARGET_CTS_PORT
      #define LEUART0_CTS_PORT RETARGET_CTS_PORT
      #define LEUART0_CTS_PIN  RETARGET_CTS_PIN
    #else // RETARGET_CTS_PORT
      #define LEUART0_CTS_PORT 0
      #define LEUART0_CTS_PIN  0
    #endif// RETARGET_CTS_PORT
    #ifdef RETARGET_RTS_PORT
      #define LEUART0_RTS_PORT RETARGET_RTS_PORT
      #define LEUART0_RTS_PIN  RETARGET_RTS_PIN
    #else // RETARGET_RTS_PORT
      #define LEUART0_RTS_PORT 0
      #define LEUART0_RTS_PIN  0
    #endif// RETARGET_RTS_PORT
  #endif //RETARGET_LEUART0

  #ifdef COM_LEUART0_HW_FC
    #define LEUART0_FC_MODE uartdrvFlowControlHw
  #elif defined (COM_LEUART0_SW_FC)
    #define LEUART0_FC_MODE uartdrvFlowControlSw
  #else
    #define LEUART0_FC_MODE uartdrvFlowControlNone
  #endif

  #ifdef _LEUART_ROUTELOC0_MASK
    #define LEUART0_INIT                                                                                         \
    {                                                                                                               \
      LEUART0,                                                              /* LEUART port                  */      \
      115200,                                                               /* Baud rate                    */      \
      LEUART0_TX_LOCATION,                                                  /* LEUART TX location number    */      \
      LEUART0_RX_LOCATION,                                                  /* LEUART TX location number    */      \
      (LEUART_Stopbits_TypeDef)LEUART_CTRL_STOPBITS_ONE,                    /* Stop bits                    */      \
      (LEUART_Parity_TypeDef)LEUART_CTRL_PARITY_NONE,                       /* Parity                       */      \
      LEUART0_FC_MODE,                                                      /* Flow control                 */      \
      (GPIO_Port_TypeDef)RETARGET_CTS_PORT,                                 /* CTS port number              */      \
      RETARGET_CTS_PIN,                                                     /* CTS pin number               */      \
      (GPIO_Port_TypeDef)RETARGET_RTS_PORT,                                 /* RTS port number              */      \
      RETARGET_RTS_PIN,                                                     /* RTS pin number               */      \
      NULL,                                                                 /* RX operation queue           */      \
      NULL                                                                  /* TX operation queue           */      \
    }
  #else // _LEUART_ROUTELOC0_MASK
    #define LEUART0_INIT                                                                                         \
    {                                                                                                               \
      LEUART0,                                                              /* LEUART port                   */      \
      115200,                                                               /* Baud rate                    */      \
      LEUART0_LOCATION,                                                     /* LEUART location number       */      \
      (LEUART_Stopbits_TypeDef)LEUART_CTRL_STOPBITS_ONE,                    /* Stop bits                    */      \
      (LEUART_Parity_TypeDef)LEUART_CTRL_PARITY_NONE,                       /* Parity                       */      \
      LEUART0_FC_MODE,                                                      /* Flow control                 */      \
      (GPIO_Port_TypeDef)LEUART0_CTS_PORT,                                  /* CTS port number              */      \
      LEUART0_CTS_PIN,                                                      /* CTS pin number               */      \
      (GPIO_Port_TypeDef)LEUART0_RTS_PORT,                                  /* RTS port number              */      \
      LEUART0_RTS_PIN,                                                      /* RTS pin number               */      \
      NULL,                                                                 /* RX operation queue           */      \
      NULL                                                                  /* TX operation queue           */      \
    }
  #endif // _LEUART_ROUTELOC0_MASK

  #ifndef COM_LEUART0_RXSTOP
    #define COM_LEUART0_RXSTOP 16
  #endif
  #ifndef COM_LEUART0_RXSTART
    #define COM_LEUART0_RXSTART COM_LEUART0_RXSTOP
  #endif
  // Start threshold and size must be multiples of the stop threshold if using
  // hardware flow control.
  #ifdef COM_LEUART0_HW_FC
    #if COM_LEUART0_RX_QUEUE_SIZE % COM_LEUART0_RXSTOP != 0
      #error RX queue size must be a multiple of RX stop
    #endif
    #if COM_LEUART0_RXSTART % COM_LEUART0_RXSTOP != 0
      #error RX start must be a multiple of RX stop
    #endif
  #endif

  #define COM_LEUART0_DEFAULT                                                   \
  {                                                                             \
    {.leuartinit = LEUART0_INIT},            /* LEUART initdata       */ \
    COM_LEUART0_RXSTOP,                             /* RX stop threshold     */ \
    COM_LEUART0_RXSTART                             /* RX start threshold    */ \
  }
#endif //COM_LEUART0_ENABLE

#ifdef COM_LEUART1_ENABLE

  #ifdef RETARGET_LEUART1
    #ifdef  _LEUART_ROUTELOC0_MASK
      #define LEUART1_TX_LOCATION RETARGET_TX_LOCATION
      #define LEUART1_RX_LOCATION RETARGET_RX_LOCATION
    #else // _LEUART_ROUTELOC0_MASK
      #define LEUART1_LOCATION RETARGET_LOCATION
    #endif // _LEUART_ROUTELOC0_MASK
    #ifdef RETARGET_CTS_PORT
      #define LEUART1_CTS_PORT RETARGET_CTS_PORT
      #define LEUART1_CTS_PIN  RETARGET_CTS_PIN
    #else // RETARGET_CTS_PORT
      #define LEUART1_CTS_PORT 0
      #define LEUART1_CTS_PIN  0
    #endif// RETARGET_CTS_PORT
    #ifdef RETARGET_RTS_PORT
      #define LEUART1_RTS_PORT RETARGET_RTS_PORT
      #define LEUART1_RTS_PIN  RETARGET_RTS_PIN
    #else // RETARGET_RTS_PORT
      #define LEUART1_RTS_PORT 0
      #define LEUART1_RTS_PIN  0
    #endif// RETARGET_RTS_PORT
  #endif //RETARGET_LEUART1

  #ifdef COM_LEUART1_HW_FC
    #define LEUART1_FC_MODE uartdrvFlowControlHw
  #elif defined (COM_LEUART1_SW_FC)
    #define LEUART1_FC_MODE uartdrvFlowControlSw
  #else
    #define LEUART1_FC_MODE uartdrvFlowControlNone
  #endif

  #ifdef _LEUART_ROUTELOC0_MASK
    #define LEUART1_INIT                                                                                         \
    {                                                                                                               \
      LEUART1,                                                              /* LEUART port                  */      \
      115200,                                                               /* Baud rate                    */      \
      LEUART1_TX_LOCATION,                                                  /* LEUART TX location number    */      \
      LEUART1_RX_LOCATION,                                                  /* LEUART TX location number    */      \
      (LEUART_Stopbits_TypeDef)LEUART_CTRL_STOPBITS_ONE,                    /* Stop bits                    */      \
      (LEUART_Parity_TypeDef)LEUART_CTRL_PARITY_NONE,                       /* Parity                       */      \
      LEUART1_FC_MODE,                                                      /* Flow control                 */      \
      (GPIO_Port_TypeDef)RETARGET_CTS_PORT,                                 /* CTS port number              */      \
      RETARGET_CTS_PIN,                                                     /* CTS pin number               */      \
      (GPIO_Port_TypeDef)RETARGET_RTS_PORT,                                 /* RTS port number              */      \
      RETARGET_RTS_PIN,                                                     /* RTS pin number               */      \
      NULL,                                                                 /* RX operation queue           */      \
      NULL                                                                  /* TX operation queue           */      \
    }
  #else // _LEUART_ROUTELOC0_MASK
    #define LEUART1_INIT                                                                                         \
    {                                                                                                               \
      LEUART1,                                                              /* LEUART port                   */      \
      115200,                                                               /* Baud rate                    */      \
      LEUART1_LOCATION,                                                     /* LEUART location number       */      \
      (LEUART_Stopbits_TypeDef)LEUART_CTRL_STOPBITS_ONE,                    /* Stop bits                    */      \
      (LEUART_Parity_TypeDef)LEUART_CTRL_PARITY_NONE,                       /* Parity                       */      \
      LEUART1_FC_MODE,                                                      /* Flow control                 */      \
      (GPIO_Port_TypeDef)LEUART1_CTS_PORT,                                  /* CTS port number              */      \
      LEUART1_CTS_PIN,                                                      /* CTS pin number               */      \
      (GPIO_Port_TypeDef)LEUART1_RTS_PORT,                                  /* RTS port number              */      \
      LEUART1_RTS_PIN,                                                      /* RTS pin number               */      \
      NULL,                                                                 /* RX operation queue           */      \
      NULL                                                                  /* TX operation queue           */      \
    }
  #endif // _LEUART_ROUTELOC0_MASK

  #ifndef COM_LEUART1_RXSTOP
    #define COM_LEUART1_RXSTOP 16
  #endif
  #ifndef COM_LEUART1_RXSTART
    #define COM_LEUART1_RXSTART COM_LEUART1_RXSTOP
  #endif
  // Start threshold and size must be multiples of the stop threshold if using
  // hardware flow control.
  #ifdef COM_LEUART1_HW_FC
    #if COM_LEUART1_RX_QUEUE_SIZE % COM_LEUART1_RXSTOP != 0
      #error RX queue size must be a multiple of RX stop
    #endif
    #if COM_LEUART1_RXSTART % COM_LEUART1_RXSTOP != 0
      #error RX start must be a multiple of RX stop
    #endif
  #endif

  #define COM_LEUART1_DEFAULT                                                   \
  {                                                                             \
    {.leuartinit = LEUART1_INIT},            /* LEUART initdata       */ \
    COM_LEUART1_RXSTOP,                             /* RX stop threshold     */ \
    COM_LEUART1_RXSTART                             /* RX start threshold    */ \
  }
#endif //COM_LEUART1_ENABLE

#ifndef UART_RX_INT_PORT
  #define UART_RX_INT_PORT           gpioPortA
#endif
#ifndef UART_RX_INT_PIN
  #define UART_RX_INT_PIN            1
#endif

#endif // COM_CONFIG_H
