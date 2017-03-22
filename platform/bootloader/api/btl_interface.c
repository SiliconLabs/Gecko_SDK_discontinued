/***************************************************************************//**
 * @file btl_interface.c
 * @brief Application interface to the bootloader.
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

#include "btl_interface.h"

void bootloader_getInfo(BootloaderInformation_t *info)
{
  if (!BTL_TABLE_PTR_VALID(mainBootloaderTable)) {
    info->type = NONE;
    info->capabilities = 0;
  } else if (mainBootloaderTable->header.type == BOOTLOADER_MAGIC_MAIN) {
    info->type = SL_BOOTLOADER;
    info->version = mainBootloaderTable->header.version;
    info->capabilities = mainBootloaderTable->capabilities;
  } else {
    info->type = NONE;
    info->capabilities = 0;
  }
}

int32_t bootloader_init(void)
{
  if (!BTL_TABLE_PTR_VALID(mainBootloaderTable)) {
    return BOOTLOADER_ERROR_INIT_TABLE;
  }
  return mainBootloaderTable->init();
}

int32_t bootloader_deinit(void)
{
  if (!BTL_TABLE_PTR_VALID(mainBootloaderTable)) {
    return BOOTLOADER_ERROR_INIT_TABLE;
  }
  return mainBootloaderTable->deinit();
}

void bootloader_rebootAndInstall(void)
{
  // Clear resetcause
  RMU->CMD = RMU_CMD_RCCLR;

  // Set reset reason to bootloader entry
  BootloaderResetCause_t* resetCause = (BootloaderResetCause_t*) (RAM_MEM_BASE);
  resetCause->reason = BOOTLOADER_RESET_REASON_BOOTLOAD;
  resetCause->signature = BOOTLOADER_RESET_SIGNATURE_VALID;

  // Trigger a software system reset
  RMU->CTRL = (RMU->CTRL & ~_RMU_CTRL_SYSRMODE_MASK) | RMU_CTRL_SYSRMODE_FULL;
  NVIC_SystemReset();
}

int32_t bootloader_initParser(BootloaderParserContext_t *context,
                              size_t contextSize)
{
  if (!BTL_TABLE_PTR_VALID(mainBootloaderTable)) {
    return BOOTLOADER_ERROR_PARSE_FAILED;
  }
  return mainBootloaderTable->initParser(context, contextSize);
}

int32_t bootloader_parseBuffer(BootloaderParserContext_t *context,
                               BootloaderParserCallbacks_t *callbacks,
                               uint8_t data[],
                               size_t numBytes)
{
  if (!BTL_TABLE_PTR_VALID(mainBootloaderTable)) {
    return BOOTLOADER_ERROR_PARSE_FAILED;
  }
  return mainBootloaderTable->parseBuffer(context, callbacks, data, numBytes);
}

bool bootloader_verifyApplication(uint32_t startAddress)
{
  if (!BTL_TABLE_PTR_VALID(mainBootloaderTable)) {
    return false;
  }
  return mainBootloaderTable->verifyApplication(startAddress);
}
