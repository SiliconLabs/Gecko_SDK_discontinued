/**************************************************************************//**
 * @file
 * @brief helper functions for drawing capsense button states on the LCD
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

#include "cpt112s_config.h"
#include "cpt112s_i2c.h"
#include "render.h"
#include "lcd.h"
#include "em_device.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static DISPLAY_Device_t displayDevice;
#define		slider_pressed		0xFFFF
#define DISPLAY_DEVICE_NO       (0)

/******************************************************************************
 * @brief Button display definitions
 *****************************************************************************/
static const int buttons_x_pos[] =
{0, 32, 64, 96};
static const int buttons_y_pos[] =
{32, 64, 96};
static DemoObject buttons[N_BUTTONS];

/******************************************************************************
 * @brief Initiates a Demo Button Object
 *****************************************************************************/
static void initDemoObject(DemoObject *newObj, int x, int y)
{
  newObj->posX = x;
  newObj->posY = y;
}

#if !defined(__CROSSWORKS_ARM) && defined(__GNUC__)
/* sniprintf does not process floats, but occupies less flash memory ! */
#define snprintf sniprintf
#endif

/******************************************************************************
 * @brief Redraws all the demo button objects into the framebuffer.
 *****************************************************************************/
void DEMO_Redraw(void)
{
  unsigned int i;
  uint8_t row, col;

  // update OUT[OUT_ROWS][OUT_COLS] data structure
  for (row = 0; row < N_ROWS; row++)
  {
    for (col = 0; col < N_COLS; col++)
    {
      // get current Cap-sense Button number
      uint8_t curButton = col + row * N_COLS;
      if (curButton >= TOTAL_CAPSENSE_PIN - 1)
      {
        // check for unused sprites, make them to disabled state
        buttons[curButton].status = 2;
      }
      else if (CPT112S_getCapsenseCurrent() & (0x01 << curButton))
      {
        // check if it's being pressed, if so set to ON state
        buttons[curButton].status = 1;
      }
      else
      {
        // if not being pressed, set to OFF state
        buttons[curButton].status = 0;
      }
    }
  }

  for (i = 4; i < TOTAL_CAPSENSE_PIN; i++)
  {
    RENDER_DrawButton(buttons[i].posX, buttons[i].posY, buttons[i].status);
  }

  if (CPT112S_getSliderCurrent() != slider_pressed) // on slider press
  {
    Render_PrintBall();
  }
}

/**************************************************************************//**
 * @brief Initiates a new demo with buttons
 *****************************************************************************/
void DEMO_Init(void)
{
  unsigned int i, j, n;

  /* Create all buttons */
  for (i = 1; i < N_ROWS; i++)
  {
    for (j = 0; j < N_COLS; j++)
    {
      n = i * N_COLS + j;
      initDemoObject(&buttons[n], buttons_x_pos[j], buttons_y_pos[i]);
    }
  }
}

/******************************************************************************
 * @brief Initiate the demo display
 *****************************************************************************/
void initLCD(void)
{
  EMSTATUS status;

  /* Initialize the DISPLAY driver. */
  DISPLAY_Init();

  RENDER_ClearFramebufferArea(0, 0, 127, 127, 0);

  /* Retrieve the properties of the DISPLAY. */
  status = DISPLAY_DeviceGet(DISPLAY_DEVICE_NO, &displayDevice);
  while (DISPLAY_EMSTATUS_OK != status);

  DEMO_Init();
}

/******************************************************************************
 * @brief The display update loop
 *****************************************************************************/
void updateLCD(void)
{

  DEMO_Redraw();
  // set to only update dirty lines
  RENDER_UpdateDisplay(false, &displayDevice);
  // clear buffer to ready for next redraw
  RENDER_ClearFramebufferArea(0, 0, 127, 127, 0);
}

