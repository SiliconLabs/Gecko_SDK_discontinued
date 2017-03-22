// Copyright 2016 Silicon Laboratories, Inc.                                *80*

#include PLATFORM_HEADER
#include CONFIGURATION_HEADER
#include "stack/include/ember-types.h"
#include "stack/include/event.h"
#include "hal/hal.h"

#include EMBER_AF_API_BULB_PWM_DRIVER

// EFR headers
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_timer.h"

// ---------- MACRO DEFINITIONS ----------
#ifndef PWM_DEFAULT_FREQUENCY
  #define PWM_DEFAULT_FREQUENCY 1000
#endif

#define CLOCK_FREQUENCY    38400000

#if defined(EMBER_AF_PLUGIN_PWM_CONTROL_PWM_POLARITY)
  #if EMBER_AF_PLUGIN_PWM_CONTROL_PWM_POLARITY == 0
    #define PWM_INVERT_OUTPUT true
  #else
    #define PWM_INVERT_OUTPUT false
  #endif
#else
  #define PWM_INVERT_OUTPUT false
#endif

#define TIMER_DEFAULT_INIT \
  { \
    .eventCtrl  = timerEventEveryEdge, \
    .edge       = timerEdgeBoth, \
    .prsSel     = timerPRSSELCh0, \
    .cufoa      = timerOutputActionNone, \
    .cofoa      = timerOutputActionNone, \
    .cmoa       = timerOutputActionToggle, \
    .mode       = timerCCModePWM, \
    .filter     = false, \
    .prsInput   = false, \
    .coist      = false, \
    .outInvert  = PWM_INVERT_OUTPUT, \
  }

// ---------- GLOBAL VARIABLES ----------
static uint16_t ticksPerPeriod;
static uint16_t pwmFrequency;

// ---------- Predeclared callbacks ----------
uint16_t halBulbPwmDriverFrequencyCallback( void );
void halBulbPwmDriverInitCompleteCallback( void );

// Funciton to initialize the blink code datastructures.
void halBulbPwmDriverBlinkInit( void );

// ----------- function definitions
static void initGpio( void )
{
  // GPIO setup
  CMU_ClockEnable(cmuClock_GPIO, 1);

#ifdef BULB_PWM_WHITE_PORT
  GPIO_PinModeSet(BULB_PWM_WHITE_PORT,
                  BULB_PWM_WHITE_PIN,
                  gpioModePushPull,
                  0);
#endif
#ifdef BULB_PWM_RED_PORT
  GPIO_PinModeSet(BULB_PWM_RED_PORT, BULB_PWM_RED_PIN, gpioModePushPull, 0);
#endif
#ifdef BULB_PWM_GREEN_PORT
  GPIO_PinModeSet(BULB_PWM_GREEN_PORT, BULB_PWM_GREEN_PIN, gpioModePushPull, 0);
#endif
#ifdef BULB_PWM_BLUE_PORT
  GPIO_PinModeSet(BULB_PWM_BLUE_PORT, BULB_PWM_BLUE_PIN, gpioModePushPull, 0);
#endif
#ifdef BULB_PWM_LOWTEMP_PORT
  GPIO_PinModeSet(BULB_PWM_LOWTEMP_PORT,
                  BULB_PWM_LOWTEMP_PIN,
                  gpioModePushPull,
                  0);
#endif
#ifdef BULB_PWM_STATUS
  GPIO_PinModeSet(BULB_PWM_STATUS_PORT,
                  BULB_PWM_STATUS_PIN,
                  gpioModePushPull,
                  0);
#endif

}

static void initFrequency( void )
{
  TIMER_InitCC_TypeDef timerCCInit = TIMER_DEFAULT_INIT;

  /* Select timer parameters */
  TIMER_Init_TypeDef timerInit = {
    .enable     = true,
    .debugRun   = true,
    .prescale   = timerPrescale1,
    .clkSel     = timerClkSelHFPerClk,
    .fallAction = timerInputActionNone,
    .riseAction = timerInputActionNone,
    .mode       = timerModeUp,
    .dmaClrAct  = false,
    .quadModeX4 = false,
    .oneShot    = false,
    .sync       = false,
  };

  uint32_t ticksPerPeriod32;

  pwmFrequency = halBulbPwmDriverFrequencyCallback();

  if(pwmFrequency == HAL_BULB_PWM_DRIVER_USE_DEFAULT_FREQUENCY) {
    pwmFrequency = PWM_DEFAULT_FREQUENCY;
  }

  ticksPerPeriod32 = (uint32_t) CLOCK_FREQUENCY;
  ticksPerPeriod32 /= (uint32_t) pwmFrequency;

  ticksPerPeriod = (uint16_t) ticksPerPeriod32;

#ifdef BULB_PWM_USING_TIMER0
  CMU_ClockEnable(cmuClock_TIMER0, true);

#ifdef TIMER0_CHANNEL0_LOC
  TIMER_InitCC(TIMER0, 0, &timerCCInit);
  TIMER0->ROUTEPEN |= TIMER_ROUTEPEN_CC0PEN;
  TIMER0->ROUTELOC0 |= TIMER0_CHANNEL0_LOC;
  TIMER_CompareBufSet(TIMER0, 0, 0);
#endif

#ifdef TIMER0_CHANNEL1_LOC
  TIMER_InitCC(TIMER0, 1, &timerCCInit);
  TIMER0->ROUTEPEN |= TIMER_ROUTEPEN_CC1PEN;
  TIMER0->ROUTELOC0 |= TIMER0_CHANNEL1_LOC;
  TIMER_CompareBufSet(TIMER0, 1, 0);
#endif

#ifdef TIMER0_CHANNEL2_LOC
  TIMER_InitCC(TIMER0, 2, &timerCCInit);
  TIMER0->ROUTEPEN |= TIMER_ROUTEPEN_CC2PEN;
  TIMER0->ROUTELOC0 |= TIMER0_CHANNEL2_LOC;
  TIMER_CompareBufSet(TIMER0, 2, 0);
#endif

  // Set Top Value
  TIMER_TopSet(TIMER0, ticksPerPeriod);

  // Set compare value starting at 0 - it will be incremented in the interrupt
  // handler
  TIMER_Init(TIMER0, &timerInit);
#endif

#ifdef BULB_PWM_USING_TIMER1
  CMU_ClockEnable(cmuClock_TIMER1, true);

#ifdef TIMER1_CHANNEL0_LOC
  TIMER_InitCC(TIMER1, 0, &timerCCInit);
  TIMER1->ROUTEPEN |= TIMER_ROUTEPEN_CC0PEN;
  TIMER1->ROUTELOC0 |= TIMER1_CHANNEL0_LOC;
  TIMER_CompareBufSet(TIMER1, 0, 0);
#endif

#ifdef TIMER1_CHANNEL1_LOC
  TIMER_InitCC(TIMER1, 1, &timerCCInit);
  TIMER1->ROUTEPEN |= TIMER_ROUTEPEN_CC1PEN;
  TIMER1->ROUTELOC0 |= TIMER1_CHANNEL1_LOC;
  TIMER_CompareBufSet(TIMER1, 1, 0);
#endif

#ifdef TIMER1_CHANNEL2_LOC
  TIMER_InitCC(TIMER1, 2, &timerCCInit);
  TIMER1->ROUTEPEN |= TIMER_ROUTEPEN_CC2PEN;
  TIMER1->ROUTELOC0 |= TIMER1_CHANNEL2_LOC;
  TIMER_CompareBufSet(TIMER1, 2, 0);
#endif

#ifdef TIMER1_CHANNEL3_LOC
  TIMER_InitCC(TIMER1, 3, &timerCCInit);
  TIMER1->ROUTEPEN |= TIMER_ROUTEPEN_CC3PEN;
  TIMER1->ROUTELOC0 |= TIMER1_CHANNEL3_LOC;
  TIMER_CompareBufSet(TIMER1, 3, 0);
#endif

  // Set Top Value
  TIMER_TopSet(TIMER1, ticksPerPeriod);

  // Set compare value starting at 0 - it will be incremented in the interrupt
  // handler
  TIMER_Init(TIMER1, &timerInit);
#endif
}

void halBulbPwmDriverInitialize( void )
{
  initGpio();

  initFrequency();

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
#ifdef BULB_PWM_WHITE
  case BULB_PWM_WHITE:
    TIMER_CompareBufSet(BULB_PWM_WHITE_TIMER, BULB_PWM_WHITE_CHANNEL, value);
    break;
#endif
#ifdef BULB_PWM_LOWTEMP
  case BULB_PWM_LOWTEMP:
    TIMER_CompareBufSet(BULB_PWM_LOWTEMP_TIMER, BULB_PWM_LOWTEMP_CHANNEL, value);
    break;
#endif
#ifdef BULB_PWM_STATUS
  case BULB_PWM_STATUS:
    TIMER_CompareBufSet(BULB_PWM_STATUS_TIMER, BULB_PWM_STATUS_CHANNEL, value);
    break;
#endif
#ifdef BULB_PWM_RED
  case BULB_PWM_RED:
    TIMER_CompareBufSet(BULB_PWM_RED_TIMER, BULB_PWM_RED_CHANNEL, value);
    break;
#endif
#ifdef BULB_PWM_GREEN
  case BULB_PWM_GREEN:
    TIMER_CompareBufSet(BULB_PWM_GREEN_TIMER, BULB_PWM_GREEN_CHANNEL, value);
    break;
#endif
#ifdef BULB_PWM_BLUE
  case BULB_PWM_BLUE:
    TIMER_CompareBufSet(BULB_PWM_BLUE_TIMER, BULB_PWM_BLUE_CHANNEL, value);
    break;
#endif
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
