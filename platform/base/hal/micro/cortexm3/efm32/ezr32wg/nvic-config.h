 /***************************************************************************//**
 * @file nvic-config.h
 * @brief NVIC Config Header
 * @version 0.01.0
 *******************************************************************************
 * @section License
 * <b>(C) Copyright 2014 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/
#include "emlib_config.h"

/** @addtogroup nvic_config
 * Nested Vectored Interrupt Controller configuration header.
 *
 * This header defines the functions called by all of the NVIC exceptions/
 * interrupts.  The following are the nine peripheral ISRs available for
 * modification.  To use one of these ISRs, it must be instantiated
 * somewhere in the HAL.  Each ISR may only be instantiated once.  It is
 * not necessary to instantiate all or any of these ISRs (a stub will
 * be automatically generated if an ISR is not instantiated).
 *
 * - \code void halTimer1Isr(void); \endcode
 * - \code void halTimer2Isr(void); \endcode
 * - \code void halSc1Isr(void);    \endcode
 * - \code void halSc1Isr(void);    \endcode
 * - \code void halAdcIsr(void);    \endcode
 * - \code void halIrqAIsr(void);   \endcode
 * - \code void halIrqBIsr(void);   \endcode
 * - \code void halIrqCIsr(void);   \endcode
 * - \code void halIrqDIsr(void);   \endcode
 *
 * @note This file should \b not be modified.
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

// \b NOTE NOTE NOTE NOTE NOTE NOTE - The physical layout of this file, that
// means the white space, is CRITICAL!  Since this file is \#include'ed by
// the assembly file isr-stubs.s79, the white space in this file translates
// into the white space in that file and the assembler has strict requirements.
// Specifically, there must be white space *before* each "SEGMENT()" and there
// must be an *empty line* between each "EXCEPTION()" and "SEGMENT()".
//
// \b NOTE NOTE NOTE - The order of the EXCEPTIONS in this file is critical
// since it translates to the order they are placed into the vector table in
// cstartup.
//
// The purpose of this file is to consolidate NVIC configuration, this
// includes basic exception handler (ISR) function definitions, into a single
// place where it is easily tracked and changeable.
//
// We establish 8 levels of priority (3 bits), with 0 (0 bits) of tie break
// subpriority. Lower priority values are higher priorities. This means that
// 0 is the highest and 7 is the lowest. The NVIC mapping is detailed below.
//

//The 'PRIGROUP' field is 3 bits within the AIRCR register and indicates the
//bit position within a given exception's 8-bit priority field to the left of
//which is the "binary point" separating the preemptive priority level (in the
//most-significant bits) from the subpriority (in the least-significant bits).
//The preemptive priority determines whether an interrupt can preempt an
//executing interrupt. The subpriority helps choose which interrupt to run if
//multiple interrupts with the same preemptive priority are active at the same
//time. If no supriority is given or there is a tie then the hardware defined
//exception number is used to break the tie.
//
//The table below shows for each PRIGROUP value (the PRIGROUP value is the
//number on the far left) how the priority bits are split and how this maps
//to the priorities on chip. A 'P' is preemption bit, 'S' is a subpriority bit
//and an 'X' indicates that that bit is not implemented.
//0=7:1= PPPXXXX.X
//1=6:2= PPPXXX.XX
//2=5:3= PPPXX.XXX
//3=4:4= PPPX.XXXX
//4=3:5= PPP.XXXXX
//5=2:6= PP.SXXXXX
//6=1:7= P.SSXXXXX
//7=0:8= SSSXXXXXX
//
//Below is the default priority configuration for the chip's interrupts
// Pri.Sub  Purpose
//   0.0    faults (highest)
//   1.0    FPUEH
//   2.0    not used
//   3.0    DISABLE_INTERRUPTS(), INTERRUPTS_OFF(), ATOMIC()
//   4.0    normal interrupts
//   5.0    not used
//   6.0    not used
//   7.0    PendSV (lowest)
#if defined(FREE_RTOS)
  // FreeRTOS is compatible with our default choice of PRIGROUP_POSITION
#endif
#define PRIGROUP_POSITION 4  // PPP.XXXXX

// Priority level used by DISABLE_INTERRUPTS() and INTERRUPTS_OFF()
// Must be lower priority than pendsv
// NOTE!! INTERRUPTS_DISABLED_PRIORITY AFFECTS SPRM.S79
// NOTE!! FreeRTOSConfig.h configMAX_SYSCALL_INTERRUPT_PRIORITY must match
//        INTERRUPTS_DISABLED_PRIORITY!
#define NVIC_ATOMIC CORE_ATOMIC_BASE_PRIORITY_LEVEL
#define INTERRUPTS_DISABLED_PRIORITY  (NVIC_ATOMIC << (PRIGROUP_POSITION+1))


//Exceptions with fixed priorities cannot be changed by software.  Simply make
//them 0 since they are high priorities anyways.
#define NVIC_FIXED 0
//Reserved exceptions are not instantiated in the hardware.  Therefore
//exception priorities don't exist so just default them to lowest level.
#define NVIC_NONE  0xFF

#ifndef SEGMENT
  #define SEGMENT()
#endif
#ifndef SEGMENT2
  #define SEGMENT2()
#endif
#ifndef PERM_EXCEPTION
  #define PERM_EXCEPTION(vectorNumber, functionName, priority) \
    EXCEPTION(vectorNumber, functionName, priority, 0)
#endif

    // SEGMENT()
    //   **Place holder required by isr-stubs.s79 to define __CODE__**
    // SEGMENT2()
    //   **Place holder required by isr-stubs.s79 to define __THUMB__**
    // EXCEPTION(vectorNumber, functionName, priorityLevel, subpriority)
    //   vectorNumber = exception number defined by hardware (not used anywhere)
    //   functionName = name of the function that the exception should trigger
    //   priorityLevel = priority level of the function
    //   supriority = Used to break ties between exceptions at the same level
    // PERM_EXCEPTION
    //   is used to define an exception that should not be intercepted by the
    //   interrupt debugging logic, or that not should have a weak stub defined.
    //   Otherwise the definition is the same as EXCEPTION

//INTENTIONALLY INDENTED AND SPACED APART! Keep it that way!  See comment above!
    SEGMENT()
    SEGMENT2()
    PERM_EXCEPTION( 1, halEntryPoint,       NVIC_FIXED           ) //Reset

    SEGMENT()
    SEGMENT2()
    EXCEPTION(      2, halNmiIsr,           NVIC_FIXED,         0)

    SEGMENT()
    SEGMENT2()
    EXCEPTION(      3, halHardFaultIsr,     NVIC_FIXED,         0)

    SEGMENT()
    SEGMENT2()
    EXCEPTION(      4, halMemoryFaultIsr,            0,         0)

    SEGMENT()
    SEGMENT2()
    EXCEPTION(      5, halBusFaultIsr,               0,         0)

    SEGMENT()
    SEGMENT2()
    EXCEPTION(      6, halUsageFaultIsr,             0,         0)

    SEGMENT()
    SEGMENT2()
    EXCEPTION(      7, halReserved07Isr,     NVIC_NONE, NVIC_NONE)

    SEGMENT()
    SEGMENT2()
    EXCEPTION(      8, halReserved08Isr,     NVIC_NONE, NVIC_NONE)

    SEGMENT()
    SEGMENT2()
    EXCEPTION(      9, halReserved09Isr,     NVIC_NONE, NVIC_NONE)

    SEGMENT()
    SEGMENT2()
    EXCEPTION(     10, halReserved10Isr,     NVIC_NONE, NVIC_NONE)

    SEGMENT()
    SEGMENT2()     // Above ATOMIC for FREE_RTOS task startup from ATOMIC level
    EXCEPTION(     11, halSvCallIsr,     NVIC_ATOMIC-1,         0)

    SEGMENT()
    SEGMENT2()
    EXCEPTION(     12, halDebugMonitorIsr,           4,         0)

    SEGMENT()
    SEGMENT2()
    EXCEPTION(     13, halReserved13Isr,     NVIC_NONE, NVIC_NONE)

    SEGMENT()
    SEGMENT2()     // Should be lowest priority
    EXCEPTION(     14, halPendSvIsr,                 7,         0)

    SEGMENT()
    SEGMENT2()
    EXCEPTION(     15, halInternalSysTickIsr,        4,         0)

    //The following handlers map to "External Interrupts 16 and above"
    //In the NVIC Interrupt registers, this corresponds to bits 39:0 with bit
    //0 being DMA (exception 16) and bit 39 being FPUEH (exception 55), etc.
    SEGMENT()
    SEGMENT2()
    EXCEPTION(     16, DMA_IRQHandler,               4,         0)

    SEGMENT()
    SEGMENT2()
    EXCEPTION(     17, GPIO_EVEN_IRQHandler,         4,         0)

    SEGMENT()
    SEGMENT2()
    EXCEPTION(     18, TIMER0_IRQHandler,            4,         0)

    SEGMENT()
    SEGMENT2()
    EXCEPTION(     19, USART0_RX_IRQHandler,         4,         0)

    SEGMENT()
    SEGMENT2()
    EXCEPTION(     20, USART0_TX_IRQHandler,         4,         0)

    SEGMENT()
    SEGMENT2()
    EXCEPTION(     21, USB_IRQHandler,               4,         0)

    SEGMENT()
    SEGMENT2()
    EXCEPTION(     22, ACMP0_IRQHandler,             4,         0)

    SEGMENT()
    SEGMENT2()
    EXCEPTION(     23, ADC0_IRQHandler,              4,         0)

    SEGMENT()
    SEGMENT2()
    EXCEPTION(     24, DAC0_IRQHandler,              4,         0)

    SEGMENT()
    SEGMENT2()
    EXCEPTION(     25, I2C0_IRQHandler,              4,         0)

    SEGMENT()
    SEGMENT2()
    EXCEPTION(     26, I2C1_IRQHandler,              4,         0)

    SEGMENT()
    SEGMENT2()
    EXCEPTION(     27, GPIO_ODD_IRQHandler,          4,         0)

    SEGMENT()
    SEGMENT2()
    EXCEPTION(     28, TIMER1_IRQHandler,            4,         0)

    SEGMENT()
    SEGMENT2()
    EXCEPTION(     29, TIMER2_IRQHandler,            4,         0)

    SEGMENT()
    SEGMENT2()
    EXCEPTION(     30, TIMER3_IRQHandler,            4,         0)

    SEGMENT()
    SEGMENT2()
    EXCEPTION(     31, USART1_RX_IRQHandler,         4,         0)

    SEGMENT()
    SEGMENT2()
    EXCEPTION(     32, USART1_TX_IRQHandler,         4,         0)

    SEGMENT()
    SEGMENT2()
    EXCEPTION(     33, LESENSE_IRQHandler,           4,         0)

    SEGMENT()
    SEGMENT2()
    EXCEPTION(     34, USART2_RX_IRQHandler,         4,         0)

    SEGMENT()
    SEGMENT2()
    EXCEPTION(     35, USART2_TX_IRQHandler,         4,         0)

    SEGMENT()
    SEGMENT2()
    EXCEPTION(     36, UART0_RX_IRQHandler,          4,         0)

    SEGMENT()
    SEGMENT2()
    EXCEPTION(     37, UART0_TX_IRQHandler,          4,         0)

    SEGMENT()
    SEGMENT2()
    EXCEPTION(     38, UART1_RX_IRQHandler,          4,         0)

    SEGMENT()
    SEGMENT2()
    EXCEPTION(     39, UART1_TX_IRQHandler,          4,         0)

    SEGMENT()
    SEGMENT2()
    EXCEPTION(     40, LEUART0_IRQHandler,           4,         0)

    SEGMENT()
    SEGMENT2()
    EXCEPTION(     41, LEUART1_IRQHandler,           4,         0)

    SEGMENT()
    SEGMENT2()
    EXCEPTION(     42, LETIMER0_IRQHandler,          4,         0)

    SEGMENT()
    SEGMENT2()
    EXCEPTION(     43, PCNT0_IRQHandler,             4,         0)

    SEGMENT()
    SEGMENT2()
    EXCEPTION(     44, PCNT1_IRQHandler,             4,         0)

    SEGMENT()
    SEGMENT2()
    EXCEPTION(     45, PCNT2_IRQHandler,             4,         0)

    SEGMENT()
    SEGMENT2()
    EXCEPTION(     46, RTC_IRQHandler,               4,         0)

    SEGMENT()
    SEGMENT2()
    EXCEPTION(     47, BURTC_IRQHandler,             4,         0)

    SEGMENT()
    SEGMENT2()
    EXCEPTION(     48, CMU_IRQHandler,               4,         0)

    SEGMENT()
    SEGMENT2()
    EXCEPTION(     49, VCMP_IRQHandler,              4,         0)

    SEGMENT()
    SEGMENT2()
    EXCEPTION(     50, LCD_IRQHandler,               4,         0)

    SEGMENT()
    SEGMENT2()
    EXCEPTION(     51, MSC_IRQHandler,               4,         0)

    SEGMENT()
    SEGMENT2()
    EXCEPTION(     52, AES_IRQHandler,               4,         0)

    SEGMENT()
    SEGMENT2()
    EXCEPTION(     53, EBI_IRQHandler,               4,         0)

    SEGMENT()
    SEGMENT2()
    EXCEPTION(     54, EMU_IRQHandler,               4,         0)

    SEGMENT()
    SEGMENT2()
    EXCEPTION(     55, FPUEH_IRQHandler,             1,         0)

#undef SEGMENT
#undef SEGMENT2
#undef PERM_EXCEPTION

#endif //DOXYGEN_SHOULD_SKIP_THIS
