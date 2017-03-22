/**************************************************************************//**
 * @file   plot.h
 * @brief  Simple wrapper for some emWin functions.
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

#ifndef PLOT_H
#define PLOT_H

#include <GUI.h>

#if defined(__cplusplus)
extern "C" {
#endif

void PLOT_Bar( int height, int lowerRightXpos, int lowerYpos, int width, int maxHeight );
void PLOT_BarSolid( int height, int lowerRightXpos, int lowerYpos, int width, int maxHeight );
void PLOT_DisplayInit( void );
void PLOT_DisplayUpdate( void );
void PLOT_Init( void );
void PLOT_Puts(const char *str, int xpos, int ypos);
void PLOT_PutsCentered(const char *str, int xpos, int ypos);
void PLOT_ToneBar( int level, int lowerRightXpos );
void PLOT_VolumeBar( float level, int lowerRightXpos );

#if defined(__cplusplus)
}
#endif

#endif
