/** @file hal/micro/cortexm3/efm32/board/brd4250b.h
 * BRD4250B-EFR32FG 2.4 GHz / 868 MHz 13dBm (SLWSTK6061A)
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
// Don't enable HW flow control
// #define COM_USART0_HW_FC

// spi cs connection not the default
#define EXTERNAL_FLASH_CS_PORT      gpioPortA
#define EXTERNAL_FLASH_CS_PIN       4

// Include common definitions
#include "board/wstk-common.h"

#endif //__BOARD_H__
/** @} END Board Specific Functions */

/** @} END addtogroup */
