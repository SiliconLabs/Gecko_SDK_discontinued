// Copyright 2016 Silicon Laboratories, Inc.                                *80*

#include PLATFORM_HEADER
#include CONFIGURATION_HEADER
#include "stack/include/ember-types.h"
#include "stack/include/event.h"
#include "hal/hal.h"

#include EMBER_AF_API_BULB_PWM_DRIVER

// ---------- MACRO DEFINITIONS ----------
#ifndef PWM_DEFAULT_FREQUENCY
  // Ideally, this is specified in the board.h file.  If it is not, we are
  // assuming the customer is using our reference design.

  #define PWM_DEFAULT_FREQUENCY 1000 // 1 kHz.
#endif

#ifndef CLOCK_FREQUENCY
  #define CLOCK_FREQUENCY    6000000
#endif

#define PWM_POLARITY      EMBER_AF_PLUGIN_PWM_CONTROL_PWM_POLARITY

// ---------- GLOBAL VARIABLES ----------
static uint16_t ticksPerPeriod;
static uint16_t pwmFrequency;

// predeclare callbacks.
uint16_t halBulbPwmDriverFrequencyCallback( void );
void halBulbPwmDriverInitCompleteCallback( void );
void halBulbPwmDriverBlinkInit( void );

// ----------- function definitions

void initFrequency( void )
{
  uint32_t ticksPerPeriod32;

  pwmFrequency = halBulbPwmDriverFrequencyCallback();

  if (pwmFrequency == HAL_BULB_PWM_DRIVER_USE_DEFAULT_FREQUENCY) {
     pwmFrequency = PWM_DEFAULT_FREQUENCY;
  }

  ticksPerPeriod32 = (uint32_t) CLOCK_FREQUENCY;
  ticksPerPeriod32 /= (uint32_t) pwmFrequency;

  ticksPerPeriod = (uint16_t) ticksPerPeriod32;
}

void halBulbPwmDriverInitialize( void )
{
  initFrequency();

  // limiting choices to PB 1, 2, 3, and 4.  This is Timer 2 alternate
  // channels.
  TIM2_OR = TIM_REMAPC4 | TIM_REMAPC3 | TIM_REMAPC2 | TIM_REMAPC1;

  TIM2_PSC = 1; //1^2=2 -> 12MHz/2 = 6 MHz = 6000 ticks per 1/1000 of a second
  TIM2_EGR = 1; //trigger update event to load new prescaler value
  TIM2_CCMR1  = 0; //start from a zeroed configuration
  TIM2_ARR = halBulbPwmDriverTicksPerPeriod();  // set the period
  TIM2_CNT = 0; //force the counter back to zero

  // set all PWMs to 0
  TIM2_CCR1 = 0;
  TIM2_CCR2 = 0;
  TIM2_CCR3 = 0;
  TIM2_CCR4 = 0;

  // Output waveforms on all channels.
  TIM2_CCMR2 |= (0x7 << TIM_OC3M_BIT) |
    (0x7 << TIM_OC4M_BIT);

  TIM2_CCMR1 |= (0x7 << TIM_OC1M_BIT) |
    (0x7 << TIM_OC2M_BIT);

  ATOMIC(
  // polarity:
#if (PWM_POLARITY == 0)
#if PWM_DEFAULT_FREQUENCY > 0
  TIM2_CCER |= TIM_CC1P;    // set up PWM 1 as active low
  TIM2_CCER |= TIM_CC2P;    // set up PWM 2 as active low
  TIM2_CCER |= TIM_CC3P;    // set up PWM 3 as active low
  TIM2_CCER |= TIM_CC4P;    // set up PWM 4 as active low
#endif // timer 2 setup

#endif // active low

  TIM2_CCER |= TIM_CC1E;    //enable output on channel 1
  TIM2_CCER |= TIM_CC2E;    //enable output on channel 2
  TIM2_CCER |= TIM_CC3E;    //enable output on channel 3
  TIM2_CCER |= TIM_CC4E;    //enable output on channel 4
  TIM2_CR1  |= TIM_CEN;     //enable counting
  )

  halBulbPwmDriverBlinkInit();

  halBulbPwmDriverInitCompleteCallback();
}

uint16_t halBulbPwmDriverTicksPerPeriod( void )
{
  return ticksPerPeriod;
}

uint16_t halBulbPwmDriverTicksPerMicrosecond( void )
{
  return (uint16_t) (CLOCK_FREQUENCY / 1000000);
}

void halBulbPwmDriverSetPwmLevel( uint16_t value, uint8_t pwm )
{
  switch(pwm) {
  case 1:
    TIM2_CCR1 = value;
    break;
  case 2:
    TIM2_CCR2 = value;
    break;
  case 3:
    TIM2_CCR3 = value;
    break;
  case 4:
    TIM2_CCR4 = value;
    break;
  default:
    assert(0);
    break;
  }
}

// Required for AFv2 support
void emberAfPluginBulbPwmDriverInitCallback()
{
  halBulbPwmDriverInitialize();
}

void halBulbPwmDriverStatusLedOn( void )
{
#ifdef BULB_PWM_STATUS
  halBulbPwmDriverSetPwmLevel( halBulbPwmDriverTicksPerPeriod(),
                               BULB_PWM_STATUS );
#endif
}

void halBulbPwmDriverStatusLedOff( void )
{
#ifdef BULB_PWM_STATUS
  halBulbPwmDriverSetPwmLevel( 0, BULB_PWM_STATUS );
#endif
}
