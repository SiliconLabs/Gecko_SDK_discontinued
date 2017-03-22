/**************************************************************************//**
 * @file
 * @brief Binary Support Package demo for EFM32ZG_STK3200. Read MCU voltage
 *        and current consumption from board controller.
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
#include "em_chip.h"
#include "em_cmu.h"

#include "bsp.h"
#include "display.h"
#include "textdisplay.h"
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
  int loopCount;
  int hfrcoBand;
  float32_t current, voltage;

  /* Chip errata */
  CHIP_Init();

  /* Initialize board support package */
  BSP_Init(BSP_INIT_BCC);

  /* Initialize the display module. */
  DISPLAY_Init();

  /* Retarget stdio to the display. */
  if (TEXTDISPLAY_EMSTATUS_OK != RETARGET_TextDisplayInit())
  {
    /* Text display initialization failed. */
    while(1);
  }

  /* This is static text on the display that never change !. */
  printf( "\n\n\n\n CPU frequency\n\n"
          "\n\n\nVoltage Current"
          "\n  (V)    (mA)\n\n" );

  loopCount = 0;
  hfrcoBand = 0;
  while (1)
  {
    /* Change HFRCO band, i.e. CPU frequency after a few seconds. */
    if (loopCount % 25 == 0)
    {
      switch (hfrcoBand)
      {
      case 0:
        CMU_HFRCOBandSet(cmuHFRCOBand_7MHz);
        break;
      case 1:
        CMU_HFRCOBandSet(cmuHFRCOBand_11MHz);
        break;
      case 2:
        CMU_HFRCOBandSet(cmuHFRCOBand_14MHz);
        break;

      default:
        CMU_HFRCOBandSet(cmuHFRCOBand_21MHz);
        /* Restart iteration */
        hfrcoBand = -1;
        break;
      }
      hfrcoBand++;

      /* Recalculate SysTick timer setup. */
      if (SysTick_Config(SystemCoreClockGet() / 1000)) while (1) ;

      /* Recalculate BCC baudrate. */
      BSP_Init(BSP_INIT_BCC);

      /* Recalculate display stuff. */
      DISPLAY_DriverRefresh();

      /* First send escape sequence to move cursor to home (top-left) position
         on the text display. */
      printf( TEXTDISPLAY_ESC_SEQ_CURSOR_HOME_VT100 );

      /* Write the new CPU frequency to the display. */
      printf("\n\n\n\n\n\n    %lu MHz \n\n\n\n\n\n",
             SystemCoreClockGet() / 1000000);
    }

    /* Update display with voltage and current measurements. */
    current = BSP_CurrentGet();
    voltage = BSP_VoltageGet();
    printf( "\r  %d.%d   %d.%d",
            (int)voltage, (int)(100*(voltage-(int)voltage)),
            (int)current, (int)(100*(current-(int)current)));

    Delay(100);
    loopCount++;
  }
}
