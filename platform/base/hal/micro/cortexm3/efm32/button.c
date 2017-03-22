/** @file hal/micro/cortexm3/button.c
 *  @brief  Sample API functions for operating buttons (external interrupts).
 *  Please see the board header file for the definition of the BUTTON*
 *  instances used below.  The BUTTON* defines used here are designed to make
 *  this library generic such that only the board header needs to be modified
 *  to change which GPIO the buttons are on.
 * 
 * <!-- Author(s): Brooks Barrett -->
 * <!-- Copyright 2007 by Ember Corporation. All rights reserved.       *80*-->   
 */

#include PLATFORM_HEADER
#include "stack/include/ember.h"
#include "hal/hal.h"
#include "serial/serial.h"

#include "em_cmu.h"
#include "em_gpio.h"
#include "gpiointerrupt.h"

// This state is kept track of so the IRQ ISRs knows when to notify buttonIsrs.
#ifdef BUTTON0
uint8_t button0State;
#endif

#ifdef BUTTON1
uint8_t button1State;
#endif

#if (defined(BUTTON0) || defined (BUTTON1))
void BUTTON_ISR(uint8_t pin);
#endif

void halInternalInitButton(void)
{
  /* Initialize GPIO interrupt dispatcher */
  GPIOINT_Init();

  /* Enable GPIO in CMU */
  CMU_ClockEnable(cmuClock_GPIO, true);

  
#ifdef BUTTON0
  /* Configure pin as input */
  GPIO_PinModeSet(BUTTON0_PORT, BUTTON0, gpioModeInput, 0);
  /* Register callbacks before setting up and enabling pin interrupt. */
  GPIOINT_CallbackRegister(BUTTON0, BUTTON_ISR);
  /* Set rising and falling edge interrupts */
  GPIO_ExtIntConfig(BUTTON0_PORT, BUTTON0, BUTTON0, true, true, true);
#endif //BUTTON0

#ifdef BUTTON1
  /* Configure pin as input */
  GPIO_PinModeSet(BUTTON1_PORT, BUTTON1, gpioModeInput, 0);
  /* Register callbacks before setting up and enabling pin interrupt. */
  GPIOINT_CallbackRegister(BUTTON1, BUTTON_ISR);
  /* Set rising and falling edge interrupts */
  GPIO_ExtIntConfig(BUTTON1_PORT, BUTTON1, BUTTON1, true, true, true);
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

void halInternalButtonStateSet(uint8_t button, uint8_t state)
{
  // Note: this returns the "soft" state rather than reading the port
  //  so it jives with the interrupts and their callbacks
  switch(button) {
#ifdef BUTTON0
    case BUTTON0:
      button0State = state;
      break;
#endif
#ifdef BUTTON1
    case BUTTON1:
      button1State = state;
      break;
#endif
    default:
      break;
  }
}

void halInternalButtonToggleState(uint8_t button)
{
  // Note: this returns the "soft" state rather than reading the port
  //  so it jives with the interrupts and their callbacks
  switch(button) {
#ifdef BUTTON0
    case BUTTON0:
      button0State = !button0State;
      break;
#endif
#ifdef BUTTON1
    case BUTTON1:
      button1State = !button1State;
      break;
#endif
    default:
      break;
  }
}

uint8_t halButtonPinState(uint8_t button)
{

  switch(button) {
#ifdef BUTTON0
    case BUTTON0:
      return (GPIO_PinInGet(BUTTON0_PORT, BUTTON0)) ? BUTTON_RELEASED : BUTTON_PRESSED;
#endif
#ifdef BUTTON1
    case BUTTON1:
      return (GPIO_PinInGet(BUTTON1_PORT, BUTTON1)) ? BUTTON_RELEASED : BUTTON_PRESSED;
#endif
    default:
      return BUTTON_RELEASED;
  }
}


//DEBOUNCE operation is based upon the theory that when multiple reads in a row
//return the same value, we have passed any debounce created by the mechanical
//action of a button.  The define "DEBOUNCE" says how many reads in a row
//should return the same value.  The value '5', below, is the recommended value
//since this should require the signal to have stabalized for approximately
//100us which should be much longer than any debounce action.
//Uncomment the following line to enable software debounce operation:
//#define DEBOUNCE 5

//Typically, software debounce is disabled by defaulting to a value of '0'
//which will cause the preprocessor to strip out the debounce code and save
//flash space.
#ifndef DEBOUNCE
#define DEBOUNCE 5
#endif//DEBOUNCE


#if (defined(BUTTON0) || defined (BUTTON1))
void BUTTON_ISR(uint8_t pin)
{
  uint8_t buttonStateNow;
  #if (DEBOUNCE > 0)
    uint8_t buttonStatePrev;
    uint32_t debounce;
  #endif //(DEBOUNCE > 0)

  buttonStateNow = halButtonPinState(pin);
  #if (DEBOUNCE > 0)
    //read button until get "DEBOUNCE" number of consistent readings
    for( debounce = 0;
         debounce < DEBOUNCE;
         debounce = (buttonStateNow == buttonStatePrev) ? debounce + 1 : 0 ) {
      buttonStatePrev = buttonStateNow;
      buttonStateNow = halButtonPinState(pin);
    }
  #endif //(DEBOUNCE > 0)


  if(halButtonState(pin) != buttonStateNow) { //state changed, notify app
    halInternalButtonStateSet(pin, buttonStateNow);
    halButtonIsr(pin,  halButtonState(pin));
  } else {  //state unchanged, then notify app of a double-transition
    halInternalButtonToggleState(pin); //invert temporarily for calling Isr
    halButtonIsr(pin, halButtonState(pin));
    halInternalButtonToggleState(pin); //and put it back to current state
    halButtonIsr(pin, halButtonState(pin));
  }

}

WEAK(void halButtonIsr(uint8_t button, uint8_t state) {})
#endif

