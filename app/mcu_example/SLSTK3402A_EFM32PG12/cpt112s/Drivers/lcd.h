/**************************************************************************//**
 * @file
 * @brief helper functions for drawing capsense button states on the LCD
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

#ifndef LCD_H
#define LCD_H

typedef struct _demoobject {
    int  posX;
    int  posY;
    int  status;
} DemoObject;

/******************************************************************************
 * @brief initialize and clear the 128x128 lcd display (Happy Gecko)
 *                   or the 160 segment display (Leopard/Wonder Gecko)
 *****************************************************************************/
void initLCD(void);

/******************************************************************************
 * @brief update lcd display frame
 *****************************************************************************/
void updateLCD(void);

#endif /* LCD_H */
