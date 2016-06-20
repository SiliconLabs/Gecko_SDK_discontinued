/***************************************************************************//**
 * @file
 * @brief Provide BSP (board support package) configuration parameters.
 * @author Energy Micro AS
 * @version 4.0.0
 *******************************************************************************
 * @section License
 * <b>(C) Copyright 2013 Energy Micro AS, http://www.energymicro.com</b>
 *******************************************************************************
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 * 4. The source and compiled code may only be used on Energy Micro "EFM32"
 *    microcontrollers and "EFR4" radios.
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Energy Micro AS has no
 * obligation to support this Software. Energy Micro AS is providing the
 * Software "AS IS", with no express or implied warranties of any kind,
 * including, but not limited to, any implied warranties of merchantability
 * or fitness for any particular purpose or warranties against infringement
 * of any proprietary rights of a third party.
 *
 * Energy Micro AS will not be liable for any consequential, incidental, or
 * special damages, or any other relief, or for any claim by any third party,
 * arising from your use of this Software.
 *
 *****************************************************************************/
#ifndef __BSPCONFIG_H
#define __BSPCONFIG_H

#define BSP_STK
#define BSP_STK_2200
#define BSP_NET

#define BSP_BCC_USART       USART2
#define BSP_BCC_CLK         cmuClock_USART2
#define BSP_BCC_LOCATION    UART_ROUTE_LOCATION_LOC1
#define BSP_BCC_TXPORT      gpioPortB
#define BSP_BCC_TXPIN       3
#define BSP_BCC_RXPORT      gpioPortB
#define BSP_BCC_RXPIN       4
#define BSP_BCC_ENABLE_PORT gpioPortA
#define BSP_BCC_ENABLE_PIN  12

#define BSP_DISP_ENABLE_PORT  gpioPortA
#define BSP_DISP_ENABLE_PIN   14                /* MemLCD display enable */

#define BSP_GPIO_LEDS
#define BSP_NO_OF_LEDS  2
#define BSP_GPIO_LEDARRAY_INIT {{gpioPortF,6},{gpioPortF,7}}

#define BSP_GPIO_BUTTONARRAY
#define BSP_NO_OF_BUTTONS 2
#define BSP_GPIO_PB0_PORT gpioPortE
#define BSP_GPIO_PB0_PIN  3
#define BSP_GPIO_PB1_PORT gpioPortE
#define BSP_GPIO_PB1_PIN  2

#define BSP_GPIO_BUTTONARRAY_INIT {{BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN}, {BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN}}

#define BSP_INIT_DEFAULT  0

#define BSP_BCP_VERSION 2
#include "bsp_bcp.h"

#endif
