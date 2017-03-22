/**************************************************************************//**
 * @file bsp_os.h
 * @brief uC/OS-III BSP header file
 * @version 5.1.2
 *
 ******************************************************************************
 * @section License
 * <b>Copyright 2016 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#ifndef BSP_OS_H_
#define BSP_OS_H_

#ifdef __cplusplus
extern  "C" {
#endif

void BSP_OSTickInit(void);
void BSP_OSTickDisable(void);

#ifdef __cplusplus
}
#endif

#endif
