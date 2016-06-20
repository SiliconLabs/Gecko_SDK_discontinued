/**************************************************************************//**
 * @file tg_emodes.h
 * @brief Tiny Gecko energy mode setups (See Data Sheet Table 3.3), header file
 * @version 4.2.0
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2015 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/


#ifndef TG_EMODES_H
#define TG_EMODES_H

#include "em_cmu.h"

void TG_EM0_Hfxo(void);

void TG_EM0_Hfrco(CMU_HFRCOBand_TypeDef band);

void TG_EM1_Hfxo(void);

void TG_EM1_Hfrco(CMU_HFRCOBand_TypeDef band);

void TG_EM2_LfrcoRTC(void);

void TG_EM3(void);

void TG_EM4(void);

#endif // TG_EMODES_H
