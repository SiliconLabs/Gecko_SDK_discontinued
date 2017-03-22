/**************************************************************************//**
 * @file  preamp.h
 * @brief Preamp example project definitions.
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

#ifndef PREAMP_H
#define PREAMP_H

#define USE_GUI           /* Use emWin library to make nice gui. */
#define BALANCE_MAX       10
#define BALANCE_CENTER    5
#define TONE_MAX          10
#define TONE_CENTER       5

extern int volume, balance, bass, treble;
extern const float volumeTable[];

#ifdef USE_GUI
extern const float volumeTable_dB[];
extern const float balanceTable_dB[];
#endif

#endif
