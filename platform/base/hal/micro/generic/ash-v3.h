/*
 * File: ash-v3.h
 * Description: ASHv3 Functionality for realiable UART communication
 *
 * Copyright 2013 by Silicon Laboratories. All rights reserved.             *80*
 */

/**
 * @addtogroup ashv3
 * @verbatim
  ASHv3 Header (4 bytes)
          0                 1                 2                 3
  +-+-+-+-+-+-+-+-+|+-+-+-+-+-+-+-+-+|+-+-+-+-+-+-+-+-+|+-+-+-+-+-+-+-+-+
  +   ASH_FLAG     |  HEADER ESCAPE  |  CONTROL BYTE   | PAYLOAD LENGTH +
  +-+-+-+-+-+-+-+-+|+-+-+-+-+-+-+-+-+|+-+-+-+-+-+-+-+-+|+-+-+-+-+-+-+-+-+

  ASH_FLAG is defined below

  Header Escape bytes contains escape data for the control byte and the payload
  length byte, and is encoded as:

    +-+-+-+-+-+-+-+-+
    +A B            +
    +-+-+-+-+-+-+-+-+

    where A and B are booleans representing:
    A = is the control byte escaped?          (1 << 7)
    B = is the payload length byte escaped?   (1 << 6)

  The control byte has the syntax:

    +-+-+-+-+-+-+-+-+
    + T + OFC + AFC +
    +-+-+-+-+-+-+-+-+

  where:

    T = type (see AshMessageType below)
    OFC = outgoing frame counter
    AFC = ack/nack frame counter

  Payload length is in the range: [0, MAX_ASH_PAYLOAD_SIZE] (defined below)

  The CRC is 3 bytes and contains 2 bytes of data. It is stored in such a way
  that each of its bytes never needs escaping.

  The escape bytes are:

    ASH_FLAG             0b01111110
    ASH_ESC              0b01111101
    ASH_XON              0b00010001
    ASH_XOFF             0b00010011
    ASH_COBRA_FORCE_BOOT 0b11111000
                              ^ They all have this bit set

  The CRC is expanded in the following way:

  +-+-+-+-+-+-+-+-+|+-+-+-+-+-+-+-+-+
  +  High CRC      +     Low CRC    +
  +-+-+-+-+-+-+-+-+|+-+-+-+-+-+-+-+-+
   F E D C B A 9 8   7 6 5 4 3 2 1 0
         ^                 ^-------------+
         +-----------------------------+ |
  +-+-+-+-+-+-+-+-+|+-+-+-+-+-+-+-+-+|+-+-+-+-+-+-+-+-+
  +  New High CRC  |   New Low CRC   |   Bits         +
  +-+-+-+-+-+-+-+-+|+-+-+-+-+-+-+-+-+|+-+-+-+-+-+-+-+-+
                                       7 6 5 4 3 2 1 0

  -----------------------------------------------------------------------------
  PROTOCOL

  * Design

  The upward interface sends and receives a stream of bytes. The upper layer
  writes and reads data in blocks, but the block boundaries are not preserved
  over the wire. The UART framing is independent of the upward interface; the
  data is treated as a stream.

  The protocol is organized around using a single DMA operation to send a
  frame. The maximum size is of a DMA buffer is 100 bytes. Getting good
  throughput requires allowing two frames in flight; having more does not help.
  The UART is responsible for ACKing and retrying frames over the wire. Each
  frame contains a sequence number, the sequence number of the most recently
  received frame, and a flag indicating whether or not corrupt data was
  received after that frame.

  An OFC value of 0 is never used because 0 is the AFC's default/uninitialized
  value.

  * Resets

  To reset the UART link a device sends a RESET frame and the peer responds
  with a RESET_ACK frame. RESET and RESET_ACK frames have a single data byte that
  gives the type of reset. For a RESET packet, the OFC is set to 1 and the AFC
  is set to 0. For a RESET_ACK packet with no payload, the OFC is 1 and the AFC
  is 1. For an RESET_ACK with non-empty payload, the OFC is 2 and the AFC is 1.

  * ACKs, NACKs and retransmission

  The driver retains transmitted frames until a matching ACK is received.
  Frames are retransmitted if a NACK is received or, in case an ACK is lost,
  if a resend timer expires.

  ACK, NACK, and RESET_ACK frames can carry data. The ACK/NACK counter has the
  frame counter from the last correctly received frame. A NACK frame is sent if
  corrupt data or a corrupt frame is received. An ACK frame is sent whenever a
  data-containing frame is received correctly. If an ACK/NACK has no payload,
  its OFC is ignored and no ACK is sent in response. Note that we record an
  RESET_ACK's OFC whether or not its payload is populated. An ACK/NACK/RESET_ACK with
  empty payload must not have an incremented OFC.

  After sending a RESET frame all incoming ACK and NACK frames are ignored
  until an RESET_ACK is received. Additional incoming RESET frames are answered
  with a matching RESET_ACK.

  * Waking

  When using the UART to wake up the other device, the byte 0xFF (ASH_WAKEUP)
  can be sent in betweem frames. This only applies between frames, so 0xFF does
  not need to be escaped within a frame. Any number of wake bytes can be sent
  between the ASH_FLAG at the end of one frame and an ASH_FLAG at the beginning
  of the next frame.
  @endverbatim
*/

// bytes that require escaping
#define ASH_FLAG 0x7E             // 126   0b01111110
#define ASH_ESC  0x7D             // 125   0b01111101
#define ASH_XON  0x11             // 17    0b00010001
#define ASH_XOFF 0x13             // 19    0b00010011
#define ASH_COBRA_FORCE_BOOT 0xF8 // 248   0b11111000

#define ASH_WAKEUP 0xFF // 255
#define ASH_ESCAPE_BYTE 0x20 // 32

// the ACK timeout used for retries
#define ASH_ACK_TIMEOUT 500

// ASH_FLAG + control-byte + 1-byte-data-length + header escape + 3 CRC bytes
#define ASH_BIG_HEADER_LENGTH 7

typedef enum {
  ASH_FLAG_INDEX               = 0,
  ASH_HEADER_ESCAPE_BYTE_INDEX = 1,
  ASH_CONTROL_BYTE_INDEX       = 2,
  ASH_PAYLOAD_LENGTH_INDEX     = 3,
  ASH_HEADER_LENGTH            = 4
} AshHeaderBytesLocation;

#define ASH_CONTROL_BYTE_ESCAPED        (1 << 7)
#define ASH_PAYLOAD_LENGTH_BYTE_ESCAPED (1 << 6)

#define AshHeaderEscapeType uint8_t

#define ASH_CRC_SIZE 3

// set the max ash packet size
#define MAX_ASH_PACKET_SIZE 64
#define MAX_ASH_PAYLOAD_SIZE (MAX_ASH_PACKET_SIZE - ASH_BIG_HEADER_LENGTH)
#define MAX_ASH_RESEND_COUNT 10

#if ((defined UNIX_HOST) || (defined EMBER_TEST) || (defined EMBER_STACK_COBRA)) && (!defined INCLUDE_ASH_V3_TEST_DATA)
#  define INCLUDE_ASH_V3_TEST_DATA 1
#endif

//
// AshState
//
#define ASH_STATE_STRINGS                        \
  "ASH_STATE_RESET_TX_PRE",                      \
  "ASH_STATE_RESET_TX_POST",                     \
  "ASH_STATE_RUNNING",                           \
  "ASH_STATE_LAST"

typedef enum {
  // the RESET packet has been sent to the UART link
  ASH_STATE_RESET_TX_PRE,

  // the UART is done sending the RESET packet
  ASH_STATE_RESET_TX_POST,

  // we're received an RESET_ACK to our RESET
  ASH_STATE_RUNNING,
  ASH_STATE_LAST
} AshState;

// ash message types
typedef enum {
  ASH_RESET      = 0,
  ASH_RESET_ACK  = 1,
  ASH_ACK        = 2,
  ASH_NACK       = 3,
  LAST_ASH_MESSAGE_TYPE
} AshMessageType;

typedef enum {
  ASH_TX_INACTIVE      = 0, // no data
  ASH_TX_ACTIVE        = 1, // some data
  ASH_TX_FLUSH         = 2, // send data now
  ASH_TX_EN_ROUTE_PRE  = 3, // the UART has been notified to push the bits
                            // and the CRC has been added
  ASH_TX_EN_ROUTE_POST = 4, // the UART is done pushing the bits
  ASH_TX_RESEND_ACKED  = 5, // a resend was acked en-flight
  LAST_TX_STATE
} AshTxDmaBufferState;

//
// TX
//
typedef struct {
  uint8_t data[MAX_ASH_PACKET_SIZE];
  uint8_t *finger;
  AshTxDmaBufferState state;

  // does this buffer need a resend
  bool resend;
  uint8_t resendCount;

  // for debug
  bool isCorrupt;
} AshTxDmaBuffer;

typedef struct {
  AshTxDmaBuffer dmaBufferA;
  AshTxDmaBuffer dmaBufferB;

  // pointer to either txDmaBufferA or txDmaBufferB
  AshTxDmaBuffer *dmaBuffer;

  // TODO: save a byte of RAM by storing these variables in a single byte
  uint8_t outgoingFrameCounter;
  uint8_t ackNackFrameCounter;

  // transient variable that indicates whether the serial layer
  // replied during RX handling
  bool serialLayerReplied;
} AshTxState;

extern AshTxState ashTxState;

// framing states, each follows the state before it
typedef enum {
  ASH_INACTIVE = 0,
  ASH_NEED_HEADER_ESCAPE_BYTE,
  ASH_NEED_CONTROL_BYTE,
  ASH_NEED_PAYLOAD_LENGTH,
  ASH_NEED_PAYLOAD,
  ASH_NEED_HIGH_CRC,
  ASH_NEED_IN_BETWEEN_CRC,
  ASH_NEED_LOW_CRC,
  ASH_DONE,
  ASH_LAST_FRAME_STATE
} AshRxFrameState;

//
// This struct contains the ASH Receive State
//
typedef struct {
  //
  // Paylaod data
  //
  uint8_t payload[MAX_ASH_PAYLOAD_SIZE];

  // index of the "real", or unescaped data in payload
  // its range: [0, MAX_ASH_PAYLOAD_SIZE - (number of ASH_ESC characters)]
  uint8_t payloadIndex;

  // index of the escaped data
  // its range: [0, MAX_ASH_PAYLOAD_SIZE]
  // is always >= payloadIndex
  uint8_t escapedPayloadIndex;

  uint8_t payloadLength;
  //
  // /payload data
  //

  // the control byte, described in ash-v3.h
  uint8_t controlByte;

  // the header escape byte, described in ash-v3.h
  uint8_t headerEscapeByte;

  // the running CRC-16 that is computed over the entire ASH packet
  // (except the CRC-16 itself)
  uint16_t computedCrc;

  // the high CRC high byte received
  uint8_t highCrcByte;

  // the in between CRC high byte received
  uint8_t inBetweenCrcByte;

  // the frame state, aka what do we need to parse next
  AshRxFrameState frameState;

  // do we need to escape the next byte?
  bool escapeNextByte;
} AshRxState;

#if defined(EMBER_TEST) || defined(UNIX_HOST) || defined(UNIX_HOST_SIM)
typedef struct {
  // for debugging, the actual received data including the ASH frame
  uint8_t rawData[MAX_ASH_PACKET_SIZE * 2];
  uint8_t rawDataStartIndex;
  uint8_t rawDataIndex;
  uint8_t receiveCount;
} AshRxTestState;

extern AshRxTestState ashRxTestState;
#endif

extern AshRxState ashRxState;

#define ASH_FRAME_COUNTER_ROLLOVER 8
#define NEXT_ASH_OUTGOING_FRAME_COUNTER(x)      \
  (x < ASH_FRAME_COUNTER_ROLLOVER - 1           \
   ? x + 1                                      \
   : 1)

// should this byte be escaped?
bool emAshByteShouldBeEscaped(uint8_t byte);

uint8_t emProcessAshRxInput(const uint8_t *input, uint8_t inputLength);

typedef uint16_t (*SerialRxHandler)(const uint8_t *data, uint16_t length);
uint8_t emProcessAshRxInputWithCallback(const uint8_t *data,
                                        uint8_t dataLength,
                                        SerialRxHandler serialRxHandler);
bool isAshActive(void);

// ash debug functions
void emPrintAshState(void);
void emPrintAshRxState(void);
void emPrintAshTxState(void);
void emPrintAshPacketInformation(const uint8_t *packet);
void emMakeAshPacket(AshTxDmaBuffer *target,
                     AshMessageType type,
                     uint8_t outgoingFrameCounter,
                     uint8_t ackNackFrameCounter,
                     const uint8_t *payload,
                     uint8_t payloadLength,
                     bool flush);
void emCopyAshDmaBuffer(AshTxDmaBuffer *target,
                        AshTxDmaBuffer *source,
                        AshMessageType type,
                        uint8_t outgoingFrameCounter,
                        uint8_t ackNackFrameCounter);
uint8_t emAshGetOutgoingFrameCounter(AshTxDmaBuffer *buffer);
uint8_t emAshGetAckNackFrameCounter(AshTxDmaBuffer *buffer);
AshMessageType emAshGetType(const AshTxDmaBuffer *buffer);
void emPrintBytes(const uint8_t *bytes, uint16_t legnth, uint8_t indent);

// set packet information
void emAshSetType(AshTxDmaBuffer *buffer, AshMessageType type);

// add payload data to the ASH TX packet
uint16_t emAddAshTxData(const uint8_t *payloadData, uint16_t payloadDataLength);
bool uartTxSpaceAvailable(void);

// reset the ASH TX state
void emResetAshState(void);
void emResetAshTxState(void);
void emResetAshRxState(void);
void emInitializeAshTxDmaBuffer(AshTxDmaBuffer *target);

// store the current ASH DMA buffer and its length into a target
bool emGetAshTxPacket(uint8_t **target, uint16_t *length);
uint16_t halHostReallyEnqueueTx(const uint8_t *data,
                              uint16_t dataLength,
                              AshTxDmaBuffer *target,
                              bool forceFlush);

void uartLinkReset(void);

void uartFlushTx(void);

// Set the drop and corruption rate for the UART
void emAshConfigUart(uint8_t dropPercentage, uint8_t corruptPercentage);

// This function is called by the UART when it is done sending the bits
// ASH previously gave it to.
// It is non-reentrant and must not be called from halHostUartLinkTx().
void emAshNotifyTxComplete(void);

// called directly by emAshNotifyTxComplete() with loadTx = true
void emAshReallyNotifyTxComplete(bool loadTx);

// populate a TX DMA Buffer with header data
uint16_t emCreateAshHeader(AshTxDmaBuffer *target,
                         AshMessageType type,
                         uint8_t outgoingFrameCounter,
                         uint8_t ackNackFrameCounter,
                         uint8_t payloadLength);

void emEraseAndPrepareDmaBuffer(AshTxDmaBuffer *buffer);
uint8_t emAshTxDmaBufferPayloadLength(const AshTxDmaBuffer *buffer);

// testing //
void emAssertAshTxState(uint8_t outgoingFrameCounter,
                        uint8_t ackNackFrameCounter,
                        uint8_t dmaBuffersAvailable);
void emSetAshTxState(uint8_t outgoingFrameCounter,
                     uint8_t ackNackFrameCounter);

bool emAshPreparingForPowerDown(void);

void emExtractAshPacketInformation(const uint8_t *packet,
                                   uint8_t *headerEscapeByteLoc,
                                   uint8_t *outgoingFrameCounterLoc,
                                   uint8_t *ackNackFrameCounterLoc,
                                   AshMessageType *typeLoc,
                                   uint8_t *payloadLengthLoc);
uint32_t emGetAshCrc(const uint8_t *data, uint16_t length);
uint8_t *emStoreAshCrc(uint8_t *location, uint16_t crc, uint8_t *escapeByteLocation);

// notification that ash has changed state
void emberAshStatusHandler(AshState state);

// ack and nack
void emAshHandleNack(uint8_t frameCounter);
void emAshHandleAck(uint8_t frameCounter);

// tell the application that the serial link was reset
void emResetSerialState(bool external);

/**
 * @addtogroup ashv3
 *
 * @{
 */

/**  @name Application Functions
 * Implement these functions in your application
 *
 * @{
 *
 * The following functions are only for builds that support software flow
 * control and that are compiled with
 * EMBER_APPLICATION_SUPPORTS_SOFTWARE_FLOW_CONTROL
 */

/**
 * @brief Tell the application that we received an XON
 */
void emberXOnHandler(void);

/**
 * @brief Tell the application that we received an XOFF
 */
void emberXOffHandler(void);

/**
 * @}
 */
