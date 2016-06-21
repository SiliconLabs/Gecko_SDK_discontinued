 /*************************************************************************//**
 * @file dmd_if_ssd2119_ebi.h
 * @brief Dot matrix display SSD2119 interface using EBI
 ******************************************************************************
 * @section License
 * <b>Copyright 2015 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/


#ifndef __DMD_IF_SSD2119_EBI_H_
#define __DMD_IF_SSD2119_EBI_H_

#include <stdint.h>
#include "em_types.h"

/* Module Prototypes */
EMSTATUS DMDIF_init(uint32_t cmdRegAddr, uint32_t dataRegAddr);
EMSTATUS DMDIF_writeReg(uint8_t reg, uint16_t data);
uint16_t DMDIF_readDeviceCode(void);
EMSTATUS DMDIF_prepareDataAccess(void);
EMSTATUS DMDIF_writeData(uint32_t data);
EMSTATUS DMDIF_writeDataRepeated(uint32_t data, int len);
uint32_t DMDIF_readData(void);
EMSTATUS DMDIF_delay(uint32_t ms);

#endif
