/**************************************************************************//**
 * @file
 * @brief
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

#ifndef LCD_CONF_H
#define LCD_CONF_H

#if defined(__cplusplus)
extern "C" {
#endif

void LCD_X_Config(void);
void LCD_X_InitDriver(void);
int  LCD_X_DisplayDriver(unsigned LayerIndex, unsigned Cmd, void * pData);

#if defined(__cplusplus)
}
#endif

#endif
