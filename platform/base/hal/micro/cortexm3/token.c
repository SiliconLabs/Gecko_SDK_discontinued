/** @file hal/micro/cortexm3/token.c
 * @brief Token implementation for the Cortex-M3/EM3XX chip.
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
//[[ Strip emulator only code from official build
#ifdef EMBER_EMU_TEST
  //On emulators, the CIB and FIB come up as 0x00 which can confuse code
  //that expects 0xFF for a token's erased state.  If we're on emulators
  //and the CIB/FIB is 0x00, fake 0xFF.
  bool emuFib0x00 = true;
  bool emuCib0x00 = true;
#endif //EMBER_EMU_TEST
//]]


EmberStatus halStackInitTokens(void)
{
  #if !defined(BOOTLOADER) && !defined(EMBER_TEST)
    tokTypeMfgFibVersion tokMfg;
    EmberStatus status=EMBER_ERR_FATAL;
    tokTypeStackNvdataVersion tokStack;
    bool mfgTokenVersionValid = false;
    tokTypeMfgFibVersion validMfgTokens[] = VALID_MFG_TOKEN_VERSIONS;
  #endif
  tokensActive = true;

  //[[ Strip emulator only code from official build
  #ifdef EMBER_EMU_TEST
    //If we're on an emu, scan the FIB and CIB MFG tokens to see if
    //we're in the emu's default state of 0x00.  This means MFG tokens have
    //not been loaded and we'll have to fake an erased, 0xFF, state.
    {
      uint32_t i;
      uint8_t * ptr;
      ptr = (uint8_t *)(DATA_BIG_INFO_BASE+0x077E);
      for(i=0;i<(0x07F8-0x077E);i++) {
        if(ptr[i] != 0x00) {
          emuFib0x00 = false;
          break;
        }
      }
      ptr = (uint8_t *)(DATA_BIG_INFO_BASE+0x0810);
      for(i=0;i<(0x08F0-0x0810);i++) {
        if(ptr[i] != 0x00) {
          emuCib0x00 = false;
          break;
        }
      }
    }
  #endif //EMBER_EMU_TEST
  //]]

  if(halInternalSimEeInit()!=EMBER_SUCCESS) {
    TOKENDBG(emberSerialPrintf(SER232,"halInternalSimEeInit Attempt 1 fail\r\n");)
    if(halInternalSimEeInit()!=EMBER_SUCCESS) {
      TOKENDBG(emberSerialPrintf(SER232,"halInternalSimEeInit Attempt 2 fail\r\n");)
      tokensActive = false;
      return EMBER_SIM_EEPROM_INIT_2_FAILED;
    }
    TOKENDBG(} else {emberSerialPrintf(SER232,"halInternalSimEeInit Successful\r\n");)
  }

  // TODO: remove emulator reference.
  #if !defined(BOOTLOADER) && !defined(EMBER_TEST) && !defined(EMBER_EMU_TEST)
    halCommonGetToken(&tokMfg, TOKEN_MFG_FIB_VERSION);
    halCommonGetToken(&tokStack, TOKEN_STACK_NVDATA_VERSION);
    
    // See if this manufacturing token version matches any known valid version
    {
      uint32_t i;
      for(i = 0; i < (sizeof(validMfgTokens)/sizeof(*validMfgTokens)); i++) {
        if(validMfgTokens[i] == tokMfg) {
          mfgTokenVersionValid = true;
        }
      }
    }

    if (!mfgTokenVersionValid || CURRENT_STACK_TOKEN_VERSION != tokStack) {
      if(!mfgTokenVersionValid && (CURRENT_STACK_TOKEN_VERSION != tokStack)) {
        status = EMBER_EEPROM_MFG_STACK_VERSION_MISMATCH;
      } else if(!mfgTokenVersionValid) {
        status = EMBER_EEPROM_MFG_VERSION_MISMATCH;
      } else if(CURRENT_STACK_TOKEN_VERSION != tokStack) {
        status = EMBER_EEPROM_STACK_VERSION_MISMATCH;
      }
      #if defined(DEBUG)
        if(!mfgTokenVersionValid) {
          //Even is we're a debug image, a bad manufacturing token version
          //is a fatal error, so return the error status.
          tokensActive = false;
          return status;
        }
        if (CURRENT_STACK_TOKEN_VERSION != tokStack) {
          //Debug images with a bad stack token version should attempt to
          //fix the SimEE before continuing on.
          TOKENDBG(emberSerialPrintf(SER232,"Stack Version mismatch, reloading\r\n");)
          halInternalSimEeRepair(true);
          if(halInternalSimEeInit()!=EMBER_SUCCESS) {
            TOKENDBG(emberSerialPrintf(SER232,"halInternalSimEeInit Attempt 3 fail\r\n");)
            tokensActive = false;
            return EMBER_SIM_EEPROM_INIT_3_FAILED;
          }
        }
      #else //  node release image
        TOKENDBG(emberSerialPrintf(SER232,"EEPROM_x_VERSION_MISMATCH (%d)\r\n",status);)
        tokensActive = false;
        #if defined(PLATFORMTEST) || defined(LEVEL_ONE_TEST)
          tokensActive = true;  //Keep tokens active for test code.
        #endif //defined(PLATFORMTEST)
        return status;
      #endif
    }
  #endif //!defined(BOOTLOADER) && !defined(EMBER_TEST) && !defined(EMBER_EMU_TEST)

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


void halInternalGetIdxTokenPtr(void *ptr, uint16_t ID, uint8_t index, uint8_t len)
{
  if(ID < 256) {
    //the ID is within the SimEEPROM's range, route to the SimEEPROM
    if(tokensActive) {
      halInternalSimEeGetPtr(ptr, ID, index, len);
    } else {
      TOKENDBG(emberSerialPrintf(SER232, "getIdxToken supressed.\r\n");)
    }
  } else {
    #ifdef EMBER_TEST
      assert(false);
    #else //EMBER_TEST
      uint32_t *ptrOut = (uint32_t *)ptr;
      uint32_t realAddress;

      //0x7F is a non-indexed token.  Remap to 0 for the address calculation
      index = (index==0x7F) ? 0 : index;

      realAddress = (DATA_BIG_INFO_BASE|ID) + (len*index);

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
    //The Information Blocks can only be written by an external tool!
    //Something is making a set token call on a manufacturing token, and that
    //is not allowed!
    assert(0);
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
    #include "hal/micro/cortexm3/token-manufacturing.h"
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
    #include "hal/micro/cortexm3/token-manufacturing.h"
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
    #include "hal/micro/cortexm3/token-manufacturing.h"
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

