/*
 *  Common functions for AES based algorithms for the CRYPTO hw module.
 *
 *  Copyright (C) 2016 Silicon Labs, http://www.silabs.com
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
#include "em_device.h"

#if defined(CRYPTO_COUNT) && (CRYPTO_COUNT > 0)

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include "aesdrv_common_crypto.h"
#include "cryptodrv_internal.h"
#if defined( MBEDTLS_INCLUDE_IO_MODE_DMA )
#include "dmadrv.h"
#endif
#include "em_crypto.h"
#include "em_assert.h"
#include <string.h>

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/
#define AESDRV_UTILS_DMA_CHANNEL_NOT_SET 0xFFFF

/*******************************************************************************
 **************************   STATIC FUNCTIONS   *******************************
 ******************************************************************************/
#if defined( MBEDTLS_INCLUDE_IO_MODE_DMA )
static void    aesdrvDmaReset     (AESDRV_Context_t* pAesdrvContext);
static int     aesdrvDmaInit      (AESDRV_Context_t* pAesdrvContext);
static void    aesdrvDmaSetup     (AESDRV_Context_t* pAesdrvContext,
                                   uint8_t const*    pData,
                                   uint32_t          authDataLength,
                                   uint32_t          textLength
                                   );
static void    aesdrvDmaAddrLenGet(uint8_t**         pBufIn,
                                   uint8_t**         pBufOut,
                                   uint16_t          lengthA,
                                   uint16_t          lengthB,
                                   uint16_t*         dmaLengthIn,
                                   uint16_t*         dmaLengthOut);
#endif /* #if defined( MBEDTLS_INCLUDE_IO_MODE_DMA ) */

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/*
 *   Initializes an AESDRV context structure.
 *   Please refer to aesdrv.h for detailed description.
 */
int AESDRV_Init(AESDRV_Context_t* pAesdrvContext)
{
  /* Start by clearing the device context. */
  memset(pAesdrvContext, 0, sizeof(AESDRV_Context_t));

  /* Set I/O mode to mcu core. */
  pAesdrvContext->ioMode = aesdrvIoModeCore;
  
  /* Clear the DMA channel config */
  pAesdrvContext->ioModeSpecific.dmaConfig.dmaChIn =
    AESDRV_UTILS_DMA_CHANNEL_NOT_SET;
  pAesdrvContext->ioModeSpecific.dmaConfig.dmaChOut =
    AESDRV_UTILS_DMA_CHANNEL_NOT_SET;
   
  /* Disable authentication tag optimization */
  pAesdrvContext->authTagOptimize         = false;

  /* Set default CRYPTO device instance to use. */
  cryptodrvSetDeviceInstance(&pAesdrvContext->cryptodrvContext, 0);

  /* Set I/O mode to mcu core. */
  return AESDRV_SetIoMode(pAesdrvContext, aesdrvIoModeCore, 0);
}

/*
 *   DeInitializes AESDRV context.
 *   Please refer to aesdrv.h for detailed description.
 */
int AESDRV_DeInit(AESDRV_Context_t* pAesdrvContext)
{
  switch( pAesdrvContext->ioMode )
  {
  case aesdrvIoModeCore:
    /* Do nothing. */
    break;
  case aesdrvIoModeDma:
#if defined( MBEDTLS_INCLUDE_IO_MODE_DMA )
    aesdrvDmaReset(pAesdrvContext);
#endif
    break;
  }

  /* Clear the device context. */
  memset(pAesdrvContext, 0, sizeof(AESDRV_Context_t));
  
  return 0;
}

/*
 *   Set the AES/CRYPTO device instance.
 *   Please refer to aesdrv.h for detailed description.
 */
int AESDRV_SetDeviceInstance(AESDRV_Context_t*  pAesdrvContext,
                             unsigned int       devno)
{
  /* Set default CRYPTO device instance to use. */
  return cryptodrvSetDeviceInstance(&pAesdrvContext->cryptodrvContext,
                                    devno);
}

/*
 *   Setup CRYPTO I/O mode.
 *   Please refer to aesdrv.h for detailed description.
 */
int AESDRV_SetIoMode
(
 AESDRV_Context_t*        pAesdrvContext,
 AESDRV_IoMode_t          ioMode,
 AESDRV_IoModeSpecific_t* ioModeSpecific
 )
{
  int retval = 0;
  (void) ioModeSpecific; /* Unused since BUFC I/O mode was removed. */

  /* Start by reseting any previous settings, if applicable. */
  if ( ioMode != pAesdrvContext->ioMode )
  {
    switch(pAesdrvContext->ioMode )
    {
      case aesdrvIoModeCore:
        break;
      case aesdrvIoModeDma:
#if defined( MBEDTLS_INCLUDE_IO_MODE_DMA )
        aesdrvDmaReset(pAesdrvContext);
#endif
        break;
    }

    /* Set requested I/O mode now. */
    switch( ioMode )
    {
    case aesdrvIoModeCore:
      /* Do nothing. Just continue without hitting default label which signals
         error.*/
      break;
    
    case aesdrvIoModeDma:
#if defined( MBEDTLS_INCLUDE_IO_MODE_DMA )
      /* Start by reseting previous settings.*/
      aesdrvDmaReset(pAesdrvContext);
      retval = aesdrvDmaInit(pAesdrvContext);
#else
      retval = MBEDTLS_ECODE_AESDRV_NOT_SUPPORTED;
#endif
      break;
      
    default:
      retval = MBEDTLS_ECODE_AESDRV_NOT_SUPPORTED;
    }

    if (0 == retval)
    {
      /* If success we store the I/O mode for later references. */
      pAesdrvContext->ioMode = ioMode;
    }
  }
  
  return retval;
}

/*
 *   Prepare CRYPTO I/O mode to transfer data
 */
void AESDRV_HwIoSetup(AESDRV_Context_t* pAesdrvContext,
                      uint8_t*          pData,
                      uint32_t          authDataLength,
                      uint32_t          textLength)
{
#if !defined( MBEDTLS_INCLUDE_IO_MODE_DMA )
  (void)pAesdrvContext; (void)pData; (void)authDataLength; (void)textLength;
#endif
  switch (pAesdrvContext->ioMode)
  {
  case aesdrvIoModeDma:
#if defined( MBEDTLS_INCLUDE_IO_MODE_DMA )
    aesdrvDmaSetup( pAesdrvContext, pData, authDataLength, textLength );
#endif
    break;
  default:
    break;
  }
}

/*******************************************************************************
 ***********************   LOCAL STATIC FUNCTIONS   ****************************
 ******************************************************************************/

#if defined( MBEDTLS_INCLUDE_IO_MODE_DMA )

/***************************************************************************//**
 * Function resets DMA settings for CRYPTO.
 *
 * @details
 *   Function clears trigger settings for channel setup to be used by CRYPTO.
 *
 ******************************************************************************/
static void aesdrvDmaReset(AESDRV_Context_t* pAesdrvContext)
{
  AESDRV_DmaConfig_t* dmaConfig= &pAesdrvContext->ioModeSpecific.dmaConfig;
    
  if (dmaConfig->dmaChIn != AESDRV_UTILS_DMA_CHANNEL_NOT_SET)
  {
    DMADRV_FreeChannel(dmaConfig->dmaChIn);
  }

  if (dmaConfig->dmaChOut != AESDRV_UTILS_DMA_CHANNEL_NOT_SET)
  {
    DMADRV_FreeChannel(dmaConfig->dmaChOut);
  }
  dmaConfig->dmaChIn = AESDRV_UTILS_DMA_CHANNEL_NOT_SET;
  dmaConfig->dmaChOut = AESDRV_UTILS_DMA_CHANNEL_NOT_SET;
}

/***************************************************************************//**
 * Function initializes DMADRV for CRYPTO and allocates two channels.
 *
 * @details
 *  Function assumes that same buffer is used for input and output (in place).
 *  Additionally, it supports packet authentication&encryption where first part
 *  of the packet is only authenticated (CCM,GCM).
 *
 * @return
 *  OK when DMA channel allocation completed, OUT_OF_RESOURCES if not
 *
 ******************************************************************************/
static int aesdrvDmaInit( AESDRV_Context_t* pAesdrvContext )
{
  int     retval = 0;
  Ecode_t status;
  AESDRV_DmaConfig_t* dmaConfig = &pAesdrvContext->ioModeSpecific.dmaConfig;
  
  status = DMADRV_Init();
  if ( status == ECODE_EMDRV_DMADRV_ALREADY_INITIALIZED || 
       status == ECODE_EMDRV_DMADRV_OK ) 
  {
    retval = 0;
  }
  else
  {
    return MBEDTLS_ECODE_AESDRV_OUT_OF_RESOURCES;
  }
  
  // Allocate first DMA channel
  status = DMADRV_AllocateChannel(&dmaConfig->dmaChIn, NULL);
  if ( status != ECODE_EMDRV_DMADRV_OK )
  {
    return MBEDTLS_ECODE_AESDRV_OUT_OF_RESOURCES;
  }
  // Allocate second DMA channel
  status = DMADRV_AllocateChannel(&dmaConfig->dmaChOut, NULL);
  if ( status != ECODE_EMDRV_DMADRV_OK ) {
    DMADRV_FreeChannel(dmaConfig->dmaChIn);
    dmaConfig->dmaChIn = AESDRV_UTILS_DMA_CHANNEL_NOT_SET;
    return MBEDTLS_ECODE_AESDRV_OUT_OF_RESOURCES;
  }

  return retval;
}

/**
 * Function setup DMA for CRYPTO.
 *
 * @details
 *  Function assumes that same buffer is used for input and output (in place).
 *  Additionally, it supports packet authentication&encryption where first part
 *  of the packet is only authenticated (CCM,GCM).
 *
 * @param authDataLen
 *  Length of authentication part.
 *
 * @param textLength
 *  Length of encrypted part.
 *
 * @warning
 *  Function is modifying CRYPTO_SEQCTRL and CRYPTO_SEQCTRLB registers. It must
 *  be ensured that access to those registers is done in
 *  correct order and settings are not overwritten.
 *
 */
static void aesdrvDmaSetup(AESDRV_Context_t* pAesdrvContext,
                            uint8_t const*   pData,
                            uint32_t         authDataLength,
                            uint32_t         textLength)
{
  uint8_t * _authData = (uint8_t*)pData;
  uint8_t * _textData = (uint8_t*)((uint32_t)pData + authDataLength);
  CRYPTO_TypeDef* crypto = pAesdrvContext->cryptodrvContext.device->crypto;
  AESDRV_DmaConfig_t* dmaConfig = &pAesdrvContext->ioModeSpecific.dmaConfig;
  uint16_t lenIn, lenOut;
  uint32_t seqctrl;
    
  /* Ensure that dma for crypto was initialized. */
  EFM_ASSERT((dmaConfig->dmaChIn != AESDRV_UTILS_DMA_CHANNEL_NOT_SET) &&
             (dmaConfig->dmaChOut != AESDRV_UTILS_DMA_CHANNEL_NOT_SET));

  crypto->CTRL &= (~_CRYPTO_CTRL_DMA0RSEL_MASK) & (~_CRYPTO_CTRL_DMA1RSEL_MASK);
  seqctrl  = crypto->SEQCTRL;
  seqctrl &= (~_CRYPTO_SEQCTRL_DMA0SKIP_MASK) & (~_CRYPTO_SEQCTRL_DMA0PRESA_MASK);
  seqctrl |= (uint32_t)_authData % 4 << _CRYPTO_SEQCTRL_DMA0SKIP_SHIFT;
  crypto->SEQCTRL = seqctrl;
  crypto->SEQCTRLB |= CRYPTO_SEQCTRLB_DMA0PRESB;
  aesdrvDmaAddrLenGet((uint8_t **)&_authData,
                       (uint8_t **)&_textData,
                       authDataLength,
                       textLength,
                       &lenIn, &lenOut);
  
  DMADRV_MemoryPeripheral( dmaConfig->dmaChIn,
#ifdef EMDRV_DMADRV_LDMA
                           pAesdrvContext->cryptodrvContext.device->dmaReqSigChIn,
#else
#error "UDMA + CRYPTO is a non-exisiting combination"
#endif
                           (void*)&crypto->DATA0,
                           (void *)_authData,
                           true,
                           lenIn / 4,
                           dmadrvDataSize4,
                           NULL,
                           NULL );
  
  if (textLength)
  {
    DMADRV_PeripheralMemory( dmaConfig->dmaChOut,
                             pAesdrvContext->cryptodrvContext.device->dmaReqSigChOut,
                             (void *)_textData,
                             (void*)&crypto->DATA0,
                             true,
                             lenOut / 4,
                             dmadrvDataSize4,
                             NULL,
                             NULL );
  }
}

/**
 * Function calculates length transfers for reading to crypto and from crypto.
 *
 * @details
 *  Function takes into account address alignment for input and output buffer.
 *  It overwrites provided addresses applying alignment.
 *
 * @param pBufIn
 *  Address of input buffer. Will be modified by the function if not word
 *  aligned.
 *
 * @param pBufOut
 *  Address of output buffer. Will be modified by the function if not word
 *  aligned.
 *
 * @param lengthA
 *  Length of authentication part of CCM.
 * @param lengthB
 *  Length of payload to be encrypted.
 * @return
 */
static void aesdrvDmaAddrLenGet(uint8_t**  pBufIn,
                                uint8_t**  pBufOut,
                                uint16_t   lengthA,
                                uint16_t   lengthB,
                                uint16_t*  dmaLengthIn,
                                uint16_t*  dmaLengthOut)
{
  uint8_t* pIn     = *pBufIn;
  uint8_t* pOut    = *pBufOut;
  uint8_t  moduloA = (uint32_t)pIn % 4;
  uint8_t  moduloB = (uint32_t)pOut % 4;

  *dmaLengthIn  = lengthA+lengthB;
  *dmaLengthOut = lengthB;

  pIn -= moduloA;
  *dmaLengthIn += moduloA;
  *dmaLengthIn = (*dmaLengthIn+3) & 0xFFFC;

  pOut -= moduloB;
  *dmaLengthOut += moduloB;
  *dmaLengthOut = (*dmaLengthOut+3) & 0xFFFC;

  *pBufIn = pIn;
  *pBufOut = pOut;

  return;
}

#endif /* #if defined( MBEDTLS_INCLUDE_IO_MODE_DMA ) */

#endif /* #if defined(CRYPTO_COUNT) && (CRYPTO_COUNT > 0) */
