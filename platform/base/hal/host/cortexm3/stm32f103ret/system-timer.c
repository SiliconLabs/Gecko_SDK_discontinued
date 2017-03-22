/** @file hal/host/cortexm3/stm32f103ret/system-timer.c
 * @brief Functions that provide access to the system timer.
 *
 * The system timer uses the RTC clock which is a 32bit hardware counter
 * and is configured for ~1ms ticks.
 *
 * Typically, the RTC is sourced from the low speed internal RC clock.
 * Enabling the LSI takes a few milliseconds and produces a 1.015ms tick.
 *
 * Defining ENABLE_OSC32K causes the RTC to be sourced from the low speed
 * external XTAL clock which is the 32kHz XTAL.  Enabling the XTAL takes
 * about 665ms and produces a 1.0046ms tick.
 *
 * <!-- Copyright 2010 by Ember Corporation. All rights reserved.        *80*-->
 */

#include PLATFORM_HEADER
#include "stack/include/error.h"
#include "hal/hal.h"


#ifdef ENABLE_OSC32K
static void initRtc(void)
{
  //Enable Low Speed External (LSE) xtal clock
  RCC_LSEConfig(RCC_LSE_ON);
  
  //Wait until the LSE is ready.  This takes about 665ms!
  while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET) {}

  //Select LSE as RTC Clock Source
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

  //Enable RTC Clock
  RCC_RTCCLKCmd(ENABLE);

  //Wait for RTC registers to synchronize
  RTC_WaitForSynchro();

  //Wait until last write operation to RTC registers has finished
  RTC_WaitForLastTask();

  //Set RTC prescaler for a 1ms tick
  RTC_SetPrescaler(32);

  //Wait until last write operation to RTC registers has finished
  RTC_WaitForLastTask();
  
  //RTC should now be producing a 1.0046ms tick
}
#else //!ENABLE_OSC32K
static void initRtc(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_ICInitTypeDef  TIM_ICInitStructure;
  RCC_ClocksTypeDef RCC_Clocks;
  uint16_t lsiCapture1;
  uint16_t lsiCapture2;
  uint32_t lsiPeriod;
  uint32_t lsiFreq;

  //Enable Low Speed Internal (LSI) RC clock
  RCC_LSICmd(ENABLE);
  
  //Wait until the LSI is ready
  while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET) {}

  //Select LSI as RTC Clock Source
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);

  //Enable RTC Clock
  RCC_RTCCLKCmd(ENABLE);

  //Wait for RTC registers to synchronize
  RTC_WaitForSynchro();

  //Wait until last write operation to RTC registers has finished
  RTC_WaitForLastTask();
  
  //Get PCLK1 frequency
  RCC_GetClocksFreq(&RCC_Clocks);

  //Connect internally the TM5_CH4 Input Capture to the LSI clock output
  GPIO_PinRemapConfig(GPIO_Remap_TIM5CH4_LSI, ENABLE);

  //TIM5 Time base configuration
  TIM_TimeBaseStructure.TIM_Prescaler = 0;                      
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Period = 0xFFFF;
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);

  //TIM5 Channel4 Input capture Mode configuration
  TIM_ICInitStructure.TIM_Channel = TIM_Channel_4;
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  TIM_ICInitStructure.TIM_ICFilter = 0;
  TIM_ICInit(TIM5, &TIM_ICInitStructure);

  //Enable the TIM5 Input Capture counter
  TIM_Cmd(TIM5, ENABLE);

  //Use input capture to obtain the period of the LSI measured against PCLK1
  TIM_ClearFlag(TIM5, TIM_FLAG_CC4);
  while(TIM_GetFlagStatus(TIM5, TIM_FLAG_CC4) == RESET) {}
  lsiCapture1 = TIM_GetCapture4(TIM5);
  
  TIM_ClearFlag(TIM5, TIM_FLAG_CC4);
  while(TIM_GetFlagStatus(TIM5, TIM_FLAG_CC4) == RESET) {}
  lsiCapture2 = TIM_GetCapture4(TIM5);
  
  lsiPeriod = (uint32_t)(lsiCapture2-lsiCapture1);
  
  lsiFreq = ((RCC_Clocks.PCLK1_Frequency * 2)/(lsiPeriod));
  
  //Set RTC prescaler for a 1ms tick
  RTC_SetPrescaler((lsiFreq/1000));
  
  //Wait until last write operation to RTC registers has finished
  RTC_WaitForLastTask();
  
  //RTC should now be producing a 1.015ms tick
}
#endif //ENABLE_OSC32K

uint16_t halInternalStartSystemTimer(void)
{
  initRtc();
  
  return 0;
}

uint16_t halCommonGetInt16uMillisecondTick(void)
{
  return (uint16_t)halCommonGetInt32uMillisecondTick();
}

uint16_t halCommonGetInt16uQuarterSecondTick(void)
{
  return (uint16_t)(halCommonGetInt32uMillisecondTick() >> 8);
}

uint32_t halCommonGetInt32uMillisecondTick(void)
{
  uint32_t time;
  
  ATOMIC(
    time = RTC_GetCounter();
  )
  
  return time;
}

void halCommonSetSystemTime(uint32_t time)
{
  uint32_t initialTime;
  
  ATOMIC(
    initialTime = RTC_GetCounter();
    RTC_SetCounter(time);
    //Updating the RTC is not instantaneous and requires a few dozen
    //microseconds.  Block until the counter is updated so calling
    //code doesn't get confused by not observing the new time.
    while(initialTime == RTC_GetCounter()){}
  )
}

