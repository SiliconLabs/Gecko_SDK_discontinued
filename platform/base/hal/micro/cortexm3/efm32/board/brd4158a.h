/** @file hal/micro/cortexm3/efm32/board/brd4158a.h
 * BRD4158A-EFR32MG13 2.4GHz / 915 MHz 19dBm
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

// #define DISABLE_DOC

#if !defined(REGRESSION_TEST)
// Enable COM port to use retarget serial configuration
#define COM_RETARGET_SERIAL
// Enable HW flow control
#define COM_USART0_HW_FC
#endif

#define EXTERNAL_FLASH_MOSI_PORT    gpioPortC
#define EXTERNAL_FLASH_MOSI_PIN     6
#define EXTERNAL_FLASH_MISO_PORT    gpioPortC
#define EXTERNAL_FLASH_MISO_PIN     7
#define EXTERNAL_FLASH_CLK_PORT     gpioPortC
#define EXTERNAL_FLASH_CLK_PIN      8
#define EXTERNAL_FLASH_CS_PORT      gpioPortA
#define EXTERNAL_FLASH_CS_PIN       4

#define EXTERNAL_FLASH_USART_TXLOC  USART_ROUTELOC0_TXLOC_LOC11
#define EXTERNAL_FLASH_USART_RXLOC  USART_ROUTELOC0_RXLOC_LOC11
#define EXTERNAL_FLASH_USART_CLKLOC USART_ROUTELOC0_CLKLOC_LOC11
// Define nHOST_INT to PF6 -> EXP7 on SLWSTK6000A
#define NHOST_INT_PORT gpioPortF
#define NHOST_INT_PIN  6
// Define nWAKE to PF7 -> EXP9 on SLWSTK6000A
#define NWAKE_PORT     gpioPortF
#define NWAKE_PIN      7

// Include common definitions
#include "board/wstk-common.h"

#endif //__BOARD_H__
/** @} END Board Specific Functions */

/** @} END addtogroup */
