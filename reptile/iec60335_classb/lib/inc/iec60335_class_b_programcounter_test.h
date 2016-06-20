/***************************************************************************//**
 * @file    iec60335_class_b_programcounter_test.h
 * @brief   header for c based PC test
 *              POST and BIST PC test for all compiler
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

#ifndef __IEC60335_CLASS_B_PROGRAMCOUNTER_TEST_H__
#define __IEC60335_CLASS_B_PROGRAMCOUNTER_TEST_H__

/*! @addtogroup IEC60335_PC_TEST
 * @{
 * @image html PC-Test.jpg
 */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

 #ifdef TESSY
/*! @cond TESSY */
extern Exec_t PCTest_funcTable[6];
extern IEC60335_B_PCTest_ReturnValues PCTest_resultTable[6];
/*! @endcond */
#else
extern const Exec_t                         PCTest_funcTable[6];

extern const IEC60335_B_PCTest_ReturnValues PCTest_resultTable[6];
#endif

/*!
 * @brief   public function referenced by POST test
 * @param   fTable  Flash location of subroutine table
 * @param   rTable  Flash location of expected return values
 * @return  passed or failed. See \link #testResult_enum testResult_t \endlink.
 * @note    The function is placed in a specified code segment to force
 * the linker to locate it separately. The function saves input parameter,
 * calls subroutines, compares the return values with the according table
 * and returns a good or error state. The function is realized in "inline assembler"
 * and a corresponding C-source is provided for better understanding.
 */
/*!
 * @brief   POST test of PC.
 *
 * @attention   for PC test, the C environment must be loaded!
 * @return      passed or failed. See \link #testResult_enum testResult_t \endlink.
 */
testResult_t IEC60335_ClassB_PCTest_POST(const Exec_t *fTable, const IEC60335_B_PCTest_ReturnValues *rTable);
/*!
 * @brief   BIST test of PC.
 *
 * @attention   for PC test, the C environment must be loaded!
 * @return      passed or failed. See \link #testResult_enum testResult_t \endlink.
 * @note    this test checks a one step branch to a sub-function
 *          the test performs different calls on each activation
 */
testResult_t IEC60335_ClassB_PCTest_BIST(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

/*!
 * @}
 */

#endif  /* __IEC60335_CLASS_B_PROGRAMCOUNTER_TEST_H__ */

/************************************** EOF *********************************/
