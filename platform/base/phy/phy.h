#ifndef __PHY_H__
#define __PHY_H__

// Redefinition mechanism to build dual phy Connect in a single image.
#if defined (EMBER_STACK_CONNECT) && defined(CONNECT_DUAL_PHY)
#if defined (PHY_EM3XX)
  #include "mac/dual-phy-renaming-em3xx.h"
#elif defined (PHY_PRO2PLUS)
  #include "mac/dual-phy-renaming-pro2plus.h"
#endif
#endif // EMBER_STACK_CONNECT && CONNECT_DUAL_PHY

#ifdef  PHY_DUAL
  #include "dual/phy.h"
#endif//PHY_DUAL

#ifndef DUAL_DCL
  #define DUAL_DCL(var)      var
  #define DUAL_CBG(var)      var
  #define DUAL_CBS(var,type) var
  #define DUAL_GET(var)      var
  #define DUAL_INC(var)      ((var)++)
  #define DUAL_SET(var,type) var
#endif//DUAL_DCL

#include "phy/phy-appended-info.h"

#define IS_VALID_CHANNEL(newValue)                           \
  ( ((newValue) <= EMBER_MAX_802_15_4_CHANNEL_NUMBER) &&     \
    ( (EMBER_MIN_802_15_4_CHANNEL_NUMBER == 0) ||            \
      ((newValue) >= EMBER_MIN_802_15_4_CHANNEL_NUMBER) ) )

#define IS_VALID_CHANNEL_INDEX(newValue)                     \
  ( (newValue) < EMBER_NUM_802_15_4_CHANNELS )

// 802.15.4 PHY payload length.
#define PHY_MAX_SIZE          127
// 802.15.4 PHY header length.
#define PHY_LENGTH_BYTE_SIZE  1
// 802.15.4 MAC footer length
#define PHY_APPENDED_CRC_SIZE 2

// IEEE 802.15.4-2003 6.1.6:
#define IEEE_154_PER_TEST_PHY_PAYLOAD_LENGTH 20

// where do error codes go?
#define ERR_RADIO_TX_BUSY  1
#define CCA_BUSY 0
#define CCA_CLEAR 1
#define LQI_DONT_USE 0

// An int8_t value that represents an invalid RSSI energy level.
// Maybe someday we'll need to make it public in ember-types.h.
#define EMBER_PHY_INVALID_RSSI -128

//------------------------------------------------------------------
// 15.4 MAC Frame Control and Information Element fields move to
// separate file for clients that need only those macros.
//------------------------------------------------------------------
#include "ieee802154mac.h"

//------------------------------------------------------------------------
// 15.4 Commands used by phy code
#define DATA_REQUEST_COMMAND 0x04

//------------------------------------------------------------------
// 15.4g 16-bit PHY Header (PHR) definitions -- in host order
#define PHR_154G_MODE_SWITCH_MASK        0x8000u
#define PHR_154G_MS_PARAMETER_ENTRY_MASK 0x6000u
#define PHR_154G_MS_FEC_MASK             0x1000u
#define PHR_154G_MS_NEW_MODE_MASK        0x0FE0u
#define PHR_154G_MS_CHECKSUM_MASK        0x001Eu
#define PHR_154G_MS_PARITY_MASK          0x0001u
#define PHR_154G_FCS_MASK                0x1000u
#define PHR_154G_DW_MASK                 0x0800u
#define PHR_154G_FRAME_LENGTH_MASK       0x07FFu

//------------------------------------------------------------------
// Miscellaneous PHY functions.

// This function stores the 64-bit EUI.  It relies on the value already having
// been stored in the emLocalEui64 variable.  For some implementations,
// this variable may be memory-mapped over the appropriate registers, in 
// which case this function may evaluate to whitespace as the work is already
// done.
void emPhySetEui64(void);

void emPhyTick(bool isrContext);

//------------------------------------------------------------------
// PHY Power Management.

// Radio power modes.
enum {
  EMBER_RADIO_POWER_MODE_RX_ON,
  EMBER_RADIO_POWER_MODE_ED_ON = EMBER_RADIO_POWER_MODE_RX_ON,
  EMBER_RADIO_POWER_MODE_OFF
};
typedef uint8_t RadioPowerMode;

void emRadioInit(RadioPowerMode initialRadioPowerMode);

// Turns the radio power completely off immediately.
void emRadioSleep(void);

// Wakes the radio up from sleep and puts it in RX mode.
void emRadioWakeUp(void);

// Returns the current power status of the radio.
RadioPowerMode emRadioGetPowerStatus(void);

//------------------------------------------------------------------
// Radio configuration (for PHYs that support this)

// APIs for dealing with MAC vs. PHY channel numbering
#define INVALID_CHANNEL 0xFF
#ifndef MAC_HAS_CHANNEL_PAGES
#define MAC_HAS_CHANNEL_PAGES ( (!defined(EMBER_STACK_CONNECT)) \
                              &&( defined(PHY_PRO2PLUS)         \
                                ||defined(PHY_DUAL)             \
                                ||defined(PHY_PRO2)             \
                                ||defined(PHY_RAIL)             \
                                ||defined(PHY_EFR32) )          \
                              )
#endif//MAC_HAS_CHANNEL_PAGES

#ifdef  PHY_EZR2
  typedef struct EzConfig {
    uint8_t    sizePart1;
    uint8_t  * configPart1;
    uint8_t    sizePart2;
    uint8_t  * configPart2;
    uint16_t   configCRC;
  } EzConfig;
  typedef struct EzConfigs {
    EzConfig * configTRX;
    EzConfig * configCW;
    EzConfig * configPN9;
  } EzConfigs;
  typedef EzConfigs EmPhyRadioConfig;
#else//!PHY_EZR2
  typedef uint8_t EmPhyRadioConfig;
#endif//PHY_EZR2

typedef struct EmPhySubBandParams {
  uint8_t  lbtBackoffSym;     // aLBTGranularity
  uint8_t  lbtBackoffMinExp;  // Random Backoff        LBT_OFFSET
  uint8_t  lbtBackoffMaxExp;  // = rand(2^[lbtBackoffMinExp..lbtBackoffMaxExp]
  int8_t   lbtBackoffAdjust;  //        - 1 + lbtBackoffAdjust) * lbtBackoffSym
  uint8_t  lbtMinBackoffs;    // Minimum backoffs required (usually 0)
  int8_t   lbtTries;          // aLBTMaxTxRetries + 1 (negate if no backoff on 1st try)
  uint16_t lbtTimeoutSym;     // aLBTTimeout
  uint16_t lbtCcaSym;         // aLBTMinFree           LBT_CCA
  uint16_t dutyTxOffSym;      // aLBTTxMinOff          LBT_BACKOFF
  uint16_t dutyTxMaxPktByte;  // aLBTTxMaxPKT          LBT_MAXTX
  uint16_t dutyTxMaxSeqByte;  // aLBTMaxDlg            LBT_MAXDIALOGUE
  uint8_t  pad[1];            //(alignment pad for future use)
  uint8_t  ackMinSym;         // aLBTAckWindowStart    LBT_MINACK
  uint16_t ackMaxSym;         // aLBTAckWindow+ACKtime LBT_MAXACK
} EmPhySubBandParams;

typedef struct EmPhySubBandConfig {
  uint8_t  minPhyChan;        // Lowest PHY channel for sub-band
  uint8_t  maxPhyChan;        // Highest PHY channel for sub-band
  int8_t   txMinPowerdBm;     // Minimum Tx Power allowed in sub-band
  int8_t   txMaxPowerdBm;     // Maximum Tx Power allowed in sub-band
  int8_t   edThresholddBm;    // Energy-Detect threshold
  uint8_t  lbtDutyDenom;      // aDUTYCYCLERegulated   LBT_MAXTX1HR
  uint8_t  criDutyDenom;      // aDUTYCYCLECriticalThresh
  uint8_t  nonDutyDenom10;    // non-LBT duty cycle denom / 10
 #if     MAC_HAS_CHANNEL_PAGES
  uint8_t  macChanPage;       // MAC Channel Page mapping for sub-band
  uint8_t  macChanOffset;     // MAC Channel Page offset of 1st channel
  uint8_t  pad[2];            //(alignment pad for future use)
  uint32_t* macChanMaskPtr;   // Pointer to MAC channel mask to enforce
                              // (not 'const' so it can be changed by app)
 #endif//MAC_HAS_CHANNEL_PAGES
  const EmPhySubBandParams* subBandParams; // Pointer to sub-band parameters
  const uint8_t* ccaConfigArray; // NULL,emPhyCcaCsmaConfigArray,emPhyCcaLbtConfigArray
} EmPhySubBandConfig;

typedef struct EmPhyBandConfig {
  uint8_t  minPhyChan;        // Lowest PHY channel for band
  uint8_t  maxPhyChan;        // Highest PHY channel for band
  uint8_t  pad[2];            //(alignment pad for future use)
  const uint8_t* bandName;    // Displayable name for the band
  uint32_t baseFreqHz;        // Base frequency of band, in Hz
  uint32_t chanFreqHz;        // Channel bandwidth (separation frequency), in Hz
  int32_t  radioXoFreq;       // Radio Xtal frequency from radio configuration
  uint32_t dataRateBps;       // Data rate in bits per second
  uint8_t  symbolBits;        // Number of bits in a symbol
  uint8_t  preambleBits;      // Preamble bits
  uint8_t  sfdBits;           // Sync word bits
  uint8_t  numSubBandConfigs; // Number of sub-band configs
  const EmPhySubBandConfig* subBandConfigs; // Pointer to sub-band configs
  const EmPhyRadioConfig* radioConfigArray; // Pointer to generated radio config
} EmPhyBandConfig;

typedef struct EmPhyTxPowerLevel {
  int8_t   dBm;
  uint8_t  regValue;
} EmPhyTxPowerLevel;

typedef struct EmPhyTxPA {
  const uint8_t* paName;      // Printable name for the PA
  uint8_t  paSelEnum;         // PROP_PA_MODE_PA_SEL_ENUM_*
  uint8_t  numPowerLevels;    // Number of powerLevels
  const EmPhyTxPowerLevel* powerLevels;
} EmPhyTxPA;

typedef struct EmPhyConfig {
  const uint8_t* phyName;     // Printable name for the PHY
  uint8_t  rssidBmOffset;     // RSSI to dBm arithmetic conversion factor (subtracted)
  uint8_t  pad[1];            //(alignment pad for future use)
  uint8_t  numBands;          // Number of bands in bandConfigs[] for this PHY
  uint8_t  numPAs;            // Number of Tx PAs for this PHY
  const EmPhyBandConfig* const* bandConfigs; // Pointer to band configs
  const EmPhyTxPA* PAs;     // Pointer to PA descriptors
} EmPhyConfig;

// The "public" structure for Duty Cycle information
typedef struct EmPhyDutyCycleParams {
  uint8_t  lbtRdcDenom;       // Regulatory duty cycle denominator w/  LBT
  uint8_t  criRdcDenom;       // Critical threshold level of lbtRdcDenom
  uint8_t  nonRdcDenom10;     // Regulatory duty cycle denominator w/o LBT / 10
  uint8_t  lbtFlags;          // Flags, e.g. lbtParams are LBT vs. CSMA
  uint32_t lbtMinOffTimeUs;   // Min post-LBT-Tx time the channel must be quiet
  uint16_t lbtCcaTimeUs;      // LBT CCA time
 #define   maxSeqDeadTimeUs lbtCcaTimeUs // Max inter-packet deadtime to stay in sequence
  uint16_t maxSeqBytes;       // Max # bytes allowed in a dialog sequence
  uint16_t maxPktBytes;       // Max # bytes allowed per transmission
  uint8_t  byteTimeUs;        // # microseconds per byte
  uint8_t  pktOvhBytes;       // Per-packet transmit overhead in bytes
  const EmPhySubBandParams* lbtParams; // LBT parameters
} EmPhyDutyCycleParams;

//------------------------------------------------------------------
// Transmit APIs.

#ifdef  LABTEST
// don't want to retry for phy testing as this breaks hw tests
#define RADIO_CCA_ATTEMPT_MAX_DEFAULT      1
#else//!LABTEST
#define RADIO_CCA_ATTEMPT_MAX_DEFAULT      5 // 1 + macMaxCSMAAttempts' retries
#endif//LABTEST
#define RADIO_BACKOFF_EXPONENT_MIN_DEFAULT 3
#define RADIO_BACKOFF_EXPONENT_MAX_DEFAULT 5
#ifdef PHY_TRANSCEIVER_SIM
//Need min of 1 backoff otherwise it breaks while cca response
#define RADIO_MINIMUM_BACKOFF_DEFAULT      1
#else //!PHY_TRANSCEIVER_SIM
#define RADIO_MINIMUM_BACKOFF_DEFAULT      0
#endif  //PHY_TRANSCEIVER_SIM

#define RADIO_TRANSMIT_CONFIG_DEFAULTS                            \
  {                                                               \
    true,  /* waitForAck */                                       \
    true,  /* checkCca */                                         \
    RADIO_CCA_ATTEMPT_MAX_DEFAULT,      /* ccaAttemptMax */       \
    RADIO_BACKOFF_EXPONENT_MIN_DEFAULT, /* backoffExponentMin */  \
    RADIO_BACKOFF_EXPONENT_MAX_DEFAULT, /* backoffExponentMax */  \
    RADIO_MINIMUM_BACKOFF_DEFAULT,      /* minimumBackoff */      \
    true  /* appendCrc */                                         \
  }

#define RADIO_TRANSMIT_CONFIG_NOCCA_DEFAULTS                      \
  {                                                               \
    true,  /* waitForAck */                                       \
    false, /* checkCca */                                         \
    RADIO_CCA_ATTEMPT_MAX_DEFAULT,      /* ccaAttemptMax */       \
    RADIO_BACKOFF_EXPONENT_MIN_DEFAULT, /* backoffExponentMin */  \
    RADIO_BACKOFF_EXPONENT_MAX_DEFAULT, /* backoffExponentMax */  \
    RADIO_MINIMUM_BACKOFF_DEFAULT,      /* minimumBackoff */      \
    true  /* appendCrc */                                         \
  }

typedef struct {
  bool waitForAck;       // Wait for ACK if ACK request set in FCF.
  bool checkCca;         // backoff and check CCA before transmit.
  uint8_t ccaAttemptMax;      // The number of CCA attempts before failure;
  uint8_t backoffExponentMin; // Backoff exponent for the initial CCA attempt.
  uint8_t backoffExponentMax; // Backoff exponent for the final CCA attempt(s).
  uint8_t minimumBackoff;     // Minimum number of backoffs which should be used
  bool appendCrc;        // Append CRC to transmitted packets.
} RadioTransmitConfig;

extern RadioTransmitConfig radioTransmitConfig;

// the mac will call this function
// -memptr must point to a phy packet.
// -memptr[0] (length byte) must be set to the value that is to be transmitted
//  over the air.
//  This function does not modify the length byte to reflect the two CRC bytes
//  that the phy appends.
EmberStatus emRadioTransmit(uint8_t* memptr);

EmberStatus emRadioPrepareTransmit(uint8_t* memptr);

// the mac will implement this function
extern void emRadioTransmitCompleteCallback(EmberStatus status,
                                            uint32_t sfdSentTime,
                                            uint8_t framePending);

extern void emRadioReceiveCompleteCallback(EmberMessageBuffer header);

// Returns true if radio needs calibrating; false otherwise.
// This function must not be called while a transmit is in progress.
bool emRadioCheckRadio(void);

// Setters and getters for radio node id and pan id.
EmberNodeId emRadioGetNodeId(void);
void emRadioSetNodeId(uint16_t nodeId);
EmberPanId emRadioGetPanId(void);
void emRadioSetPanId(uint16_t panId);

// This API returns INVALID_CHANNEL if the passed channel is invalid.
uint8_t emPhyGetPhyChannel(uint8_t macPgChan);

#if     MAC_HAS_CHANNEL_PAGES
  #define MAX_CHANNELS_PER_PAGE     27u // channels 0-26 // Must be < 32!
  #define CHANNEL_BITS              5u  // need 5 bits for 27 channels
  // Some macros for messing with single-byte-encoded MAC Page+Channel values
  #define emMacPgChanPg(macPgChan)  ((uint8_t)(macPgChan) >> CHANNEL_BITS)
  #define emMacPgChanCh(macPgChan)  ((uint8_t)(macPgChan) & (BIT(CHANNEL_BITS)-1))
  #define emMacPgChan(page, chan)   ( ((uint8_t)((page) << CHANNEL_BITS)) \
                                    | ((chan) & (BIT(CHANNEL_BITS)-1)) )
  void emPhyEnableChannelMaskEnforcement(bool enable);
  bool emPhyChannelMaskEnforcementEnabled(void);
  EmberStatus emPhySetChannelMask(uint32_t pageAndChannelMask);
  uint32_t emPhyGetChannelMask(uint8_t macPage);
  EmberStatus emPhySetChannelPageToUse(uint8_t macPage);
  uint8_t emPhyGetChannelPageInUse(void);
  uint8_t emPhyGetChannelPageForChannel(uint8_t macPgChan);
#else//!MAC_HAS_CHANNEL_PAGES
  #undef  MAC_HAS_CHANNEL_PAGES // Prevent some lame-o using #ifdef vs. #if
  #define MAX_CHANNELS_PER_PAGE     255u // channels 0-254
  #define CHANNEL_BITS              8u   // need 8 bits for 255 channels
  // Some macros for messing with single-byte-encoded MAC Page+Channel values
  #define emMacPgChanPg(macPgChan)  0
  #define emMacPgChanCh(macPgChan)  (macPgChan)
  #define emMacPgChan(page, chan)   (chan)
  #define emPhyEnableChannelMaskEnforcement(enable) /*no-op*/
  #define emPhyChannelMaskEnforcementEnabled(void)  (false)
  #define emPhySetChannelMask(pageAndChannelMask)   (EMBER_ERR_FATAL)
  #define emPhyGetChannelMask(macPage)              (0ul)
  #define emPhySetChannelPageToUse(macPage) ( (macPage) ? EMBER_ERR_FATAL \
                                                        : EMBER_SUCCESS )
  #define emPhyGetChannelPageInUse()                (0)
  #define emPhyGetChannelPageForChannel(macPgChan)  (0)
#endif//MAC_HAS_CHANNEL_PAGES
#define PHY_INVALID_FREQ_HZ 0xFFFFFFFFul
uint32_t emPhyGetChannelFreqHz(uint8_t macPgChan);

// Setters and getters for radio channel and power.
EmberStatus emSetPhyRadioChannel(uint8_t radioChannel);
uint8_t emGetPhyRadioChannel(void);
EmberStatus emSetPhyRadioPower(int8_t power);
int8_t emGetPhyRadioPower(void);
// Set floating point radio power
EmberStatus emSetPhyRadioPowerFl(int16_t power);
int16_t emGetPhyRadioPowerFl(void);

uint16_t emGetTxPowerMode(void);

uint8_t emRadioGetCca(void);

// Gets a true random number out of radios that support this.
// This will typically take a while, and so should be used to seed a PRNG
// and not as a source of random numbers for regular use.
// Returns true if the call succeeds, and false otherwise.
bool emRadioGetRandomNumbers(uint16_t *rn, uint8_t count);

// Old interface which only gets a single uint16_t.
#define emRadioGetRandomNumber(rn) \
 (emRadioGetRandomNumbers((rn), 1))

// Use the radio to generate a seed for the HAL pseudo-random number generator.
void emRadioSeedRandom(void);

#ifndef  BOOTLOADER_PHY
void emRadioEnableAddressMatching(uint8_t enable);
uint8_t emRadioAddressMatchingEnabled(void);

void emRadioEnableAutoAck(bool enable);
bool emRadioAutoAckEnabled(void);
bool emRadioHoldOffIsActive(void);
#endif// BOOTLOADER_PHY

void emRadioEnablePacketTrace(bool enable);
bool emRadioPacketTraceEnabled(void);

void emRadioEnableReceiveCrc(bool enable);
bool emRadioReceiveCrcEnabled(void);

void emStartTransmitStream(void);

void emStopTransmitStream(void);

void emStartTransmitTone(void);

void emStopTransmitTone(void);

void emMacCheckIncomingQueue(void);

// Exposed for tests
void emRadioEnableSfdRecvNotification(bool enable);
bool emRadioSfdRecvNotificationEnabled(void);
void emRadioSfdRecvIsrCallback(void);
void emRadioEnableReceiveCompleteInt(bool enable);
bool emRadioReceiveCompleteIntEnabled(void);
bool emRadioReceiveCompleteIntPending(bool acknowledgePending);

#if ( (defined(PHY_EM3XX) || defined(PHY_EFR32) || defined(PHY_RAIL))   \
    &&(!defined(BOOTLOADER_PHY))                 \
    &&(!defined(EMBER_TEST))                     \
    &&( defined(EMBER_STACK_IP))                 \
    )
  void emRadioPowerFem(bool powerUp);
#else
  #define emRadioPowerFem(powerUp) /* no-op */
#endif

#ifndef MAP_MAC_PG0_CHANNELS
#define MAP_MAC_PG0_CHANNELS ( PHY_PRO2PLUS \
                             ||(PHY_DUAL  && !PHY_THIS) \
                             ||(PHY_RAIL  && !PHY_DUAL) \
                             ||(PHY_EFR32 && !PHY_DUAL) )
#endif//MAP_MAC_PG0_CHANNELS

#if defined( PHY_EM2420 )
  #error The EM2420 is no longer supported.
#elif defined( PHY_EM2420B )
  #error The EM2420 is no longer supported.
#elif defined( PHY_EM250 )
  #include "em250/phy.h"
#elif defined( PHY_EM250B )
  #include "em250b/phy.h"
#elif defined( PHY_EM3XX )
  #include "em3xx/phy.h"
#elif defined( PHY_SI446X_US ) || defined( PHY_SI446X_EU ) || defined( PHY_SI446X_CN ) || defined( PHY_SI446X_JP )
  #include "si446x/phy.h"
#elif defined( PHY_COBRA )
  #include "cobra/phy.h"
#elif defined( PHY_PRO2PLUS )
  #include "pro2class/phy.h"
#elif defined( PHY_PRO2 ) || defined( PHY_EZR2 )
  #define PHY_PRO2PLUS 1 // So don't have to add PHY_PRO2 to many places
  #include "pro2class/phy.h"
#elif defined( PHY_EFR32 )
  #include "efr32/phy.h"
#elif defined( PHY_TRANSCEIVER_SIM )
  #include "transceiver/phy-trx-sim.h"
#elif defined ( PHY_RAIL )
  #include "rail/phy.h"
#elif !defined( PHY_NULL )
  #error No radio defined - include ember-config.h
#endif

#if     (PHY_DUAL && !PHY_THIS)
  #include "pro2class/phy.h"
#endif//(PHY_DUAL && !PHY_THIS)
#if     PHY_THIS
  #define PHY_THIS_ID (PHY_THIS - 1)
#else//!PHY_THIS
  #define PHY_THIS_ID 0
#endif//PHY_THIS

// The above per-PHY include file should define the following parameters
// (with example values shown for two representative PHY possibilities):
//                               QPSK       802.15.4            MR-FSK
// Per-PHY Parameter Name        2011       Parameter           2011-g
// ----------------------------  ---------  ------------------  ------
// EMBER_PHY_MAX_PHR_BYTES       1          aMRFSKPHRLength---> 2
// EMBER_PHY_MAX_PAYLOAD_BYTES   127        aMaxPHYPacketSize   2047
// EMBER_PHY_MAX_CRC_BYTES       2          --                  4
// EMBER_PHY_DATA_RATE_BPS       250        kb/s                100
// EMBER_PHY_BIT_TIME_US         4          (1000/data rate)    10
// EMBER_PHY_SYMBOL_BITS         4        8/phySymbolsPerOctet  1
// EMBER_PHY_PREAMBLE_BITS       32 Sym2Bit(phyPreambleSymbolLength)
// EMBER_PHY_SFD_BITS            8  Sym2Bit(phySHRDuration-phyPreambleSymbolLength)
//                          -or-          8*aMRFSKSFDLength---> 16
// EMBER_PHY_BYTE_BITS           8  Sym2Bit(phySymbolsPerOctet) 8
// EMBER_PHY_TURNAROUND_SYMBOLS  12         aTurnaroundTime     1ms / n us/sym
// EMBER_PHY_CCA_SYMBOLS         8          aCCATime            8
//                          -or- 0..1000    phyCCADuration*     0..1000
//                              *For PHYs operating in Japan 920,950MHz band
//
// The per-PHY include file can also optionally override:
// PHY_MAX_SIZE
// EMBER_PHY_SYMBOL_DURATION_US
// EMBER_PHY_ACK_TIMEOUT_SYMBOLS
// EMBER_PHY_BACKOFF_SYMBOLS

#ifndef EMBER_PHY_MAX_PAYLOAD_BYTES
#define EMBER_PHY_MAX_PAYLOAD_BYTES PHY_MAX_SIZE
#endif//EMBER_PHY_MAX_PAYLOAD_BYTES
#if     (EMBER_PHY_MAX_PAYLOAD_BYTES > PHY_MAX_SIZE)
 #warning Limiting EMBER_PHY_MAX_PAYLOAD_BYTES to PHY_MAX_SIZE
 #undef  EMBER_PHY_MAX_PAYLOAD_BYTES
 #define EMBER_PHY_MAX_PAYLOAD_BYTES PHY_MAX_SIZE
#endif//(EMBER_PHY_MAX_PAYLOAD_BYTES > PHY_MAX_SIZE)

// Convenience macros and definitions for converting per-PHY parameters.
// These should work fine for known PHYs yielding values that will fit
// within 16-bit (or larger) signed quantities.
#define EMBER_PHY_BITS_TO_BYTES(bits)     (((bits) + EMBER_PHY_BYTE_BITS - 1) / EMBER_PHY_BYTE_BITS)
#define EMBER_PHY_BITS_TO_SYMBOLS(bits)   (((bits) + EMBER_PHY_SYMBOL_BITS - 1) / EMBER_PHY_SYMBOL_BITS)
#define EMBER_PHY_BYTES_TO_SYMBOLS(bytes) (((bytes) * EMBER_PHY_BYTE_BITS) / EMBER_PHY_SYMBOL_BITS)
#define EMBER_PHY_BITS_TO_US(bits)        ((bits) * EMBER_PHY_BIT_TIME_US)
#define EMBER_PHY_BYTES_TO_US(bytes)      ((bytes) * EMBER_PHY_BITS_TO_US(EMBER_PHY_BYTE_BITS))
#define EMBER_PHY_SYMBOL_TIME_US          EMBER_PHY_BITS_TO_US(EMBER_PHY_SYMBOL_BITS)
#define EMBER_PHY_SYMBOLS_TO_US(symbols)  ((symbols) * EMBER_PHY_SYMBOL_TIME_US)
// Some PHYs separate actual Symbol 'time' from 'duration' where the former
// is actual time on the air, and the latter is a worst-case duration applied
// to CSMA, CCA, and MAC timing/timeouts.  Otherwise treat 'em the same.
#ifndef EMBER_PHY_SYMBOL_DURATION_US
#define EMBER_PHY_SYMBOL_DURATION_US      EMBER_PHY_SYMBOL_TIME_US
#endif//EMBER_PHY_SYMBOL_DURATION_US
#define EMBER_PHY_SYMBOLS_TO_DURATION_US(symbols) \
                                          ((symbols) * EMBER_PHY_SYMBOL_DURATION_US)
#define EMBER_PHY_CCA_DURATION_US         EMBER_PHY_SYMBOLS_TO_DURATION_US(EMBER_PHY_CCA_SYMBOLS)
#define EMBER_PHY_TURNAROUND_DURATION_US  EMBER_PHY_SYMBOLS_TO_DURATION_US(EMBER_PHY_TURNAROUND_SYMBOLS)
#define EMBER_PHY_ACK_TIMEOUT_DURATION_US EMBER_PHY_SYMBOLS_TO_DURATION_US(EMBER_PHY_ACK_TIMEOUT_SYMBOLS)
// 802.15.4-2011g Section 6.4.3:
// macAckWaitDuration = aUnitBackoffPeriod + aTurnaroundTime
//   + phySHRDuration + ceil((PHR+FCF+Seq#+CRC) * phySymbolsPerOctet)
// where:
// aUnitBackoffPeriod = aTurnaroundTime + aCCATime (or phyCCADuration)
// phySHRDuration     = PreambleDuration + SFDDuration
#ifndef EMBER_PHY_BACKOFF_SYMBOLS
#define EMBER_PHY_BACKOFF_SYMBOLS ( 0                          \
          + EMBER_PHY_TURNAROUND_SYMBOLS                            \
          + EMBER_PHY_CCA_SYMBOLS                                   \
          )
#endif//EMBER_PHY_BACKOFF_SYMBOLS
#ifndef EMBER_PHY_ACK_TIMEOUT_SYMBOLS
#define EMBER_PHY_ACK_TIMEOUT_SYMBOLS ( 0                           \
          + EMBER_PHY_BACKOFF_SYMBOLS                          \
          + EMBER_PHY_TURNAROUND_SYMBOLS                            \
          + EMBER_PHY_BITS_TO_SYMBOLS(EMBER_PHY_PREAMBLE_BITS)      \
          + EMBER_PHY_BITS_TO_SYMBOLS(EMBER_PHY_SFD_BITS)           \
          + ((EMBER_PHY_MAX_PHR_BYTES + 3 + EMBER_PHY_MAX_CRC_BYTES)\
             * EMBER_PHY_BITS_TO_SYMBOLS(EMBER_PHY_BYTE_BITS))      \
          )
#endif//EMBER_PHY_ACK_TIMEOUT_SYMBOLS

#ifdef EMBER_TEST
bool _radioReceive(uint8_t *packet, uint32_t rxSynctime, uint8_t linkQuality);
void _radioTransmitComplete(void);
#endif

#ifndef EMBER_STACK_IP
// On the ZIP stack all ember... / mfglib... APIs are renamed to
// emApi... / mfglibApi... due to RTOS namespace conflicts.
// This change doesn't apply to ZNET.
  #define emApiSetTxPowerMode emberSetTxPowerMode
  #define mfglibApiStart mfglibStart
  #define mfglibApiEnd mfglibEnd
  #define mfglibApiStartTone mfglibStartTone
  #define mfglibApiStopTone mfglibStopTone
  #define mfglibApiStartStream mfglibStartStream
  #define mfglibApiStopStream mfglibStopStream
  #define mfglibApiSendPacket mfglibSendPacket
  #define mfglibApiSetChannel mfglibSetChannel
  #define mfglibApiGetChannel mfglibGetChannel
  #define mfglibApiSetPower mfglibSetPower
  #define mfglibApiGetPower mfglibGetPower
  #define mfglibApiGetPowerMode mfglibGetPowerMode
  #define mfglibApiSetSynOffset mfglibSetSynOffset
  #define mfglibApiGetSynOffset mfglibGetSynOffset
  #define mfglibApiTestContModCal mfglibTestContModCal
  #define mfglibApiSetOptions mfglibSetOptions
  #define mfglibApiGetOptions mfglibGetOptions
#endif

// Map some internally-consistent names with less consistent API naming
#define emPhyGetRadioChannel     emGetPhyRadioChannel
#define emPhyGetRadioPower       emGetPhyRadioPower
#define emPhySetRadioChannel     emSetPhyRadioChannel
#define emPhySetRadioPower       emSetPhyRadioPower
#define emPhyStartTransmitStream emStartTransmitStream
#define emPhyStartTransmitTone   emStartTransmitTone
#define emPhyStopTransmitStream  emStopTransmitStream
#define emPhyStopTransmitTone    emStopTransmitTone

#ifdef  CORTEXM3_EFR32_MICRO
  // Provide some em3xx-like compatibility macros for certain test code
  // for converting microseconds to emGetMacTimer()'s ticks.
  #define MAC_TIMER (emGetMacTimer())
 #ifdef  PHY_RAIL
  // The RAIL timer ticks every 1us.
  #define MAC_TIMER_US(ticks)  (ticks)
  #define MAC_TIMER_TICKS(microseconds) (microseconds)
  #define MAC_TIMER_MAC_TIMER_MASK 0xFFFFFFFFul
 #else//!PHY_RAIL
  // The PROTIMER ticks every 2us.
  #define MAC_TIMER_US(ticks)  ((ticks) * 2)
  #define MAC_TIMER_TICKS(microseconds) (((microseconds) + 1) / 2)
  #define MAC_TIMER_MAC_TIMER_MASK (PROTIMER->WRAPCNTTOP)
 #endif//PHY_RAIL
#endif//CORTEXM3_EFR32_MICRO

#endif //__PHY_H__
