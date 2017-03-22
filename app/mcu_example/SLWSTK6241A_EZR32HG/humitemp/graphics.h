/**************************************************************************//**
 * @file graphics.h
 * @brief Draws the graphics on the display
 * @version 5.1.2
 *******************************************************************************
 * @section License
 * <b>Copyright 2015 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#ifndef __SILICON_LABS_GRAPHICS_H__
#define __SILICON_LABS_GRAPHICS_H__

#include <stdint.h>
#include <stdbool.h>
#include "em_i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DEMO_VERSION "Demo v1.0"

/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/
void GRAPHICS_Init(void);
void GRAPHICS_ShowStatus(bool si7013_status, bool lowBat);
void GRAPHICS_Draw(int32_t tempData, uint32_t rhData, bool lowBat);

#ifdef __cplusplus
}
#endif


#endif /* __SILICON_LABS_GRAPHICS_H__ */
