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
#include "stack/include/ember-types.h"
#include "hal/hal.h"

// This state is kept track of so the IRQ ISRs knows when to notify buttonIsrs.
#ifdef BUTTON0
uint8_t button0State;
#endif

#ifdef BUTTON1
uint8_t button1State;
#endif

#ifdef BUTTON2
uint8_t button2State;
#endif

#ifdef BUTTON3
uint8_t button3State;
#endif

void halInternalInitButton(void)
{
#ifdef BUTTON0
  //start from a fresh state just in case
  BUTTON0_INTCFG = 0;              //disable BUTTON0 triggering
  INT_CFGCLR = BUTTON0_INT_EN_BIT; //clear BUTTON0 top level int enable
  INT_GPIOFLAG = BUTTON0_FLAG_BIT; //clear stale BUTTON0 interrupt
  INT_MISS = BUTTON0_MISS_BIT;     //clear stale missed BUTTON0 interrupt
  //configure BUTTON0
  BUTTON0_SEL();                             //point IRQ at the desired pin
  BUTTON0_INTCFG  = (0 << GPIO_INTFILT_BIT); //no filter
  BUTTON0_INTCFG |= (3 << GPIO_INTMOD_BIT);  //3 = both edges
  button0State = halButtonPinState(BUTTON0);
#endif

#ifdef BUTTON1
  //start from a fresh state just in case
  BUTTON1_INTCFG = 0;              //disable BUTTON1 triggering
  INT_CFGCLR = BUTTON1_INT_EN_BIT; //clear BUTTON1 top level int enable
  INT_GPIOFLAG = BUTTON1_FLAG_BIT; //clear stale BUTTON1 interrupt
  INT_MISS = BUTTON1_MISS_BIT;     //clear stale missed BUTTON1 interrupt
  //configure BUTTON1
  BUTTON1_SEL();                             //point IRQ at the desired pin
  BUTTON1_INTCFG  = (0 << GPIO_INTFILT_BIT); //no filter
  BUTTON1_INTCFG |= (3 << GPIO_INTMOD_BIT);  //3 = both edges
  button1State = halButtonPinState(BUTTON1);
#endif

#ifdef BUTTON2
  //start from a fresh state just in case
  BUTTON2_INTCFG = 0;              //disable BUTTON2 triggering
  INT_CFGCLR = BUTTON2_INT_EN_BIT; //clear BUTTON2 top level int enable
  INT_GPIOFLAG = BUTTON2_FLAG_BIT; //clear stale BUTTON2 interrupt
  INT_MISS = BUTTON2_MISS_BIT;     //clear stale missed BUTTON2 interrupt
  //configure BUTTON2
  BUTTON2_SEL();                             //point IRQ at the desired pin
  BUTTON2_INTCFG  = (0 << GPIO_INTFILT_BIT); //no filter
  BUTTON2_INTCFG |= (3 << GPIO_INTMOD_BIT);  //3 = both edges
  button2State = halButtonPinState(BUTTON2);
#endif

#ifdef BUTTON3
  //start from a fresh state just in case
  BUTTON3_INTCFG = 0;              //disable BUTTON3 triggering
  INT_CFGCLR = BUTTON3_INT_EN_BIT; //clear BUTTON3 top level int enable
  INT_GPIOFLAG = BUTTON3_FLAG_BIT; //clear stale BUTTON3 interrupt
  INT_MISS = BUTTON3_MISS_BIT;     //clear stale missed BUTTON3 interrupt
  //configure BUTTON3
  BUTTON3_SEL();                             //point IRQ at the desired pin
  BUTTON3_INTCFG  = (0 << GPIO_INTFILT_BIT); //no filter
  BUTTON3_INTCFG |= (3 << GPIO_INTMOD_BIT);  //3 = both edges
  button3State = halButtonPinState(BUTTON3);
#endif
  
#ifdef BUTTON0
  INT_CFGSET = BUTTON0_INT_EN_BIT; //set top level interrupt enable
#endif

#ifdef BUTTON1
  INT_CFGSET = BUTTON1_INT_EN_BIT; //set top level interrupt enable
#endif
  
#ifdef BUTTON2
  INT_CFGSET = BUTTON2_INT_EN_BIT; //set top level interrupt enable
#endif
  
#ifdef BUTTON3
  INT_CFGSET = BUTTON3_INT_EN_BIT; //set top level interrupt enable
#endif
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
#ifdef BUTTON2
    case BUTTON2:
      return button2State;
#endif
#ifdef BUTTON3
    case BUTTON3:
      return button3State;
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
  switch(button) {
#ifdef BUTTON0
    case BUTTON0:
      return (BUTTON0_IN & BIT(BUTTON0&7)) ? BUTTON_RELEASED : BUTTON_PRESSED;
#endif
#ifdef BUTTON1
    case BUTTON1:
      return (BUTTON1_IN & BIT(BUTTON1&7)) ? BUTTON_RELEASED : BUTTON_PRESSED;
#endif
#ifdef BUTTON2
    case BUTTON2:
      return (BUTTON2_IN & BIT(BUTTON2&7)) ? BUTTON_RELEASED : BUTTON_PRESSED;
#endif
#ifdef BUTTON3
    case BUTTON3:
      return (BUTTON3_IN & BIT(BUTTON3&7)) ? BUTTON_RELEASED : BUTTON_PRESSED;
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
#define DEBOUNCE 0
#endif//DEBOUNCE


#ifdef BUTTON0
void BUTTON0_ISR(void)
{
  uint8_t buttonStateNow;
  #if (DEBOUNCE > 0)
    uint8_t buttonStatePrev;
    uint32_t debounce;
  #endif //(DEBOUNCE > 0)

  //clear int before read to avoid potential of missing interrupt
  INT_MISS = BUTTON0_MISS_BIT;     //clear missed BUTTON0 interrupt flag
  INT_GPIOFLAG = BUTTON0_FLAG_BIT; //clear top level BUTTON0 interrupt flag
  
  buttonStateNow = halButtonPinState(BUTTON0);
  #if (DEBOUNCE > 0)
    //read button until get "DEBOUNCE" number of consistent readings
    for( debounce = 0;
         debounce < DEBOUNCE;
         debounce = (buttonStateNow == buttonStatePrev) ? debounce + 1 : 0 ) {
      buttonStatePrev = buttonStateNow;
      //clear int before read to avoid potential of missing interrupt
      INT_GPIOFLAG = BUTTON0_FLAG_BIT; //clear top level BUTTON0 interrupt flag
      buttonStateNow = halButtonPinState(BUTTON0);
    }
  #endif //(DEBOUNCE > 0)
  if(button0State != buttonStateNow) { //state changed, notify app
    button0State = buttonStateNow;
    halButtonIsr(BUTTON0,  button0State);
  } else {  //state unchanged, then notify app of a double-transition
    button0State = !button0State; //invert temporarily for calling Isr
    halButtonIsr(BUTTON0, button0State);
    button0State = !button0State; //and put it back to current state
    halButtonIsr(BUTTON0, button0State);
  }
  
}
#endif

#ifdef BUTTON1
void BUTTON1_ISR(void)
{
  uint8_t buttonStateNow;
  #if (DEBOUNCE > 0)
    uint8_t buttonStatePrev;
    uint32_t debounce;
  #endif //(DEBOUNCE > 0)
  
  //clear int before read to avoid potential of missing interrupt
  INT_MISS = BUTTON1_MISS_BIT;     //clear missed BUTTON1 interrupt flag
  INT_GPIOFLAG = BUTTON1_FLAG_BIT; //clear top level BUTTON1 interrupt flag
  
  buttonStateNow = halButtonPinState(BUTTON1);
  #if (DEBOUNCE > 0)
    //read button until get "DEBOUNCE" number of consistent readings
    for( debounce = 0;
         debounce < DEBOUNCE;
         debounce = (buttonStateNow == buttonStatePrev) ? debounce + 1 : 0 ) {
      buttonStatePrev = buttonStateNow;
      //clear int before read to avoid potential of missing interrupt
      INT_GPIOFLAG = BUTTON1_FLAG_BIT; //clear top level BUTTON1 interrupt flag
      buttonStateNow = halButtonPinState(BUTTON1);
    }
  #endif //(DEBOUNCE > 0)
  if(button1State != buttonStateNow) { //state changed, notify app
    button1State = buttonStateNow;
    halButtonIsr(BUTTON1,  button1State);
  } else {  //state unchanged, then notify app of a double-transition
    button1State = !button1State; //invert temporarily for calling Isr
    halButtonIsr(BUTTON1, button1State);
    button1State = !button1State; //and put it back to current state
    halButtonIsr(BUTTON1, button1State);
  }
}
#endif

#ifdef BUTTON2
void BUTTON2_ISR(void)
{
  uint8_t buttonStateNow;
  #if (DEBOUNCE > 0)
    uint8_t buttonStatePrev;
    uint32_t debounce;
  #endif //(DEBOUNCE > 0)

  //clear int before read to avoid potential of missing interrupt
  INT_MISS = BUTTON2_MISS_BIT;     //clear missed BUTTON2 interrupt flag
  INT_GPIOFLAG = BUTTON2_FLAG_BIT; //clear top level BUTTON2 interrupt flag
  
  buttonStateNow = halButtonPinState(BUTTON2);
  #if (DEBOUNCE > 0)
    //read button until get "DEBOUNCE" number of consistent readings
    for( debounce = 0;
         debounce < DEBOUNCE;
         debounce = (buttonStateNow == buttonStatePrev) ? debounce + 1 : 0 ) {
      buttonStatePrev = buttonStateNow;
      //clear int before read to avoid potential of missing interrupt
      INT_GPIOFLAG = BUTTON2_FLAG_BIT; //clear top level BUTTON2 interrupt flag
      buttonStateNow = halButtonPinState(BUTTON2);
    }
  #endif //(DEBOUNCE > 0)
  if(button2State != buttonStateNow) { //state changed, notify app
    button2State = buttonStateNow;
    halButtonIsr(BUTTON2,  button2State);
  } else {  //state unchanged, then notify app of a double-transition
    button2State = !button2State; //invert temporarily for calling Isr
    halButtonIsr(BUTTON2, button2State);
    button2State = !button2State; //and put it back to current state
    halButtonIsr(BUTTON2, button2State);
  }
  
}
#endif

#ifdef BUTTON3
void BUTTON3_ISR(void)
{
  uint8_t buttonStateNow;
  #if (DEBOUNCE > 0)
    uint8_t buttonStatePrev;
    uint32_t debounce;
  #endif //(DEBOUNCE > 0)

  //clear int before read to avoid potential of missing interrupt
  INT_MISS = BUTTON3_MISS_BIT;     //clear missed BUTTON3 interrupt flag
  INT_GPIOFLAG = BUTTON3_FLAG_BIT; //clear top level BUTTON3 interrupt flag
  
  buttonStateNow = halButtonPinState(BUTTON3);
  #if (DEBOUNCE > 0)
    //read button until get "DEBOUNCE" number of consistent readings
    for( debounce = 0;
         debounce < DEBOUNCE;
         debounce = (buttonStateNow == buttonStatePrev) ? debounce + 1 : 0 ) {
      buttonStatePrev = buttonStateNow;
      //clear int before read to avoid potential of missing interrupt
      INT_GPIOFLAG = BUTTON3_FLAG_BIT; //clear top level BUTTON3 interrupt flag
      buttonStateNow = halButtonPinState(BUTTON3);
    }
  #endif //(DEBOUNCE > 0)
  if(button3State != buttonStateNow) { //state changed, notify app
    button3State = buttonStateNow;
    halButtonIsr(BUTTON3,  button3State);
  } else {  //state unchanged, then notify app of a double-transition
    button3State = !button3State; //invert temporarily for calling Isr
    halButtonIsr(BUTTON3, button3State);
    button3State = !button3State; //and put it back to current state
    halButtonIsr(BUTTON3, button3State);
  }
  
}
#endif

