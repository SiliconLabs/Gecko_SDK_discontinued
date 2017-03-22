/*
 *  FreeRTOS V7.4.2 - Copyright (C) 2013 Real Time Engineers Ltd.
 *
 *  FEATURES AND PORTS ARE ADDED TO FREERTOS ALL THE TIME.  PLEASE VISIT
 *  http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.
 *
 ***************************************************************************
 *                                                                       *
 *    FreeRTOS tutorial books are available in pdf and paperback.        *
 *    Complete, revised, and edited pdf reference manuals are also       *
 *    available.                                                         *
 *                                                                       *
 *    Purchasing FreeRTOS documentation will not only help you, by       *
 *    ensuring you get running as quickly as possible and with an        *
 *    in-depth knowledge of how to use FreeRTOS, it will also help       *
 *    the FreeRTOS project to continue with its mission of providing     *
 *    professional grade, cross platform, de facto standard solutions    *
 *    for microcontrollers - completely free of charge!                  *
 *                                                                       *
 *    >>> See http://www.FreeRTOS.org/Documentation for details. <<<     *
 *                                                                       *
 *    Thank you for using FreeRTOS, and thank you for your support!      *
 *                                                                       *
 ***************************************************************************
 *
 *
 *  This file is part of the FreeRTOS distribution.
 *
 *  FreeRTOS is free software; you can redistribute it and/or modify it under
 *  the terms of the GNU General Public License (version 2) as published by the
 *  Free Software Foundation AND MODIFIED BY the FreeRTOS exception.
 *
 *  >>>>>>NOTE<<<<<< The modification to the GPL is included to allow you to
 *  distribute a combined work that includes FreeRTOS without being obliged to
 *  provide the source code for proprietary components outside of the FreeRTOS
 *  kernel.
 *
 *  FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 *  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 *  details. You should have received a copy of the GNU General Public License
 *  and the FreeRTOS license exception along with FreeRTOS; if not it can be
 *  viewed here: http://www.freertos.org/a00114.html and also obtained by
 *  writing to Real Time Engineers Ltd., contact details for whom are available
 *  on the FreeRTOS WEB site.
 *
 *  1 tab == 4 spaces!
 *
 ***************************************************************************
 *                                                                       *
 *    Having a problem?  Start by reading the FAQ "My application does   *
 *    not run, what could be wrong?"                                     *
 *                                                                       *
 *    http://www.FreeRTOS.org/FAQHelp.html                               *
 *                                                                       *
 ***************************************************************************
 *
 *
 *  http://www.FreeRTOS.org - Documentation, books, training, latest versions,
 *  license and Real Time Engineers Ltd. contact details.
 *
 *  http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
 *  including FreeRTOS+Trace - an indispensable productivity tool, and our new
 *  fully thread aware and reentrant UDP/IP stack.
 *
 *  http://www.OpenRTOS.com - Real Time Engineers ltd license FreeRTOS to High
 *  Integrity Systems, who sell the code with commercial support,
 *  indemnification and middleware, under the OpenRTOS brand.
 *
 *  http://www.SafeRTOS.com - High Integrity Systems also provide a safety
 *  engineered and independently SIL3 certified version for use in safety and
 *  mission critical applications that require provable dependability.
 */

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Emlib includes. */
#include "em_cmu.h"
#include "em_emu.h"
#include "em_rtc.h"
#include "em_burtc.h"
#include "em_rmu.h"
#include "em_core.h"

/* emdrv includes */
#include "sleep.h"

#if (configUSE_SLEEP_MODE_IN_IDLE == 1)
/**************************************************************************//**
 * @brief vApplicationIdleHook
 * Override the default definition of vApplicationIdleHook()
 *****************************************************************************/
void vApplicationIdleHook(void)
{
    SLEEP_Sleep();
}
#endif

/* Including only if tickless_idle is set to 1 or ( configUSE_TICKLESS_IDLE is set to 0  and  configUSE_SLEEP_MODE_IN_IDLE is set to 1 ) and EM2 or EM3 mode is choosed
 * in other hand standard Cortex M3 FreeRTOS functions are used. */
#if (((configUSE_TICKLESS_IDLE == 1) || ((configUSE_TICKLESS_IDLE == 0) && (configUSE_SLEEP_MODE_IN_IDLE == 1))) && (configSLEEP_MODE == 2 || configSLEEP_MODE == 3))

/*
 * The number of SysTick increments that make up one tick period.
 */
static const unsigned long ulReloadValueForOneTick = configSYSTICK_CLOCK_HZ / configTICK_RATE_HZ;

#if (configUSE_TICKLESS_IDLE == 1)
/*
 * The maximum number of tick periods that can be suppressed is limited by the
 * 24 bit resolution RTC and 32 bit BURTC.
 */
static unsigned long xMaximumPossibleSuppressedTicks = 0;

/*
 * Compensate for the CPU cycles that pass while the SysTick is stopped (low
 * power functionality only.
 */
#define TIMER_COMPENSATION    (45)
static const unsigned long ulStoppedTimerCompensation = TIMER_COMPENSATION / (configCPU_CLOCK_HZ / configSYSTICK_CLOCK_HZ);

#endif /* (configUSE_TICKLESS_IDLE == 1) */

/* Functions which are used in EM2 mode*/
#if (configSLEEP_MODE == 2)
#define TIMER_CAPACITY        (0xFFFFFF)

/**************************************************************************//**
 * @brief RTC_IRQHandler
 * Interrupt Service Routine for RTC which is used as system tick counter in EM2
 *****************************************************************************/
void RTC_IRQHandler(void)
{
  /* Reset the RTC compare value to one system tick period. */
  if (RTC_CompareGet(0) != ulReloadValueForOneTick)
  {
    RTC_Enable(false);
    RTC_CompareSet(0, ulReloadValueForOneTick);
    RTC_Enable(true);
  }

  /* Clear interrupt */
  RTC_IntClear(_RTC_IFC_MASK);

  /* Critical section which protect incrementing the tick*/
  ( void ) portSET_INTERRUPT_MASK_FROM_ISR();
  {
    BaseType_t xSwitchRequired = xTaskIncrementTick();
    portYIELD_FROM_ISR(xSwitchRequired);
  }
  portCLEAR_INTERRUPT_MASK_FROM_ISR(0);
}

/**************************************************************************//**
 * @brief vPortSetupTimerInterrupt
 * Override the default definition of vPortSetupTimerInterrupt() that is weakly
 * defined in the FreeRTOS Cortex-M3, which set source of system tick interrupt
 *****************************************************************************/
void vPortSetupTimerInterrupt(void)
{
  RTC_Init_TypeDef init;

#if (configUSE_TICKLESS_IDLE == 1)
  xMaximumPossibleSuppressedTicks = TIMER_CAPACITY / ulReloadValueForOneTick;
#endif /* (configUSE_TICKLESS_IDLE == 1) */

  /* Enable and select RTC oscillator */
  CMU_ClockEnable(cmuClock_CORELE, true);
#if (configCRYSTAL_IN_EM2 == 1)
  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);
#else
  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFRCO);
#endif
  /* Set 2 times divider to reduce energy*/
  CMU_ClockDivSet(cmuClock_RTC, cmuClkDiv_2);

  /* Enable clock to RTC module */
  CMU_ClockEnable(cmuClock_RTC, true);
  init.enable   = false;
  init.debugRun = false;
  init.comp0Top = true;
  /* Initialization of RTC */
  RTC_Init(&init);

  /* Disable interrupt generation from RTC0 */
  RTC_IntDisable(RTC_IFC_COMP0);

  /* Tick interrupt MUST execute at the lowest interrupt priority. */
  NVIC_SetPriority(RTC_IRQn, 255);

  /* Enable interrupts */
  NVIC_ClearPendingIRQ(RTC_IRQn);
  NVIC_EnableIRQ(RTC_IRQn);
  RTC_CompareSet(0, ulReloadValueForOneTick);
  RTC_IntClear(RTC_IFC_COMP0);
  RTC_IntEnable(RTC_IF_COMP0);
  RTC_Enable(true);
}

#if (configUSE_TICKLESS_IDLE == 1)
/**************************************************************************//**
 * @brief vPortSuppressTicksAndSleep
 * Override the default definition of vPortSuppressTicksAndSleep() that is weakly
 * defined in the FreeRTOS Cortex-M3 port layer layer
 *****************************************************************************/
void vPortSuppressTicksAndSleep(portTickType xExpectedIdleTime)
{
  unsigned long ulCountBeforeSleep, ulCountAfterSleep;
  unsigned long ulReloadValue, ulCompleteTickPeriods;
  portTickType  xModifiableIdleTime;
  bool rtcFlag;
  CORE_DECLARE_IRQ_STATE;

  /* Make sure the SysTick reload value does not overflow the counter. */
  if (xExpectedIdleTime > xMaximumPossibleSuppressedTicks)
  {
    xExpectedIdleTime = xMaximumPossibleSuppressedTicks;
  }

  /* Calculate the reload value required to wait xExpectedIdleTime
   * tick periods. */
  ulReloadValue = ulReloadValueForOneTick * xExpectedIdleTime;
  if (ulReloadValue > ulStoppedTimerCompensation)
  {
    /* Compensate for the fact that the RTC is going to be stopped
     * momentarily. */
    ulReloadValue -= ulStoppedTimerCompensation;
  }

  /* Stop the RTC momentarily.  The time the RTC is stopped for is accounted
   * for as best it can be, but using the tickless mode will inevitably result
   * in some tiny drift of the time maintained by the kernel with respect to
   * calendar time.  The count is latched before stopping the timer as stopping
   * the timer appears to clear the count. */
  ulCountBeforeSleep = RTC_CounterGet();
  RTC_Enable(false);

  /* If this function is re-entered before one complete tick period then the
   * reload value might be set to take into account a partial time slice. So
   * we adjust the count before sleep to take this into account.
   *
   * Just reading the count would be wrong, as it assumes that we are
   * counting up to a full ticks worth.
   */
  ulCountBeforeSleep += (ulReloadValueForOneTick - RTC_CompareGet(0));

  /* Enter a critical section but don't use the taskENTER_CRITICAL()
   * method as that will mask interrupts that should exit sleep mode. */
  CORE_ENTER_CRITICAL();

  /* If a context switch is pending or a task is waiting for the scheduler
   * to be unsuspended then abandon the low power entry. */
  if (eTaskConfirmSleepModeStatus() == eAbortSleep)
  {
    /* Restart tick and count up to whatever was left of the current time
     * slice. */
    RTC_CompareSet(0, (ulReloadValueForOneTick - ulCountBeforeSleep));
    RTC_Enable(true);

    /* Re-enable interrupts */
    CORE_EXIT_CRITICAL();
  }
  else
  {
    /* Adjust the reload value to take into account that the current time
     * slice is already partially complete. */
    ulReloadValue -= ulCountBeforeSleep;
    RTC_CompareSet(0, ulReloadValue);

    /* Restart the counter */
    RTC_Enable(true);

    /* Sleep until something happens.  configPRE_SLEEP_PROCESSING() can
     * set its parameter to 0 to indicate that its implementation contains
     * its own wait for interrupt or wait for event instruction, and so wfi
     * should not be executed again.  However, the original expected idle
     * time variable must remain unmodified, so a copy is taken. */
    xModifiableIdleTime = xExpectedIdleTime;
    configPRE_SLEEP_PROCESSING(xModifiableIdleTime);
    if (xModifiableIdleTime > 0)
    {
      SLEEP_Sleep();
      __DSB();
      __ISB();
    }
    configPOST_SLEEP_PROCESSING(xExpectedIdleTime);

    /* Stop the RTC and save the counter value since stopping the RTC clears
     * the counter. Again, the time the SysTick is stopped for is
     * accounted for as best it can be, but using the tickless mode will
     * inevitably result in some tiny drift of the time maintained by the
     * kernel with respect to calendar time. */
    ulCountAfterSleep = RTC_CounterGet();
    RTC_Enable(false);
    rtcFlag = (RTC_IntGet() & RTC_IF_COMP0) != 0;

    /* Re-enable interrupts */
    CORE_EXIT_CRITICAL();

    if (rtcFlag)
    {
      /* The tick interrupt handler will already have pended the tick
       * processing in the kernel.  As the pending tick will be
       * processed as soon as this function exits, the tick value
       * maintained by the tick is stepped forward by one less than the
       * time spent waiting. */
      ulCompleteTickPeriods = xExpectedIdleTime - 1UL;
    }
    else
    {
      /* Some other interrupt than the RTC ended the sleep, now we need to
       * calculate how much time has passed since the last tick. */
      ulCountAfterSleep += ulCountBeforeSleep;

      /* Calculate how many complete ticks have passed since the last tick */
      ulCompleteTickPeriods = ulCountAfterSleep / ulReloadValueForOneTick;

      /* The reload value is set to whatever fraction of a single tick
       * period remains. */
      ulCountAfterSleep -= (ulCompleteTickPeriods * ulReloadValueForOneTick);
      ulReloadValue = ulReloadValueForOneTick - ulCountAfterSleep;

      /* Reset the RTC compare value to trigger at the configured tick rate */
      RTC_CompareSet(0, ulReloadValue);
    }

    /* Start the counter */
    RTC_Enable(true);

    /* The tick forward by the number of tick periods that
     * remained in a low power state. */
    vTaskStepTick(ulCompleteTickPeriods);
  }
}
#endif /* (configUSE_TICKLESS_IDLE == 1) */
#endif /* (configSLEEP_MODE == 2) */

/* Functions which are used in EM3 mode*/
#if (configSLEEP_MODE == 3)

#define TIMER_CAPACITY        (0xFFFFFFFF)

/**************************************************************************//**
 * @brief BURTC_IRQHandler
 * Interrupt Service Routine for RTC which is used as system tick counter in EM3
 *****************************************************************************/
void BURTC_IRQHandler(void)
{
  /* Reset the BURTC compare value to one system tick period. */
  if (BURTC_CompareGet(0) != ulReloadValueForOneTick)
  {
    BURTC_Enable(false);
    BURTC_CompareSet(0, ulReloadValueForOneTick);
    BURTC_Enable(true);
  }

  /* Clear interrupt */
  BURTC_IntClear(_BURTC_IFC_MASK);

  /* Critical section which protect incrementing the tick*/
  ( void ) portSET_INTERRUPT_MASK_FROM_ISR();
  {
    BaseType_t xSwitchRequired = xTaskIncrementTick();
    portYIELD_FROM_ISR(xSwitchRequired);
  }
  portCLEAR_INTERRUPT_MASK_FROM_ISR(0);
}

/**************************************************************************//**
 * @brief vPortSetupTimerInterrupt
 * Override the default definition of vPortSetupTimerInterrupt() that is weakly
 * defined in the FreeRTOS Cortex-M3, which set source of system tick interrupt
 *****************************************************************************/
void vPortSetupTimerInterrupt(void)
{
  BURTC_Init_TypeDef burtcInit = BURTC_INIT_DEFAULT;

#if (configUSE_TICKLESS_IDLE == 1)
  xMaximumPossibleSuppressedTicks = TIMER_CAPACITY / ulReloadValueForOneTick;
#endif /* (configUSE_TICKLESS_IDLE == 1) */
  /* Ensure LE modules are accessible */
  CMU_ClockEnable(cmuClock_CORELE, true);

  /* Enable access to BURTC registers */
  RMU_ResetControl(rmuResetBU, rmuResetModeClear);

  /* Configure BURTC as system tick source */
  burtcInit.mode        = burtcModeEM3;      /* BURTC is enabled to EM3 */
  burtcInit.clkSel      = burtcClkSelULFRCO; /* Select ULFRCO as clock source */
  burtcInit.clkDiv      = burtcClkDiv_1;     /* Choose 2kHz ULFRCO clock frequency */
  burtcInit.compare0Top = true;              /* Wrap on COMP0. */
  /* Initialization of BURTC */
  BURTC_Init(&burtcInit);

  /* Disable interrupt generation from BURTC */
  BURTC_IntDisable(BURTC_IF_COMP0);

  /* Tick interrupt MUST execute at the lowest interrupt priority. */
  NVIC_SetPriority(BURTC_IRQn, 255);

  /* Enable interrupts */
  NVIC_ClearPendingIRQ(BURTC_IRQn);
  NVIC_EnableIRQ(BURTC_IRQn);
  BURTC_CompareSet(0, ulReloadValueForOneTick);
  BURTC_IntClear(BURTC_IF_COMP0);
  BURTC_IntEnable(BURTC_IF_COMP0);
  BURTC_CounterReset();
}

#if (configUSE_TICKLESS_IDLE == 1)
/**************************************************************************//**
 * @brief vPortSetupTimerInterrupt
 * Override the default definition of vPortSuppressTicksAndSleep() that is weakly
 * defined in the FreeRTOS Cortex-M3 port layer layer
 *****************************************************************************/
void vPortSuppressTicksAndSleep(portTickType xExpectedIdleTime)
{
  unsigned long ulCountBeforeSleep, ulCountAfterSleep;
  unsigned long ulReloadValue, ulCompleteTickPeriods;
  portTickType  xModifiableIdleTime;
  bool burtcFlag;
  CORE_DECLARE_IRQ_STATE;

  /* Make sure the SysTick reload value does not overflow the counter. */
  if (xExpectedIdleTime > xMaximumPossibleSuppressedTicks)
  {
    xExpectedIdleTime = xMaximumPossibleSuppressedTicks;
  }

  /* Calculate the reload value required to wait xExpectedIdleTime
   * tick periods. */
  ulReloadValue = ulReloadValueForOneTick * xExpectedIdleTime;
  if (ulReloadValue > ulStoppedTimerCompensation)
  {
    ulReloadValue -= ulStoppedTimerCompensation;
  }

  /* Stop the BURTC momentarily.  The time the BURTC is stopped for is accounted
   * for as best it can be, but using the tickless mode will inevitably result
   * in some tiny drift of the time maintained by the kernel with respect to
   * calendar time.  The count is latched before stopping the timer as stopping
   * the timer appears to clear the count. */
  ulCountBeforeSleep = BURTC_CounterGet();
  BURTC_Enable(false);

  /* If this function is re-entered before one complete tick period then the
   * reload value might be set to take into account a partial time slice, but
   * just reading the count assumes it is counting up to a full ticks worth - so
   * add in the difference if any. */
  ulCountBeforeSleep += (ulReloadValueForOneTick - BURTC_CompareGet(0));

  /* Enter a critical section but don't use the taskENTER_CRITICAL()
   * method as that will mask interrupts that should exit sleep mode. */
  CORE_ENTER_CRITICAL();

  /* If a context switch is pending or a task is waiting for the scheduler
   * to be unsuspended then abandon the low power entry. */
  if (eTaskConfirmSleepModeStatus() == eAbortSleep)
  {
    /* Restart tick and count up to whatever was left of the current time
     * slice. */
    BURTC_CompareSet(0, (ulReloadValueForOneTick - ulCountBeforeSleep));
    BURTC_Enable(true);

    /* Re-enable interrupts */
    CORE_EXIT_CRITICAL();
  }
  else
  {
    /* Adjust the reload value to take into account that the current time
     * slice is already partially complete. */
    ulReloadValue -= ulCountBeforeSleep;
    BURTC_CompareSet(0, ulReloadValue);

    /* Restart the counter*/
    BURTC_Enable(true);

    /* Sleep until something happens.  configPRE_SLEEP_PROCESSING() can
     * set its parameter to 0 to indicate that its implementation contains
     * its own wait for interrupt or wait for event instruction, and so wfi
     * should not be executed again.  However, the original expected idle
     * time variable must remain unmodified, so a copy is taken. */
    xModifiableIdleTime = xExpectedIdleTime;
    configPRE_SLEEP_PROCESSING(xModifiableIdleTime);
    if (xModifiableIdleTime > 0)
    {
      SLEEP_Sleep();
      __DSB();
      __ISB();
    }
    configPOST_SLEEP_PROCESSING(xExpectedIdleTime);

    /* Stop the BURTC and save the counter value since stopping the BURTC
     * clears the counter. Again, the time the SysTick is stopped for is
     * accounted for as best it can be, but using the tickless mode will
     * inevitably result in some tiny drift of the time maintained by the
     * kernel with respect to calendar time. */
    ulCountAfterSleep = BURTC_CounterGet();
    BURTC_Enable(false);
    burtcFlag = (BURTC_IntGet() & BURTC_IF_COMP0) != 0;

    /* Re-enable interrupts. */
    CORE_EXIT_CRITICAL();

    if (burtcFlag)
    {
      /* The tick interrupt handler will already have pended the tick
       * processing in the kernel.  As the pending tick will be
       * processed as soon as this function exits, the tick value
       * maintained by the tick is stepped forward by one less than the
       * time spent waiting. */
      ulCompleteTickPeriods = xExpectedIdleTime - 1UL;
    }
    else
    {
      /* Some other interrupt than the BURTC ended the sleep, now we need to
       * calculate how much time has passed since the last tick. */
      ulCountAfterSleep += ulCountBeforeSleep;

      /* Calculate how many complete ticks have passed since the last tick */
      ulCompleteTickPeriods = ulCountAfterSleep / ulReloadValueForOneTick;

      /* The reload value is set to whatever fraction of a single tick
       * period remains. */
      ulCountAfterSleep -= (ulCompleteTickPeriods * ulReloadValueForOneTick);
      ulReloadValue = ulReloadValueForOneTick - ulCountAfterSleep;

      BURTC_CompareSet(0, ulReloadValue);
    }
    /* Start the BURTC again. */
    BURTC_Enable(true);

    /* The tick forward by the number of tick periods that
     * remained in a low power state. */
    vTaskStepTick(ulCompleteTickPeriods);
  }
}
#endif /* (configUSE_TICKLESS_IDLE == 1) */
#endif /* (configSLEEP_MODE == 3) */
#endif /* (((configUSE_TICKLESS_IDLE == 1) ||  (( configUSE_TICKLESS_IDLE == 0 ) && ( configUSE_SLEEP_MODE_IN_IDLE == 1 ))) && (configSLEEP_MODE == 2 || configSLEEP_MODE == 3)) */
