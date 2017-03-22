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
 * This program demonstrates the capacitance button function of CPT112S
 * cap-sense fixed function board. The program runs on EFM32G STK, and
 * communicate to CPT112S through I2C. Virtual buttons on STK's display
 * indicate whether the capacitive sensing buttons on CPT112S is pressed or
 * released.
 *
 * ----------------------------------------------------------------------------
 * How To Test: EFM32G STK
 * ----------------------------------------------------------------------------
 * 1) Place the SW104 switch in "AEM" mode.
 * 2) Attach CPT112S to STK through EXP headers
 * 3) Connect the EFM32G STK board to a PC using a mini USB cable.
 * 4) Compile and download code to the EFM32G STK board.
 *    In Simplicity Studio IDE, select Run -> Debug from the menu bar,
 *    click the Debug button in the quick menu, or press F11.
 * 5) Run the code.
 *    In Simplicity Studio IDE, select Run -> Resume from the menu bar,
 *    click the Resume button in the quick menu, or press F8.
 * 6) The demo program should start with a set of four 8's on display-
 *    each representing one button along with a slider counter. By
 *    pressing/releasing capacitive buttons on CPT112S, the 8's turns
 *    ON/OFF. When the slider is touched, the slider counter shows the
 *    value of the slider between 0 to 100 (Raw data is 0 to 0x40). A
 *    ring is also appears on the screen depending on the finger's
 *    position.
 *
 *    Release 0.1 (MR)
 *    - Initial Revision
 *    - 04 Nov 2015
 *
 *****************************************************************************/

#include "cpt112s_config.h"
#include "cpt112s_i2c.h"
#include "retargetswo.h"
#include "segmentlcd.h"
#include "em_chip.h"
#include "em_gpio.h"
#include <stdio.h>
#include <string.h>

/*******************************************************************************
 ******************************   DEFINES   ************************************
 ******************************************************************************/

#define NUM_TOP_BLOCKS    4   // Number of capsense inputs/blocks in the top row
#define NUM_BOT_BLOCKS    3   // Number of capsense inputs/blocks in the bottom row

/*******************************************************************************
 ******************************  PROTOTYPES ************************************
 ******************************************************************************/

static void capSenseAringUpdate(uint16_t sliderPos);
static void capSenseBlockUpdate(uint16_t capsense);
static void capSenseNumberText(uint16_t sliderPos);
static void updateLCD(uint16_t capsense, uint16_t slider);
static void updateSWO(void);

/*******************************************************************************
 ******************************   STRUCTS   ************************************
 ******************************************************************************/

/**************************************************************************//**
 * @brief Defines capsense input channel to LCD block mapping
 *****************************************************************************/
typedef struct
{
  uint8_t channel;  /**< Capsense Input Channel */
  uint8_t block;    /**< LCD Block position */

} Capsense_LCD_Map_TypeDef;

/*******************************************************************************
 ******************************  CONSTANTS  ************************************
 ******************************************************************************/

// Capsense channel to top row block mapping
static const Capsense_LCD_Map_TypeDef topChannelBlocks[NUM_TOP_BLOCKS] =
{
 {4, 0},
 {5, 1},
 {6, 2},
 {7, 3}
};

// Capsense channel to bottom row block mapping
static const Capsense_LCD_Map_TypeDef botChannelBlocks[NUM_BOT_BLOCKS] =
{
 {8, 0},
 {9, 1},
 {10, 2}
};

/*******************************************************************************
 **************************   LOCAL FUNCTIONS   ********************************
 ******************************************************************************/

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  /* Chip errata */
  CHIP_Init();

  // Extra initializations
  CPT112S_init();

  /* Enable LCD without voltage boost */
  SegmentLCD_Init(false);
  setupSWOForPrint();

  updateLCD(CPT112S_getCapsenseCurrent(), CPT112S_getSliderCurrent());

  while(1)
  {
    CPT112S_update();

    if(CPT112S_getCapsensePrevious() != CPT112S_getCapsenseCurrent() ||
       CPT112S_getSliderPrevious() != CPT112S_getSliderCurrent())
    {
      updateLCD(CPT112S_getCapsenseCurrent(), CPT112S_getSliderCurrent());
      updateSWO();
    }
  }
}

/**************************************************************************//**
 * @brief Update rings according to slider position
 * @par sliderPos The current Slider position
 *****************************************************************************/
static void capSenseAringUpdate(uint16_t sliderPos)
{
  int i;
  int stop;

  if (sliderPos == 0xFFFF)
  {
    /* No ring if touch slider is not touched */
    stop = -1;
  }
  else
  {
    /* Map 8 segments to 40 slider positions */
    stop = (int)(sliderPos * 8) / 0x40;
  }

  /* Draw ring */
  for (i=0; i < 8; i++)
  {
    if (i <= stop )
    {
      SegmentLCD_ARing(i, true);
    }
    else
    {
      SegmentLCD_ARing(i, false);
    }
  }
}


/**************************************************************************//**
 * @brief  Update top/bottom row blocks to match capsense state
 *****************************************************************************/
static void capSenseBlockUpdate(uint16_t capsense)
{
  SegmentLCD_BlockMode_TypeDef topMode[SEGMENT_LCD_NUM_BLOCK_COLUMNS];
  SegmentLCD_BlockMode_TypeDef botMode[SEGMENT_LCD_NUM_BLOCK_COLUMNS];

  uint8_t channel;
  uint8_t block;

  memset(topMode, segmentLCDBlockModeBlank, sizeof(topMode));
  memset(botMode, segmentLCDBlockModeBlank, sizeof(botMode));

  for (int i = 0; i < NUM_TOP_BLOCKS; i++)
  {
    channel= topChannelBlocks[i].channel;
    block = topChannelBlocks[i].block;

    // Capsense channel is active
    if (capsense & (1 << channel))
    {
      // Outline and fill corresponding block
      topMode[block] = segmentLCDBlockModeOutlineFill;
    }
    // Not active
    else
    {
      // Outline corresponding block (don't fill)
      topMode[block] = segmentLCDBlockModeOutline;
    }
  }

  for (int i = 0; i < NUM_BOT_BLOCKS; i++)
  {
    channel= botChannelBlocks[i].channel;
    block = botChannelBlocks[i].block;

    // Capsense channel is active
    if (capsense & (1 << channel))
    {
      // Outline and fill corresponding block
      botMode[block] = segmentLCDBlockModeOutlineFill;
    }
    // Not active
    else
    {
      // Outline corresponding block (don't fill)
      botMode[block] = segmentLCDBlockModeOutline;
    }
  }

  SegmentLCD_Block(topMode, botMode);
}


/**************************************************************************//**
 * @brief  Output slider position in the number section of the LCD
 *****************************************************************************/
static void capSenseNumberText(uint16_t sliderPos)
{
  int sliderPosInt = (int) sliderPos;

  if (sliderPos == 0xFFFF)
  {
    sliderPosInt = 0;
  }
  else
  {
    sliderPosInt = (int)(sliderPos * 100)/(0x40);
  }

  SegmentLCD_Number(sliderPosInt);
}


/******************************************************************************
 * @brief update capsense status on LCD
 *****************************************************************************/
static void updateLCD(uint16_t capsense, uint16_t slider)
{
  capSenseBlockUpdate(capsense);
  capSenseAringUpdate(slider);
  capSenseNumberText(slider);
}


/******************************************************************************
 * @brief update SWO output contents
 *****************************************************************************/
static void updateSWO(void)
{
  uint8_t i;
  uint16_t shiftOne = 0x0001;

  for (i = 0; i < TOTAL_CAPSENSE_PIN; i++)
  {
    char tempstr[15];
    if (!(CPT112S_getCapsensePrevious() & shiftOne) && (CPT112S_getCapsenseCurrent() & shiftOne))
    {
      sprintf(tempstr, "CS%02u pressed\n", (uint16_t) i);
      printf(tempstr);
    }
    else if ((CPT112S_getCapsensePrevious() & shiftOne) && !(CPT112S_getCapsenseCurrent() & shiftOne))
    {
      sprintf(tempstr, "CS%02u released\n", (uint16_t) i);
      printf(tempstr);
    }
    else
    {
      // nothing
    }
    shiftOne <<= 1;
  }
}

