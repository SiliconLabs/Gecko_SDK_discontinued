/** @file hal/host/cortexm3/stm32f103ret/uart.c
 * @brief LED manipulation routines
 *
 * <!-- Copyright 2010 by Ember Corporation. All rights reserved.        *80*-->
 */

#include PLATFORM_HEADER
#include "stack/include/error.h"
#include "hal/hal.h"


static void configureLedGpio(GPIO_TypeDef * port, uint16_t pin)
{
  GPIO_InitTypeDef GPIOParameters;
  
  // Initialize GPIO Pin as push-pull output
  GPIOParameters.GPIO_Pin =  pin;
  GPIOParameters.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIOParameters.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(port, &GPIOParameters);
  
  //Default the active low LED to off
  GPIO_WriteBit(port, pin, Bit_SET);
}

void halInternalInitLed(void)
{
  configureLedGpio(BOARDLED0_PORT, BOARDLED0_PIN);
  configureLedGpio(BOARDLED1_PORT, BOARDLED1_PIN);
}

void halSetLed(HalBoardLed led)
{
  GPIO_TypeDef* port;
  uint16_t pin;
  
  switch(led) {
  case BOARDLED0:
    port = BOARDLED0_PORT;
    pin = BOARDLED0_PIN;
    break;
  case BOARDLED1:
    port = BOARDLED1_PORT;
    pin = BOARDLED1_PIN;
    break;
  }
  
  // Drive LED GPIO low since our LEDs are active low
  GPIO_WriteBit(port, pin, Bit_RESET);
}

void halClearLed(HalBoardLed led)
{
  GPIO_TypeDef* port;
  uint16_t pin;
  
  switch(led) {
  case BOARDLED0:
    port = BOARDLED0_PORT;
    pin = BOARDLED0_PIN;
    break;
  case BOARDLED1:
    port = BOARDLED1_PORT;
    pin = BOARDLED1_PIN;
    break;
  }
  
  // Drive LED GPIO high since our LEDs are active low
  GPIO_WriteBit(port, pin, Bit_SET);
}

void halToggleLed(HalBoardLed led)
{
  GPIO_TypeDef* port;
  uint16_t pin;
  
  switch(led) {
  case BOARDLED0:
    port = BOARDLED0_PORT;
    pin = BOARDLED0_PIN;
    break;
  case BOARDLED1:
    port = BOARDLED1_PORT;
    pin = BOARDLED1_PIN;
    break;
  }
  
  if(GPIO_ReadOutputDataBit(port, pin)) {
    // LED is off, turn on
    GPIO_WriteBit(port, pin, Bit_RESET);
  } else {
    // LED is on, turn off
    GPIO_WriteBit(port, pin, Bit_SET);
  }
}

