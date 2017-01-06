/**************************************************************************//**
 * @file lcd_setup.h
 * @brief Setup LCD for energy mode demo, header file
 * @version 5.0.0
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

#include <stdint.h>

/* Energy mode enumerations */
typedef enum
{
  EM0_HFXO_40MHZ_WHILE,          /**< EM0 with HFXO at 40MHz (while loop)                    */
  EM0_HFRCO_38MHZ_PRIME,         /**< EM0 with HFRCO at 38MHz (prime calculations)           */
  EM0_HFRCO_38MHZ_WHILE,         /**< EM0 with HFRCO at 38MHz (while loop)                   */
  EM0_HFRCO_38MHZ_COREMARK,      /**< EM0 with HFRCO at 38MHz (Coremark)                     */
  EM0_HFRCO_26MHZ_WHILE,         /**< EM0 with HFRCO at 26MHz (while loop)                   */
  EM0_HFRCO_1MHZ_WHILE,          /**< EM0 with HFRCO at 1MHz (while loop)                    */
  EM1_HFXO_40MHZ,                /**< EM1 with HFXO at 40MHz                                 */
  EM1_HFRCO_38MHZ,               /**< EM1 with HFRCO at 38MHz                                */
  EM1_HFRCO_26MHZ,               /**< EM1 with HFRCO at 26MHz                                */
  EM1_HFRCO_1MHZ,                /**< EM1 with HFRCO at 1MHz                                 */
  EM2_LFXO_RTCC,                 /**< EM2 with LFXO RTCC                                     */
  EM2_LFRCO_RTCC,                /**< EM2 with LFRCO RTCC                                    */
  EM3_ULFRCO_CRYO,               /**< EM3 with ULFRCO CRYOTIMER                              */
  EM4H_LFXO_RTCC,                /**< EM4H with RTCC                                         */
  EM4H_ULFRCO_CRYO,              /**< EM4H with ULFRCO CRYOTIMER                             */
  EM4H,                          /**< EM4H                                                   */
  EM4S,                          /**< EM4S                                                   */
  EM0_HFXO_40MHZ_WHILE_DCDC,     /**< EM0 with HFXO at 40MHz with DCDC (while loop)          */
  EM0_HFRCO_38MHZ_PRIME_DCDC,    /**< EM0 with HFRCO at 38MHz with DCDC (prime calculations) */
  EM0_HFRCO_38MHZ_WHILE_DCDC,    /**< EM0 with HFRCO at 38MHz with DCDC (while loop)         */
  EM0_HFRCO_38MHZ_COREMARK_DCDC, /**< EM0 with HFRCO at 38MHz with DCDC (Coremark)           */
  EM0_HFRCO_26MHZ_WHILE_DCDC,    /**< EM0 with HFRCO at 26MHz with DCDC (while loop)         */
  EM1_HFXO_40MHZ_DCDC,           /**< EM1 with HFXO at 40MHz with DCDC                       */
  EM1_HFRCO_38MHZ_DCDC,          /**< EM1 with HFRCO at 38MHz with DCDC                      */
  EM1_HFRCO_26MHZ_DCDC,          /**< EM1 with HFRCO at 26MHz with DCDC                      */
  EM1_HFRCO_1MHZ_DCDC,           /**< EM1 with HFRCO at 1MHz with DCDC                       */
  EM2_LFXO_RTCC_DCDC,            /**< EM2 with LFXO RTCC with DCDC                           */
  EM2_LFRCO_RTCC_DCDC,           /**< EM2 with LFRCO RTCC with DCDC                          */
  EM3_ULFRCO_CRYO_DCDC,          /**< EM3 with ULFRCO CRYOTIMER with DCDC                    */
  EM4H_LFXO_RTCC_DCDC,           /**< EM4H with RTCC with DCDC                               */
  EM4H_ULFRCO_CRYO_DCDC,         /**< EM4H with ULFRCO CRYOTIMER with DCDC                   */
  EM4H_DCDC,                     /**< EM4H with DCDC                                         */
  EM4S_DCDC,                     /**< EM4S with DCDC                                         */
  NUM_EMODES                     /**< Number of Energy Modes                                 */
} Energy_Mode_TypeDef;

/* Energy mode select using LCD and buttons */
Energy_Mode_TypeDef LCD_SelectMode(void);

#endif // LCD_SETUP_H
