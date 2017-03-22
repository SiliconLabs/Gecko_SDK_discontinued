/** @file hal/micro/cortexm3/led.c
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

#define GPIO_PxCLR_BASE (GPIO_PACLR_ADDR)
#define GPIO_PxSET_BASE (GPIO_PASET_ADDR)
#define GPIO_PxOUT_BASE (GPIO_PAOUT_ADDR)
// Each port is offset from the previous port by the same amount
#define GPIO_Px_OFFSET  (GPIO_PBCFGL_ADDR-GPIO_PACFGL_ADDR)

#ifdef LED_ACTIVE_HIGH
#define GPIO_BASE_SET_LED   GPIO_PxSET_BASE
#define GPIO_BASE_CLEAR_LED GPIO_PxCLR_BASE
#else
#define GPIO_BASE_SET_LED   GPIO_PxCLR_BASE
#define GPIO_BASE_CLEAR_LED GPIO_PxSET_BASE      
#endif

void halSetLed(HalBoardLed led)
{
#if (!defined(MINIMAL_HAL) && !defined(BOOTLOADER) && defined(RHO_GPIO))
  if(led == RHO_GPIO && halGetRadioHoldOff()) { // Avoid Radio HoldOff conflict
    return;
  }
#endif
  if(led/8 < 3) {
    *((volatile uint32_t *)(GPIO_BASE_SET_LED+(GPIO_Px_OFFSET*(led/8)))) 
      = BIT(led&7);
  }
}

void halClearLed(HalBoardLed led)
{
#if (!defined(MINIMAL_HAL) && !defined(BOOTLOADER) && defined(RHO_GPIO))
  if(led == RHO_GPIO && halGetRadioHoldOff()) { // Avoid Radio HoldOff conflict
    return;
  }
#endif
  if(led/8 < 3) {
    *((volatile uint32_t *)(GPIO_BASE_CLEAR_LED+(GPIO_Px_OFFSET*(led/8)))) 
      = BIT(led&7);
  }
}

void halToggleLed(HalBoardLed led)
{
#if (!defined(MINIMAL_HAL) && !defined(BOOTLOADER) && defined(RHO_GPIO))
  if(led == RHO_GPIO && halGetRadioHoldOff()) { // Avoid Radio HoldOff conflict
    return;
  }
#endif
  //to avoid contention with other code using the other pins for other
  //purposes, we disable interrupts since this is a read-modify-write
  ATOMIC(
    if(led/8 < 3) {
      *((volatile uint32_t *)(GPIO_PxOUT_BASE+(GPIO_Px_OFFSET*(led/8)))) ^= BIT(led&7);
    }
  )
}

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
