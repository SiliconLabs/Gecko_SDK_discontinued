/*! @file radio_config.h
 * @brief This file contains the automatically generated
 * configurations.
 *
 * @n WDS GUI Version: 3.2.6.17
 * @n Device: Si4468 Rev.: C1                                 
 *
 * @b COPYRIGHT
 * @n Silicon Laboratories Confidential
 * @n Copyright 2014 Silicon Laboratories, Inc.
 * @n http://www.silabs.com
 */

#ifndef PHY_CONFIG_BRD4502_H_
#define PHY_CONFIG_BRD4502_H_

#include "radio_config_BRD4505A_PHY0.h"

// CONFIGURATION PARAMETERS
#define RADIO_CONFIGURATION_DATA_PHY_NAME                           "PRO2+ 490 MHz"
#define RADIO_CONFIGURATION_DATA_BAND_NAME                          "490 MHz"
#define RADIO_CONFIGURATION_DATA_BASE_FREQ_HZ                       490000000ul
#define RADIO_CONFIGURATION_DATA_CHANNEL_FREQ_HZ                    200000ul
#define RADIO_CONFIGURATION_DATA_MAX_CHANNEL                        (33 - 1)     // -1 due to Ch #0. Channels are 0 - 32

#define RADIO_CONFIGURATION_DATA_RATE_BPS                           10000
#define RADIO_CONFIGURATION_SYMBOL_BITS                             1
#define RADIO_CONFIGURATION_PREAMBLE_BITS                           64
#define RADIO_CONFIGURATION_SYNC_BITS                               16

#define RADIO_CONFIGURATION_DATA_RSSI_DBM_OFFSET                    130     // From Si446x data sheet and used here too -----vvv

// SubBandConfig
#define RADIO_CONFIGURATION_DATA_SUBBAND_CONFIG     { \
    { \
      /*minPhyChan     */   RADIO_CONFIGURATION_DATA_CHANNEL_NUMBER, \
      /*maxPhyChan     */   RADIO_CONFIGURATION_DATA_MAX_CHANNEL + \
                            RADIO_CONFIGURATION_DATA_CHANNEL_NUMBER, \
      /*txMinPowerdBm  */ -35, \
      /*txMaxPowerdBm  */  20, \
      /*edThresholddBm */ -75, \
      /*lbtDutyDenom   */   1, \
      /*criDutyDenom   */   1, \
      /*nonDutyDenom   */   1, \
      /*subBandParams  */ NULL, \
      /*ccaConfigArray */ emPhyCcaCsmaConfigArray, \
    } \
  }

// PA
#define RADIO_CONFIGURATION_DATA_PA_1   { \
    /*paName        */ "+20dBm Coarse", \
    /*paSelEnum     */ 2, /* PROP_PA_MODE_PA_SEL_ENUM_HP_COARSE */ \
    /*numPowerLevels*/ sizeof(emPhyTxPaLevels)/sizeof(*emPhyTxPaLevels), \
    /*powerLevels   */ emPhyTxPaLevels \
  }

#define RADIO_CONFIGURATION_DATA_PAS    { \
    RADIO_CONFIGURATION_DATA_PA_1, \
  }

// PA levels
// From Si4467_C2_868MHz_OutPwr.xlsm:PTS_PwrVsDdac_868_4467 3.3V 25C
#define RADIO_CONFIGURATION_DATA_PA_LEVELS   { \
    /* dBm, PA_PWR_LVL */ \
    { -35,   0 }, \
    { -20,   1 }, \
    { -12,   2 }, \
    {  -8,   3 }, \
    {  -5,   4 }, \
    {  -1,   6 }, \
    {   2,   8 }, \
    {   4,  10 }, \
    {   6,  12 }, \
    {   8,  16 }, \
    {  10,  19 }, \
    {  12,  24 }, \
    {  14,  32 }, \
    {  16,  40 }, \
    {  17,  48 }, \
    {  18,  56 }, \
    {  19,  64 }, \
    {  20, 127 }, \
  }

// From the WDS3-generated configuration file, we only really care about
// certain RF_GLOBAL, RF_PREAMBLE, RF_SYNC, RF_MODEM, RF_SYNTH and RF_FREQ
// properties.  PRO2 mode allows MATCH and PKT while IE154 doesn't.
// The rest we handle ourselves internally or are not applicable.
#ifdef  PHY_PRO2
#define PRO2_NON_COMMON_CONFIG \
          RF_PKT_CRC_CONFIG_1, \
          RF_PKT_WHT_SEED_15_8_4, \
          RF_PKT_LEN_12, \
          RF_PKT_FIELD_2_CRC_CONFIG_12, \
          RF_PKT_FIELD_5_CRC_CONFIG_12, \
          RF_PKT_RX_FIELD_3_CRC_CONFIG_9, \
          RF_PKT_CRC_SEED_31_24_4, \
          RF_MATCH_VALUE_1_12, \
          /* Please don't remove this comment! */
#else//!PHY_PRO2
#define PRO2_NON_COMMON_CONFIG \
          /* Please don't remove this comment! */
#endif//PHY_PRO2

#undef  RADIO_CONFIGURATION_DATA_ARRAY
#define RADIO_CONFIGURATION_DATA_ARRAY { \
          RF_GLOBAL_XO_TUNE_2, \
          RF_GLOBAL_CONFIG_1, \
          RF_PREAMBLE_TX_LENGTH_9, \
          RF_SYNC_CONFIG_6, \
          RF_MODEM_MOD_TYPE_12, \
          RF_MODEM_FREQ_DEV_0_1, \
          RF_MODEM_TX_RAMP_DELAY_12, \
          RF_MODEM_BCR_NCO_OFFSET_2_12, \
          RF_MODEM_AFC_LIMITER_1_3, \
          RF_MODEM_AGC_CONTROL_1, \
          RF_MODEM_AGC_WINDOW_SIZE_12, \
          RF_MODEM_RAW_CONTROL_8, \
          RF_MODEM_RSSI_CONTROL_3, \
          RF_MODEM_RAW_SEARCH2_2, \
          RF_MODEM_SPIKE_DET_2, \
          RF_MODEM_RSSI_MUTE_1, \
          RF_MODEM_DSA_CTRL1_5, \
          RF_MODEM_CHFLT_RX1_CHFLT_COE13_7_0_12, \
          RF_MODEM_CHFLT_RX1_CHFLT_COE1_7_0_12, \
          RF_MODEM_CHFLT_RX2_CHFLT_COE7_7_0_12, \
          RF_PA_MODE_4, \
          RF_SYNTH_PFDCP_CPFF_7, \
          RF_FREQ_CONTROL_INTE_8, \
          PRO2_NON_COMMON_CONFIG /*per-PHY are penultimate*/ \
          0x00 \
        }

#endif /* PHY_CONFIG_BRD4502_H_ */
