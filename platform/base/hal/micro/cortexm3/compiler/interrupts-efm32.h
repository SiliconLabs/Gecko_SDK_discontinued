/***************************************************************************//**
 * @file hal/micro/cortexm3/compiler/interrupts-efm32.h
 * @brief Interrupt Macros
 * @version 0.01.0
 *******************************************************************************
 * @section License
 * <b>(C) Copyright 2016 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *

 ******************************************************************************/

#include "em_core.h"

// -----------------------------------------------------------------------------
//  Global Interrupt Manipulation Macros
//@{

#define ATOMIC_LITE(blah) ATOMIC(blah)
#define DECLARE_INTERRUPT_STATE_LITE DECLARE_INTERRUPT_STATE
#define DISABLE_INTERRUPTS_LITE() DISABLE_INTERRUPTS()
#define RESTORE_INTERRUPTS_LITE() RESTORE_INTERRUPTS()

#ifdef BOOTLOADER
  #ifndef DOXYGEN_SHOULD_SKIP_THIS
    // The bootloader does not use interrupts
    #define DECLARE_INTERRUPT_STATE
    #define DISABLE_INTERRUPTS() do { } while(0)
    #define RESTORE_INTERRUPTS() do { } while(0)
    #define INTERRUPTS_ON() do { } while(0)
    #define INTERRUPTS_OFF() do { } while(0)
    #define INTERRUPTS_ARE_OFF() (false)
    #define ATOMIC(blah) { blah }
    #define HANDLE_PENDING_INTERRUPTS() do { } while(0)
  #endif  // DOXYGEN_SHOULD_SKIP_THIS
#else  // BOOTLOADER
  
  #if defined(__CORTEX_M) 
    
    #define DECLARE_INTERRUPT_STATE CORE_DECLARE_IRQ_STATE
    /**
     * @brief Disable interrupts, saving the previous state so it can be
     * later restored with RESTORE_INTERRUPTS().
     * \note Do not fail to call RESTORE_INTERRUPTS().
     * \note It is safe to nest this call.
     */
    #define DISABLE_INTERRUPTS() CORE_ENTER_ATOMIC()

    /** 
     * @brief Restore the global interrupt state previously saved by
     * DISABLE_INTERRUPTS()
     * \note Do not call without having first called DISABLE_INTERRUPTS()
     * to have saved the state.
     * \note It is safe to nest this call.
     */
    #define RESTORE_INTERRUPTS() CORE_EXIT_ATOMIC()
    
    /**
     * @brief Enable global interrupts without regard to the current or
     * previous state.
     */
    #define INTERRUPTS_ON() CORE_ATOMIC_IRQ_ENABLE()
    
    /**
     * @brief Disable global interrupts without regard to the current or
     * previous state.
     */
    #define INTERRUPTS_OFF() CORE_ATOMIC_IRQ_DISABLE()

    /**
     * @returns true if global interrupts are disabled.
     */
    #define INTERRUPTS_ARE_OFF() CORE_IrqIsDisabled()

    /**
     * @returns true if global interrupt flag was enabled when 
     * ::DISABLE_INTERRUPTS() was called.
     */
    #define INTERRUPTS_WERE_ON() (irqState == 0)
    
    /**
     * @brief A block of code may be made atomic by wrapping it with this
     * macro.  Something which is atomic cannot be interrupted by interrupts.
     */
    #define ATOMIC(blah) CORE_ATOMIC_SECTION(blah)
    
    
    /**
     * @brief Allows any pending interrupts to be executed. Usually this
     * would be called at a safe point while interrupts are disabled (such as
     * within an ISR).
     * 
     * Takes no action if interrupts are already enabled.
     */
    #define HANDLE_PENDING_INTERRUPTS() CORE_YIELD_ATOMIC()
  #endif // __CORTEX_M

#endif // BOOTLOADER
////////////////////////////////////////////////////////////////////////////////
//@}  // end of Global Interrupt Manipulation Macros
////////////////////////////////////////////////////////////////////////////////
