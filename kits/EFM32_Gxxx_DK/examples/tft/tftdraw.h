/**************************************************************************//**
 * @file
 * @brief TFT Display refresh handling
 * @version 4.1.0
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

#ifndef __TFTDRAW_H
#define __TFTDRAW_H

#include "em_device.h"

#ifdef __cplusplus
extern "C" {
#endif

void TFT_displayUpdate(void);
void TFT_init(void);

#ifdef __cplusplus
}
#endif

#endif

