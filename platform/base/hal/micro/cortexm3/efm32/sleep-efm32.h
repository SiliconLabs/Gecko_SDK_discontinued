/** @file hal/micro/cortexm3/efm32/sleep-efm32.h
 *
 * @brief EFM32/EFR32 specific sleep functions.
 *
 * <!-- Copyright 2016 Silicon Laboratories, Inc.                       *80* -->
 */

#ifndef __SLEEP_EFM32_H__
#define __SLEEP_EFM32_H__

typedef struct
{
  uint32_t gpioFlags;
} Em4WakeupCause_t;

/**
 * @brief This function should be called when waking up from EM4 to classify
 * the wakeup reason and also to clear the wakeup flags so that application
 * is able to enter EM4 again.
 */
void halInternalEm4Wakeup(void);

/**
 * @brief This function can be used to figure out why the device was woken up
 * from EM4.
 *
 * @return a pointer to the recorded wakeup cause
 */
const Em4WakeupCause_t * halInternalEm4WakeupCause(void);

#endif
