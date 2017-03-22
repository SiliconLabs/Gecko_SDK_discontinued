/** @file hal/micro/cortexm3/mfg-token.c
 * @brief Cortex-M3 Manufacturing-Token system
 *
 * <!-- Copyright 2014 Silicon Laboratories, Inc.                        *80*-->
 */
#include PLATFORM_HEADER
#include "include/error.h"
#include "hal/micro/cortexm3/flash.h"
#include "mfg-token.h"

//[[ mfg token addresses are defined as constant variables and not enums so that 
//   the stack can be built without knowing the precise locations.
//]]
#define DEFINETOKENS
#define TOKEN_MFG(name,creator,iscnt,isidx,type,arraysize,...) \
  const uint16_t TOKEN_##name = TOKEN_##name##_ADDRESS;
  #include "hal/micro/cortexm3/token-manufacturing.h"
#undef TOKEN_DEF
#undef TOKEN_MFG
#undef DEFINETOKENS

//[[ Strip emulator only code from official build
#ifdef EMBER_EMU_TEST
  extern bool emuFib0x00;
  extern bool emuCib0x00;
#endif //EMBER_EMU_TEST
//]]

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
    //There are two EUI64's stored in the Info Blocks, Ember and Custom.
    //0x0A00 is the address used by the generic EUI64 token, and it is
    //token.c's responbility to pick the returned EUI64 from either Ember
    //or Custom.  Return the Custom EUI64 if it is not all FF's, otherwise
    //return the Ember EUI64.
    tokTypeMfgEui64 eui64;
    halCommonGetMfgToken(&eui64, TOKEN_MFG_CUSTOM_EUI_64);
    if(MEMCOMPARE(eui64,nullEui, 8 /*EUI64_SIZE*/) == 0) {
      halCommonGetMfgToken(&eui64, TOKEN_MFG_EMBER_EUI_64);
    }
    MEMCOPY(ram, eui64, 8 /*EUI64_SIZE*/);
  } else {
    //read from the Information Blocks.  The token ID is only the
    //bottom 16bits of the token's actual address.  Since the info blocks
    //exist in the range DATA_BIG_INFO_BASE-DATA_BIG_INFO_END, we need
    //to OR the ID with DATA_BIG_INFO_BASE to get the real address.
    uint32_t realAddress = (DATA_BIG_INFO_BASE|token) + (len*index);
    uint8_t *flash = (uint8_t *)realAddress;
    //[[ Strip emulator only code from official build
    #ifdef EMBER_EMU_TEST
      //If we're on an emulator and the MFG area is 0x00, fake the erased
      //state of 0xFF.  Some builds (as I discovered with serial-*-bootloader)
      //don't have the full token system that would have defined emu*ib0x00
      //when building for EMBER_EMU_TEST.
      #ifndef emuFib0x00
        bool emuFib0x00 = true;
      #endif
      #ifndef emuCib0x00
        bool emuCib0x00 = true;
      #endif
      if((emuFib0x00) && (token<0x0800)) {
        MEMSET(ram, 0xFF, len);
        return;
      }
      if((emuCib0x00) && (token>=0x0800)) {
        MEMSET(ram, 0xFF, len);
        return;
      }
    #endif //EMBER_EMU_TEST
    //]]
    MEMCOPY(ram, flash, len);
  }
}


void halInternalSetMfgTokenData(uint16_t token, void *data, uint8_t len)
{
  EmberStatus flashStatus;
  uint32_t realAddress = (DATA_BIG_INFO_BASE|token);
  uint8_t * flash = (uint8_t *)realAddress;
  uint32_t i;
  
  //The flash library (and hardware) requires the address and length to both
  //be multiples of 16bits.  Since this API is only valid for writing to
  //the CIB, verify that the token+len falls within the CIB.
  assert((token&1) != 1);
  assert((len&1) != 1);
  assert((realAddress>=CIB_BOTTOM) && ((realAddress+len-1)<=CIB_TOP));
  
  //CIB manufacturing tokens can only be written by on-chip code if the token
  //is currently unprogrammed.  Verify the entire token is unwritten.  The
  //flash library performs a similar check, but verifying here ensures that
  //the entire token is unprogrammed and will prevent partial writes.
  for(i=0;i<len;i++) {
    assert(flash[i] == 0xFF);
  }
  
  //Remember, the flash library operates in 16bit quantities, but the
  //token system operates in 8bit quantities.  Hence the divide by 2.
  flashStatus = halInternalFlashWrite(realAddress, data, (len/2));
  assert(flashStatus == EMBER_SUCCESS);
}

