/*
 * File: micro-common.c
 * Description: EM3XX micro specific HAL functions common to
 *  full and minimal hal
 *
 * Copyright 2013 Silicon Laboratories, Inc.                                *80*
 */

#include PLATFORM_HEADER
#include "include/error.h"
#include "hal/micro/micro-common.h"
#include "hal/micro/cortexm3/micro-common.h"
#if defined(BOARD_HEADER) && !defined(MINIMAL_HAL)
  #include BOARD_HEADER
#endif //defined(BOARD_HEADER)

//NOTE:  The reason ENABLE_OSC32K gets translated into an uint8_t is so that
//       haltest can override the build time configuration and force use of
//       a different OSC32K mode when running the "RTC" command which is used
//       to check proper connectivity of the 32kHz XTAL or external clock.
#define DO_EXPAND(sym)  1 ## sym
#define EXPAND(sym)     DO_EXPAND(sym)
#define CHECK_BLANK_DEFINE(sym) (1 == EXPAND(sym))

#ifdef  ENABLE_OSC32K
  #if CHECK_BLANK_DEFINE(ENABLE_OSC32K) // Empty #define => OSC32K_CRYSTAL
    #undef  ENABLE_OSC32K
    #define ENABLE_OSC32K OSC32K_CRYSTAL // Default if ENABLE_OSC32K is blank
  #endif
#else//!ENABLE_OSC32K
  #define ENABLE_OSC32K OSC32K_DISABLE // Default if BOARD_HEADER doesn't say
#endif//ENABLE_OSC32K
uint8_t useOsc32k = ENABLE_OSC32K;
#ifndef OSC32K_STARTUP_DELAY_MS
  #define OSC32K_STARTUP_DELAY_MS (0) // Not all BOARD_HEADERs define this
#endif//OSC32K_STARTUP_DELAY_MS

// Would like to move this to micro-common.h, but that's #included *before*
// the BOARD_HEADER where ENABLE_OSC24M might be defined :-(
#if     DISABLE_OSC24M
  // For compatibility, DISABLE_OSC24M takes precedence over ENABLE_OSC24M
  #undef  ENABLE_OSC24M
  #define ENABLE_OSC24M 0
#endif//DISABLE_OSC24M
#ifndef ENABLE_OSC24M
  #define  ENABLE_OSC24M 24 // Default is 24 MHz Xtal
#endif//ENABLE_OSC24M
#if     ((ENABLE_OSC24M >= 1000000) || (ENABLE_OSC24M <= -1000000)) // Hz
  #define OSC24M_HZ (ENABLE_OSC24M)
#elif   ((ENABLE_OSC24M >=    1000) || (ENABLE_OSC24M <=    -1000)) // KHz
  #define OSC24M_HZ (ENABLE_OSC24M * 1000L)
#else//!((ENABLE_OSC24M >=    1000) || (ENABLE_OSC24M <=    -1000)) // MHz
  #define OSC24M_HZ (ENABLE_OSC24M * 1000000L)
#endif//(ENABLE_OSC24M >= 1000000)

//[[
#if     (defined(EMBER_EMU_TEST) || defined(EMBER_EMU))
  extern bool halInternalIAmAnEmulator(void); // In hal library
  #define HALF_SPEED_SYSCLK halInternalIAmAnEmulator()
#else//!(defined(EMBER_EMU_TEST) || defined(EMBER_EMU))
//]]
  #define HALF_SPEED_SYSCLK (false)
//[[
#endif//(defined(EMBER_EMU_TEST) || defined(EMBER_EMU))
//]]

#if     BTL_SYSCLK_KNOWN

// serial-ota-bootloader must use 24 MHz Xtal
// This saves some much needed code space so serial-ota-bootloader fits

#else//!BTL_SYSCLK_KNOWN

// Expose configuration to library code
#if     (OSC24M_HZ == -1000000) // Sentinal for runtime determination
int32_t halSystemXtalHz = OSC24M_HZ;
#else//!(OSC24M_HZ == -1000000)
const int32_t halSystemXtalHz = OSC24M_HZ;
#endif//(OSC24M_HZ == -1000000)

uint32_t halSystemClockHz(void)
{
  if ((CMLV->OSC24MCTRL & CMLV_OSC24MCTRL_SEL) == 0) {
    // Running off internal fast RC
    #if     (defined(DISABLE_RC_CALIBRATION) || (ENABLE_OSC24M == 0))
      // RC is uncalibrated -- estimate using OSCHF_TUNE,
      // assuming typical centered at 9 MHz and 0.5 MHz +/- tuning increments
      // (per Lipari-S-002-Lipari_analogue_specification.doc Table 25.5)
      // Use bitfield to make unsigned field within word into signed word
      struct { int16_t bitfield : OSCHF_TUNE_FIELD_BITS; } offsetbf;
      offsetbf.bitfield = OSCHF_TUNE;
      int32_t offset = offsetbf.bitfield;
      return (9 * 1000000) - (offset * 500000);
    #elif  (ENABLE_OSC24M < 0)
      // Assume calibrated to 1/2 of halSystemXtalHz
      return -(halSystemXtalHz / 2);
    #else//(ENABLE_OSC24M > 0)
      // Assume calibrated to 1/2 of OSC24M
      return (OSC24M_HZ / 2);
    #endif//(defined(DISABLE_RC_CALIBRATION) || (ENABLE_OSC24M == 0))
  } else {
    // Running off external Xtal or signal -- or want what that would be
    #if     (ENABLE_OSC24M == 0)
      // RC is uncalibrated -- estimate using OSCHF_TUNE_VALUE (or actual
      // if no tuning will be done),
      // assuming typical centered at 9 MHz and 0.5 MHz +/- tuning increments
      // (per Lipari-S-002-Lipari_analogue_specification.doc Table 25.5)
      #if     (defined(DISABLE_RC_CALIBRATION) || !defined(OSCHF_TUNE_VALUE))
        // Use bitfield to make unsigned field within word into signed word
        struct { int16_t bitfield : OSCHF_TUNE_FIELD_BITS; } offsetbf;
        offsetbf.bitfield = OSCHF_TUNE;
        int32_t offset = offsetbf.bitfield;
      #else//!(defined(DISABLE_RC_CALIBRATION) || !defined(OSCHF_TUNE_VALUE))
        int32_t offset = OSCHF_TUNE_VALUE;
      #endif//(defined(DISABLE_RC_CALIBRATION) || !defined(OSCHF_TUNE_VALUE))
      return (9 * 1000000) - (offset * 500000);
    #elif   (ENABLE_OSC24M < 0)
      return -halSystemXtalHz;
    #else//!(ENABLE_OSC24M < 0)
      return (OSC24M_HZ / (HALF_SPEED_SYSCLK ? 2 : 1));
    #endif//(ENABLE_OSC24M < 0)
  }
}

uint32_t halMcuClockHz(void)
{
  uint32_t clkHz = halSystemClockHz();
  if (!CMLV->CPUCLKSEL && !HALF_SPEED_SYSCLK) {
    // MCU (FCLK) is running off PCLK = SYSCLK/2
    clkHz /= 2;
  } else {
    // MCU (FCLK) is (or will be) running off SYSCLK directly
    // With HALF_SPEED_SYSCLK, PCLK == SYSCLK and CMLV->CPUCLKSEL doesn't matter
  }
  return clkHz;
}

uint32_t halPeripheralClockHz()
{
  uint32_t clkHz = halSystemClockHz();
  if (HALF_SPEED_SYSCLK) {
    // PCLK == SYSCLK
  } else {
    clkHz /= 2; // PCLK == SYSLCK/2
  }
  return clkHz;
}

#endif//BTL_SYSCLK_KNOWN

extern void halInternalClocksCalibrateFastRc(void); // Belongs in a clocks.h
extern void halInternalClocksCalibrateSlowRc(void); // Belongs in a clocks.h

void halInternalCalibrateFastRc(void)
{
  #ifndef DISABLE_RC_CALIBRATION
    #if     (ENABLE_OSC24M == 0)
      #ifdef  OSCHF_TUNE_VALUE
        OSCHF_TUNE = (uint32_t) OSCHF_TUNE_VALUE;
      #endif//OSCHF_TUNE_VALUE
    #else//!(ENABLE_OSC24M == 0)
      halInternalClocksCalibrateFastRc();
    #endif//(ENABLE_OSC24M == 0)
  #endif//DISABLE_RC_CALIBRATION
}

void halInternalCalibrateSlowRc(void)
{
  #ifndef DISABLE_RC_CALIBRATION
    #if     (ENABLE_OSC24M != 0)
      halInternalClocksCalibrateSlowRc();
    #endif//(ENABLE_OSC24M == 0)
  #endif//DISABLE_RC_CALIBRATION
}

void halInternalEnableWatchDog(void)
{
  //Just to be on the safe side, restart the watchdog before enabling it
  WDOG->RESET = 1;
  WDOG->KEY = 0xEABE;
  WDOG->CFG = WDOG_CFG_ENABLE;
}

void halInternalResetWatchDog(void)
{
  //Writing any value will restart the watchdog
  WDOG->RESET = 1;
}

void halInternalDisableWatchDog(uint8_t magicKey)
{
  if (magicKey == MICRO_DISABLE_WATCH_DOG_KEY) {
    WDOG->KEY = 0xDEAD;
    WDOG->CFG = WDOG_CFG_DISABLE;
  }
}

bool halInternalWatchDogEnabled(void)
{
  if(WDOG->CFG & WDOG_CFG_ENABLE) {
    return true;
  } else {
    return false;
  }
}

void halGpioSetConfig(uint32_t gpio, HalGpioCfg_t config)
{
  uint32_t port = gpio / 8;
  uint32_t shift =  (gpio & 0x3) * 4;
  uint32_t saved = (gpio & 0x4) ? GPIO->P[port].CFGH : GPIO->P[port].CFGL;
  saved &= ~(0xF << shift);
  saved |= (config << shift);
  if(gpio & 0x4) {
    GPIO->P[port].CFGH = saved;
  } else {
    GPIO->P[port].CFGL = saved;
  }
}

HalGpioCfg_t halGpioGetConfig(uint32_t gpio)
{
  uint32_t port = gpio / 8;
  uint32_t shift =  (gpio & 0x3) * 4;
  uint32_t saved = (gpio & 0x4) ? GPIO->P[port].CFGH : GPIO->P[port].CFGL;

  return (saved >> shift) & 0xF;
}

void halGpioClear(uint32_t gpio)
{
  uint32_t port = gpio / 8;
  uint32_t pin = gpio & 0x7;

  GPIO->P[port].CLR = BIT(pin);
}

void halGpioSet(uint32_t gpio)
{
  uint32_t port = gpio / 8;
  uint32_t pin = gpio & 0x7;

  GPIO->P[port].SET = BIT(pin);
}

uint32_t halGpioRead(uint32_t gpio)
{
  uint32_t port = gpio / 8;
  uint32_t pin = gpio & 0x7;
  return ((GPIO->P[port].IN) & BIT(pin)) >> pin;
}

uint32_t halGpioReadOutput(uint32_t gpio)
{
  uint32_t port = gpio / 8;
  uint32_t pin = gpio & 0x7;
  return ((GPIO->P[port].OUT) & BIT(pin)) >> pin;
}

//[[ Most of the system-timer functionality is part of the hal-library
//  This functionality is kept public because it depends on configuration
//  options defined in the BOARD_HEADER.  Only for the full HAL, though. In
//  the minimal HAL if the user has to supply the two ifdefs they do so
//  in whatever manner they choose.
//]]
uint16_t halInternalStartSystemTimer(void)
{
  //Since the SleepTMR is the only timer maintained during deep sleep, it is
  //used as the System Timer (RTC).  We maintain a 32 bit hardware timer
  //configured for a tick value time of 1024 ticks/second (0.9765625 ms/tick)
  //using either the 10 kHz internal SlowRC clock divided and calibrated to
  //1024 Hz or the external 32.768 kHz crystal divided to 1024 Hz.
  //With a tick time of ~1ms, this 32bit timer will wrap after ~48.5 days.

  //disable top-level interrupt while configuring
  NVIC_DisableIRQ(SLEEPTMR_IRQn);

  if (useOsc32k > OSC32K_DISABLE) {
    if (useOsc32k == OSC32K_DIGITAL) {
      //Disable both OSC32K and SLOWRC if using external digital clock input
      CMHV->SLEEPTMRCLKEN = 0;
    } else { // OSC32K_CRYSTAL || OSC32K_SINE_1V
      //Enable the 32kHz XTAL (and disable SlowRC since it is not needed)
      CMHV->SLEEPTMRCLKEN = CMHV_SLEEPTMRCLKEN_CLK32KEN;
    }
    //Sleep timer configuration is the same for crystal and external clock
    SLEEPTMR->CFG = ( SLEEPTMR_CFG_ENABLE                  //enable TMR
                    | (0 << _SLEEPTMR_CFG_DBGPAUSE_SHIFT)  //TMR not paused when halted
                    | (5 << _SLEEPTMR_CFG_CLKDIV_SHIFT)    //divide down to 1024Hz
                    | (1 << _SLEEPTMR_CFG_CLKSEL_SHIFT)) ; //select CLK32K external clock
    halCommonDelayMilliseconds(OSC32K_STARTUP_DELAY_MS);
  } else {
    //Enable the SlowRC (and disable 32kHz XTAL since it is not needed)
    CMHV->SLEEPTMRCLKEN = CMHV_SLEEPTMRCLKEN_CLK10KEN;
    SLEEPTMR->CFG = (SLEEPTMR_CFG_ENABLE                   //enable TMR
                    | (0 << _SLEEPTMR_CFG_DBGPAUSE_SHIFT)  //TMR not paused when halted
                    | (0 << _SLEEPTMR_CFG_CLKDIV_SHIFT)    //already 1024Hz
                    | (0 << _SLEEPTMR_CFG_CLKSEL_SHIFT)) ; //select CLK1K internal SlowRC
    #ifndef DISABLE_RC_CALIBRATION
      halInternalCalibrateSlowRc(); //calibrate SlowRC to 1024Hz
    #endif//DISABLE_RC_CALIBRATION
  }

  //clear out any stale interrupts
  EVENT_SLEEPTMR->FLAG = ( EVENT_SLEEPTMR_FLAG_WRAP
                         | EVENT_SLEEPTMR_FLAG_CMPA
                         | EVENT_SLEEPTMR_FLAG_CMPB);
  //turn off second level interrupts.  they will be enabled elsewhere as needed
  EVENT_SLEEPTMR->CFG = _EVENT_SLEEPTMR_CFG_RESETVALUE;
  //enable top-level interrupt
  NVIC_EnableIRQ(SLEEPTMR_IRQn);

  return 0;
}

// These arrays are used by the macros in micro-common.h.
#if defined(SC1) && defined(SC3)
  //Used by SCx_REG(port, reg)
  __IO SC_TypeDef * const halInternalScxReg[] = {
    SC1,
    SC3,
  };
  //Used by EVENT_SCxCFG(port)
  __IO uint32_t * const halInternalEventScxCfg[] = {
    &(((EVENT_SC12_TypeDef *)EVENT_SC1_BASE)->CFG),
    &(((EVENT_SC34_TypeDef *)EVENT_SC3_BASE)->CFG),
  };
  //Used by EVENT_SCxFLAG(port)
  __IO uint32_t * const halInternalEventScxFlag[] = {
    &(((EVENT_SC12_TypeDef *)EVENT_SC1_BASE)->FLAG),
    &(((EVENT_SC34_TypeDef *)EVENT_SC3_BASE)->FLAG),
  };
  //Used by SCx_IRQn(port)
  uint8_t const halInternalEventScxIrqn[] = {
    SC1_IRQn,
    SC3_IRQn,
  };
#endif

#ifdef DISABLE_INTERNAL_1V8_REGULATOR
//Out of reset 1v8 is enabled.  The 1v8 regulator survives deep sleep.
volatile int8_t halCommonVreg1v8EnableCount = 1;

//Disabling the 1V8 regulator saves current when an external 1.8V
//supply is used.
//The 1V8_TEST bit must be set to allow control of the 1V8 regulator
//with the 1V8_EN bit.  Normally (out of reset) the 1V8 regulator is enabled.
void halCommonDisableVreg1v8(void)
{
  ATOMIC(
    assert(halCommonVreg1v8EnableCount > 0);
    if (--halCommonVreg1v8EnableCount == 0) {
      CMHV->VREGCTRL = ( ( CMHV->VREGCTRL
                         & (~(_CMHV_VREGCTRL_1V8EN_MASK
                           | _CMHV_VREGCTRL_1V8TEST_MASK)) )
                       | (0 << _CMHV_VREGCTRL_1V8EN_SHIFT)
                       | (1 << _CMHV_VREGCTRL_1V8TEST_SHIFT) );
    }
  )
}

//The 1V8 regulator is needed for a stable ADC reading of any external signals.
//This includes analog source ADC0 thru ADC5 with the general purpose ADC and
//VDD_PADS/4 with the calibration ADC.
//The 1V8 regulator is not needed for any internal signals.  Such internal
//signals include the calibration ADC for radio calibration.
//Typically the 1V8_EN signal is set in the hardware, but not visible in the
//CMHV->VREGCTRL register.  The 1V8_TEST bit must be set directly control 1V8_EN.
//Clearing 1V8_TEST allows the hardware to return 1V8_EN to its enabled state.
//Specifically also set 1V8_EN (though it won't have affect) to be more
//verbose about the 1V8_EN state.
void halCommonEnableVreg1v8(void)
{
  ATOMIC(
    assert(halCommonVreg1v8EnableCount < 2); // at most 2 concurrent ops: AUXADC and CALADC
    if (++halCommonVreg1v8EnableCount == 1) {
      CMHV->VREGCTRL = ( ( CMHV->VREGCTRL
                         & (~(_CMHV_VREGCTRL_1V8EN_MASK
                           | _CMHV_VREGCTRL_1V8TEST_MASK)) )
                       | (1 << _CMHV_VREGCTRL_1V8EN_SHIFT)
                       | (0 << _CMHV_VREGCTRL_1V8TEST_SHIFT) );
    }
  )
}
#else
void halCommonDisableVreg1v8(void) {}
void halCommonEnableVreg1v8(void) {}
#endif
