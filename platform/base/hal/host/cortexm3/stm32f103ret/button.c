/** @file hal/host/cortexm3/stm32f103ret/button.c
 * @brief Sample API functions for using push-buttons (external interrupts).
 *
 * <!-- Copyright 2010 by Ember Corporation. All rights reserved.        *80*-->
 */

#include PLATFORM_HEADER
#include "stack/include/error.h"
#include "hal/hal.h"


// This state is kept track of so the IRQ ISRs knows when to notify buttonIsrs.
#ifdef BUTTON0
uint8_t button0State;
#endif

#ifdef BUTTON1
uint8_t button1State;
#endif

//Enable selected irq channel in the NVIC
static void initButtonNvic(uint8_t irqChannel)
{ 
  NVIC_InitTypeDef NVICParameters;
  
  NVICParameters.NVIC_IRQChannel = irqChannel;
  NVICParameters.NVIC_IRQChannelPreemptionPriority = 0;
  NVICParameters.NVIC_IRQChannelSubPriority = 0;
  NVICParameters.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVICParameters);
}

//Enable selected pin as an input with an internal pull-up
static void initButtonGpio(GPIO_TypeDef * port, uint16_t pin)
{
  GPIO_InitTypeDef GPIOParameters;
  
  // Initialize GPIO Pin as input with pull-up
  GPIOParameters.GPIO_Pin =  pin;
  GPIOParameters.GPIO_Mode = GPIO_Mode_IPU;
  GPIOParameters.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(port, &GPIOParameters);
}

void halInternalInitButton(void)
{
  EXTI_InitTypeDef EXTIParameters;
  
#ifdef BUTTON0
  //Clear any possible stale interrupt
  EXTI_ClearITPendingBit(BUTTON0_PIN);	
  
  //Initialize Button 0 GPIO
  initButtonGpio(BUTTON0_PORT, BUTTON0_PIN);
  
  //Enable IRQ channel in NVIC
  initButtonNvic(BUTTON0_IRQ);
  
  //Set up button as an external interrupt source
  GPIO_EXTILineConfig(BUTTON0_EXTI_SOURCE_PORT, BUTTON0_EXTI_SOURCE_PIN);
  
  EXTIParameters.EXTI_Line = BUTTON0_PIN;
  EXTIParameters.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTIParameters.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
  EXTIParameters.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTIParameters);
  
  button0State = halButtonPinState(BUTTON0);
#endif //BUTTON0

#ifdef BUTTON1
  //Clear any possible stale interrupt
  EXTI_ClearITPendingBit(BUTTON1_PIN);	
  
  //Initialize Button 1 GPIO
  initButtonGpio(BUTTON1_PORT, BUTTON1_PIN);
  
  //Enable IRQ channel in NVIC
  initButtonNvic(BUTTON1_IRQ);
  
  //Set up button as an external interrupt source
  GPIO_EXTILineConfig(BUTTON1_EXTI_SOURCE_PORT, BUTTON1_EXTI_SOURCE_PIN);
  
  EXTIParameters.EXTI_Line = BUTTON1_PIN;
  EXTIParameters.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTIParameters.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
  EXTIParameters.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTIParameters);
  
  button1State = halButtonPinState(BUTTON1);
#endif //BUTTON1
}

uint8_t halButtonState(uint8_t button)
{
  // Note: this returns the "soft" state rather than reading the port
  //  so it jives with the interrupts and their callbacks
  switch(button) {
#ifdef BUTTON0
    case BUTTON0:
      return button0State;
#endif
#ifdef BUTTON1
    case BUTTON1:
      return button1State;
#endif
    default:
      return BUTTON_RELEASED;
  }
}

uint8_t halButtonPinState(uint8_t button)
{
  // Note: this returns the current state of the button's pin.  It may not
  // jive with the interrupts and their callbacks, but it is useful for
  // checking the state of the button on startup.
  uint8_t buttonState;
  
  switch(button) {
#ifdef BUTTON0
  case BUTTON0:
    buttonState  = GPIO_ReadInputDataBit(BUTTON0_PORT, BUTTON0_PIN);
    return (buttonState) ? BUTTON_RELEASED : BUTTON_PRESSED;
#endif //BUTTON0
#ifdef BUTTON1
  case BUTTON1:
    buttonState  = GPIO_ReadInputDataBit(BUTTON1_PORT, BUTTON1_PIN);
    return (buttonState) ? BUTTON_RELEASED : BUTTON_PRESSED;
#endif //BUTTON1
  default:
    return BUTTON_RELEASED;
  }
}

//Due to the choice of GPIO, BUTTON0 and BUTTON1 share the same ISR handler.
#if defined(BUTTON0) || defined(BUTTON1)
void BUTTON01_ISR(void)
{
  uint8_t buttonStateNow;
  FlagStatus status;
  
  #ifdef BUTTON0
    //The flag status gets set at the same time as the interrupt pending bit,
    //so we check it to determine which button state has changed.
    status = EXTI_GetFlagStatus(BUTTON0_PIN);
    if(status == SET) {
      buttonStateNow = halButtonPinState(BUTTON0);
      if(button0State != buttonStateNow) {
        //Button 0 state has changed
        button0State = buttonStateNow;
        //Notify app of the state change
        halButtonIsr(BUTTON0, button0State);
      } else {
        //Button 0 state unchanged, double-transistion must have occured
        //Invert state temporarily and notify app of the state change
        button0State = !button0State;
        halButtonIsr(BUTTON0, button0State);
        //Invert state again to restore original state, notify app again
        button0State = !button0State;
        halButtonIsr(BUTTON0, button0State);
      }   
      EXTI_ClearFlag(BUTTON0_PIN);
      EXTI_ClearITPendingBit(BUTTON0_PIN);
    }
  #endif //BUTTON0
  
  #ifdef BUTTON1
    //The flag status gets set at the same time as the interrupt pending bit,
    //so we check it to determine which button state has changed.
    status = EXTI_GetFlagStatus(BUTTON1_PIN);
    if(status == SET) {
      buttonStateNow = halButtonPinState(BUTTON1);
      if(button1State != buttonStateNow) {
        //Button 1 state has changed
        button1State = buttonStateNow;
        //Notify app of the state change
        halButtonIsr(BUTTON1, button1State);
      } else {
        //Button 1 state unchanged, double-transistion must have occured
        //Invert state temporarily and notify app of the state change
        button1State = !button1State;
        halButtonIsr(BUTTON1, button1State);
        //Invert state again to restore original state, notify app again
        button1State = !button1State;
        halButtonIsr(BUTTON1, button1State);
      }
      EXTI_ClearFlag(BUTTON1_PIN);
      EXTI_ClearITPendingBit(BUTTON1_PIN);
    }
  #endif //BUTTON1
}
#endif //defined(BUTTON0) || defined(BUTTON1)

