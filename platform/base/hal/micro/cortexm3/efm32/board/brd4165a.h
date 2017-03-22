/** @file hal/micro/cortexm3/efm32/board/brd4165a.h
 * BRD4165A-EFR32MG13 2.4GHz 19dBm
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

#define NO_DCDC

#if !defined(REGRESSION_TEST)
// Enable COM port to use retarget serial configuration
#define COM_RETARGET_SERIAL
// Enable HW flow control
#define COM_USART0_HW_FC
#endif

#define EXTERNAL_FLASH_MOSI_PORT    gpioPortF
#define EXTERNAL_FLASH_MOSI_PIN     5
#define EXTERNAL_FLASH_MISO_PORT    gpioPortC
#define EXTERNAL_FLASH_MISO_PIN     9
#define EXTERNAL_FLASH_CLK_PORT     gpioPortF
#define EXTERNAL_FLASH_CLK_PIN      4
#define EXTERNAL_FLASH_CS_PORT      gpioPortC
#define EXTERNAL_FLASH_CS_PIN       8

#define EXTERNAL_FLASH_USART_TXLOC  USART_ROUTELOC0_TXLOC_LOC29
#define EXTERNAL_FLASH_USART_RXLOC  USART_ROUTELOC0_RXLOC_LOC13
#define EXTERNAL_FLASH_USART_CLKLOC USART_ROUTELOC0_CLKLOC_LOC26


// This part isn't expected to be used as a SPI NCP.
// if it needs to be used as such, define for SPI_NCP_USART1
// and SSEL_PORT and SSEL_PIN should be added here

// Define nHOST_INT to PD10 -> EXP7 on SLWSTK6000A
#define NHOST_INT_PORT gpioPortD
#define NHOST_INT_PIN  10
// Define nWAKE to PD11 -> EXP9 on SLWSTK6000A
#define NWAKE_PORT     gpioPortD
#define NWAKE_PIN      11

// Include common definitions
#include "board/wstk-common.h"

#endif //__BOARD_H__
/** @} END Board Specific Functions */

/** @} END addtogroup */
