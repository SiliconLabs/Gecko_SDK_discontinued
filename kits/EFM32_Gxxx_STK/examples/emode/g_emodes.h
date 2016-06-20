/**************************************************************************//**
 * @file g_emodes.h
 * @brief Gecko energy mode setups (See Data Sheet Table 3.3), header file
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

#ifndef G_EMODES_H
#define G_EMODES_H

#include "em_cmu.h"

void G_EM0_Hfxo(void);

void G_EM0_Hfrco(CMU_HFRCOBand_TypeDef band);

void G_EM1_Hfxo(void);

void G_EM1_Hfrco(CMU_HFRCOBand_TypeDef band);

void G_EM2_LfrcoRTC(void);

void G_EM3(void);

void G_EM4(void);

#endif // G_EMODES_H
