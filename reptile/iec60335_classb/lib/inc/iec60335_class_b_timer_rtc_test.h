/***************************************************************************//**
 * @file    iec60335_class_b_timer_rtc_test.h
 * @brief   header file for clock structure test
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
#ifndef __IEC60335_CLASS_B_TIMER_RTC_TEST_H__
#define __IEC60335_CLASS_B_TIMER_RTC_TEST_H__

/*! @addtogroup IEC60335_TIMER_RTC_TEST
 * @{
 */

#include "iec60335_class_b_typedef.h"

extern ClockTest_t ClockTest;

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/*!
 * @brief   This function initializes the IEC60335 Class B clock test
 *          for BIST usage
 *          all test structures are reset to defined values
 *
 * @param   ratio           setup value for timer test ratio to RTC in counter ticks
 * @param   tolerance       setup value for deviation tolerance in percent
 * @attention  This function must be called once inside the main loop.
 */
void IEC60335_ClassB_initClockTest(uint32_t ratio, uint32_t tolerance);
/*!
 * @brief   This function represents the part of the IEC60335 Class B clock test
 *          which has to be executed within the main loop.
 *  It checks several thing:
 *  - If the clock test timer interrupts are triggered
 *  - If the clock test RTC interrupt is triggered
 *  - or, If in any of the two interrupts an error was detected...
 *
 * @attention   This function must be called once inside the main loop.
 *
 * @attention For this function, it is necessary to estimate the count how often this function could be called.
 * This is important to find
 *  valid threshold values, which are used to test timer and RTC interrupt occurrence.
 *
 * @return passed or failed. See \link #testResult_enum testResult_t \endlink.
 */
testResult_t IEC60335_ClassB_Clocktest_PollHandler(void);

/*!
 * @brief   This function is intended to use as timer interrupt service handler or to be
 *          called once inside the timer interrupt service handler.
 */
__INLINE static void IEC60335_ClassB_Clocktest_TimerHandler(void)
{
  /* Increment counter necessary in test*/
  ClockTest.timerCounter++;
}

/*!
 * @brief   This function is intended to be called once inside the custom RTC interrupt service handler.
 *
 *  This function tests the timer time frame and in this case the CPU frequency.
 *
 *  Also, this function checks if the main loop function was called.
 */
void IEC60335_ClassB_Clocktest_RTCHandler(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

/*!
 * @}
 */

#endif  /* __IEC60335_CLASS_B_TIMER_RTC_TEST_H__ */

/************************************** EOF *********************************/
