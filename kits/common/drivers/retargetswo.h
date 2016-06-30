/**************************************************************************//**
 * @file
 * @brief EFM32 Segment LCD Display driver, header file
 * @version 4.4.0
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


#ifndef RETARGETSWO_H
#define RETARGETSWO_H

#include "segmentlcdconfig.h"

/***************************************************************************//**
 * @addtogroup kitdrv
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup RetargetSWO
 * @{
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/* Regular functions */
int RETARGET_WriteChar(char c);
int RETARGET_ReadChar(void);
void setupSWOForPrint(void);
#ifdef __cplusplus
}
#endif

/** @} (end group RetargetSWO) */
/** @} (end group Drivers) */

#endif
