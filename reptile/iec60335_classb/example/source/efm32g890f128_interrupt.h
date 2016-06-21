/***************************************************************************//**
 * @file    efm32g890f128_interrupt.h
 * @brief   handler routines prototypes for interrupts and exception
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

#ifndef __INTERRUPT_EM890F128_H__
#define __INTERRUPT_EM890F128_H__
/*!
 * @addtogroup INT
 * @{
 * @addtogroup EXCEPTIONS
 * @{
 */

/*!
 * @fn      void FaultHandler (void)
 * @brief   handle all error conditions
 * @note    used as DefaultHandler
 */
void FaultHandler(void);

/*!
 * @fn      void NMI_Handler(void)
 * @brief   This function handles NMI exception.
 * @note    uses DefaultHandler
 */
void NMI_Handler(void);

/*!
 * @fn      void HardFault_Handler(void)
 * @brief   This function handles Hard Fault exception.
 * @note    uses DefaultHandler
 *          void DefaultHardFault_Handler (void)
 */
void HardFault_Handler(void);

/*!
 * @fn      void MemManage_Handler(void)
 * @brief   This function handles Memory Manage Fault exception.
 * @note    uses DefaultHandler
 *          void DefaultMemManage_Handler (void)
 */
void MemManage_Handler(void);

/*!
 * @fn      void BusFault_Handler(void)
 * @brief   This function handles Bus Fault exception.
 * @note    uses DefaultHandler
 *          void DefaultBusFault_Handler (void)
 */
void BusFault_Handler(void);

/*!
 * @fn      void UsageFault_Handler(void)
 * @brief   This function handles Usage Fault exception.
 * @note    uses DefaultHandler
 *          void DefaultUsageFault_Handler (void)
 */
void UsageFault_Handler(void);

/*!
 * @fn      void DebugMon_Handler(void)
 * @brief   This function handles Debug Monitor Fault exception.
 * @note    uses DefaultHandler
 *          void DefaultDebugMon_Handler (void)
 */
void DebugMon_Handler(void);

/*!
 * @fn      void SVC_Handler(void)
 * @brief   This function handles SVC exception.
 * @note    uses DefaultHandler
 *          void DefaultSVC_Handler (void)
 */
void SVC_Handler(void);

/*!
 * @fn      void PendSV_Handler(void)
 * @brief   This function handles Pending SVC exception.
 * @note    uses DefaultHandler
 *          void DefaultPendSV_Handler (void)
 */
void PendSV_Handler(void);

/*!
 * @fn      void SysTick_Handler(void)
 * @brief   This function handles SysTick exception.
 * @note    uses DefaultHandler
 */
void SysTick_Handler(void);

/*!
 * @}
 */

/*!
 * @note
 * The CMSIS implementation provides the weak defined standard interrupt handler prototypes
 * which are replaced by the default handler if not implemented in other sources
 * Tasking compiler : the default handler is defined here and placed in the vector section of linker file
 * GCC compiler     : the Default handler is defined in the startup.c
 */

void DMA_IRQHandler(void);              /*!< 0, handler for DMA interrupt */
void GPIO_EVEN_IRQHandler(void);        /*!< 1, handler for GPIO_EVEN interrupt */
void TIMER0_IRQHandler(void);           /*!< 2, handler for TIMER0 interrupt */
void USART0_RX_IRQHandler(void);        /*!< 3, handler for USART0_RX interrupt */
void USART0_TX_IRQHandler(void);        /*!< 4, handler for USART0_TX interrupt */
void ACMP0_IRQHandler(void);            /*!< 5, handler for ACMP0 interrupt */
void ADC0_IRQHandler(void);             /*!< 6, handler for ADC0 interrupt */
void DAC0_IRQHandler(void);             /*!< 7, handler for DAC0 interrupt */
void I2C0_IRQHandler(void);             /*!< 8, handler for I2C0 interrupt */
void GPIO_ODD_IRQHandler(void);         /*!< 9, handler for GPIO_ODD interrupt */
void TIMER1_IRQHandler(void);           /*!< 10, handler for TIMER1 interrupt */
void TIMER2_IRQHandler(void);           /*!< 11, handler for TIMER2 interrupt */
void USART1_RX_IRQHandler(void);        /*!< 12, handler for USART1_RX interrupt */
void USART1_TX_IRQHandler(void);        /*!< 13, handler for USART1_TX interrupt */
void USART2_RX_IRQHandler(void);        /*!< 14, handler for USART2_RX interrupt */
void USART2_TX_IRQHandler(void);        /*!< 15, handler for USART2_TX interrupt */
void UART0_RX_IRQHandler(void);         /*!< 16, handler for UART0_RX interrupt */
void UART0_TX_IRQHandler(void);         /*!< 17, handler for UART0_TX interrupt */
void LEUART0_IRQHandler(void);          /*!< 18, handler for LEUART0 interrupt */
void LEUART1_IRQHandler(void);          /*!< 19, handler for LEUART1 interrupt */
void LETIMER0_IRQHandler(void);         /*!< 20, handler for LETIMER0 interrupt */
void PCNT0_IRQHandler(void);            /*!< 21, handler for PCNT0 interrupt */
void PCNT1_IRQHandler(void);            /*!< 22, handler for PCNT1 interrupt */
void PCNT2_IRQHandler(void);            /*!< 23, handler for PCNT2 interrupt */
void RTC_IRQHandler(void);              /*!< 24, handler for RTC interrupt */
void CMU_IRQHandler(void);              /*!< 25, handler for CMU interrupt */
void VCMP_IRQHandler(void);             /*!< 26, handler for VCMP interrupt */
void LCD_IRQHandler(void);              /*!< 27, handler for LCD interrupt */
void MSC_IRQHandler(void);              /*!< 28, handler for MSC interrupt */
void AES_IRQHandler(void);              /*!< 29, handler for AES interrupt */

/*!
 * @fn      DefaultHandler
 * @brief   Interrupt entry point for any unexpected exception
 *          should never been executed
 */
#if defined (__ICCARM__)
__noreturn void DefaultHandler(void);
#else
void DefaultHandler(void) __attribute__((noreturn));
#endif

#if 0
/*!
 * @fn      enableExceptions
 * @brief   inline function enabling the exceptions subsystem
 */
__INLINE void enableExceptions(void)
{
  SCB->SHCSR |= (SCB_SHCSR_MEMFAULTENA_Msk | SCB_SHCSR_BUSFAULTENA_Msk | SCB_SHCSR_USGFAULTENA_Msk);
}
#endif
/*!
 * @}
 */

#endif /* __INTERRUPT_EM890F128_H__ */

/************************************** EOF *********************************/
