/***************************************************************************//**
 * @file    iec60335_class_b.h
 * @brief   header file containing all includes and setup for C based
 *              POST and BIST tests, global types and definitions
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

#ifndef __IEC60335_CLASS_B_H__
#define __IEC60335_CLASS_B_H__

/*! @defgroup IEC60335_LIB IEC60335 Class-B Library
 * @{
 */

/*!
 * @defgroup IEC60335_MODULES Library Requirements
 * @{
 * @brief main modules of the Class-B library.
 * @details This defines the complete\n
 * library including setup and functions. The modules are fixed on\n
 * @arg development environment
 * @arg physical target devices
 * @arg other internal components and structures\n
 *
 * @par formal parameter
 *
 * formal text and coding stile definitions
 * @arg indent size   : 2
 * @arg indent steps  : using characters instead of tab
 * @arg formal coding : following the C99 definition
 * @}
 * @defgroup IEC60335_CONFIG Library Configuration
 * @{
 * @brief  The modules are prepared for specific devices and compilers.
 * @details Changing any of this will cause non or mail functionality and requires\m
 * new compilation and tests.\n
 * Compiler specific issues must be regarded in concern to the user\n
 * application. Specific modules have their own configurations to keep\n
 * cross effects minimized.
 * @}
 * @defgroup IEC60335_POST POST Library Function
 * @{
 * @brief  The POST collects the main test routines for boot up test
 * @details POST is designed to be implemented directly after reset vector.\n
 * This collection of POST stage test functions will check the complete system before\n
 * any RAM or hardware initialization is done.
 * @}
 * @defgroup IEC60335_PC_TEST Program Counter Test
 * @{
 * @brief  The program counter test checks the functionality of the CPUs PC
 * @details POST and BIST tests are available. The PC test is realized in assembler\n
 * language to ensure direct access to the CPUs register.\m
 * This tests are compiler coded specific according to their mnemonics.\n
 * Selecting the correct assembler source is necessary for compilation.\n
 * @}
 * @defgroup IEC60335_CPUREG_TEST CPU Register Test
 * @{
 * @brief  The CPU register test checks the functionality of the CPUs registers
 * @details POST and BIST tests are available. The register test is realized in assembler\n
 * language to ensure direct access to all register.\m
 * This tests are compiler coded specific according to their mnemonics.\n
 * Selecting the correct assembler source is necessary for compilation.\n
 * @}
 * @defgroup IEC60335_CRITICAL_DATA Critical Data
 * @{
 * @brief  The Critical Data macros provide secured handling to critical
 * user data in BIST condition.
 * @details critical data helps to keep data consistent and enables verification structures\n
 * for easy and save data access.
 * @}
 * @defgroup IEC60335_FLASH_TEST FLASH Test
 * @{
 * @brief  The FLASH test checks reliability and consistency of FLASH data\n
 * and code.
 * @details using POST and BIST Flash test algorithm enables consistency checking features\n
 * on the nonvolatile memory of the chip. all tests references to a predefined CRCValue\n
 * calculated outside the runtime of the application. The CRCValue is stored in a structure\n
 * outside the checked code.
 * @}
 * @defgroup IEC60335_RAM_TEST RAM Test
 * @{
 * @brief  The RAM test checks functionality of RAM cells
 * @details using the production test of the chip vendor all possible stuck and coupling\n
 * effects are tested.
 * @}
 * @defgroup IEC60335_IRQ_TEST Interrupt Test
 * @{
 * @brief  The interrupt test checks functionality of NVIC
 * @details Providing an intermediate vector which is used to check the occurrence of an\n
 * interrupt the original vector can be configured, so interrupts can be simulated
 * @}
 * @defgroup IEC60335_TIMER_RTC_TEST Timer/RTC Test
 * @{
 * @brief  The interrupt test checks functionality of a Timer\n
 * @details RTC is checked against the selected timer device by running both with a specified\n
 * number of counts with tolerance during a defined period of the RTC
 * @}
 * @defgroup IEC60335_WDOG_TEST Watchdog Timer Test
 * @{
 * @brief  POST test of WDOG unit and processing a reset\n
 * @details starting the WDOG will force a reset of the chip. a second entry to the functions will\n
 * be checked for a WDOG reset reason.
 * @}
 * @defgroup IEC60335_TOOLS Tools to test the library
 * @{
 * @brief  testing the library requires some helping functions on the hardware\n
 *         showing the functionality of the tests.
 * @details Generic functions like memcpy etc. should not be used from libraries.
 * @}
 */

#include "em_device.h"
#ifdef TESSY
#include "iec60335_class_b_tessy.h"
#endif
#include "iec60335_class_b_def.h"
#include "iec60335_class_b_typedef.h"
#include "iec60335_class_b_programcounter_test.h"
#include "iec60335_class_b_cpureg_test.h"
#include "iec60335_class_b_interrupt_test.h"
#include "iec60335_class_b_ram_test.h"
#include "iec60335_class_b_flash_test.h"
#include "iec60335_class_b_timer_rtc_test.h"
#include "iec60335_class_b_critical_data.h"
#include "iec60335_class_b_wdt_test.h"
#include "iec60335_class_b_post.h"

/* applicational */
#include "dvk_spi.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
/*! @addtogroup IEC60335_CONFIG
 * @{
 */
/*	User configuration	*/
#define CPU_CLK               (32000000)        /*!< CPU main clock */
/* definition from EFM32 library */
#ifndef FLASH_SIZE
#define FLASH_SIZE            (0x00020000UL)    /*!< size of FLASH memory */
#endif

#define IEC60335_RAM_START    (0x20000000UL)    /*!< start address of RAM memory */
#ifndef SRAM_SIZE
#define SRAM_SIZE             (0x00004000UL)    /*!< size of RAM memory */
#endif
#define IEC60335_RAM_SIZE     SRAM_SIZE         /*!< size of RAM memory in kBytes */

/* POST test configuration */
#define TEST_POST_WDOG        0                 /*!< WatchDog test enabled = 1, disabled = 0 */
#define TEST_POST_FLASH       0                 /*!< FLASH test enabled = 1, disabled = 0 */
#define TEST_POST_RAM         1                 /*!< RAM test enabled = 1, disabled = 0 */
#define TEST_POST_CPUREG      1                 /*!< Register test enabled = 1, disabled = 0 */
#define TEST_POST_PC          1                 /*!< Program Counter test enabled = 1, disabled = 0 */

/* configuration of the timer tests */
#define USE_SYSTICK           0                 /*!< specifies timer test with systick */
#define USE_TIMER0            1                 /*!< specifies timer test with TIMER0 */
#define USE_TIMER1            0                 /*!< specifies timer test with TIMER1 */

/* End of user configuration */
/*!
 * @}
 */

/*! @addtogroup IEC60335_TOOLS
 * @{
 */

/*!
 * @brief   public inline function referenced by BIST test
 *          A memcpy() function, if library is not used
 * @param   [in, out]  t uint8 *          ;Pointer to target buffer
 * @param   [in, out]  s uint8 *          ;Pointer to source buffer
 * @param   [in]       byte_count uint32  ;Byte count to copy from source to target
 * @remarks Does not detect overlapping source/target pointers
 */
static __INLINE void util_memcpy(void * t, void * s, uint32_t byte_count)
{
  uint8_t * ptr_t = (uint8_t *) t;
  uint8_t * ptr_s = (uint8_t *) s;
  while (byte_count)
  {
    byte_count--;
    *ptr_t = *ptr_s;
    ptr_s++;
    ptr_t++;
  }
}
/*!
 * @}
 */
#ifdef __cplusplus
}
#endif /* __cplusplus */

/*!
 * @}
 */

#endif  /* __IEC60335_CLASS_B_H__ */

/************************************** EOF *********************************/
