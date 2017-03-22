/** @file hal/micro/cortexm3/efm32/mfg-token.c
 * @brief EFM32 Cortex-M3 Manufacturing-Token system
 *
 * <!-- Copyright 2015 Silicon Laboratories, Inc.                        *80*-->
 */
#include PLATFORM_HEADER
#include "include/error.h"
#include "hal/micro/cortexm3/flash.h"
#include "mfg-token.h"


#define DEFINETOKENS
#define TOKEN_MFG(name,creator,iscnt,isidx,type,arraysize,...) \
  const uint16_t TOKEN_##name = TOKEN_##name##_ADDRESS;
  #include "hal/micro/cortexm3/efm32/token-manufacturing.h"
#undef TOKEN_DEF
#undef TOKEN_MFG
#undef DEFINETOKENS


static const uint8_t nullEui[] = { 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF };


void halInternalGetMfgTokenData(void *data,
                                uint16_t token,
                                uint8_t index,
                                uint8_t len)
{
  uint8_t *ram = (uint8_t*)data;

  //0x7F is a non-indexed token.  Remap to 0 for the address calculation
  index = (index==0x7F) ? 0 : index;
  
  if(token == MFG_EUI_64_LOCATION) {
    //There are two EUI64's stored in the flash, Ember and Custom.
    //MFG_EUI_64_LOCATION is the address used by the generic EUI64 token.
    //It is the low level routine's responbility to pick and return EUI64 from
    //either Ember or Custom.  Return the Custom EUI64 if it is not all FF's,
    //otherwise return the Ember EUI64.
    tokTypeMfgEui64 eui64;
    halCommonGetMfgToken(&eui64, TOKEN_MFG_CUSTOM_EUI_64);
    if(MEMCOMPARE(eui64,nullEui, 8 /*EUI64_SIZE*/) == 0) {
      halCommonGetMfgToken(&eui64, TOKEN_MFG_EMBER_EUI_64);
    }
    MEMCOPY(ram, eui64, 8 /*EUI64_SIZE*/);
  } else if(token == MFG_EMBER_EUI_64_LOCATION) {
    uint32_t low = (DEVINFO->UNIQUEL);
    uint32_t high = (DEVINFO->UNIQUEH);
    ram[0] = (low>>0)&0xFF;
    ram[1] = (low>>8)&0xFF;
    ram[2] = (low>>16)&0xFF;
    ram[3] = (low>>24)&0xFF;
    ram[4] = (high>>0)&0xFF;
    ram[5] = (high>>8)&0xFF;
    ram[6] = (high>>16)&0xFF;
    ram[7] = (high>>24)&0xFF;
  } else if((token&0xF000) == (USERDATA_TOKENS&0xF000) ) {
    uint32_t realAddress = ((USERDATA_BASE | (token&0x0FFF)) + (len*index));
    uint8_t *flash = (uint8_t *)realAddress;

    MEMCOPY(ram, flash, len);
  } else if( ((token&0xF000) == (LOCKBITS_TOKENS&0xF000))
             || ((token&0xF000) == (LOCKBITSDATA_TOKENS&0xF000)) ) {
    uint32_t realAddress = ((LOCKBITS_BASE | (token&0x0FFF)) + (len*index));
    uint8_t *flash = (uint8_t *)realAddress;

    MEMCOPY(ram, flash, len);
  }
}


void halInternalSetMfgTokenData(uint16_t token, void *data, uint8_t len)
{
  EmberStatus flashStatus = EMBER_ERR_FATAL;
  uint32_t realAddress=0;
  //Initializing to a high memory address adds protection by causing a
  //hardfault if accidentally used.
  uint8_t *flash = (uint8_t *)0xFFFFFFF0;
  uint32_t i;
  //The flash library requires the address and length to both
  //be multiples of 16bits.  Since this API is only valid for writing to
  //the UserPage or LockBits page, verify that the token+len falls within
  //acceptable addresses..
  assert((token&1) != 1);
  assert((len&1) != 1);

  if((token&0xF000) == (USERDATA_TOKENS&0xF000) ) {
    realAddress = ((USERDATA_BASE | (token&0x0FFF)));
    flash = (uint8_t *)realAddress;
    assert((realAddress>=USERDATA_BASE) && ((realAddress+len-1)<(USERDATA_BASE+FLASH_PAGE_SIZE)));
  } else if( ((token&0xF000) == (LOCKBITS_TOKENS&0xF000))
             || ((token&0xF000) == (LOCKBITSDATA_TOKENS&0xF000)) ) {
    realAddress = ((LOCKBITS_BASE | (token&0x0FFF)));
    flash = (uint8_t *)realAddress;
  } else {
    //in coming token parameter is an illegal address. 
    assert(0);
  }
  
  //UserData and LockBits manufacturing tokens can only be written by on-chip
  //code if the token is currently unprogrammed.  Verify the entire token is
  //unwritten.  The flash library performs a similar check, but verifying here
  //ensures that the entire token is unprogrammed and will prevent partial
  //writes.
  //NOTE:  On chip code CANNOT erase LockBits Page.
  for(i=0;i<len;i++) {
    assert(flash[i] == 0xFF);
  }
  
  //Remember, the flash library's token support operates in 16bit quantities,
  //but the token system operates in 8bit quantities.  Hence the divide by 2.
  //NOTE: The actual flash for the EFM32 device prefer to work with 32b writes,
  //but 16b is possible and supported by the flash driver - primarily for the
  //purposes of the SimEE/Token system that likes to use 8b and 16b.
  flashStatus = halInternalFlashWrite(realAddress, data, (len/2));
  assert(flashStatus == EMBER_SUCCESS);
}

