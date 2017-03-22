/* File: phy/pro2class/phy-config-connect.c
 * Description: MHz PHY on PRO2+
 *
 * Copyright 2014 Silicon Laboratories, Inc.                                *80*
 */

#include BOARD_HEADER

#ifndef CONNECT_PHY_OPTION
#define CONNECT_PHY_OPTION  0
#endif

#define DEFINE_EZR32_RADIO_BOARDS "hal/micro/cortexm3/efm32/board/ezr32-radio-boards.h"
#define UNDEF_EZR32_RADIO_BOARDS  "hal/micro/cortexm3/efm32/board/ezr32-radio-boards-undef.h"

#include DEFINE_EZR32_RADIO_BOARDS 

#include PLATFORM_HEADER
#include "include/ember-types.h"
#include "phy/phy.h"

#define stringify(x) #x
#define PHY_INC2(board, phy) stringify(ezr32/connect/phy_config_ ## board ## _PHY ## phy ## .h)
#define PHY_INC(board, phy) PHY_INC2(board, phy)

// Undefine all radio boards, so stringify can make its job
#include UNDEF_EZR32_RADIO_BOARDS 

// Include the configuration file of the corresponding board
#include PHY_INC(RADIO_BOARD, CONNECT_PHY_OPTION)

// Add back the board configurations
#include DEFINE_EZR32_RADIO_BOARDS 


#if defined(PHY_EZR2)

//-----------------------------------------------------------------------------
// SubGig PHY on EZR2+
//-----------------------------------------------------------------------------

// FIXME: These are valid only for BRD4502B!
static const EmPhySubBandConfig subBandConfigs[] = {
  { //
    /*minPhyChan     */   0,
    /*maxPhyChan     */  RADIO_CONFIGURATION_DATA_MAX_CHANNEL,
    /*txMinPowerdBm  */ -43,
    /*txMaxPowerdBm  */  10,
    /*edThresholddBm */ RADIO_CONFIGURATION_DATA_ED_DBM_THRESHOLD,
    /*lbtDutyDenom   */   1, // 100%
    /*criDutyDenom   */   1, // 100%
    /*nonDutyDenom10 */   0, // 100%
    /*subBandParams  */ NULL,
    /*ccaConfigArray */ emPhyCcaCsmaConfigArray,
  },
};

static const EmPhyTxPowerLevel emPhyTxPaLevels[]    = RADIO_CONFIGURATION_DATA_PA_LEVELS;
static const EmPhyTxPA emPhyTxPAs[]                 = RADIO_CONFIGURATION_DATA_PAS;

static const uint8_t radioConfigPart1[] = EZCONFIG_DATA_PART1;
static const uint8_t radioConfigPart2[] = EZCONFIG_DATA_PART2;

static const EzConfig radioConfigTRX = {
    /*sizePart1      */   sizeof(radioConfigPart1),
    /*configPart1    */   (uint8_t *) radioConfigPart1,
    /*sizePart2      */   sizeof(radioConfigPart2),
    /*configPart2    */   (uint8_t *) radioConfigPart2,
    /*configCRC      */   EZCONFIG_CHECKSUM,
};

#ifdef EMBER_NO_STACK
#undef EZCONFIG_DATA_PART1
#undef EZCONFIG_DATA_PART2
#undef EZCONFIG_CHECKSUM

#include "ezr32/radio_config_Si4455_CW_434MHz.h"

static const uint8_t radioConfigPart1CW[] = EZCONFIG_DATA_PART1;
static const uint8_t radioConfigPart2CW[] = EZCONFIG_DATA_PART2;

static const EzConfig radioConfigCW = {
    /*sizePart1      */   sizeof(radioConfigPart1CW),
    /*configPart1    */   (uint8_t *) radioConfigPart1CW,
    /*sizePart2      */   sizeof(radioConfigPart2CW),
    /*configPart2    */   (uint8_t *) radioConfigPart2CW,
    /*configCRC      */   EZCONFIG_CHECKSUM,
};

#undef EZCONFIG_DATA_PART1
#undef EZCONFIG_DATA_PART2
#undef EZCONFIG_CHECKSUM

#include "ezr32/radio_config_Si4455_PN9_434MHz.h"

static const uint8_t radioConfigPart1PN9[] = EZCONFIG_DATA_PART1;
static const uint8_t radioConfigPart2PN9[] = EZCONFIG_DATA_PART2;

static const EzConfig radioConfigPN9 = {
    /*sizePart1      */   sizeof(radioConfigPart1PN9),
    /*configPart1    */   (uint8_t *) radioConfigPart1PN9,
    /*sizePart2      */   sizeof(radioConfigPart2PN9),
    /*configPart2    */   (uint8_t *) radioConfigPart2PN9,
    /*configCRC      */   EZCONFIG_CHECKSUM,
};
#endif

static const EzConfigs radioConfigs = {
    /*configTRX;     */   (EzConfig *)&radioConfigTRX,
#ifdef EMBER_NO_STACK
    /*configCW;      */   (EzConfig *)&radioConfigCW,
    /*configPN9;     */   (EzConfig *)&radioConfigPN9,
#else
    /*configCW;      */   (EzConfig *) NULL,
    /*configPN9;     */   (EzConfig *) NULL,
#endif
};

const EmPhyBandConfig emPhyBandConfig = {
  /*minPhyChan         */  0,
  /*maxPhyChan         */ RADIO_CONFIGURATION_DATA_MAX_CHANNEL,
  /*pad[]              */ {       0, },
  /*bandName           */ RADIO_CONFIGURATION_DATA_BAND_NAME,
  /*baseFreqHz         */ RADIO_CONFIGURATION_DATA_BASE_FREQ_HZ,
  /*chanFreqHz         */ RADIO_CONFIGURATION_DATA_CHANNEL_FREQ_HZ,
  /*radioXoFreq        */ RADIO_CONFIGURATION_DATA_RADIO_XO_FREQ,
  /*dataRateBps        */ RADIO_CONFIGURATION_DATA_RATE_BPS,
  /*symbolBits         */ RADIO_CONFIGURATION_SYMBOL_BITS,
  /*preambleBits       */ RADIO_CONFIGURATION_PREAMBLE_BITS,
  /*sfdBits            */ RADIO_CONFIGURATION_SYNC_BITS,
  /*numSubBandConfigs  */ sizeof(subBandConfigs)/sizeof(*subBandConfigs),
  /*subBandConfigs     */ subBandConfigs,
  /*radioConfigArray   */ &radioConfigs,
};

static const EmPhyBandConfig* const emPhyBands[] = {
  &emPhyBandConfig,
};

const EmPhyConfig emPhyConfigMHz = {
  /*phyName       */ RADIO_CONFIGURATION_DATA_PHY_NAME,
  /*rssidBmOffset */ RADIO_CONFIGURATION_DATA_RSSI_DBM_OFFSET,
  /*pad[]         */ { 0, },
  /*numBands      */ sizeof(emPhyBands)/sizeof(*emPhyBands),
  /*numPAs        */ sizeof(emPhyTxPAs)/sizeof(*emPhyTxPAs),
  /*bandConfigs   */ emPhyBands,
  /*PAs           */ emPhyTxPAs,
};

#elif defined(PHY_PRO2) || defined(PHY_PRO2PLUS)

//-----------------------------------------------------------------------------
// SubGig PHY on PRO2+
//-----------------------------------------------------------------------------

static const uint8_t radioConfigArray[]             = RADIO_CONFIGURATION_DATA_ARRAY;
static const EmPhySubBandConfig subBandConfigs[]    = RADIO_CONFIGURATION_DATA_SUBBAND_CONFIG;

static const EmPhyTxPowerLevel emPhyTxPaLevels[]    = RADIO_CONFIGURATION_DATA_PA_LEVELS;
static const EmPhyTxPA emPhyTxPAs[]                 = RADIO_CONFIGURATION_DATA_PAS;

const EmPhyBandConfig emPhyBandConfig = {
  /*minPhyChan         */ RADIO_CONFIGURATION_DATA_CHANNEL_NUMBER,
  /*maxPhyChan         */ RADIO_CONFIGURATION_DATA_CHANNEL_NUMBER +
                          RADIO_CONFIGURATION_DATA_MAX_CHANNEL,
  /*pad[]              */ {       0, },
  /*bandName           */ RADIO_CONFIGURATION_DATA_BAND_NAME,
  /*baseFreqHz         */ RADIO_CONFIGURATION_DATA_BASE_FREQ_HZ,
  /*chanFreqHz         */ RADIO_CONFIGURATION_DATA_CHANNEL_FREQ_HZ,
  /*radioXoFreq        */ RADIO_CONFIGURATION_DATA_RADIO_XO_FREQ,
  /*dataRateBps        */ RADIO_CONFIGURATION_DATA_RATE_BPS,
  /*symbolBits         */ RADIO_CONFIGURATION_SYMBOL_BITS,
  /*preambleBits       */ RADIO_CONFIGURATION_PREAMBLE_BITS,
  /*sfdBits            */ RADIO_CONFIGURATION_SYNC_BITS,
  /*numSubBandConfigs  */ sizeof(subBandConfigs)/sizeof(*subBandConfigs),
  /*subBandConfigs     */ subBandConfigs,
  /*radioConfigArray   */ radioConfigArray,
};

static const EmPhyBandConfig* const emPhyBands[] = {
  &emPhyBandConfig,
};

const EmPhyConfig emPhyConfigMHz = {
  /*phyName       */ RADIO_CONFIGURATION_DATA_PHY_NAME,
  /*rssidBmOffset */ RADIO_CONFIGURATION_DATA_RSSI_DBM_OFFSET,
  /*pad[]         */ { 0, },
  /*numBands      */ sizeof(emPhyBands)/sizeof(*emPhyBands),
  /*numPAs        */ sizeof(emPhyTxPAs)/sizeof(*emPhyTxPAs),
  /*bandConfigs   */ emPhyBands,
  /*PAs           */ emPhyTxPAs,
};

#else
 #error Missing PHY macro definition!
#endif
