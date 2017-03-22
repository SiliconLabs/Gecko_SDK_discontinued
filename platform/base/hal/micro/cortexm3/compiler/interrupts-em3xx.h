/***************************************************************************//**
 * @file hal/micro/cortexm3/compiler/interrupts-em3xx.h
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
 
////////////////////////////////////////////////////////////////////////////////
/** \name Global Interrupt Manipulation Macros
 *
 * \b Note: The special purpose BASEPRI register is used to enable and disable
 * interrupts while permitting faults.
 * When BASEPRI is set to 1 no interrupts can trigger. The configurable faults
 * (usage, memory management, and bus faults) can trigger if enabled as well as 
 * the always-enabled exceptions (reset, NMI and hard fault).
 * When BASEPRI is set to 0, it is disabled, so any interrupt can triggger if 
 * its priority is higher than the current priority.
 */
////////////////////////////////////////////////////////////////////////////////
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
    #define SET_BASE_PRIORITY_LEVEL(basepri) do { } while(0)
  #endif  // DOXYGEN_SHOULD_SKIP_THIS
#else  // BOOTLOADER
  
  #ifndef DOXYGEN_SHOULD_SKIP_THIS
    // A series of macros for the diagnostics of the global interrupt state.
    // These macros either enable or disable the debugging code in the
    // Interrupt Manipulation Macros as well as define the two pins used for
    // indicating the entry and exit of critical sections.
    //UNCOMMENT the below #define to enable interrupt debugging.
    //#define INTERRUPT_DEBUGGING
    #ifdef INTERRUPT_DEBUGGING
      // Designed to use the breakout board LED (PC5)
      #define ATOMIC_DEBUG(x)  x
      #define I_PIN         5
      #define I_PORT        C
      #define I_CFG_HL      H
      // For the concatenation to work, we need to define the regs via their own macros:
      #define I_SET_REG(port)           GPIO_P ## port ## SET
      #define I_CLR_REG(port)           GPIO_P ## port ## CLR
      #define I_OUT_REG(port)           GPIO_P ## port ## OUT
      #define I_CFG_MSK(port, pin)      P ## port ## pin ## _CFG_MASK
      #define I_CFG_BIT(port, pin)      P ## port ## pin ## _CFG_BIT
      #define I_CFG_REG(port, hl)       GPIO_P ## port ## CFG ## hl 
      // Finally, the macros to actually manipulate the interrupt status IO
      #define I_OUT(port, pin, hl) \
        do { I_CFG_REG(port, hl) &= ~(I_CFG_MSK(port, pin));      \
             I_CFG_REG(port, hl) |= (GPIOCFG_OUT << I_CFG_BIT(port, pin)); \
        } while (0)
      #define I_SET(port, pin)   do { I_SET_REG(port) = (BIT(pin)); } while (0)
      #define I_CLR(port, pin)   do { I_CLR_REG(port) = (BIT(pin)); } while (0)
      #define I_STATE(port, pin)   ((I_OUT_REG(port) & BIT(pin)) == BIT(pin))
      #if !defined(RTOS)
        #define DECLARE_INTERRUPT_STATE uint8_t _emIsrState, _emIsrDbgIoState
      #else //!defined(RTOS)
        #define DECLARE_INTERRUPT_STATE uint8_t _emIsrDbgIoState
      #endif //!defined(RTOS)
    #else//!INTERRUPT_DEBUGGING
      #define ATOMIC_DEBUG(x)
      #if !defined(RTOS)
        /**
         * @brief This macro should be called in the local variable
         * declarations section of any function which calls DISABLE_INTERRUPTS()
         * or RESTORE_INTERRUPTS().
         */
        #define DECLARE_INTERRUPT_STATE uint8_t _emIsrState
      #else //!defined(RTOS)
        /**
         * @brief This macro should be called in the local variable
         * declarations section of any function which calls DISABLE_INTERRUPTS()
         * or RESTORE_INTERRUPTS().
         */
        #define DECLARE_INTERRUPT_STATE
      #endif //!defined(RTOS)

    #endif//INTERRUPT_DEBUGGING
    
    // Prototypes for the BASEPRI and PRIMASK access functions.  They are very
    // basic and instantiated in assembly code in the file spmr.s37 (since
    // there are no C functions that cause the compiler to emit code to access
    // the BASEPRI/PRIMASK). This will inhibit the core from taking interrupts
    // with a priority equal to or less than the BASEPRI value.
    // Note that the priority values used by these functions are 5 bits and 
    // right-aligned
#if !defined(__CORTEX_M) || (__CORTEX_M >= 0x03)
    // Only Cortex-M3 and higher have BASEPRI
    extern uint8_t _readBasePri(void);
    extern void _writeBasePri(uint8_t priority);

    // Prototypes for BASEPRI functions used to disable and enable interrupts
    // while still allowing enabled faults to trigger.
    extern void _enableBasePri(void);
    extern uint8_t _disableBasePri(void);
    extern bool _basePriIsDisabled(void);
#endif
    
    // Prototypes for setting and clearing PRIMASK for global interrupt
    // enable/disable.
    extern void _setPriMask(void);
    extern void _clearPriMask(void);
    extern uint32_t _getPriMask(void);

    #if defined(RTOS)
      // Prototypes for the RTOS critical functions we will need to do ATOMIC()
      uint8_t rtosEnterCritical(void);
      uint8_t rtosExitCritical(void);
    #endif //defined(RTOS)

  #endif  // DOXYGEN_SHOULD_SKIP_THIS

  //The core Global Interrupt Manipulation Macros start here.
  
  // When building for an RTOS be sure to use compatible interrupt routines
  #if defined(RTOS)
    /**
     * @brief Disable interrupts, saving the previous state so it can be
     * later restored with RESTORE_INTERRUPTS().
     * \note Do not fail to call RESTORE_INTERRUPTS().
     * \note It is safe to nest this call.
     */
    #define DISABLE_INTERRUPTS()                    \
      do {                                          \
        rtosEnterCritical();                        \
        ATOMIC_DEBUG(                               \
          _emIsrDbgIoState = I_STATE(I_PORT,I_PIN); \
          I_SET(I_PORT, I_PIN);                     \
        )                                           \
      } while(0)

    /** 
     * @brief Restore the global interrupt state previously saved by
     * DISABLE_INTERRUPTS()
     * \note Do not call without having first called DISABLE_INTERRUPTS()
     * to have saved the state.
     * \note It is safe to nest this call.
     */
    #define RESTORE_INTERRUPTS()        \
      do {                              \
          ATOMIC_DEBUG(                 \
          if(!_emIsrDbgIoState)         \
            I_CLR(I_PORT, I_PIN);       \
        )                               \
        rtosExitCritical();             \
      } while(0)  
  #else
    /**
     * @brief Disable interrupts, saving the previous state so it can be
     * later restored with RESTORE_INTERRUPTS().
     * \note Do not fail to call RESTORE_INTERRUPTS().
     * \note It is safe to nest this call.
     */
    #if defined(__CORTEX_M) && (__CORTEX_M == 0x00)
      // Cortex-M0+ does not have BASEPRI, use PRIMASK instead
      #define DISABLE_INTERRUPTS()                    \
        do {                                          \
          _emIsrState = _getPriMask();                \
          _setPriMask();                              \
          ATOMIC_DEBUG(                               \
            _emIsrDbgIoState = I_STATE(I_PORT,I_PIN); \
            I_SET(I_PORT, I_PIN);                     \
          )                                           \
        } while(0)
    #else
      #define DISABLE_INTERRUPTS()                    \
        do {                                          \
          _emIsrState = _disableBasePri();            \
          ATOMIC_DEBUG(                               \
            _emIsrDbgIoState = I_STATE(I_PORT,I_PIN); \
            I_SET(I_PORT, I_PIN);                     \
          )                                           \
        } while(0)
    #endif
    
    /** 
     * @brief Restore the global interrupt state previously saved by
     * DISABLE_INTERRUPTS()
     * \note Do not call without having first called DISABLE_INTERRUPTS()
     * to have saved the state.
     * \note It is safe to nest this call.
     */
    #if defined(__CORTEX_M) && (__CORTEX_M == 0x00)
      // Cortex-M0+ does not have BASEPRI, use PRIMASK instead
      #define RESTORE_INTERRUPTS()      \
        do {                            \
          ATOMIC_DEBUG(                 \
            if(!_emIsrDbgIoState)       \
              I_CLR(I_PORT, I_PIN);     \
          )                             \
          if (!_emIsrState) {           \
            _clearPriMask();            \
          }                             \
        } while(0)
    #else
      #define RESTORE_INTERRUPTS()      \
        do {                            \
          ATOMIC_DEBUG(                 \
            if(!_emIsrDbgIoState)       \
              I_CLR(I_PORT, I_PIN);     \
          )                             \
          _writeBasePri(_emIsrState);   \
        } while(0)
      #endif
   #endif // defined(RTOS)
  
  /**
   * @brief Enable global interrupts without regard to the current or
   * previous state.
   */
#if defined(__CORTEX_M) && (__CORTEX_M == 0x00)
   // Cortex-M0+ does not have BASEPRI, use PRIMASK instead
  #define INTERRUPTS_ON()               \
    do {                                \
      ATOMIC_DEBUG(                     \
        I_OUT(I_PORT, I_PIN, I_CFG_HL); \
        I_CLR(I_PORT, I_PIN);           \
      )                                 \
      _clearPriMask();                  \
    } while(0)
#else
  #define INTERRUPTS_ON()               \
    do {                                \
      ATOMIC_DEBUG(                     \
        I_OUT(I_PORT, I_PIN, I_CFG_HL); \
        I_CLR(I_PORT, I_PIN);           \
      )                                 \
      _enableBasePri();                 \
    } while(0)
#endif
  
  /**
   * @brief Disable global interrupts without regard to the current or
   * previous state.
   */
#if defined(__CORTEX_M) && (__CORTEX_M == 0x00)
   // Cortex-M0+ does not have BASEPRI, use PRIMASK instead
  #define INTERRUPTS_OFF()      \
    do {                        \
      _setPriMask();            \
      ATOMIC_DEBUG(             \
        I_SET(I_PORT, I_PIN);   \
      )                         \
    } while(0)
#else
  #define INTERRUPTS_OFF()      \
    do {                        \
      (void)_disableBasePri();  \
      ATOMIC_DEBUG(             \
        I_SET(I_PORT, I_PIN);   \
      )                         \
    } while(0)
#endif

  /**
   * @returns true if global interrupts are disabled.
   */
#if defined(__CORTEX_M) && (__CORTEX_M == 0x00)
  #define INTERRUPTS_ARE_OFF() ( _getPriMask() )
#else
  #define INTERRUPTS_ARE_OFF() ( _basePriIsDisabled() )
#endif

  /**
   * @returns true if global interrupt flag was enabled when 
   * ::DISABLE_INTERRUPTS() was called.
   */
  #define INTERRUPTS_WERE_ON() (_emIsrState == 0)
  
  /**
   * @brief A block of code may be made atomic by wrapping it with this
   * macro.  Something which is atomic cannot be interrupted by interrupts.
   */
  #define ATOMIC(blah)         \
  {                            \
    DECLARE_INTERRUPT_STATE;   \
    DISABLE_INTERRUPTS();      \
    { blah }                   \
    RESTORE_INTERRUPTS();      \
  }
  
  
  /**
   * @brief Allows any pending interrupts to be executed. Usually this
   * would be called at a safe point while interrupts are disabled (such as
   * within an ISR).
   * 
   * Takes no action if interrupts are already enabled.
   */
  #define HANDLE_PENDING_INTERRUPTS() \
    do {                              \
      if (INTERRUPTS_ARE_OFF()) {     \
        INTERRUPTS_ON();              \
        INTERRUPTS_OFF();             \
      }                               \
   } while (0)
  
  
  /**
   * @brief Sets the base priority mask (BASEPRI) to the value passed,
   * bit shifted up by PRIGROUP_POSITION+1.  This will inhibit the core from
   * taking all interrupts with a preemptive priority equal to or less than
   * the BASEPRI mask.  This macro is dependent on the value of
   * PRIGROUP_POSITION in nvic-config.h. Note that the value 0 disables the
   * the base priority mask.
   *
   * Refer to the "PRIGROUP" table in nvic-config.h to know the valid values
   * for this macro depending on the value of PRIGROUP_POSITION.  With respect
   * to the table, this macro can only take the preemptive priority group
   * numbers denoted by the parenthesis.
   */
#if defined(__CORTEX_M) && (__CORTEX_M == 0x00)
   // Cortex-M0+ does not have BASEPRI
   // TODO: Delete this to catch usages
  #define SET_BASE_PRIORITY_LEVEL(basepri) \
    do {                                   \
    } while(0)
#else
  #define SET_BASE_PRIORITY_LEVEL(basepri) \
    do {                                   \
      _writeBasePri(basepri);              \
    } while(0)
#endif

#endif // BOOTLOADER
////////////////////////////////////////////////////////////////////////////////
//@}  // end of Global Interrupt Manipulation Macros
////////////////////////////////////////////////////////////////////////////////

