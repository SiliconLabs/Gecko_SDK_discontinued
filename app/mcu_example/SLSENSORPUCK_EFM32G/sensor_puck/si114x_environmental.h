/**************************************************************************//**
* @file
* @brief Si114x Environmental mode functions

* @version 5.1.2

******************************************************************************
* @section License
* <b>Copyright 2015 Silicon Labs, Inc. http://www.silabs.com</b>
*******************************************************************************
*
* This file is licensensed under the Silabs License Agreement. See the file
* "Silabs_License_Agreement.txt" for details. Before using this software for
* any purpose, you must agree to the terms of that agreement.
*
******************************************************************************/
#ifndef __SI114x_ENVIRONMENTAL_H
#define __SI114x_ENVIRONMENTAL_H

int Si114x_MeasureEnvironmental(HANDLE si114x_handle, uint16_t *uvIndex, uint16_t *ps1, uint16_t *als);

int Si114x_ConfigureEnvironmental(HANDLE si114x_handle);

int Si114x_ConfigureHRM(HANDLE si114x_handle);

void Si114x_NoUV(void);

void Si114x_MeasureDarkOffset(HANDLE si114x_handle);
#endif
