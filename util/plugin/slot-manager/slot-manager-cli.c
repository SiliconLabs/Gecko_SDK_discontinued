/**************************************************************************//**
 * Copyright 2017 Silicon Laboratories, Inc.
 *
 *****************************************************************************/

#include "slot-manager-cli.h"

#ifdef EMBER_AF_API_AF_HEADER
 #include EMBER_AF_API_AF_HEADER
#endif

#if defined (EMBER_STACK_ZIGBEE)
#include "app/util/serial/command-interpreter2.h"
EmberCommandEntry emberAfPluginSlotManagerCommands[] = {SLOT_MANAGER_COMMAND_LIST};
#elif defined (EMBER_STACK_BLE)
#include COMMON_HEADER
#include "command_interpreter.h"
#else
#error "Slot Manger CLI: Unsupported stack!"
#endif // EMBER_STACK_ZIGBEE

#include "api/btl_interface.h"
#include "slot-manager.h"

void emAfPluginSlotManagerCliBootloadSlot(CLI_HANDLER_PARAM_LIST)
{
  uint32_t slotId = (uint32_t)emAfPluginSlotManagerCliUnsignedCommandArgument(0);

  if (emberAfPluginSlotManagerVerifyAndBootloadSlot(slotId) != SLOT_MANAGER_SUCCESS) {
    slotManagerPrintln("Unable to boot image at slot %d", slotId);
  }
}

void emAfPluginSlotManagerCliEraseSlot(CLI_HANDLER_PARAM_LIST)
{
  uint32_t slotId = (uint32_t)emAfPluginSlotManagerCliUnsignedCommandArgument(0);
  if (SLOT_MANAGER_SUCCESS != emberAfPluginSlotManagerEraseSlot(slotId)) {
    slotManagerPrintln("Slot Manager failed to erase slot");
  }
}

void emAfPluginSlotManagerCliPrintExternalFlashInfo(CLI_HANDLER_PARAM_LIST)
{
  emberAfPluginSlotManagerPrintExternalFlashInfo();
}

void emAfPluginSlotManagerCliReadExtFlash(CLI_HANDLER_PARAM_LIST)
{
  uint8_t offset, data[MAX_FLASH_READ_BYTES];
  uint32_t address = (uint32_t)emAfPluginSlotManagerCliUnsignedCommandArgument(0);
  uint8_t len = (uint8_t)emAfPluginSlotManagerCliUnsignedCommandArgument(1);

  if (len > MAX_FLASH_READ_BYTES) {
    slotManagerPrintln("Max read limit set to %d bytes", MAX_FLASH_READ_BYTES);
    len = MAX_FLASH_READ_BYTES;
  }

  if (emberAfPluginSlotManagerReadExtFlash(address, data, len) == SLOT_MANAGER_SUCCESS) {
    slotManagerPrintln("Address          Data");
    for (offset = 0; offset < (len-1); offset++) {
      slotManagerPrintln("0x%4x       0x%x", address + offset, data[offset]);
    }
  }
  else {
    slotManagerPrintln("Failed to read from flash");
  }
}

void emAfPluginSlotManagerCliPrintSlotsInfo(CLI_HANDLER_PARAM_LIST)
{
  uint32_t slotId = 0;
  SlotManagerSlotInfo_t slotInfo;
  bool imagePresentInSlot;
  uint8_t index;

  while (SLOT_MANAGER_SUCCESS ==
         emberAfPluginSlotManagerGetSlotInfo(slotId, &slotInfo)) {
    imagePresentInSlot = true;
    slotManagerPrintln("Slot %d\n"
                       "  Address     : 0x%4X\n"
                       "  Length      : 0x%4X (%d bytes)",
                       slotId,
                       slotInfo.slotStorageInfo.address,
                       slotInfo.slotStorageInfo.length,
                       slotInfo.slotStorageInfo.length);
    slotManagerPrint("  Type        : ");
    switch (slotInfo.slotAppInfo.type)
    {
      case 0:
        // This means there's no image there
        slotManagerPrintln("No image present");
        imagePresentInSlot = false;
        break;
      case APPLICATION_TYPE_ZIGBEE:
        slotManagerPrintln("ZigBee");
        break;
      case APPLICATION_TYPE_THREAD:
        slotManagerPrintln("Thread");
        break;
      case APPLICATION_TYPE_FLEX:
        slotManagerPrintln("Connect");
        break;
      case APPLICATION_TYPE_BLUETOOTH:
        slotManagerPrintln("Bluetooth");
        break;
      case APPLICATION_TYPE_BLUETOOTH_APP:
        slotManagerPrintln("Bluetooth application");
        break;
      case APPLICATION_TYPE_MCU:
        slotManagerPrintln("MCU");
        break;
      default:
        slotManagerPrintln("Other (0x%X)", slotInfo.slotAppInfo.type);
        break;
    }

    if (imagePresentInSlot) {
      slotManagerPrintln("  Version     : 0x%4X",
                                          slotInfo.slotAppInfo.version);
      slotManagerPrintln("  Capabilities: 0x%4X",
                                          slotInfo.slotAppInfo.capabilities);
      slotManagerPrint  ("  Product ID  : ");
      uint8_t numProductIdDigits = COUNTOF(slotInfo.slotAppInfo.productId);
      for (index = 0; index < numProductIdDigits; index++) {
        slotManagerPrint("0x%X ", slotInfo.slotAppInfo.productId[index]);
      }
      slotManagerPrintln("");
    }
    slotId++;
  }
}

