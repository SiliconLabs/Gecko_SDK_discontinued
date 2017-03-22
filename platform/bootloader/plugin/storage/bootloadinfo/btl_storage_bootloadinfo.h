/***************************************************************************//**
 * @file btl_storage_bootloadinfo.h
 * @brief Bootload Info for Silicon Labs Bootloader.
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
#ifndef BTL_STORAGE_BOOTLOADINFO_H
#define BTL_STORAGE_BOOTLOADINFO_H

#include <stdint.h>
#include <stddef.h>

/***************************************************************************//**
 * @addtogroup Plugin
 * @{
 * @addtogroup Storage
 * @{
 * @addtogroup BootloadInfo Bootload Info
 * @{
 * @brief Info about what firmware update image should be bootloaded next
 * @details
 *   This plugin provides the bootloader with support for storing multiple
 *   images, and attempting to bootload a prioritized list of them. The Bootload
 *   Info struct is placed at a known location in storage, and points to a list
 *   of images to attempt to bootload.
 *
 *   While the Bootload Info list of images to attempt to bootload has a
 *   compile-time configurable size, the bootloader is capable of handling lists
 *   of images with different sizes, e.g. if a bootloader upgrade changes the
 *   slot layout, or if a storage device that was initialized on a different
 *   device is used.
 *
 *   If only one storage slot is available, the functions available in this API
 *   will do nothing, and return applicable error codes (@ref BOOTLOADER_OK for
 *   @ref storage_getBootloadList and @ref storage_setBootloadList, and
 *   @ref BOOTLOADER_ERROR_BOOTLOAD_LIST_FULL for
 *   @ref storage_appendBootloadList).
 ******************************************************************************/

/// Magic word indicating @ref BootloadInfo_t struct
#define BTL_STORAGE_BOOTLOADINFO_MAGIC 0xE18F5239UL

/// Version number for the @ref BootloadInfo_t struct
#define BTL_STORAGE_BOOTLOADINFO_VERSION 0x00000001UL

/// Maximum number of items in the @ref BootloadInfo_t.bootloadList
#define BTL_STORAGE_BOOTLOAD_LIST_MAX_LENGTH 16UL

/// Definition of the Bootload Info struct.
///
/// The Bootload Info struct contains a prioritized list of firmware update
/// images that the
/// bootloader should attempt to bootload. The first image to pass verification
/// will be bootloaded.
typedef struct {
  /// @brief Magic word indicating that this is a Bootload Info struct
  ///        (@ref BTL_STORAGE_BOOTLOADINFO_MAGIC)
  uint32_t  magic;
  /// Struct version number
  uint32_t  structVersion;
  /// Size of the BootloadInfo_t struct
  uint32_t  length;
  /// List of addresses of slots to bootload from.
  int32_t  bootloadList[BTL_STORAGE_BOOTLOAD_LIST_MAX_LENGTH];
  /// Checksum of the struct
  uint32_t  crc32;
} BootloadInfo_t;

/***************************************************************************//**
 * Get list of firmware update images to attempt to bootload
 *
 * @param[out] slotIds Pointer to array of integers to fill with slot IDs,
 *                     or -1 if list position does not contain a valid slot ID.
 * @param[in]  length  Number of slot IDs to get.
 *
 * @return Error code: BOOTLOADER_OK if list of slots was successfully filled
 *          else error code in @ref BOOTLOADER_ERROR_STORAGE_BASE range
 ******************************************************************************/
int32_t storage_getBootloadList(int32_t slotIds[], size_t length);

/***************************************************************************//**
 * Set list of firmware update images to attempt to bootload
 *
 * @param[in]  slotIds Pointer to array of slot IDs to set
 * @param[in]  length  Number of slot IDs to set.
 *
 * @return Error code: BOOTLOADER_OK if list of slots was successfully filled
 ******************************************************************************/
int32_t storage_setBootloadList(int32_t slotIds[], size_t length);

/***************************************************************************//**
 * Append a storage slot to the list of storage slots to attempt to bootload
 * from
 *
 * @param[in]  slotId  ID of slot
 *
 * @return Error code: BOOTLOADER_OK if slot was successfully appended
 ******************************************************************************/
int32_t storage_appendBootloadList(int32_t slotId);

/** @} // addtogroup BootloadInfo */
/** @} // addtogroup Storage */
/** @} // addtogroup Plugin */

#endif // BTL_STORAGE_BOOTLOADINFO_H
