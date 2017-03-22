/**************************************************************************//**
 * @file
 * @brief CPT112S Demo
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
 ******************************************************************************
 *
 * Program Description:
 *
 * This demo demonstrates the capacitive button function of CPT112S fixed
 * function board. This demo runs on the EFM32HG STK,and communicates with
 * CPT112S through the SMBUS pins on the EXP header. Virtual buttons on
 * STK's LCD screen indicate whether the capacitive sensing buttons on CPT112S
 * are pressed or released.
 *
 * This demo supports 11 buttons on the CPT112S device.(The CS11 button is
 * disabled in the default configuration of the board to enable the buzzer)
 *
 * ----------------------------------------------------------------------------
 * How To Test: EFM32Z STK
 * ----------------------------------------------------------------------------
 * 1) Place the SW104 switch in "AEM" mode.
 * 2) Attach CPT112S to STK through EXP headers
 * 3) Connect the EFM32HG STK board to a PC using a mini USB cable.
 * 4) Compile and download code to the EFM32HG STK board.
 *    In Simplicity Studio IDE, select Run -> Debug from the menu bar,
 *    click the Debug button in the quick menu, or press F11.
 * 5) Run the code.
 *    In Simplicity Studio IDE, select Run -> Resume from the menu bar,
 *    click the Resume button in the quick menu, or press F8.
 * 6) The demo program should start with a set of circles on display, by
 *    pressing/releasing capacitive buttons on CPT112S, the circles turns
 *    ON/OFF.On touching the slider, a circle appears on the LCD screen
 *	  indicating the position of the finger on the slider.
 *
 * Release 0.1 (MR)
 *    - Initial Revision
 *    - 05 Nov 2015
 *
 *****************************************************************************/

#include "lcd.h"
#include "cpt112s_i2c.h"
#include "em_chip.h"

/**************************************************************************//**
 * @brief  Main function of GLIB example.
 *****************************************************************************/
int main(void)
{
	/* Chip errata */
	CHIP_Init();

  // Extra initializations
	CPT112S_init();
  initLCD();

  updateLCD();

  while(1)
  {
    CPT112S_update();
    if (CPT112S_getCapsensePrevious() != CPT112S_getCapsenseCurrent() ||
        CPT112S_getSliderPrevious() != CPT112S_getSliderCurrent())
    {
      updateLCD();
    }
  }
}
