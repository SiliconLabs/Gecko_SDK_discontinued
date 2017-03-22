/** @file hal/micro/cortexm3/efm32/board/brd4157a.h
 * BRD4157A-EFR32MG12 2.4 GHz / 915 MHz 19.5dBm Touch
 * See @ref board for detailed documentation.
 *
 * <!-- Copyright 2016 Silicon Laboratories, Inc.                        *80*-->
 */

/** @addtogroup board
 *  @brief Functions and definitions specific to the breakout board.
 *
 *@{
 */

#ifndef __BOARD_H__
#define __BOARD_H__

#include "bspconfig.h"

// Define board features

// #define DISABLE_PTI

// #define DISABLE_DCDC

// Enable COM port to use retarget serial configuration
#define COM_RETARGET_SERIAL

// Enable HW flow control
#define COM_USART0_HW_FC

// Use USART 2 for SPI NCP
#define SPI_NCP_USART_PORT 2

// Configuration data for SPI NCP slave using USART2.
// Location 1 will output on EXP header on WSTK6001
// EXP    Function  Pin  Peripheral  Location  Description
// EXP4   SPI_MOSI  PA6  USART2_TX   1         Data input to NCP
// EXP6   SPI_MISO  PA7  USART2_RX   1         Data output from NCP
// EXP8   SPI_CLK   PA8  USART2_CLK  1         Clock input to NCP
// EXP10  SPI_CS    PA9  USART2_CS   1         Slave select input to NCP
#define SPI_NCP_USART2                                                   \
  {                                                                      \
    USART2,                     /* USART port                       */   \
    _USART_ROUTELOC0_TXLOC_LOC1, /* USART Tx pin location number     */  \
    _USART_ROUTELOC0_RXLOC_LOC1, /* USART Rx pin location number     */  \
    _USART_ROUTELOC0_CLKLOC_LOC1, /* USART Clk pin location number    */ \
    _USART_ROUTELOC0_CSLOC_LOC1, /* USART Cs pin location number     */  \
    0,                          /* Bitrate                          */   \
    8,                          /* Frame length                     */   \
    0xFF,                       /* Dummy tx value for rx only funcs */   \
    spidrvSlave,                /* SPI mode                         */   \
    spidrvBitOrderMsbFirst,     /* Bit order on bus                 */   \
    spidrvClockMode0,           /* SPI clock/phase mode             */   \
    spidrvCsControlAuto,        /* CS controlled by the driver      */   \
    spidrvSlaveStartImmediate   /* Slave start transfers immediately*/   \
  }

// Define nHOST_INT to PD10 -> EXP7
#define NHOST_INT_PORT gpioPortD
#define NHOST_INT_PIN  10

// Define nWAKE to PD11 -> EXP9
#define NWAKE_PORT     gpioPortD
#define NWAKE_PIN      11

#define NSSEL_PORT gpioPortA
#define NSSEL_PIN 9

// Include common definitions
#include "board/wstk-common.h"

#endif // __BOARD_H__

/** @} END Board Specific Functions */

/** @} END addtogroup */
