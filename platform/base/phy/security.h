#ifndef __SECURITY_H__
#define __SECURITY_H__


// ZigBee definitions:

#define SECURITY_AUTHENTICATION_MASK 0x03
#define SECURITY_ENCRYPTION_MASK     0x04

// 802.15.4 definitions:

// MAC security nonce offsets
#define NONCE_FLAGS_OFFSET         15
#define NONCE_SOURCE_ADDR_OFFSET   7
#define NONCE_FRAME_COUNTER_OFFSET 3
#define NONCE_KEY_SEQ_OFFSET       2
#define NONCE_BLOCK_COUNTER_OFFSET 0

// M MIC size
#define MIC_SIZE_0   0
#define MIC_SIZE_4   4
#define MIC_SIZE_8   8
#define MIC_SIZE_16  16
#define MIC_SIZE_MAX MIC_SIZE_16

// MAC security field sizes
#define SECURITY_FRAME_COUNTER_SIZE 4
#define SECURITY_KEY_SEQ_NUM_SIZE   1

// Zero is not a valid 802.15.4 key sequence number.
#define INVALID_SEQUENCE_NUMBER 0

#if (! defined(EMBER_STACK_IP))
  // Pro Stack
  #define SECURITY_AUX_HDR_LENGTH (SECURITY_FRAME_COUNTER_SIZE + \
                                   SECURITY_KEY_SEQ_NUM_SIZE)
								 
  // TODO: document this function.
  bool emSecurityInit(void);

  // This function implements the platform specific routines
  // necesssary to initialize the security hardware.
  void emSecurityHardwareInit(void);

#else
  // IP Stack
  //
  // This assumes that we are always including the EUI64, which will hopefully
  // change at some point.
  #define SECURITY_AUX_HDR_LENGTH (1      /* frame control */     \
                                   + SECURITY_FRAME_COUNTER_SIZE  \
                                   + EUI64_SIZE                   \
                                   + SECURITY_KEY_SEQ_NUM_SIZE)

  void emSetNwkFrameCounter(uint32_t newFrameCounter);

#endif // !EMBER_STACK_IP


// Serves as a convenience for checking the over-air fcf's security flag.
#define USING_MAC_SECURITY(macInfoField) (macInfoField & MAC_INFO_MAC_SECURITY_MASK)

typedef struct {
  uint8_t  keySequenceNumber;
  EmberEUI64 sourceEui64;
  uint8_t frameCounterBytes[SECURITY_FRAME_COUNTER_SIZE];
} EmberSecurityNonce;

// We currently only support 4 byte MICs.
#define emMicLength 4

// Equivalent to emMicLength, providing a hint of encapsulation.
#define emSecurityGetMicLength() emMicLength

uint32_t emGetSecurityFrameCounter(void);

#if defined EMBER_TEST
extern uint32_t nextNwkFrameCounter;
extern uint32_t nextApsFrameCounter;
#endif

// Initializes the outgoing network frame counter with the value from the token.
void emSecurityReadNetworkFrameCounterToken(void);

// Increments the outgoing frame counter and manages its non-volatile storage.
// Returns the new counter value.
uint32_t emSecurityIncrementOutgoingFrameCounter(void);

// Resets the outgoing NWK frame counter to zero.  
// Updates the non-volatile storage.
void emResetNwkOutgoingFrameCounter(void);

// we expose these to interested parties
extern uint32_t emSecurityNonceFrameCounter;


// TODO: why do we have APS-related security routine signatures at the PHY?
void emSecurityReadApsFrameCounterToken(void);

// Retreives the current value of the outgoing APS Frame counter
uint32_t emGetApsFrameCounter(void);

// Increments the current value for the outgoing APS Frame counter.
uint32_t emNextApsFrameCounter(void);

// Resets the value for the outgoing APS frame counter.  Updates the 
// non-volatile storage.
void emResetApsFrameCounter(void);


// API

// Authenticates and (maybe) encrypts the message corresponding
// to header.  This function generates an authentication tag (MIC)
// either 4, 8, or 16 bytes long that is appeded to the payload.
// The encryption is done in-situ.
bool emEncryptPacket(PacketHeader header,
                        uint8_t authenticationStartIndex,
                        uint8_t auxFrameIndex);

#if (! defined(EMBER_STACK_IP))
  // Pro Stack
  //
  // This does the same thing, except that the packet is in a flat buffer.
  void emNetworkEncryptFlatPacket(uint8_t* packet,
                                  uint8_t packetLength,
                                  uint8_t authenticationStartIndex,
                                  uint8_t auxFrameIndex);

  // Authenticates and (maybe) decrypts the message corresponding to header.
  // 'sourceEui64' is used if the auxiliary frame does not contain the
  // senders EUI64.
  // The decryption is done in-situ, and payload will be shorter after the
  // call, reflecting the removal of the authentication tag.
  bool emDecryptPacket(PacketHeader header,
                          uint8_t authenticationStartIndex,
                          uint8_t auxFrameIndex,
                          EmberEUI64 sourceEui64);
#else
  // IP Stack
  //
  // This does the same thing, except that the packet is in a flat buffer.
  void emEncryptFlatPacket(uint8_t* packet,
                           uint8_t packetLength,
                           uint8_t authenticationStartIndex,
                           uint8_t auxFrameIndex,
                           bool macMode);

  #define emEncryptFlatZigbeePacket(packet, length, authIndex, auxIndex) \
   (emEncryptFlatPacket((packet), (length), (authIndex), (auxIndex), false))

  #define emEncryptFlat802d15d4Packet(packet, length, authIndex, auxIndex) \
   (emEncryptFlatPacket((packet), (length), (authIndex), (auxIndex), true))

  // Authenticates and (maybe) decrypts the message corresponding to header.
  // 'sourceEui64' is used if the auxiliary frame does not contain the
  // senders EUI64.
  // The decryption is done in-situ, and payload will be shorter after the
  // call, reflecting the removal of the authentication tag.
  bool emDecryptPacket(PacketHeader header,
                          uint8_t authenticationStartIndex,
                          uint8_t auxFrameIndex,
                          EmberEUI64 sourceEui64,
                          bool macMode);
#endif // !EMBER_STACK_IP

#endif //__SECURITY_H__
