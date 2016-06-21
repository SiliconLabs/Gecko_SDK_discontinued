/***************************************************************************//**
 * @file    iec60335_class_b_cpureg_test.c
 * @brief   IEC60335 Class B CPU register tests
 *             C based POST and BIST CPU register test for all compiler
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
#include <stdint.h>
#include "iec60335_class_b_cpureg_test.h"

/*! @addtogroup IEC60335_CPUREG_TEST
 * @{
 */

IEC60335_CPUreg_struct IEC60335_CPUregTestPOST;
IEC60335_CPUreg_struct IEC60335_CPUregTestBIST;

testResult_t IEC60335_ClassB_CPUregTest_POST(void)
{
  testResult_t Result = IEC60335_testFailed;
  ASMCPUregTestPOST();

  Result = IEC60335_CPUregTestPOST.testResult;

  return(Result);
}

testResult_t IEC60335_ClassB_CPUregTest_BIST(void)
{
  /* Clear the current test states */
  IEC60335_CPUregTestBIST.testState  = IEC60335_statePending;
  IEC60335_CPUregTestBIST.testResult = IEC60335_testFailed;

  /* Test the low registers r0-r7 */
  ASMCPUregTestLOW();
  if (IEC60335_CPUregTestBIST.testResult != IEC60335_testPassed)
  {
    return(IEC60335_testFailed);
  }

  /* Clear the PASS bit */
  IEC60335_CPUregTestBIST.testResult = IEC60335_testFailed;
  /* Test the high registers r8-r12 */
  ASMCPUregTestHIGH();
  if (IEC60335_CPUregTestBIST.testResult != IEC60335_testPassed)
  {
    return(IEC60335_testFailed);
  }

  /* Clear the PASS bit */
  IEC60335_CPUregTestBIST.testResult = IEC60335_testFailed;
  /* Test the stack pointer registers
   * NOTE: DISABLES ALL INTERRUPTS!
   */
  ASMCPUregTestSP();
  if (IEC60335_CPUregTestBIST.testResult != IEC60335_testPassed)
  {
    return(IEC60335_testFailed);
  }

  /* Clear the PASS bit */
  IEC60335_CPUregTestBIST.testResult = IEC60335_testFailed;
  /* Test the Special registers */
  ASMCPUregTestSPEC();
  if (IEC60335_CPUregTestBIST.testResult != IEC60335_testPassed)
  {
    return(IEC60335_testFailed);
  }

  /* Indicate all tests passed */
  IEC60335_CPUregTestBIST.testResult = IEC60335_testPassed;
  IEC60335_CPUregTestBIST.testState  = IEC60335_stateDone;

  return(IEC60335_testPassed);
}

/*!
 * @}
 */

/************************************** EOF *********************************/
