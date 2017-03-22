/** @file hal/micro/cortexm3/efm32/board/brd4163a.h
 * BRD4163A-EFR32MG12 2.4GHz / 868 MHz 10dBm 
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

#if !defined(REGRESSION_TEST)
// Enable COM port to use retarget serial configuration
#define COM_RETARGET_SERIAL
// Enable HW flow control
#define COM_USART0_HW_FC
#endif

// External Flash Defines

// figure out what pins are internally connected to flash
// Define onboard external flash locations
#define EXTERNAL_FLASH_MOSI_PORT    gpioPortC
#define EXTERNAL_FLASH_MOSI_PIN     6
#define EXTERNAL_FLASH_MISO_PORT    gpioPortC
#define EXTERNAL_FLASH_MISO_PIN     7
#define EXTERNAL_FLASH_CLK_PORT     gpioPortC
#define EXTERNAL_FLASH_CLK_PIN      8
#define EXTERNAL_FLASH_CS_PORT      gpioPortA
#define EXTERNAL_FLASH_CS_PIN       4

#define EXTERNAL_FLASH_USART_TXLOC USART_ROUTELOC0_TXLOC_LOC11
#define EXTERNAL_FLASH_USART_RXLOC USART_ROUTELOC0_RXLOC_LOC11
#define EXTERNAL_FLASH_USART_CLKLOC USART_ROUTELOC0_CLKLOC_LOC11


// SPI NCP defines

// Define nSSEL_INT to PA9 -> EXP10 on SLWSTK6000A    (Default C9)
#define NSSEL_PORT     gpioPortA
#define NSSEL_PIN      9
// Define nHOST_INT to PD10 -> EXP7 on SLWSTK6000A   (Default A4)
#define NHOST_INT_PORT gpioPortD
#define NHOST_INT_PIN  10
// Define nWAKE to PD11 -> EXP9 on SLWSTK6000A       (Default B11)
#define NWAKE_PORT     gpioPortD
#define NWAKE_PIN      11

#define SPI_NCP_USART_PORT 2

#define SPI_NCP_USART2                                                    \
{                                                                         \
  USART2,                       /* USART port                       */    \
  _USART_ROUTELOC0_TXLOC_LOC1,  /* USART Tx pin location number     */    \
  _USART_ROUTELOC0_RXLOC_LOC1,  /* USART Rx pin location number     */    \
  _USART_ROUTELOC0_CLKLOC_LOC1, /* USART Clk pin location number    */    \
  _USART_ROUTELOC0_CSLOC_LOC1,  /* USART Cs pin location number     */    \
  0,                            /* Bitrate                          */    \
  8,                            /* Frame length                     */    \
  0xFF,                         /* Dummy tx value for rx only funcs */    \
  spidrvSlave,                  /* SPI mode                         */    \
  spidrvBitOrderMsbFirst,       /* Bit order on bus                 */    \
  spidrvClockMode0,             /* SPI clock/phase mode             */    \
  spidrvCsControlAuto,          /* CS controlled by the driver      */    \
  spidrvSlaveStartImmediate     /* Slave start transfers immediately*/    \
}

// Include common definitions
#include "board/wstk-common.h"

#endif //__BOARD_H__
/** @} END Board Specific Functions */

/** @} END addtogroup */
