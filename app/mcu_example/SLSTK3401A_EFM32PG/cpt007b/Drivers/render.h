/**************************************************************************//**
 * @file
 * @brief helper functions for rendering capsense button states on the LCD
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

#ifndef RENDER_H
#define RENDER_H

#include "display.h"
#include <stdbool.h>

/* lcd display dimensions */
#define N_COLS                        2
#define N_ROWS                        4
#define N_BUTTONS                     (N_COLS * N_ROWS)
#define BUTTON_H                      out_off_height
#define BUTTON_W                      out_off_width
#define TOTAL_OUT_HEIGHT              (BUTTON_H * OUT_ROWS)
#define TOTAL_OUT_WIDTH               (BUTTON_W * OUT_COLS)

void RENDER_DrawButton( int posx, int posy, int status );
void RENDER_ClearFramebufferArea( int xstart, int ystart, int xend, int yend, int color );
void RENDER_UpdateDisplay( bool fullUpdate, DISPLAY_Device_t* displayDevice );

#endif /* RENDER_H */
