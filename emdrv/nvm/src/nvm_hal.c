/***************************************************************************//**
 * @file nvm_hal.c
 * @brief Non-Volatile Memory HAL for EFM32/EZR32/EFR32 flash.
 * @version 4.1.0
 *******************************************************************************
 * @section License
 * <b>(C) Copyright 2015 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include <stdbool.h>
#include "em_msc.h"
#include "nvm.h"
#include "nvm_hal.h"

/*******************************************************************************
 ******************************   CONSTANTS   **********************************
 ******************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/* Padding value */
#define NVMHAL_FFFFFFFF      0xffffffffUL

#if (NVMHAL_DMAREAD == true)
/* DMA related defines. */
#define NVMHAL_DMA_CHANNELS         1
#define NVMHAL_DMA_CHANNEL_FLASH    0

/* DMA control block, must be aligned to 256. */
#if defined (__ICCARM__)
#pragma data_alignment=256
static DMA_DESCRIPTOR_TypeDef NVMHAL_dmaControlBlock[DMA_CHAN_COUNT * 2];
#elif defined (__CC_ARM)
static DMA_DESCRIPTOR_TypeDef NVMHAL_dmaControlBlock[DMA_CHAN_COUNT * 2] __attribute__ ((aligned(256)));
#elif defined (__GNUC__)
static DMA_DESCRIPTOR_TypeDef NVMHAL_dmaControlBlock[DMA_CHAN_COUNT * 2] __attribute__ ((aligned(256)));
#else
#error Undefined toolkit, need to define alignment
#endif

#endif

#if (NVMHAL_SLEEP == true || NVMHAL_DMAREAD == true)
/* Transfer Flag. */
static volatile bool NVMHAL_FlashTransferActive;
#endif

/** @endcond */

/*******************************************************************************
 ***************************   LOCAL FUNCTIONS   *******************************
 ******************************************************************************/


/***************************************************************************//**
 * Prototypes of functions that should be placed in RAM. Since it operate on
 * the flash they cannot be read out during operation.
 ******************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

#if (NVMHAL_SLEEP == true)
#ifdef __CC_ARM  /* MDK-ARM compiler */
static msc_Return_TypeDef NVMHAL_MSC_WriteWord(uint32_t *address, void const *data, uint32_t numBytes);
static msc_Return_TypeDef NVMHAL_MSC_ErasePage(uint32_t *startAddress);
#endif /* __CC_ARM */

#ifdef __ICCARM__ /* IAR compiler */
__ramfunc static msc_Return_TypeDef NVMHAL_MSC_WriteWord(uint32_t *address, void const *data, uint32_t numBytes);
__ramfunc static msc_Return_TypeDef NVMHAL_MSC_ErasePage(uint32_t *startAddress);
#endif /* __ICCARM__ */

#ifdef __GNUC__  /* GCC based compilers */
#ifdef __CROSSWORKS_ARM  /* Rowley Crossworks (GCC based) */
static msc_Return_TypeDef NVMHAL_MSC_WriteWord(uint32_t *address, void const *data, uint32_t numBytes) __attribute__ ((section(".fast")));
static msc_Return_TypeDef NVMHAL_MSC_ErasePage(uint32_t *startAddress) __attribute__ ((section(".fast")));
#else /* GCC */
static msc_Return_TypeDef NVMHAL_MSC_WriteWord(uint32_t *address, void const *data, uint32_t numBytes) __attribute__ ((section(".ram")));
static msc_Return_TypeDef NVMHAL_MSC_ErasePage(uint32_t *startAddress) __attribute__ ((section(".ram")));
#endif /* __GNUC__ */
#endif /* __CROSSWORKS_ARM */
#endif

/** @endcond */

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

#if (NVMHAL_DMAREAD == true)
/**************************************************************************//**
 * @brief  Called When Flash Transfer is Complete
 *****************************************************************************/
static void NVM_ReadFromFlashComplete(unsigned int channel, bool primary, void *user)
{
  /* Clearing flag to indicate that transfer is complete */
  NVMHAL_FlashTransferActive = false;
}
#endif

#if (NVMHAL_SLEEP == true)
/**************************************************************************//**
 * @brief  MSC interrupt handler. Resets interrupts and the transfer flag.
 *****************************************************************************/
void MSC_IRQHandler(void)
{
  /* Clear interrupt source */
  MSC_IntClear(MSC_IFC_ERASE);
  MSC_IntClear(MSC_IFC_WRITE);

  NVMHAL_FlashTransferActive = false;
}
#endif

#if (NVMHAL_SLEEP_WRITE == true)

/***************************************************************************//**
 * @brief
 *   Writes a single word to flash memory. Data to write must be aligned to
 *   words and contain a number of bytes that is divisable by four.
 * @note
 *   This is a modified version of the write code from the emlib
 *   (em_msc.c). It also sets up interrupts to return after a completed
 *   operation, and can therefore go to sleep during the flash operation.
 *
 *   This implementation currently lacks timeout functionality since it is
 *   asleep. This could be fixed using a wake-up timer. This should be
 *   implemented using the  timer library, but could also be an integrated
 *   part of the  sleep functionality.
 *
 *   The flash must be erased prior to writing a new word.
 *   This function must be run from RAM. Failure to execute this portion
 *   of the code in RAM will result in a hardfault. For IAR, Rowley and
 *   Codesourcery this will be achieved automatically. For Keil uVision 4 you
 *   must define a section called "ram_code" and place this manually in your
 *   project's scatter file.
 *
 * @param[in] address
 *   Pointer to the flash word to write to. Must be aligned to words.
 * @param[in] data
 *   Data to write to flash.
 * @param[in] numBytes
 *   Number of bytes to write from flash. NB: Must be divisable by four.
 * @return
 *   Returns the status of the write operation, #msc_Return_TypeDef
 * @verbatim
 *   flashReturnOk - Operation completed successfully.
 *   flashReturnInvalidAddr - Operation tried to erase a non-flash area.
 *   flashReturnLocked - Operation tried to erase a locked area of the flash.
 *   flashReturnTimeOut - Operation timed out waiting for flash operation
 *       to complete.
 * @endverbatim
 ******************************************************************************/
#ifdef __CC_ARM  /* MDK-ARM compiler */
#pragma arm section code="ram_code"
#endif /* __CC_ARM */
static msc_Return_TypeDef NVMHAL_MSC_WriteWord(uint32_t *address, void const *data, uint32_t numBytes)
{
  uint32_t timeOut;
  uint32_t wordCount;
  uint32_t numWords;

  /* Enable writing to the MSC */
  MSC->WRITECTRL |= MSC_WRITECTRL_WREN;

  /* Convert bytes to words */
  numWords = numBytes >> 2;

  for (wordCount = 0; wordCount < numWords; wordCount++)
  {
    /* Load address */
    MSC->ADDRB    = (uint32_t)(address + wordCount);
    MSC->WRITECMD = MSC_WRITECMD_LADDRIM;

    /* Check for invalid address */
    if (MSC->STATUS & MSC_STATUS_INVADDR)
    {
      /* Disable writing to the MSC */
      MSC->WRITECTRL &= ~MSC_WRITECTRL_WREN;
      return mscReturnInvalidAddr;
    }

    /* Check for write protected page */
    if (MSC->STATUS & MSC_STATUS_LOCKED)
    {
      /* Disable writing to the MSC */
      MSC->WRITECTRL &= ~MSC_WRITECTRL_WREN;
      return mscReturnLocked;
    }

    /* Wait for the MSC to be ready for a new data word */
    /* Due to the timing of this function, the MSC should already by ready */
    timeOut = MSC_PROGRAM_TIMEOUT;
    while (((MSC->STATUS & MSC_STATUS_WDATAREADY) == 0) && (timeOut != 0))
    {
      timeOut--;
    }

    /* Check for timeout */
    if (timeOut == 0)
    {
      /* Disable writing to the MSC */
      MSC->WRITECTRL &= ~MSC_WRITECTRL_WREN;
      return mscReturnTimeOut;
    }

    /* Load data into write data register */
    MSC->WDATA = *(((uint32_t *) data) + wordCount);

    /* Set up interrupt. */
    MSC->IFC                                = MSC_IEN_WRITE;
    MSC->IEN                               |= MSC_IEN_WRITE;
    NVIC->ISER[((uint32_t)(MSC_IRQn) >> 5)] = (1 << ((uint32_t)(MSC_IRQn) & 0x1F));

    /* Set active flag. */
    NVMHAL_FlashTransferActive = true;

    /* Trigger write once. */
    MSC->WRITECMD = MSC_WRITECMD_WRITEONCE;

    /* Go to sleep and wait in a loop for the write operation to finish. Here
     * it is necessary to turn on and off interrupts in an interesting way to
     * avoid certain race conditions that might happen if the operation finishes
     * between the while is evaluated and the MCU is put in EM1.
     *
     * Short low level calls are used since this is a RAM function and we are
     * not allowed to call external functions. The functions we need are marked
     * as inline, but if the optimizer is turned off these are not inlined and
     * stuff hardfaults. */

    /* Turn off interrupts, so that we cannot get an interrupt during the while
     * evaluation. */
    INT_Disable();
    /* Wait for the write to complete */
    while ((MSC->STATUS & MSC_STATUS_BUSY) && NVMHAL_FlashTransferActive)
    {
      /* Just enter Cortex-M3 sleep mode. If there has already been an interrupt
       * we will wake up again immediately. */
      SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;
      __WFI();
      /* Enable interrupts again to run interrupt functions. */
      INT_Enable();
      /* Return to disabled before re-evaluating the while condition. */
      INT_Disable();
    }
    /* Re-enable interrupts. */
    INT_Enable();

    /* We might have passed through the loop due to the status value, so reset
     * the flag here in case it hasn't been done yet. */
    NVMHAL_FlashTransferActive = false;

    /* Clear interrupt. */
    MSC->IFC = MSC_IEN_WRITE;
  }

  /* Disable writing to the MSC */
  MSC->WRITECTRL &= ~MSC_WRITECTRL_WREN;
  return mscReturnOk;
}
#ifdef __CC_ARM  /* MDK-ARM compiler */
#pragma arm section code
#endif /* __CC_ARM */

#endif

#if (NVMHAL_SLEEP == true)
/***************************************************************************//**
 * @brief
 *   Erases a page in flash memory.
 * @note
 *   This is a modified version of the page erase code from the emlib
 *   (em_msc.c). It also sets up interrupts to return after a completed
 *   operation, and can therefore go to sleep during the flash operation.
 *
 *   This implementation currently lacks timeout functionality since it is
 *   asleep. This could be fixed using a wake-up timer. This should be
 *   implemented using the  timer library, but could also be an integrated
 *   part of the  sleep functionality.
 *
 *   This function MUST be executed from RAM. Failure to execute this portion
 *   of the code in RAM will result in a hardfault. For IAR, Rowley and
 *   Codesourcery this will be achieved automatically. For Keil uVision 4 you
 *   must define a section called "ram_code" and place this manually in your
 *   project's scatter file.
 * @param[in] startAddress
 *   Pointer to the flash page to erase. Must be aligned to beginning of page
 *   boundary.
 * @return
 *   Returns the status of erase operation, #msc_Return_TypeDef
 * @verbatim
 *   flashReturnOk - Operation completed successfully.
 *   flashReturnInvalidAddr - Operation tried to erase a non-flash area.
 *   flashReturnLocked - Operation tried to erase a locked area of the flash.
 *   flashReturnTimeOut - Operation timed out waiting for flash operation
 *       to complete.
 * @endverbatim
 ******************************************************************************/
#ifdef __CC_ARM  /* MDK-ARM compiler */
#pragma arm section code="ram_code"
#endif /* __CC_ARM */
static msc_Return_TypeDef NVMHAL_MSC_ErasePage(uint32_t *startAddress)
{
  /* Enable writing to the MSC */
  MSC->WRITECTRL |= MSC_WRITECTRL_WREN;

  /* Load address */
  MSC->ADDRB    = (uint32_t) startAddress;
  MSC->WRITECMD = MSC_WRITECMD_LADDRIM;

  /* Check for invalid address */
  if (MSC->STATUS & MSC_STATUS_INVADDR)
  {
    /* Disable writing to the MSC */
    MSC->WRITECTRL &= ~MSC_WRITECTRL_WREN;
    return mscReturnInvalidAddr;
  }

  /* Check for write protected page */
  if (MSC->STATUS & MSC_STATUS_LOCKED)
  {
    /* Disable writing to the MSC */
    MSC->WRITECTRL &= ~MSC_WRITECTRL_WREN;
    return mscReturnLocked;
  }

  /* Set up interrupt. */
  MSC->IFC = MSC_IEN_ERASE;
  MSC->IEN |= MSC_IEN_ERASE;
  NVIC->ISER[((uint32_t)(MSC_IRQn) >> 5)] = (1 << ((uint32_t)(MSC_IRQn) & 0x1F));

  NVMHAL_FlashTransferActive = true;

  /* Send erase page command */
  MSC->WRITECMD = MSC_WRITECMD_ERASEPAGE;

  INT_Disable();
  /* Wait for the erase to complete */
  while ((MSC->STATUS & MSC_STATUS_BUSY) && NVMHAL_FlashTransferActive)
  {
    /* Just enter Cortex-M3 sleep mode */
    SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;
    __WFI();
    INT_Enable();
    INT_Disable();
  }
  INT_Enable();

  NVMHAL_FlashTransferActive = false;

  /* Clear interrupt. */
  MSC->IFC = MSC_IEN_ERASE;

  /* Disable writing to the MSC */
  MSC->WRITECTRL &= ~MSC_WRITECTRL_WREN;
  return mscReturnOk;
}
#ifdef __CC_ARM  /* MDK-ARM compiler */
#pragma arm section code
#endif /* __CC_ARM */

#endif

/** @endcond */

/***************************************************************************//**
 * @brief
 *   Convert return type.
 *
 * @details
 *   This function converts between the return type of the emlib and the
 *   NVM API.
 *
 * @param[in] result
 *   Operation result in a msc_Return_TypeDef.
 *
 * @return
 *   Returns the corresponding Ecode_t.
 ******************************************************************************/
static Ecode_t NVMHAL_ReturnTypeConvert(msc_Return_TypeDef result)
{
  /* Direct return from switch gives smallest code size (smaller than if-else).
   * Could try using an offset value to translate. */
  switch (result)
  {
  case mscReturnOk:
    return ECODE_EMDRV_NVM_OK;
  case mscReturnInvalidAddr:
    return ECODE_EMDRV_NVM_ADDR_INVALID;
  case mscReturnUnaligned:
    return ECODE_EMDRV_NVM_ALIGNMENT_INVALID;
  default:
    return ECODE_EMDRV_NVM_ERROR;
  }
}

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *   Initialize NVM driver.
 *
 * @details
 *   This function is run upon initialization, at least once before any other
 *   functions. It can be used to call necessary startup routines before the
 *   hardware can be accessed.
 ******************************************************************************/
void NVMHAL_Init(void)
{
  MSC_Init();

#if (NVMHAL_DMAREAD == true)
  /* Enable the DMA clock */
  CMU_ClockEnable(cmuClock_DMA, true);

  /* Initialize DMA */
  DMA_Init_TypeDef dmaInit;
  dmaInit.hprot        = 0;
  dmaInit.controlBlock = NVMHAL_dmaControlBlock;
  DMA_Init(&dmaInit);
#endif
}

/***************************************************************************//**
 * @brief
 *   De-initialize NVM .
 *
 * @details
 *   This function is run when the API deinit function is run. This should be
 *   done before any graceful halts.
 ******************************************************************************/
void NVMHAL_DeInit(void)
{
  MSC_Deinit();
}

/***************************************************************************//**
 * @brief
 *   Read data from NVM.
 *
 * @details
 *   This function is used to read data from the NVM hardware. It should be a
 *   blocking call, since the thread asking for data to be read cannot continue
 *   without the data.
 *
 *   Another requirement is the ability to read unaligned blocks of data with
 *   single byte precision.
 *
 * @param[in] *pAddress
 *   Memory address in hardware for the data to read.
 *
 * @param[in] *pObject
 *   RAM buffer to store the data from NVM.
 *
 * @param[in] len
 *   The length of the data.
 ******************************************************************************/
void NVMHAL_Read(uint8_t *pAddress, void *pObject, uint16_t len)
{
#if (NVMHAL_DMAREAD == true)
  /* Setting call-back function */
  DMA_CB_TypeDef cb[DMA_CHAN_COUNT];
  cb[NVMHAL_DMA_CHANNEL_FLASH].cbFunc = NVM_ReadFromFlashComplete;

  /* usrPtr can be used to send data to the callback function,
   * but this is not used here, which is indicated by the NULL pointer */
  cb[NVMHAL_DMA_CHANNEL_FLASH].userPtr = NULL;

  /* Setting up channel */
  DMA_CfgChannel_TypeDef chnlCfg;
  chnlCfg.highPri   = false;
  chnlCfg.enableInt = true;
  chnlCfg.select    = 0;
  chnlCfg.cb        = &(cb[NVMHAL_DMA_CHANNEL_FLASH]);
  DMA_CfgChannel(NVMHAL_DMA_CHANNEL_FLASH, &chnlCfg);

  /* Setting up transfer descriptor */
  DMA_CfgDescr_TypeDef descrCfg;
  descrCfg.dstInc  = dmaDataInc1;
  descrCfg.srcInc  = dmaDataInc1;
  descrCfg.size    = dmaDataSize1;
  descrCfg.arbRate = dmaArbitrate1;
  descrCfg.hprot   = 0;
  DMA_CfgDescr(NVMHAL_DMA_CHANNEL_FLASH, true, &descrCfg);

  /* Setting flag to indicate that transfer is in progress
   * will be cleared by call-back function */
  NVMHAL_FlashTransferActive = true;

  /* Starting the transfer. Using Auto (all data is transfered at once) */
  DMA_ActivateAuto(NVMHAL_DMA_CHANNEL_FLASH,
                   true,
                   pObject,
                   pAddress,
                   len - 1);

  /* Entering EM1 to wait for completion (the DMA requires EM1) */
  INT_Disable();
  while (NVMHAL_FlashTransferActive)
  {
    EMU_EnterEM1();
    INT_Enable();
    INT_Disable();
  }
  INT_Enable();
#else
  /* Create a pointer to the void* pBuffer with type for easy movement. */
  uint8_t *pObjectInt = (uint8_t*) pObject;

  while (0 < len) /* While there is more data to fetch. */
  {
    /* Move the data from memory to the buffer. */
    *pObjectInt = *pAddress;
    /* Move active location in buffer, */
    ++pObjectInt;
    /* and in memory. */
    ++pAddress;
    /* More data is read. */
    --len;
  }
#endif

}

/***************************************************************************//**
 * @brief
 *   Write data to NVM.
 *
 * @details
 *   This function is used to write data from the NVM. It should be a
 *   blocking call. This may be changed in the future, but this is currently a
 *   requirement made due to the design of the API code.
 *
 *   Another requirement is the ability to write unaligned blocks of data with
 *   single byte precision.
 *
 * @param[in] *pAddress
 *   Memory address to write to.
 *
 * @param[in] *pObject
 *   Pointer to data to write.
 *
 * @param[in] len
 *   The length of the data.
 *
 * @return
 *   Returns the result of the write operation using a Ecode_t.
 ******************************************************************************/
Ecode_t NVMHAL_Write(uint8_t *pAddress, void const *pObject, uint16_t len)
{
  /* Used to carry return data. */
  msc_Return_TypeDef msc_Return = mscReturnOk;
  /* Used as a temporary variable to create the blocks to write when padding to closest word. */
  uint32_t tempWord;

  /* Pointer to let us traverse the given pObject by single bytes. */
  uint8_t *pObjectInt = (uint8_t*) pObject;

  /* Temporary variable to cache length of padding needed. */
  uint8_t padLen;

  /* Pad in front. */
  padLen = (uint32_t) pAddress % sizeof(tempWord);

  if (padLen != 0)
  {
    pAddress -= padLen;

    /* Get first word. */
    tempWord = *(uint32_t *) pObjectInt;
    /* Shift to offset. */
    tempWord = tempWord << 8 * padLen;
    /* Add nochanging padding. */
    tempWord |= NVMHAL_FFFFFFFF >> (8 * (sizeof(tempWord) - padLen));

    /* Special case for unaligned writes smaller than 1 word. */
    if (len < sizeof(tempWord) - padLen)
    {
      /* Add nochanging padding. */
      tempWord |= NVMHAL_FFFFFFFF << (8 * (padLen + len));
      len       = 0;
    }
    else
    {
      len -= sizeof(tempWord) - padLen;
    }

#if (NVMHAL_SLEEP_WRITE == true)
    msc_Return = NVMHAL_MSC_WriteWord((uint32_t *) pAddress, &tempWord, sizeof(tempWord));
#else
    msc_Return = MSC_WriteWord((uint32_t *) pAddress, &tempWord, sizeof(tempWord));
#endif
    pObjectInt += sizeof(tempWord) - padLen;
    pAddress   += sizeof(tempWord);
  }

  /* Loop over body. */
  while ((len >= sizeof(tempWord)) && (mscReturnOk == msc_Return))
  {
#if (NVMHAL_SLEEP_WRITE == true)
    msc_Return = NVMHAL_MSC_WriteWord((uint32_t *) pAddress, pObjectInt, sizeof(tempWord));
#else
    msc_Return = MSC_WriteWord((uint32_t *) pAddress, pObjectInt, sizeof(tempWord));
#endif
    pAddress   += sizeof(tempWord);
    pObjectInt += sizeof(tempWord);
    len        -= sizeof(tempWord);
  }

  /* Pad in back. */
  if ((len > 0) && (mscReturnOk == msc_Return))
  {
    /* Get final word. */
    tempWord = *(uint32_t *) pObjectInt;
    /* Fill rest of word with padding. */
    tempWord |= NVMHAL_FFFFFFFF << (8 * len);

#if (NVMHAL_SLEEP_WRITE == true)
    msc_Return = NVMHAL_MSC_WriteWord((uint32_t *) pAddress, &tempWord, sizeof(tempWord));
#else
    msc_Return = MSC_WriteWord((uint32_t *) pAddress, &tempWord, sizeof(tempWord));
#endif
  }

  /* Convert between return types, and return. */
  return NVMHAL_ReturnTypeConvert(msc_Return);
}

/***************************************************************************//**
 * @brief
 *   Erase a page in the NVM.
 *
 * @details
 *   This function is used to erase a page in the NVM (typically an aligned
 *   512 byte block of memory). It is important that the size and alignment of
 *   the page erased is the same as specified in the configuration.
 *
 * @param[in] *pAddress
 *   Memory address pointing to the start of the page to erase.
 *
 * @return
 *   Returns the result of the erase operation using a Ecode_t.
 ******************************************************************************/
#if (NVMHAL_SLEEP == true)
Ecode_t NVMHAL_PageErase(uint8_t *pAddress)
{
  /* Call underlying library and convert between return types, and return. */
  return NVMHAL_ReturnTypeConvert(NVMHAL_MSC_ErasePage((uint32_t *) pAddress));
}

#else
Ecode_t NVMHAL_PageErase(uint8_t *pAddress)
{
  /* Call underlying library and convert between return types, and return. */
  return NVMHAL_ReturnTypeConvert(MSC_ErasePage((uint32_t *) pAddress));
}
#endif

/***************************************************************************//**
 * @brief
 *   Calculate checksum according to CCITT CRC16.
 *
 * @details
 *   This function calculates a checksum of the supplied buffer.
 *   The checksum is calculated using CCITT CRC16 plynomial x^16+x^12+x^5+1.
 *
 *   This functionality is also present internally in the API, but is duplicated
 *   here to allow for much more efficient calculations specific to the
 *   hardware.
 *
 * @param[in] pChecksum
 *   Pointer to where the checksum should be calculated and stored. This buffer
 *   should be initialized. A good consistent starting point would be
 *   NVM_CHECKSUM_INITIAL.
 *
 * @param[in] pMemory
 *   Pointer to the data you want to calculate a checksum for.
 *
 * @param[in] len
 *   The length of the data.
 ******************************************************************************/
void NVMHAL_Checksum(uint16_t *pChecksum, void *pMemory, uint16_t len)
{
  uint8_t *pointer = (uint8_t *) pMemory;
  uint16_t crc = *pChecksum;

  while(len--)
  {
    crc = (crc >> 8) | (crc << 8);
    crc ^= *pointer++;
    crc ^= (crc & 0xf0) >> 4;
    crc ^= (crc & 0x0f) << 12;
    crc ^= (crc & 0xff) << 5;
  }

  *pChecksum = crc;
}
