/**************************************************************************//**
 * @file
 * @brief LCD demo and test, prototypes and definitions
 * @version 5.1.2
 ******************************************************************************
 * @section License
 * <b>Copyright 2015 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#ifndef __LCDTEST_H
#define __LCDTEST_H

#ifdef __cplusplus
extern "C" {
#endif

/** Timer used for bringing the system back to EM0. */
extern RTCDRV_TimerID_t xTimerForWakeUp;

/* Test patterns */
void ScrollText(char *scrolltext);
void Test(void);
void BlinkTest(void);
void AnimateTest(void);

#ifdef __cplusplus
}
#endif

#endif
