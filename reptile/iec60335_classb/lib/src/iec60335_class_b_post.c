/***************************************************************************//**
 * @file    iec60335_class_post.c
 * @brief   IEC60335 Class B c based POST test collection
 *              POST test for all compiler
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

#include "iec60335_class_b.h"

/*! @addtogroup IEC60335_POST
 * @{
 */

/*!
 *  This function is intended to use as an additional initialization routine
 *  The function will stuck at error condition
 */
/*! @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
#if defined (__ICCARM__)
    #pragma section="IEC60335_code"
#else
__attribute__((section(".IEC60335_code")))
#endif
/*! @endcond (DO_NOT_INCLUDE_WITH_DOXYGEN) */
void IEC60335_ClassB_POST(void)
{
  volatile testResult_t result = IEC60335_testFailed;   /* variable is located in the stack */
#if ((TEST_POST_WDOG == 1) || (TESSY))
/*! @cond TEST_POST_WDOG */
  result = IEC60335_ClassB_Force_WDT_Reset();           /* test WDOG with reset */

  if (result == IEC60335_testInProgress)
  {
    /* enable LED0 */
    DVK_SPI(0);                                         /* enable SPI */
    DVK_SPI(1);                                         /* enable LED0 */
  }
  if (result != IEC60335_testPassed)
  {
    LoopForever();                                      /* remains until WDOG test passes */
  }
  /* enable LED0 and LED1 */
  DVK_SPI(0);                                           /* enable SPI */
  DVK_SPI(2);                                           /* enable LED0,1 */
/*! @endcond (TEST_POST_WDOG) */
#endif

#if (TEST_POST_FLASH == 1)
/*! @cond TEST_POST_FLASH */
  result = IEC60335_testFailed;
  result = IEC60335_ClassB_FLASHtest_POST();            /* test FLASH content */

/* TODO : insert correct CRC value to line 80 in iec60355_class_b_flash_test.h */
  if (result != IEC60335_testPassed)
  {
    LoopForever();                                      /* remains if Flash content is not consistent */
  }
/*! @endcond (TEST_POST_FLASH) */
#endif

#if (TEST_POST_RAM == 1)
/*! @cond TEST_POST_RAM */
  result = IEC60335_testFailed;
  result = IEC60335_ClassB_RAMtest_POST();              /* stack is not tested here */

  if (result != IEC60335_testPassed)
  {
    LoopForever();                                      /* remains if RAM test fails */
  }
/*! @endcond (TEST_POST_RAM) */
#endif
#if (TEST_POST_CPUREG == 1)
/*! @cond TEST_POST_CPUREG */
  result = IEC60335_testFailed;
  result = IEC60335_ClassB_CPUregTest_POST();

  if (result != IEC60335_testPassed)
  {
    LoopForever();                                      /* remains if registers test fails */
  }
/*! @endcond (TEST_POST_CPUREG) */
#endif
#if (TEST_POST_PC == 1)
/*! @cond TEST_POST_PC */
  result = IEC60335_testFailed;
  result = IEC60335_ClassB_PCTest_POST(PCTest_funcTable, PCTest_resultTable);

  if (result != IEC60335_testPassed)
  {
    LoopForever();                                      /* remains if PC test fails */
  }
/*! @endcond (TEST_POST_PC) */
#endif
}

/*!
 * @}
 */

/************************************** EOF *********************************/
