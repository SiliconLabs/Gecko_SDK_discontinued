/** @file hal/host/cortexm3/stm32f103ret/stm32f10x_conf.h
 * @ref host and @ref stm32f103ret_host for documentation.
 */ 

/** @addtogroup stm32f10x_conf
 * @brief ST Microcontroller's Standard Peripherals Library inclusions and
 * definitions.
 *
 * This file is included from ST's Standard Peripherals Library and includes
 * the headers for the peripherals found in ST's Library.  It also
 * defines the assert macro used by ST's Library.  The actual documentation
 * for ST's Standard Peripherals Library is beyond the scope of Ember's
 * documentation.
 *
 * @note While this file's name, stm32f10x_conf.h, does not conform to
 * Ember's file naming convention, this file is included from ST's Standard
 * Peripherals Library.  Not renaming this file means the library does
 * not have to be modified.
 *
 * See @ref host for common documentation.
 *
 * See stm32f10x_conf.h for source code.
 *@{
 */


#ifndef __STM32F10x_CONF_H
#define __STM32F10x_CONF_H


//Peripheral header file inclusion.  There is a header per peripheral source
//found in the library.
#include "stm32f10x_adc.h"
#include "stm32f10x_bkp.h"
#include "stm32f10x_can.h"
#include "stm32f10x_cec.h"
#include "stm32f10x_crc.h"
#include "stm32f10x_dac.h"
#include "stm32f10x_dbgmcu.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_flash.h"
#include "stm32f10x_fsmc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_i2c.h"
#include "stm32f10x_iwdg.h"
#include "stm32f10x_pwr.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_rtc.h"
#include "stm32f10x_sdio.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_wwdg.h"
//misc.h is for High level functions for NVIC and SysTick, which 
//are add-on to CMSIS functions.
#include "misc.h"


//The library uses it's own assert macro (assert_param), so link the library's
//assert to our usual assert.
#if !defined(SIMPLER_ASSERT_REBOOT)
  /**
   * @brief A prototype definition of the Ember assert function for use
   * by the assert_param macro.
   */
  void halInternalAssertFailed(const char * filename, int linenumber);
  
  /**
   * @brief  The assert_param macro is used by ST's Library to check a
   * function's parameters.  This macro redirects to Ember's assert function.
   * This macro redirect is the same definition of assert as used
   * in the the PLATFORM_HEADER.
   */
  #define assert_param(condition)                              \
        do {                                                   \
          if (! (condition)) {                                 \
            halInternalAssertFailed(__SOURCEFILE__, __LINE__); \
          }                                                    \
        } while(0)
#else
  #define assert_param(condition) \
            do { if( !(condition) ) while(1){} } while(0)
#endif


#endif /* __STM32F10x_CONF_H */

/**@} //END addtogroup 
 */

