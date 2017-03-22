/***************************************************************************//**
 * @file em_usbtimer.c
 * @brief USB protocol stack library, timer API.
 * @version 3.20.10
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

#include PLATFORM_HEADER
#include "stack/include/ember.h"
#include "hal/hal.h"

#if CORTEXM3_EM35X_USB
#include "em_usb.h"
#include "em_usbtypes.h"
#include "em_usbhal.h"

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

#define TMR1_VECNUM 16
#define TMR2_VECNUM 17

#ifndef USB_TIMER
#define USB_TIMER USB_TIMER0
#endif

#if ( USB_TIMER == USB_TIMER0 )
  #define TMR_INT_MASK      INT_TIM1CFG
  #define TMR_CR1           TIM1_CR1
  #define TMR_CR2           TIM1_CR2
  #define TMR_SMCR          TIM1_SMCR
  #define TMR_EGR           TIM1_EGR
  #define TMR_CCMR1         TIM1_CCMR1
  #define TMR_CCMR2         TIM1_CCMR2
  #define TMR_CCER          TIM1_CCER
  #define TMR_CNT           TIM1_CNT
  #define TMR_PSC           TIM1_PSC
  #define TMR_ARR           TIM1_ARR
  #define TMR_CCR1          TIM1_CCR1
  #define TMR_CCR2          TIM1_CCR2
  #define TMR_CCR3          TIM1_CCR3
  #define TMR_CCR4          TIM1_CCR4
  #define TMR_OR            TIM1_OR
  #define TMR_INT_SRC       INT_TIM1FLAG
  #define TMR_INT_MISSED    INT_TIM1MISS
  #define TMR_VECNUM        TMR1_VECNUM
  #define INT_TIM           INT_TIM1
#elif ( USB_TIMER == USB_TIMER1 )
  #define TMR_INT_MASK      INT_TIM2CFG
  #define TMR_CR1           TIM2_CR1
  #define TMR_CR2           TIM2_CR2
  #define TMR_SMCR          TIM2_SMCR
  #define TMR_EGR           TIM2_EGR
  #define TMR_CCMR1         TIM2_CCMR1
  #define TMR_CCMR2         TIM2_CCMR2
  #define TMR_CCER          TIM2_CCER
  #define TMR_CNT           TIM2_CNT
  #define TMR_PSC           TIM2_PSC
  #define TMR_ARR           TIM2_ARR
  #define TMR_CCR1          TIM2_CCR1
  #define TMR_CCR2          TIM2_CCR2
  #define TMR_CCR3          TIM2_CCR3
  #define TMR_CCR4          TIM2_CCR4
  #define TMR_OR            TIM2_OR
  #define TMR_INT_SRC       INT_TIM2FLAG
  #define TMR_INT_MISSED    INT_TIM2MISS
  #define TMR_VECNUM        TMR2_VECNUM
  #define INT_TIM           INT_TIM2
#else
#error "Illegal USB TIMER definition"
#endif


typedef struct _timer
{
  uint32_t                  timeout;  /* Delta value relative to prev. timer */
  struct _timer             *next;
  USBTIMER_Callback_TypeDef callback;
  bool                      running;
} USBTIMER_Timer_TypeDef;

#if ( NUM_QTIMERS > 0 )
static USBTIMER_Timer_TypeDef timers[ NUM_QTIMERS ];
static USBTIMER_Timer_TypeDef *head = NULL;
#endif

static uint32_t ticksPrMs, ticksPr1us, ticksPr10us, ticksPr100us;

#if ( NUM_QTIMERS > 0 )

static void TimerTick( void );

void TIMER_IRQHandler( void )
{
  if ( TMR_INT_SRC & INT_TIMCC1IF )
  {
    // clear compare interrupt flag
    TMR_INT_SRC |= INT_TIMCC1IF_MASK;
    //set compare register
    TMR_CCR1 = TMR_CNT + ticksPrMs;
    
    TimerTick();
  }
}
#endif /* ( NUM_QTIMERS > 0 ) */

static void DelayTicks( uint16_t ticks )
{
  uint16_t startTime;
  volatile uint16_t now;

  if ( ticks )
  {
    startTime = TMR_CNT;
    do
    {
      now = TMR_CNT;
    } while ( (uint16_t)( now - startTime ) < ticks );
  }
}

/** @endcond */

/** @addtogroup USB_COMMON
 *  @{*/

/***************************************************************************//**
 * @brief
 *   Active wait millisecond delay function. Can also be used inside
 *   interrupt handlers.
 *
 * @param[in] msec
 *   Number of milliseconds to wait.
 ******************************************************************************/
void USBTIMER_DelayMs( uint32_t msec )
{
  uint64_t totalTicks;

  totalTicks = (uint64_t)ticksPrMs * msec;
  while ( totalTicks > 20000 )
  {
    DelayTicks( 20000 );
    totalTicks -= 20000;
  }
  DelayTicks( (uint16_t)totalTicks );
}

/***************************************************************************//**
 * @brief
 *   Active wait microsecond delay function. Can also be used inside
 *   interrupt handlers.
 *
 * @param[in] usec
 *   Number of microseconds to wait.
 ******************************************************************************/
void USBTIMER_DelayUs( uint32_t usec )
{
  uint64_t totalTicks;

  totalTicks = (uint64_t)ticksPr1us * usec;
  if ( totalTicks == 0 )
  {
    usec /= 10;
    totalTicks = (uint64_t)ticksPr10us * usec;

    if ( totalTicks == 0 )
    {
      usec /= 10;
      totalTicks = (uint64_t)ticksPr100us * usec;
    }
  }

  while ( totalTicks > 60000 )
  {
    DelayTicks( 60000 );
    totalTicks -= 60000;
  }
  DelayTicks( (uint16_t)totalTicks );
}

/***************************************************************************//**
 * @brief
 *   Activate the hardware timer used to pace the 1 millisecond timer system.
 *
 * @details
 *   Call this function whenever the HFPERCLK frequency is changed.
 *   This function is initially called by HOST and DEVICE stack xxxx_Init()
 *   functions.
 ******************************************************************************/
void USBTIMER_Init( void )
{
  uint32_t freq;
  freq = halPeripheralClockHz();
  ticksPrMs = ( freq + 500 ) / 1000;
  ticksPr1us = ( freq + 500000 ) / 1000000;
  ticksPr10us = ( freq + 50000 ) / 100000;
  ticksPr100us = ( freq + 5000 ) / 10000;

#if ( NUM_QTIMERS > 0 )
  halRegisterRamVector(TMR_VECNUM, (uint32_t)TIMER_IRQHandler);
  //set compare register
  TMR_CCR1 = TMR_CNT + ticksPrMs;
  // enable interrupt
  TMR_INT_MASK = INT_TIMCC1IF;
  INT_CFGSET = INT_TIM;  
#endif /* ( NUM_QTIMERS > 0 ) */
  //configure channel as output
  TMR_CCMR1 &= ~TIM_CC1S_MASK;
  //enable output pin
  TMR_CCER |= TIM_CC1E_MASK;
  //set output mode to toggle output pin when counter reaches
  //compare values, preload is not used and the output pin is
  //active high.
  TMR_CCMR1 &= ~TIM_OC1M_MASK;
  TMR_CCMR1 |= (0x3 << TIM_OC1M_BIT);
  //enable timer
  TMR_CR1 |= TIM_CEN_MASK;
}

#if ( NUM_QTIMERS > 0 ) || defined( DOXY_DOC_ONLY )
/***************************************************************************//**
 * @brief
 *   Start a timer.
 *
 * @details
 *   If the timer is already running, it will be restarted with new timeout.
 *
 * @param[in] id
 *   Timer id (0..).
 *
 * @param[in] timeout
 *   Number of milliseconds before timer will elapse.
 *
 * @param[in] callback
 *   Function to be called on timer elapse, ref. @ref USBTIMER_Callback_TypeDef.
 ******************************************************************************/
void USBTIMER_Start( uint32_t id, uint32_t timeout,
                     USBTIMER_Callback_TypeDef callback )
{
  uint32_t accumulated;
  USBTIMER_Timer_TypeDef *this, **last;

  
  DECLARE_INTERRUPT_STATE;
  DISABLE_INTERRUPTS();

  if ( timers[ id ].running )
  {
    USBTIMER_Stop( id );
  }

  if ( timeout == 0 )
  {
    callback();
    RESTORE_INTERRUPTS();
    return;
  }

  timers[ id ].running  = true;
  timers[ id ].callback = callback;
  timers[ id ].next     = NULL;

  if ( !head )                                        /* Queue empty ? */
  {
    timers[ id ].timeout  = timeout;
    head = &timers[ id ];
  }
  else
  {
    this = head;
    last = &head;
    accumulated = 0;

    /* Do a sorted insert */
    while ( this  )
    {
      if ( timeout < accumulated + this->timeout )  /* Insert before "this" ? */
      {
        timers[ id ].timeout  = timeout - accumulated;
        timers[ id ].next     = this;
        *last = &timers[ id ];
        this->timeout -= timers[ id ].timeout;        /* Adjust timeout     */
        break;
      }
      else if ( this->next == NULL )                  /* At end of queue ?  */
      {
        timers[ id ].timeout  = timeout - accumulated - this->timeout;
        this->next = &timers[ id ];
        break;
      }
      accumulated += this->timeout;
      last = &this->next;
      this = this->next;
    }
  }

  RESTORE_INTERRUPTS();
}

/***************************************************************************//**
 * @brief
 *   Stop a timer.
 *
 * @param[in] id
 *   Timer id (0..).
 ******************************************************************************/
void USBTIMER_Stop( uint32_t id )
{
  USBTIMER_Timer_TypeDef *this, **last;

  ATOMIC(
  if ( head )                                           /* Queue empty ?    */
  {
    this = head;
    last = &head;
    timers[ id ].running = false;

    while ( this  )
    {
      if ( this == &timers[ id ] )                      /* Correct timer ?  */
      {
        if ( this->next )
        {
          this->next->timeout += timers[ id ].timeout;  /* Adjust timeout   */
        }
        *last = this->next;
        break;
      }
      last = &this->next;
      this = this->next;
    }
  }
  ) //ATOMIC
}
#endif /* ( NUM_QTIMERS > 0 ) */

/** @} (end addtogroup USB_COMMON) */

#if ( NUM_QTIMERS > 0 )
/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

static void TimerTick( void )
{
  USBTIMER_Callback_TypeDef cb;

  ATOMIC(
  if ( head )
  {
    head->timeout--;

    while ( head  )
    {
      if ( head->timeout == 0 )
      {
        cb = head->callback;
        head->running = false;
        head = head->next;
        /* The callback may place new items in the queue !!! */
        if ( cb )
        {
          (cb)();
        }
        continue; /* There might be more than one timeout pr. tick */
      }
      break;
    }
  }
  ) //ATOMIC
}
/** @endcond */
#endif /* ( NUM_QTIMERS > 0 ) */

#endif //CORTEXM3_EM35X_USB
