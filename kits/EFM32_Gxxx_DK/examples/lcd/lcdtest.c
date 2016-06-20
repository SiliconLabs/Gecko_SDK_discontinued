/**************************************************************************//**
 * @file
 * @brief LCD test/demo routines
 * @version 4.2.0
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2014 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "em_device.h"
#include "em_lcd.h"
#include "segmentlcd.h"
#include "lcdtest.h"

/** SysTick polled Delay routine */
extern void Delay(uint32_t dlyTicks);

/** Demo scroll text */
static char *stext = "Welcome to Silicon Labs EFM32        ";

/**************************************************************************//**
 * @brief LCD scrolls a text over the display, sort of "polled printf"
 *****************************************************************************/
void ScrollText(char *scrolltext)
{
  int  i, len;
  char buffer[8];

  buffer[7] = 0x00;
  len       = strlen(scrolltext);
  if (len < 7) return;
  for (i = 0; i < (len - 7); i++)
  {
    memcpy(buffer, scrolltext + i, 7);
    SegmentLCD_Write(buffer);
    Delay(125);
  }
}


/**************************************************************************//**
 * @brief LCD Blink Test
 *****************************************************************************/
void BlinkTest(void)
{
  SegmentLCD_EnergyMode(0, 1);
  SegmentLCD_EnergyMode(1, 1);
  SegmentLCD_EnergyMode(2, 1);
  SegmentLCD_EnergyMode(3, 1);
  SegmentLCD_EnergyMode(4, 1);
  SegmentLCD_Number(2359);
  SegmentLCD_Symbol(LCD_SYMBOL_COL10, 1);
  SegmentLCD_Symbol(LCD_SYMBOL_GECKO, 1);
  SegmentLCD_Symbol(LCD_SYMBOL_EFM32, 1);
  SegmentLCD_Write(" EFM32 ");

  LCD_BlinkEnable(true);

  Delay(2000);
  SegmentLCD_EnergyMode(4, 0);
  Delay(62);
  SegmentLCD_EnergyMode(3, 0);
  Delay(62);
  SegmentLCD_EnergyMode(2, 0);
  Delay(62);
  SegmentLCD_EnergyMode(1, 0);
  Delay(62);
  SegmentLCD_EnergyMode(0, 0);

  LCD_BlinkEnable(false);
}

/**************************************************************************//**
 * @brief LCD Test Routine, shows various text and patterns
 *****************************************************************************/
void Test(void)
{
  int i, numberOfIterations = 1000;

  /* Exercise for the reader: Go to EM2 here and on all delays... */
  while (--numberOfIterations)
  {
    SegmentLCD_AllOff();
    for (i = 100; i > 0; i--)
    {
      SegmentLCD_Number(i);
      Delay(10);
    }
    SegmentLCD_NumberOff();

    SegmentLCD_Symbol(LCD_SYMBOL_GECKO, 1);
    SegmentLCD_Symbol(LCD_SYMBOL_EFM32, 1);
    SegmentLCD_Write(" Gecko ");
    Delay(1000);

    SegmentLCD_AllOn();
    Delay(1000);

    SegmentLCD_AllOff();
    SegmentLCD_Write("OOOOOOO");
    Delay(62);
    SegmentLCD_Write("XXXXXXX");
    Delay(62);
    SegmentLCD_Write("+++++++");
    Delay(62);
    SegmentLCD_Write("@@@@@@@");
    Delay(62);
    SegmentLCD_Write("SILICON");
    Delay(250);
    SegmentLCD_Write("@@LICON");
    Delay(62);
    SegmentLCD_Write(" @@ICON");
    Delay(62);
    SegmentLCD_Write(" L@@CON");
    Delay(62);
    SegmentLCD_Write(" LA@@ON");
    Delay(62);
    SegmentLCD_Write(" LAB@@N");
    Delay(62);
    SegmentLCD_Write(" LABS@@");
    Delay(62);
    SegmentLCD_Write(" LABS @");
    Delay(250);
    SegmentLCD_Write("-EFM32-");
    Delay(250);

    /* Various eye candy */
    SegmentLCD_AllOff();
    for (i = 0; i < 8; i++)
    {
      SegmentLCD_Number(numberOfIterations + i);
      SegmentLCD_ARing(i, 1);
      Delay(20);
    }
    for (i = 0; i < 8; i++)
    {
      SegmentLCD_Number(numberOfIterations + i);
      SegmentLCD_ARing(i, 0);
      Delay(100);
    }

    for (i = 0; i < 5; i++)
    {
      SegmentLCD_Number(numberOfIterations + i);
      SegmentLCD_Battery(i);
      SegmentLCD_EnergyMode(i, 1);
      Delay(100);
      SegmentLCD_EnergyMode(i, 0);
      Delay(100);
    }
    SegmentLCD_Symbol(LCD_SYMBOL_ANT, 1);
    for (i = 0; i < 4; i++)
    {
      SegmentLCD_EnergyMode(i, 1);
      Delay(100);
    }
    /* Scroll text */
    ScrollText(stext);

    /* Blink test*/
    BlinkTest(); \
  }
  /* Segment bits */
  SegmentLCD_AllOff();
  SegmentLCD_Disable();
}

