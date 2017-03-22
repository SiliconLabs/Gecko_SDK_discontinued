/** @file hal/micro/bootloader-interface.h
 * See @ref common_bootload for detailed documentation.
 * 
 * <!-- Copyright 2007-2009 by Ember Corporation. All rights reserved.  *80* -->
 */

/** @addtogroup common_bootload
 * @brief Common bootloader interface defines and functions.
 *
 * See bootloader-interface.h for source code.
 *@{
 */

#ifndef __BOOTLOADER_INTERFACE_H__
#define __BOOTLOADER_INTERFACE_H__

#include "bootloader-interface-app.h"
#include "bootloader-interface-standalone.h"

/** @name Bootloader Numerical Definitions
 * These are numerical definitions for the possible bootloader types and
 * a typedef of the bootloader base type.
 *@{
 */
/** @brief Numerical definition for a bootloader type.
 */
#define BL_TYPE_NULL        (0)
#define BL_TYPE_STANDALONE  (1)
#define BL_TYPE_APPLICATION (2)
#define BL_TYPE_BOOTLOADER  (3)        // Generic bootloader type
#define BL_TYPE_SMALL_BOOTLOADER  (4)  // Generic, but small bootloader type

/**@} */

/** @name Bootloader type definitions
 * These are the type definitions for the bootloader.
 *@{
 */
/** @brief Define the bootloader base type.
 */
typedef uint8_t BlBaseType;
/** @brief Define the bootloader extended type.
 */
typedef uint16_t BlExtendedType;
/**@} */

/** @brief Returns the bootloader base type the application was
 *  built for.
 *
 * @return ::BL_TYPE_NULL, ::BL_TYPE_STANDALONE, or ::BL_TYPE_APPLICATION
 */
BlBaseType halBootloaderGetType(void);


/** @brief Macro returning the base type of a bootloader when given an
 * extended type.
 */
#define BOOTLOADER_BASE_TYPE(extendedType)  \
                                 ((uint8_t)(((extendedType) >> 8) & 0xFF))

/** @brief Macro returning the extended type of a bootloader when given a
 * base type and extendedSpecifier.
 */
#define BOOTLOADER_MAKE_EXTENDED_TYPE(baseType, extendedSpecifier) \
        ((uint16_t)(((uint16_t)baseType) << 8) | (((uint16_t)extendedSpecifier)&0xFF))
        
/** @brief Macro defining the extended NULL bootloader type.
 */
#define BL_EXT_TYPE_NULL                ((BL_TYPE_NULL << 8) | 0x00)

/** @brief Macro defining the extended standalone unknown bootloader type.
 */
#define BL_EXT_TYPE_STANDALONE_UNKNOWN  ((BL_TYPE_STANDALONE << 8) | 0x00) 

/** @brief Macro defining the extended standalone UART bootloader type.
 */
#define BL_EXT_TYPE_SERIAL_UART         ((BL_TYPE_STANDALONE << 8) | 0x01) 

// skipping the extSpecifier of 0x02 in case we decide we want it to 
//   be a bitmask for "OTA only"

/** @brief Macro defining the extended standalone OTA and UART bootloader type.
 */
#define BL_EXT_TYPE_SERIAL_UART_OTA     ((BL_TYPE_STANDALONE << 8) | 0x03) 
#define BL_EXT_TYPE_EZSP_SPI            ((BL_TYPE_STANDALONE << 8) | 0x04)
#define BL_EXT_TYPE_EZSP_SPI_OTA        ((BL_TYPE_STANDALONE << 8) | 0x06)

/** @brief Macro defining the extended standalone USB bootloader type.
 */
#define BL_EXT_TYPE_SERIAL_USB         ((BL_TYPE_STANDALONE << 8) | 0x07) 

/** @brief Macro defining the extended standalone OTA and USB bootloader type.
 */
#define BL_EXT_TYPE_SERIAL_USB_OTA     ((BL_TYPE_STANDALONE << 8) | 0x08) 


/** @brief Macro defining the extended application unknown bootloader type.
 */
#define BL_EXT_TYPE_APP_UNKNOWN         ((BL_TYPE_APPLICATION << 8) | 0x00)

/** @brief Macro defining the extended application SPI bootloader type.
 */
#define BL_EXT_TYPE_APP_SPI             ((BL_TYPE_APPLICATION << 8) | 0x01) 

/** @brief Macro defining the extended application I2C bootloader type.
 */
#define BL_EXT_TYPE_APP_I2C             ((BL_TYPE_APPLICATION << 8) | 0x02)

/** @brief Macro defining a type for the local storage app bootloader.
 */
#define BL_EXT_TYPE_APP_LOCAL_STORAGE   ((BL_TYPE_APPLICATION << 8) | 0x03)

/** @brief Returns the extended bootloader type of the bootloader that
 *         is present on the chip.
 */
BlExtendedType halBootloaderGetInstalledType(void);



/** @brief Define an invalid bootloader version.
 */
#define BOOTLOADER_INVALID_VERSION 0xFFFF

/** @brief Returns the version of the installed bootloader, regardless of its
 * type.
 *
 * @return Version if bootloader installed, or ::BOOTLOADER_INVALID_VERSION.
 *          A returned version of 0x1234 would indicate version 1.2 build 34
 */
uint16_t halGetBootloaderVersion(void);


/** @brief Return extended bootloader version information, if supported. This API
 * is not supported for EM2XX chips and only returns extra information on bootloaders
 * built on or after the 4.7 release.
 *
 * @param emberVersion If specified, we will return the full 32bit ember version for
 *  this bootloader. Format is major, minor, patch, doc (4bit nibbles) followed by
 *  a 16bit build number.
 * @param customerVersion This will return the 32bit value specified in
 *  CUSTOMER_BOOTLOADER_VERSION at build time.
 */
void halGetExtendedBootloaderVersion(uint32_t* emberVersion, uint32_t* customerVersion);


/** @brief Macro defining the customer application version stored in the
 *         ApplicationProperties_t struct.
 */
#ifndef CUSTOMER_APPLICATION_VERSION
  #define CUSTOMER_APPLICATION_VERSION 0
#endif

/** @brief Macro defining the customer application capabilities stored in the
 *         ApplicationProperties_t struct.
 *  @note The capabilities field in the ApplicationProperties_t struct is shared
 *        with other values.
 */
#ifndef CUSTOMER_APPLICATION_CAPABILITIES
  #define CUSTOMER_APPLICATION_CAPABILITIES 0
#endif

/** @brief Macro defining the customer application product ID stored in the
 *         ApplicationProperties_t struct.
 */
#ifndef CUSTOMER_APPLICATION_PRODUCT_ID
  #define CUSTOMER_APPLICATION_PRODUCT_ID {0}
#endif

/** @brief Macro defining the support for the MPSI protocol stored in the
 *         capabilities field of the ApplicationProperties_t struct.
 */
#ifdef EMBER_AF_PLUGIN_MPSI_SUPPORT
  #define MPSI_PLUGIN_SUPPORT 1
#else
  #define MPSI_PLUGIN_SUPPORT 0
#endif

/** @brief Macro defining the bit position that corresponds to MPSI support in
 *         the capabilities field of the ApplicationProperties_t struct.
 */
#define APPLICATION_PROPERTIES_CAPABILITIES_MPSI_SUPPORT_BIT 31

/** @brief Macro defining the capabilities that this application has. This value
 *         is set in the capabilities field of the ApplicationProperties_t
 *         struct.
 */
#define APPLICATION_PROPERTIES_CAPABILITIES \
  (MPSI_PLUGIN_SUPPORT << APPLICATION_PROPERTIES_CAPABILITIES_MPSI_SUPPORT_BIT)\
| (CUSTOMER_APPLICATION_CAPABILITIES & 0x7FFFFFFF)

#endif // __BOOTLOADER_INTERFACE_H__

/**@}  // end group
 */
