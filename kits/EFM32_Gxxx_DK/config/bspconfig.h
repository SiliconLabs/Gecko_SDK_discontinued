/***************************************************************************//**
 * @file
 * @brief Provide BSP (board support package) configuration parameters.
 * @version 4.2.0
 *******************************************************************************
 * @section License
 * <b>(C) Copyright 2014 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#ifndef __BSPCONFIG_H
#define __BSPCONFIG_H

#define BSP_DK
#define BSP_DK_BRD3200

#if defined( EBI_PRESENT ) && !defined( LCD_PRESENT )
#define BSP_BC_CTRL_EBI
#else
#define BSP_BC_CTRL_SPI
#endif

#include "bsp_dk_bcreg_3200.h"

#define BSP_DK_LEDS
#define BSP_NO_OF_LEDS      16
#define BSP_LED_MASK        0xFFFF
#define BSP_LED_PORT        BC_LED
#define BSP_GPIO_INT_PORT   gpioPortC
#define BSP_GPIO_INT_PIN    14

#define BSP_INIT_DEFAULT  0

#endif
