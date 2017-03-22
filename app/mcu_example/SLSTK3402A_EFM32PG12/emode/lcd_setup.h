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

#include <stdint.h>
#include "em_device.h"

/* Enable advanced low-power features if ported to EFM32xG12 or later. */
#if (defined(_EFM32_PEARL_FAMILY) || defined(_EFM32_JADE_FAMILY)) \
     && (_SILICON_LABS_32B_SERIES_1_CONFIG > 1)
#define ADVANCED_LOWPOWER_FEATURES
#endif

/* Energy mode enumerations */
typedef enum
{
  EM0_HFXO_40MHZ_WHILE,              /**< EM0 with HFXO at 38.4MHz (while loop)                                        */
  EM0_HFRCO_38MHZ_PRIME,             /**< EM0 with HFRCO at 38MHz (prime calculations)                                 */
  EM0_HFRCO_38MHZ_WHILE,             /**< EM0 with HFRCO at 38MHz (while loop)                                         */
  EM0_HFRCO_38MHZ_COREMARK,          /**< EM0 with HFRCO at 38MHz (Coremark)                                           */
  EM0_HFRCO_26MHZ_WHILE,             /**< EM0 with HFRCO at 26MHz (while loop)                                         */
  EM0_HFRCO_1MHZ_WHILE,              /**< EM0 with HFRCO at 1MHz (while loop)                                          */
  EM1_HFXO_40MHZ,                    /**< EM1 with HFXO at 38.4MHz                                                     */
  EM1_HFRCO_38MHZ,                   /**< EM1 with HFRCO at 38MHz                                                      */
  EM1_HFRCO_26MHZ,                   /**< EM1 with HFRCO at 26MHz                                                      */
  EM1_HFRCO_1MHZ,                    /**< EM1 with HFRCO at 1MHz                                                       */
#if defined(ADVANCED_LOWPOWER_FEATURES)
  EM0_HFXO_40MHZ_WHILE_LP,           /**< EM0 with HFXO at 38.4MHz (while loop)                                        */
  EM0_HFRCO_38MHZ_PRIME_LP,          /**< EM0 with HFRCO at 38MHz (prime calculations)                                 */
  EM0_HFRCO_38MHZ_WHILE_LP,          /**< EM0 with HFRCO at 38MHz (while loop)                                         */
  EM0_HFRCO_38MHZ_COREMARK_LP,       /**< EM0 with HFRCO at 38MHz (Coremark)                                           */
  EM0_HFRCO_26MHZ_WHILE_LP,          /**< EM0 with HFRCO at 26MHz (while loop)                                         */
  EM0_HFRCO_1MHZ_WHILE_LP,           /**< EM0 with HFRCO at 1MHz (while loop)                                          */
  EM1_HFXO_40MHZ_LP,                 /**< EM1 with HFXO at 38.4MHz                                                     */
  EM1_HFRCO_38MHZ_LP,                /**< EM1 with HFRCO at 38MHz                                                      */
  EM1_HFRCO_26MHZ_LP,                /**< EM1 with HFRCO at 26MHz                                                      */
  EM1_HFRCO_1MHZ_LP,                 /**< EM1 with HFRCO at 1MHz                                                       */
#endif
  EM2_LFXO_RTCC,                     /**< EM2 with LFXO RTCC                                                           */
  EM2_LFRCO_RTCC,                    /**< EM2 with LFRCO RTCC                                                          */
  EM3_ULFRCO_CRYO,                   /**< EM3 with ULFRCO CRYOTIMER                                                    */
  EM4H_LFXO_RTCC,                    /**< EM4H with RTCC                                                               */
  EM4H_ULFRCO_CRYO,                  /**< EM4H with ULFRCO CRYOTIMER                                                   */
  EM4H,                              /**< EM4H                                                                         */
  EM4S,                              /**< EM4S                                                                         */
  EM0_HFXO_40MHZ_WHILE_DCDC_DCM,     /**< EM0 with HFXO at 38.4MHz with DCDC in Low Noise DCM mode (while loop)        */
  EM0_HFRCO_38MHZ_PRIME_DCDC_DCM,    /**< EM0 with HFRCO at 38MHz with DCDC in Low Noise DCM mode (prime calculations) */
  EM0_HFRCO_38MHZ_WHILE_DCDC_DCM,    /**< EM0 with HFRCO at 38MHz with DCDC in Low Noise DCM mode (while loop)         */
  EM0_HFRCO_38MHZ_COREMARK_DCDC_DCM, /**< EM0 with HFRCO at 38MHz with DCDC in Low Noise DCM mode (Coremark)           */
  EM0_HFRCO_26MHZ_WHILE_DCDC_DCM,    /**< EM0 with HFRCO at 26MHz with DCDC in Low Noise DCM mode (while loop)         */
  EM0_HFXO_40MHZ_WHILE_DCDC_CCM,     /**< EM0 with HFXO at 38.4MHz with DCDC in Low Noise CCM mode (while loop)        */
  EM0_HFRCO_38MHZ_PRIME_DCDC_CCM,    /**< EM0 with HFRCO at 38MHz with DCDC in Low Noise CCM mode (prime calculations) */
  EM0_HFRCO_38MHZ_WHILE_DCDC_CCM,    /**< EM0 with HFRCO at 38MHz with DCDC in Low Noise CCM mode (while loop)         */
  EM0_HFRCO_38MHZ_COREMARK_DCDC_CCM, /**< EM0 with HFRCO at 38MHz with DCDC in Low Noise CCM mode (Coremark)           */
  EM0_HFRCO_26MHZ_WHILE_DCDC_CCM,    /**< EM0 with HFRCO at 26MHz with DCDC in Low Noise CCM mode (while loop)         */
  EM1_HFXO_40MHZ_DCDC_DCM,           /**< EM1 with HFXO at 38.4MHz with DCDC in Low Noise DCM mode                     */
  EM1_HFRCO_38MHZ_DCDC_DCM,          /**< EM1 with HFRCO at 38MHz with DCDC in Low Noise DCM mode                      */
  EM1_HFRCO_26MHZ_DCDC_DCM,          /**< EM1 with HFRCO at 26MHz with DCDC in Low Noise DCM mode                      */
  EM1_HFXO_40MHZ_DCDC_CCM,           /**< EM1 with HFXO at 38.4MHz with DCDC in Low Noise CCM mode                     */
  EM1_HFRCO_38MHZ_DCDC_CCM,          /**< EM1 with HFRCO at 38MHz with DCDC in Low Noise CCM mode                      */
  EM1_HFRCO_26MHZ_DCDC_CCM,          /**< EM1 with HFRCO at 26MHz with DCDC in Low Noise CCM mode                      */
  EM2_LFXO_RTCC_DCDC_LPM,            /**< EM2 with LFXO RTCC with DCDC in Low Power mode                               */
  EM2_LFRCO_RTCC_DCDC_LPM,           /**< EM2 with LFRCO RTCC with DCDC in Low Power mode                              */
  EM3_ULFRCO_CRYO_DCDC,              /**< EM3 with ULFRCO CRYOTIMER with DCDC                                          */
  EM4H_LFXO_RTCC_DCDC,               /**< EM4H with RTCC with DCDC                                                     */
  EM4H_ULFRCO_CRYO_DCDC,             /**< EM4H with ULFRCO CRYOTIMER with DCDC                                         */
  EM4H_DCDC,                         /**< EM4H with DCDC                                                               */
  EM4S_DCDC,                         /**< EM4S with DCDC                                                               */
  NUM_EMODES                         /**< Number of Energy Modes                                                       */
} Energy_Mode_TypeDef;

/* Energy mode select using LCD and buttons */
Energy_Mode_TypeDef LCD_SelectMode(void);

#endif // LCD_SETUP_H
