/***************************************************************************//**
 * @file btl_image_parser.h
 * @brief Definition of the interface between the core bootloader and the
 *   firmware upgrade image file parser.
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
#ifndef BTL_IMAGE_PARSER_H
#define BTL_IMAGE_PARSER_H

#include "api/application_properties.h"
#include "api/btl_interface_parser.h"

// The EBL parser is the only parser, so include it unconditionally
#include "ebl/btl_ebl_parser.h"

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/***************************************************************************//**
 * @addtogroup Plugin
 * @{
 * @addtogroup ImageParser Image Parser
 * @{
 * @brief Image parser plugin
 * @details
 *   Generic image parser interface for Silicon Labs bootloader.
 ******************************************************************************/

/// Structure containing state of the image file processed
typedef struct {
  /// Flag to indicate image contains a Second Stage Bootloader
  bool                        imageContainsBootloader;
  /// Flag to indicate image contains an application
  bool                        imageContainsApplication;
  /// Flag to indicate parsing has completed
  bool                        imageCompleted;
  /// Flag to indicate the image file has been validated
  bool                        imageVerified;
  /// Version number of main bootloader extracted from image file
  uint32_t                    bootloaderVersion;
  /// Information about the application
  ApplicationData_t           application;
} ImageProperties_t;

/***************************************************************************//**
 * Initialize the parser's context
 *
 * @param context         Pointer to context for the parser implementation
 * @param decryptContext  Pointer to context for decryption of parsed file
 * @param authContext     Pointer to context for authentication of parsed file
 *
 * @return @ref BOOTLOADER_OK if OK, error code otherwise.
 ******************************************************************************/
int32_t parser_init(void *context,
                    void *decryptContext,
                    void *authContext);

/***************************************************************************//**
 * Parse an image file in order to extract the binary and some metadata.
 *
 * Pushes data into the image file parser to be parsed.
 *
 * @param context Pointer to the specific parser's context variable
 * @param imageProperties Pointer to the image file state variable
 * @param buffer Pointer to byte array containing data to parse
 * @param length Size in bytes of the data in buffer
 * @param callbacks Struct containing function pointers to be called by the
 *   parser to pass the extracted binary data back to BTL.
 *
 * @return @ref BOOTLOADER_OK if OK, error code otherwise.
 ******************************************************************************/
int32_t parser_parse(void                              *context,
                     ImageProperties_t                 *imageProperties,
                     uint8_t                           buffer[],
                     size_t                            length,
                     const BootloaderParserCallbacks_t *callbacks);

/***************************************************************************//**
 * Check whether the parser requires images to be signed
 *
 * @return True if authenticity is required, else false
 ******************************************************************************/
bool parser_requireAuthenticity(void);

/***************************************************************************//**
 * Check whether the parser requires images to be encrypted
 *
 * @return True if confidentiality is required, else false
 ******************************************************************************/
bool parser_requireConfidentiality(void);

/***************************************************************************//**
 * Check whether the parser is allowed to parse legacy images
 *
 * @warning Depending on the file format, this might have serious security
 *   implications!
 *
 * @return True if compatibility is enabled, false otherwise.
 ******************************************************************************/
bool parser_allowLegacyFormat(void);

/***************************************************************************//**
 * Return the start address of the application
 *
 * @return start address of the application
 ******************************************************************************/
uint32_t parser_getApplicationAddress(void);

/***************************************************************************//**
 * Return the start address of the bootloader upgrade location
 *
 * @return start address of the bootloader upgrade location if upgrading is
 *         supported, otherwise 0.
 ******************************************************************************/
uint32_t parser_getBootloaderUpgradeAddress(void);

/** @} addtogroup ImageParser */
/** @} addtogroup Plugin */

#endif // BTL_IMAGE_PARSER_H
