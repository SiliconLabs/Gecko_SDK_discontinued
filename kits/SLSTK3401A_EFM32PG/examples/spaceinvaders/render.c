/**************************************************************************//**
 * @file render.c
 * @brief Graphic render functions for spaceinvaders game.
 * @version 4.2.1
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

#include <string.h>

#include "img/thinfont.xbm"
#include "img/aliens.xbm"
#include "img/backdrop.xbm"
#include "img/splash.xbm"

#include "display.h"
#include "render.h"

#define SIZE_X 128
#define SIZE_Y 128

#define FONT_WIDTH  6
#define FONT_HEIGHT 8

#define ALIEN_WIDTH  12
#define ALIEN_HEIGHT 8

static char frameBuffer[SIZE_X/8*SIZE_Y];
static char dirtyLines[32];
static char *dirtyLinesOld = &dirtyLines[0];
static char *dirtyLinesNew = &dirtyLines[16];

static int fontColor = 0;
static int fontSize  = 1;

const Sprite sprites[] = {
  SPRITE_ALIEN_0,
  SPRITE_ALIEN_1,
  SPRITE_ALIEN_2,
  SPRITE_ALIEN_3,
  SPRITE_ALIEN_4,
  SPRITE_MISSILE_0,
  SPRITE_MISSILE_1,
  SPRITE_MISSILE_2,
  SPRITE_TANK
};

const Sprite *s_aliens   = &sprites[0];
const Sprite *s_missiles = &sprites[5];
const Sprite *s_tank     = &sprites[8];

void RENDER_SetFontColor(int blackFont)
{
   fontColor = blackFont;
}

void RENDER_SetFontResizeFactor(int factor)
{
   fontSize = factor;
}

void RENDER_Write(int posx, int posy, char *str)
{
  int i;
  int x, y;
  char c;
  unsigned char font_byte;
  char *framePointer;
  int offset;

  i = 0;
  while (str[i] != '\0')
  {
    /* Check for non-printable characters */
    c = str[i];
    if (c < ' ' || c > '~')
    {
      c = ' ';
    }

    /* Update each line of character */
    for (y = 0; y < FONT_HEIGHT*fontSize; y++)
    {
      /* Each line occupies a specific byte */
      font_byte = thinfont_bits[(c - ' ') + thinfont_width/8 * (y/fontSize)];

      /* Update each bit of the character line */
      for (x = 0; x < FONT_WIDTH*fontSize; x++)
      {
        if (font_byte & (1 << (x/fontSize)))
        {
          framePointer = &frameBuffer[((posx + x) + (posy + y)*SIZE_X)/8];
          offset = (posx + x) % 8;
          if (fontColor)
            *framePointer &= ~(1 << offset);
          else
            *framePointer |= (1 << offset);

        }
      }

      /* Mark line dirty */
      dirtyLinesNew[(posy + y)/8] |= (1 << ((posy + y) % 8));
    }

    i++;
    posx += FONT_WIDTH*fontSize;
  }
}

void RENDER_DrawSprite(int posx, int posy, const Sprite *sp)
{
  int x, y;
  int alienOffset, frameOffset;
  unsigned char alienByte;
  char *framePointer;

  for (y = 0; y < sp->height; y++)
  {
    for (x = 0; x < sp->width; x++)
    {
      /* Get correct alien byte*/
      alienByte   = aliens_bits[(sp->index*16 + x)/8 + y*aliens_width/8];
      alienOffset = x % 8;

      /* Mark bits that are coloured */
      if (alienByte & (1 << alienOffset))
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


void RENDER_ClearFramebufferArea(int xstart, int ystart, int xend, int yend, int color)
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


void RENDER_ClearFramebufferLines(int firstLine, int lineCount)
{
   unsigned int start  = firstLine * SIZE_X / 8;
   unsigned int length = lineCount * SIZE_X / 8;

   if((length <= 0) || ((start + length) > sizeof(frameBuffer)))
   {
      return;
   }

   memset(&frameBuffer[start], 0, length);
}


void RENDER_SetFramebuffer(const unsigned char *img)
{
   /* Copy image into the framebuffer */
   memcpy(frameBuffer, img, (SIZE_X*SIZE_Y/8));
}


void RENDER_DrawBackdrop(void)
{
   /* Copy backdrop into the framebuffer */
   RENDER_SetFramebuffer(backdrop);
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
