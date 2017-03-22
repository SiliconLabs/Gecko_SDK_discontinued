/** @file hal/micro/cortexm3/efm32/token.c
 * @brief Token implementation.
 * See token.h for details.
 *
 * <!-- Copyright 2014 Silicon Laboratories, Inc.                        *80*-->
 */
#include PLATFORM_HEADER

#include "stack/include/ember.h"
#include "stack/include/error.h"
#include "hal/hal.h"
#include "serial/serial.h"
#include "hal/plugin/sim-eeprom/sim-eeprom.h"

#ifndef SER232
  #define SER232 0
#endif

//prints debug data from the token access functions
#define TOKENDBG(x)
//#define TOKENDBG(x) x

bool tokensActive = false;


EmberStatus halStackInitTokens(void)
{
  tokensActive = true;
  if(halInternalSimEeInit()!=EMBER_SUCCESS) {
    TOKENDBG(emberSerialPrintf(SER232,"halInternalSimEeInit Attempt 1 fail\r\n");)
    if(halInternalSimEeInit()!=EMBER_SUCCESS) {
      TOKENDBG(emberSerialPrintf(SER232,"halInternalSimEeInit Attempt 2 fail\r\n");)
      tokensActive = false;
      return EMBER_SIM_EEPROM_INIT_2_FAILED;
     }
    TOKENDBG(} else {emberSerialPrintf(SER232,"halInternalSimEeInit Successful\r\n");)
  }

  #if !defined(BOOTLOADER) && !defined(EMBER_TEST)
  {
    tokTypeStackNvdataVersion tokStack;
    halCommonGetToken(&tokStack, TOKEN_STACK_NVDATA_VERSION);

    if (CURRENT_STACK_TOKEN_VERSION != tokStack) {
      #if defined(DEBUG)
        //Debug images with a bad stack token version should attempt to
        //fix the SimEE before continuing on.
        TOKENDBG(emberSerialPrintf(SER232,"Stack Version mismatch, reloading\r\n");)
        halInternalSimEeRepair(true);
        if(halInternalSimEeInit()!=EMBER_SUCCESS) {
          TOKENDBG(emberSerialPrintf(SER232,"halInternalSimEeInit Attempt 3 fail\r\n");)
          tokensActive = false;
          return EMBER_SIM_EEPROM_INIT_3_FAILED;
        }
      #else //  node release image
        TOKENDBG(emberSerialPrintf(SER232,"EEPROM_x_VERSION_MISMATCH (%d)\r\n",
                 EMBER_EEPROM_STACK_VERSION_MISMATCH);)
        tokensActive = false;
        #if defined(PLATFORMTEST) || defined(LEVEL_ONE_TEST)
          tokensActive = true;  //Keep tokens active for test code.
        #endif //defined(PLATFORMTEST)
        return EMBER_EEPROM_STACK_VERSION_MISMATCH;
      #endif
    }
  }
  #endif //!defined(BOOTLOADER) && !defined(EMBER_TEST)

  TOKENDBG(emberSerialPrintf(SER232,"TOKENS ACTIVE\r\n");)
  return EMBER_SUCCESS;
}


void halInternalGetTokenData(void *data, uint16_t token, uint8_t index, uint8_t len)
{
  if(token < 256) {
    //the token ID is within the SimEEPROM's range, route to the SimEEPROM
    if(tokensActive) {
      halInternalSimEeGetData(data, token, index, len);
    } else {
      TOKENDBG(emberSerialPrintf(SER232, "getIdxToken supressed.\r\n");)
    }
  } else {
    #ifdef EMBER_TEST
      assert(false);
    #else //EMBER_TEST
      halInternalGetMfgTokenData(data,token,index,len);
    #endif //EMBER_TEST
  }
}


void halInternalGetIdxTokenPtr(void *ptr, uint16_t token, uint8_t index, uint8_t len)
{
  if(token < 256) {
    //the token is within the SimEEPROM's range, route to the SimEEPROM
    if(tokensActive) {
      halInternalSimEeGetPtr(ptr, token, index, len);
    } else {
      TOKENDBG(emberSerialPrintf(SER232, "getIdxToken supressed.\r\n");)
    }
  } else {
    #ifdef EMBER_TEST
      assert(false);
    #else //EMBER_TEST
      uint32_t *ptrOut = (uint32_t *)ptr;
      uint32_t realAddress=0;

      //0x7F is a non-indexed token.  Remap to 0 for the address calculation
      index = (index==0x7F) ? 0 : index;

      if((token&0xF000) == (USERDATA_TOKENS&0xF000) ) {
        realAddress = ((USERDATA_BASE | (token&0x0FFF)) + (len*index));
      } else if((token&0xF000) == (LOCKBITSDATA_TOKENS&0xF000) ) {
        realAddress = ((LOCKBITS_BASE | (token&0x0FFF)) + (len*index));
      } else {
        //This function must only ever be called from token code that passes
        //a proper "token" parameter.  A valid 16bit token must pass the
        //above check to find the 32bit realAddress.
        assert(0);
      }

      *ptrOut = realAddress;
    #endif //EMBER_TEST
  }
}


bool simEeSetDataActiveSemaphore = false;
void halInternalSetTokenData(uint16_t token, uint8_t index, void *data, uint8_t len)
{
  if(token < 256) {
    //the token ID is within the SimEEPROM's range, route to the SimEEPROM
    if(tokensActive) {
      //You cannot interrupt SetData with another SetData!
      assert(!simEeSetDataActiveSemaphore);
      simEeSetDataActiveSemaphore = true;
      halInternalSimEeSetData(token, data, index, len);
      simEeSetDataActiveSemaphore = false;
    } else {
      TOKENDBG(emberSerialPrintf(SER232, "setIdxToken supressed.\r\n");)
    }
  } else {
    //Something is making a set token call on a manufacturing token,
    //on efm32 we permit this if the token is otherwise unprogrammed
    halInternalSetMfgTokenData(token,data,len);
  }
}


void halInternalIncrementCounterToken(uint8_t token)
{
  if(tokensActive) {
    halInternalSimEeIncrementCounter(token);
  } else {
    TOKENDBG(emberSerialPrintf(SER232, "IncrementCounter supressed.\r\n");)
  }
}


#ifndef EMBER_TEST

// The following interfaces are admittedly code space hogs but serve
// as glue interfaces to link creator codes to tokens for test code.

uint16_t getTokenAddress(uint16_t creator)
{
  #define DEFINETOKENS
  switch (creator) {
    #define TOKEN_MFG TOKEN_DEF
    #define TOKEN_DEF(name,creator,iscnt,isidx,type,arraysize,...) \
      case creator: return TOKEN_##name;
    #include "hal/micro/cortexm3/efm32/token-manufacturing.h"
    #include "stack/config/token-stack.h"
    #undef TOKEN_MFG
    #undef TOKEN_DEF
  default: {
  }
  };
  #undef DEFINETOKENS
  return INVALID_EE_ADDRESS;
}

uint8_t getTokenSize(uint16_t creator)
{
  #define DEFINETOKENS
  switch (creator) {
    #define TOKEN_MFG TOKEN_DEF
    #define TOKEN_DEF(name,creator,iscnt,isidx,type,arraysize,...) \
      case creator: return sizeof(type);
    #include "hal/micro/cortexm3/efm32/token-manufacturing.h"
    #include "stack/config/token-stack.h"
    #undef TOKEN_MFG
    #undef TOKEN_DEF
  default: {
  }
  };
  #undef DEFINETOKENS
  return 0;
}

uint8_t getTokenArraySize(uint16_t creator)
{
  #define DEFINETOKENS
  switch (creator) {
    #define TOKEN_MFG TOKEN_DEF
    #define TOKEN_DEF(name,creator,iscnt,isidx,type,arraysize,...) \
      case creator: return arraysize;
    #include "hal/micro/cortexm3/efm32/token-manufacturing.h"
    #include "stack/config/token-stack.h"
    #undef TOKEN_MFG
    #undef TOKEN_DEF
  default: {
  }
  };
  #undef DEFINETOKENS
  return 0;
}

#endif //EMBER_TEST

