/***************************************************************************//**
 * @file    iec60335_class_b_tessy.h
 * @brief   header file containing all global types and definitions for unit tests
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

#ifndef __IEC60335_CLASS_B_TESSY_H__
#define __IEC60335_CLASS_B_TESSY_H__

#ifdef TESSY
#undef RTC
#undef CMU
#undef TIMER0
#undef SysTick
#undef NVIC
#undef SCB
#undef RMU
#undef WDOG
#ifdef __I
#undef __I
#define __I volatile
#endif
RTC_TypeDef*         RTC_Register     = (RTC_TypeDef*)  RTC_BASE;
CMU_TypeDef*         CMU_Register     = (CMU_TypeDef*)  CMU_BASE;
TIMER_TypeDef*       TIMER0_Register  = (TIMER_TypeDef*)TIMER0_BASE;
SysTick_Type*        SysTick_Register = (SysTick_Type*) SysTick_BASE;
NVIC_Type*           NVIC_Register    = (NVIC_Type*)    NVIC_BASE;
SCB_Type*            SCB_Register     = (SCB_Type*)     SCB_BASE;
RMU_TypeDef*         RMU_Register     = (RMU_TypeDef*)  RMU_BASE;
WDOG_TypeDef*        WDOG_Register    = (WDOG_TypeDef*) WDOG_BASE;
#define RTC          RTC_Register           
#define CMU          CMU_Register
#define TIMER0       TIMER0_Register
#define SysTick      SysTick_Register
#define NVIC         NVIC_Register
#define SCB          SCB_Register
#define RMU          RMU_Register
#define WDOG         WDOG_Register
#endif

#endif  /* __IEC60335_CLASS_B_TESSY_H__ */

/************************************** EOF *********************************/
