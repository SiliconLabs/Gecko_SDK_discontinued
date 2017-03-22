// -----------------------------------------------------------------------------
// @file
// @brief EFM micro specific HAL functions common to full and minimal hal
//
// @author Silicon Laboratories Inc.
// @version 1.0.0
//
// @section License
// <b>(C) Copyright 2014 Silicon Laboratories, http://www.silabs.com</b>
//
// This file is licensed under the Silabs License Agreement. See the file
// "Silabs_License_Agreement.txt" for details. Before using this software for
// any purpose, you must agree to the terms of that agreement.
//
// -----------------------------------------------------------------------------

#include PLATFORM_HEADER
#include "em_device.h"
#include "em_wdog.h"
#include "em_cmu.h"
#include "em_rmu.h"
#include "em_gpio.h"

#include "include/error.h"
#include "hal/micro/micro-common.h"
#include "hal/micro/cortexm3/efm32/micro-common.h"
#include "hal/plugin/adc/adc.h"
#include "ustimer.h"

#if defined(WDOG0)
#define WDOG WDOG0
#if (WDOG_COUNT > 1)
#warning "Multiple watchdogs not supported"
#endif
#endif

#if defined(BOARD_HEADER) && !defined(MINIMAL_HAL)
  #include BOARD_HEADER
#endif //defined(BOARD_HEADER)

// -----------------------------------------------------------------------------
// Watchdog Functions
#if defined(WDOG_IF_WARN) && !defined(BOOTLOADER)
void WDOG0_IRQHandler(void)
{
  assert(!(WDOG->IF & WDOG_IF_WARN));
}
#endif

void halInternalEnableWatchDog( void )
{
  /* Enable LE interface */
  CMU_ClockEnable(cmuClock_CORELE, true);

  /* Make sure FULL reset is used on WDOG timeout */
#if defined(_RMU_CTRL_WDOGRMODE_MASK)
  RMU_ResetControl(rmuResetWdog, rmuResetModeFull);
#endif

  /* EMLIB WDOG is missing some support for WARNSEL and interrupt API. The code
     below should be updated once this support is added to EMLIB. */

  /* Set PERSEL to 2k ticks,
     Use ULFRCO and set
     WARN interrupt to 75% of timeout. */
  WDOG->CTRL = WDOG_CTRL_EN
#if defined(_WDOG_CTRL_WARNSEL_SHIFT)
               | (3 << _WDOG_CTRL_WARNSEL_SHIFT)
#endif
               | (8 << _WDOG_CTRL_PERSEL_SHIFT)
               | WDOG_CTRL_CLKSEL_ULFRCO;

  /* Enable WARN interrupt. */
#if defined(WDOG_IF_WARN) && !defined(BOOTLOADER)
  NVIC_ClearPendingIRQ(WDOG0_IRQn);
  WDOG->IFC = WDOG_IFC_WARN;
  NVIC_EnableIRQ(WDOG0_IRQn);
  WDOG->IEN |= WDOG_IEN_WARN;
#endif
}

void halInternalResetWatchDog( void )
{
  WDOGn_Feed(WDOG);
}

void halInternalDisableWatchDog( uint8_t magicKey )
{
  if ( magicKey == MICRO_DISABLE_WATCH_DOG_KEY )
  {
    /* The watchdog will hang if you attempt to disable it when already disabled.
       Clear the EN bit without waiting for SYNCBUSY as we cannot afford the 1ms wait
       here if ULFRCO is selected. Assume there is not following write to WDOG->CTRL
       within the busy period. If other bits in WDOG->CTRL are corrupted by an early
       write, then halInternalEnableWatchDog() is always doing a full init. */
    if (halInternalWatchDogEnabled())
    {
      BUS_RegBitWrite(&WDOG->CTRL, _WDOG_CTRL_EN_SHIFT, 0);
    }
  }
}

bool halInternalWatchDogEnabled( void )
{
  if ( WDOG->CTRL & WDOG_CTRL_EN )
  {
    return true;
  }
  else
  {
    return false;
  }
}

//------------------------------------------------------------------------------
// GPIO Functions

void halGpioSetConfig(uint32_t gpio, HalGpioCfg_t config)
{
  GPIO_PinModeSet(GPIO_PORT(gpio), GPIO_PIN(gpio),
                  (GPIO_Mode_TypeDef)config, halGpioReadOutput(gpio));
}

HalGpioCfg_t halGpioGetConfig(uint32_t gpio)
{
  // Unfortunately, there is no emlib API for this
  uint8_t port = GPIO_PORT(gpio);
  uint8_t pin  = GPIO_PIN(gpio);
  assert(GPIO_PORT_PIN_VALID(port, pin));
  return (HalGpioCfg_t)
          ((BUS_RegMaskedRead((pin < 8) ? &GPIO->P[port].MODEL
                                        : &GPIO->P[port].MODEH,
                              0xF << ((pin % 8) * 4))) >> ((pin % 8) * 4));
}

void halGpioClear(uint32_t gpio)
{
  GPIO_PinOutClear(GPIO_PORT(gpio), GPIO_PIN(gpio));
}

void halGpioSet(uint32_t gpio)
{
  GPIO_PinOutSet(GPIO_PORT(gpio), GPIO_PIN(gpio));
}

uint32_t halGpioRead(uint32_t gpio)
{
  return GPIO_PinInGet(GPIO_PORT(gpio), GPIO_PIN(gpio));
}

uint32_t halGpioReadOutput(uint32_t gpio)
{
  return GPIO_PinOutGet(GPIO_PORT(gpio), GPIO_PIN(gpio));
}
