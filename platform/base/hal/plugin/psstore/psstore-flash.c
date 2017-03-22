//The bulk of this file is copied from the old nvm_hal driver. 

#include <stdint.h>
#include "em_msc.h"
#include "ecode.h"


#define ECODE_EMDRV_NVM_OK                      ( ECODE_OK )                                    ///< Success return value
#define ECODE_EMDRV_NVM_ADDR_INVALID            ( ECODE_EMDRV_NVM_BASE | 0x00000001 )        ///< Invalid address
#define ECODE_EMDRV_NVM_ALIGNMENT_INVALID       ( ECODE_EMDRV_NVM_BASE | 0x00000002 )        ///< Invalid data alignment
#define ECODE_EMDRV_NVM_DATA_INVALID            ( ECODE_EMDRV_NVM_BASE | 0x00000003 )        ///< Invalid input data or format
#define ECODE_EMDRV_NVM_WRITE_LOCK              ( ECODE_EMDRV_NVM_BASE | 0x00000004 )        ///< A write is currently in progress
#define ECODE_EMDRV_NVM_NO_PAGES_AVAILABLE      ( ECODE_EMDRV_NVM_BASE | 0x00000005 )        ///< Initialization didn't find any pages available to allocate
#define ECODE_EMDRV_NVM_PAGE_INVALID            ( ECODE_EMDRV_NVM_BASE | 0x00000006 )        ///< Could not find the page specified
#define ECODE_EMDRV_NVM_ERROR                   ( ECODE_EMDRV_NVM_BASE | 0x00000007 )        ///< General error


#define NVMHAL_FFFFFFFF      0xffffffffUL


static Ecode_t returnTypeConvert(MSC_Status_TypeDef result)
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


static uint32_t readUnalignedWord(uint8_t *addr)
{
  /* Check if the unaligned access trap is enabled */
  if (SCB->CCR & SCB_CCR_UNALIGN_TRP_Msk)
  {
    /* Read word as bytes (always aligned) */
    return (*(addr + 3) << 24) | (*(addr + 2) << 16) | (*(addr + 1) << 8) | *addr;
  }
  else
  {
    /* Use unaligned access */
    return *(uint32_t *)addr;
  }
}


int flash_init()
{
    MSC_Init();
    return 0;
}


int flash_erase_page(void *pAddress)
{
    return returnTypeConvert(MSC_ErasePage((uint32_t *) pAddress));
}

int flash_write_buf(uint32_t *pAddress,const uint32_t *pObject, uint32_t len)
{
  /* Used to carry return data. */
  MSC_Status_TypeDef mscStatus = mscReturnOk;
  /* Used as a temporary variable to create the blocks to write when padding to closest word. */
  uint32_t tempWord;

  /* Pointer to let us traverse the given pObject by single bytes. */
  uint8_t *pObjectInt = (uint8_t*)pObject;

  /* Temporary variable to cache length of padding needed. */
  uint8_t padLen;

  /* Get length of pad in front. */
  padLen = (uint32_t) pAddress % sizeof(tempWord);

  if (padLen != 0)
  {
    /* Get first word. */
    tempWord = readUnalignedWord((uint8_t *)pObject);

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

    /* Align the NVM write address */
    pAddress -= padLen;
    mscStatus = MSC_WriteWord((uint32_t *) pAddress, &tempWord, sizeof(tempWord));

    pObjectInt += sizeof(tempWord) - padLen;
    pAddress   += sizeof(tempWord);
  }


  /* Loop over body. */
  while ((len >= sizeof(tempWord)) && (mscReturnOk == mscStatus))
  {
    tempWord = readUnalignedWord((uint8_t *)pObjectInt);
    mscStatus = MSC_WriteWord((uint32_t *) pAddress, &tempWord, sizeof(tempWord));

    pAddress   += sizeof(tempWord);
    pObjectInt += sizeof(tempWord);
    len        -= sizeof(tempWord);
  }

  /* Pad at the end */
  if ((len > 0) && (mscReturnOk == mscStatus))
  {
    /* Get final word. */
    tempWord = readUnalignedWord((uint8_t *)pObjectInt);

    /* Fill rest of word with padding. */
    tempWord |= NVMHAL_FFFFFFFF << (8 * len);
    mscStatus = MSC_WriteWord((uint32_t *) pAddress, &tempWord, sizeof(tempWord));
  }

  /* Convert between return types, and return. */
  return returnTypeConvert(mscStatus);
}


int flash_write_u32(uint32_t *address,uint32_t val)
{
    return   flash_write_buf(address,&val,sizeof(uint32_t));
}


