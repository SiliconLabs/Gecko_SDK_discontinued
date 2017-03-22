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

/* image bitmap file */
#include "out_off.xbm"
#include "out_on.xbm"
#include "out_disable.xbm"
#include "circle.xbm"

#include "display.h"
#include "render.h"
#include "cpt112s_config.h"
#include "cpt112s_i2c.h"
#include <string.h>
#include <stdbool.h>

#define SIZE_X 128
#define SIZE_Y 128

static char frameBuffer[SIZE_X / 8 * SIZE_Y];
static char dirtyLines[32];
static char *dirtyLinesOld = &dirtyLines[0];
static char *dirtyLinesNew = &dirtyLines[16];

/******************************************************************************
 * @brief Draw the Ball on the LCD screen when the slider is used
 *****************************************************************************/

void Render_PrintBall(void)
{
  int x, y;
  unsigned char ballByte;
  char *targetBitmap;
  char *framePointer;
  int ballOffset, frameOffset;

  targetBitmap = circle_bits;

  for (y = 0; y < circle_height; y++)
  {
    for (x = 0; x < circle_width; x++)
    {
      /* Get correct alien byte*/
      ballByte = targetBitmap[(x + (y * circle_width)) / 8];
      ballOffset = x % 8;

      /* Mark bits that are coloured */
      if (ballByte & (1 << ballOffset))
      {
        uint16_t sliderPos = (CPT112S_getSliderCurrent() * (128 - circle_height))
                             / 0x40;
        framePointer = &frameBuffer[((SIZE_X - (sliderPos + circle_height) + x)
                                     + (4 + y) * SIZE_X)
                                    / 8];
        frameOffset = (SIZE_X - (sliderPos + circle_height) + x) % 8;
        *framePointer |= (1 << frameOffset);
      }
    }

    /* Mark line dirty */
    dirtyLinesNew[(4 + y) / 8] |= (1 << ((4 + y) % 8));
  }
}

void RENDER_DrawButton(int posx, int posy, int status)
{
  int x, y;
  int buttonOffset, frameOffset;
  unsigned char buttonByte;
  char *framePointer;
  char *targetBitmap = NULL;

  switch (status)
  {
    // render button OFF
    case 0:
      targetBitmap = out_off_bits;
      break;

      // render button ON
    case 1:
      targetBitmap = out_on_bits;
      break;

      // render button DISABLE
    case 2:
      targetBitmap = out_disable_bits;
      break;

    default:
      return;
  }

  for (y = 0; y < BUTTON_H; y++)
  {
    for (x = 0; x < BUTTON_W; x++)
    {
      /* Get correct sprite byte*/
      buttonByte = targetBitmap[(x + (y * BUTTON_W)) / 8];
      buttonOffset = x % 8;

      /* Mark bits that are coloured */
      if (buttonByte & (1 << buttonOffset))
      {
        framePointer = &frameBuffer[((posx + x) + (posy + y) * SIZE_X) / 8];
        frameOffset = (posx + x) % 8;
        *framePointer |= (1 << frameOffset);
      }
    }

    /* Mark line dirty */
    dirtyLinesNew[(posy + y) / 8] |= (1 << ((posy + y) % 8));
  }
}

void RENDER_ClearFramebufferArea(int xstart,
                                 int ystart,
                                 int xend,
                                 int yend,
                                 int color)
{
  int x, y;
  char *framePointer;
  int frameOffset;

  for (y = ystart; y < yend; y++)
  {
    for (x = xstart; x < xend; x++)
    {
      framePointer = &frameBuffer[(x + y * SIZE_X) / 8];
      frameOffset = x % 8;
      if (color)
      {
        *framePointer |= (1 << frameOffset);
      }
      else
      {
        *framePointer &= ~(1 << frameOffset);
      }

    }

    /* Mark line dirty */
    dirtyLinesNew[y / 8] |= (1 << (y % 8));
  }

}

void RENDER_UpdateDisplay(bool fullUpdate, DISPLAY_Device_t* displayDevice)
{
  char *tmp;
  int startLine, lineCount = 0;
  int i;

  if (fullUpdate)
  {
    /* Update entire display */
    displayDevice->pPixelMatrixDraw(displayDevice,
                                    frameBuffer,
                                    0,
                                    displayDevice->geometry.width,
                                    0,
                                    SIZE_Y);
  }
  else
  {
    /* Only update lines that are dirty */
    for (i = 0; i < SIZE_Y; i++)
    {
      if ((dirtyLinesNew[i / 8] & (1 << (i % 8)))
          || (dirtyLinesOld[i / 8] & (1 << (i % 8))))
      {
        lineCount += 1;
      }
      else if (lineCount > 0)
      {
        startLine = i - lineCount;
        displayDevice->pPixelMatrixDraw(displayDevice,
                                        &frameBuffer[startLine * SIZE_X / 8],
                                        0,
                                        displayDevice->geometry.width,
                                        startLine,
                                        lineCount);
        lineCount = 0;
      }
    }

    /* We reached the last line, any updates left? */
    if (lineCount > 0)
    {
      startLine = SIZE_Y - lineCount;
      displayDevice->pPixelMatrixDraw(displayDevice,
                                      &frameBuffer[startLine * SIZE_X / 8],
                                      0,
                                      displayDevice->geometry.width,
                                      startLine,
                                      lineCount);
    }

    /* Swap old and new dirty buffers */
    tmp = dirtyLinesNew;
    dirtyLinesNew = dirtyLinesOld;
    dirtyLinesOld = tmp;

    /* Clear new dirty lines buffer */
    for (i = 0; i < SIZE_Y / 8; i++)
    {
      dirtyLinesNew[i] = 0x00;
    }
  }
}
