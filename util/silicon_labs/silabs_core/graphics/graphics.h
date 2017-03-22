/**************************************************************************//**
 * @brief Draws the graphics on the display
 * @version 3.20.5
 *******************************************************************************
 * @section License
 * <b>(C) Copyright 2015 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#ifndef __GRAPHICS_H
#define __GRAPHICS_H

#include <stdint.h>
#include <stdbool.h>
#include "glib.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/
void GRAPHICS_Init(void);
void GRAPHICS_Sleep(void);
void GRAPHICS_Wakeup(void);
void GRAPHICS_Update(void);
void GRAPHICS_AppendString(char *str);
void GRAPHICS_Clear(void);
void GRAPHICS_InsertTriangle(uint32_t x,
                             uint32_t y,
                             uint32_t size,
                             bool up,
                             int8_t fillPercent);

#ifdef __cplusplus
}
#endif


#endif /* __GRAHPHICS_H */
