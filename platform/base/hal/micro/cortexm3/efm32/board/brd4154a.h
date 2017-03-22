/** @file hal/micro/cortexm3/efm32/board/brd4154a.h
 * BRDFIREFLY-EFR32MG 
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

#define NO_LED 1

// #define DISABLE_PTI

// #define DISABLE_DCDC

// Enable COM port to use retarget serial configuration
#define COM_RETARGET_SERIAL
// Enable HW flow control
#define COM_USART0_HW_FC

// figure out what pins are internally connected to flash
// Define onboard external flash locations
#define EXTERNAL_FLASH_MOSI_PORT    gpioPortF
#define EXTERNAL_FLASH_MOSI_PIN     6
#define EXTERNAL_FLASH_MISO_PORT    gpioPortC
#define EXTERNAL_FLASH_MISO_PIN     7
#define EXTERNAL_FLASH_CLK_PORT     gpioPortC
#define EXTERNAL_FLASH_CLK_PIN      9
#define EXTERNAL_FLASH_CS_PORT      gpioPortC
#define EXTERNAL_FLASH_CS_PIN       6

#define EXTERNAL_FLASH_HOLD_PORT    gpioPortC
#define EXTERNAL_FLASH_HOLD_PIN     8
#define EXTERNAL_FLASH_WP_PORT      gpioPortF
#define EXTERNAL_FLASH_WP_PIN       7

#define EXTERNAL_FLASH_USART_TXLOC USART_ROUTELOC0_TXLOC_LOC30
#define EXTERNAL_FLASH_USART_RXLOC USART_ROUTELOC0_RXLOC_LOC11
#define EXTERNAL_FLASH_USART_CLKLOC USART_ROUTELOC0_CLKLOC_LOC12

#define IO_EXPANDER_WAKE_PORT     gpioPortF
#define IO_EXPANDER_WAKE_PIN      3
#define IO_EXPANDER_I2C_SCL_PORT  gpioPortC
#define IO_EXPANDER_I2C_SCL_PIN   11
#define IO_EXPANDER_I2C_SDA_PORT  gpioPortC
#define IO_EXPANDER_I2C_SDA_PIN   10

// VCOM enable signal on this board is controlled by the IO expander and
// defaults to enabled, nothing we have to do to enable it.
#define halEnableVCOM()   \
  do {                    \
  } while(0)

// Include common definitions
#include "board/wstk-common.h"

#endif //__BOARD_H__
/** @} END Board Specific Functions */

/** @} END addtogroup */
