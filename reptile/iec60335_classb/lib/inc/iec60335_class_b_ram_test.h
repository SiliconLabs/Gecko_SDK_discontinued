/***************************************************************************//**
 * @file    iec60335_class_b_ram_test.h
 * @brief   header for c based RAM test
 *            POST and BIST RAM test for all compiler
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

#ifndef __IEC60335_CLASS_B_RAM_TEST_H__
#define __IEC60335_CLASS_B_RAM_TEST_H__

/*! @addtogroup IEC60335_RAM_TEST
 * @{
 * @image html RAM-Test.jpg
 */

#include "iec60335_class_b_typedef.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/*! @addtogroup IEC60335_CONFIG
 * @{
 */

/* BIST RAM buffer size in Byte */
#define IEC60335_RAM_buffersize    40
#define IEC60335_RAM_SP_Offset     (0x20UL)
/*!
 * @}
 */
/* ------------------------ */
/* RAM test suite status definitions */
#define TS_STA_RAM_BIST_OK          (0x00B00000UL)
#define TS_STA_RAM_BIST_R0_FAULT    (0x00B10000UL)
#define TS_STA_RAM_BIST_R1_FAULT    (0x00B20000UL)

#define BLOCK_SEL_CORE              0x1
#define BLOCK_SEL_PLINK             0x2

/* RAM algo constants */
#define initial_sp                  (*(uint32_t *) 0x00u)
#define get_initial_sp              (*initial_sp)
#define CORE_RAM_BASE_ADDR          IEC60335_RAM_START
#define CORE_RAM_BASE_PTR           (volatile uint8_t *) CORE_RAM_BASE_ADDR
#define CORE_RAM_MAX_BC             (1024 * 4 * 4)
#define CORE_RAM_R0_EXP             0x0
#define CORE_RAM_R1_EXP             (~CORE_RAM_R0_EXP)
#define PLINK_RAM_BASE_ADDR         (0x40012200UL)
#define PLINK_RAM_BASE_PTR          (volatile uint8_t *) PLINK_RAM_BASE_ADDR
#define PLINK_RAM_MAX_BC            384
#define PLINK_RAM_R0_EXP            0x0
#define PLINK_RAM_R1_EXP            0xFF

/*!
 * @brief   private function referenced by POST and BIST tests
 * @param   startAddr   start address in RAM memory (>0x20000000)
 * @param   size        size of RAM to check in bytes (<RAM_MEM_SIZE)
 * @param   block_sel   selection of RAM block to check, valid values are 
 *                  BLOCK_SEL_CORE for direct RAM access
 *                  BLOCK_SEL_PLINK for peripherals (ignoring size and startAddr)
 * @return passed or failed. See \link #testResult_enum testResult_t \endlink.
 * The function allows check in RAM area only. The stack can not be tested here.
 * If an area is tested with stack inside the stack area is excluded. Interrupts
 * during the RAM test may change stack size and corrupt the test.
 */
testResult_t IEC60335_ClassB_RAMtest(uint32_t startAddrs, uint32_t size, uint8_t block_sel);

/*!
 * @brief	POST public function testing the complete RAM
 * @return  passed or failed. See \link #testResult_enum testResult_t \endlink.
 * The function calls IEC60335_ClassB_RAMtest() with all available RAM memory as parameter.
 * The stack area is excluded from this test in fact there is no relevant stack 
 * depth expected at the POST test.
 */
extern testResult_t IEC60335_ClassB_RAMtest_POST(void);

/*!
 * @brief   BIST public function referenced by BIST tests
 * @param   startAddr   start address in RAM
 * @param   length      length of RAM to check in Bytes
 * @return passed or failed. See \link #testResult_enum testResult_t \endlink.
 * @image html RAM-Test_BIST.jpg
 * The stack area is excluded from this test.
 */
extern testResult_t IEC60335_ClassB_RAMtest_BIST(uint32_t startAddr, uint32_t length);

#ifdef __cplusplus
}
#endif /* __cplusplus */

/*!
 * @}
 */

#endif  /* __IEC60335_CLASS_B_RAM_TEST_H__ */

/************************************** EOF *********************************/
