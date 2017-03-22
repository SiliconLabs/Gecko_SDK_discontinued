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

#ifndef __TFTDRAW_H
#define __TFTDRAW_H

#include "em_device.h"
#include "glib/glib.h"

#ifdef __cplusplus
extern "C" {
#endif

void TFT_displayUpdate(GLIB_Context_t *gcPtr);

#ifdef __cplusplus
}
#endif

#endif

