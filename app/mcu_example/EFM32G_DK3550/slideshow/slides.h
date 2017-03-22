/**************************************************************************//**
 * @file
 * @brief TFT Display refresh handling
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

#ifndef __SLIDES_H
#define __SLIDES_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RGB_BUFFER_SIZE    512
#define PALETTE_SIZE       1024

void SLIDES_showError(bool fatal, const char* fmt, ...);
void SLIDES_showBMP(char *fileName);
void SLIDES_init(void);

#ifdef __cplusplus
}
#endif

#endif
