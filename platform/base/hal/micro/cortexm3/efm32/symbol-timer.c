/*
 * File: symbol-timer.c
 * Description: Symbol timer code.
 *
 * Copyright 2013 by Silicon Laboratories. All rights reserved.
 */


#include PLATFORM_HEADER
#include BOARD_HEADER

#include "em_device.h"

// The EFR uses a different symbol timer implementation
#ifndef _EFR_DEVICE

#include "em_cmu.h"
#include "em_timer.h"

#include "stack/include/ember-types.h"
#include "phy/phy.h"
#include "hal/micro/micro-common.h"
#include "hal/micro/cortexm3/efm32/micro-common.h"

#define EMBER_PHY_UTILS_DIV_AND_ROUND(a,b) (((a) + ((b)/2))/(b))

#if defined(_EZR32_HAPPY_FAMILY)
#define SYMBOL_TIMER            TIMER2
#define SYMBOL_TIMER_CMU        cmuClock_TIMER2
#define SYMBOL_TIMER_IRQN       TIMER2_IRQn
#define SYMBOL_TIMER_IRQHANDLER TIMER2_IRQHandler
#define SYMBOL_TIMER_PRESCALE   timerPrescale128
#define SYMBOL_TIMER_MAX        0x0000FFFF
#elif defined(_EZR32_LEOPARD_FAMILY) || defined(_EZR32_WONDER_FAMILY)
#define SYMBOL_TIMER            TIMER3
#define SYMBOL_TIMER_CMU        cmuClock_TIMER3
#define SYMBOL_TIMER_IRQN       TIMER3_IRQn
#define SYMBOL_TIMER_IRQHANDLER TIMER3_IRQHandler
#define SYMBOL_TIMER_PRESCALE   timerPrescale128
#define SYMBOL_TIMER_MAX        0x0000FFFF
#else
#error "No symbol timer defined (symbol-timer.c)"
#endif //_EZR32_HAPPY_FAMILY

// We need to make sure that the high byte didn't change. This function serves
// to this purpose.
uint16_t halGetSymbolTimerTick(void)
{
  return TIMER_CounterGet(SYMBOL_TIMER);
}

void halInternalStartSymbolTimer(void)
{
  // Select TIMER3 parameters
  TIMER_Init_TypeDef timerInit =
  {
    .enable     = true,
    .debugRun   = false,
    .prescale   = SYMBOL_TIMER_PRESCALE,
    .clkSel     = timerClkSelHFPerClk,
    .fallAction = timerInputActionNone,
    .riseAction = timerInputActionNone,
    .mode       = timerModeUp,
    .dmaClrAct  = false,
    .quadModeX4 = false,
    .oneShot    = false,
    .sync       = false,
  };

  // Enable peripheral clocks
  CMU_ClockEnable(cmuClock_HFPER, true);

  // Enable clock for SYMBOL_TIMER module
  CMU_ClockEnable(SYMBOL_TIMER_CMU, true);

  // Set SYMBOL_TIMER Top value
  TIMER_TopSet(SYMBOL_TIMER, SYMBOL_TIMER_MAX);

  // Configure SYMBOL_TIMER
  TIMER_Init(SYMBOL_TIMER, &timerInit);
}

void halStackOrderInt16uSymbolDelayA(uint16_t symbols)
{

  // Read master clock rate, scale it to symbol timer and convert to kHz.
  uint32_t symbolTimerkHz = (CMU_ClockFreqGet(SYMBOL_TIMER_CMU) / 1000) >> SYMBOL_TIMER_PRESCALE;

  // Delay at least for 1 symbol time.
  if (!symbols) 
  {
    symbols++;
  }

  // Note: the timer frequency is designed in a way to be able to operate at
  //       least between the following conditions (BE: random back-off exponent):
  //            - data rate:   4800 bps, symbol bits: 1,  BE: 1
  //            - data rate: 200000 bps, symbol bits: 1,  BE: 5
  uint32_t ticksUnscaled = (uint32_t)( EMBER_PHY_SYMBOLS_TO_US((uint32_t)symbols) * symbolTimerkHz );
  uint32_t ticks         = (uint32_t)( EMBER_PHY_UTILS_DIV_AND_ROUND(ticksUnscaled, 1000) );
  
  // Assert if requested delay would cause the timer to overflow or happens to be zero.
  assert ( (ticks - 1) < SYMBOL_TIMER_MAX );

  // Set-up the symbol timer
  TIMER_CounterSet(SYMBOL_TIMER, 0);
  TIMER_TopSet(SYMBOL_TIMER, (uint16_t)ticks);

  // Clear flag for SYMBOL_TIMER overflow interrupt
  TIMER_IntClear(SYMBOL_TIMER, TIMER_IF_OF);
  // Enable timer interrupt and start the timer
  TIMER_IntEnable(SYMBOL_TIMER, TIMER_IF_OF);

  // Enable SYMBOL_TIMER interrupt vector in NVIC
  NVIC_EnableIRQ(SYMBOL_TIMER_IRQN);
}

void halStackCancelSymbolDelayA(void) {
  // Clear flag for SYMBOL_TIMER overflow interrupt
  TIMER_IntClear(SYMBOL_TIMER, TIMER_IF_OF);

  /* Disable overflow IT */
  TIMER_IntDisable(SYMBOL_TIMER, TIMER_IF_OF);
}

/**************************************************************************//**
 * @brief SYMBOL_TIMER_IRQHANDLER
 * Interrupt Service Routine SYMBOL_TIMER Interrupt Line
 *****************************************************************************/

extern void halStackSymbolDelayAIsr(void);

void SYMBOL_TIMER_IRQHANDLER(void)
{
  // Clear flag for SYMBOL_TIMER overflow interrupt
  TIMER_IntClear(SYMBOL_TIMER, TIMER_IF_OF);

  // Disable overflow IT
  TIMER_IntDisable(SYMBOL_TIMER, TIMER_IF_OF);

  // Call ISR callback
  halStackSymbolDelayAIsr();
}

#endif // _EFR_DEVICE
