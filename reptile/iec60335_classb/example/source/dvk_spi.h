/***************************************************************************//**
 * @file    dvk_spi.h
 * @brief   header code for spi applications based on Cortex M3
 * @version 1.0.0
 *******************************************************************************
 * @section License
 * <b>(C) Copyright 2010 Energy Micro AS, http://www.energymicro.com</b>
 *******************************************************************************
 *
 * This source code is the property of Energy Micro AS. The source and compiled
 * code may only be used on Energy Micro "EFM32" microcontrollers.
 *
 * This copyright notice may not be removed from the source code nor changed.
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
 ******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DVK_SPI_H__
#define __DVK_SPI_H__

/*! @addtogroup IEC60335_TOOLS
* @{
*/
/*! public function to enable LED communication, set LED0, set LED0 and 1
 * use this function to show that the WDOG reset occurred and changed the system.
 * a complete check will enable the LED0 until the WDOG reset occurred and 
 * than change the LEDs to LED0 and LED1.
 * @param mode  available options are : 0 = init interface and reset all LEDs\n
 *                                      1 = set LED0\n
 *                                  other = set LED0 and LED1
 */
void DVK_SPI(uint8_t mode);
/*!
 * @}
*/
#endif /* __DVK_SPI_H__ */

/************************************** EOF *********************************/
