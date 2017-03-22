/**************************************************************************//**
 * @file main.c
 * @brief Capacitive touch example for SLSTK3400A-EFM32HG
 *
 * @version 5.1.2
 ******************************************************************************
 * @section License
 * <b>Copyright 2015 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "em_device.h"
#include "em_adc.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"

#include "capsense.h"
#include "display.h"
#include "retargettextdisplay.h"

static volatile uint32_t msTicks; /* counts 1ms timeTicks */

/**************************************************************************//**
 * @brief SysTick_Handler
 * Interrupt Service Routine for system tick counter
 *****************************************************************************/
void SysTick_Handler(void)
{
  msTicks++;       /* increment counter necessary in Delay()*/
}

/**************************************************************************//**
 * @brief Delays number of msTick Systicks (typically 1 ms)
 * @param dlyTicks Number of ticks to delay
 *****************************************************************************/
static void Delay(uint32_t dlyTicks)
{
  uint32_t curTicks;

  curTicks = msTicks;
  while ((msTicks - curTicks) < dlyTicks) ;
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  int i;

  /* Chip errata */
  CHIP_Init();

  /* Setup SysTick timer for 1 msec interrupts  */
  if (SysTick_Config(SystemCoreClockGet() / 1000)) while (1) ;

  /* Initialize the display module. */
  DISPLAY_Init();

  /* Retarget stdio to the display. */
  RETARGET_TextDisplayInit();

  /* Start capacitive sense buttons */
  CAPSENSE_Init();

  i = 0;
  printf("\n\n\n  %3d", i );
  while (1)
  {
    Delay( 100 );


    CAPSENSE_Sense();

    if ( CAPSENSE_getPressed(BUTTON1_CHANNEL) &&
        !CAPSENSE_getPressed(BUTTON0_CHANNEL))
    {
      i++;
      printf("\r  %3d", i );
    }
    else if ( CAPSENSE_getPressed(BUTTON0_CHANNEL) &&
             !CAPSENSE_getPressed(BUTTON1_CHANNEL))
    {
      if ( i>0 )
        i--;
      printf("\r  %3d", i );
    }
  }
}
