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

#include "cpt007b_config.h"
#include "gpio.h"
#include "lcd.h"
#include "render.h"
#include "displaypal.h"
#include "em_device.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define DISPLAY_DEVICE_NO       (0)

static DISPLAY_Device_t displayDevice;
static const int buttons_x_pos[] = {96, 80, 64, 48, 32, 16, 0 };
static const int buttons_y_pos[] = {  64, 32 };
static DemoObject buttons[N_BUTTONS];

/******************************************************************************
 * @brief Initiates a Demo Button Object
 *****************************************************************************/
static void initDemoObject( DemoObject *newObj, int x, int y)
{
  newObj->posX = x;
  newObj->posY = y;
}

/**************************************************************************//**
 * @brief Initiates a new demo with buttons
 *****************************************************************************/
static void DEMO_Init( void )
{
  unsigned int i, j, n;
  unsigned int x = 0; //x position counter for buttons
  unsigned int y = 0; //y position counter for buttons

  /* Create all buttons */
  /* CS00, CS022, CS04 and CS06 go to the bottom row */
  /*CS01, CS03, CS05 go to the top row */
  for (i = 0; i < N_COLS; i++)
  {
    for (j = 0; j < N_ROWS ; j++)
    {
      n = i * N_ROWS + j;
      if (n % 2 == 0){
    	  initDemoObject( &buttons[n], buttons_x_pos[x], buttons_y_pos[y]);
    	  y++;
      }
      else{
    	  initDemoObject( &buttons[n], buttons_x_pos[x], buttons_y_pos[y]);
    	  y = 0;
      }
      x++;
    }
  }
}

/******************************************************************************
 * @brief Redraws all the demo button objects into the framebuffer.
 *****************************************************************************/
static void DEMO_Redraw( void )
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
      if (curButton >= TOTAL_CAPSENSE_PIN)
      {
        // check for unused sprites, make them to disabled state
        buttons[curButton].status = 2;
      }
      else if (getCapsenseCurrent() & (0x01 << curButton))
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

  for (i = 0; i < TOTAL_CAPSENSE_PIN; i++)
  {
    RENDER_DrawButton( buttons[i].posX, buttons[i].posY, buttons[i].status );
  }
}

/******************************************************************************
 * @brief Initiate the demo display
 *****************************************************************************/
void initLCD( void )
{
   EMSTATUS status;

   /* Initialize the DISPLAY driver. */
   DISPLAY_Init();
   RENDER_ClearFramebufferArea( 0, 0, 128, 128, 0 );

   /* Retrieve the properties of the DISPLAY. */
   status = DISPLAY_DeviceGet(DISPLAY_DEVICE_NO, &displayDevice);
   while (DISPLAY_EMSTATUS_OK != status){;}

   DEMO_Init();
}

/******************************************************************************
 * @brief The display update loop
 *****************************************************************************/
void updateLCD( void )
{
  DEMO_Redraw();
  // set to only update dirty lines
  RENDER_UpdateDisplay( false, &displayDevice );
  // clear buffer to ready for next redraw
  RENDER_ClearFramebufferArea( 0, 0, 128, 128, 0 );
}
