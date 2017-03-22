/** @file symbol-timer.h
 *
 * @brief Functions that provide access to symbol time. One symbol period is 16
 * microseconds.
 *
 * See @ref symbol_timer for documentation.
 *
 * <!-- Copyright 2005 by Ember Corporation. All rights reserved. -->
 */

/** @addtogroup symbol_timer
 * See symbol-timer.h for source code.
 *@{
 */


#ifndef __SYMBOL_TIMER_H__
#define __SYMBOL_TIMER_H__

/** @name Symbol Timer Functions
 */
//@{

/** @brief Initializes the symbol timer.  When a dedicated symbol timer
    peripheral exists (e.g. EM2xx, EM3xx) this initialization is generally
    performed directly by the PHY, so this routine may be a no-op.
  */
void halInternalStartSymbolTimer(void);

/** @brief Returns the current symbol time in symbol ticks (units are
 * platform-dependent, but typically on the order of microseconds).
 * @return The least significant 32 bits of the current symbol time in symbol
 * timer ticks.
 */
uint32_t halStackGetInt32uSymbolTick(void);

// New API:

/** @brief Returns true if symbol tick time st1 is greater than symbol tick
 * time st2, as determined by half the range of the symbol timer.  Can only
 * account for 1 wrap around between st1 and st2 before it is wrong.
 */
bool halStackInt32uSymbolTickGTorEqual(uint32_t st1, uint32_t st2);

/**@brief Obtains the number of symtol timer ticks in one second of real time.
 * Can be used for conversion between real time and symbol ticks.
 *
 * @return  How many symbol ticks occur in a second.
 */
uint32_t halStackGetSymbolTicksPerSecond(void);

/** @} END of Symbol Timer Functions */


/** @name MAC Timer Support Functions
 * These functions are used for MAC layer timing and symbol-based delays.
 *
 * Applications should not directly call these functions. They
 * are used internally by the operation of the stack.
 */
//@{

// New API:

/** @brief Specifies two independent channels for symbol delay operations.
 */
typedef enum {
  EM_HAL_SYMBOL_DELAY_CHANNEL_A,  // 0
  EM_HAL_SYMBOL_DELAY_CHANNEL_B,  // 1
  EM_HAL_SYMBOL_DELAY_CHANNELS    // Must be last
} EmHalSymbolDelayChannel_t;

/** @brief Specifies the callback API triggered when the symbol timer channel
 * expires.
 */
typedef void (*EmHalSymbolDelayCallback_t)(EmHalSymbolDelayChannel_t delayChan);

/**@brief Sets up a delay timer to call the indicated interrupt-context callback
 * when it expires.
 *
 * Used by the MAC to request an interrupt callback at a specified number of
 * symbol timer ticks in the future.
 *
 * @param delayChan  The symbol timer delay channel to use.
 * @param callback  The callback to call in interrupt context when the delay
 * expires.  If NULL, the timer is not actually started, but the return value
 * is accurate; useful for polling.
 * @param microseconds  The delay, in units of microseconds.
 *
 * @return  The absolute symbol tick value of the delay (usually in the future).
 *
 * @note  Internal conversion of microseconds to symbol ticks will use a
 * rounding function.
 * @note  No minimum delay is enforced; short delays may trigger the callback
 * immediately, before this routine returns to its caller.
 */
uint32_t halStackOrderSymbolDelay(EmHalSymbolDelayChannel_t delayChan,
                                  EmHalSymbolDelayCallback_t callback,
                                  uint32_t microseconds);

/**@brief Cancels the delay set up by an earlier halStackOrderSymbolDelay()
 * call.
 *
 * @param delayChan  The symbol timer delay channel specified in the earlier
 * halStackOrderSymbolDelay() call.
 * @param callback  The callback specified in the earlier
 * halStackOrderSymbolDelay() call.
 *
 * @note If cancelled prior to the delay expiring, the original callback will
 * not be called.
 */
void halStackCancelSymbolDelay(EmHalSymbolDelayChannel_t delayChan,
                               EmHalSymbolDelayCallback_t callback);

// Old API below is deprecated; retained for backwards-compatibility only:

/**@brief Sets up a timer and calls an interrupt-context callback
 * when it expires.
 *
 * Used by the MAC to request an interrupt callback at a specified amount
 * of time in the future.
 *
 * @param symbols  The delay, in symbols.
 */
void halStackOrderInt16uSymbolDelayA(uint16_t symbols);

/**@brief Cancels the timer set up by halStackOrderInt16uSymbolDelayA().
 */
void halStackCancelSymbolDelayA(void);

/** @brief This is the interrupt level callback into the stack that is
 * called when the timers set by halStackOrderInt16uSymbolDelayA expire.
 */
void halStackSymbolDelayAIsr(void);

/** @} END of MAC Timer Support Functions */

#endif //__SYMBOL_TIMER_H__

/** @} END addtogroup */
