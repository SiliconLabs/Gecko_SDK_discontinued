/**************************************************************************//**
 * Copyright 2016 by Silicon Laboratories Inc. All rights reserved.
 *
 * http://developer.silabs.com/legal/version/v11/Silicon_Labs_Software_License_Agreement.txt
 *****************************************************************************/


#ifndef __RETARGET_VIRTUAL_H__
#define __RETARGET_VIRTUAL_H__

#include "retargetvirtualconfig.h"

/***************************************************************************//**
 * @addtogroup Drivers
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup RetargetIo
 * @{
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__CROSSWORKS_ARM)
int __putchar(int ch);
int __getchar(void);
#endif

int  RETARGET_ReadChar(void);
int  RETARGET_WriteChar(char c);

void RETARGET_SerialCrLf(int on);
void RETARGET_SerialInit(void);
void RETARGET_BlockOnTX(void);
#ifdef __cplusplus
}
#endif

/** @} (end group RetargetIo) */
/** @} (end group Drivers) */

#endif // __RETARGET_VIRTUAL_H__
