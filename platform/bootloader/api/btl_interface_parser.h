/***************************************************************************//**
 * @file btl_interface_parser.h
 * @brief Application interface to the bootloader parser.
 * @author Silicon Labs
 * @version 1.0.0
 *******************************************************************************
 * # License
 * <b>Copyright 2016 Silicon Laboratories, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#ifndef BTL_INTERFACE_PARSER_H
#define BTL_INTERFACE_PARSER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/***************************************************************************//**
 * @addtogroup Interface
 * @{
 * @addtogroup ParserInterface Application Parser Interface
 * @brief Application interface for interfacing with the bootloader image parser.
 * @details The Parser Interface can be used to parse upgrade images from the
 *          context of the application.
 * @{
 ******************************************************************************/

// -----------------------------------------------------------------------------
// Typedefs

/***************************************************************************//**
 * Bootloader parser callback
 *
 * @param address Address of the data
 * @param data    Raw data
 * @param length  Size in bytes of raw data.
 * @param ctx     A context variable defined by the implementation that
 *                is implementing this callback.
 ******************************************************************************/
typedef void (*BootloaderParserCallback_t)(uint32_t address,
                                           uint8_t  *data,
                                           size_t   length,
                                           void     *context);

/// Context for the bootloader image parser routine.
typedef struct BootloaderParserContext BootloaderParserContext_t;

/// Function pointers to parser callbacks
typedef struct {
  /// Opaque pointer passed to the callback functions
  void                        *context;
  /// Callback function pointer for application image data
  BootloaderParserCallback_t  applicationCallback;
  /// Callback function pointer for image metadata
  BootloaderParserCallback_t  metadataCallback;
  /// Callback function pointer for bootloader upgrade image data
  BootloaderParserCallback_t  bootloaderCallback;
} BootloaderParserCallbacks_t;

/***************************************************************************//**
 * Initialize image parser
 *
 * @param[in] context     Pointer to parser context struct
 * @param[in] contextSize Size of context struct
 *
 * @return BOOTLOADER_OK if success, BOOTLOADER_ERROR_PARSE_CONTEXT if context
 *         struct is too small.
 ******************************************************************************/
int32_t bootloader_initParser(BootloaderParserContext_t *context,
                              size_t contextSize);

/***************************************************************************//**
 * Parse a buffer
 * @param[in] context   Pointer to parser context struct
 * @param[in] callbacks Callbacks to be called by the parser
 * @param[in] data      Data to be parsed
 * @param[in] numBytes  Size of data buffer
 *
 * @return BOOTLOADER_ERROR_PARSE_CONTINUE if the chunk was parsed correcly, and
 *         a new chunk is expected. BOOTLOADER_ERROR_PARSE_ERROR if something
 *         went wrong during parsing. BOOTLOADER_ERROR_PARSE_SUCCESS if the
 *         entire file was successfully parsed.
 ******************************************************************************/
int32_t bootloader_parseBuffer(BootloaderParserContext_t *context,
                               BootloaderParserCallbacks_t *callbacks,
                               uint8_t data[],
                               size_t numBytes);


/** @} // addtogroup ParserInterface */
/** @} // addtogroup Interface */

#endif // BTL_INTERFACE_PARSER_H
