/**************************************************************************//**
 * @file lcd_setup.h
 * @brief Setup LCD for energy mode demo, header file
 * @version 5.1.2
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

#ifndef LCD_SETUP_H
#define LCD_SETUP_H

/* Energy mode enumerations */
typedef enum
{
  EM0_HFXO_32MHZ,  /**< EM0 with HFXO at 32MHz   */
  EM0_HFRCO_28MHZ, /**< EM0 with HFRCO at 28MHz  */  
  EM0_HFRCO_21MHZ, /**< EM0 with HFRCO at 21MHz  */
  EM0_HFRCO_14MHZ, /**< EM0 with HFRCO at 14MHz  */
  EM0_HFRCO_11MHZ, /**< EM0 with HFRCO at 11MHz  */
  EM0_HFRCO_7MHZ,  /**< EM0 with HFRCO at 7MHz   */
  EM0_HFRCO_1MHZ,  /**< EM0 with HFRCO at 1MHz   */
  EM1_HFXO_32MHZ,  /**< EM1 with HFXO at 32MHz   */  
  EM1_HFRCO_28MHZ, /**< EM1 with HFRCO at 28MHz  */  
  EM1_HFRCO_21MHZ, /**< EM1 with HFRCO at 21MHz  */
  EM1_HFRCO_14MHZ, /**< EM1 with HFRCO at 14MHz  */
  EM1_HFRCO_11MHZ, /**< EM1 with HFRCO at 11MHz  */
  EM1_HFRCO_7MHZ,  /**< EM1 with HFRCO at 7MHz   */
  EM1_HFRCO_1MHZ,  /**< EM1 with HFRCO at 1MHz   */
  EM2_LFRCO_RTC,   /**< EM2 with LFRCO RTC       */
  EM3,             /**< EM3                      */
  EM4,             /**< EM4                      */
  NUM_EMODES,      /**< Number of Energy Modes   */
} Energy_Mode_TypeDef;

/* Energy mode select using LCD and buttons */
Energy_Mode_TypeDef LCD_SelectMode(void);

#endif // LCD_SETUP_H
