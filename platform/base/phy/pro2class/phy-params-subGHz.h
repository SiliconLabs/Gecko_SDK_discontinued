/*
 * File: phy/pro2class/phy-params-868MHz.h
 * Description: EMBER_PHY parameters for 868 MHz on PRO2+
 *
 * Copyright 2014 Silicon Laboratories, Inc.                                *80*
 */

#ifndef __PHY_PARAMS_868MHZ_H__
#define __PHY_PARAMS_868MHZ_H__

#ifndef EMBER_PHY_USE_PHR_154G
#define EMBER_PHY_USE_PHR_154G   ( (defined(PHY_PRO2PLUS)) \
                                 &&( (defined(CORTEXM3_EMBER_MICRO)) \
                                   ||( (defined(CORTEXM3_EFR32_MICRO)) \
                                     &&(!defined(RADIO_BOARD)) ) ) )
#endif//EMBER_PHY_USE_PHR_154G
#ifndef EMBER_PHY_DATA_WHITENING
#define EMBER_PHY_DATA_WHITENING EMBER_PHY_USE_PHR_154G
#endif//EMBER_PHY_DATA_WHITENING

// Basic timing parameters for 802.15.4 MR-FSK 868 MHz
#undef  EMBER_PHY_MAX_PHR_BYTES
#if     EMBER_PHY_USE_PHR_154G
#define EMBER_PHY_MAX_PHR_BYTES      2        // 802.15.4g-2012
#else//!EMBER_PHY_USE_PHR_154G
#define EMBER_PHY_MAX_PHR_BYTES      1        // 802.15.4-2003/2006/2011
#endif//EMBER_PHY_USE_PHR_154G
#undef  EMBER_PHY_MAX_PAYLOAD_BYTES
#define EMBER_PHY_MAX_PAYLOAD_BYTES  PHY_MAX_SIZE // What upper layers support
#undef  EMBER_PHY_MAX_CRC_BYTES
#define EMBER_PHY_MAX_CRC_BYTES      2        // 802.15.4-2003/2006/2011
#undef  EMBER_PHY_DATA_RATE_BPS
#define EMBER_PHY_DATA_RATE_BPS      (EM_PHY_CURRENT_BAND_CONFIG->dataRateBps)
#undef  EMBER_PHY_BIT_TIME_US
#define EMBER_PHY_BIT_TIME_US        (1000000ul / EMBER_PHY_DATA_RATE_BPS)
#undef  EMBER_PHY_SYMBOL_BITS
#define EMBER_PHY_SYMBOL_BITS        (EM_PHY_CURRENT_BAND_CONFIG->symbolBits)
#undef  EMBER_PHY_PREAMBLE_BITS
#define EMBER_PHY_PREAMBLE_BITS      (EM_PHY_CURRENT_BAND_CONFIG->preambleBits)
#undef  EMBER_PHY_SFD_BITS
#define EMBER_PHY_SFD_BITS           (EM_PHY_CURRENT_BAND_CONFIG->sfdBits)
#undef  EMBER_PHY_BYTE_BITS
#define EMBER_PHY_BYTE_BITS          8        // BYTE_TIME_US       =   80 us
#undef  EMBER_PHY_RAMP_TIME_BITS
#define EMBER_PHY_RAMP_TIME_BITS     1        // Ramp up or down time, ~10us
#undef  EMBER_PHY_RAMP_SETTLE_BITS
#define EMBER_PHY_RAMP_SETTLE_BITS   4        // Settle after ramp up, ~40us

#undef  EMBER_PHY_TURNAROUND_SYMBOLS
#if     EMBER_PHY_USE_PHR_154G
// 802.15.4g-2012 requires that aTurnaroundTime, macAckWaitDuration, and
// aCCATime be calculated using the slowest operating mode's symbol time,
// which we call SYMBOL_DURATION to distinguish it from actual SYMBOL_TIME.
// Confusing? Yes, but reading 802.15.4g-2012 will confuse you moreso!
// (equated implicitly by upper phy.h by not specifying it here).
//#define EMBER_PHY_SYMBOL_DURATION_US 20     // Must use slower 50 kbps rate
//#define EMBER_PHY_TURNAROUND_SYMBOLS (1000 / EMBER_PHY_SYMBOL_DURATION_US) // 100
//                                            // TURNAROUND_TIME_US = 1000 us
// ZigBee GB868 ignores 15.4G and R22 specifies turnaround time of 50 symbols.
#define EMBER_PHY_TURNAROUND_SYMBOLS 50       // TURNAROUND_TIME_US =  500 us
#else//!EMBER_PHY_USE_PHR_154G
#define EMBER_PHY_TURNAROUND_SYMBOLS 12       // TURNAROUND_TIME_US =  120 us
#endif//EMBER_PHY_USE_PHR_154G
#undef  EMBER_PHY_CCA_SYMBOLS
#define EMBER_PHY_CCA_SYMBOLS        8        // CCA_TIME_US        =   80 us
//*Override these calculated ones in outer phy.h*
#undef  EMBER_PHY_ACK_TIMEOUT_SYMBOLS
// ZigBee GB868 R22 specifies 100 symbols
//#define EMBER_PHY_ACK_TIMEOUT_SYMBOLS 100     //*ACK_TIMEOUT_US     = 1000 us
//TODO: Fix this per spec someday; for now be rather permissive and forgiving
#define EMBER_PHY_ACK_TIMEOUT_SYMBOLS 255     //*ACK_TIMEOUT_US     = 2550 us
#undef  EMBER_PHY_MAX_FRAME_RESPONSE_MULTIPLIER
#define EMBER_PHY_MAX_FRAME_RESPONSE_MULTIPLIER 10  //TODO: Tune this; default is 1

// In SubGHz, channels span several sub-bands so be lenient about min/max.
// Unfortunately EMBER_MIN_802_15_4_CHANNEL_NUMBER has also come to mean
// the Default channel selected on bootup, so must deal with that annoyance.
#if defined(EMBER_NO_STACK) || defined(EMBER_STACK_CONNECT)
#undef  EMBER_MIN_802_15_4_CHANNEL_NUMBER
#define EMBER_MIN_802_15_4_CHANNEL_NUMBER 0 // 0 for RAM-Radio Config
#else//!EMBER_NO_STACK
#if     (MAP_MAC_PG0_CHANNELS || PHY_DUAL || CORTEXM3_EFR32_MICRO)
//TODO: Leave EMBER_MIN_802_15_4_CHANNEL_NUMBER as GHz default of 11
#else//!(MAP_MAC_PG0_CHANNELS || PHY_DUAL || CORTEXM3_EFR32_MICRO)
#undef  EMBER_MIN_802_15_4_CHANNEL_NUMBER
#define EMBER_MIN_802_15_4_CHANNEL_NUMBER 30
#endif//(MAP_MAC_PG0_CHANNELS || PHY_DUAL || CORTEXM3_EFR32_MICRO)
#endif//EMBER_NO_STACK
#undef  EMBER_MAX_802_15_4_CHANNEL_NUMBER
#define EMBER_MAX_802_15_4_CHANNEL_NUMBER 254 // 255 reserved for 'invalid'

#endif//__PHY_PARAMS_868MHZ_H__
