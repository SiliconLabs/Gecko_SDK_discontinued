/** @file hal/micro/cortexm3/efm32/led.c
 *  @brief LED manipulation routines; stack and example APIs
 *
 * <!-- Author(s): Brooks Barrett -->
 * <!-- Copyright 2007 by Ember Corporation. All rights reserved.       *80*-->
 */

#include PLATFORM_HEADER
#if !defined(MINIMAL_HAL) && defined(BOARD_HEADER)
  // full hal needs the board header to get pulled in
  #include "hal/micro/micro.h"
  #include BOARD_HEADER
#endif
#include "hal/micro/led.h"
#include "em_cmu.h"

// Stub out LED functions when there aren't any LEDs
#if BSP_NO_OF_LEDS == 0
  void halInternalInitLed(void)
  {
    /* no-op */
  }
  void halSetLed(HalBoardLed led)
  {
    /* no-op */
  }
  void halClearLed(HalBoardLed led)
  {
    /* no-op */
  }
  void halToggleLed(HalBoardLed led)
  {
    /* no-op */
  }
#else
  typedef struct
  {
    GPIO_Port_TypeDef   port;
    unsigned int        pin;
  } tLedArray;
  static const tLedArray ledArray[ BSP_NO_OF_LEDS ] = BSP_GPIO_LEDARRAY_INIT;

  // HWCONF will intialize LED GPIOs
  #ifndef EMBER_AF_USE_HW_CONF
  void halInternalInitLed(void)
  {
    int i;

    CMU_ClockEnable(cmuClock_HFPER, true);
    CMU_ClockEnable(cmuClock_GPIO, true);
    for ( i=0; i<BSP_NO_OF_LEDS; i++ )
    {
      GPIO_PinModeSet(ledArray[i].port, ledArray[i].pin, gpioModePushPull, 0);
    }
  }
  #endif

  void halSetLed(HalBoardLed led)
  {
    GPIO_PinOutSet(ledArray[led].port, ledArray[led].pin);
  }

  void halClearLed(HalBoardLed led)
  {
    GPIO_PinOutClear(ledArray[led].port, ledArray[led].pin);
  }

  void halToggleLed(HalBoardLed led)
  {
    GPIO_PinOutToggle(ledArray[led].port, ledArray[led].pin);
  }
#endif

#ifndef MINIMAL_HAL
void halStackIndicateActivity(bool turnOn)
{
 #if     NO_LED
  // Don't touch LEDs when built with NO_LED
 #else//!NO_LED
  if(turnOn) {
    halSetLed(BOARD_ACTIVITY_LED);
  } else {
    halClearLed(BOARD_ACTIVITY_LED);
  }
 #endif//NO_LED
}
#endif //MINIMAL_HAL
