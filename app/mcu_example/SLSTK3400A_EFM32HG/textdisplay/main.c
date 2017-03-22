/**************************************************************************//**
 * @file main.c
 * @brief Example that demonstrates the use of printf and other textdisplay
 *        features on the Sharp Memory LCD on the SLSTK3400A_EFM32HG.
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
#include "em_device.h"
#include "em_cmu.h"
#include "em_chip.h"
#include "bsp.h"

#include "display.h"
#include "textdisplay.h"
#include "retargettextdisplay.h"

volatile uint32_t msTicks; /* counts 1ms timeTicks */

/* Local prototypes */
void Delay(uint32_t dlyTicks);

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
void Delay(uint32_t dlyTicks)
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
  int toggleLED = 0;
  int cnt, x, y;
  bool movedown, moveright;

  /* Chip errata */
  CHIP_Init();

  /* Use 48MHZ HFXO as core clock frequency */
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);

  /* Initialize DK board register access */
  /* This demo currently only works in EBI mode */
  BSP_Init(BSP_INIT_DEFAULT);
  BSP_LedsInit();

  /* Setup SysTick Timer for 10 msec interrupts  */
  if (SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 1000))
  {
    while (1) ;
  }

  /* Initialize the display module. */
  DISPLAY_Init();

  /* Retarget stdio to a text display. */
  if (RETARGET_TextDisplayInit() != TEXTDISPLAY_EMSTATUS_OK)
  {
    while (1) ;
  }

  /* Output text on Memory LCD */
  printf("Hello, EFM32 Happy Gecko world!");
  Delay(2000);

  /* Clear screen */
  printf("\f");

  cnt = x = y = 0;
  movedown = moveright = true;

  /* Update Memory LCD display forever */
  while (1)
  {
    printf("%d", cnt );

    if (movedown)
    {
      y++;
      printf( TEXTDISPLAY_ESC_SEQ_CURSOR_DOWN_ONE_LINE );
    }
    else
    {
      y--;
      printf( TEXTDISPLAY_ESC_SEQ_CURSOR_UP_ONE_LINE );
    }

    if (y >= TEXTDISPLAY_DEVICE_0_LINES-1)
      movedown=false;
    if (y == 0)
      movedown=true;

    if (moveright)
    {
      x++;
      if (cnt >= 10)
        printf( TEXTDISPLAY_ESC_SEQ_CURSOR_LEFT_ONE_CHAR );
    }
    else
    {
      x--;
      if (cnt > 10)
      {
        printf( TEXTDISPLAY_ESC_SEQ_CURSOR_LEFT_ONE_CHAR );
        printf( TEXTDISPLAY_ESC_SEQ_CURSOR_LEFT_ONE_CHAR );
        printf( TEXTDISPLAY_ESC_SEQ_CURSOR_LEFT_ONE_CHAR );
      }
      else
      {
        printf( TEXTDISPLAY_ESC_SEQ_CURSOR_LEFT_ONE_CHAR );
        printf( TEXTDISPLAY_ESC_SEQ_CURSOR_LEFT_ONE_CHAR );
      }
    }

    if (x >= TEXTDISPLAY_DEVICE_0_COLUMNS-2)
      moveright=false;
    if (x == 0)
      moveright=true;

    /* Clear screen and reset counter when 100 is reached. */
    if (++cnt == 100)
    {
      printf("\f");
      cnt = x = y = 0;
      movedown = moveright = true;
    }

    /* Toggle led after each Memory LCD_displayUpdate iteration */
    if (toggleLED)
    {
      BSP_LedsSet(0x0000);
      toggleLED = 0;
    }
    else
    {
      BSP_LedsSet(0x000f);
      toggleLED = 1;
    }
    Delay(100);
  }
}
