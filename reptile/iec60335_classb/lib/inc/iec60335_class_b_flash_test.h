/***************************************************************************//**
 * @file    iec60335_class_b_flash_test.h
 * @brief   header for c based test for internal FLASH
 *              POST and BIST internal FLASH test for all compiler
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

#ifndef __IEC60335_CLASS_B_FLASH_TEST_H__
#define __IEC60335_CLASS_B_FLASH_TEST_H__

/*! @addtogroup IEC60335_FLASH_TEST
 * @{
 * @image html Flash-Test.jpg
 */


#include "iec60335_class_b_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif
/*! @addtogroup IEC60335_CONFIG
 * @{
 */

#define dynamic_CRC_table    0
/*!
 * @}
 */

#define SIZE32K              0x00007FFF
#define SIZE64K              0x0000FFFF
#define SIZE128K             0x0001FFFF

#define FLASHSIZE            SIZE128K

#define FLASH_SIGN1          1
#define FLASH_SIGN2          2

#define CRC_block_size       1024

#define FLASH_CRC_Done       1          /*!< status information of this struct */
#define FLASH_CRC_InProg     2
#define FLASH_CRC_Missing    3
#define FLASH_CRC_Valid      4
#define FLASH_CRC_Restart    5

/*! @addtogroup IEC60335_CONFIG
 * @{
 */
/*!
 * @brief     struct defining the FLASH content with CRC, size etc.
 * @attention this struct must be filled correctly with values before using in the Flash tests.
 * CRC32Val: this can be calculated by firmware (function IEC60335_ClassB_FLASHtest_POST) or external tools
 * Status  : set to valid to run the test
 */
/*  predefined               CRC32Val  , MemLenght, NextAddress, BlockSize,  Status; */
#define ENTRY_FLASH_CRC    { 0x0A59B834, FLASHSIZE, (uint32_t) __STEXT, FLASHSIZE, FLASH_CRC_Valid }
/*!
 * @}
 */

/*!
 * @brief   private function calculating CRC32 table based checksum by POST and BIST tests
 * @param   crc         start CRC32 value if partial usage
 * @param   src         pointer to start of the memory to check
 * @param   len         length of processed memory in byte
 * @return  uint32_t    CRC32 calculated checksum
 * @attention  the routine cahnges the content of the src pointer to the next address in the
 *             runtime. As a result the next valid address to check can be read out of  the scr
 *             pointers content
 */
uint32_t crc32(uint32_t crc,uint8_t ** const pp_src, uint32_t len);

/*!
 * @brief   public function to check the Flash content POST only
 *          by recalculation and comparison with Flash stored CRC32 value
 * @return passed or failed. See \link #testResult_enum testResult_t \endlink.
 * @attention FLASHSIZE must be configured in the file iec60335_class_b.h and
 *            linker label _stext must be available
 */
testResult_t IEC60335_ClassB_FLASHtest_POST(void);

/*!
 * @brief   public function (competitive) to check a part of the Flash content
 * @param   StartMode   flag to restart the BIST test
 *                      available values are : FLASH_CRC_Restart restarts the calculation
 *                      all other will continue to test next addresses
 * @return  passed or failed. See \link #testResult_enum testResult_t \endlink.
 * @note    the memory section which is processed is defined in the struct bistFlashCRC
 */
testResult_t IEC60335_ClassB_FLASHtest_BIST(uint8_t StartMode);

#ifdef __cplusplus
}
#endif /* __cplusplus */

/*!
 * @}
 */

#endif /* __IEC60335_CLASS_B_FLASH_TEST_H__ */

/************************************** EOF *********************************/
