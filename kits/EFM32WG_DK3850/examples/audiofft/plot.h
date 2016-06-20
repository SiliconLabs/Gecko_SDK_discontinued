/**************************************************************************//**
 * @file
 * @brief
 * @version 4.2.0
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

#ifndef PLOT_H
#define PLOT_H

#include <GUI.h>

#if defined(__cplusplus)
extern "C" {
#endif

void PLOT_Plot( float *plotData );
void PLOT_Init( int points );
void PLOT_Puts(const char *str, int xpos, int ypos);

#if defined(__cplusplus)
}
#endif

#endif
