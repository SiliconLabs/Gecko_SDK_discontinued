/***************************************************************************//**
 * @file btl_parse.h
 * @brief Functions to use the parser plugin to parse an image
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
#ifndef BTL_PARSE_H
#define BTL_PARSE_H

#include "plugin/parser/btl_image_parser.h"
#include "plugin/security/btl_security_types.h"

/// Context used by bootloader image parser to store parser state
struct BootloaderParserContext {
  /// Properties of the image being parsed
  ImageProperties_t     imageProperties;
  /// Context information for the image parser
  ParserContext_t       parserContext;
  /// Decryption context information for the image parser
  DecryptContext_t      decryptContext;
  /// Authentication context information for the image parser
  AuthContext_t         authContext;
  /// Slot ID to try and parse an image from
  uint32_t              slotId;
  /// Size of storage slot
  uint32_t              slotSize;
  /// Offset into storage slot
  uint32_t              slotOffset;
  /// Error code returned by the image parser
  int32_t               errorCode;
};

/***************************************************************************//**
 * Initialize image parser
 *
 * @param[in] context     Pointer to parser context struct
 * @param[in] contextSize Size of context struct
 *
 * @return BOOTLOADER_OK if success, BOOTLOADER_ERROR_PARSE_CONTEXT if context
 *         struct is too small.
 ******************************************************************************/
int32_t core_initParser(BootloaderParserContext_t *context,
                        size_t contextSize);

/***************************************************************************//**
 * Parse a buffer
 * @param     context     Pointer to parser context struct
 * @param[in] callbacks   Pointer to struct of callbacks for different types of
 *                        data
 * @param[in] data        Data to be parsed
 * @param[in] numBytes    Size of data buffer
 *
 * @return BOOTLOADER_ERROR_PARSE_CONTINUE if the chunk was parsed correcly, and
 *         a new chunk is expected. BOOTLOADER_ERROR_PARSER_* if something
 *         went wrong during parsing. BOOTLOADER_ERROR_PARSE_SUCCESS if the
 *         entire file was successfully parsed.
 ******************************************************************************/
int32_t core_parseBuffer(BootloaderParserContext_t *context,
                         const BootloaderParserCallbacks_t *callbacks,
                         uint8_t data[],
                         size_t numBytes);

#endif // BTL_PARSE_H
