/**************************************************************************//**
 * @file
 * @brief CPT007B Demo
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
 * This demo demonstrates the capacitive button function of CPT007B fixed
 * function board. This demo runs on the EFM32WG STK, and communicates
 *  with CPT007B through the GPIO pins on the EXP header. Virtual buttons
 *  on STK's LCD screen indicate whether the capacitive sensing buttons on
 *  CPT007B are pressed or released.
 * This demo supports up to 7 capacitive buttons on CPT007B
 *
 * ----------------------------------------------------------------------------
 * How To Test: EFM32WG STK
 * ----------------------------------------------------------------------------
 * 1) Place the switch in "DBG" mode.
 * 2) Attach CPT007B to STK through EXP headers
 * 3) Connect the EFM32WG STK board to a PC using a mini USB cable.
 * 4) Compile and download code to the EFM32WG STK board.
 *    In Simplicity Studio IDE, select Run -> Debug from the menu bar,
 *    click the Debug button in the quick menu, or press F11.
 * 5) Run the code.
 *    In Simplicity Studio IDE, select Run -> Resume from the menu bar,
 *    click the Resume button in the quick menu, or press F8.
 * 6) The demo program should start with a set of circles on display, by
 *    pressing/releasing capacitive buttons on CPT007B, the circles turns
 *    ON/OFF.
 *
 * Release 0.1 (MR)
 *    - Initial Revision
 *    - 04 Nov 2015
 *****************************************************************************/

#include "cpt007b_config.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "segmentlcd.h"
#include "retargetswo.h"
#include <stdio.h>
#include <string.h>

/*******************************************************************************
 ******************************   DEFINES   ************************************
 ******************************************************************************/

#define NUM_TOP_BLOCKS    3   // Number of capsense inputs/blocks in the top row
#define NUM_BOT_BLOCKS    4   // Number of capsense inputs/blocks in the bottom row

/*******************************************************************************
 ******************************  PROTOTYPES ************************************
 ******************************************************************************/

static void initGPIO(void);
static void updateGPIO(void);
static void updateLCD(uint8_t capsense);
static void updateSWO(void);
static uint8_t getCapsensePrevious(void);
static uint8_t getCapsenseCurrent(void);

/*******************************************************************************
 ******************************   STRUCTS   ************************************
 ******************************************************************************/

/**************************************************************************//**
 * @brief Defines capsense input channel to LCD block mapping
 *****************************************************************************/
typedef struct
{
  uint8_t channel; /**< Capsense Input Channel */
  uint8_t block; /**< LCD Block position */

} Capsense_LCD_Map_TypeDef;

/*******************************************************************************
 ******************************  CONSTANTS  ************************************
 ******************************************************************************/

// Capsense channel to top row block mapping
static const Capsense_LCD_Map_TypeDef topChannelBlocks[NUM_TOP_BLOCKS] =
{
  {5, 1},
  {3, 3},
  {1, 5}
};

// Capsense channel to bottom row block mapping
static const Capsense_LCD_Map_TypeDef botChannelBlocks[NUM_BOT_BLOCKS] =
{
  {6, 0},
  {4, 2},
  {2, 4},
  {0, 6}
};

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  CHIP_Init();

  // initializations
  initGPIO();
  /* Enable LCD without voltage boost */
  SegmentLCD_Init(false);
  setupSWOForPrint();

  updateLCD(getCapsenseCurrent());

  while (1)
  {
    updateGPIO();
    if (getCapsensePrevious() != getCapsenseCurrent())
    {
      updateLCD(getCapsenseCurrent());
      updateSWO();
    }
  }
}

/******************************************************************************
 * @brief GPIO data structure
 * For CPT007B there are 7 capsense outputs, each pin
 * corresponds to a capsenseCurrent bit showing whether a
 * cap-sense button pressed or not, and capsensePrevious
 * bit showing the status of previous cycle.
 *
 * If capsenseCurrent==1 && capsensePrevious==0, detects a "press"
 * If capsenseCurrent==0 && capsensePrevious==1, detects a "release"
 *
 * capsenseCurrent bit-map byte:
 * | N/A | cC6 | cC5 | cC4 | cC3 | cC2 | cC1 | cC0 |
 *
 * capsensePrevious bit-map byte:
 * | N/A | cP6 | cP5 | cP4 | cP3 | cP2 | cP1 | cP0 |
 *
 * *cC,cP are abbreviation for capsenseCurrent and capsensePrevious
 *
 *****************************************************************************/

// GPIO data structure declaration
static uint8_t capsenseCurrent;
static uint8_t capsensePrevious;

/******************************************************************************
 * @brief Return a bitmask containing the current state for all capsense
 * buttons.
 *****************************************************************************/
uint8_t getCapsenseCurrent(void)
{
  return capsenseCurrent;
}

/******************************************************************************
 * @brief Return a bitmask containing the previous state for all capsense
 * buttons.
 *****************************************************************************/
uint8_t getCapsensePrevious(void)
{
  return capsensePrevious;
}

/******************************************************************************
 * @brief GPIO data structure initialization
 *****************************************************************************/
void initGPIO(void)
{
  capsenseCurrent = 0;
  capsensePrevious = 0;

  /* Enable GPIO in CMU */
  CMU_ClockEnable(cmuClock_GPIO, true);

  /* Configure cap-sense input */
  GPIO_PinModeSet(CS0_0_PORT, CS0_0_PIN, gpioModeInput, 0);  // CS00
  GPIO_PinModeSet(CS0_1_PORT, CS0_1_PIN, gpioModeInput, 0);  // CS01
  GPIO_PinModeSet(CS0_2_PORT, CS0_2_PIN, gpioModeInput, 0);  // CS02
  GPIO_PinModeSet(CS0_3_PORT, CS0_3_PIN, gpioModeInput, 0);  // CS03
  GPIO_PinModeSet(CS0_4_PORT, CS0_4_PIN, gpioModeInput, 0);  // CS04
  GPIO_PinModeSet(CS0_5_PORT, CS0_5_PIN, gpioModeInput, 0);  // CS05
  GPIO_PinModeSet(CS0_6_PORT, CS0_6_PIN, gpioModeInput, 0);  // CS06
}

/******************************************************************************
 * @brief update GPIO data structure current level of seven GPIO pins
 *****************************************************************************/
void updateGPIO(void)
{
  // get previous states of Cap-sense button array
  capsensePrevious = capsenseCurrent;

  // update current button states
  capsenseCurrent =  ((GPIO->P[CS0_0_PORT].DIN & (1 << CS0_0_PIN)) ? 0 : CS0_0_PRESENT);
  capsenseCurrent |= ((GPIO->P[CS0_1_PORT].DIN & (1 << CS0_1_PIN)) ? 0 : CS0_1_PRESENT);
  capsenseCurrent |= ((GPIO->P[CS0_2_PORT].DIN & (1 << CS0_2_PIN)) ? 0 : CS0_2_PRESENT);
  capsenseCurrent |= ((GPIO->P[CS0_3_PORT].DIN & (1 << CS0_3_PIN)) ? 0 : CS0_3_PRESENT);
  capsenseCurrent |= ((GPIO->P[CS0_4_PORT].DIN & (1 << CS0_4_PIN)) ? 0 : CS0_4_PRESENT);
  capsenseCurrent |= ((GPIO->P[CS0_5_PORT].DIN & (1 << CS0_5_PIN)) ? 0 : CS0_5_PRESENT);
  capsenseCurrent |= ((GPIO->P[CS0_6_PORT].DIN & (1 << CS0_6_PIN)) ? 0 : CS0_6_PRESENT);
}

/******************************************************************************
 * @brief update capsense status on LCD
 *****************************************************************************/
static void updateLCD(uint8_t capsense)
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

/******************************************************************************
 * @brief update SWO output contents
 *****************************************************************************/
void updateSWO(void)
{
  uint8_t i;
  uint16_t shiftOne = 0x0001;

  for (i = 0; i < TOTAL_CAPSENSE_PIN; i++)
  {
    char tempstr[15];
    if (!(getCapsensePrevious() & shiftOne)
        && (getCapsenseCurrent() & shiftOne))
    {
      sprintf(tempstr, "CS%02u pressed\n", (uint16_t) i);
      printf(tempstr);
    }
    else if ((getCapsensePrevious() & shiftOne)
             && !(getCapsenseCurrent() & shiftOne))
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

