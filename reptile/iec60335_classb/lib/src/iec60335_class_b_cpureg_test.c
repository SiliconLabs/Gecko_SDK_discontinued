/***************************************************************************//**
 * @file    iec60335_class_b_cpureg_test.c
 * @brief   IEC60335 Class B CPU register tests
 *             C based POST and BIST CPU register test for all compiler
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
