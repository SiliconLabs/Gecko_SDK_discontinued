/**************************************************************************//**
 * @file render.h
 * @brief Graphic render functions for spaceinvaders game.
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

#ifndef __RENDER_H__
#define __RENDER_H__

#include "stdbool.h"

#include "display.h"

/* Total number of sprites */
#define N_SPRITES       9

/* Number of sprites of each type */
#define N_ALIENS_SPRITES   5
#define N_MISSILES_SPRITES 3
#define N_TANK_SPRITES     1

/* Alien widths and heights */
#define ALIEN_0_W       11
#define ALIEN_0_H       8

#define ALIEN_1_W       12
#define ALIEN_1_H       8

#define ALIEN_2_W       8
#define ALIEN_2_H       8

#define ALIEN_3_W       11
#define ALIEN_3_H       8

#define ALIEN_4_W       12
#define ALIEN_4_H       8

/* Missile widths and heights */
#define MISSILE_0_W     1
#define MISSILE_0_H     4

#define MISSILE_1_W     3
#define MISSILE_1_H     7

#define MISSILE_2_W     3
#define MISSILE_2_H     7

/* Tank width and height */
#define TANK_W          13
#define TANK_H          8


#define SPRITE_ALIEN_0          \
{                               \
  0,                            \
  ALIEN_0_W,                    \
  ALIEN_0_H                     \
}

#define SPRITE_ALIEN_1          \
{                               \
  1,                            \
  ALIEN_1_W,                    \
  ALIEN_1_H                     \
}

#define SPRITE_ALIEN_2          \
{                               \
  2,                            \
  ALIEN_2_W,                    \
  ALIEN_2_H                     \
}

#define SPRITE_ALIEN_3          \
{                               \
  3,                            \
  ALIEN_3_W,                    \
  ALIEN_3_H                     \
}

#define SPRITE_ALIEN_4          \
{                               \
  4,                            \
  ALIEN_4_W,                    \
  ALIEN_4_H                     \
}

#define SPRITE_MISSILE_0        \
{                               \
  5,                            \
  MISSILE_0_W,                  \
  MISSILE_0_H                   \
}

#define SPRITE_MISSILE_1        \
{                               \
  6,                            \
  MISSILE_1_W,                  \
  MISSILE_1_H                   \
}

#define SPRITE_MISSILE_2        \
{                               \
  7,                            \
  MISSILE_2_W,                  \
  MISSILE_2_H                   \
}

#define SPRITE_TANK             \
{                               \
  8,                            \
  TANK_W,                       \
  TANK_H                        \
}

typedef struct _sprite {
  int index;
  int width;
  int height;
} Sprite;

extern const Sprite sprites[];

extern const Sprite *s_aliens;
extern const Sprite *s_missiles;
extern const Sprite *s_tank;


void RENDER_SetFontColor(int blackFont);
void RENDER_SetFontResizeFactor(int factor);
void RENDER_Write(int posx, int posy, char *str);
void RENDER_DrawSprite(int posx, int posy, const Sprite *sp);
void RENDER_ClearFramebufferLines(int firstLine, int lineCount);
void RENDER_ClearFramebufferArea(int xstart, int ystart, int xend, int yend, int color);
void RENDER_SetFramebuffer(const unsigned char *img);
void RENDER_DrawBackdrop(void);
void RENDER_UpdateDisplay(bool fullUpdate, DISPLAY_Device_t* displayDevice);

#endif /* __RENDER_H__ */
