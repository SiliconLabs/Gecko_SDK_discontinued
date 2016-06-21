/***************************************************************************//**
 * @file    iec60335_class_b_cpureg_test.h
 * @brief   IEC60335 Class B CPU register tests
 *              header for C based POST and BIST CPU register test for all compiler
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

#ifndef __IEC60335_CLASS_B_CPUREG_TEST_H__
#define __IEC60335_CLASS_B_CPUREG_TEST_H__

/*! @addtogroup IEC60335_CPUREG_TEST
 * @{
 * @image html CPU-Register-C.jpg
 */

#include "iec60335_class_b_typedef.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/*! @brief ordered register test states to verify all tests are done
 * after the CPU register POST test or determine the test which fails
 * (order from LSB up)
 */
#define TestClear             0x00000000            /*!< default test mask */
#define TestState_r0          0x00000010            /*!< R0 register test mask */
#define TestState_r1r7        0x00000020            /*!< R8..R7 register test mask */
#define TestState_r8r12       0x00000040            /*!< R8..R12 register test mask */
#define TestStateMSP          0x00000100            /*!< MSP register test mask */
#define TestStatePSP          0x00000200            /*!< PSP register test mask */
#define TestStateLR           0x00000400            /*!< LR register test mask */
#define TestStateAPSR         0x00000800            /*!< APSR register test mask */
#define TestStatePRIMASK      0x00001000            /*!< PRIMASK register test mask */
#define TestStateFAULTMASK    0x00002000            /*!< FAULTMASK register test mask */
#define TestStateBASEPRI      0x00004000            /*!< BASEPRI register test mask */

/*!
 * @brief   private IEC607335 CPU register test POST (Pre-Operation Self Test)
 *          consecutive test of all registers in assembler
 */
extern void ASMCPUregTestPOST(void);

/*!
 * @brief   private IEC607335 CPU register test
 *          Low-range registers tests r0-r7
 *
 *          r0 register test block
 *          r1-r7 test with pattern comparison
 * @image html CPU-Register_1_4.jpg
 */
extern void ASMCPUregTestLOW(void);

/*!
 * @brief   private IEC607335 CPU register test
 *          Mid-range registers tests r8-r12
 *
 *          r8-r12 test with pattern comparison
 */
extern void ASMCPUregTestHIGH(void);

/*!
 * @brief   private IEC607335 CPU register test
 *          Stack pointer registers tests MSP,PSP
 *
 *          MSP register tests r/w r13, MSR and MRS
 */
extern void ASMCPUregTestSP(void);

/*!
 * @brief   private IEC607335 CPU register test
 *          Other special register tests
 *
 *          LR register test written with pattern1, compared, and inverse
 */
extern void ASMCPUregTestSPEC(void);

/*!
 * @brief   public IEC607335 CPU register test BIST (Build in Self Test)
 *          consecutive test of all registers in assembler are called
 * @return passed or failed. See \link #testResult_enum testResult_t \endlink.
 * @image html CPU-Register_BIST_LOW.jpg
 * @image html CPU-Register_BIST_HIGH.jpg
 * @image html CPU-Register_BIST_SP.jpg
 * @image html CPU-Register_BIST_SFR1.jpg
 * @image html CPU-Register_BIST_SFR2.jpg
 */
testResult_t IEC60335_ClassB_CPUregTest_BIST(void);

/*!
 * @brief   public IEC607335 CPU register test POST (Pre-Operation Self Test)
 *          consecutive test of all registers in assembler are called
 * @return passed or failed. See \link #testResult_enum testResult_t \endlink.
 * @image html CPU-Register_1_4.jpg
 * @image html CPU-Register_2_4.jpg
 * @image html CPU-Register_3_4.jpg
 * @image html CPU-Register_4_4.jpg
 */
testResult_t IEC60335_ClassB_CPUregTest_POST(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

/*!
 * @}
 */

#endif /* __IEC60335_CLASS_B_CPUREG_TEST_H__ */

/************************************** EOF *********************************/
