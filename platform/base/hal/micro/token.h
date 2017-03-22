/** @file hal/micro/token.h
 * @brief Token system for storing non-volatile information.
 * See @ref token for documentation.
 *
 * <!-- Copyright 2008-2011 by Ember Corporation. All rights reserved.   *80*-->
 */

/** @addtogroup tokens
 * The token system stores such non-volatile information as the manufacturing
 * ID, channel number, transmit power, and various pieces of information
 * that the application needs to be persistent between device power cycles.
 * The token system is design to abstract implementation details and simplify
 * interacting with differing non-volatile systems.  The majority of tokens
 * are stored in Simulated EEPROM (in Flash) where they can be rewritten.
 * Manufacturing tokens are stored in dedicated regions of flash and are
 * not designed to be rewritten.
 *
 * Refer to the @ref token module for a detailed description of the token
 * system.\n
 * Refer to the @ref simeeprom module for a detailed description of the
 * necessary support functions for Simulated EEPROM.\n
 * Refer to the @ref simeeprom2 module for a detailed description of the
 * necessary support functions for Simulated EEPROM, version 2.\n
 * Refer to token-stack.h for stack token definitions.\n
 * Refer to token-manufacturing.h for manufaturing token definitions.\n
 * @note Simulated EEPROM, version 2 is only supported on EM335x chips.\n
 */
 
/** @addtogroup token 
 * There are three main types of tokens: 
 * - <b>Manufacturing tokens:</b> Tokens that are set at the factory and 
 *   must not be changed through software operations.
 * - <b>Stack-level tokens:</b> Tokens that can be changed via the 
 *   appropriate stack API calls.  
 * - <b>Application level tokens:</b> Tokens that can be set via the  
 *   token system API calls in this file.
 *
 * The token system API controls writing tokens to non-volatile data and reading
 * tokens from non-volatile data. If an application wishes to use application
 * specific normal tokens, it must do so by creating its own token header file
 * similar to token-stack.h. The macro 
 * <code>APPLICATION_TOKEN_HEADER</code> should be defined to
 * equal the name of the header file in which application tokens are defined.
 * If an application wishes to use application specific manufacturing tokens,
 * it must do so by creating its own manufacturing token header file similar to
 * token-manufacturing.h.  The macro <code>APPLICATION_MFG_TOKEN_HEADER</code>
 * should be defined to equal the name of the header file in which
 * manufacturing tokens are defined.
 * 
 * Because the token system is based on memory locations within non-volatile
 * storage, the token information could become out of sync without some kind of
 * version tracking.  The two defines, <code>CURRENT_MFG_TOKEN_VERSION
 * </code> and <code>CURRENT_STACK_TOKEN_VERSION</code>, are used
 * to make sure the stack stays in sync with the proper token set.  If the
 * application defines its own tokens, it is recommended that the application
 * also define an application token to be a application version to ensure the
 * application stays in sync with the proper token set.
 *
 * The most general format of a token definition is:
 *
 * @code
 * #define CREATOR_name 16bit_value
 * #ifdef DEFINETYPES
 *    typedef data_type type
 * #endif //DEFINETYPES
 * #ifdef DEFINETOKENS
 *    DEFINE_*_TOKEN(name, type, ... ,defaults)
 * #endif //DEFINETOKENS
 * @endcode
 *
 * The defined CREATOR is used as a distinct identifier tag for the token.
 * The CREATOR is necessary because the token name is defined differently
 * depending on underlying implementation, so the CREATOR makes sure token
 * definitions and data stay tagged and known.  The only requirement
 * on these creator definitions is that they all must be unique.  A favorite
 * method for picking creator codes is to use two ASCII characters inorder
 * to make the codes more memorable.  The 'name' part of the
 * <code>\#define CREATOR_name</code> must match the 'name' provided in the
 * <code>DEFINE_*_TOKEN</code> because the token system uses this name 
 * to automatically link the two.
 *
 * The typedef provides a convenient and efficient abstraction of the token
 * data.  Since some tokens are structs with multiple pieces of data inside
 * of them, type defining the token type allows more efficient and readable
 * local copies of the tokens throughout the code.
 *
 * The typedef is wrapped with an <code>\#ifdef DEFINETYPES</code> because
 * the typdefs and token defs live in the same file, and DEFINETYPES is used to
 * select only the typedefs when the file is included.  Similarly, the 
 * <code>DEFINE_*_TOKEN</code> is wrapped with an 
 * <code>\#ifdef DEFINETOKENS</code> as a method for selecting only the
 * token definitions when the file is included.
 * 
 * 
 * The abstract definition, 
 * <code>DEFINE_*_TOKEN(name, type, ... ,defaults)</code>, has
 * seven possible complete definitions:<br>
 * <code>
 * <ul>
 *  <li>DEFINE_BASIC_TOKEN(name, type, ...)
 *  <li>DEFINE_INDEXED_TOKEN(name, type, arraysize, ...)
 *  <li>DEFINE_COUNTER_TOKEN(name, type, ...)
 *  <li>DEFINE_MFG_TOKEN(name, type, address, ...)
 * </ul>
 * </code>
 * The three fields common to all <code>DEFINE_*_TOKEN</code> are:\n
 * name - The name of the token, which all information is tied to.\n
 * type - Type of the token which is the same as the typedef mentioned before.\n
 * ... - The default value to which the token is set upon initialization.
 * 
 * @note The old DEFINE_FIXED* token definitions are no longer used.  They
 * remain defined for backwards compatibility.  In current systems, the
 * Simulated EEPROM is used for storing non-manufacturing tokens and the
 * Simulated EEPROM intelligently manages where tokens are stored to provide
 * wear leveling across the flash memory and increase the number of write
 * cycles.  Manufacturing tokens live at a fixed address, but they must use
 * DEFINE_MFG_TOKEN so the token system knows they are manufacturing
 * tokens.
 *
 * \b DEFINE_BASIC_TOKEN is the simplest definition and will be used for
 * the majority of tokens (tokens that are not indexed, not counters, and not
 * manufacturing).  Basic tokens are designed for data storage that is
 * always accessed as a single element.
 *
 * \b DEFINE_INDEXED_TOKEN should be used on tokens that look like arrays.
 * For example, data storage that looks like:<br>
 * <pre><code>   uint32_t myData[5]</code></pre><br>
 * This example data storage can be a token with typedef of uint32_t and defined
 * as INDEXED with arraysize of 5.  The extra field in this token definition is:
 * arraysize - The number of elements in the indexed token.  Indexed tokens
 * are designed for data storage that is logically grouped together, but
 * elements are accessed individually.
 * 
 * \b DEFINE_COUNTER_TOKEN should be used on tokens that are simple numbers
 * where the majority of operations on the token is to increment the count.
 * The reason for using DEFINE_COUNTER_TOKEN instead of DEFINE_BASIC_TOKEN is
 * the special support that the token system provides for incrementing counters.
 * The function call <code>halCommonIncrementCounterToken()</code> only
 * operates on counter tokens and is more efficient in terms of speed, data
 * compression, and write cyles for incrementing simple numbers in the
 * token system.
 * 
 * \b DEFINE_MFG_TOKEN is a DEFINE_BASIC_TOKEN token at a specific address and
 * the token is manufacturing data that is written only once.  The major
 * difference is this token is designated manufacturing, which means the 
 * token system treats it differently from stack or app tokens.  Primarily,
 * a manufacturing token is written only once and lives at a fixed address 
 * outside of the Simulated EEPROM system.  Being a write once token, the
 * token system will also aid in debugging by asserting if there is an
 * attempt to write a manufacturing token.
 *
 * Here is an example of two application tokens:
 *
 * @code
 * #define CREATOR_SENSOR_NAME        0x5354
 * #define CREATOR_SENSOR_PARAMETERS  0x5350
 * #ifdef DEFINETYPES
 *   typedef uint8_t tokTypeSensorName[10];
 *   typedef struct {
 *     uint8_t initValues[5];
 *     uint8_t reportInterval;
 *     uint16_t calibrationValue;
 *   } tokTypeSensorParameters;
 * #endif //DEFINETYPES
 * #ifdef DEFINETOKENS
 *   DEFINE_BASIC_TOKEN(SENSOR_NAME,
 *                      tokTypeSensorName,
 *                      {'U','N','A','M','E','D',' ',' ',' ',' '})
 *   DEFINE_BASIC_TOKEN(SENSOR_PARAMETERS,
 *                      tokTypeSensorParameters,
 *                      {{0x01,0x02,0x03,0x04,0x05},5,0x0000})
 * #endif //DEFINETOKENS
 * @endcode
 *
 * Here is an example of how to use the two application tokens:
 * @code
 * {
 *   tokTypeSensorName sensor;
 *   tokTypeSensorParameters params;
 *
 *   halCommonGetToken(&sensor, TOKEN_SENSOR_NAME);
 *   halCommonGetToken(&params, TOKEN_SENSOR_PARAMETERS);
 *   if(params.calibrationValue == 0xBEEF) {
 *     params.reportInterval = 5;
 *   }
 *   halCommonSetToken(TOKEN_SENSOR_PARAMETERS, &params);
 * }
 * @endcode
 *
 * See token-stack.h to see the default set of tokens and their values.
 *
 *
 * The nodetest utility app can be used for generic manipulation such
 * as loading default token values, viewing tokens, and writing tokens.
 * <b>The nodetest utility cannot work with customer defined application
 * tokens or manufacturing tokens.  Using the nodetest utility will
 * erase customer defined application tokens in the Simulated EEPROM.</b>
 *
 * The Simulated EEPROM will initialize
 * tokens to their default values if the token does not yet exist, the token's
 * creator code is changed, or the token's size changes.
 *
 * Changing the number indexes in an INDEXED token will not alter
 * existing entries.  If the number of indexes is reduced, the entires that
 * still fit in the token will retain their data and the entries that no longer
 * fit will be erased.  If the number of indexes is increased, the existing
 * entries retain their data and the new entries are initialized to the token's
 * defaults.
 *
 * Further details on exact implementation can be found in code
 * comments in token-stack.h file, the platform specific
 * token-manufacturing.h file, the platform specific token.h file, and the
 * platform specific token.c file.
 *
 * Some functions in this file return an ::EmberStatus value. See 
 * error-def.h for definitions of all ::EmberStatus return values.
 *
 * See hal/micro/token.h for source code. 
 *@{ 
 */

#ifndef __TOKEN_H__
#define __TOKEN_H__

#if defined(CORTEXM3)
  #ifdef MINIMAL_HAL
    #include "cortexm3/nvm-token.h"
    #include "cortexm3/mfg-token.h"
  #else //MINIMAL_HAL
    #include "cortexm3/token.h"
  #endif //MINIMAL_HAL
#elif defined(C8051)
  #if defined(C8051_COBRA)
    #include "c8051/cobra/token.h"
  #else
    #include "c8051/silabs/token.h"
  #endif
#elif defined(EMBER_TEST)
  #include "generic/token-ram.h"
#elif (defined(EZSP_HOST) || defined(UNIX_HOST)) && defined(EMBER_AF_API_TOKEN)
  #include "unix/host/token.h"
#else
  #error invalid platform
#endif


/**
 * @brief Initializes and enables the token system. Checks if the
 * manufacturing and stack non-volatile data versions are correct.
 *
 * @return An EmberStatus value indicating the success or
 *  failure of the command.
 */
EmberStatus halStackInitTokens(void);

// NOTE:
// The following API as written below is purely for doxygen
// documentation purposes.  The live API used in code is actually macros
// defined in the platform specific token headers and provide abstraction
// that can allow easy and efficient access to tokens in different
// implementations.

#ifdef DOXYGEN_SHOULD_SKIP_THIS

/**
 * @brief Macro that copies the token value from non-volatile storage into a RAM
 * location.  This macro can only be used with tokens that are defined using
 * DEFINE_BASIC_TOKEN.
 *
 * @note  To better understand the parameters of this macro, refer to the
 *           example of token usage above.
 *
 * @param data  A pointer to where the token data should be placed.
 *
 * @param token  The token name used in <code>DEFINE_*_TOKEN</code>,
 *  prepended with <code>TOKEN_</code>.
 */
#define halCommonGetToken( data, token )

/**
 * @brief Macro that copies the token value from non-volatile storage into a RAM
 * location.  This macro can only be used with tokens that are defined using
 * DEFINE_MFG_TOKEN.
 *
 * @note  To better understand the parameters of this macro, refer to the
 *           example of token usage above.
 *
 * @param data  A pointer to where the token data should be placed.
 *
 * @param token  The token name used in <code>DEFINE_*_TOKEN</code>,
 *  prepended with <code>TOKEN_</code>.
 */
#define halCommonGetMfgToken( data, token )

/**
 * @brief Macro that copies the token value from non-volatile storage into a RAM
 * location.  This macro can only be used with tokens that are defined using
 * DEFINE_INDEXED_TOKEN.
 *
 * @note To better understand the parameters of this macro, refer to the
 *           example of token usage above.
 *
 * @param data   A pointer to where the token data should be placed.
 *
 * @param token  The token name used in <code>DEFINE_*_TOKEN</code>,
 *               prepended with <code>TOKEN_</code>.
 * @param index  The index to access in the indexed token.
 */
#define halCommonGetIndexedToken( data, token, index )

/**
 * @brief Macro that sets the value of a token in non-volatile storage.  This
 * macro can only be used with tokens that are defined using DEFINE_BASIC_TOKEN.
 *
 * @note  To better understand the parameters of this macro, refer to the
 *           example of token usage above.
 *
 * @param token The token name used in <code>DEFINE_*_TOKEN</code>,
 * prepended with <code>TOKEN_</code>.
 * 
 * @param data  A pointer to the data being written.
 */
#define halCommonSetToken( token, data )

/**
 * @brief Macro that sets the value of a token in non-volatile storage.  This
 * macro can only be used with tokens that are defined using
 * DEFINE_INDEXED_TOKEN.
 *
 * @note  To better understand the parameters of this macro, refer to the
 *           example of token usage above.
 *
 * @param token  The token name used in <code>DEFINE_*_TOKEN</code>,
 * prepended with <code>TOKEN_</code>.
 *
 * @param index  The index to access in the indexed token.
 *
 * @param data   A pointer to where the token data should be placed.
 */
#define halCommonSetIndexedToken( token, index, data )

 /**
 * @brief Macro that increments the value of a token that is a counter.  This
 * macro can only be used with tokens that are defined using either
 * DEFINE_COUNTER_TOKEN.
 *
 * @note  To better understand the parameters of this macro, refer to the
 *           example of token usage above.
 *
 * @param token  The token name used in <code>DEFINE_*_TOKEN</code>,
 * prepended with <code>TOKEN_</code>.
 */
#define halCommonIncrementCounterToken( token )

#endif //DOXYGEN_SHOULD_SKIP_THIS



#ifndef DOXYGEN_SHOULD_SKIP_THIS
  // These interfaces serve only as a glue layer
  // to link creator codes to tokens (primarily for *test code)
  #define INVALID_EE_ADDRESS 0xFFFF
  uint16_t getTokenAddress(uint16_t creator);
  uint8_t getTokenSize(uint16_t creator );
  uint8_t getTokenArraySize(uint16_t creator);
#endif //DOXYGEN_SHOULD_SKIP_THIS


#endif // __TOKEN_H__

/**@} // END token group
 */
  

