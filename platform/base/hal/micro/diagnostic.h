/** @file hal/micro/diagnostic.h
 * See @ref diagnostics for detailed documentation.
 *
 * <!-- Copyright 2004 by Ember Corporation. All rights reserved.-->   
 */
 
/** @addtogroup diagnostics
 * @brief Program counter diagnostic functions.
 *
 * Unless otherwise noted, the EmberNet stack does not use these functions, 
 * and therefore the HAL is not required to implement them. However, many of
 * the supplied example applications do use them.
 *
 * Use these utilities with microcontrollers whose watchdog circuits do not make 
 * it possible to find out where they were executing when a watchdog was triggered.
 * Enabling the program counter (PC) diagnostics enables a periodic timer 
 * interrupt that saves the current PC to a reserved, unitialized memory address.  
 * This address can be read after a  reset. 
 *
 * Some functions in this file return an EmberStatus value. See 
 * error-def.h for definitions of all EmberStatus return values.
 *
 * See hal/micro/diagnostic.h for source code.
 *
 *@{
 */
 
#ifndef __DIAGNOSTIC_H__
#define __DIAGNOSTIC_H__

/** @brief Starts a timer that initiates sampling the 
 * microcontroller's program counter.
 */
void halStartPCDiagnostics(void);

/** @brief Stops the timer that was started by 
 * halStartPCDiagnostics().
 */
void halStopPCDiagnostics(void);

/** @brief Provides a captured PC location from the last run. 
 *   
 * @appusage Call this function when an application starts, before
 * calling halStartPCDiagnostics().
 *  
 * @return The most recently sampled program counter location. 
 */
uint16_t halGetPCDiagnostics(void);

#ifdef ENABLE_DATA_LOGGING
// A pair of utility functions for logging trace data
void halLogData(uint8_t v1, uint8_t v2, uint8_t v3, uint8_t v4);
void halPrintLogData(uint8_t port);
#endif

#endif //__DIAGNOSTIC_H__

/**@} // End addtogroup
 */



