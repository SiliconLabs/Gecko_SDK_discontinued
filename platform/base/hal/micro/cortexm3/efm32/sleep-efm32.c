/** @file hal/micro/cortexm3/efm32/sleep-efm32.c
 *
 * @brief EFM32/EFR32 specific sleep functions.
 *
 * <!-- Copyright 2016 Silicon Laboratories, Inc.                       *80* -->
 */

#include PLATFORM_HEADER
#include "sleep-efm32.h"
#include "hal/micro/micro.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "serial/com.h"

//[[
#ifdef SLEEP_TRACE //WBB350FIXME -- Find a less intrusive technique
  extern bool sleepTraceOn;
  extern uint8_t sleepTraceBuffer[];
  extern uint8_t sleepTraceIndex;
  extern uint8_t sleepTraceDelayPosition;
  #define SLEEP_TRACE_ADD_MARKER(byte)                 \
    do {                                               \
      if(sleepTraceOn) {                               \
        if(sleepTraceIndex<50) {                       \
          sleepTraceBuffer[sleepTraceIndex] = byte;    \
        }                                              \
        sleepTraceIndex++;                             \
      }                                                \
    } while(0)
  #define SLEEP_TRACE_1SEC_DELAY(position)             \
    do {                                               \
      if(sleepTraceDelayPosition==position) {          \
        uint8_t delayCnt=(20*1);                       \
        while(delayCnt-->0) {                          \
          halCommonDelayMicroseconds(50000);           \
        }                                              \
      }                                                \
    } while(0)
#else //SLEEP_TRACE
  #define SLEEP_TRACE_ADD_MARKER(byte) do{}while(0)
  #define SLEEP_TRACE_1SEC_DELAY(position) do{}while(0)
#endif //SLEEP_TRACE
//]]

#if defined( RTCC_PRESENT ) && ( RTCC_COUNT == 1 )
#define SYSTIMER_IRQ_N   RTCC_IRQn
#else
#define SYSTIMER_IRQ_N   RTC_IRQn
#endif

static WakeEvents wakeInfo;
static Em4WakeupCause_t em4WakeupCause;

WakeEvents halGetWakeInfo(void)
{
  return wakeInfo;
}

const Em4WakeupCause_t * halInternalEm4WakeupCause(void)
{
  return &em4WakeupCause;
}

void halInternalEm4Wakeup(void)
{
  uint32_t gpioEm4Flags = 0;

#if defined(_GPIO_IF_EM4WU_MASK)
  gpioEm4Flags = GPIO_IntGet() & _GPIO_IF_EM4WU_MASK;
  GPIO_IntClear(gpioEm4Flags);
#elif defined(_GPIO_EM4WUCAUSE_MASK)
  gpioEm4Flags = GPIO->EM4WUCAUSE & _GPIO_EM4WUCAUSE_MASK;
#endif

  em4WakeupCause.gpioFlags = gpioEm4Flags;
}

/**
 * @brief Get Enable Interrupt
 *
 * @details Read the enable register in the NVIC and returns if the enable bit
 * is set for the specific interrupt.
 *
 * @param [in] IRQn Interrupt number
 * @return        1 Interrupt is enabled
 *                0 Interrupt is disabled
 */
__STATIC_INLINE bool NVIC_GetEnableIRQ(IRQn_Type IRQn)
{
  return ((NVIC->ISER[(((uint32_t)(int32_t)IRQn) >> 5UL)] & (1UL << (((uint32_t)(int32_t)IRQn) & 0x1FUL))) != 0UL);
}

/**
 * @brief Check if an interrupt is triggered
 *
 * @details This function checks if an interrupt is enabled and pending
 *
 * @param  [in] IRQn Interrupt number
 *
 * @return true   Interrupt is triggered
 *         false  Interrupt is not triggered
 */
static bool irqTriggered(IRQn_Type IRQn)
{
  return NVIC_GetPendingIRQ(IRQn) && NVIC_GetEnableIRQ(IRQn);
}

void halInternalSleep(SleepModes sleepMode)
{
  bool watchdogDisableInSleep;

  // SLEEPMODE_POWERDOWN and SLEEPMODE_POWERSAVE are deprecated.  Remap them
  // to their appropriate, new mode name.
  if (sleepMode == SLEEPMODE_POWERDOWN) {
    sleepMode = SLEEPMODE_MAINTAINTIMER;
  } else if (sleepMode == SLEEPMODE_POWERSAVE) {
    sleepMode = SLEEPMODE_WAKETIMER;
  }

  // Disable and restore watchdog if already on and going for EM1 sleep,
  // since we can't clear it asleep in EM1. The watchdog is frozen in
  // EM2 and 3 and disabling it while in EM2 and 3 sleep is not needed.
  watchdogDisableInSleep = halInternalWatchDogEnabled()
                           && (sleepMode == SLEEPMODE_IDLE);

  if (watchdogDisableInSleep)
  {
    halInternalDisableWatchDog(MICRO_DISABLE_WATCH_DOG_KEY);
  }
  //[[
  SLEEP_TRACE_ADD_MARKER('A');
  //]]

  // Globally disable interrupts with PRIMASK. Note that this will still
  // allow all interrupts to wakeup the MCU.
  CORE_CRITICAL_IRQ_DISABLE();

#if (__CORTEX_M >= 3)
  // Bring the BASEPRI up to 0 to allow all interrupts to wakeup the MCU.
  __set_BASEPRI(0);
#endif

  COM_InternalPowerDown();
  wakeInfo = 0;

#ifdef BOOTLOADER_OPEN
  #error no bootloader support yet
#endif

  switch(sleepMode)
  {
    case SLEEPMODE_IDLE:
      //[[
      SLEEP_TRACE_ADD_MARKER('B');
      //]]
      EMU_EnterEM1();
      break;
    // there is no difference between wake/maintain timer
    case SLEEPMODE_WAKETIMER:
    case SLEEPMODE_MAINTAINTIMER:
      //[[
      SLEEP_TRACE_ADD_MARKER('C');
      //]]
      EMU_EnterEM2(true);
      break;
    case SLEEPMODE_NOTIMER:
      //[[
      SLEEP_TRACE_ADD_MARKER('E');
      //]]
      EMU_EnterEM3(true);
      break;
    case SLEEPMODE_HIBERNATE:
      EMU_EnterEM4();
      break;
    default:
      //Oops!  Invalid sleepMode parameter.
      assert(0);
      break;
  }
  //[[
  SLEEP_TRACE_ADD_MARKER('F');
  //]]

  wakeInfo = GPIO_IntGetEnabled() & _GPIO_IF_EXT_MASK;
  if (irqTriggered(GPIO_EVEN_IRQn) || irqTriggered(GPIO_ODD_IRQn))
  {
    wakeInfo |= WAKE_IRQ_GPIO;
  }
  if (irqTriggered(SYSTIMER_IRQ_N))
  {
    wakeInfo |= WAKE_IRQ_SYSTIMER;
  }
#if defined(RFSENSE_PRESENT) && (RFSENSE_COUNT == 1)
  if (irqTriggered(RFSENSE_IRQn))
  {
    wakeInfo |= WAKE_IRQ_RFSENSE;
  }
#endif

  // restart watchdog if it was running when we entered sleep
  // do this before dispatching interrupts while we still have tight
  // control of code execution
  if (watchdogDisableInSleep) {
    halInternalEnableWatchDog();
  }

  COM_InternalPowerUp();

  // Clear PRIMASK to enable all interrupts. Note that at after this
  // point BASEPRI=0 and PRIMASK=0 which means that all interrupts
  // are enabled. The interrupt state is not saved/restored due to
  // API restrictions.
  CORE_CRITICAL_IRQ_ENABLE();

  //[[
  SLEEP_TRACE_ADD_MARKER('G');
  //]]
}

void halSleepWithOptions(SleepModes sleepMode, WakeMask wakeMask)
{
  uint32_t gpioIen = GPIO->IEN & _GPIO_IEN_EXT_MASK;
  GPIO->IEN = (GPIO->IEN & ~(_GPIO_IEN_EXT_MASK))
              | (wakeMask & _GPIO_IEN_EXT_MASK);
  halInternalSleep(sleepMode);
  GPIO->IEN = (GPIO->IEN & ~(_GPIO_IEN_EXT_MASK))
              | (gpioIen & _GPIO_IEN_EXT_MASK);
}

void halSleep(SleepModes sleepMode)
{
  halInternalSleep(sleepMode);
}
