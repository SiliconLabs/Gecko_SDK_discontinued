/** @file hal/micro/cortexm3/efm32/board/brd4300b.h
 * MGM111 Mesh Module Radio Board 
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

// Define nHOST_INT to PF4 -> EXP7
#define NHOST_INT_PORT gpioPortF
#define NHOST_INT_PIN  4
// Define nWAKE to PF5 -> EXP9
#define NWAKE_PORT     gpioPortF
#define NWAKE_PIN      5

// Include common definitions
#include "board/wstk-common.h"

#endif //__BOARD_H__
/** @} END Board Specific Functions */

/** @} END addtogroup */
