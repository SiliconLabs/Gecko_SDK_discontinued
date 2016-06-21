 /*************************************************************************//**
 * @file dmdif_ssd2119_spi.h
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


#ifndef __DMDIF_SSD2119_SPI_H_
#define __DMDIF_SSD2119_SPI_H_

#include <stdint.h>

/* Module Prototypes */
EMSTATUS DMDIF_init(uint32_t cmdRegAddr, uint32_t dataRegAddr);
EMSTATUS DMDIF_writeReg(uint8_t reg, uint16_t data);
uint16_t DMDIF_readDeviceCode(void);
EMSTATUS DMDIF_prepareDataAccess(void);
EMSTATUS DMDIF_writeData(uint32_t data);
uint32_t DMDIF_readData(void);
EMSTATUS DMDIF_delay(uint32_t ms);

#endif
