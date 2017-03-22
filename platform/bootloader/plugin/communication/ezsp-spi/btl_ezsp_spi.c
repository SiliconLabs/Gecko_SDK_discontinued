/***************************************************************************//**
 * @file btl_ezsp_spi.c
 * @brief EZSP-SPI communication plugin for Silicon Labs Bootloader.
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

#include "em_device.h"
#include "em_gpio.h"

#include "plugin/communication/ezsp-spi/btl_ezsp_spi.h"
#include "driver/btl_driver_delay.h"
#include "driver/btl_driver_spislave.h"

#include "core/btl_bootload.h"

// Parser
#include "plugin/parser/btl_image_parser.h"
#include "plugin/security/btl_security_types.h"

#include "plugin/debug/btl_debug.h"

// ‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
// Local variables
static EzspSpiState_t state;
static EzspSpiTxMessage_t reply;
static EzspSpiTxMessage_t interrupt;
static EzspSpiRxMessage_t rxBuffer;

static bool done;
static bool imageError;
static bool interruptPending;
static bool errorPending;
static bool querySent;

static bool transferActive;
static unsigned int transferPolls;

static const EzspSpiQueryResponse_t queryResponse = {
  .btlCommand = EZSP_SPI_FRAME_BTL_QUERYRESP,
  .btlActive = 0x01,
  .manufacturerId = {0xFF, 0xFF},
  .hardwareTag = {0},
  .btlCapabilities = 0x00,
  // CortexM3 platform
  .platform = 4,
  // EFR32 micro
  .micro = 24,
  // EFR32 PHY
  .phy = 9,
  .btlMajorVersion = BOOTLOADER_VERSION_MAIN_MAJOR,
  .btlMinorVersion = BOOTLOADER_VERSION_MAIN_MINOR
};

static const BootloaderParserCallbacks_t parseCb = {
  .context = NULL,
  .applicationCallback = bootload_applicationCallback,
  // Standalone bootloading doesn't care about metadata
  .metadataCallback = NULL,
  .bootloaderCallback = NULL
};

static ImageProperties_t imageProps = { 0 };

ParserContext_t parserContext;
DecryptContext_t decryptContext;
AuthContext_t authContext;

// ‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
// Local function prototypes
static void ezspSpi_parse(EzspSpiRxMessage_t* buffer);
static uint8_t ezspSpi_parseFrame(EzspSpiRxMessage_t* buffer);
static void ezspSpi_error(uint8_t error);
static void ezspSpi_wakeHandshake(void);

// ‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
// Helper macros
#define nHOST_ASSERT()    GPIO_PinOutClear(BTL_EZSPSPI_HOST_INT_PORT,\
                                           BTL_EZSPSPI_HOST_INT_PIN)
#define nHOST_DEASSERT()  GPIO_PinOutSet(BTL_EZSPSPI_HOST_INT_PORT,\
                                         BTL_EZSPSPI_HOST_INT_PIN)
#define nCS_ACTIVE()     (GPIO_PinInGet(BTL_SPISLAVE_CS_PORT,\
                                        BTL_SPISLAVE_CS_PIN) == 0)

/***************************************************************************//**
 * Initialize hardware for the protocol plugin.
 *
 * @return @ref BOOTLOADER_OK on success, else error code.
 ******************************************************************************/
void communication_init(void)
{
  // Setup EZSP-specific GPIO (nHOST_INT, nWAKE)
  CMU->CTRL        |= CMU_CTRL_HFPERCLKEN;
  CMU->HFBUSCLKEN0 |= CMU_HFBUSCLKEN0_GPIO;

  GPIO_PinModeSet(BTL_EZSPSPI_HOST_INT_PORT,
                  BTL_EZSPSPI_HOST_INT_PIN,
                  gpioModePushPull,
                  1);
  GPIO_PinModeSet(BTL_EZSPSPI_WAKE_INT_PORT,
                  BTL_EZSPSPI_WAKE_INT_PIN,
                  gpioModeInputPull,
                  1);

  state = EZSP_SPI_STATE_INITIAL;
  done = false;
  interruptPending = false;

  // Init SPI slave driver
  spislave_init();
}

/***************************************************************************//**
 * Initialize communication between the bootloader and external host. E.g.
 * indicate to the external host that we're alive and kicking.
 *
 * @return @ref BOOTLOADER_OK on success, else error code
 ******************************************************************************/
int32_t communication_start(void)
{

  // Assert nHOST_INT to signal to the host that we have data to send.
  // But first check for a handshake
  ezspSpi_wakeHandshake();
  nHOST_ASSERT();

  state = EZSP_SPI_STATE_RESETMESSAGE;

  reply.messageBuffer[0] = EZSP_SPI_ERROR_RESET;
  reply.messageBuffer[1] = 0x09U; // TODO: fill out actual reset cause
  reply.messageBuffer[2] = EZSP_SPI_FRAME_TERMINATOR;
  reply.messageLength = 3;

  querySent = false;
  interruptPending = false;
  errorPending = true;
  imageError = false;

  transferActive = false;
  transferPolls = 0;
  xmodem_reset();

  return BOOTLOADER_OK;
}

/***************************************************************************//**
 * This function is not supposed to return until the host signals the end of the
 * current session, or a new image has been flashed and verified.
 *
 * @return @ref BOOTLOADER_OK when a new image was flashed,
           @ref BOOTLOADER_ERROR_COMMUNICATION_ERROR on protocol error,
           @ref BOOTLOADER_ERROR_COMMUNICATION_DONE when no image was received,
           @ref BOOTLOADER_ERROR_COMMUNICATION_IMAGE_ERROR when received image
                is invalid
 ******************************************************************************/
int32_t communication_main(void)
{
  int32_t retVal;
  size_t rxSize;
  bool exit = false;
  bool timeout = false;

  parser_init(&parserContext, &decryptContext, &authContext);

  while (!exit) {
    switch (state) {
      case EZSP_SPI_STATE_INITIAL:
        return BOOTLOADER_ERROR_COMMUNICATION_START;

      case EZSP_SPI_STATE_PROCESSING:
        spislave_enableReceiver(false);
        spislave_flush(false, true);

        if (!errorPending) {
          ezspSpi_parse(&rxBuffer);
        } else {
          delay_microseconds(25);
        }

        state = EZSP_SPI_STATE_ANSWERING;
        break;

      case EZSP_SPI_STATE_ANSWERING:
        retVal = spislave_sendBuffer(reply.messageBuffer,
                                     reply.messageLength);

        if (retVal != BOOTLOADER_OK) {
          // Error condition...
        } else {
          // Assert nHOST_INT to signal TX availability
          nHOST_ASSERT();
          while(spislave_getTxBytesLeft() >= reply.messageLength) {
            //TODO: account for timeout here
          }
          nHOST_DEASSERT();
          errorPending = false;
        }

        while (nCS_ACTIVE()) {
          //TODO: timeout here as well? Or is this considered invalid behavior
          // by the host which is clocking us?
        }

        spislave_enableTransmitter(false);

        if (interruptPending) {
          state = EZSP_SPI_STATE_INTERRUPTING;
        } else {
          exit = done | timeout;
          state = EZSP_SPI_STATE_IDLE;
        }
        break;

      case EZSP_SPI_STATE_RESETMESSAGE:
      case EZSP_SPI_STATE_INTERRUPTING:
        // Wait for SS to deassert
        while (nCS_ACTIVE());

        delay_microseconds(10);
        nHOST_ASSERT();

        state = EZSP_SPI_STATE_IDLE;
        break;

      case EZSP_SPI_STATE_IDLE:
        // Wait for SS to deassert before re-enabling the receiver to avoid
        // garbage in the RX buffer
        while (nCS_ACTIVE());

        // Re-enable the receiver to be ready for incoming commands
        spislave_enableReceiver(true);

        if (transferActive) {
          delay_milliseconds(5000, false);
        } else {
          delay_milliseconds(1000, false);
        }

        while (!nCS_ACTIVE()) {
          // During idle time, watch for WAKE handshake
          ezspSpi_wakeHandshake();
          // Also send XMODEM-CRC polling characters. Every second (up to 60)
          // when transfer hasn't started yet, every 5 seconds (up to 60) when
          // a transfer is active.
          if (delay_expired() && !interruptPending) {
            transferPolls++;
            if (transferActive) {
              if (transferPolls > 12) {
                // Timed out, cancel transfer
                interrupt.messageBuffer[0] = XMODEM_CMD_CAN;
                interrupt.messageLength = 1;
                interruptPending = true;
                timeout = true;
                state = EZSP_SPI_STATE_INTERRUPTING;
                break;
              } else {
                delay_milliseconds(5000, false);
              }
            } else {
              if (transferPolls > 60) {
                // Timed out, cancel transfer
                interrupt.messageBuffer[0] = XMODEM_CMD_CAN;
                interrupt.messageLength = 1;
                interruptPending = true;
                timeout = true;
                state = EZSP_SPI_STATE_INTERRUPTING;
                break;
              } else {
                delay_milliseconds(1000, false);
              }
            }
          }
        }

        if (errorPending) {
          state = EZSP_SPI_STATE_PROCESSING;
          break;
        }

        // Wait for the receiver to send a command
        rxBuffer.data.btlMessage.command = 0xFFU;
        while (rxBuffer.data.btlMessage.command == 0xFFU) {
          spislave_receiveByte(&(rxBuffer.data.btlMessage.command));
        }

        nHOST_DEASSERT();

        // Get length or terminator
        spislave_receiveByte(&(rxBuffer.data.btlMessage.length));

        // Parse command byte
        if (rxBuffer.data.btlMessage.command == EZSP_SPI_COMMAND_BTL_FRAME
            || rxBuffer.data.btlMessage.command == EZSP_SPI_COMMAND_EZSP_FRAME) {
          // Check for oversized packets
          if (rxBuffer.data.btlMessage.length > 133) {
            ezspSpi_error(EZSP_SPI_ERROR_OVERSIZED);
          }

          // Get packet data and terminator
          retVal = spislave_receiveBuffer(rxBuffer.data.btlMessage.payload.buffer,
                                 rxBuffer.data.btlMessage.length + 1,
                                 &rxSize,
                                 true,
                                 500);

          // Check status and terminator
          if (retVal != BOOTLOADER_OK) {
            ezspSpi_error(EZSP_SPI_ERROR_ABORTED);
          } else if (rxBuffer.data.btlMessage.payload.buffer[rxSize-1]
                     != EZSP_SPI_FRAME_TERMINATOR) {
            ezspSpi_error(EZSP_SPI_ERROR_NO_TERMINATOR);
          }
        } else {
          // Check terminator
          if (rxBuffer.data.btlMessage.length != EZSP_SPI_FRAME_TERMINATOR) {
            ezspSpi_error(EZSP_SPI_ERROR_NO_TERMINATOR);
          }
        }

        state = EZSP_SPI_STATE_PROCESSING;
        break;
      default:
        break;
    }
  }

  if (timeout) {
    return BOOTLOADER_ERROR_COMMUNICATION_ERROR;
  }

  if (imageError) {
    return BOOTLOADER_ERROR_COMMUNICATION_IMAGE_ERROR;
  }

  if (done) {
    return BOOTLOADER_OK;
  }

  return BOOTLOADER_ERROR_COMMUNICATION_DONE;
}

/***************************************************************************//**
 * Stop communication between the bootloader and external host.
 *
 * @return @ref BOOTLOADER_OK on success
 ******************************************************************************/
void communication_shutdown(void)
{
  // Reset USART
  spislave_deinit();

  // Reset specific GPIO to Hi-Z
  GPIO_PinModeSet(BTL_EZSPSPI_HOST_INT_PORT,
                  BTL_EZSPSPI_HOST_INT_PIN,
                  gpioModeDisabled,
                  0);
  GPIO_PinModeSet(BTL_EZSPSPI_WAKE_INT_PORT,
                  BTL_EZSPSPI_WAKE_INT_PIN,
                  gpioModeDisabled,
                  0);
}

// ‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
// Local functions
// Parses raw data clocked in by SPI host
void ezspSpi_parse(EzspSpiRxMessage_t* buffer)
{
  switch (buffer->data.btlMessage.command) {
    case EZSP_SPI_COMMAND_SPI_STATUS:
      reply.messageBuffer[0] = EZSP_SPI_RESPONSE_ALIVE;
      reply.messageBuffer[1] = EZSP_SPI_FRAME_TERMINATOR;
      reply.messageLength = 2;
      return;

    case EZSP_SPI_COMMAND_SPI_VERSION:
      reply.messageBuffer[0] = EZSP_SPI_RESPONSE_VERSION;
      reply.messageBuffer[1] = EZSP_SPI_FRAME_TERMINATOR;
      reply.messageLength = 2;
      return;

    case EZSP_SPI_COMMAND_BTL_FRAME:
    case EZSP_SPI_COMMAND_EZSP_FRAME:
      // Parsing logic executes command, and overwrites reply if necessary
      reply.messageBuffer[0] = EZSP_SPI_COMMAND_BTL_FRAME;
      reply.messageBuffer[3] = EZSP_SPI_FRAME_TERMINATOR;
      reply.messageLength = 1;
      reply.messageBuffer[2] = ezspSpi_parseFrame(buffer);
      reply.messageBuffer[1] = reply.messageLength;
      reply.messageLength += 3;
      return;
    default:
      ezspSpi_error(EZSP_SPI_ERROR_UNSUPPORTED);
      return;
  }
}

// Parses the EZSP frame content and populates a reply
// Returns synchronous answer byte
uint8_t ezspSpi_parseFrame(EzspSpiRxMessage_t* buffer)
{
  int32_t retVal;
  uint8_t response;

  if (buffer->data.btlMessage.length == 1
      && buffer->data.btlMessage.payload.buffer[0] == EZSP_SPI_FRAME_BTL_QUERY) {
    // Bootloader query
    if (interruptPending) {
      // Reply with the interrupt data we wanted to get out
      memcpy(&reply.messageBuffer[2],
             interrupt.messageBuffer,
             interrupt.messageLength);
      reply.messageBuffer[2+interrupt.messageLength] = EZSP_SPI_FRAME_TERMINATOR;
      reply.messageLength = interrupt.messageLength;
      interruptPending = false;
      return reply.messageBuffer[2];
    } else {
      // Reply with bootloader data
      memcpy(interrupt.messageBuffer,
             &queryResponse,
             sizeof(EzspSpiQueryResponse_t));
      interrupt.messageLength = sizeof(EzspSpiQueryResponse_t);

      // TODO: overwrite reset cause

      interruptPending = true;
      querySent = true;
      return EZSP_SPI_FRAME_BTL_QUERYFOUND;
    }
  } else {
    // Parse as XMODEM
    if (!querySent) {
      return EZSP_SPI_FRAME_BTL_NO_QUERY;
    }

    if (buffer->data.btlMessage.payload.xmodemPacket.header == XMODEM_CMD_SOH
        && buffer->data.btlMessage.length != (XMODEM_DATA_SIZE + 5)) {
      return EZSP_SPI_FRAME_BTL_BLOCKERR_PARTIAL;
    }

    retVal = xmodem_parsePacket(&(buffer->data.btlMessage.payload.xmodemPacket),
                                &response);

    // Store XMODEM response as an interrupt for later
    interrupt.messageBuffer[0] = response;
    if (response == XMODEM_CMD_ACK) {
      // Add the packet number
      interrupt.messageBuffer[1] = xmodem_getLastPacketNumber();
      interrupt.messageBuffer[2] = 0;
      interrupt.messageLength = 3;
    } else {
      interrupt.messageLength = 1;
    }
    interruptPending = true;

    switch (retVal) {
      case BOOTLOADER_OK:
        transferActive = true;
        transferPolls = 0;
        // Push data through EBL parser and flash
        retVal = parser_parse(&parserContext,
                              &imageProps,
                              buffer->data.btlMessage.payload.xmodemPacket.data,
                              XMODEM_DATA_SIZE,
                              &parseCb);
        if (retVal != BOOTLOADER_OK && !imageProps.imageCompleted) {
          // Error condition! Break off transfer
          interrupt.messageBuffer[0] = XMODEM_CMD_CAN;
          interrupt.messageLength = 1;

          return EZSP_SPI_FRAME_BTL_FILEABORT;
        } else {
          return EZSP_SPI_FRAME_BTL_BLOCKOK;
        }

      case BOOTLOADER_ERROR_XMODEM_CRCL:
        return EZSP_SPI_FRAME_BTL_BLOCKERR_CRCL;

      case BOOTLOADER_ERROR_XMODEM_CRCH:
        return EZSP_SPI_FRAME_BTL_BLOCKERR_CRCH;

      case BOOTLOADER_ERROR_XMODEM_PKTNUM:
        return EZSP_SPI_FRAME_BTL_BLOCKERR_CHK;

      case BOOTLOADER_ERROR_XMODEM_PKTDUP:
        return EZSP_SPI_FRAME_BTL_BLOCKERR_DUP;

      case BOOTLOADER_ERROR_XMODEM_PKTSEQ:
        return EZSP_SPI_FRAME_BTL_BLOCKERR_SEQUENCE;

      case BOOTLOADER_ERROR_XMODEM_DONE:
        done = true;
        // EZSP-SPI expects packet number + 1 on ACK'ing the EOT command
        interrupt.messageBuffer[1] = xmodem_getLastPacketNumber() + 1;
        return EZSP_SPI_FRAME_BTL_FILEDONE;

      case BOOTLOADER_ERROR_XMODEM_CANCEL:
        return EZSP_SPI_FRAME_BTL_BLOCKOK;

      case BOOTLOADER_ERROR_XMODEM_NO_SOH:
        return EZSP_SPI_FRAME_BTL_BLOCKERR_SOH;

      default:
        return EZSP_SPI_FRAME_BTL_FILEABORT;
    }
  }
}

// Updates the reply buffer with the given error message
static void ezspSpi_error(uint8_t error)
{
  if (!errorPending) {
    reply.messageBuffer[0] = error;
    reply.messageBuffer[1] = 0;
    reply.messageBuffer[2] = EZSP_SPI_FRAME_TERMINATOR;
    reply.messageLength = 3;

    errorPending = true;
  }
}

// Checks the WAKE line and performs handshake if needed
static void ezspSpi_wakeHandshake(void)
{
  unsigned int hostIntState = GPIO_PinOutGet(BTL_EZSPSPI_HOST_INT_PORT,
                                             BTL_EZSPSPI_HOST_INT_PIN);

  if (hostIntState == 0) {
    // AN711 10: A wake handshake cannot be performed if nHOST_INT is already
    // asserted.
    return;
  }

  //check for wake up sequence
  if (GPIO_PinInGet(BTL_EZSPSPI_WAKE_INT_PORT, BTL_EZSPSPI_WAKE_INT_PIN) == 0) {
    // This is a handshake. Assert nHOST_INT until WAKE deasserts
    nHOST_ASSERT();
    while (GPIO_PinInGet(BTL_EZSPSPI_WAKE_INT_PORT, BTL_EZSPSPI_WAKE_INT_PIN)
           == 0);
    delay_microseconds(10);
    nHOST_DEASSERT();
    delay_microseconds(25);
  }
}
