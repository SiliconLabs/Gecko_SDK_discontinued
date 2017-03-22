/** @file hal/plugin/buzzer/buzzer-stm32f103ret.c
 * @brief  Sample API functions for using a timer to play tunes on a buzzer.
 *
 * <!-- Copyright 2010 by Ember Corporation. All rights reserved.        *80*-->
 */

#include PLATFORM_HEADER
#include "stack/include/error.h"
#include "hal/hal.h"

uint8_t PGM hereIamTune[] = {
  NOTE_B4,  1,
  0,        1,
  NOTE_B4,  1,
  0,        1,
  NOTE_B4,  1,
  0,        1,
  NOTE_B5,  5,
  0,        0
};

uint8_t PGM *currentTune = NULL;
uint8_t tunePos = 0;
uint16_t currentDuration = 0;
uint8_t tuneDone = true;

static void configureBuzzer(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_OCInitTypeDef TIM_OCInitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  
  //Configure GPIO for alternate function of PC6 and remap TIM3_CH1 to PC6
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE);
  
  //Configure time base for 1us tick (72MHz/72 = 1MHz)
  TIM_TimeBaseStructure.TIM_Prescaler = 72;
  //Default to max period (ARR register).  This will be updated as needed.
  TIM_TimeBaseStructure.TIM_Period = 0xFFFF;
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
  
  //Configure timer to toggle output of CH1 on every OC event
  TIM_OCStructInit(&TIM_OCInitStructure);
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Toggle;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OC1Init(TIM3, &TIM_OCInitStructure);
  
  //Enable preloading so the ARR/OC only update on events
  TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
  TIM_ARRPreloadConfig(TIM3, ENABLE);
  
  //Configure TIM3 (top level) NVIC interrupt and leave it disabled
  TIM_ITConfig(TIM3, TIM_IT_CC1, DISABLE);
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void halPlayTune_P(uint8_t PGM *tune, bool bkg)
{
  configureBuzzer();
  
  currentTune = tune;
  //1us timer tick and notes are stored as (freq/4)
  TIM_SetAutoreload(TIM3, (125000/tune[0]));
  TIM_SetCompare1(TIM3, (125000/tune[0]));
  TIM_SetCounter(TIM3, 0);
  
  //Magic duration calculation based on frequency
  //determine how many timer toggles of this note fit in 100ms
  currentDuration = ((100*((uint16_t)tune[0]))*((uint16_t)tune[1]))/125;
  tunePos = 2;  // First note is already set up
  tuneDone = false;
  
  ATOMIC(
    //enable OC1 second-level interrupt
    TIM_ITConfig(TIM3, TIM_IT_CC1, ENABLE);
    //enable TIM3 top-level interrupt
    NVIC_EnableIRQ(TIM3_IRQn);
    //clear second-level interrupt
    TIM_ClearFlag(TIM3, TIM_FLAG_CC1);
    //clear top-level interrupt
    NVIC_ClearPendingIRQ(TIM3_IRQn);
    TIM_Cmd(TIM3, ENABLE); //Enable timer
  )
  
  while((!bkg) && (!tuneDone)) {
    halResetWatchdog();
  }
}

void TIM3_IRQHandler(void)
{
  TIM_ClearFlag(TIM3, TIM_FLAG_CC1);
  
  if(currentDuration-- == 0) {
    if(currentTune[tunePos+1]) {
      if(currentTune[tunePos]) {
        //generate a note
        //1us timer tick and notes are stored as (freq/4)
        TIM_SetAutoreload(TIM3, (125000/currentTune[tunePos]));
        TIM_SetCompare1(TIM3, (125000/currentTune[tunePos]));
        TIM_SetCounter(TIM3, 0);
        TIM_CCxCmd(TIM3, TIM_Channel_1, TIM_CCx_Enable);
        //Magic duration calculation based on frequency
        //determine how many timer toggles of this note fit in 100ms
        currentDuration = ((100*((uint16_t)currentTune[tunePos]))*
                            ((uint16_t)currentTune[tunePos+1]))/125;
      } else {
        //generate a pause by waiting 100 1000 1us tick duration with
        //output disabled (to get a multiple of 100ms).
        TIM_SetAutoreload(TIM3, 1000);
        TIM_SetCompare1(TIM3, 1000);
        TIM_SetCounter(TIM3, 0);
        TIM_CCxCmd(TIM3, TIM_Channel_1, TIM_CCx_Disable);
        currentDuration = (currentTune[tunePos+1]*100);
      }
      tunePos += 2;
    } else {
      //End tune: disable interrupts, timer, and force output low
      TIM_ITConfig(TIM3, TIM_IT_CC1, DISABLE);
      NVIC_DisableIRQ(TIM3_IRQn);
      TIM_CCxCmd(TIM3, TIM_Channel_1, TIM_CCx_Disable);
      TIM_Cmd(TIM3, DISABLE);
      tuneDone = true;
    }
  }
}

void halStartBuzzerTone(uint16_t frequency)
{
  configureBuzzer();
  
  //Tone doesn't use interrupts since the tone doesn't change over time.
  TIM_ITConfig(TIM3, TIM_IT_CC1, DISABLE);
  NVIC_DisableIRQ(TIM3_IRQn);
  
  TIM_SetAutoreload(TIM3, (500000/frequency));
  TIM_SetCompare1(TIM3, (500000/frequency));
  TIM_SetCounter(TIM3, 0);
  
  TIM_Cmd(TIM3, ENABLE); //Enable timer
}

void halStopBuzzerTone(void)
{
  TIM_CCxCmd(TIM3, TIM_Channel_1, TIM_CCx_Disable);
  TIM_Cmd(TIM3, DISABLE);
}

