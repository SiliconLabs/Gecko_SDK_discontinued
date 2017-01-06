 /*************************************************************************//**
 * @file dmd_ssd2119.h
 * @brief Dot matrix display driver for LCD controller SSD2119
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


#ifndef __DMD_SSD2119_H
#define __DMD_SSD2119_H

#include "dmd/dmd.h"

/** Frame update frequency of display */
#define DMD_FRAME_FREQUENCY    80
/** Horizontal size of the display */
#define DMD_HORIZONTAL_SIZE    320
/** Vertical size of the display */
#define DMD_VERTICAL_SIZE      240

EMSTATUS DMDIF_init(uint32_t cmdRegAddr, uint32_t dataRegAddr);

#endif
