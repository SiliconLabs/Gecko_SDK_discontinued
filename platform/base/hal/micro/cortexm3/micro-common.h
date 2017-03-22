/** @file hal/micro/cortexm3/micro-common.h
 * @brief Utility and convenience functions for EM35x microcontroller,
 *        common to both the full and minimal hal.
 * See @ref micro for documentation.
 *
 * <!-- Copyright 2013 Silicon Laboratories, Inc.                        *80*-->
 */

/** @addtogroup micro
 * See also hal/micro/cortexm3/micro.h for source code.
 *@{
 */

#ifndef __EM3XX_MICRO_COMMON_H__
#define __EM3XX_MICRO_COMMON_H__

#if defined(sky66107) || \
    defined(CORTEXM3_EM317) || \
    defined(CORTEXM3_EM341) || defined(CORTEXM3_EM342) || defined(CORTEXM3_EM346) || \
    defined(CORTEXM3_EM351) || defined(CORTEXM3_EM355) || defined(CORTEXM3_EM3555) || defined(CORTEXM3_EM356) || defined(CORTEXM3_EM357) || \
    defined(CORTEXM3_EM35X_GEN4)
  //This top level device inclusion comes from Device/SiliconLabs/emXXXX/Include/
  #include "em_device.h"
#endif

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#ifndef __EMBERSTATUS_TYPE__
#define __EMBERSTATUS_TYPE__
  //This is necessary here because halSleepForQsWithOptions returns an
  //EmberStatus and not adding this typedef to this file breaks a
  //whole lot of builds.
  typedef uint8_t EmberStatus;
#endif //__EMBERSTATUS_TYPE__
#endif // DOXYGEN_SHOULD_SKIP_THIS

// Forward declarations of the handlers used in the vector table
// These declarations are extracted from the NVIC configuration file.
#define EXCEPTION(vectorNumber, functionName, priorityLevel, subpriority) \
  void functionName(void);
  #include NVIC_CONFIG
#undef  EXCEPTION


/**
 * @brief OSC32K_x values can be used in board headers to specify the
 * desired mode for system timekeeping.  Because preprocessor logic
 * is used to check validity, these have to be #defines and not an enum.
 */
#define OSC32K_DISABLE 0 // Use 10 kHz int RC (same as not defining ENABLE_OSC32K)
#define OSC32K_CRYSTAL 1 // 32.768 kHz Crystal oscillator on PC6 and PC7
#define OSC32K_SINE_1V 2 // 32.768 kHz Sine wave 0-1V analog on PC7
#define OSC32K_DIGITAL 3 // 32.768 kHz Digital clock (0-Vdd square wave) on PC7
#define OSC32K_CHOICES 4 // Must be last

/**
 * @brief Some registers and variables require identifying GPIO by
 * a single number instead of the port and pin.  These macros convert
 * Port A-F pins into a single number.
 */
//@{
#define PORTA_PIN(y) ((0<<3)|(y))
#define PORTB_PIN(y) ((1<<3)|(y))
#define PORTC_PIN(y) ((2<<3)|(y))
#define PORTD_PIN(y) ((3<<3)|(y))
#define PORTE_PIN(y) ((4<<3)|(y))
#define PORTF_PIN(y) ((5<<3)|(y))
//@}

/**
 * @brief Resets the watchdog timer.  This function is pointed
 * to by the macro ::halResetWatchdog().
 * @warning Be very careful when using this as you can easily get into an
 * infinite loop.
 */
void halInternalResetWatchDog( void );


/**
 * IO pin config type
 */
typedef uint32_t HalGpioCfg_t;

/**
 * @brief Configure an IO pin's operating mode
 *
 * @param gpio   The IO pin to use, can be specified with the convenience macros
 *               PORTA_PIN(), PORTB_PIN(), PORTC_PIN(), etc.
 * @param config   The configuration mode to use.
 *
 */
void halGpioSetConfig(uint32_t gpio, HalGpioCfg_t config);

/**
 * @brief Get an IO pin's operating mode
 *
 * @param gpio   The IO pin to use, can be specified with the convenience macros
 *               PORTA_PIN(), PORTB_PIN(), PORTC_PIN(), etc.
 * @return The current configuration mode.
 *
 */
HalGpioCfg_t halGpioGetConfig(uint32_t gpio);

/**
 * @brief Set an output pin high
 *
 * @param gpio   The IO pin to use, can be specified with the convenience macros
 *               PORTA_PIN(), PORTB_PIN(), PORTC_PIN(), etc.
 *
 */
void halGpioSet(uint32_t gpio);

/**
 * @brief Set an output pin low
 *
 * @param gpio   The IO pin to use, can be specified with the convenience macros
 *               PORTA_PIN(), PORTB_PIN(), PORTC_PIN(), etc.
 *
 */
void halGpioClear(uint32_t gpio);

/**
 * @brief Read the input voltage level on an IO pin
 *
 * @param gpio   The IO pin to use, can be specified with the convenience macros
 *               PORTA_PIN(), PORTB_PIN(), PORTC_PIN(), etc.
 * @return 1 if the pin reads high, 0 otherwise.
 *
 */
uint32_t halGpioRead(uint32_t gpio);

/**
 * @brief Read the output voltage level on an IO pin
 *
 * @param gpio   The IO pin to use, can be specified with the convenience macros
 *               PORTA_PIN(), PORTB_PIN(), PORTC_PIN(), etc.
 * @return 1 if the pin reads high, 0 otherwise.
 *
 */
uint32_t halGpioReadOutput(uint32_t gpio);


/**
 * @brief Calibrates the internal SlowRC to generate a 1024 Hz (1kHz) clock.
 */
void halInternalCalibrateSlowRc( void );

/**
 * @brief Calibrates the internal FastRC to generate a 12MHz clock.
 */
void halInternalCalibrateFastRc(void);


/**
 * @brief Sets the trim values for the 1.8V and 1.2V regulators based upon
 * manufacturing configuration.
 *
 * @param boostMode  Alter the regulator trim based upon the state
 * of boost mode.  true if boost mode is active, false otherwise.
 */
void halInternalSetRegTrim(bool boostMode);

/** @brief Determine VREG_OUT in the current mode (normal or boost).
 *
 * @return VREG_OUT in millivolts, depending on normal or boost mode
 */
uint16_t halInternalGetVreg(void);

/**
 * @brief Calibrates the GPIO pads.  This function is called from within
 * the stack and HAL at appropriate times.
 */
void halCommonCalibratePads(void);

enum {
  SYSCLK_BOOT_START  = 1, // halCommonStartXtal()
  SYSCLK_BOOT_SWITCH = 2, // halCommonSwitchToXtal()
  SYSCLK_WAKE_START  = 3, // halInternalPowerUpKickXtal()
  SYSCLK_WAKE_SWITCH = 4, // halInternalBlockUntilXtal()
  SYSCLK_NEED_STABLE = 5, // halCommonBlockUntilXtal()
  SYSCLK_BIAS_EVENT  = 6, // Timed event occurred
};
typedef uint8_t SysClkCallbackContext;

#if     ( (BTL_HAS_SERIAL && BTL_HAS_RADIO) \
        ||(!defined(CORTEXM3_EM317)) \
        ||(defined(HAL_NON_LIBRARY) &&( !defined(ENABLE_OSC24M) \
                                      ||(ENABLE_OSC24M == 24) \
                                      ||(ENABLE_OSC24M == 24000) \
                                      ||(ENABLE_OSC24M == 24000000) )) )

// serial-ota-bootloader must use 24 MHz Xtal
// This saves some code space when we know app will use 24 MHz Xtal
#define BTL_SYSCLK_KNOWN              1
#define halSystemXtalHz               24000000L
#define halMcuClockHz()  ((wakeupXtalState <= WAKEUP_XTAL_STATE_READY_SWITCH) \
                                    ? 12000000UL : 24000000UL)
#define halPeripheralClockHz()        12000000UL
#define halSystemClockEnableCallback(context) (0) /*no-op dummy return*/
#define MAC_TIMER_TICKS(microseconds) (microseconds)
#define MAC_TIMER_US(ticks)           (ticks)
#define SYSTICK_TIMER_TICKS(microseconds) ((microseconds)*24)
#define SYSTICK_TIMER_US(ticks)           (((ticks)+12)/24)

#else//!( (BTL_HAS_SERIAL && BTL_HAS_RADIO) ...)

/**
 * @brief This callback can be implemented outside of the HAL to activate
 * a non-crystal system clock, if such activation is not done purely by
 * hardware but needs software assist (e.g. enabling it via GPIO).
 * It is called from within halInit() during bootup and from within
 * halPowerUp() during sleep wakeup if the system clock has become
 * disabled or deselected.  In each case, if the callback returns a non-zero
 * value, therewill be more calls made to poll until 0 is returned.  The first
 * callback occurs prior to halInternalInitBoard() or halInternalPowerUpBoard()
 * with a context of SYSCLK_BOOT_START or SYSCLK_WAKE_START to "start" the
 * clock, and remaining callbacks, if needed, are made subsequently.
 * In the case of a non-Xtal, OSC24M_CTRL_OSC24M_EN will have been set
 * before this callback is issued, and OSC24M_CTRL_OSC24M_SEL will only
 * be set when the callback returns 0, allowing the system clock switch
 * to occur.
 *
 * @param context Is the situation for the callback, per the
 * SysClkCallbackContext enum above.
 *
 * @return Time, in microseconds, to idle-delay (while still running on
 * internal fast RC system clock), and be called back again with a
 * SYSCLK_BIAS_EVENT, or 0 if can switch immediately to the external clock.
 */
uint16_t halSystemClockEnableCallback(SysClkCallbackContext context);

/**
 * @brief This function can be used to determine the system clock frequency.
 *
 * @return The current system clock frequency, in hertz.
 */
uint32_t halSystemClockHz(void);

/**
 * @brief This function can be used to determine the MCU clock frequency.
 *
 * @return The current MCU clock frequency, in hertz.
 */
uint32_t halMcuClockHz(void);

/**
 * @brief This function can be used to determine the peripheral clock frequency.
 *
 * @return The current peripheral clock frequency, in hertz.
 */
uint32_t halPeripheralClockHz(void);

/**
 * @brief Convenience macro to convert microseconds to ticks of the MAC_TIMER,
 * which is designed to tick every 1 microsecond on a 12 MHz Periph clock.
 * Using 64-bit arithmetic and rounding seems to give the best results.
 */
#define MAC_TIMER_TICKS(microseconds) \
  ((uint32_t)((((uint64_t)(microseconds) * halPeripheralClockHz()) \
               + (12000000UL / 2)) / 12000000UL))

/**
 * @brief Convenience macro to convert MAC_TIMER ticks to microseconds and
 * rounding to nearest microsecond.
 * Using 64-bit arithmetic and rounding seems to give the best results.
 */
static inline uint32_t MAC_TIMER_US(uint32_t ticks)
{
  uint32_t periphHz = halPeripheralClockHz();
  return ((uint32_t)((((uint64_t)(ticks) * 12000000UL)
                      + (periphHz / 2)) / periphHz));
}

/**
 * @brief Convenience macro to convert microseconds to SysTicks
 * which is designed to tick at same frequency as the MCU (FCLK).
 */
#define SYSTICK_TIMER_TICKS(microseconds) \
  ((((uint32_t)(microseconds) * (halMcuClockHz() / 250000)) + 3) / 4)

/**
 * @brief Convenience macro to convert SysTicks to microseconds and
 * rounding to nearest microsecond.
 * (Because the FCLK might not evenly divide by 1000000 or even 500000, use
 * quad arithmetic, dividing it by 250000.)
 */
#define SYSTICK_TIMER_US(ticks) \
  (((((uint32_t)(ticks) * 8) / (halMcuClockHz() / 250000)) + 1) / 2)

#endif//( (BTL_HAS_SERIAL && BTL_HAS_RADIO) ...)

/**
 * @brief This function schedules a SysTick interrupt the specified
 * number of microseconds in the future.  It is used during crystal
 * startup and by emRadioIdleMicro().
 *
 * @param usDelay  Microseconds to when SysTick interrupt will fire.
 * Minimum value must be 50 partly due to overhead.
 */
void halInternalScheduleSysTickInMicroseconds(int32_t usDelay);

/**
 * @brief This function can be used to configure an arbitrary baud rate
 * on a UART port, based on the current peripheral clock rate.
 *
 * @param port     Serial port number (0 or 1).
 *
 * @param baud     An arbitrary baud rate > 0.
 *
 * @return EMBER_SERIAL_INVALID_BAUD_RATE if the baud rate is out of range,
 * otherwise EMBER_SUCCESS.
 */
EmberStatus halInternalUartSetBaudRate(uint8_t port, uint32_t baud);

/**
 * @brief This function returns the current baud rate configured on
 * a UART port, based on the current peripheral clock rate.
 *
 * @param port     Serial port number (0 or 1).
 *
 * @return The port's baud rate, or 0 if not configured or no such port.
 */
uint32_t halInternalUartGetBaudRate(uint8_t port);

/**
 * @brief Convenience macro to get current TPIU speed in Hz
 */
#define halInternalTpiuGetSpeedHz() halInternalUartGetBaudRate(0)

/**
 * @brief Convenience macro to set current TPIU speed in Hz
 */
#define halInternalTpiuSetSpeedHz(hz) halInternalUartSetBaudRate(0, hz)

/**
 * @brief This is the core function for enabling the XTAL, biasing
 * the XTAL, checking the XTAL biasing, switching to the XTAL,
 * configuring FCLK, and configuring flash access settings.  The ultimate
 * result of calling this function until it returns true is the chip is
 * operating from the 24MHz XTAL, the XTAL is biased for lowest current
 * consumption, the CPU's FCLK is being sourced from SYSCLK, and the flash
 * is configured for optimal current consumption.
 *
 * The basic principle of this function is that it takes time for the
 * XTAL to stabilize whenever it is enabled and/or the biasing is change;
 * about 1.5ms every time the bias is changed.  This function will handle
 * the XTAL configuration, set an interrupt event to indicate when the
 * appropriate delay has elapsed, and return immediately.  This interrupt
 * event should not be monitored directly by any code other than the clock
 * module itself.  The state of the XTAL is learned by the return code
 * of this function.  As long as this function returns false, the XTAL
 * is unstable.  Calling code can perform other actions until the XTAL
 * is stable.
 *
 * The suggested use of the four XTAL API functions is as follows:
 *  - halCommonStartXtal() is called once as soon as possible to start
 *    the XTAL.  Other actions may be performed while waiting for the XTAL
 *    to stabilize.
 *  - halCommonTryToSwitchToXtal() is called repeatedly to drive
 *    the biasing and switch process.  Other actions that do not require
 *    a stable XTAL may be performed until this function returns 0.
 *  - halCommonSwitchToXtal() is called just once when a stable XTAL is
 *    required before moving on.  This function will block in the idle
 *    sleep state until the switch procedure has completed.
 *  - halCommonCheckXtalBiasTrim() is called periodically, after a switch
 *    has occurred, to check that the XTAL biasing is optimal and
 *    adjust if needed.
 *
 * halCommonTryToSwitchToXtal() function will return immediately.  This
 * function drives the switch process.  This function can be called
 * any number of times and at anytime.
 *
 * @param amBlocking true if being called in a blocking context (loop),
 * false otherwise.  Used to distinguish startup from waiting on Xtal.
 *
 * @return time, in microseconds, to delay until next bias event for crystal
 * stability.  If 0, the chip has switched to and is now using the XTAL.
 * No further bias events are in process.  If non-zero the Xtal is
 * <b>unstable<b> and the chip has not modified it's XTAL selection; it
 * remains on the same clock source (OSCHF or XTAL) and there is a bias event
 * in process.
 */
uint16_t halCommonTryToSwitchToXtal(SysClkCallbackContext context);

void halInternalPowerUpKickXtal(void);
void halInternalBlockUntilXtal(void);
void halCommonBlockUntilXtal(void);

enum {
  WAKEUP_XTAL_STATE_START          = 0,
  WAKEUP_XTAL_STATE_BEFORE_LO_EN   = 1,
  WAKEUP_XTAL_STATE_LO_EN          = 2,
  WAKEUP_XTAL_STATE_READY_SWITCH   = 3,
  WAKEUP_XTAL_STATE_WAITING_FINAL  = 4,
  WAKEUP_XTAL_STATE_FINAL          = 5,
};
typedef uint8_t WakeupXtalState;

extern volatile WakeupXtalState wakeupXtalState;

/**
 * @brief This function is intended to be called periodically, from the
 * HAL and application, to check the XTAL bias trim is within
 * appropriate levels and adjust if not.  It will return immediately.
 * This function can be called any number of times and at anytime.
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
void halCommonCheckXtalBiasTrim(void);
#else //DOXYGEN_SHOULD_SKIP_THIS
//Simply redirect to the primary switch function, which handles all XTAL
//switching and biasing activities.  We don't care about the return code.
//0 is ideal but we could get non-zero (even if on the XTAL) because there
//is a biasing event in process and the XTAL is unstable.
#define halCommonCheckXtalBiasTrim() halCommonTryToSwitchToXtal(SYSCLK_BOOT_SWITCH)
#endif //DOXYGEN_SHOULD_SKIP_THIS

/**
 * @brief This function is intended to initiate the XTAL start, bias, and
 * switch procedure. It will return immediately.  This allows the calling
 * code to do other things while waiting for the XTAL to stabilize.  The
 * functions halCommonTryToSwitchToXtal() and halCommonSwitchToXtal() are
 * intended for completing the process.  This function can be called any
 * number of times and at anytime.
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
void halCommonStartXtal(void);
#else //DOXYGEN_SHOULD_SKIP_THIS
//Simply redirect to the primary switch function, which handles all XTAL
//switching and biasing activities.  We don't care about the return code.
//0 is ideal but we could get non-zero (even if on the XTAL) because there
//is a biasing event in process and the XTAL is unstable.
#define halCommonStartXtal() halCommonTryToSwitchToXtal(SYSCLK_BOOT_START)
#endif //DOXYGEN_SHOULD_SKIP_THIS

/**
 * @brief This function switches the chip to using the XTAL.  This includes
 * biasing the XTAL and changing the CPU to 24MHz (FCLK sourced from SYSCLK).
 * This function <b>blocks</b> until the switch procedure has occurred.
 * While blocking, this function invokes idle sleep to reduce current
 * consumption.
 *
 * NOTE: It is possible to use this function as a replacement for
 * the three other XTAL APIs, including halCommonCheckXtalBiasTrim(), as
 * long as blocking is acceptable.
 *
 * This function can be called any number of times and at anytime.
 */
void halCommonSwitchToXtal(void);

/**
 * @brief This function configures flash access for optimal current.
 */
void halInternalConfigFlashAccess(void);

/** @brief Blocks the current thread of execution for the specified
 * amount of time, in milliseconds.
 *
 * The function is implemented with cycle-counted busy loops
 * and is intended to create the short delays required when interfacing with
 * hardware peripherals.  This function works by simply adding another
 * layer on top of halCommonDelayMicroseconds().
 *
 * @param ms  The specified time, in milliseconds.
 */
void halCommonDelayMilliseconds(uint16_t ms);


/** @brief Puts the microcontroller to sleep in a specified mode, allows
 * the GPIO wake sources to be determined at runtime.  The function halSleep()
 * requires the GPIO wake sources to be defined at compile time in the board
 * file.
 *
 * @note This routine always enables interrupts.
 *
 * @param sleepMode  A microcontroller sleep mode.
 *
 * @param gpioWakeBitMask  A bit mask of the GPIO that are allowed to wake
 * the chip from deep sleep.  A high bit in the mask will enable waking
 * the chip if the corresponding GPIO changes state.  bit0 is PA0, bit1 is
 * PA1, bit8 is PB0, bit16 is PCO, bit23 is PC7, bits[31:24] are ignored.
 *
 * @sa ::SleepModes
 */
void halSleepWithOptions(SleepModes sleepMode, WakeMask gpioWakeBitMask);


/**
 * @brief Uses the system timer to enter ::SLEEPMODE_WAKETIMER for
 * approximately the specified amount of time (provided in quarter seconds),
 * the GPIO wake sources can be provided at runtime.
 *
 * This function returns ::EMBER_SUCCESS and the duration parameter is
 * decremented to 0 after sleeping for the specified amount of time.  If an
 * interrupt occurs that brings the chip out of sleep, the function returns
 * ::EMBER_SLEEP_INTERRUPTED and the duration parameter reports the amount of
 * time remaining out of the original request.
 *
 * @note This routine always enables interrupts.
 *
 * @note The maximum sleep time of the hardware is limited on EM35x platforms
 * to 48.5 days.  Any sleep duration greater than this limit will wake up
 * briefly (e.g. 16 microseconds) to reenable another sleep cycle.
 *
 * @nostackusage
 *
 * @param duration The amount of time, expressed in quarter seconds, that the
 * micro should be placed into ::SLEEPMODE_WAKETIMER.  When the function returns,
 * this parameter provides the amount of time remaining out of the original
 * sleep time request (normally the return value will be 0).
 *
 * @param gpioWakeBitMask  A bit mask of the GPIO that are allowed to wake
 * the chip from deep sleep.  A high bit in the mask will enable waking
 * the chip if the corresponding GPIO changes state.  bit0 is PA0, bit1 is
 * PA1, bit8 is PB0, bit16 is PCO, bit23 is PC7, bits[31:24] are ignored.
 *
 * @return An EmberStatus value indicating the success or
 *  failure of the command.
 */
EmberStatus halSleepForQsWithOptions(uint32_t *duration, WakeMask gpioWakeBitMask);


/** 
 * @brief Provides access to assembly code that is used to save/restore
 * context when deep sleeping.
 */
void halInternalContextSaveRestore(uint32_t save);

/**
 * @brief Provides access to assembly code which triggers idle sleep.
 */
void halInternalIdleSleep(void);

/** @brief Puts the microcontroller to sleep in a specified mode.  This
 *  internal function performs the actual sleep operation.  This function
 *  assumes all of the wake source registers are configured properly.
 *
 * @note This routine always enables interrupts.
 *
 * @param sleepMode  A microcontroller sleep mode
 */
void halInternalSleep(SleepModes sleepMode);


/**
 * @brief Obtains the events that caused the last wake from sleep.  The
 * meaning of each bit is as follows:
 * - [31] = WakeInfoValid
 * - [30] = SleepSkipped
 * - [29] = CSYSPWRUPREQ
 * - [28] = CDBGPWRUPREQ
 * - [27] = PWRUP_WAKECORE
 * - [26] = PWRUP_SLEEPTMRWRAP
 * - [25] = PWRUP_SLEEPTMRCOMPB
 * - [24] = PWRUP_SLEEPTMRCOMPA
 * - [23:0] = corresponding GPIO activity
 *
 * WakeInfoValid means that ::halSleep (::halInternalSleep) has been called
 * at least once.  Since the power on state clears the wake event info,
 * this bit says the sleep code has been called since power on.
 *
 * SleepSkipped means that the chip never left the running state.  Sleep can
 * be skipped if any wake event occurs between going ::ATOMIC and transferring
 * control from the CPU to the power management state machine.  Sleep can
 * also be skipped if the debugger is connected (JTAG/SerialWire CSYSPWRUPREQ
 * signal is set).  The net affect of skipping sleep is the Low Voltage
 * domain never goes through a power/reset cycle.
 *
 * @return The events that caused the last wake from sleep.
 */
WakeEvents halGetWakeInfo(void);

/**
 * @brief Provide convenience macros for accessing CMSIS style Serial Controller
 * peripheral definitions based on the UART port being used.
 * 
 * The macros will decay into simple register access if a CMSIS Device
 * header only includes just SC1 or just SC3.
 * When SC1 and SC3 exist, the macros consume arrays defined in micro-common.c.
 * 
 * @param port The port number as known to the UART realm.  Keep in mind
 * that port 1 is SC1 and port 2 is SC3 (this is the tricky one).
 */
#if defined(SC1) && !defined(SC3)
    #define SCx_REG(port, reg) SC1->reg
    #define EVENT_SCxCFG(port) EVENT_SC1->CFG
    #define EVENT_SCxFLAG(port) EVENT_SC1->FLAG
    #define SCx_IRQn(port) (SC1_IRQn)
#elif !defined(SC1) && defined(SC3)
    #define SCx_REG(port, reg) SC3->reg
    #define EVENT_SCxCFG(port) EVENT_SC3->CFG
    #define EVENT_SCxFLAG(port) EVENT_SC3->FLAG
    #define SCx_IRQn(port) (SC3_IRQn)
#elif defined(SC1) && defined(SC3)
  extern __IO SC_TypeDef * const halInternalScxReg[2];
  extern __IO uint32_t * const halInternalEventScxCfg[2];
  extern __IO uint32_t * const halInternalEventScxFlag[2];
  extern  uint8_t const halInternalEventScxIrqn[2];
  
  // The actual arrays referenced here live in micro-common.c
  // Keep in mind port 1 is SC1 and port 2 is SC3 (this is the tricky one)
  #define SCx_REG(port, reg) ((halInternalScxReg[(port)-1])->reg)
  #define EVENT_SCxCFG(port) (*(halInternalEventScxCfg[(port)-1]))
  #define EVENT_SCxFLAG(port) (*(halInternalEventScxFlag[(port)-1]))
  #define SCx_IRQn(port) ((IRQn_Type)halInternalEventScxIrqn[(port)-1])
#endif

/**
 * @brief Change the CTUNE value. Involves switching to HFRCO and turning off
 * the HFXO temporarily. (Only valid on EFR32)
 *
 * @param tune   Value to set CTUNE to.
 *
 */
#define halInternalSetCtune(tune)

/**
 * @brief Get the CTUNE value. (Only valid on EFR32)
 *
 * @return Current CTUNE value.
 *
 */
#define halInternalGetCtune() 0


#endif //__EM3XX_MICRO_COMMON_H__

/**@} // END micro group
 */
