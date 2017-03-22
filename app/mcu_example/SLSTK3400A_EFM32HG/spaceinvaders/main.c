/**************************************************************************//**
 * @file main.c
 * @brief Space Invaders Game for SLSTK3400A_EFM32HG
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

#include "em_device.h"
#include "em_emu.h"
#include "em_cmu.h"
#include "em_chip.h"
#include "spaceinvaders.h"


/************************************************************
 **                                                        **
 **   SYSTICK interrupt handler                            **
 **                                                        **
 ************************************************************/
void SysTick_Handler( void ) {

   SPACEINVADERS_TimerEventHandler();
}


/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
   /* Chip errata */
   CHIP_Init();

   /* Enable HFXO as the main clock */
   CMU_ClockSelectSet( cmuClock_HF, cmuSelect_HFXO );

   /* Setup SysTick Timer for 1 msec interrupts  */
   if (SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 1000)) while (1) ;

   /* Initiate game */
   SPACEINVADERS_Init();

   /* Enter game loop. Will not return */
   SPACEINVADERS_GameLoop();

   return 0;
}
