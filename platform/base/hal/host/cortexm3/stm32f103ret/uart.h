/** @file hal/host/cortexm3/stm32f103ret/uart.h
 * See @ref serial and @ref stm32f103ret_uart for documentation.
 *
 * <!-- Copyright 2010 by Ember Corporation. All rights reserved.        *80*-->
 */

/** @addtogroup stm32f103ret_uart
 * @brief STM32F102RET host uart driver operating on top of ST's Standard
 * Peripheral Library; supporting IAR's standard library IO routines.
 *
 * See @ref serial for common documentation.
 *
 * See uart.h for source code.
 *@{
 */


#ifndef __UART_H__
#define __UART_H__
#include <yfuns.h>

/**
 * @brief Flush the output stream.  DLib_Config_Full.h defines fflush(), but
 * this library includes too much code so we compile with DLib_Config_Normal.h
 * instead which does not define fflush().  Therefore, we manually define
 * fflush() in the low level UART driver.  This function simply redirects
 * to the __write() function with a NULL buffer, triggering a flush.
 * 
 * @param handle  The output stream.  Should be set to 'stdout' like normal.
 *  
 * @return Zero, indicating success.
 */
size_t fflush(int handle);

/**
 * @brief Define the stdout stream.  Since we compile with DLib_Config_Normal.h
 * it does not define 'stdout'.  There is a low-level IO define '_LLIO_STDOUT'
 * which is equivalent to stdout.  Therefore, we define 'stdout' to be
 * '_LLIO_STDOUT'.
 */
#define stdout _LLIO_STDOUT

#endif //__UART_H__

/**@} //END addtogroup 
 */

