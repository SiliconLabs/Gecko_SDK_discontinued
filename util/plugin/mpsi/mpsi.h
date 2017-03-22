/**************************************************************************//**
 * Copyright 2017 Silicon Laboratories, Inc.
 *
 *****************************************************************************/

#ifndef MPSI_H
#define MPSI_H

//------------------------------------------------------------------------------
// Includes

#include PLATFORM_HEADER
#ifdef EMBER_AF_API_AF_HEADER
 #include EMBER_AF_API_AF_HEADER
#endif

// BLE stack generates different prefix for plugin definitions
#if defined(SILABS_AF_PLUGIN_MPSI_STORAGE)
#define EMBER_AF_PLUGIN_MPSI_STORAGE
#endif

// BLE stack generates different prefix for plugin definitions
#if defined(SILABS_AF_PLUGIN_MPSI_IPC)
#define EMBER_AF_PLUGIN_MPSI_IPC
#endif

#if !defined(BLE_NCP_MOBILE_APP)
 #if defined(EMBER_AF_PLUGIN_MPSI_STORAGE)
  #include "mpsi-storage.h"
 #elif defined(EMBER_AF_PLUGIN_MPSI_IPC)
  #include "mpsi-ipc.h"
 #else
  #error "You must include either the MPSI Storage plugin or the MPSI IPC plugin"
 #endif // EMBER_AF_PLUGIN_MPSI_STORAGE || EMBER_AF_PLUGIN_MPSI_IPC
#endif // BLE_NCP_MOBILE_APP

#include "mpsi-message-ids.h"

//------------------------------------------------------------------------------
// Defines

#if defined(EMBER_STACK_ZIGBEE)
 #define mpsiPrint(...)   emberAfAppPrint(__VA_ARGS__)
 #define mpsiPrintln(...) emberAfAppPrintln(__VA_ARGS__)
#elif defined(EMBER_STACK_BLE)
 #define mpsiPrint(...)   do { printf(__VA_ARGS__); } while(0)
 #define mpsiPrintln(...) do { printf(__VA_ARGS__); printf("\n"); } while(0)
#else
 #error "Stack not defined"
#endif

#if defined(EMBER_AF_PLUGIN_MPSI_STORAGE)
 #define mpsiSendMessageToStack(...) emAfPluginMpsiStorageStoreMessage(__VA_ARGS__)
#elif defined(EMBER_AF_PLUGIN_MPSI_IPC)
 #define mpsiSendMessageToStack(...) emAfPluginMpsiIpcSendMessage(__VA_ARGS__)
#elif defined(BLE_NCP_MOBILE_APP)
 #define mpsiSendMessageToStack(...) emAfPluginSendMpsiMessageToStack(__VA_ARGS__)
#endif // EMBER_AF_PLUGIN_MPSI_STORAGE || EMBER_AF_PLUGIN_MPSI_IPC


// Return values
#define   MPSI_SUCCESS                  0
#define   MPSI_ERROR                    1
#define   MPSI_INVALID_PARAMETER        2
#define   MPSI_INVALID_FUNCTION         3
#define   MPSI_UNSUPPORTED_COMMAND      4
#define   MPSI_WRONG_APP                5
#define   MPSI_UNSUPPORTED_MPSI_FEATURE 6

//------------------------------------------------------------------------------
// Prototypes

void    emberAfPluginMpsiInitCallback(void);

// Public APIs
uint8_t emberAfPluginMpsiReceiveMessage(MpsiMessage_t* mpsiMessage);
uint8_t emberAfPluginMpsiSendMessage(MpsiMessage_t* mpsiMessage);

// Internal APIs
uint8_t emAfPluginMpsiMessageIdSupportedByLocalStack(uint16_t messageId);
uint8_t emAfPluginSendMpsiMessageToStack(MpsiMessage_t* mpsiMessage);
void    emAfPluginMpsiProcessStatus(MpsiMessage_t* mpsiMessage, uint8_t status);

// Private functions
bool emIsCustomMpsiMessage(uint16_t messageId);
uint8_t emProcessMpsiMessage(MpsiMessage_t* mpsiMessage);

#if defined(EMBER_STACK_BLE)
uint8_t emBleSendMpsiMessageToMobileApp(MpsiMessage_t* mpsiMessage);
#else
#define emBleSendMpsiMessageToMobileApp(...)  MPSI_INVALID_FUNCTION
#endif // EMBER_STACK_BLE

#endif  // MPSI_H
