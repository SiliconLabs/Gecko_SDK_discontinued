/***************************************************************************//**
 * @file    iec60335_class_b_programcounter_test.h
 * @brief   header for c based PC test
 *              POST and BIST PC test for all compiler
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
