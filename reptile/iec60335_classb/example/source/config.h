/***************************************************************************//**
 * @file    config.h
 * @brief   This file defines some essential configuration
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

#ifndef _CONFIG_H_
#define _CONFIG_H_

/*! @addtogroup IEC60335_CONFIG
* @{
*/

#ifndef DEBUG
//#define DEBUG                 /*!< should be set by the compiler settings */
#endif

// ----------------------
/*! debugging */
#define USE_SEMIHOSTING     0   /*!< switch on/off SWV feature, "0" to deactivate */
#define USE_CMSIS_NVIC      1   /*!< Use the CMSIS defined functions for NVIC processing */
#define DEBUG_WARNING       0   /*!< switch on/off debug code detection, "0" to deactivate */
#define DEBUG_IDLE_TIME     1   /*!< switch on/off debug code for idle time, "0" to deactivate */

/*! compiler specific */
#define STACKSIZE             (2*1024)     /* GCC only */
#define PRIVILEGED_VAR        __attribute__ ((section (".data.privileged")))
// ----------------------
#define REVISION              SVN_REVISION_NUMBER
#define LIB_REVISION          LIB_REVISION_NUMBER


#if (bad_configuration == 1)
#if defined ( __CC_ARM   )
    #pragma diag_remark 2
#else
#pragma message(":error: bad configuration in config.h")
#endif
#endif

/*!
* @}
*/

#endif /* _CONFIG_H_ */

/************************************** EOF *********************************/
