/**************************************************************************//**
 * @file game.h
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

#ifndef _GAME_H
#define _GAME_H

#define SCREEN_BUFF_SIZE 2048

#include <stdbool.h>
#include "render.h"

#define GAME_VICTORY    2
#define GAME_OVER       1
#define GAME_RUNNING    0



typedef struct _gameobject {
    const Sprite *sprite;
    int  posX;
    int  posY;
    bool dead;
} GameObject;


void GAME_Redraw( void );
void GAME_Init( int lvl );
void GAME_MoveTank( int speed );
int  GAME_Update( void );
void GAME_FirePlayerMissile( void );

#endif /* _GAME_H */
