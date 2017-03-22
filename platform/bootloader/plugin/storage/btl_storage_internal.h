/***************************************************************************//**
 * @file btl_storage_internal.h
 * @brief Internal header for the storage plugin of the Silicon Labs Bootloader.
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

#ifndef BTL_STORAGE_INTERNAL_H
#define BTL_STORAGE_INTERNAL_H

#include <stddef.h>
#include <stdbool.h>

#include "api/btl_interface.h"
#include "btl_storage.h"

/// Size of the buffer used for chunked reads from storage
#define BTL_STORAGE_READ_BUFFER_SIZE            (128UL)

/// Storage layout information for the storage implementation.
extern const BootloaderStorageLayout_t storageLayout;

/***************************************************************************//**
 * Get information about the connected storage device.
 *
 * @return Pointer to device information.
 ******************************************************************************/
const BootloaderStorageImplementationInformation_t * getDeviceInfo(void);

/***************************************************************************//**
 * Get base address of the storage medium to use for bootloader storage.
 *
 * @return The base address of the storage medium.
 ******************************************************************************/
uint32_t storage_getBaseAddress(void);

/***************************************************************************//**
 * Custom storage init function.
 *
 * This function can be implemented by the user to perform extra operations
 * during initialization of the storage plugin, such as turning on a GPIO power
 * pin to an external storage medium, toggling a write protect pin, etc.
 * The custom init function is called by @ref storage_init.
 ******************************************************************************/
void storage_customInit(void);

/***************************************************************************//**
 * Custom storage shutdown function.
 *
 * This function can be implemented by the user to perform extra operations
 * during shutdown of the storage plugin, such as turning off a GPIO power
 * pin to an external storage medium.
 * The custom shutdown function is called by @ref storage_shutdown.
 ******************************************************************************/
void storage_customShutdown(void);

#endif // BTL_STORAGE_INTERNAL_H
