/***************************************************************************//**
 * @file hal/micro/cortexm3/efm32/bootloader/btl-ezsp-spi-protocol.c
 * @brief EFM32 internal SPI protocol implementation for use with the 
 *   standalone ezsp spi bootloader.
 * @version
 *******************************************************************************
 * @section License
 * <b>(C) Copyright 2015 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Silicon Labs has no
 * obligation to support this Software. Silicon Labs is providing the
 * Software "AS IS", with no express or implied warranties of any kind,
 * including, but not limited to, any implied warranties of merchantability
 * or fitness for any particular purpose or warranties against infringement
 * of any proprietary rights of a third party.
 *
 * Silicon Labs will not be liable for any consequential, incidental, or
 * special damages, or any other relief, or for any claim by any third party,
 * arising from your use of this Software.
 *
 ******************************************************************************/
#ifndef __BTL_EZSP_SPI_DEVICE_H__
#define __BTL_EZSP_SPI_DEVICE_H__

void ezspSetupSpiAndDma(void);

void ezspSpiInitSpiAndDma(void);

void configureSpiGpio(void);

void ezspSpiConfigureInterrupts(void);

void ezspSpiDisableReceptionInterrupts(void);

void ezspSpiDisableReception(void);

void ezspSpiAckUnload(void);

void ezspSpiEnableReception(void);

void ezspSpiStartTxTransfer(uint8_t responseLength);

bool ezspSpiRxActive(void);

void ezspSpiFlushRxFifo(void);

bool ezspSpiValidStartOfData(void);

bool ezspSpiHaveTwoBytes(void);

bool ezspSpiHaveAllData(void);

bool ezspSpiPollForMosi(uint8_t responseLength);

bool ezspSpiPollForNWAKE(void);

bool ezspSpiPollForNSSEL(void);

bool ezspSpiTransactionValid(uint8_t responseLength);

#endif //__BTL_EZSP_SPI_DEVICE_H__
