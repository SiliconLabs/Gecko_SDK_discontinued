/**************************************************************************//**
 * @brief Background image for the weatherstation demo
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


#ifndef __BACKGROUND_H__
#define __BACKGROUND_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BACKGROUND_XSIZE 384
#define BACKGROUND_YSIZE 128
extern const uint8_t background[6144];

#ifdef __cplusplus
}
#endif

#endif /* __BACKGROUND_H__ */
