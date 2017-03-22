/** @file hal/micro/cortexm3/efm32/board/brd4153a.h
 * BRD4153A-EFR32MG 2.4 GHz 13dBm (SLWSTK6000A)
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
#include "mx25flash_config.h"

// Define board features

// #define DISABLE_PTI

// #define DISABLE_DCDC

#if !defined(REGRESSION_TEST)
// Enable COM port to use retarget serial configuration
#define COM_RETARGET_SERIAL
// Enable HW flow control
#define COM_USART0_HW_FC
#endif

// Define onboard external flash locations
#define EXTERNAL_FLASH_MOSI_PORT    MX25_PORT_MOSI
#define EXTERNAL_FLASH_MOSI_PIN     MX25_PIN_MOSI
#define EXTERNAL_FLASH_MISO_PORT    MX25_PORT_MISO
#define EXTERNAL_FLASH_MISO_PIN     MX25_PIN_MISO
#define EXTERNAL_FLASH_CLK_PORT     MX25_PORT_SCLK
#define EXTERNAL_FLASH_CLK_PIN      MX25_PIN_SCLK
#define EXTERNAL_FLASH_CS_PORT      MX25_PORT_CS
#define EXTERNAL_FLASH_CS_PIN       MX25_PIN_CS

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
