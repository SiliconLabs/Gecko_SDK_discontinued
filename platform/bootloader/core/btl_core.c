/***************************************************************************//**
 * @file btl_core.c
 * @brief Core functionality for Silicon Labs bootloader.
 * @author Silicon Labs
 * @version 1.0.0
 *******************************************************************************
 * @section License
 * <b>Copyright 2016 Silicon Laboratories, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include "config/btl_config.h"

#include "api/btl_interface.h"
#include "btl_core.h"
#include "em_device.h"

#ifdef BOOTLOADER_SUPPORT_STORAGE
#include "plugin/storage/btl_storage.h"
#endif

int32_t btl_init(void)
{
  int32_t retval = BOOTLOADER_OK;

#ifdef BOOTLOADER_SUPPORT_STORAGE
  retval = storage_init();
  if (retval != BOOTLOADER_OK) {
    return retval;
  }
#endif

  return retval;
}

int32_t btl_deinit(void)
{
  int32_t retval = BOOTLOADER_OK;

#ifdef BOOTLOADER_SUPPORT_STORAGE
  retval = storage_shutdown();
  if (retval != BOOTLOADER_OK) {
    return retval;
  }
#endif

  return retval;
}
