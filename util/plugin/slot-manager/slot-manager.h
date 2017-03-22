/**************************************************************************//**
 * Copyright 2017 Silicon Laboratories, Inc.
 *
 *****************************************************************************/

#ifndef __SLOT_MANAGER_H__
#define __SLOT_MANAGER_H__

#include "api/btl_interface.h"
#include "api/btl_interface_storage.h"

#if defined (EMBER_STACK_ZIGBEE)
#include "app/framework/include/af.h"
#define slotManagerPrint(...) emberAfAppPrint(__VA_ARGS__)
#define slotManagerPrintln(...) emberAfAppPrintln(__VA_ARGS__)
#define pokeStackOrWatchDog()  halInternalResetWatchDog()

#elif defined (EMBER_STACK_BLE)
#define slotManagerPrint(...) printf(__VA_ARGS__)
#define slotManagerPrintln(...) do {printf(__VA_ARGS__); printf("\n"); } while (0)
#define pokeStackOrWatchDog() (void)0

#else
#error "Slot Manager plugin: Stack is either not defined or not supported!"
#endif // EMBER_STACK_ZIGBEE

// Return values
#define   SLOT_MANAGER_SUCCESS          0
#define   SLOT_MANAGER_ERROR            1
#define   SLOT_MANAGER_INVALID_CALL     2
#define   SLOT_MANAGER_BAD_ARG          3

#define   SLOT_MANAGER_INVALID_SLOT               (uint32_t)-1
#define   SLOT_MANAGER_VERIFICATION_CONTEXT_SIZE  384

// Defines
typedef struct {
  BootloaderStorageSlot_t slotStorageInfo;
  ApplicationData_t       slotAppInfo;
} SlotManagerSlotInfo_t;

// Prototypes
void emberAfPluginSlotManagerInitCallback(void);

void      emberAfPluginSlotManagerPrintExternalFlashInfo(void);
uint8_t   emberAfPluginSlotManagerReadExtFlash(uint32_t address,
                                               uint8_t* data,
                                               uint8_t len);
uint8_t   emberAfPluginSlotManagerVerifyAndBootloadSlot(uint32_t slotId);
uint8_t   emberAfPluginSlotManagerGetSlotInfo(uint32_t slotId,
                                              SlotManagerSlotInfo_t* slotInfo);
uint8_t   emberAfPluginSlotManagerWriteToSlot(uint32_t slotId,
                                              uint32_t offset,
                                              uint8_t* buffer,
                                              size_t length);
uint8_t   emberAfPluginSlotManagerEraseSlot(uint32_t slotId);


#endif  //__SLOT_MANAGER_H__
