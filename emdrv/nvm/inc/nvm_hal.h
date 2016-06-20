/***************************************************************************//**
 * @file nvm_hal.h
 * @brief Non-Volatile Memory HAL.
 * @version 4.1.0
 *******************************************************************************
 * @section License
 * <b>(C) Copyright 2014 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/


#ifndef __NVMHAL_H
#define __NVMHAL_H

#include <stdbool.h>
#include "nvm.h"
#include "ecode.h"

/* Defines for changing HAL functionality. These are both a bit experimental,
 * but should work properly. */

/* Custom write and format methods based on the emlib are used in place of
* the originals. These methods put the CPU to sleep by going to EM1 while the
* operation progresses.
*
* NVMHAL_SLEEP_FORMAT and NVMHAL_SLEEP_WRITE is only used for toggling
* which function is called, and includes about the same amount of code. */

/** Use energy saving version of format function */
#ifndef NVMHAL_SLEEP_FORMAT
#define NVMHAL_SLEEP_FORMAT    false
#endif

/** Use energy saving version of write function */
#ifndef NVMHAL_SLEEP_WRITE
#define NVMHAL_SLEEP_WRITE     false
#endif

/** DMA read uses the DMA to read data from flash. This also works, but takes a
 * bit more time than the usual reading operations, while not providing a high
 * amount of power saving since read operations are normally very fast. */
#ifndef NVMHAL_DMAREAD
#define NVMHAL_DMAREAD    false
#endif

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
#define NVMHAL_SLEEP           (NVMHAL_SLEEP_FORMAT | NVMHAL_SLEEP_WRITE)
/** @endcond */

#include "em_device.h"

#if (NVMHAL_SLEEP == true)
#include "em_msc.h"
#include "em_dma.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_int.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/

void NVMHAL_Init(void);
void NVMHAL_DeInit(void);
void NVMHAL_Read(uint8_t *pAddress, void *pObject, uint16_t len);
Ecode_t NVMHAL_Write(uint8_t *pAddress, void const *pObject, uint16_t len);
Ecode_t NVMHAL_PageErase(uint8_t *pAddress);
void NVMHAL_Checksum(uint16_t *checksum, void *pMemory, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif /* __NVMHAL_H */
