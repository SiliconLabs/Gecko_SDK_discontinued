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
#include "display.h"
#include "render.h"
#include <string.h>
#include <stdbool.h>


#define SIZE_X 128
#define SIZE_Y 128

static char frameBuffer[SIZE_X/8*SIZE_Y];
static char dirtyLines[32];
static char *dirtyLinesOld = &dirtyLines[0];
static char *dirtyLinesNew = &dirtyLines[16];


void RENDER_ClearFramebufferArea( int xstart, int ystart, int xend, int yend, int color )
{
   int x, y;
   char *framePointer;
   int frameOffset;

   for (y = ystart; y < yend; y++)
   {
      for (x = xstart; x < xend; x++)
      {
         framePointer = &frameBuffer[(x + y * SIZE_X)/8];
         frameOffset  = x % 8;
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
      dirtyLinesNew[y/8] |= (1 << (y % 8));
   }
}


void RENDER_DrawButton( int posx, int posy, int status)
{
  int x, y;
  int buttonOffset, frameOffset;
  unsigned char buttonByte;
  char *framePointer;
  char *targetBitmap = NULL;

  switch(status)
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

    // button N/A
    case 3:
    // error if falls in this case with current firmware version
    break;

    default:
    ;// do nothing
  }

  for (y = 0; y < BUTTON_H; y++)
  {
    for (x = 0; x < BUTTON_W; x++)
    {
      /* Get correct sprite byte*/
      buttonByte   = targetBitmap[ (x + (y * BUTTON_W)) / 8];
      buttonOffset = x % 8;

      /* Mark bits that are coloured */
      if (buttonByte & (1 << buttonOffset))
      {
        framePointer = &frameBuffer[((posx + x) + (posy + y)*SIZE_X)/8];
        frameOffset  = (posx + x) % 8;
        *framePointer |= (1 << frameOffset);
      }
    }

    /* Mark line dirty */
    dirtyLinesNew[(posy + y)/8] |= (1 << ((posy + y) % 8));
  }
}


void RENDER_UpdateDisplay( bool fullUpdate, DISPLAY_Device_t* displayDevice )
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
       if ((dirtyLinesNew[i/8] & (1 << (i % 8))) || (dirtyLinesOld[i/8] & (1 << (i % 8))))
       {
         lineCount += 1;
       }
       else if (lineCount > 0)
       {
         startLine = i - lineCount;
         displayDevice->pPixelMatrixDraw(displayDevice,
                                         &frameBuffer[startLine*SIZE_X/8],
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
                                       &frameBuffer[startLine*SIZE_X/8],
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
     for (i = 0; i < SIZE_Y/8; i++)
     {
       dirtyLinesNew[i] = 0x00;
     }
  }
}
