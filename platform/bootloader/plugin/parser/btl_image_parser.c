/***************************************************************************//**
 * @file btl_image_parser.c
 * @brief Image parser for Silicon Labs bootloader
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

#include <stdbool.h>
#include "api/btl_interface.h"

bool parser_requireAuthenticity(void)
{
#if defined(BOOTLOADER_ENFORCE_SIGNED_UPGRADE)
  return true;
#else
  return false;
#endif
}

bool parser_requireConfidentiality(void)
{
#if defined(BOOTLOADER_ENFORCE_ENCRYPTED_UPGRADE)
  return true;
#else
  return false;
#endif
}

bool parser_allowLegacyFormat(void)
{
  // Default to backwards compatibility not allowed
#if defined(BTL_EBL_PARSER_ALLOW_V2)
  return true;
#else
  return false;
#endif
}

uint32_t parser_getApplicationAddress(void)
{
  return (uint32_t)(&(mainBootloaderTable->startOfAppSpace->stackTop));
}

uint32_t parser_getBootloaderUpgradeAddress(void)
{
  if (firstBootloaderTable->header.type == BOOTLOADER_MAGIC_FIRST_STAGE) {
    return (uint32_t)(&(firstBootloaderTable->upgradeLocation->stackTop));
  } else {
    return 0UL;
  }
}
