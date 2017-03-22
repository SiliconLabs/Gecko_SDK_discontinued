/***************************************************************************//**
 * @file btl_interface.h
 * @brief Application interface to the bootloader.
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
#ifndef BTL_INTERFACE_H
#define BTL_INTERFACE_H

#include <stddef.h>

#include "btl_errorcode.h"
#include "btl_reset_info.h"
#include "application_properties.h"

// Include plugin-specific interfaces
#include "btl_interface_parser.h"
#include "btl_interface_storage.h"

// Get flash page size
#include "core/btl_util.h"
MISRAC_DISABLE
#include "em_device.h"
MISRAC_ENABLE

/***************************************************************************//**
 * @addtogroup Interface Application Interface
 * @brief Application interface to the bootloader
 * @details
 *   The application interface consists of functions that can be included
 *   into the customer application, and will communicate with the
 *   bootloader through the @ref MainBootloaderTable_t. This table
 *   contains function pointers into the bootloader. The 10th word of the
 *   bootloader contains a pointer to this struct, allowing any application to
 *   easily locate
 *   Using the wrapper functions is preferred over accessing the
 *   bootloader table directly.
 *
 * @{
 * @addtogroup CommonInterface Common Application Interface
 * @brief Generic application interface available on all versions of the
 *        bootloader, independently of which plugins are present.
 * @details
 * @{
 ******************************************************************************/

/// Bare boot table. Can be mapped on top of vector table to access contents.
typedef struct {
  /// Pointer to top of stack
  uint32_t *stackTop;
  /// Pointer to reset vector
  void     (*resetVector)(void);
  /// Reserved pointers to fault handlers
  uint32_t reserved0[5];
  /// Reserved pointers to RESERVED fields
  uint32_t reserved1[3];
  /// Pointer to bootloader table
  void     *table;
  /// Reserved pointers to SVC and DebugMon interrupts
  uint32_t reserved2[2];
  /// Pointer to application signature
  void     *signature;
} BareBootTable_t;

// --------------------------------
// Bootloader information typedefs

/// Type of bootloader
typedef enum {
  /// No bootloader present.
  NONE,
  /// Bootloader is a Silicon Labs bootloader.
  SL_BOOTLOADER
} BootloaderType_t;

/// Information about the current bootloader
typedef struct {
  /// The type of bootloader.
  BootloaderType_t type;
  /// Version number of the bootloader
  uint32_t version;
  /// Capability mask for the bootloader.
  uint32_t capabilities;
} BootloaderInformation_t;

/// Common header for bootloader tables
typedef struct {
  /// Type of image
  uint32_t type;
  /// Version number of the bootloader/application table
  uint32_t layout;
  /// Version number of the image
  uint32_t version;
} BootloaderHeader_t;

/// Address table for the First Stage Bootloader
typedef struct {
  /// Header of the First Stage Bootloader table
  BootloaderHeader_t header;
  /// Start address of the Main Bootloader
  BareBootTable_t *mainBootloader;
  /// Location of the Main Bootloader upgrade image
  BareBootTable_t *upgradeLocation;
} FirstBootloaderTable_t;

/// Address table for the Main Bootloader
typedef struct {
  /// Header of the Main Bootloader table
  BootloaderHeader_t header;
  /// Size of the Main Bootloader
  uint32_t size;
  /// Start address of the application
  BareBootTable_t *startOfAppSpace;
  /// End address of the allocated application space
  uint32_t *endOfAppSpace;
  /// Capabilities of the bootloader
  uint32_t capabilities;
  // ------------------------------
  /// Initialize bootloader for use from application
  int32_t (*init)(void);
  /// Deinitialize bootloader after use from application
  int32_t (*deinit)(void);
  // ------------------------------
  /// Verify application
  bool (*verifyApplication)(uint32_t startAddress);
  // ------------------------------
  /// Initialize parser
  int32_t (*initParser)(BootloaderParserContext_t *context, size_t contextSize);
  /// Parse a buffer
  int32_t (*parseBuffer)(BootloaderParserContext_t *context, const BootloaderParserCallbacks_t *callbacks, uint8_t data[], size_t numBytes);

  // ------------------------------
  /// Function table for storage plugin
  const BootloaderStorageFunctions_t *storage;
} MainBootloaderTable_t;

// --------------------------------
// Bootloader capabilities

/// Bootloader enforces signed application upgrade images
#define BOOTLOADER_CAPABILITY_ENFORCE_UPGRADE_SIGNATURE   (1 << 0)
/// Bootloader enforces encrypted application upgrade images
#define BOOTLOADER_CAPABILITY_ENFORCE_UPGRADE_ENCRYPTION  (1 << 1)
/// @brief Bootloader enforces signature verification of the application image
///        before every boot
#define BOOTLOADER_CAPABILITY_ENFORCE_SECURE_BOOT         (1 << 2)

/// Bootloader has the capability of being upgraded
#define BOOTLOADER_CAPABILITY_BOOTLOADER_UPGRADE          (1 << 4)

/// Bootloader has the capability of parsing EBL files
#define BOOTLOADER_CAPABILITY_EBL                         (1 << 5)
/// Bootloader has the capability of parsing signed EBL files
#define BOOTLOADER_CAPABILITY_EBL_SIGNATURE               (1 << 6)
/// Bootloader has the capability of parsing encrypted EBL files
#define BOOTLOADER_CAPABILITY_EBL_ENCRYPTION              (1 << 7)

/// @brief Bootloader has the capability of storing data in an internal or external
/// storage medium
#define BOOTLOADER_CAPABILITY_STORAGE                     (1 << 16)
/// @brief Bootloader has the capability of communicating with host processors using
/// a communication interface
#define BOOTLOADER_CAPABILITY_COMMUNICATION               (1 << 20)

// --------------------------------
// Magic constants for bootloader tables

/// Magic word indicating first stage bootloader table
#define BOOTLOADER_MAGIC_FIRST_STAGE          (0xB00710ADUL)
/// Magic word indicating main bootloader table
#define BOOTLOADER_MAGIC_MAIN                 (0x5ECDB007UL)

/// @cond DO_NOT_INCLUDE_WITH_DOXYGEN
#define BOOTLOADER_HEADER_VERSION_FIRST_STAGE (0x00000001UL)
#define BOOTLOADER_HEADER_VERSION_MAIN        (0x00000001UL)
/// @endcond

// --------------------------------
// Bootloader table access

/// @cond DO_NOT_INCLUDE_WITH_DOXYGEN

#define BTL_FIRST_STAGE_SIZE              ((size_t)(FLASH_PAGE_SIZE))

#if defined(_SILICON_LABS_GECKO_INTERNAL_SDID_80)
// EFM32PG1, EFM32JG1, EFR32MG1, EFR32BG1 and EFR32FG1 don't have writeable
// bootloader area: Place the bootloader in main flash
#define BTL_FIRST_STAGE_BASE              0x00000000UL
#define BTL_APPLICATION_BASE              0x00004000UL
#define BTL_MAIN_STAGE_MAX_SIZE           (BTL_APPLICATION_BASE - BTL_FIRST_STAGE_SIZE)
#elif defined(_SILICON_LABS_GECKO_INTERNAL_SDID_84)
// EFM32PG12, EFM32JG12, EFR32xG12 have a dedicated bootloader area of 38k
// Place the bootloader in the dedicated bootloader area of the information block
#define BTL_FIRST_STAGE_BASE              0x0FE10000UL
#define BTL_APPLICATION_BASE              0x00000000UL
#define BTL_MAIN_STAGE_MAX_SIZE           (0x00009800UL - BTL_FIRST_STAGE_SIZE)
#elif defined(_SILICON_LABS_GECKO_INTERNAL_SDID_89)
// EFM32PG13, EFM32JG13, EFR32xG13 have a dedicated bootloader area of 16k
// Place the bootloader in the dedicated bootloader area of the information block
#define BTL_FIRST_STAGE_BASE              0x0FE10000UL
#define BTL_APPLICATION_BASE              0x00000000UL
#define BTL_MAIN_STAGE_MAX_SIZE           (0x00004000UL - BTL_FIRST_STAGE_SIZE)
#else
#error "This part is not supported in this bootloader version."
#endif

#define BTL_MAIN_STAGE_BASE               (BTL_FIRST_STAGE_BASE + BTL_FIRST_STAGE_SIZE)
#define BTL_TABLE_PTR_VALID(table)        (((size_t)(table) >= BTL_MAIN_STAGE_BASE) \
                                           && ((size_t)(table) < BTL_MAIN_STAGE_BASE + BTL_MAIN_STAGE_MAX_SIZE))

#define BTL_FIRST_BOOTLOADER_TABLE_BASE   (BTL_FIRST_STAGE_BASE + offsetof(BareBootTable_t, table))
#define BTL_MAIN_BOOTLOADER_TABLE_BASE    (BTL_MAIN_STAGE_BASE + offsetof(BareBootTable_t, table))

/// @endcond // DO_NOT_INCLUDE_WITH_DOXYGEN

/// Pointer to first stage bootloader table
#define firstBootloaderTable   (*(FirstBootloaderTable_t **)(BTL_FIRST_BOOTLOADER_TABLE_BASE))
/// Pointer to main bootloader table
#define mainBootloaderTable    (*(MainBootloaderTable_t **)(BTL_MAIN_BOOTLOADER_TABLE_BASE))

// --------------------------------
// Functions

/***************************************************************************//**
 * Get information about the bootloader on this device.
 *
 * The information returned is fetched from the main bootloader
 * information table
 *
 * @param[out] info Pointer to bootloader information struct.
 ******************************************************************************/
void bootloader_getInfo(BootloaderInformation_t *info);

/***************************************************************************//**
 * Initialize components of the bootloader that need to be initialized
 * for the app to use the interface. This typically includes initializing
 * serial peripherals for communication with external SPI flashes, etc.
 *
 * @return Error code. @ref BOOTLOADER_OK on success, else error code in
 *         @ref BOOTLOADER_ERROR_INIT_BASE range.
 ******************************************************************************/
int32_t bootloader_init(void);

/***************************************************************************//**
 * De-initialize components of the bootloader that were previously initialized.
 * This typically includes powering down external SPI flashes, and de-initializing
 * the serial peripheral used for communication with the external flash.
 *
 * @return Error code. @ref BOOTLOADER_OK on success, else error code in
 *         @ref BOOTLOADER_ERROR_INIT_BASE range.
 ******************************************************************************/
int32_t bootloader_deinit(void);

/***************************************************************************//**
 * Reboot into the bootloader, with the purpose of installing something.
 *
 * If a storage plugin is present, and a slot is marked for bootload, install
 * the image in that slot after verifying it.
 *
 * If a communication plugin is present, open the communication channel and receive
 * an image to be installed.
 ******************************************************************************/
void bootloader_rebootAndInstall(void);

/***************************************************************************//**
 * Verify that the application starting at startAddress is valid.
 *
 * @param[in] startAddress Starting address of the application
 *
 * @return True if the application is valid, else false.
 ******************************************************************************/
bool bootloader_verifyApplication(uint32_t startAddress);

/** @} // addtogroup CommonInterface */
/** @} // addtogroup Interface */

#endif // BTL_INTERFACE_H
