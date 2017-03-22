/**************************************************************************//**
 * @file game.c
 * @brief Spaceinvaders game.
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

#include <stdlib.h>
#include <string.h>
#include "render.h"
#include "game.h"

#define SCREEN_SIZE_X   128
#define SCREEN_SIZE_Y   128

#define N_COLS   (sizeof(aliens_x_pos)/sizeof(aliens_x_pos[0]))
#define N_ROWS   (sizeof(aliens_y_pos)/sizeof(aliens_y_pos[0]))
#define N_ALIENS (N_COLS * N_ROWS)

static const int aliens_x_pos[] = { 10, 30, 50, 70, 90 };
static const int aliens_y_pos[] = { 15, 25, 35 };
static const int aliens_sprites[N_ROWS][N_COLS] = { {0, 3, 0, 3, 0},
                                                  {1, 4, 1, 4, 1},
                                                  {2, 2, 2, 2, 2}};

static GameObject aliens[N_ALIENS];
static GameObject alienMissile;
static GameObject playerMissile;
static GameObject tank;

static int alienSpeedX = 1;
static int alienSpeedY = 4;

static int level = 1;
static int score = 0;

static int animationWaitCount = 0;
static int alienMissileWaitCount = 0;
static int alienMoveWaitCount = 0;
static int alienMissileAnimationCount = 0;

static int alienMoveThres    = 10;
static int alienMissileThres = 10;


/* Local function declarations */
static void initGameObject( GameObject *newObj, const Sprite *sp, int x, int y );
static void fireAlienMissile( void );
static bool onScreen( GameObject *obj );
static void updateMissile( GameObject *missile, int speed );
static bool missileCollision( GameObject *missile, GameObject *target );
static int  getLowestAlien( void );
static void animateGameObject( GameObject *obj );

/**************************************************************************//**
 * @brief Initiates a Game Object
 *        These includes aliens, missiles and the tank.
 *****************************************************************************/
static void initGameObject( GameObject *newObj, const Sprite *sp, int x, int y )
{
  newObj->sprite = sp;
  newObj->posX = x;
  newObj->posY = y;
  newObj->dead = false;
}

/**************************************************************************//**
 * @brief Fire the alien missile
 *        This function chooses a random alien to be the origin of a new
 *        alien missile, given that the alien misssile is not already on
 *        the screen.
 *****************************************************************************/
static void fireAlienMissile( void )
{
  int origin;

  if (alienMissile.dead == true)
  {
    origin = rand() * N_ALIENS / RAND_MAX;

    /* Make sure alien is not dead */
    while (aliens[origin].dead)
    {
      origin = (origin + 1) % N_ALIENS;
    }

    alienMissile.sprite = &s_missiles[1];
    alienMissile.posX = aliens[origin].posX + aliens[origin].sprite->width/2;
    alienMissile.posY = aliens[origin].posY + aliens[origin].sprite->height + 1;
    alienMissile.dead = false;
  }
}

/**************************************************************************//**
 * @brief Check if a game object is on screen
 *        This function returns true if an object is found to be completely
 *        on the screen.
 *****************************************************************************/
static bool onScreen( GameObject *obj )
{
  return ((obj->posX >= 0 && (obj->posX + obj->sprite->width) < 128) &&
          (obj->posY >= 0 && (obj->posY + obj->sprite->height)< 128));
}

/**************************************************************************//**
 * @brief Updates missile position and removes it if off screen.
 *****************************************************************************/
static void updateMissile( GameObject *missile, int speed )
{
  /* Update missile position */
  missile->posY += speed;

  /* Check if off screen */
  if (missile->dead == false)
  {
    if (!onScreen(missile))
    {
      missile->dead = true;
      /* We missed! deduct points */
      if (missile == &playerMissile && score > 10)
      {
        score -= 10;
      }
    }
  }
}

/**************************************************************************//**
 * @brief Check if a missile collides with a given target.
 *        This function returns true if the missile hits the target
 *        specified.
 *****************************************************************************/
static bool missileCollision( GameObject *missile, GameObject *target )
{
  /* No colissions with dead aliens */
  if (target->dead) return false;

  /* Check if x-coordinates are within */
  if ((missile->posX + missile->sprite->width) >= target->posX &&
      (missile->posX) < (target->posX + target->sprite->width))
  {
    /* Check y-coordinate */
    if (missile->posY <= (target->posY + target->sprite->height) &&
        (missile->posY + missile->sprite->height) > target->posY)
    {
      return true;
    }
  }

  return false;
}

/**************************************************************************//**
 * @brief Returns the index of the Alien closest to the ground
 *****************************************************************************/
static int getLowestAlien( void )
{
  int ymax = 0;
  unsigned int i;

  for (i = 0; i < N_ALIENS; i++)
  {
    if (aliens[i].dead) continue;

    if ((aliens[i].posY + aliens[i].sprite->height) > ymax)
    {
      ymax = (aliens[i].posY + aliens[i].sprite->height);
    }
  }

  return ymax;
}

/**************************************************************************//**
 * @brief Animates aliens and missiles
 *        This function sets the next sprite based on the current sprite
 *        for animated game objects.
 *****************************************************************************/
static void animateGameObject( GameObject *obj )
{
   /* Do not animate if not alive */
   if (obj->dead) return;

   if (obj->sprite == &s_aliens[0])
   {
      obj->sprite = &s_aliens[3];
   }
   else if (obj->sprite == &s_aliens[3])
   {
      obj->sprite = &s_aliens[0];
   }
   else if (obj->sprite == &s_aliens[1])
   {
      obj->sprite = &s_aliens[4];
   }
   else if (obj->sprite == &s_aliens[4])
   {
      obj->sprite = &s_aliens[1];
   }
   else if (obj->sprite == &s_missiles[1])
   {
      obj->sprite = &s_missiles[2];
   }
   else if (obj->sprite == &s_missiles[2])
   {
      obj->sprite = &s_missiles[1];
   }

}

/**************************************************************************//**
 * @brief Set the difficulty based on the level
 *        This function sets game parameters that control the game
 *        difficulty based on the current level.
 *****************************************************************************/
static void setDifficulty( int level )
{
   switch (level)
   {
   case 1:
      alienMissileThres = 100;
      alienMoveThres    = 6;
      break;

   case 2:
      alienMissileThres = 75;
      alienMoveThres    = 4;
      break;

   case 3:
      alienMissileThres = 50;
      alienMoveThres    = 2;
      break;

   case 4:
      alienMissileThres = 25;
      alienMoveThres    = 2;
      break;

   case 5:
      alienMissileThres = 10;
      alienMoveThres    = 2;
      break;

   case 6:
      alienMissileThres = 10;
      alienMoveThres    = 1;

   case 7:
      alienMissileThres = 5;
      alienMoveThres    = 1;

   default:
      alienMissileThres = 5;
      alienMoveThres    = 0;
      break;
   }
}


/**************************************************************************//**
 * @brief Update the tank position.
 *****************************************************************************/
void GAME_MoveTank( int speed )
{
  if (!tank.dead)
  {
    if ((tank.posX + speed) >= 0 && (tank.posX + tank.sprite->width + speed) < 128)
    {
      tank.posX += speed;
    }
  }
}

/**************************************************************************//**
 * @brief Fire the Player's missile, if not already on screen.
 *****************************************************************************/
void GAME_FirePlayerMissile( void )
{
  /* Check if missile is already on screen */
  if (playerMissile.dead == true)
  {
    playerMissile.posX = tank.posX + tank.sprite->width/2;
    playerMissile.posY = tank.posY - 1;
    playerMissile.dead = false;
  }
}

/***************************************************************************//**
 * @brief Converts a 32 bit value to a readable ASCII string.
 ******************************************************************************/
static void ConvertHexToString(char* str, int strLen, uint32_t val,
                               bool stripLeadingZeros)
{
  int digit = strLen-1;

  /* NULL terminate string first. */
  str[strLen-1] = 0;

  while (digit)
  {
    str[--digit] = (val % 10) + '0';
    val /= 10;
  }
  if (stripLeadingZeros)
  {
    for (digit=0; '0'==str[digit]; digit++);
    if (digit)
    {
      int numDigits = strLen-1-digit;
      if (0==numDigits)
      {
        /* Value is zero and we need to copy one zero '0' character to the
           first byte of the string, and NULL terminate. */
        numDigits = 1;
        digit--;
      }
      memcpy(str, &str[digit], numDigits);
      str[numDigits] = 0;
    }
  }
}


/**************************************************************************//**
 * @brief Redraws all the game objects into the framebuffer.
 *****************************************************************************/
void GAME_Redraw( void )
{
  unsigned int i;
  char num[5];

  /* Draw background */
  RENDER_DrawBackdrop();

  /* Update score and level */
  RENDER_Write( 2,  2, "LEVEL" );
  ConvertHexToString(num, sizeof(num), level, true);
  RENDER_Write( 36, 2, num );

  RENDER_Write( 61, 2, "SCORE:" );
  ConvertHexToString(num, sizeof(num), score, false);
  RENDER_Write( 103, 2, num );

  /* Draw aliens */
  for (i = 0; i < N_ALIENS; i++)
  {
    if (!aliens[i].dead)
    {
      RENDER_DrawSprite( aliens[i].posX, aliens[i].posY, aliens[i].sprite );
    }
  }

  /* Draw missiles */
  if (!playerMissile.dead)
  {
    RENDER_DrawSprite( playerMissile.posX, playerMissile.posY, playerMissile.sprite );
  }

  if (!alienMissile.dead)
  {
    RENDER_DrawSprite( alienMissile.posX, alienMissile.posY, alienMissile.sprite );
  }

  /* Draw tank */
  RENDER_DrawSprite( tank.posX, tank.posY, tank.sprite );
}



/**************************************************************************//**
 * @brief Updates the game state
 *        This function returns an integer depending on wether the game
 *        is still running, if it is over, or if we can proceed to the
 *        next level.
 *****************************************************************************/
int GAME_Update( void )
{
  unsigned int i;
  int deadCount;

  /* Move aliens */
  if (++alienMoveWaitCount > alienMoveThres)
  {
    alienMoveWaitCount = 0;

    for (i = 0; i < N_ALIENS; i++)
    {
      aliens[i].posX += alienSpeedX;
    }

    if ((aliens[N_ALIENS-1].posX >= (128 - 12)) || (aliens[0].posX <= 0))
    {
      alienSpeedX *= -1;

      for (i = 0; i < N_ALIENS; i++)
      {
        aliens[i].posY += alienSpeedY;
      }
    }
  }


  /* Animate aliens */
  if (++animationWaitCount == 10)
  {
    animationWaitCount = 0;

    for (i = 0; i < N_ALIENS; i++)
    {
      animateGameObject(&aliens[i]);
    }

  }

  if (++alienMissileAnimationCount == 2)
  {
     alienMissileAnimationCount = 0;
     animateGameObject(&alienMissile);
  }

  /* Update missiles */
  if (!playerMissile.dead)
  {
    updateMissile( &playerMissile, -2 );

    /* Check collision with aliens */
    for (i = 0; i < N_ALIENS; i++)
    {
      if (missileCollision( &playerMissile, &aliens[i] ))
      {
        playerMissile.dead = true;
        aliens[i].dead = true;
        score += 20;
        break;
      }
    }
  }

  if (alienMissile.dead)
  {
    /* Fire missile from random alien */
    if (++alienMissileWaitCount > alienMissileThres)
    {
      fireAlienMissile();
      alienMissileWaitCount = 0;
    }
  }
  else
  {
    updateMissile( &alienMissile, 1 );

    if (missileCollision( &alienMissile, &tank ))
    {
      return GAME_OVER;
    }
  }

  /* Check victory conditions */
  deadCount = 0;
  for (i = 0; i < N_ALIENS; i++)
  {
    if (aliens[i].dead) deadCount++;
  }

  if (deadCount == N_ALIENS)
  {
    return GAME_VICTORY;
  }
  else if (getLowestAlien() > tank.posY)
  {
    return GAME_OVER;
  }
  else return GAME_RUNNING;
}


/**************************************************************************//**
 * @brief Initiates a new game (level)
 *****************************************************************************/
void GAME_Init( int lvl )
{
  unsigned int i, j, n;

  /* Create all aliens */
  for (i = 0; i < N_ROWS; i++)
  {
    for (j = 0; j < N_COLS; j++)
    {
      n = i * N_COLS + j;
      initGameObject( &aliens[n], &s_aliens[aliens_sprites[i][j]], aliens_x_pos[j], aliens_y_pos[i] );
    }
  }

  /* Create missiles */
  initGameObject( &playerMissile, &s_missiles[0], 0, 0 );
  playerMissile.dead = true;

  initGameObject( &alienMissile, &s_missiles[1], 0, 0 );
  alienMissile.dead = true;

  /* Create tank */
  initGameObject( &tank, s_tank, 57, 117 );

  /* Set difficulty */
  level = lvl;
  setDifficulty( level );

  /* Reset score upon new game (level 1 init) */
  if (level == 1)
  {
    score = 0;
  }

  animationWaitCount = 0;
  alienMissileWaitCount = 0;
  alienMoveWaitCount = 0;
}
