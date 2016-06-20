/***************************************************************************//**
 * @file    iec60335_class_b_wdt_test.h
 * @brief   header file for WDT structure test
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
#ifndef __IEC60335_CLASS_B_WDT_TEST_H__
#define __IEC60335_CLASS_B_WDT_TEST_H__

#include "iec60335_class_b.h"

/*! @addtogroup IEC60335_WDOG_TEST
 * @{
 */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/*!
 * @brief   This function initializes the IEC60335 Class B WDT test
 *          for POST usage\n
 *          the function will be called after reset and checks the reset conditions\n
 * The function will enable the WDOG module if the last reset condition was POR or EXT.\n
 * In this case the WDOG start counting and will cause a WDOG reset after a period of time.\n
 * Residing in the POST function the routine now will check for WDOG reset and return a pass result.
 * @return passed or failed. See \link #testResult_enum testResult_t \endlink.
 * @note        debugging can affect the WDT behavior defined in the DEBUGRUN bit in the WDOG_CTRL register.
 * @attention   This function must not be called outside the POST tests.
 * @image html WDOG-init.jpg
 */
testResult_t IEC60335_ClassB_initWDT(void);

/*!
 * @brief   This function represents the part of the IEC60335 Class B WDT test
 *          which has to be executed within a loop. It refreshes the WDT counter.
 *
 * @attention This function must be called periodically inside a loop.
 *
 * @attention For this function, it is necessary to estimate the count how often this function could be called.
 * @image html WDOG-refresh.jpg
 */
void IEC60335_ClassB_Refresh_WDT(void);

/*!
 * @brief   This function starts the Watchdog and waits for positive reset conditions.
 * @return  passed or failed. See \link #testResult_enum testResult_t \endlink.
 * @note    POST use only! The function will lead to a reset.
 * @attention The function may not work in debugging mode. The reset cause of the debugge is not obtained and\n
 *            debugger interface may affect the WDOG runing conditions. If the reset occures the debugger may\n
 *            fail reconnect the target without resetting it again.
 * @image html WDOG-force.jpg
 */
testResult_t IEC60335_ClassB_Force_WDT_Reset(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

/*!
 * @}
 */

#endif  /* __IEC60335_CLASS_B_WDT_TEST_H__ */

/************************************** EOF *********************************/
