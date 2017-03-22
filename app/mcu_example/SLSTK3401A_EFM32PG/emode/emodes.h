/**************************************************************************//**
 * @file emodes.h
 * @brief Pearl Gecko energy mode setups (See Data Sheet Table 4.4-4.6), header file
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

#ifndef EMODES_H
#define EMODES_H

#include "em_cmu.h"

/* Enter energy mode functions */
void em_EM0_Hfxo(void);

void em_EM0_Hfrco(CMU_HFRCOFreq_TypeDef band);

void em_EM1_Hfxo(void);

void em_EM1_Hfrco(CMU_HFRCOFreq_TypeDef band);

void em_EM2_RTCC(CMU_Select_TypeDef osc, bool powerdownRam);

void em_EM3_UlfrcoCRYO(void);

void em_EM4H_LfxoRTCC(void);

void em_EM4H_UlfrcoCRYO(void);

void em_EM4H(void);

void em_EM4S(void);

/* Calculate Primes */
void primeCalc(void);

#endif // EMODES_H
