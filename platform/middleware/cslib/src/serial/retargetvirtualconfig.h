/**************************************************************************//**
 * Copyright 2016 by Silicon Laboratories Inc. All rights reserved.
 *
 * http://developer.silabs.com/legal/version/v11/Silicon_Labs_Software_License_Agreement.txt
 *****************************************************************************/

#ifndef __RETARGET_VIRTUAL_CONFIG_H__
#define __RETARGET_VIRTUAL_CONFIG_H__

/* STK          VCOM ENABLE     CDC             TX      RX      LOCATION */
/* GG/LG/WG     PF7             UART0           PE0     PE1     1        */
/* HG           PA9             USART1          PF2     PA0     4        */
/* HG           PA9             LEUART0         PF2     PA0     4        */
/* ZG           PA9             LEUART0         PD4     PD5     0        */

#if defined(_EFM32_GIANT_FAMILY) || defined(_EFM32_WONDER_FAMILY) || defined(_EFM32_LEOPARD_FAMILY)
  #define RETARGET_IRQ_NAME    UART0_RX_IRQHandler              /* USART IRQ Handler */
  #define RETARGET_CLK         cmuClock_UART0                   /* HFPER Clock */
  #define RETARGET_IRQn        UART0_RX_IRQn                    /* IRQ number */
  #define RETARGET_UART        UART0                            /* USART instance */
  #define RETARGET_TX          USART_Tx                         /* Set TX to USART_Tx */
  #define RETARGET_RX          USART_Rx                         /* Set RX to USART_Rx */
  #define RETARGET_LOCATION    UART_ROUTE_LOCATION_LOC1         /* Location of of the USART I/O pins */
  #define RETARGET_TXPORT      gpioPortE                        /* USART transmission port */
  #define RETARGET_TXPIN       0                                /* USART transmission pin */
  #define RETARGET_RXPORT      gpioPortE                        /* USART reception port */
  #define RETARGET_RXPIN       1                                /* USART reception pin */
  #define RETARGET_USART       1                                /* Includes em_usart.h */
  #define RETARGET_ENPORT      gpioPortF                        /* USART transmission port */
  #define RETARGET_ENPIN       7                                /* USART transmission pin */

#elif (defined(_EFM32_HAPPY_FAMILY) && !defined (RETARGET_LEUART0))
  #define RETARGET_IRQ_NAME    USART1_RX_IRQHandler             /* USART IRQ Handler */
  #define RETARGET_CLK         cmuClock_USART1                  /* HFPER Clock */
  #define RETARGET_IRQn        USART1_RX_IRQn                   /* IRQ number */
  #define RETARGET_UART        USART1                           /* USART instance */
  #define RETARGET_TX          USART_Tx                         /* Set TX to USART_Tx */
  #define RETARGET_RX          USART_Rx                         /* Set RX to USART_Rx */
  #define RETARGET_LOCATION    USART_ROUTE_LOCATION_LOC4        /* Location of of the USART I/O pins */
  #define RETARGET_TXPORT      gpioPortF                        /* USART transmission port */
  #define RETARGET_TXPIN       2                                /* USART transmission pin */
  #define RETARGET_RXPORT      gpioPortA                        /* USART reception port */
  #define RETARGET_RXPIN       0                                /* USART reception pin */
  #define RETARGET_USART       1                                /* Includes em_usart.h */
  #define RETARGET_ENPORT      gpioPortA                        /* USART transmission port */
  #define RETARGET_ENPIN       9                                /* USART transmission pin */

#elif (defined(_EFM32_HAPPY_FAMILY) && defined (RETARGET_LEUART0))
  #define RETARGET_IRQ_NAME    LEUART0_IRQHandler               /* LEUART IRQ Handler */
  #define RETARGET_CLK         cmuClock_LEUART0                 /* LFB Clock */
  #define RETARGET_IRQn        LEUART0_IRQn                     /* IRQ number */
  #define RETARGET_UART        LEUART0                          /* LEUART instance */
  #define RETARGET_TX          LEUART_Tx                        /* Set TX to LEUART_Tx */
  #define RETARGET_RX          LEUART_Rx                        /* Set RX to LEUART_Rx */
  #define RETARGET_TXPORT      gpioPortF                        /* USART transmission port */
  #define RETARGET_TXPIN       2                                /* USART transmission pin */
  #define RETARGET_RXPORT      gpioPortA                        /* USART reception port */
  #define RETARGET_RXPIN       0                                /* USART reception pin */
  #define RETARGET_LOCATION    LEUART_ROUTE_LOCATION_LOC4       /* Location of of the LEUART I/O pins */
  #define RETARGET_LEUART      1                                /* Includes em_leuart.h */
  #define RETARGET_ENPORT      gpioPortA                        /* USART transmission port */
  #define RETARGET_ENPIN       9                                /* USART transmission pin */

#elif defined(_EFM32_ZERO_FAMILY)
  #define RETARGET_IRQ_NAME    LEUART0_IRQHandler               /* LEUART IRQ Handler */
  #define RETARGET_CLK         cmuClock_LEUART0                 /* LFB Clock */
  #define RETARGET_IRQn        LEUART0_IRQn                     /* IRQ number */
  #define RETARGET_UART        LEUART0                          /* LEUART instance */
  #define RETARGET_TX          LEUART_Tx                        /* Set TX to LEUART_Tx */
  #define RETARGET_RX          LEUART_Rx                        /* Set RX to LEUART_Rx */
  #define RETARGET_TXPORT      gpioPortD                        /* LEUART transmission port */
  #define RETARGET_TXPIN       4                                /* LEUART transmission pin */
  #define RETARGET_RXPORT      gpioPortD                        /* LEUART reception port */
  #define RETARGET_RXPIN       5                                /* LEUART reception pin */
  #define RETARGET_LOCATION    LEUART_ROUTE_LOCATION_LOC0       /* Location of of the LEUART I/O pins */
  #define RETARGET_LEUART      1                                /* Includes em_leuart.h */
  #define RETARGET_ENPORT      gpioPortA                        /* USART transmission port */
  #define RETARGET_ENPIN       9                                /* USART transmission pin */

#elif (defined(_EFM32_PEARL_FAMILY))
  #define RETARGET_IRQ_NAME    USART0_RX_IRQHandler             /* USART IRQ Handler */
  #define RETARGET_CLK         cmuClock_USART0                  /* HFPER Clock */
  #define RETARGET_IRQn        USART0_RX_IRQn                   /* IRQ number */
  #define RETARGET_UART        USART0                           /* USART instance */
  #define RETARGET_TX          USART_Tx                         /* Set TX to USART_Tx */
  #define RETARGET_RX          USART_Rx                         /* Set RX to USART_Rx */
  #define RETARGET_RX_LOCATION USART_ROUTELOC0_RXLOC_LOC0
  #define RETARGET_TX_LOCATION USART_ROUTELOC0_TXLOC_LOC0
  #define RETARGET_TXPORT      gpioPortA                        /* USART transmission port */
  #define RETARGET_TXPIN       0                                /* USART transmission pin */
  #define RETARGET_RXPORT      gpioPortA                        /* USART reception port */
  #define RETARGET_RXPIN       1                                /* USART reception pin */
  #define RETARGET_USART       1                                /* Includes em_usart.h */
  #define RETARGET_ENPORT      gpioPortA                        /* USART transmission port */
  #define RETARGET_ENPIN       5                                /* USART transmission pin */
#else
#error "STK does not support virtual COM port"
#endif

#endif // __RETARGET_VIRTUAL_CONFIG_H__
