/***************************************************************************//**
 * @file btl_gpio_activation.h
 * @brief GPIO Activation plugin for Silicon Labs bootloader.
 * @author Silicon Labs
 * @version 1.0.0
 *******************************************************************************
 * # License
 * <b>Copyright 2016 Silicon Laboratories, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/
#ifndef BTL_GPIO_ACTIVATION_H
#define BTL_GPIO_ACTIVATION_H

/***************************************************************************//**
 * @addtogroup Plugin
 * @{
 * @addtogroup GpioActivation GPIO Activation
 * @brief Enter bootloader based on GPIO state
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * Enter the bootlader if the GPIO pin is active
 *
 * @return True if the bootloader should be entered
 ******************************************************************************/
bool gpio_enterBootloader(void);

/** @} // addtogroup GpioActivation */
/** @} // addtogroup Plugin */

#endif // BTL_GPIO_ACTIVATION_H
