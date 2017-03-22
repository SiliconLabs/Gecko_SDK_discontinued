/**************************************************************************//**
 * Copyright 2017 Silicon Laboratories, Inc.
 *
 *****************************************************************************/

#include "mpsi.h"
#include "mpsi-callbacks.h"
#include "mpsi-configuration.h"

#ifdef EMBER_AF_API_AF_SECURITY_HEADER
 #include EMBER_AF_API_AF_SECURITY_HEADER
#endif

#ifdef EMBER_AF_PLUGIN_SLOT_MANAGER
 #include "slot-manager.h"
#endif // EMBER_AF_PLUGIN_SLOT_MANAGER

#ifdef MPSI_MESSAGE_ID_INCLUDE_INITIATE_JOINING
 #ifdef EMBER_AF_API_NETWORK_STEERING
  #include EMBER_AF_API_NETWORK_STEERING
 #endif // EMBER_AF_API_NETWORK_STEERING
 #ifdef EMBER_AF_API_NETWORK_CREATOR_SECURITY
  #include EMBER_AF_API_NETWORK_CREATOR_SECURITY
 #else
  #error "The MPSI InitiateJoining message requires the Network Creator and "\
         "Network Creator Security plugins"
 #endif // EMBER_AF_API_NETWORK_CREATOR_SECURITY
#endif

// Sync this to bootloader-interface.c
#ifndef CUSTOMER_APPLICATION_VERSION
  #define CUSTOMER_APPLICATION_VERSION 0
#endif

// Forward declarations
#if defined (EMBER_AF_PLUGIN_SLOT_MANAGER)
uint16_t getMaxMessageIdSupportedBySlot(uint32_t slotId, uint32_t capabilities);
uint8_t bootloaderAppTypeToMpsiAppType(uint32_t type);
#endif // EMBER_AF_PLUGIN_SLOT_MANAGER

uint8_t mpsiHandleMessageGetAppsInfo(MpsiMessage_t* mpsiMessage)
{
  MpsiMessage_t response;
  MpsiAppsInfoMessage_t appsInfoMessage;
  uint8_t bytesSerialized;

  if (!mpsiMessage) {
    return MPSI_INVALID_PARAMETER;
  }

  response.destinationAppId = MPSI_APP_ID_MOBILE_APP;
  response.messageId = MPSI_MESSAGE_ID_APPS_INFO;
  response.payloadLength = 0;

  // First put in ourself in the response
  appsInfoMessage.slotId = INVALID_SLOT;
  appsInfoMessage.applicationId = MPSI_APP_ID;
  appsInfoMessage.applicationVersion = CUSTOMER_APPLICATION_VERSION;
  appsInfoMessage.maxMessageIdSupported = MPSI_MESSAGE_ID_MAX_ID;

  bytesSerialized = emAfPluginMpsiSerializeSpecificMessage(&appsInfoMessage,
                                                           response.messageId,
                                                           response.payload);
  if (0 == bytesSerialized) {
    mpsiPrintln("MPSI (0x%x) error: serialize error with len %d",
                response.messageId, sizeof(appsInfoMessage));
    return MPSI_ERROR;
  }
  response.payloadLength += bytesSerialized;

#if defined (EMBER_AF_PLUGIN_SLOT_MANAGER)
  // Now fill out the rest based on what's in the slots
  uint32_t slot = 0;
  SlotManagerSlotInfo_t slotInfo;
  while (SLOT_MANAGER_SUCCESS ==
         emberAfPluginSlotManagerGetSlotInfo(slot, &slotInfo)) {
    appsInfoMessage.slotId = slot;
    appsInfoMessage.applicationId =
      bootloaderAppTypeToMpsiAppType(slotInfo.slotAppInfo.type);
    appsInfoMessage.applicationVersion = slotInfo.slotAppInfo.version;
    appsInfoMessage.maxMessageIdSupported =
      getMaxMessageIdSupportedBySlot(slot, slotInfo.slotAppInfo.capabilities);

    bytesSerialized = emAfPluginMpsiSerializeSpecificMessage(
                                     &appsInfoMessage,
                                     response.messageId,
                                     response.payload + response.payloadLength);

    if (0 == bytesSerialized) {
      mpsiPrintln("MPSI (0x%x) error: serialize error with len %d",
                  response.messageId, sizeof(appsInfoMessage));
      return MPSI_ERROR;
    }
    response.payloadLength += bytesSerialized;

    slot++;
  }
#endif // EMBER_AF_PLUGIN_SLOT_MANAGER

  return emberAfPluginMpsiSendMessage(&response);
}

uint8_t mpsiHandleMessageAppsInfo(MpsiMessage_t* mpsiMessage)
{
  if (!mpsiMessage) {
    return MPSI_INVALID_PARAMETER;
  }

  // Nothing needs to be done here for the stacks currently
  // Receiving this message is of value to the Mobile App

  return MPSI_SUCCESS;
}

uint8_t mpsiHandleMessageBootloadSlot(MpsiMessage_t* mpsiMessage)
{
#if defined (EMBER_AF_PLUGIN_SLOT_MANAGER)
  MpsiBootloadSlotMessage_t message;
  uint8_t bytesDeserialized;
  uint8_t status;

  if (!mpsiMessage) {
    return MPSI_INVALID_PARAMETER;
  }

  if (mpsiMessage->payloadLength != sizeof(message)) {
    return MPSI_INVALID_PARAMETER;
  }

  bytesDeserialized = emAfPluginMpsiDeserializeSpecificMessage(
                                                    mpsiMessage->payload,
                                                    mpsiMessage->messageId,
                                                    &message);

  if (mpsiMessage->payloadLength != bytesDeserialized) {
    mpsiPrintln("MPSI (0x%x) error: deserialize error with len %d (%d)",
                mpsiMessage->messageId, bytesDeserialized,
                mpsiMessage->payloadLength);
    return MPSI_INVALID_PARAMETER;
  }

  // This won't return if it succeeds
  status = emberAfPluginSlotManagerVerifyAndBootloadSlot(message.slotId);

  return (SLOT_MANAGER_SUCCESS == status) ? MPSI_SUCCESS : MPSI_ERROR;
#else
  return MPSI_UNSUPPORTED_COMMAND;
#endif // EMBER_AF_PLUGIN_SLOT_MANAGER
}

uint8_t mpsiHandleMessageError(MpsiMessage_t* mpsiMessage)
{
  if (!mpsiMessage) {
    return MPSI_INVALID_PARAMETER;
  }

  // Nothing needs to be done here for the stacks currently
  // Receiving this message is of value to the Mobile App

  return MPSI_SUCCESS;
}

uint8_t mpsiHandleMessageInitiateJoining(MpsiMessage_t* mpsiMessage)
{
  MpsiInitiateJoiningMessage_t message;
  uint8_t bytesDeserialized;

  if (!mpsiMessage) {
    return MPSI_INVALID_PARAMETER;
  }

  if (mpsiMessage->payloadLength != sizeof(message)) {
    return MPSI_INVALID_PARAMETER;
  }

  bytesDeserialized = emAfPluginMpsiDeserializeSpecificMessage(
                                                    mpsiMessage->payload,
                                                    mpsiMessage->messageId,
                                                    &message);

  if (mpsiMessage->payloadLength != bytesDeserialized) {
    mpsiPrintln("MPSI (0x%x) error: deserialize error with len %d (%d)",
                mpsiMessage->messageId, bytesDeserialized,
                mpsiMessage->payloadLength);
    return MPSI_INVALID_PARAMETER;
  }

#if defined (EMBER_STACK_ZIGBEE)
  EmberNodeType nodeType;
  EmberStatus status = emberAfGetNodeType(&nodeType);
  if (EMBER_SUCCESS == status) {
    emberAfPluginNetworkCreatorSecurityOpenNetwork();
  } else if (EMBER_NOT_JOINED == status) {
#ifdef EMBER_AF_API_NETWORK_STEERING
    emberAfPluginNetworkSteeringStart();
#else
    return MPSI_INVALID_FUNCTION;
#endif // EMBER_AF_API_NETWORK_STEERING
  }
#else
  return MPSI_UNSUPPORTED_COMMAND;
#endif

  return MPSI_ERROR;
}

uint8_t mpsiHandleMessageGetZigbeeJoiningDeviceInfo(MpsiMessage_t* mpsiMessage)
{
  if (!mpsiMessage) {
    return MPSI_INVALID_PARAMETER;
  }

  // The BLE side will handle fetching the EUI and Install Code and send it to
  // the Mobile App

  return MPSI_UNSUPPORTED_COMMAND;
}

uint8_t mpsiHandleMessageZigbeeJoiningDeviceInfo(MpsiMessage_t* mpsiMessage)
{
  if (!mpsiMessage) {
    return MPSI_INVALID_PARAMETER;
  }

  // Nothing needs to be done here for the stacks currently
  // Receiving this message is of value to the Mobile App

  return MPSI_SUCCESS;
}

uint8_t mpsiHandleMessageSetZigbeeJoiningDeviceInfo(MpsiMessage_t* mpsiMessage)
{
#if defined (EMBER_STACK_ZIGBEE)
  MpsiZigbeeJoiningDeviceInfoMessage_t message;
  EmberStatus status;
  EmberKeyData key;
  uint8_t bytesDeserialized;

  if (!mpsiMessage) {
    return MPSI_INVALID_PARAMETER;
  }

  bytesDeserialized = emAfPluginMpsiDeserializeSpecificMessage(
                                                    mpsiMessage->payload,
                                                    mpsiMessage->messageId,
                                                    &message);

  if (mpsiMessage->payloadLength != bytesDeserialized) {
    mpsiPrintln("MPSI (0x%x) error: deserialize error with len %d (%d)",
                mpsiMessage->messageId, bytesDeserialized,
                mpsiMessage->payloadLength);
    return MPSI_INVALID_PARAMETER;
  }

  // Convert the install code to a key
  status = emAfInstallCodeToKey(message.installCode,
                                message.installCodeLength,
                                &key);

  if (EMBER_SUCCESS != status) {
    mpsiPrint("MPSI (0x%x) error: ", mpsiMessage->messageId);
    if (EMBER_SECURITY_DATA_INVALID == status) {
      mpsiPrintln("CRC mismatch");
    } else if (EMBER_BAD_ARGUMENT == status) {
      mpsiPrintln("invalid installation code length (%d)",
                  message.installCodeLength);
    } else {
      mpsiPrintln("hash error (0x%x)", status);
    }
    return MPSI_INVALID_PARAMETER;
  }

  // Set the key in the transient key table
  status = addTransientLinkKey(message.eui64, &key);
  if (EMBER_SUCCESS != status) {
    mpsiPrintln("MPSI (0x%x) error: failed to update key table (0x%x)",
                mpsiMessage->messageId, status);
    return MPSI_ERROR;
  }

  return MPSI_SUCCESS;
#else
  return MPSI_UNSUPPORTED_COMMAND;
#endif
}

#if defined (EMBER_AF_PLUGIN_SLOT_MANAGER)
uint16_t getMaxMessageIdSupportedBySlot(uint32_t slotId, uint32_t capabilities)
{
  uint16_t maxMessageIdSupported = 0;

  if (capabilities &
      (1u << APPLICATION_PROPERTIES_CAPABILITIES_MPSI_SUPPORT_BIT)) {
    maxMessageIdSupported = MPSI_INITIAL_MAX_MESSAGE_ID_SUPPORTED;
  }

  // TODO: for future releases, we'll want to call Slot Manager to search the
  // EBLs for a metadata tag that specifies the exact message ID supported
  (void)slotId;

  return maxMessageIdSupported;
}

uint8_t bootloaderAppTypeToMpsiAppType(uint32_t type)
{
  uint8_t mpsiAppId = MPSI_APP_ID_NONE;

  switch (type)
  {
    case APPLICATION_TYPE_ZIGBEE:
      mpsiAppId = MPSI_APP_ID_ZIGBEE;
      break;
    case APPLICATION_TYPE_THREAD:
      mpsiAppId = MPSI_APP_ID_THREAD;
      break;
    case APPLICATION_TYPE_FLEX:
      mpsiAppId = MPSI_APP_ID_CONNECT;
      break;
    case APPLICATION_TYPE_BLUETOOTH:
      mpsiAppId = MPSI_APP_ID_BLE;
      break;
    case APPLICATION_TYPE_MCU:
      mpsiAppId = MPSI_APP_ID_MCU;
      break;
    case 0:
    default:
      break;
  }

  return mpsiAppId;
}
#endif // EMBER_AF_PLUGIN_SLOT_MANAGER
