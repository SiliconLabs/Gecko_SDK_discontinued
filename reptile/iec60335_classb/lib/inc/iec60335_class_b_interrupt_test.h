/***************************************************************************//**
 * @file    iec60335_class_b_interrupt_test.h
 * @brief   header for c based interrupt test
 *              POST and BIST interrupt test for all compiler
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

#ifndef __IEC60335_CLASS_B_INTERRUPT_TEST_H__
#define __IEC60335_CLASS_B_INTERRUPT_TEST_H__

#include "iec60335_class_b.h"

/*! @addtogroup IEC60335_IRQ_TEST
 * @{
 */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#define IgnoreIRQHandler    0   /*!< original handler is ignored */
#define CallIRQHandler      1   /*!< original handler is called */

/*!
 * @brief  call the original vector
 *
 * @param vect IRQ index
 * @image html Interrupt_call.jpg
 */
/* void __call_Vect(uint32_t vect); */

/*!
 * @brief   Interrupt replacement handler to check interrupt occurrences
 * @note    The interrupt test requires an intermediate call and work on the test data.
 *          To achieve this goal the replacement handler is called instead of the user handler.
 *          If the corresponding Mode flag is set the routine calls the original vector handler.
 * @image html Interrupt_Replacement.jpg
 */
void IEC60335_IRQReplacementHandler(void);

/*!
 * @brief   Initialization of a specified interrupt test
 * @param   IRQn            number of the interrupt to test\n
 *                          if number smaller than "SysTick_IRQn" the function deactivates all tests\n
 *                          numbers bigger than "PendSV_IRQn" activates the test\n
 *                          calling a second time with the same IRQn will deactivate this test
 * @param   Mode            configure interrupt is calling the original handler
 * @param   CountSetup      counters of interrupt occurrences
 * @note    The interrupt test is deactivated when called with a exception number (< SysTick_IRQn) on the IRQn parameter.
 *          In this case the original vector table is restored and all flags are deleted.
 * @attention Ensure to place the function call #IEC60335_ClassB_InterruptOcurred into the corresponding
 * interrupt service handler!
 * @image html Interrupt_Init.jpg
 */
void IEC60335_ClassB_InitInterruptTest(IRQn_Type IRQn, uint8_t Mode, IRQTestData_t *CountSetup);

/*!
 * @brief   The interrupt check routine must be called once in known equidistant times.
 *
 * @attention   If this function is disturbed by active tested interrupts,
 *              the result counter may be out of range.
 * @param   IRQn   number of the interrupt to test
 * @return  passed or failed. See \link #testResult_enum testResult_t \endlink.
 * @image html Interrupt_Check.jpg
 */
testResult_t IEC60335_ClassB_InterruptCheck(IRQn_Type IRQn);

#ifdef __cplusplus
}
#endif /* __cplusplus */

/*!
 * @}
 */

#endif  /* __IEC60335_CLASS_B_INTERRUPT_TEST_H__ */

/************************************** EOF *********************************/
