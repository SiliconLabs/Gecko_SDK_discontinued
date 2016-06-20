/***************************************************************************//**
 * @file    main.h
 * @brief   main header code for applications based on Cortex M3
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
#ifndef __MAIN_H__
#define __MAIN_H__
/*! @defgroup IEC60335_EXAMPLE IEC60335 Class-B Library Example
* @{
* @brief The IEC60335 Class-B Library Example is a formal and simple software\n
* to show the usage of the iec60335_classb library.\n
* Easy and simple coded functions are showing most of the library functions.
* To run the example the following conditions are required:\n
* -# HiTOP for ARM Cortex or similar version and Debugging interface
* -# Hitex GCC 4.5.0 compiler for cortex
* -# Evaluation hardware from Energy Micro populated with EFM232G890F128 device
* -# knowledge of Cortex in general and the EFM device specificly
* Several Projects are implemented for the following tool chains:
* -# HiTOP for Cortex
* -# IAR Kickstart 6.1
* -# Keil uVision 4
* All the projects are related to the same source code. Special compiler specific\n
* mnemonics are defined and activated by the compiler by setting special defines\n
* during compilation and link process.
* Special focus must be set to the configuration and CMSIS library extension.
*/
void Reset(void); 
/*! 
* @defgroup IEC60335_TOOLS Library Tool functions 
* @{
* @brief the software includes functions to demonstrate the libraries capabilities\n
* on the real hardware. The modules are fixed on\n
* the Gecko Evalboard from Energy Micro.
* @}
* @defgroup IEC60335_CONFIG Library Configuration
* @{
* @brief  This modules are prepared for specific devices and compilers.\n
* Changing any of this will cause non or mail functionality and requires\m
* new compilation and tests.\n
* Compiler specific issues must be regarded in concern to the user\n
* application. Specific modules have their own configurations to keep\n
* cross effects minimized.
* -# development environment
* -# physical target devices
* -# other internal components and structures like memory size
* -# special conditions (Tessy)
* @}
* @defgroup INT CMSIS Library Extension (Interrupts)
* @{
* @brief  This modules is an extension to the EFM32 library.\n
* Here interrupt handler and exceptions are defined for direkt usage\n
* under CMSIS compliant software.
* @defgroup EXCEPTIONS Exceptions and default vectors
* @{
* @brief  This module defines a set of default handlers for the device\n
* specific interrupts. The module is an extension for CMSIS library.\n
* The handler can be replaced because they are defined "weak".
* @}
* @}
*/

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifdef __cplusplus
}
#endif /* __cplusplus */
/*!
* @}
*/
#endif /* __MAIN_H__ */

/************************************** EOF *********************************/
