/** @file hal/micro/cortexm3/token.h
 * @brief Cortex-M3 Token system for storing non-volatile information.
 * See @ref token for documentation.
 *
 * DOXYGEN NOTE:
 *  This file contains definitions, functions, and information that are
 *  internal only and should not be accessed by appilications.  This
 *  information is still documented, but should not be published in
 *  the generated doxygen.
 *
 * <!-- Copyright 2007-2011 by Ember Corporation. All rights reserved.   *80*-->
 */


#ifndef __PLAT_TOKEN_H__
#define __PLAT_TOKEN_H__

#ifndef __TOKEN_H__
#error do not include this file directly - include micro/token.h
#endif


#if defined CORTEXM3_EFM32_MICRO
  // The manufacturing tokens live outside the Simulated EEPROM.  This means
  // they are defined differently which is covered in mfg-token.h
  #include "efm32/mfg-token.h"
#else
  // The manufacturing tokens live in the Info Blocks, while all other tokens
  // live in the Simulated EEPROM.  This means they are defined differently,
  // which is covered in mfg-token.h
  #include "mfg-token.h"
#endif

//-- Build structure defines
/**
 * @description Simple declarations of all of the token types so that they can
 * be referenced from anywhere in the code base.
 */
#define DEFINETYPES
  #include "stack/config/token-stack.h"
#undef DEFINETYPES



//-- Build parameter links
#define DEFINETOKENS

#undef TOKEN_DEF

/**
 * @description Enum for translating token defs into a number.  This number is
 * used as an index into the cache of token information the token system and
 * Simulated EEPROM hold.
 *
 * The special entry TOKEN_COUNT is always at the top of the enum, allowing
 * the token and sim-eeprom system to know how many tokens there are.
 *
 * @param name: The name of the token.
 */
#define TOKEN_DEF(name,creator,iscnt,isidx,type,arraysize,...) \
  TOKEN_##name,
  enum{
    #include "stack/config/token-stack.h"
    TOKEN_COUNT
  };
#undef TOKEN_DEF


/**
 * @description Macro for translating token definitions into size variables.
 * This provides a convenience for abstracting the 'sizeof(type)' anywhere.
 *
 * @param name: The name of the token.
 *
 * @param type: The token type.  The types are found in token-stack.h.
 */
#define TOKEN_DEF(name,creator,iscnt,isidx,type,arraysize,...) \
  TOKEN_##name##_SIZE = sizeof(type),
  enum {
    #include "stack/config/token-stack.h"
  };
#undef TOKEN_DEF


/**
 * @description External declaration of an array of creator codes.  Since
 * the token and sim-eeprom systems identify tokens through an enum (see
 * below for the enum) and these two systems need to link creator codes to
 * their tokens, this array instantiates that link.
 *
 * @param creator: The creator code type.  The codes are found in
 * token-stack.h.
 */
extern const uint16_t tokenCreators[];

/**
 * @description External declaration of an array of IsCnt flags.  Since
 * the token and sim-eeprom systems identify tokens through an enum (see
 * below for the enum) and these two systems need to know which tokens
 * are counter tokens, this array provides that information.
 *
 * @param iscnt: The flag indicating if the token is a counter.  The iscnt's
 * are found in token-stack.h.
 */
extern const bool tokenIsCnt[];

/**
 * @description External declaration of an array of sizes.  Since
 * the token and sim-eeprom systems identify tokens through an enum (see
 * below for the enum) and these two systems need to know the size of each
 * token, this array provides that information.
 *
 * @param type: The token type.  The types are found in token-stack.h.
 */
extern const uint8_t tokenSize[];

/**
 * @description External declaration of an array of array sizes.  Since
 * the token and sim-eeprom systems identify tokens through an enum (see
 * below for the enum) and these two systems need to know the array size of
 * each token, this array provides that information.
 *
 * @param arraysize: The array size.
 */
extern const uint8_t tokenArraySize[];

/**
 * @description External declaration of an array of all token default values.
 * This array is filled with pointers to the set of constant declarations of
 * all of the token default values.  Therefore, the index into this array
 * chooses which token's defaults to access, and the address offset chooses the
 * byte in the defaults to use.
 *
 * For example, to get the n-th byte of the i-th token, use: 
 * uint8_t byte = *(((uint8_t *)tokenDefaults[i])+(n)
 *
 * @param TOKEN_##name##_DEFAULTS: A constant declaration of the token default
 * values, generated for all tokens.
 */
extern const void * const tokenDefaults[];

/**
 * @description A define for the token and Simulated EEPROM system that
 * specifies, in bytes, the space allocated to a counter token for
 * +1 marks.  The number of +1 marks varies between chips based on the
 * minimum write granularity for a chip's flash.  EM35x chips can use 8bit
 * per +1 while EFM32/EZM32/EZR32 chips use 16bit per +1.
 */
#define COUNTER_TOKEN_PAD        50



/**
 * @description Macro for typedef'ing the CamelCase token type found in
 * token-stack.h to a capitalized TOKEN style name that ends in _TYPE.
 * This macro allows other macros below to use 'token##_TYPE' to declare
 * a local copy of that token.
 *
 * @param name: The name of the token.
 *
 * @param type: The token type.  The types are found in token-stack.h.
 */
#define TOKEN_DEF(name,creator,iscnt,isidx,type,arraysize,...) \
  typedef type TOKEN_##name##_TYPE;
  #include "stack/config/token-stack.h"
#undef TOKEN_DEF

#undef DEFINETOKENS


/**
 * @description Copies the token value from non-volatile storage into a RAM
 * location.  This is the internal function that the two exposed APIs
 * (halCommonGetToken and halCommonGetIndexedToken) expand out to.  The
 * API simplifies the access into this function by hiding the size parameter
 * and hiding the value 0 used for the index parameter in scalar tokens.
 *
 * @note Only the public function should be called since the public
 * function provides the correct parameters.
 * 
 * @param data: A pointer to where the data being read should be placed.
 *
 * @param token: The name of the token to get data from.  On this platform
 * that name is defined as an address.
 *
 * @param index: The index to access.  If the token being accessed is not an
 * indexed token, this parameter is set by the API to be 0.
 *
 * @param len: The length of the token being worked on.  This value is
 * automatically set by the API to be the size of the token.
 */
void halInternalGetTokenData(void *data, uint16_t token, uint8_t index, uint8_t len);

/**
 * @description Sets the value of a token in non-volatile storage.  This is
 * the internal function that the two exposed APIs (halCommonSetToken and
 * halCommonSetIndexedToken) expand out to.  The API simplifies the access
 * into this function by hiding the size parameter and hiding the value 0
 * used for the index parameter in scalar tokens.
 *
 * @note Only the public function should be called since the public
 * function provides the correct parameters.
 * 
 * @param token: The name of the token to get data from.  On this platform
 * that name is defined as an address.
 *
 * @param index: The index to access.  If the token being accessed is not an
 * indexed token, this parameter is set by the API to be 0.
 *
 * @param data: A pointer to the data being written.
 *
 * @param len: The length of the token being worked on.  This value is
 * automatically set by the API to be the size of the token.
 */
void halInternalSetTokenData(uint16_t token, uint8_t index, void *data, uint8_t len);

 /**
 * @description Increments the value of a token that is a counter.  This is
 * the internal function that the exposed API (halCommonIncrementCounterToken)
 * expand out to.  This internal function is used as a level of simple
 * redirection providing clean separation from the lower token handler code.
 *
 * @note Only the public function should be called since the public
 * function provides the correct parameters.
 * 
 * @param token: The name of the token.
 */
void halInternalIncrementCounterToken(uint8_t token);


// See hal/micro/token.h for the full explanation of the token API as
// instantiated below.

//These defines Link the public API to the private internal instance.

#define halCommonGetToken( data, token )                    \
  halInternalGetTokenData(data, token, 0x7F, token##_SIZE)

#define halCommonGetIndexedToken( data, token, index )      \
  halInternalGetTokenData(data, token, index, token##_SIZE)
    
#define halStackGetIndexedToken( data, token, index, size ) \
  halInternalGetTokenData(data, token, index, size)

#define halStackGetIdxTokenPtrOrData( ptr, token, index ) \
  halInternalGetIdxTokenPtr(ptr, token, index, token##_SIZE)
void halInternalGetIdxTokenPtr(void *ptr, uint16_t ID, uint8_t index, uint8_t len);

#define halCommonSetToken( token, data )                    \
  halInternalSetTokenData(token, 0x7F, data, token##_SIZE)

#define halCommonSetIndexedToken( token, index, data )      \
  halInternalSetTokenData(token, index, data, token##_SIZE)

#define halStackSetIndexedToken( token, index, data, size ) \
  halInternalSetTokenData(token, index, data, size)

#define halCommonIncrementCounterToken( token )             \
  halInternalIncrementCounterToken(token);

// For use only by the EZSP UART protocol
#ifdef EZSP_UART
  #ifdef CORTEXM3_EMBER_MICRO
    #define halInternalMfgTokenPointer( address )  \
      ((const void *)(address + DATA_BIG_INFO_BASE))
    #define halInternalMfgIndexedToken( type, address, index )  \
      (*((const type *)(address + DATA_BIG_INFO_BASE) + index))
  #endif
  #ifdef CORTEXM3_EFM32_MICRO
    #define halInternalMfgTokenPointer( address )  \
      ((const void *)(USERDATA_BASE | (address&0x0FFF)))
    #define halInternalMfgIndexedToken( type, address, index )  \
      (*((const type *)(USERDATA_BASE | (address&0x0FFF)) + index))
  #endif
#endif


#undef TOKEN_MFG

#endif // __PLAT_TOKEN_H__

/**@} // END token group */

