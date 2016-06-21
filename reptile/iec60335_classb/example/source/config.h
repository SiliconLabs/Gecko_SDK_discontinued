/***************************************************************************//**
 * @file    config.h
 * @brief   This file defines some essential configuration
 * @version 1.0.0
 *******************************************************************************
 * @section License
 * <b>Copyright 2015 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
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
