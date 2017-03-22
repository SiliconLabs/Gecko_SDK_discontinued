/***************************************************************************//**
 * @file btl_ebl_parser.h
 * @brief EBL image file parser.
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
#ifndef BTL_EBL_PARSER_H
#define BTL_EBL_PARSER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "plugin/parser/btl_image_parser.h"

#include "plugin/security/btl_security_aes.h"
#include "plugin/security/btl_security_sha256.h"

/***************************************************************************//**
 * @addtogroup Plugin
 * @{
 * @addtogroup ImageParser Image Parser
 * @{
 * @addtogroup EblParser EBL Parser
 * @{
 * @brief EBL parser implementation.
 * @details
 *   Image parser for EBL files. Parses EBL files based on the
 *   [EBL file format specification](@ref EblParserFormat). Callbacks are used
 *   to present data and metadata contents of the EBL file to the bootloader.
 ******************************************************************************/

/// State in the EBL parser state machine
typedef enum {
  EblParserStateInit,                 ///< Initial state
  EblParserStateIdle,                 ///< Idle state
  EblParserStateHeader,               ///< Parsing header tag
  EblParserStateHeaderV2,             ///< Parsing V2 header tag
  EblParserStateBootloader,           ///< Parsing bootloader tag
  EblParserStateBootloaderData,       ///< Parsing bootloader tag data
  EblParserStateApplication,          ///< Parsing application tag
  EblParserStateMetadata,             ///< Parsing metadata tag
  EblParserStateProg,                 ///< Parsing flash program tag
  EblParserStateProgData,             ///< Parsing flash (erase &) program tag data
  EblParserStateEraseProg,            ///< Parsing flash erase&program tag
  EblParserStateFinalize,             ///< Finalizing file
  EblParserStateDone,                 ///< Parsing complete
  EblParserStateEncryptionInit,       ///< Parsing encryption init tag
  EblParserStateEncryptionContainer,  ///< Parsing encryption data tag
  EblParserStateSignature,            ///< Parsing signature tag
  EblParserStateError                 ///< Error state
} EblParserState_t;

/// EBL parser buffer
typedef struct {
  /// Buffer contents
  uint8_t           buffer[64];
  /// Amount of bytes present in buffer
  size_t            length;
  /// Current reading offset into the buffer (circular)
  size_t            offset;
} EblBuffer_t;

/// Image parser context definition
typedef struct {
  /// State of the EBL parser state machine
  EblParserState_t    internalState;
  /// Buffer to handle unaligned incoming data
  EblBuffer_t         localBuffer;
  /// AES-CCM decryption (= AES-CTR) context
  void                *aesContext;
  /// SHA256 hashing context
  void                *shaContext;
  /// Total length of the tag currently being parsed
  size_t              lengthOfTag;
  /// Current offset into tag being parsed
  size_t              offsetInTag;
  /// Total length of current encrypted data block
  size_t              lengthOfEncryptedTag;
  /// Offset into current encrypted data block
  size_t              offsetInEncryptedTag;
  /// Current address the image needs to be written to
  uint32_t            programmingAddress;
  /// Current offset of metadata being handled (starts at 0)
  uint32_t            metadataAddress;
  /// Current offset of bootloader being handled (starts at 0)
  uint32_t            bootloaderAddress;
  /// Withheld application data
  uint8_t             withheldApplicationVectors[24];
  /// Withheld bootloader upgrade data during app parsing
  uint8_t             withheldUpgradeVectors[4];
  /// Withheld bootloader upgrade data during bootloader parsing
  uint8_t             withheldBootloaderVectors[4];
  /// Running CRC-32 over the incoming EBL file
  uint32_t            fileCrc;
  /// Indicates we're handling an encrypted EBL file
  bool                encrypted;
  /// Indicates the parser is currently inside an encrypted block
  bool                inEncryptedContainer;
  /// Indicates the parser has parsed an ECDSA signature
  bool                gotSignature;
  /// Indicates we're parsing a V2 file
  bool                isV2;
} ParserContext_t;

/** @} addtogroup EblParser */
/** @} addtogroup ImageParser */
/** @} addtogroup Plugin */

#endif // BTL_EBL_PARSER_H
