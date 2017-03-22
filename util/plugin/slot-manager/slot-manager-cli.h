/**************************************************************************//**
 * Copyright 2017 Silicon Laboratories, Inc.
 *
 *****************************************************************************/

#ifndef __SLOT_MANAGER_CLI_H__
#define __SLOT_MANAGER_CLI_H__ 

#define MAX_FLASH_READ_BYTES    100

#if defined (EMBER_STACK_ZIGBEE)
#define emAfPluginSlotManagerCliEntryAction(command, function, paramList, help) \
        emberCommandEntryAction(command, function, paramList, help)

#define emAfPluginSlotManagerCliEntryTerminator() \
        emberCommandEntryTerminator()

#define emAfPluginSlotManagerCliSignedCommandArgument(arg) \
        emberSignedCommandArgument(arg)

#define emAfPluginSlotManagerCliUnsignedCommandArgument(arg) \
        emberUnsignedCommandArgument(arg)

#define CLI_HANDLER_PARAM_LIST void

#elif defined (EMBER_STACK_BLE)
#define emAfPluginSlotManagerCliEntryAction(command, function, paramList, help) \
        COMMAND_ENTRY(command, paramList, function, help)

#define emAfPluginSlotManagerCliEntryTerminator() \
        COMMAND_SEPARATOR("")

#define emAfPluginSlotManagerCliSignedCommandArgument(arg) \
        ciGetSigned(buf[arg+1])

#define emAfPluginSlotManagerCliUnsignedCommandArgument(arg) \
        ciGetUnsigned(buf[arg+1])

#define CLI_HANDLER_PARAM_LIST int abc, char **buf

#else
#error "Slot Manger CLI: Unsupported stack!"
#endif

#define SLOT_MANAGER_COMMAND_LIST \
  emAfPluginSlotManagerCliEntryAction("set-slot-bootload", \
                          emAfPluginSlotManagerCliBootloadSlot, "w", \
                          "Set slotId and bootload image from external flash"), \
  emAfPluginSlotManagerCliEntryAction("erase-slot", \
                          emAfPluginSlotManagerCliEraseSlot, "w", \
                          "Erases the specified slot"), \
  emAfPluginSlotManagerCliEntryAction("ext-flash-info", \
                          emAfPluginSlotManagerCliPrintExternalFlashInfo, "", \
                          "Print the external flash information"), \
  emAfPluginSlotManagerCliEntryAction("read", \
                          emAfPluginSlotManagerCliReadExtFlash, "wu", \
                          "Read raw data from external flash"), \
  emAfPluginSlotManagerCliEntryAction("slots-info", \
                          emAfPluginSlotManagerCliPrintSlotsInfo, "", \
                          "Print information for all available slots"), \
  emAfPluginSlotManagerCliEntryTerminator()



void emAfPluginSlotManagerCliBootloadSlot(CLI_HANDLER_PARAM_LIST);
void emAfPluginSlotManagerCliEraseSlot(CLI_HANDLER_PARAM_LIST);
void emAfPluginSlotManagerCliPrintExternalFlashInfo(CLI_HANDLER_PARAM_LIST);
void emAfPluginSlotManagerCliReadExtFlash(CLI_HANDLER_PARAM_LIST);
void emAfPluginSlotManagerCliPrintSlotsInfo(CLI_HANDLER_PARAM_LIST);

#endif // __SLOT_MANAGER_CLI_H__