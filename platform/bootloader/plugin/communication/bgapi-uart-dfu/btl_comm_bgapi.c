/***************************************************************************//**
 * @file btl_comm_bgapi.c
 * @brief Communication plugin implementing the BGAPI UART DFU protocol
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

#include "btl_comm_bgapi.h"

#include "api/btl_interface.h"

#include "core/btl_bootload.h"
#include "core/btl_reset.h"
#include "driver/btl_driver_uart.h"
#include "plugin/debug/btl_debug.h"
#include "plugin/communication/btl_communication.h"

// Parser
#include "plugin/parser/btl_image_parser.h"
#include "plugin/security/btl_security_types.h"

#include <string.h>

// -------------------------------
// Local type declarations

static int32_t sendPacket(BgapiPacket_t *packet)
{
  size_t len = sizeof(BgapiPacketHeader_t) + packet->header.len;

  return uart_sendBuffer((uint8_t *)&(packet->header), len, true);
}

static int32_t receivePacket(BgapiPacket_t *packet)
{
  int32_t ret = BOOTLOADER_OK;
  size_t requestedBytes;
  size_t receivedBytes;
  uint8_t *buf = (uint8_t *)&(packet->header);

  // Wait for a valid packet header
  do {
    // Wait for bytes to be available in RX buffer
    while (uart_getRxAvailableBytes() == 0);
    // TODO: Timeout at some point?

    // Read the first byte
    requestedBytes = 1;
    ret = uart_receiveBuffer(buf,
                             requestedBytes,
                             &receivedBytes,
                             true,
                             1000);
  } while (packet->header.type != BGAPI_PACKET_TYPE_COMMAND);

  // Read the rest of the header
  requestedBytes = sizeof(BgapiPacketHeader_t) - 1;
  ret = uart_receiveBuffer(buf + 1,
                           requestedBytes,
                           &receivedBytes,
                           true,
                           1000);
  // TODO: Handle return value

  if (receivedBytes != requestedBytes) {
    // Didn't receive entire header within 1000 ms; bail
    return BOOTLOADER_ERROR_COMMUNICATION_ERROR;
  }
  BTL_DEBUG_PRINT("BGAPI packet ");
  BTL_DEBUG_PRINT_CHAR_HEX(packet->header.type);
  BTL_DEBUG_PRINT_CHAR_HEX(packet->header.len);
  BTL_DEBUG_PRINT_CHAR_HEX(packet->header.class);
  BTL_DEBUG_PRINT_CHAR_HEX(packet->header.command);
  BTL_DEBUG_PRINT_LF();

  requestedBytes = packet->header.len;
  if (requestedBytes == 0) {
    // Packet has no payload
    return BOOTLOADER_OK;
  }

  ret = uart_receiveBuffer(buf + sizeof(BgapiPacketHeader_t),
                           requestedBytes,
                           &receivedBytes,
                           true,
                           3000);
  // TODO: Handle return value

  if (receivedBytes != requestedBytes) {
    BTL_DEBUG_PRINT("Recvd ");
    BTL_DEBUG_PRINT_WORD_HEX(receivedBytes);
    BTL_DEBUG_PRINT_LF();
    // Didn't receive entire packet within 3000 ms; bail
    return BOOTLOADER_ERROR_COMMUNICATION_ERROR;
  }

  return ret;
}

void communication_init(void)
{
  // Init UART driver
  uart_init();
}

int32_t communication_start(void)
{
  int32_t ret = BOOTLOADER_OK;

  if (reset_getResetReason() == BOOTLOADER_RESET_REASON_BADAPP) {
    // Sent evt_dfu_boot_failure
    BgapiPacket_t failEvent = BGAPI_EVENT_DFU_BOOT_FAILURE(BGAPI_ERROR_SECURITY_IMAGE_CHECKSUM_ERROR);
    ret = sendPacket(&failEvent);
  }
  // Send evt_dfu_boot
  BgapiPacket_t event = BGAPI_EVENT_DFU_BOOT();
  event.body.boot.bootloaderVersion = mainBootloaderTable->header.version;

  ret = sendPacket(&event);
  return ret;
}

int32_t communication_main(void)
{
  int32_t ret = BOOTLOADER_OK;

  BgapiPacket_t command;
  BgapiPacket_t response = BGAPI_RESPONSE_DEFAULT;

  const BootloaderParserCallbacks_t parseCb = {
    .context             = NULL,
    .applicationCallback = bootload_applicationCallback,
    .metadataCallback    = NULL,
    .bootloaderCallback  = bootload_bootloaderCallback
  };

  ImageProperties_t imageProps = {
    .imageContainsBootloader = false,
    .imageContainsApplication = false,
    .imageCompleted = false,
    .imageVerified = false,
    .bootloaderVersion = 0,
    .application = {0}
  };

  ParserContext_t parserContext;
  DecryptContext_t decryptContext;
  AuthContext_t authContext;

  parser_init(&parserContext, &decryptContext, &authContext);

  while (1) {
    // Wait for data
    memset(&command, 0, sizeof(BgapiPacket_t));
    ret = receivePacket(&command);
    if (ret != BOOTLOADER_OK) {
      BTL_DEBUG_PRINT("RX packet fail: ");
      BTL_DEBUG_PRINT_WORD_HEX(ret);
      BTL_DEBUG_PRINT_LF();
      continue;
    }

    if (command.header.class != BGAPI_PACKET_CLASS_DFU) {
      // Do nothing if the packet isn't a DFU packet
      BTL_DEBUG_PRINTLN("Unknown command");
      continue;
    }

    switch(command.header.command) {
      case FLASH_SET_ADDRESS:
        BTL_DEBUG_PRINTLN("Flash set addr");
        // Do nothing -- command is deprecated
        response.header.command = FLASH_SET_ADDRESS;
        response.body.response.error = BGAPI_ERROR_SUCCESS;
        ret = sendPacket(&response);
        break;

      case FLASH_UPLOAD:
        BTL_DEBUG_PRINTLN("Flash upload");
        // Process file contents
        ret = parser_parse(&parserContext,
                           &imageProps,
                           command.body.flashUpload.data,
                           command.body.flashUpload.len,
                           &parseCb);
        response.header.command = FLASH_UPLOAD;

        if (ret == BOOTLOADER_OK || ret == BOOTLOADER_ERROR_PARSER_EOF) {
          response.body.response.error = BGAPI_ERROR_SUCCESS;
        } else {
          response.body.response.error = BGAPI_ERROR_SECURITY_IMAGE_CHECKSUM_ERROR;
        }
        ret = sendPacket(&response);
        break;

      case FLASH_UPLOAD_FINISH:
        BTL_DEBUG_PRINTLN("Flash finish");
        response.header.command = FLASH_UPLOAD_FINISH;
        if (imageProps.imageCompleted && imageProps.imageVerified) {
          response.body.response.error = BGAPI_ERROR_SUCCESS;
        } else {
          response.body.response.error = BGAPI_ERROR_SECURITY_IMAGE_CHECKSUM_ERROR;
        }
        ret = sendPacket(&response);

        if (imageProps.imageCompleted
            && imageProps.imageVerified
            && imageProps.imageContainsBootloader) {
          // Reset to handle bootloader upgrade
          reset_resetWithReason(BOOTLOADER_RESET_REASON_UPGRADE);
        }

        break;

      case RESET:
        BTL_DEBUG_PRINTLN("Reset request");
        if (imageProps.imageCompleted && !imageProps.imageVerified) {
          reset_resetWithReason(BOOTLOADER_RESET_REASON_BADIMAGE);
        } else if (command.body.reset.mode == 1) {
          reset_resetWithReason(BOOTLOADER_RESET_REASON_BOOTLOAD);
        } else {
          reset_resetWithReason(BOOTLOADER_RESET_REASON_GO);
        }
        break;

      default:
        // Do nothing if packet is of an unknown type
        BTL_DEBUG_PRINT("Unknown DFU command: ");
        BTL_DEBUG_PRINT_CHAR_HEX(command.header.command);
        BTL_DEBUG_PRINT_LF();
        break;
    }
  }

  return ret;
}

void communication_shutdown(void)
{
  // Do nothing
}
