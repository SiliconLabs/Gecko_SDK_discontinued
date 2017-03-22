/***************************************************************************//**
 * @file btl_comm_xmodem.c
 * @brief Communication plugin implementing XMODEM over UART
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

#include "btl_comm_xmodem.h"

#include "api/btl_interface.h"

#include "core/btl_bootload.h"
#include "core/btl_reset.h"
#include "driver/btl_driver_delay.h"
#include "driver/btl_driver_uart.h"
#include "plugin/debug/btl_debug.h"
#include "plugin/communication/btl_communication.h"

// Parser
#include "plugin/parser/btl_image_parser.h"
#include "plugin/security/btl_security_types.h"

#include <string.h>

// -------------------------------
// Local type declarations

static const char transferInitStr[] = "\r\nbegin upload\r\n";
static const char transferCompleteStr[] = "\r\nSerial upload complete\r\n";
static const char transferAbortedStr[] = "\r\nSerial upload aborted\r\n";
static const char xmodemError[] = "\r\nXModem block error 0x";
static const char fileError[] = "\r\nfile error 0x";

static int32_t sendPacket(uint8_t packet)
{
  int32_t ret;
  ret = uart_sendByte(packet);

  if (packet == XMODEM_CMD_CAN) {
    // If packet is CAN, send three times
    uart_sendByte(packet);
    uart_sendByte(packet);
  }

  return ret;
}

static int32_t receivePacket(XmodemPacket_t *packet)
{
  int32_t ret = BOOTLOADER_OK;
  size_t requestedBytes;
  size_t receivedBytes;
  uint8_t *buf = (uint8_t *)packet;

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

  if (packet->header != XMODEM_CMD_SOH) {
    // All packets except XMODEM_CMD_SOH are single-byte
    return BOOTLOADER_OK;
  }

  requestedBytes = sizeof(XmodemPacket_t) - 1;
  ret = uart_receiveBuffer(buf + 1,
                           requestedBytes,
                           &receivedBytes,
                           true,
                           3000);

  if (receivedBytes != requestedBytes) {
    BTL_DEBUG_PRINT("Recvd ");
    BTL_DEBUG_PRINT_WORD_HEX(receivedBytes);
    BTL_DEBUG_PRINT_LF();
    // Didn't receive entire packet within 3000 ms; bail
    return BOOTLOADER_ERROR_COMMUNICATION_ERROR;
  }

  return ret;
}

XmodemState_t getAction(void)
{
  uint8_t c;
  uart_receiveByte(&c);


  switch (c) {
    case '1':
      return INIT_TRANSFER;
      break;
    case '2':
      return BOOT;
      break;
    case '3':
      return MENU;
      break;
    default:
      return MENU;
      break;
  }
}

__STATIC_INLINE uint8_t nibbleToHex(uint8_t nibble)
{
  return (nibble > 9) ? (nibble - 10 + 'A') : ( nibble + '0');
}

void communication_init(void)
{
  // Init UART driver
  uart_init();
}

int32_t communication_start(void)
{
  int32_t ret = BOOTLOADER_OK;
  uint32_t version = mainBootloaderTable->header.version;
  char str[] = "\r\nGecko Bootloader vX.Y.A\r\n"
               "1. upload gbl\r\n"
               "2. run\r\n"
               "3. ebl info\r\n"
               "BL > ";

  str[20] = nibbleToHex((version >> 24) & 0x0F);
  str[22] = nibbleToHex((version >> 16) & 0x0F);
  str[24] = nibbleToHex(version & 0x0F);

  uart_sendBuffer((uint8_t *)str, sizeof(str), true);

  return ret;
}

static const BootloaderParserCallbacks_t parseCb = {
  .context             = NULL,
  .applicationCallback = bootload_applicationCallback,
  .metadataCallback    = NULL,
  .bootloaderCallback  = bootload_bootloaderCallback
};

int32_t communication_main(void)
{
  int32_t ret = BOOTLOADER_OK;
  XmodemState_t state = IDLE;
  XmodemReceiveBuffer_t buf;
  uint8_t response;
  int timeout = 60;

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

  delay_init();

  while (1) {

    switch (state) {
      case MENU:
        // Print menu
        communication_start();
        BTL_DEBUG_PRINTLN("Menu");
        state = IDLE;
        break;

      case IDLE:
        // Get user input
        state = getAction();
        timeout = 60;
        break;

      case INIT_TRANSFER:
        uart_sendBuffer((uint8_t *)transferInitStr, sizeof(transferInitStr), true);

        // Initialize EBL parser
        parser_init(&parserContext, &decryptContext, &authContext);
        memset(&imageProps, 0, sizeof(ImageProperties_t));

        // Wait 5ms and see if we got any premature input; discard it
        delay_milliseconds(5, true);
        if (uart_getRxAvailableBytes()) {
          uart_flush(false, true);
        }

        // Initialize XMODEM parser
        xmodem_reset();

        state = WAIT_FOR_DATA;
        break;

      case WAIT_FOR_DATA:
        // Send 'C'
        sendPacket(XMODEM_CMD_C);
        delay_milliseconds(1000, false);
        while (uart_getRxAvailableBytes() == 0 && !delay_expired());

        if (uart_getRxAvailableBytes()) {
          // We got a response; move to receive state
          state = RECEIVE_DATA;
        } else {
          // No response within 1 second; tick towards timeout
          timeout--;
          if (timeout == 0) {
            sendPacket(XMODEM_CMD_CAN);
            state = MENU;
          }
        }
        break;

      case RECEIVE_DATA:
        // Wait for a full XMODEM packet
        memset(&(buf.packet), 0, sizeof(XmodemPacket_t));
        ret = receivePacket(&(buf.packet));

        if (ret != BOOTLOADER_OK) {
          // Receiving packet failed -- timeout
          continue;
        }

        ret = xmodem_parsePacket(&(buf.packet), &response);
        if (ret == BOOTLOADER_ERROR_XMODEM_DONE) {
          // XMODEM receive complete; return to menu
          state = COMPLETE;

          // Send CAN rather than ACK if the image verification failed
          if (!imageProps.imageCompleted || !imageProps.imageVerified) {
            BTL_DEBUG_PRINTLN("Checksum fail");
            response = XMODEM_CMD_CAN;
          }
        }

        if (ret == BOOTLOADER_OK && buf.packet.header == XMODEM_CMD_SOH) {
          // Packet is OK, parse contents
          ret = parser_parse(&parserContext,
                             &imageProps,
                             buf.packet.data,
                             XMODEM_DATA_SIZE,
                             &parseCb);

          if (ret != BOOTLOADER_OK) {
            // Parsing file failed; cancel transfer and return to menu
            BTL_DEBUG_PRINT("Parse fail ");
            BTL_DEBUG_PRINT_WORD_HEX(ret);
            BTL_DEBUG_PRINT_LF();
            response = XMODEM_CMD_CAN;
          }
        }

        if (response == XMODEM_CMD_CAN) {
          // Parsing packet failed; return to main menu
          state = COMPLETE;
        }

        // Send response
        sendPacket(response);
        break;

      case COMPLETE:
        BTL_DEBUG_PRINT("Complete ");
        BTL_DEBUG_PRINT_WORD_HEX(ret);
        BTL_DEBUG_PRINT_LF();
        uart_flush(false,true);

        delay_milliseconds(10, true);

        if (response == XMODEM_CMD_ACK && ret == BOOTLOADER_ERROR_XMODEM_DONE) {
          uart_sendBuffer((uint8_t *)transferCompleteStr,
                          sizeof(transferCompleteStr),
                          true);
        } else {
          uart_sendBuffer((uint8_t *)transferAbortedStr,
                          sizeof(transferAbortedStr),
                          true);

          if (ret >= BOOTLOADER_ERROR_XMODEM_BASE
              && ret < BOOTLOADER_ERROR_PARSER_BASE) {
            uart_sendBuffer((uint8_t *)xmodemError,
                            sizeof(xmodemError),
                            true);
          } else {
            uart_sendBuffer((uint8_t *)fileError,
                            sizeof(fileError),
                            true);
          }

          switch (ret) {
            case BOOTLOADER_ERROR_XMODEM_NO_SOH:
              response = 0x21;
              break;
            case BOOTLOADER_ERROR_XMODEM_PKTNUM:
              response = 0x22;
              break;
            case BOOTLOADER_ERROR_XMODEM_CRCL:
              response = 0x23;
              break;
            case BOOTLOADER_ERROR_XMODEM_CRCH:
              response = 0x24;
              break;
            case BOOTLOADER_ERROR_XMODEM_PKTSEQ:
              response = 0x25;
              break;
            case BOOTLOADER_ERROR_XMODEM_PKTDUP:
              response = 0x27;
              break;

            case BOOTLOADER_ERROR_PARSER_VERSION:
              response = 0x41; // BL_ERR_HEADER_EXP
              break;
            case BOOTLOADER_ERROR_PARSER_CRC:
              response = 0x43; // BL_ERR_CRC
              break;
            case BOOTLOADER_ERROR_PARSER_UNKNOWN_TAG:
              response = 0x44; // BL_ERR_UNKNOWN_TAG
              break;
            case BOOTLOADER_ERROR_PARSER_SIGNATURE:
              response = 0x45; // BL_ERR_SIG
              break;
            case BOOTLOADER_ERROR_PARSER_FILETYPE:
            case BOOTLOADER_ERROR_PARSER_UNEXPECTED:
            case BOOTLOADER_ERROR_PARSER_EOF:
              response = 0x4F; // BL_ERR_TAGBUF
              break;
            case BOOTLOADER_ERROR_PARSER_KEYERROR:
              response = 0x50; // BL_ERR_INV_KEY
              break;
          }

          uart_sendByte(nibbleToHex(response >> 4));
          uart_sendByte(nibbleToHex(response & 0x0F));
          uart_sendByte('\r');
          uart_sendByte('\n');
        }
        state = MENU;
        break;

      case BOOT:
        if (imageProps.imageCompleted && imageProps.imageVerified) {
          if (imageProps.imageContainsBootloader) {
            // Enter first stage bootloader to complete bootloader upgrade
            reset_resetWithReason(BOOTLOADER_RESET_REASON_UPGRADE);
          } else {
            // Enter app
            reset_resetWithReason(BOOTLOADER_RESET_REASON_GO);
          }
        } else {
          // No upgrade image given, or upgrade failed
          reset_resetWithReason(BOOTLOADER_RESET_REASON_BADIMAGE);
        }
        break;
    }
  }

  return ret;
}

void communication_shutdown(void)
{
  // Do nothing
}
