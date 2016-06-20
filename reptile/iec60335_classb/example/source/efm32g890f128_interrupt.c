/***************************************************************************//**
 * @file    efm32g890f128_interrupt.c
 * @brief   code for interrupts and exception, not implemented interrupts weak defined
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

/*!
 * @addtogroup INT
 * @{
 * @addtogroup EXCEPTIONS
 * @{
 */

/* ===================================================================
** 1. INCLUDE FILES
** ===================================================================
*/
#include "em_device.h"
#include "efm32g890f128_interrupt.h"

/*******************************************************************************
* Prototypes
*******************************************************************************/

/*!
 * \brief alternative start label for some compiler
 */
void START(void);

/*!
 * Prototypes
 */
extern int main(void);

/*!
 * reference to linker symbols
 */

/*!
 * FaultHandler to handle all error conditions
 */
void FaultHandler(void)
{
  /* TODO: include a fault handler routine here */
  while (1)
  {
  }
}

/*!
 * NMI
 */
void DefaultNMI_Handler(void)
{
  FaultHandler();
}

/*!
 * HardFault
 */
void DefaultHardFault_Handler(void)
{
  FaultHandler();
}

/*!
 * MemManageException
 */
void DefaultMemManage_Handler(void)
{
  FaultHandler();
}

/*!
 * BusFaultException
 */
void DefaultBusFault_Handler(void)
{
  FaultHandler();
}

/*!
 * UsageFaultException
 */
void DefaultUsageFault_Handler(void)
{
  FaultHandler();
}

/*!
 * DebugMonitor
 */
void DefaultDebugMon_Handler(void)
{
  FaultHandler();
}

/*!
 * SVCHandler
 */
void DefaultSVC_Handler(void)
{
}

/*!
 * PendSVC
 */
void DefaultPendSV_Handler(void)
{
}

/*!
 * SysTickHandler
 */
void DefaultSysTick_Handler(void)
{
  DefaultHandler();
}

/*!
 * DefaultHandler
 */
void DefaultHandler(void)
{
  /* Infinite loop. */
  while (1)
  {
  }
}

#if defined (__ICCARM__)
/* TODO */
#pragma weak NMI_Handler=DefaultNMI_Handler
#elif defined (__CC_ARM)
/* TODO */
#else
void NMI_Handler(void)          __attribute__((weak, alias("DefaultNMI_Handler")));
void HardFault_Handler(void)    __attribute__((weak, alias("DefaultHardFault_Handler")));
void MemManage_Handler(void)    __attribute__((weak, alias("DefaultMemManage_Handler")));
void BusFault_Handler(void)     __attribute__((weak, alias("DefaultBusFault_Handler")));
void UsageFault_Handler(void)   __attribute__((weak, alias("DefaultUsageFault_Handler")));
void DebugMon_Handler(void)     __attribute__((weak, alias("DefaultDebugMon_Handler")));
void SVC_Handler(void)          __attribute__((weak, alias("DefaultSVC_Handler")));
void PendSV_Handler(void)       __attribute__((weak, alias("DefaultPendSV_Handler")));
void SysTick_Handler(void)      __attribute__((weak, alias("DefaultSysTick_Handler")));
/*!
 * @}
 */
/*!
 * @note
 * if one of the following handler are implemented in other files of a project
 * the weak definition leads to the following compiler/linker behavior:
 * 1) the weak redirection of the handler to the "DefaultHandler" will be ignored,
 * 2) the new handler will be linked and the vector is placed to the new location.
 * it is not necessary to change this code.
 */
void DMA_IRQHandler(void)       __attribute__((weak, alias("DefaultHandler")));
void GPIO_EVEN_IRQHandler(void) __attribute__((weak, alias("DefaultHandler")));
void TIMER0_IRQHandler(void)    __attribute__((weak, alias("DefaultHandler")));
void USART0_RX_IRQHandler(void) __attribute__((weak, alias("DefaultHandler")));
void USART0_TX_IRQHandler(void) __attribute__((weak, alias("DefaultHandler")));
void ACMP0_IRQHandler(void)     __attribute__((weak, alias("DefaultHandler")));
void ADC0_IRQHandler(void)      __attribute__((weak, alias("DefaultHandler")));
void DAC0_IRQHandler(void)      __attribute__((weak, alias("DefaultHandler")));
void I2C0_IRQHandler(void)      __attribute__((weak, alias("DefaultHandler")));
void GPIO_ODD_IRQHandler(void)  __attribute__((weak, alias("DefaultHandler")));
void TIMER1_IRQHandler(void)    __attribute__((weak, alias("DefaultHandler")));
void TIMER2_IRQHandler(void)    __attribute__((weak, alias("DefaultHandler")));
void USART1_RX_IRQHandler(void) __attribute__((weak, alias("DefaultHandler")));
void USART1_TX_IRQHandler(void) __attribute__((weak, alias("DefaultHandler")));
void USART2_RX_IRQHandler(void) __attribute__((weak, alias("DefaultHandler")));
void USART2_TX_IRQHandler(void) __attribute__((weak, alias("DefaultHandler")));
void UART0_RX_IRQHandler(void)  __attribute__((weak, alias("DefaultHandler")));
void UART0_TX_IRQHandler(void)  __attribute__((weak, alias("DefaultHandler")));
void LEUART0_IRQHandler(void)   __attribute__((weak, alias("DefaultHandler")));
void LEUART1_IRQHandler(void)   __attribute__((weak, alias("DefaultHandler")));
void LETIMER0_IRQHandler(void)  __attribute__((weak, alias("DefaultHandler")));
void PCNT0_IRQHandler(void)     __attribute__((weak, alias("DefaultHandler")));
void PCNT1_IRQHandler(void)     __attribute__((weak, alias("DefaultHandler")));
void PCNT2_IRQHandler(void)     __attribute__((weak, alias("DefaultHandler")));
void RTC_IRQHandler(void)       __attribute__((weak, alias("DefaultHandler")));
void CMU_IRQHandler(void)       __attribute__((weak, alias("DefaultHandler")));
void VCMP_IRQHandler(void)      __attribute__((weak, alias("DefaultHandler")));
void LCD_IRQHandler(void)       __attribute__((weak, alias("DefaultHandler")));
void MSC_IRQHandler(void)       __attribute__((weak, alias("DefaultHandler")));
void AES_IRQHandler(void)       __attribute__((weak, alias("DefaultHandler")));
#endif
/*!
 * @}
 */

/************************************** EOF *********************************/
