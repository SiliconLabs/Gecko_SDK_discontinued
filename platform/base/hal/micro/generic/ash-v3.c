/*
 * File: ash-v3.c
 * Description: ASHv3 Functionality for realiable serial communication
 *
 * Copyright 2013 by Silicon Laboratories. All rights reserved.             *80*
 */

#include PLATFORM_HEADER
#include <stddef.h>
#include <stdlib.h>
#include "stack/include/ember.h"
#include "stack/include/error.h"

// TODO: this needs to be cleaned up, these headers are not shipped with the
// Connect stack.
#if !defined(EMBER_STACK_CONNECT)
#include "stack/framework/debug.h"
#include "stack/core/ember-stack.h"
#endif

#include "hal/hal.h"
#include "serial/serial.h"

// TODO: this needs to be cleaned up, code in base should not depend on
// application code or at least not enforce a specific directory structure
// at the application. Including "serial-link.h" should be enough.
#if defined(EMBER_STACK_CONNECT)
#include "serial-link.h"
#else
#include "app/ip-ncp/serial-link.h"
#endif

#include "hal/host/crc.h"
#include "hal/micro/uart-link.h"
#include "stack/framework/event-queue.h"
#include "hal/micro/generic/ash-v3.h"

#ifndef APP_SERIAL
  #define APP_SERIAL 1
#endif

#if (defined(EMBER_TEST) || defined(UNIX_HOST) || defined(UNIX_HOST_SIM) || defined(EMBER_ASH_V3_TEST_APP))
  AshRxTestState ashRxTestState = {{0}};
  #define ASH_V3_DEBUG(x) x
  #define ASH_V3_DEBUG_ENABLED 1
#else
  #define ASH_V3_DEBUG(x)
  #undef ASH_V3_DEBUG_ENABLED
#endif

// define LOG_TO_SIM to force ASHv3 debug output via simPrint() instead of
// thru the logging mechanism
//#define LOG_TO_SIM

#ifdef LOG_TO_SIM
  #define OUTPUT(indent, ...)                       \
    printIndent(indent);                            \
    simPrint(__VA_ARGS__)
  #define OUTPUT_BYTES(indent, ...)                   \
    printIndent(indent);                              \
    simPrintBytes(__VA_ARGS__)
#else
  #define OUTPUT(indent, ...)                     \
    printIndent(indent);                          \
    emLog(ASHV3, __VA_ARGS__)
  #define OUTPUT_BYTES(indent, ...)                   \
    printIndent(indent);                              \
    emLogBytes(ASHV3, __VA_ARGS__)
#endif

#if defined(EMBER_NCP_RESET_ENABLE) && !defined(UNIX_HOST_SIM) && defined(UNIX_HOST)
void resetNcp(void);
#endif

static void printIndent(uint8_t indent)
{
  uint8_t i;

  for (i = 0; i < indent; i++) {
    OUTPUT(0, " ");
  }
}

#ifdef ASH_V3_DEBUG_ENABLED
static void reallyPrintAshRxState(uint8_t indent);
static void reallyPrintAshState(uint8_t indent);
static void reallyPrintAshTxState(uint8_t indent);
static void reallyPrintAshPacketInformation(const uint8_t *packet, uint8_t indent);
#endif

static void reallyFlush(bool forceFlush, AshMessageType type);
static void transmitEventHandler(Event *event);
static void emAshDmaBufferAEventHandler(Event *event);
static void emAshDmaBufferBEventHandler(Event *event);
static void transmit(AshMessageType type);
AshTxState ashTxState = {{{0}}};

extern EventQueue emApiAppEventQueue;

#ifdef ASH_V3_DEBUG_ENABLED
static const char * const typeNames[] = {
  "ASH_RESET",
  "ASH_RESET_ACK",
  "ASH_ACK",
  "ASH_NACK"
};
#endif // ASH_V3_DEBUG_ENABLED

//
// <Events>
//

static EventActions transmitEventActions = {
  &emApiAppEventQueue,
  transmitEventHandler,
  NULL, // no marking function is needed
  "transmitting"
};

static Event transmitEvent = {&transmitEventActions, NULL};

static EventActions dmaBufferAEventActions = {
  &emApiAppEventQueue,
  emAshDmaBufferAEventHandler,
  NULL, // no marking function is needed
  "dma buffer A"
};

static Event dmaBufferAEvent = {&dmaBufferAEventActions, NULL};

static EventActions dmaBufferBEventActions = {
  &emApiAppEventQueue,
  emAshDmaBufferBEventHandler,
  NULL, // no marking function is needed
  "dma buffer B"
};

static Event dmaBufferBEvent = {&dmaBufferBEventActions, NULL};

static Event *getEvent(const AshTxDmaBuffer *buffer)
{
  if (buffer == &ashTxState.dmaBufferA) {
    return &dmaBufferAEvent;
  } else if (buffer == &ashTxState.dmaBufferB) {
    return &dmaBufferBEvent;
  }

  return NULL;
}

//
// </Events>
//

// 1100 0000
#define ASH_TYPE_MASK 0xC0

// 0011 1000
#define ASH_OUTGOING_FRAME_COUNTER_MASK 0x38

// 0000 0111
#define ASH_ACK_NACK_FRAME_COUNTER_MASK 0x07

static uint8_t reallyGetOutgoingFrameCounter(uint8_t byte)
{
  return (byte & ASH_OUTGOING_FRAME_COUNTER_MASK) >> 3;
}

static uint8_t reallyGetAckNackFrameCounter(uint8_t byte)
{
  return (byte & ASH_ACK_NACK_FRAME_COUNTER_MASK);
}

uint8_t emAshGetOutgoingFrameCounter(AshTxDmaBuffer *buffer)
{
  return reallyGetOutgoingFrameCounter(buffer->data[ASH_CONTROL_BYTE_INDEX]);
}

uint8_t emAshGetAckNackFrameCounter(AshTxDmaBuffer *buffer)
{
  return reallyGetAckNackFrameCounter(buffer->data[ASH_CONTROL_BYTE_INDEX]);
}

uint8_t emAshTxDmaBufferPayloadLength(const AshTxDmaBuffer *buffer)
{
  if (buffer->finger == NULL) {
    return 0;
  } else {
    if (buffer->state <= ASH_TX_FLUSH) {
      // if we haven't stored a length yet, return how far ahead the finger is
      assert(buffer->finger >= buffer->data + ASH_HEADER_LENGTH);
      return ((uint8_t)(buffer->finger - buffer->data) - ASH_HEADER_LENGTH);
    } else {
      // the packet is en route, and the length is set
      return (buffer->data[ASH_PAYLOAD_LENGTH_INDEX]);
    }
  }
}

// help the debugger
static AshMessageType reallyGetType(uint8_t byte)
{
  return (AshMessageType)((byte & ASH_TYPE_MASK) >> 6);
}

AshMessageType emAshGetType(const AshTxDmaBuffer *buffer)
{
  return reallyGetType(buffer->data[ASH_CONTROL_BYTE_INDEX]);
}

void emAshSetType(AshTxDmaBuffer *buffer, AshMessageType type)
{
  buffer->data[ASH_CONTROL_BYTE_INDEX] &= ~ASH_TYPE_MASK;
  buffer->data[ASH_CONTROL_BYTE_INDEX] |= (type << 6);
}

static uint8_t getOutgoingFrameCounter(const AshTxDmaBuffer *buffer)
{
  return reallyGetOutgoingFrameCounter(buffer->data[ASH_CONTROL_BYTE_INDEX]);
}

static void setOutgoingFrameCounter(AshTxDmaBuffer *buffer, uint8_t value)
{
  buffer->data[ASH_CONTROL_BYTE_INDEX] &= ~ASH_OUTGOING_FRAME_COUNTER_MASK;
  buffer->data[ASH_CONTROL_BYTE_INDEX] |= (value << 3);
}

void emAshSetAckNackFrameCounter(AshTxDmaBuffer *buffer, uint8_t value)
{
  buffer->data[ASH_CONTROL_BYTE_INDEX] &= ~ASH_ACK_NACK_FRAME_COUNTER_MASK;
  buffer->data[ASH_CONTROL_BYTE_INDEX] |= value;
}

#ifdef ASH_V3_DEBUG_ENABLED
static uint8_t theDropPercentage = 0;
static uint8_t theCorruptPercentage = 0;

void emAshConfigUart(uint8_t dropPercentage, uint8_t corruptPercentage)
{
  theDropPercentage = dropPercentage;
  theCorruptPercentage = corruptPercentage;
}

static bool randBoolean(uint8_t percentage)
{
  return ((halCommonGetRandom() % 100) < percentage);
}

static bool shouldDropPacket(void)
{
  // help the debugger
  bool result = randBoolean(theDropPercentage);
  return result;
}

static bool shouldCorruptPacket(void)
{
  // help the debugger
  bool result = randBoolean(theCorruptPercentage);
  return result;
}
#endif

#define getAshAckNackFrame(byte)                \
  (byte & ASH_ACK_NACK_FRAME_MASK)

#define setAshAckNackFrame(byte, value)         \
  (byte |= value)

#define setAshType(byte, value)                 \
  (byte |= (value << 6))

static AshState ashState = ASH_STATE_RESET_TX_PRE;

AshRxState ashRxState = {{0}};

void emEraseAndPrepareDmaBuffer(AshTxDmaBuffer *buffer)
{
  Event *event = getEvent(buffer);

  if (event != NULL) {
    emberEventSetInactive(getEvent(buffer));
  }

  MEMSET(buffer, 0, sizeof(AshTxDmaBuffer));
  buffer->finger = buffer->data + ASH_HEADER_LENGTH;
}

//
// This function sets the dmaBuffer pointer to a most eligible DMA Buffer.
// We want to append data to a DMA Buffer that already has data in it,
// if we can. This function searches for a DMA Buffer that has a state
// (in decreasing preference):
//
// ASH_TX_FLUSH
// ASH_TX_ACTIVE
// ASH_TX_INACTIVE
//
static void nextTx(void)
{
  const uint8_t states[] = {ASH_TX_FLUSH, ASH_TX_ACTIVE, ASH_TX_INACTIVE};
  uint8_t i;
  uint8_t j;

  // start with the dma buffer and move to the other
  AshTxDmaBuffer *buffers[2] = {
    ashTxState.dmaBuffer,
    (ashTxState.dmaBuffer == &ashTxState.dmaBufferA
     ? &ashTxState.dmaBufferB
     : &ashTxState.dmaBufferA)
  };

  for (i = 0; i < COUNTOF(states); i++) {
    for (j = 0; j < COUNTOF(buffers); j++) {
      uint8_t outgoingFrameCounter = getOutgoingFrameCounter(buffers[j]);

      if (buffers[j]->state == states[i]
          && (outgoingFrameCounter == 0
              || (outgoingFrameCounter
                  == ashTxState.outgoingFrameCounter))) {
        ashTxState.dmaBuffer = buffers[j];
        return;
      }
    }
  }
}

// the UART is done sending bits
void emAshNotifyTxComplete(void)
{
  emAshReallyNotifyTxComplete(true);
}

void emAshReallyNotifyTxComplete(bool loadTx)
{
  // start with the dma buffer and move to the other
  AshTxDmaBuffer *buffers[2] = {
    ashTxState.dmaBuffer,
    (ashTxState.dmaBuffer == &ashTxState.dmaBufferA
     ? &ashTxState.dmaBufferB
     : &ashTxState.dmaBufferA)
  };

  uint8_t i;
  bool found = false; // sanity

  // erase empty ACKs
  // there must only be one AshTxDmaBuffer with state == ASH_TX_EN_ROUTE_PRE
  for (i = 0; i < COUNTOF(buffers); i++) {
    if (buffers[i]->state == ASH_TX_EN_ROUTE_PRE) {
      AshMessageType type = emAshGetType(buffers[i]);
      if ((type == ASH_ACK || type == ASH_NACK || type == ASH_RESET_ACK)
          && emAshTxDmaBufferPayloadLength(buffers[i]) == 0) {
        emEraseAndPrepareDmaBuffer(buffers[i]);
      } else {
        assert(! found);
        found = true;
        buffers[i]->state = ASH_TX_EN_ROUTE_POST;
        nextTx();
      }
    } else if (buffers[i]->resend
               && buffers[i]->state == ASH_TX_RESEND_ACKED) {
      // TODO-BUG: This assert is removed to workaround an issue where the
      // global address for the NCP gets reset. See: EMIPSTACK-485
      //assert(! found);
      found = true;
      emEraseAndPrepareDmaBuffer(buffers[i]);
    }
  }

  if (ashState == ASH_STATE_RESET_TX_PRE) {
    ashState = ASH_STATE_RESET_TX_POST;
    emberAshStatusHandler(ashState);
  }

  if (loadTx) {
    emLoadSerialTx();
    uartFlushTx();
  }
}

bool emAshByteShouldBeEscaped(uint8_t byte)
{
  return (byte == ASH_FLAG
          || byte == ASH_ESC
          || byte == ASH_XON
          || byte == ASH_XOFF
          || byte == ASH_COBRA_FORCE_BOOT);
}

//
// Steps to expand the 2-byte CRC, A, into 3 bytes, B.
//
// 1) Copy A[1, 4] to B[3, 7]
// 2) Set A[1, 4] to 0
// 3) Copy A1 to B1
// 4) Copy A[2, 4] to B[3, 6]
// 5) Set A[2, 4] to 0
// 6) Copy A2 to B2
//
// +-+-+-+-+-+-+-+-+|+-+-+-+-+-+-+-+-+
// +       A1       +       A2       +
// +-+-+-+-+-+-+-+-+|+-+-+-+-+-+-+-+-+
//  7 6 5 4 3 2 1 0   7 6 5 4 3 2 1 0
//        ^                 ^-------------+
//        +-----------------------------+ |
// +-+-+-+-+-+-+-+-+|+-+-+-+-+-+-+-+-+|+-+-+-+-+-+-+-+-+
// +       B1       |       B2        |       B3       +
// +-+-+-+-+-+-+-+-+|+-+-+-+-+-+-+-+-+|+-+-+-+-+-+-+-+-+
//                                      7 6 5 4 3 2 1 0
//
static uint32_t expandCrc(uint16_t crc)
{
  // the most significant byte of result is 0
  uint32_t result = ((uint32_t)crc << 8);

  // clear bits 12 and 20
  result &= ~(1UL << 20);
  result &= ~(1UL << 12);

  // swap original bit 12 (C) with new bit 7
  if ((crc & (1 << 12)) != 0) {
    result |= (1 << 7);
  }

  // swap original bit 4 with new bit 6
  if ((crc & (1 << 4)) != 0) {
    result |= (1 << 6);
  }

  return result;
}

// debugging hook as well
uint32_t emGetAshCrc(const uint8_t *data, uint16_t length)
{
  uint16_t i;
  uint16_t crc = 0;

  for (i = 0; i < length; i++) {
    crc = halCommonCrc16(data[i], crc);
  }

  return expandCrc(crc);
}

#define escapeThisByte(byte)                    \
  (*byte ^= ASH_ESCAPE_BYTE)

#define setHeaderEscapeByteValue(target, value)     \
  target->data[ASH_HEADER_ESCAPE_BYTE_INDEX] |= value

uint16_t emCreateAshHeader(AshTxDmaBuffer *target,
                         AshMessageType type,
                         uint8_t outgoingFrameCounter,
                         uint8_t ackNackFrameCounter,
                         uint8_t payloadLength)
{
  target->data[ASH_FLAG_INDEX] = ASH_FLAG;
  target->data[ASH_HEADER_ESCAPE_BYTE_INDEX] = 0;
  emAshSetType(target, type);

  if (outgoingFrameCounter != 0xFF) {
    setOutgoingFrameCounter(target, outgoingFrameCounter);
  }

  emAshSetAckNackFrameCounter(target, ackNackFrameCounter);

  if (emAshByteShouldBeEscaped(target->data[ASH_CONTROL_BYTE_INDEX])) {
    escapeThisByte(&target->data[ASH_CONTROL_BYTE_INDEX]);
    setHeaderEscapeByteValue(target, ASH_CONTROL_BYTE_ESCAPED);
  }

  uint8_t packetSize = payloadLength + ASH_BIG_HEADER_LENGTH;
  assert(packetSize <= MAX_ASH_PACKET_SIZE);
  target->data[ASH_PAYLOAD_LENGTH_INDEX] = payloadLength;

  if (emAshByteShouldBeEscaped(target->data[ASH_PAYLOAD_LENGTH_INDEX])) {
    escapeThisByte(&target->data[ASH_PAYLOAD_LENGTH_INDEX]);
    setHeaderEscapeByteValue(target, ASH_PAYLOAD_LENGTH_BYTE_ESCAPED);
  }

  uint32_t crc = emGetAshCrc(target->data, payloadLength + ASH_HEADER_LENGTH);

  // store the three bytes of the crc
  uint8_t *crcLocation = target->data + ASH_HEADER_LENGTH + payloadLength;
  crcLocation[0] = ((crc & 0xFF0000) >> 16);
  crcLocation[1] = ((crc & 0x00FF00) >> 8);
  crcLocation[2] = (crc & 0x0000FF);
  target->finger += 3;

  return packetSize;
}

//
// Resend a non-ACKed packet
// This function is called from the event system
//
static void resend(AshTxDmaBuffer *buffer)
{
  emApiCounterHandler(EMBER_ASH_V3_RESEND, 1);
  buffer->resend = true;
  buffer->state = ASH_TX_FLUSH;
  ashTxState.dmaBuffer = buffer;
  buffer->resendCount++;

  if (ashState == ASH_STATE_RUNNING
      && buffer->resendCount >= MAX_ASH_RESEND_COUNT) {
    uartLinkReset();

#if defined(EMBER_NCP_RESET_ENABLE) && !defined(UNIX_HOST_SIM) && defined(UNIX_HOST)
    resetNcp();
#endif
  } else {
    emberEventSetInactive(getEvent(buffer));
    ASH_V3_DEBUG(OUTPUT(0,
                        "[[%u ASH TX retry buffer %s]]\n",
                        halCommonGetInt32uMillisecondTick(),
                        (ashTxState.dmaBuffer == &ashTxState.dmaBufferA
                         ? "A"
                         : "B")));
    reallyFlush(true, LAST_ASH_MESSAGE_TYPE);
  }
}

#define WINDOW_SIZE 2

static bool frameCounterGreaterThan(uint8_t left, uint8_t right)
{
  return ((left < ASH_FRAME_COUNTER_ROLLOVER
           && left == right + 1)
          || (left == 1
              && right == ASH_FRAME_COUNTER_ROLLOVER - 1));
}

static bool frameCounterGreaterThanOrEqual(uint8_t left, uint8_t right)
{
  return (left == right
          || frameCounterGreaterThan(left, right));
}

static void resendEventHelper(AshTxDmaBuffer *buffer)
{
  if (buffer->state != ASH_TX_EN_ROUTE_PRE) {
    resend(buffer);
  } else {
    // the UART hasn't finished sending this buffer, try again
    emberEventSetDelayMs(getEvent(buffer), ASH_ACK_TIMEOUT);
  }
}

static bool isWithinWindow(uint8_t frameCounter)
{
  return (frameCounter == 0
          || ashTxState.ackNackFrameCounter == frameCounter
          || (ashTxState.ackNackFrameCounter == 1 && frameCounter == 7)
          || (frameCounter == ashTxState.ackNackFrameCounter - 1));
}

static bool isNextFrameCounter(uint8_t frameCounter)
{
  return (ashTxState.ackNackFrameCounter < ASH_FRAME_COUNTER_ROLLOVER - 1
          ? (frameCounter == ((ashTxState.ackNackFrameCounter + 1)
                              % ASH_FRAME_COUNTER_ROLLOVER))
          : frameCounter == 1);
}

void emAshDmaBufferAEventHandler(Event *event)
{
  resendEventHelper(&ashTxState.dmaBufferA);
}

void emAshDmaBufferBEventHandler(Event *event)
{
  resendEventHelper(&ashTxState.dmaBufferB);
}

void emPrintBytes(const uint8_t *bytes, uint16_t length, uint8_t indent)
{
  printIndent(indent);
  uint16_t i;

  for (i = 0; i < length; i++) {
    OUTPUT(0, "%X ", bytes[i]);
  }
}

extern const char * emAppName;

static void logTx(const char *string,
                  bool corrupted,
                  const uint8_t *packet,
                  const uint8_t length,
                  bool printBytes)
{
  ASH_V3_DEBUG
    (AshMessageType type =
     reallyGetType(packet[ASH_CONTROL_BYTE_INDEX]);
     OUTPUT(0,
            "[[%s %u ASH TX %s [%u/%u] ** %s %s ** %u ",
            emAppName,
            halCommonGetInt32uMillisecondTick(),
            (type < LAST_ASH_MESSAGE_TYPE
             ? typeNames[type]
             : "OUT OF BOUNDS"),
            reallyGetOutgoingFrameCounter(packet[ASH_CONTROL_BYTE_INDEX]),
            reallyGetAckNackFrameCounter(packet[ASH_CONTROL_BYTE_INDEX]),
            string,
            (corrupted
             ? "^^ CORRUPTED ^^"
             : ""),
            length);

     if (printBytes) {
       emLogBytes(ASHV3, "bytes: ", packet, length);
     }

     OUTPUT(0, " /ASH TX]]\n"););
}

static void uartLinkTx(AshMessageType type,
                       uint8_t *packet,
                       uint8_t length,
                       bool corrupted)
{
  if (halHostUartTxIdle()
      && ashTxState.dmaBufferA.state != ASH_TX_EN_ROUTE_PRE
      && ashTxState.dmaBufferB.state != ASH_TX_EN_ROUTE_PRE) {
    logTx("Success", corrupted, packet, length, true);
    ashTxState.dmaBuffer->state = ASH_TX_EN_ROUTE_PRE;
    halHostUartLinkTx(packet, length);
    emApiCounterHandler(EMBER_ASH_V3_BYTES_SENT, length);

    if (type == ASH_NACK) {
      emApiCounterHandler(EMBER_ASH_V3_NACK_SENT, 1);
    } else if (type == ASH_ACK) {
      emApiCounterHandler(EMBER_ASH_V3_ACK_SENT, 1);
    }
  } else {
    // let the retry system perform a resend
    ASH_V3_DEBUG(emLogLine(ASHV3,
                           "%u %u %u",
                           halHostUartTxIdle(),
                           ashTxState.dmaBufferA.state != ASH_TX_EN_ROUTE_PRE,
                           ashTxState.dmaBufferB.state != ASH_TX_EN_ROUTE_PRE));
    logTx("UART BUSY", corrupted, packet, length, true);
  }
}

#ifdef ASH_V3_DEBUG_ENABLED
static uint8_t corruptedData[MAX_ASH_PACKET_SIZE] = {0};

static void transmitCorruptedPacket(AshMessageType type,
                                    AshTxDmaBuffer *buffer,
                                    uint16_t length)
{
  buffer->isCorrupt = true;
  const uint8_t *data = buffer->data;
  MEMCOPY(corruptedData, data, length);
  int index = halCommonGetRandom() % length;
  corruptedData[index] = rand();
  uartLinkTx(type, corruptedData, length, true);
}
#endif

static uint8_t getControlByte(const uint8_t *data)
{
  uint8_t controlByte = data[ASH_CONTROL_BYTE_INDEX];

  if ((data[ASH_HEADER_ESCAPE_BYTE_INDEX]
       & ASH_CONTROL_BYTE_ESCAPED)
      != 0) {
    escapeThisByte(&controlByte);
  }

  return controlByte;
}

//
// The event handler that is called to transmit a TX DMA buffer.
// This event handler may be called directly during normal transmission,
// or via the event system for retries.
//
static void transmitEventHandler(Event *event)
{
  transmit(LAST_ASH_MESSAGE_TYPE);
}

static void transmit(AshMessageType txType)
{
  emberEventSetInactive(&transmitEvent);

  if (ashTxState.dmaBuffer->state == ASH_TX_FLUSH) {
    ashTxState.serialLayerReplied = true;

    AshTxDmaBuffer *target = ashTxState.dmaBuffer;
    AshMessageType packetType = emAshGetType(target);

    // don't update by default
    uint8_t outgoingFrameCounter = 0xFF;

    if (ashTxState.dmaBuffer->resend) {
      // txType of LAST_ASH_MESSAGE_TYPE means don't update
      // don't change a RESET to anything else
      packetType = ((txType == LAST_ASH_MESSAGE_TYPE
                     || packetType == ASH_RESET)
                    ? packetType
                    : txType);
    } else {
      packetType = txType;

      // this packet hasn't been sent yet
      // only increment the outgoing frame counter if we have a payload,
      // or if the packet is an ASH_RESET
      if (emAshTxDmaBufferPayloadLength(target) > 0
          || packetType == ASH_RESET) {
        // max at 7, start at 1 (0 is for reset)
        ashTxState.outgoingFrameCounter =
          (ashTxState.outgoingFrameCounter < ASH_FRAME_COUNTER_ROLLOVER - 1
           ? ashTxState.outgoingFrameCounter + 1
           : 1);
      }

      outgoingFrameCounter = ashTxState.outgoingFrameCounter;
    }

    uint8_t length = emAshTxDmaBufferPayloadLength(target);
    assert(target->finger < target->data + MAX_ASH_PACKET_SIZE);

    // create the header
    emCreateAshHeader(target,
                      packetType,
                      outgoingFrameCounter,
                      ashTxState.ackNackFrameCounter,
                      length);
    emberEventSetDelayMs(getEvent(ashTxState.dmaBuffer), ASH_ACK_TIMEOUT);
    uint16_t packetLength = target->finger - target->data;

    // dropping and corruption are performed for testing only
#ifdef ASH_V3_DEBUG_ENABLED
    if (shouldDropPacket()) {
      logTx("DROP", false, target->data, packetLength, true);
      ashTxState.dmaBuffer->state = ASH_TX_EN_ROUTE_POST;
      emApiCounterHandler(EMBER_ASH_V3_BYTES_SENT, packetLength);
    } else if (shouldCorruptPacket()) {
      transmitCorruptedPacket(packetType, target, packetLength);
    } else {
#endif
      target->isCorrupt = false;
      uartLinkTx(packetType, target->data, packetLength, false);
#ifdef ASH_V3_DEBUG_ENABLED
    }
#endif

    // prepare for resend, back the finger up before the CRC
    if (target->state != ASH_TX_INACTIVE) {
      target->finger -= ASH_CRC_SIZE;
      target->resend = true;
    }

    // move to the next DMA buffer, maybe
    nextTx();
  }
}

void emAshHandleAck(uint8_t ackNackFrameCounter)
{
  AshTxDmaBuffer *buffers[2] = {&ashTxState.dmaBufferA, &ashTxState.dmaBufferB};
  emApiCounterHandler(EMBER_ASH_V3_ACK_RECEIVED, 1);

  if (ackNackFrameCounter > 0) {
    bool found = false;
    uint8_t i;

    for (i = 0; i < COUNTOF(buffers); i++) {
      AshTxDmaBuffer *buffer = buffers[i];

      if (buffer->resend
          && buffer->state == ASH_TX_EN_ROUTE_PRE
          && (frameCounterGreaterThanOrEqual
              (ackNackFrameCounter,
               getOutgoingFrameCounter(buffer)))) {
        // this packet is acked during a resend
        // don't erase the DMA buffer now -- the UART may be busy sending it
        // rather set the buffer's state to ASH_TX_RESEND_ACKED so that the
        // buffer can be reclaimed safely later
        buffer->state = ASH_TX_RESEND_ACKED;
      } else if ((buffer->state == ASH_TX_EN_ROUTE_POST
                  || buffer->resend)
                 && (frameCounterGreaterThanOrEqual
                     (ackNackFrameCounter,
                      getOutgoingFrameCounter(buffer)))) {
        // this packet has been ACK'd -- erase it, we're done with it
        emApiCounterHandler(EMBER_ASH_V3_PAYLOAD_BYTES_SENT,
                            buffer->data[ASH_PAYLOAD_LENGTH_INDEX]);
        emEraseAndPrepareDmaBuffer(buffers[i]);
        found = true;
      }
    }

    if (found) {
      nextTx();
    }
  }
}

void emAshHandleNack(uint8_t ackNackFrameCounter)
{
  uint8_t i;
  AshTxDmaBuffer *buffers[2] = {&ashTxState.dmaBufferA, &ashTxState.dmaBufferB};
  emApiCounterHandler(EMBER_ASH_V3_NACK_RECEIVED, 1);

  // resend all buffers that have OFC greater than ackNackFrameCounter
  for (i = 0; i < COUNTOF(buffers); i++) {
    AshTxDmaBuffer *buffer = buffers[i];

    if (buffer->state == ASH_TX_EN_ROUTE_POST
        && emAshGetType(buffer) != ASH_RESET // don't nack a RESET
        && (ackNackFrameCounter == 0
            || frameCounterGreaterThan(getOutgoingFrameCounter(buffer),
                                       ackNackFrameCounter))) {
      // set the state to active so retry will occur later
      emberEventSetActive(getEvent(buffer));
    }
  }
}

uint16_t halHostReallyEnqueueTx(const uint8_t *data,
                              uint16_t dataLength,
                              AshTxDmaBuffer *target,
                              bool forceFlush)
{
  uint16_t bytesStored = 0;

  // we can add more data to a packet with state ASH_TX_FLUSH
  if (target->state <= ASH_TX_FLUSH && ! target->resend) {
    if (target->state < ASH_TX_FLUSH) {
      target->state = ASH_TX_ACTIVE;
    }

    // leave 2 bytes for the CRC-16
    const uint8_t *top = target->data + MAX_ASH_PACKET_SIZE - ASH_CRC_SIZE;
    uint16_t i;

    for (i = 0;
         i < dataLength && target->finger < top;
         i++) {
      uint8_t byte = data[i];

      // test for escape
      if (emAshByteShouldBeEscaped(byte)) {
        if (top - target->finger >= 2) {
          *target->finger++ = ASH_ESC;
          // store the escaped version of byte
          *target->finger++ = (byte ^ ASH_ESCAPE_BYTE);
        } else {
          // don't place ASH_ESC escape in the last byte of a packet
          // we're done
          target->state = ASH_TX_FLUSH;
          break;
        }
      } else {
        *target->finger++ = byte;
      }

      bytesStored += 1;
    }

    if (target->finger == top
        || forceFlush) {
      target->state = ASH_TX_FLUSH;
    }

    assert(target->finger
           <= (target->data + MAX_ASH_PACKET_SIZE) - sizeof(uint16_t));
  }

  return bytesStored;
}

uint16_t halHostEnqueueTx(const uint8_t *data, uint16_t dataLength)
{
  uint16_t result = 0;

  // only allow TX in the running state
  // if our state is not ASH_STATE_RUNNING, then we must wait for a reset ACK
  // before we proceed
  // also wait for our AFC to be != 0, which indicates the other side is
  // initialized
  if (ashState == ASH_STATE_RUNNING
      && ashTxState.ackNackFrameCounter != 0) {
    nextTx();
    result = halHostReallyEnqueueTx(data,
                                    dataLength,
                                    ashTxState.dmaBuffer,
                                    false);
    transmit(ASH_ACK);
  }

  return result;
}

static void reallyFlush(bool forceFlush, AshMessageType type)
{
  AshTxDmaBufferState state = ashTxState.dmaBuffer->state;

  // transmit if there is a payload,
  // or if the type is RESET (it has no payload),
  // or if the type is RESET_ACK or NACK (may or may not have a payload)
  //
  // if the packet has state ASH_TX_EN_ROUTE_PRE, or ASH_TX_EN_ROUTE_POST,
  // then it has already been handed to the UART
  //
  if ((forceFlush
       && state != ASH_TX_EN_ROUTE_PRE
       && state != ASH_TX_EN_ROUTE_POST)
      || ((state == ASH_TX_ACTIVE
           || state == ASH_TX_FLUSH)
          && (type == ASH_RESET // no payload
              || type == ASH_RESET_ACK // can send with 0 payload length
              || type == ASH_NACK // can send with 0 payload length
              || (ashTxState.dmaBuffer->finger - ashTxState.dmaBuffer->data
                  > ASH_HEADER_LENGTH)))) {
    ashTxState.dmaBuffer->state = ASH_TX_FLUSH;
    transmit(type);
  }
}

void uartFlushTx(void)
{
  reallyFlush(false, ASH_ACK);
}

static void maybeUnescapeHeaderByte(uint8_t *byteLocation, AshHeaderEscapeType value)
{
  if ((ashRxState.headerEscapeByte & value) != 0) {
    escapeThisByte(byteLocation);
  }
}

// help the debugger
static void nackHelper(AshMessageType *txType)
{
  *txType = ASH_NACK;
}

static void addRawByte(uint8_t byte)
{
  // for debugging, the actual received data including the ASH frame
  ASH_V3_DEBUG(ashRxTestState.rawData[ashRxTestState.rawDataIndex] = byte;
               ashRxTestState.rawDataIndex++);
}

//
// Parse bytes and run the ASHv3 RX State machine
//
static uint8_t runRxStateMachine(const uint8_t *data,
                               uint8_t dataLength,
                               AshMessageType *txType)
{
  uint8_t i;

  for (i = 0;
       i < dataLength
         && ashRxState.frameState != ASH_DONE
         && *txType != ASH_NACK;
       i++) {
    uint8_t byte = data[i];
    bool carryOn = true;

    // ignore ASH_WAKEUP
    if (ashRxState.frameState == ASH_INACTIVE
        && byte == ASH_WAKEUP) {
      continue;
    }

    // let applications that support XON or XOFF know when they come in
#ifdef EMBER_APPLICATION_USES_SOFTWARE_FLOW_CONTROL
    if (byte == ASH_XON) {
      emberXOnHandler();
      continue;
    } else if (byte == ASH_XOFF) {
      emberXOffHandler();
      continue;
    }
#endif // EMBER_APPLICATION_USES_SOFTWARE_FLOW_CONTROL

    // if we receive anything but an ASH_FLAG to start a packet, we send a NACK
    // be greedy and eat as much junk as possible
    if (ashRxState.frameState == ASH_INACTIVE
        && byte != ASH_FLAG) {
      ashRxState.frameState = ASH_DONE;
      nackHelper(txType);

      while (i < dataLength && data[i] != ASH_FLAG) {
        i++;
      }

      return i;
    }

    if (ashRxState.frameState != ASH_INACTIVE
        && byte == ASH_FLAG) {
      // we've encountered a chomped packet, send a NACK
      ashRxState.frameState = ASH_DONE;
      nackHelper(txType);
      return i;
    }

    if (byte == ASH_FLAG) {
      // start anew
      emResetAshRxState();
      ASH_V3_DEBUG(ashRxTestState.rawDataStartIndex = ashRxTestState.rawDataIndex);
    }

    addRawByte(byte);

    // don't include the CRC bytes themselves in the CRC
    if (ashRxState.frameState < ASH_NEED_HIGH_CRC) {
      ashRxState.computedCrc = halCommonCrc16(byte, ashRxState.computedCrc);
    }

    //
    // the regular processing state
    //
    switch (ashRxState.frameState) {
    case ASH_INACTIVE:
      // do nothing, state is updated below
      break;
    case ASH_NEED_HEADER_ESCAPE_BYTE:
      ashRxState.headerEscapeByte = byte;
      break;
    case ASH_NEED_CONTROL_BYTE: {
      maybeUnescapeHeaderByte(&byte, ASH_CONTROL_BYTE_ESCAPED);
      ashRxState.controlByte = byte;
      break;
    }
    case ASH_NEED_PAYLOAD_LENGTH:
      maybeUnescapeHeaderByte(&byte, ASH_PAYLOAD_LENGTH_BYTE_ESCAPED);
      ashRxState.payloadLength = byte;

      if (ashRxState.payloadLength > MAX_ASH_PAYLOAD_SIZE) {
        // we've received a corrupted packet
        nackHelper(txType);
      }
      break;
    case ASH_NEED_PAYLOAD: {
      carryOn = false;

      if (ashRxState.escapeNextByte) {
        ashRxState.payload[ashRxState.payloadIndex] = (byte ^ ASH_ESCAPE_BYTE);
        ashRxState.payloadIndex++;
        ashRxState.escapeNextByte = false;
      } else if (ashRxState.escapedPayloadIndex < ashRxState.payloadLength) {
        if (byte == ASH_ESC) {
          ashRxState.escapeNextByte = true;

          // an escape byte can't appear at the end of an ASH packet
          if (ashRxState.payloadIndex > ashRxState.payloadLength - 1) {
            nackHelper(txType);
          }
        } else {
          ashRxState.payload[ashRxState.payloadIndex] = byte;
          ashRxState.payloadIndex++;
        }
      }

      if (*txType != ASH_NACK) {
        ashRxState.escapedPayloadIndex++;

        if (ashRxState.escapedPayloadIndex >= ashRxState.payloadLength) {
          // we're done with the data, move on!
          carryOn = true;
        }
      }

      break;
    }
    case ASH_NEED_HIGH_CRC: {
      ashRxState.highCrcByte = byte;
      break;
    }
    case ASH_NEED_IN_BETWEEN_CRC: {
      ashRxState.inBetweenCrcByte = byte;
      break;
    }
    case ASH_NEED_LOW_CRC: {
      uint32_t expandedComputedCrc = expandCrc(ashRxState.computedCrc);
      uint32_t theCrc = ((uint32_t)((uint32_t)ashRxState.highCrcByte << 16)
                       | (uint32_t)((uint32_t)ashRxState.inBetweenCrcByte << 8)
                       | (uint32_t)byte);

      // we're done, now check the two CRCs
      if (theCrc == expandedComputedCrc) {
        *txType = ASH_ACK;
      } else {
        nackHelper(txType);
      }

      break;
    }
    case ASH_DONE:
    default:
      // should not get here
      assert(false);
    }

    if (*txType == ASH_NACK) {
      ashRxState.frameState = ASH_DONE;
    } else if (carryOn) {
      ashRxState.frameState++;

      if (ashRxState.frameState == ASH_NEED_PAYLOAD
          && ashRxState.payloadLength == 0) {
        // there's no data, move on
        ashRxState.frameState++;
      }
    }
  }

  return i;
}

//
// we've done receiving a packet, now handle it
//
static void doPostRxStateMachineTasks(AshMessageType txType,
                                      SerialRxHandler serialRxHandler)
{
  uint8_t outgoingFrameCounter =
    reallyGetOutgoingFrameCounter(ashRxState.controlByte);

  ASH_V3_DEBUG(ashRxTestState.receiveCount++);

  emApiCounterHandler(EMBER_ASH_V3_VALID_BYTES_RECEIVED,
                      ashRxState.payloadIndex + ASH_BIG_HEADER_LENGTH);
  AshMessageType rxType = reallyGetType(ashRxState.controlByte);
  bool forceFlush = false;
  bool nowRunning = false;

  if (txType == ASH_NACK
      // we must NACK a RESET packet with non-empty payload
      || (rxType == ASH_RESET
          && ashRxState.payloadLength > 0)) {
    nackHelper(&txType);
    forceFlush = true;
  } else if (rxType == ASH_RESET) {
    // the other node has sent a reset, we will respond with a RESET_ACK
    assert(ashRxState.payloadLength == 0);

    // the RESET must have OFC 1
    if (outgoingFrameCounter != 1) {
      nackHelper(&txType);
    } else {
      AshTxDmaBuffer *buffers[2] = {&ashTxState.dmaBufferA,
                                    &ashTxState.dmaBufferB};
      uint8_t i;

      // clear the TX state, except for a RESET packet
      if (ashTxState.ackNackFrameCounter != 0) {
        for (i = 0; i < COUNTOF(buffers); i++) {
          if (emAshGetType(buffers[i]) != ASH_RESET) {
            emEraseAndPrepareDmaBuffer(buffers[i]);
          }
        }
      }

      ashTxState.ackNackFrameCounter = outgoingFrameCounter;
      txType = ASH_RESET_ACK;
    }

    // let the upper layer know that we've received a RESET
    emResetSerialState(true);
    forceFlush = true;
  } else if (rxType == ASH_RESET_ACK) {
    //
    // the other side has sent a RESET_ACK
    //
    ashTxState.ackNackFrameCounter = outgoingFrameCounter;

    if (ashState == ASH_STATE_RESET_TX_POST) {
      ashState = ASH_STATE_RUNNING;
      nowRunning = true;
      emAshHandleAck(reallyGetAckNackFrameCounter(ashRxState.controlByte));
    }
  } else if (ashState == ASH_STATE_RUNNING
             && (rxType == ASH_ACK
                 || rxType == ASH_NACK)) {
    //
    // the other node has sent a message
    //
    if (rxType == ASH_ACK) {
      emAshHandleAck(reallyGetAckNackFrameCounter(ashRxState.controlByte));
    } else {
      emAshHandleNack(reallyGetAckNackFrameCounter(ashRxState.controlByte));
    }

    // if the packet has a payload, then we will ACK it below
  } else if (ashState == ASH_STATE_RUNNING) {
    // send a NACK, we're truly confused
    nackHelper(&txType);
    forceFlush = true;
  }

  ashTxState.serialLayerReplied = false;

  if ((txType == ASH_ACK
       || txType == ASH_RESET_ACK)
      && ashState == ASH_STATE_RUNNING
      && ashRxState.payloadIndex > 0) {
    if ((ashTxState.ackNackFrameCounter == 0 // uninitialized
         || isNextFrameCounter(outgoingFrameCounter))) { // the next
      // The uart link may be reset during the call to serialCopyFromRx,
      // so save our payloadIndex before the call
      uint8_t payloadIndex = ashRxState.payloadIndex;
      uint8_t oldAckNackFrameCounter = ashTxState.ackNackFrameCounter;
      ashTxState.ackNackFrameCounter = outgoingFrameCounter;
      assert(ashTxState.ackNackFrameCounter < ASH_FRAME_COUNTER_ROLLOVER);

      if (serialRxHandler(ashRxState.payload, ashRxState.payloadIndex)
          != payloadIndex) {
        // the upper layer couldn't process all the given bytes,
        // we need to try again
        ashTxState.ackNackFrameCounter = oldAckNackFrameCounter;
        nackHelper(&txType);
      }
    } else if (isWithinWindow(outgoingFrameCounter)) {
      txType = ASH_ACK;
    } else {
      nackHelper(&txType);
    }

    // we want to send a response even if the data wasn't passed to the upper layer
    forceFlush = true;
  }

  emLoadSerialTx();

  if (ashTxState.serialLayerReplied) {
    forceFlush = false;
  }

  reallyFlush(forceFlush, txType);
  ashTxState.serialLayerReplied = false;

  if (nowRunning) {
    emberAshStatusHandler(ASH_STATE_RUNNING);
  }
}

uint8_t emProcessAshRxInput(const uint8_t *data,
                            uint8_t dataLength)
{
  return emProcessAshRxInputWithCallback(data, dataLength, serialCopyFromRx);
}

uint8_t emProcessAshRxInputWithCallback(const uint8_t *data,
                                        uint8_t dataLength,
                                        SerialRxHandler serialRxHandler)
{
  emApiCounterHandler(EMBER_ASH_V3_TOTAL_BYTES_RECEIVED, dataLength);
  AshMessageType txType = ASH_ACK;
  uint8_t eaten = runRxStateMachine(data, dataLength, &txType);

  while (ashRxState.frameState == ASH_DONE) {
    // log that we've received data
    ASH_V3_DEBUG(AshMessageType type =
                 reallyGetType(ashRxTestState.rawData[ASH_CONTROL_BYTE_INDEX]);
                 OUTPUT(0,
                        "[[%s %u ASH RX %s [my OFC %u | AFC %u] "
                        "[their OFC %u | AFC %u] %u ",
                        emAppName,
                        halCommonGetInt32uMillisecondTick(),
                        (type < LAST_ASH_MESSAGE_TYPE
                         ? typeNames[type]
                         : "OUT OF BOUNDS"),
                        ashTxState.outgoingFrameCounter,
                        ashTxState.ackNackFrameCounter,
                        reallyGetOutgoingFrameCounter
                        (ashRxTestState.rawData[ASH_CONTROL_BYTE_INDEX]),
                        reallyGetAckNackFrameCounter
                        (ashRxTestState.rawData[ASH_CONTROL_BYTE_INDEX]),
                        ashRxTestState.rawDataIndex);
                 OUTPUT_BYTES(0,
                              "bytes:",
                              ashRxTestState.rawData,
                              ashRxTestState.rawDataIndex);
                 OUTPUT(0, "]]\n\n"););

    // process the packet
    doPostRxStateMachineTasks(txType, serialRxHandler);
    emResetAshRxState();
    txType = ASH_ACK;

    ASH_V3_DEBUG(MEMSET(&ashRxTestState, 0, sizeof(AshRxTestState)));

    eaten += runRxStateMachine(data + eaten, dataLength - eaten, &txType);
  }

  return eaten;
}

bool isAshActive(void)
{
  return (ashRxState.frameState != ASH_INACTIVE);
}

void uartLinkReset(void)
{
  emResetAshRxState();
  emResetAshTxState();
  emResetSerialState(false);
  ashState = ASH_STATE_RESET_TX_PRE;
  emberAshStatusHandler(ashState);
  reallyFlush(true, ASH_RESET);
}

void emResetAshRxState(void)
{
  MEMSET(&ashRxState, 0, sizeof(AshRxState));
}

void emResetAshTxState(void)
{
  MEMSET(&ashTxState, 0, sizeof(AshTxState));
  ashTxState.dmaBuffer = &ashTxState.dmaBufferA;
  emEraseAndPrepareDmaBuffer(&ashTxState.dmaBufferA);
  emEraseAndPrepareDmaBuffer(&ashTxState.dmaBufferB);
}

bool uartTxSpaceAvailable(void)
{
  nextTx();

  return (ashState == ASH_STATE_RUNNING
          && ashTxState.dmaBuffer->state <= ASH_TX_FLUSH
          && !ashTxState.dmaBuffer->resend
          && (ashTxState.dmaBuffer->finger
              < ((ashTxState.dmaBuffer->data + MAX_ASH_PACKET_SIZE)
                 - ASH_CRC_SIZE)));
}

bool uartTxReady(void)
{
  return (ashTxState.dmaBuffer->state == ASH_TX_FLUSH);
}

void emExtractAshPacketInformation(const uint8_t *packet,
                                   uint8_t *headerEscapeByteLoc,
                                   uint8_t *outgoingFrameCounterLoc,
                                   uint8_t *ackNackFrameCounterLoc,
                                   AshMessageType *typeLoc,
                                   uint8_t *payloadLengthLoc)
{
  uint8_t controlByte = getControlByte(packet);

  if (headerEscapeByteLoc != NULL) {
    *headerEscapeByteLoc = packet[ASH_HEADER_ESCAPE_BYTE_INDEX];
  }

  if (typeLoc != NULL) {
    *typeLoc = reallyGetType(controlByte);
  }

  if (outgoingFrameCounterLoc != NULL) {
    *outgoingFrameCounterLoc = reallyGetOutgoingFrameCounter(controlByte);
  }

  if (ackNackFrameCounterLoc != NULL) {
    *ackNackFrameCounterLoc = reallyGetAckNackFrameCounter(controlByte);
  }

  if (payloadLengthLoc != NULL) {
    *payloadLengthLoc = packet[ASH_PAYLOAD_LENGTH_INDEX];
  }
}

//
// Debug functions and variables used for simulated testing, and for images
// that have USE_ASH_V3_DEBUG defined
//
#ifdef ASH_V3_DEBUG_ENABLED
static const char * const frameStateNames[ASH_LAST_FRAME_STATE] = {
  "ASH_INACTIVE",
  "ASH_NEED_CONTROL_BYTE",
  "ASH_NEED_PAYLOAD_LENGTH",
  "ASH_NEED_PAYLOAD",
  "ASH_NEED_HIGH_CRC",
  "ASH_NEED_IN_BETWEEN_CRC",
  "ASH_NEED_LOW_CRC",
  "ASH_DONE"
};

static const int frameStateSizeChecker[COUNTOF(frameStateNames)
                                       == ASH_LAST_FRAME_STATE
                                       ? 1
                                       : -1];

void emPrintAshRxState(void)
{
  reallyPrintAshRxState(0);
}

static void reallyPrintAshRxState(uint8_t indent)
{
  // get rid of an IAR warning
  if (frameStateSizeChecker[0] == 0) {
  }

  assert(ashRxState.frameState < ASH_LAST_FRAME_STATE);
  OUTPUT(indent, "Ash Frame State %s\r\n", frameStateNames[ashRxState.frameState]);
  OUTPUT(indent, "Control byte: %u\r\n", ashRxState.controlByte);
  OUTPUT(indent, "CRC high byte: %u\r\n", ashRxState.highCrcByte);
  OUTPUT(indent,
         "%u/%u data bytes\r\n",
         ashRxState.payloadIndex,
         ashRxState.payloadLength);

  uint8_t i;

  for (i = 0; i < ashRxState.payloadIndex; i++) {
    OUTPUT(indent, "%x ", ashRxState.payload[i]);

    if (i < ashRxState.payloadIndex - 1) {
      OUTPUT(0, ", ");
    }
  }

  OUTPUT(indent, "\r\n");
}

static const char * const stateNames[] = {
  "ASH_TX_INACTIVE",
  "ASH_TX_ACTIVE",
  "ASH_TX_FLUSH",
  "ASH_TX_EN_ROUTE_PRE",
  "ASH_TX_EN_ROUTE_POST",
  "ASH_TX_RESEND_ACKED",
};

struct VariablesSizeAssurer
{
#ifdef ASH_V3_DEBUG_ENABLED
  uint8_t unusedTypeNamesSizeAssurer[COUNTOF(typeNames) == LAST_ASH_MESSAGE_TYPE
                                   ? 1
                                   : -1];
#endif // ASH_V3_DEBUG_ENABLED
  uint8_t unusedStateNamesSizeAssurer[COUNTOF(stateNames) == LAST_TX_STATE
                                    ? 1
                                    : -1];
};

static void printDmaBuffer(const char *name,
                           const AshTxDmaBuffer *dmaBuffer,
                           uint8_t indent)
{
  assert(dmaBuffer->state < LAST_TX_STATE);

  OUTPUT(indent,
         "%s %s | ",
         (ashTxState.dmaBuffer == dmaBuffer
          ? "==> "
          : ""),
         name);

  OUTPUT(0,
         "resend: %c # %u | ",
         (dmaBuffer->resend
          ? 'Y'
          : 'N'),
         dmaBuffer->resendCount);

  Event *event = getEvent(dmaBuffer);

  OUTPUT(0,
         "%s | next @ %d |",
         stateNames[dmaBuffer->state],
         (emberEventIsScheduled(event)
          ? ((int32_t)event->timeToExecute
             - (int32_t)halCommonGetInt32uMillisecondTick())
          : 0));

  if (dmaBuffer->state != ASH_TX_INACTIVE) {
    emPrintAshPacketInformation(dmaBuffer->data);
  }

  OUTPUT(indent, "\r\n");
}

static const char * const stateStrings[] = {
  ASH_STATE_STRINGS
};

void emPrintAshState(void)
{
  reallyPrintAshState(0);
}

static void reallyPrintAshState(uint8_t indent)
{
  assert(0 <= (int)ashState && (int)ashState < COUNTOF(stateStrings));
  OUTPUT(indent, "ASH State: %s\r\n", stateStrings[ashState]);
}

void emPrintAshTxState(void)
{
  reallyPrintAshTxState(0);
}

static void reallyPrintAshTxState(uint8_t indent)
{
  OUTPUT(indent, "[ASH TX State @ %u\r\n", halCommonGetInt32uMillisecondTick());
  OUTPUT(indent + 2, "OFC: %u\r\n", ashTxState.outgoingFrameCounter);
  OUTPUT(indent + 2, "AFC: %u\r\n", ashTxState.ackNackFrameCounter);
  printDmaBuffer("DmaBufferA", &ashTxState.dmaBufferA, indent + 2);
  printDmaBuffer("DmaBufferB", &ashTxState.dmaBufferB, indent + 2);
  OUTPUT(indent, "/ASH TX State]\r\n");
}

static const char * const headerEscapeByteStrings[] = {
  "ASH_PAYLOAD_LENGTH_BYTE_ESCAPED",
  "ASH_CONTROL_BYTE_ESCAPED"
};

void emPrintAshPacketInformation(const uint8_t *packet)
{
  reallyPrintAshPacketInformation(packet, 2);
}

static void reallyPrintAshPacketInformation(const uint8_t *packet, uint8_t indent)
{
  AshMessageType type;
  uint8_t outgoingFrameCounter;
  uint8_t ackNackFrameCounter;
  uint8_t payloadLength;
  uint8_t headerEscapeByte;

  emExtractAshPacketInformation(packet,
                                &headerEscapeByte,
                                &outgoingFrameCounter,
                                &ackNackFrameCounter,
                                &type,
                                &payloadLength);

  OUTPUT(indent,
         "ASH, type: %s, header %u: ",
         (type < LAST_ASH_MESSAGE_TYPE
          ? typeNames[type]
          : "invalid"),
         headerEscapeByte);

  uint8_t i;
  // basic sanity checking
  bool gotOne = false;

  for (i = 0; i < COUNTOF(headerEscapeByteStrings); i++) {
    if ((headerEscapeByte & (1 << i)) != 0) {
      OUTPUT(0, "%s | ", headerEscapeByteStrings[i]);
      gotOne = true;
    }
  }

  if (headerEscapeByte != 0 && ! gotOne) {
    OUTPUT(0, "MALFORMED");
  }

  OUTPUT(0,
         ", OFC: %u, "
         "AFC: %u, "
         "length: %u\r\n",
         outgoingFrameCounter,
         ackNackFrameCounter,
         payloadLength);
}

static bool dmaBufferAvailable(const AshTxDmaBuffer *buffer)
{
  return (buffer->state <= ASH_TX_FLUSH
          || (buffer->state == ASH_TX_EN_ROUTE_POST
              && emAshTxDmaBufferPayloadLength(buffer) == 0
              && emAshGetType(buffer) != ASH_RESET));
}

void emAssertAshTxState(uint8_t outgoingFrameCounter,
                        uint8_t ackNackFrameCounter,
                        uint8_t dmaBuffersAvailable)
{
  assert(outgoingFrameCounter == ashTxState.outgoingFrameCounter);
  assert(ackNackFrameCounter == ashTxState.ackNackFrameCounter);
  uint8_t available = 0;

  if (dmaBufferAvailable(&ashTxState.dmaBufferA)) {
    available++;
  }

  if (dmaBufferAvailable(&ashTxState.dmaBufferB)) {
    available++;
  }

  assert(available == dmaBuffersAvailable);
}

void emSetAshTxState(uint8_t outgoingFrameCounter,
                     uint8_t ackNackFrameCounter)
{
  ashTxState.outgoingFrameCounter = outgoingFrameCounter;
  ashTxState.ackNackFrameCounter = ackNackFrameCounter;
}
#endif // EMBER_TEST

// returns true if there is still data to be sent
bool emAshPreparingForPowerDown(void)
{
  if (ashTxState.dmaBufferA.state != ASH_TX_INACTIVE
      || ashTxState.dmaBufferB.state != ASH_TX_INACTIVE) {
    uartFlushTx();
    return true;
  } else {
    return false;
  }
}

void emMakeAshPacket(AshTxDmaBuffer *target,
                     AshMessageType type,
                     uint8_t outgoingFrameCounter,
                     uint8_t ackNackFrameCounter,
                     const uint8_t *payload,
                     uint8_t payloadLength,
                     bool flush)
{
  emEraseAndPrepareDmaBuffer(target);
  // help the debugger
  uint8_t enqueued =
    halHostReallyEnqueueTx(payload, payloadLength, target, flush);
  assert(enqueued == payloadLength);
  emCreateAshHeader(target,
                    type,
                    outgoingFrameCounter,
                    ackNackFrameCounter,
                    // account for escaped bytes, don't use payloadLength
                    target->finger - target->data - ASH_HEADER_LENGTH);
}
