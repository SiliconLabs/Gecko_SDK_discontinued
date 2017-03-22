
#ifndef __RADIO_CONFIG_H__
#define __RADIO_CONFIG_H__

// USER DEFINED PARAMETERS
// Define your own parameters here

// INPUT DATA
/*
// Crys_freq(Hz): 30000000    Crys_tol(ppm): 20    IF_mode: 2    High_perf_Ch_Fil: 1    OSRtune: 0    Ch_Fil_Bw_AFC: 0    ANT_DIV: 0    PM_pattern: 15
// MOD_type: 3    Rsymb(sps): 200000    Fdev(Hz): 100000    RXBW(Hz): 114000    Manchester: 0    AFC_en: 1    Rsymb_error: 0.0    Chip-Version: 2
// RF Freq.(MHz): 434    API_TC: 28    fhst: 500000    inputBW: 0    BERT: 0    RAW_dout: 0    D_source: 0    Hi_pfm_div: 0
// API_ARR_Det_en: 0    Fdev_error: 0    API_ETSI: 0
//
// # WB filter 2 (BW = 412.25 kHz);  NB-filter 2 (BW = 412.25 kHz)

//
// Modulation index: 1
*/


// CONFIGURATION PARAMETERS
#define RADIO_CONFIGURATION_DATA_PHY_NAME                           "EZR2 434 MHz"
#define RADIO_CONFIGURATION_DATA_BAND_NAME                          "434 MHz"
#define RADIO_CONFIGURATION_DATA_BASE_FREQ_HZ                       434000000ul
#define RADIO_CONFIGURATION_DATA_CHANNEL_FREQ_HZ                    500000ul
#define RADIO_CONFIGURATION_DATA_MAX_CHANNEL                        (1 - 1)     // -1 due to Ch #0. Channels are 0 - 0

#define RADIO_CONFIGURATION_DATA_RATE_BPS                           200000
#define RADIO_CONFIGURATION_SYMBOL_BITS                             1
#define RADIO_CONFIGURATION_PREAMBLE_BITS                           32
#define RADIO_CONFIGURATION_SYNC_BITS                               16

#define RADIO_CONFIGURATION_DATA_RSSI_DBM_OFFSET                    130     // From Si446x data sheet and used here too -----vvv
#define RADIO_CONFIGURATION_DATA_ED_DBM_THRESHOLD                   -75     // -90: dBm=(PROP_IE154_TX_CCA_THRESHOLD_DEFAULT/2) - 130

#define RADIO_CONFIGURATION_DATA_RADIO_XO_FREQ                     30000000L
#define RADIO_CONFIGURATION_DATA_CHANNEL_NUMBER                    0x00


#define EZCONFIG_DATA_PART1 { \
0x98, 0x5B, 0x55, 0x9A, 0x22, 0xA6, 0x18, 0x8E, 0x83, 0x2F, 0xC1, 0xAC, \
0x92, 0xD1, 0x9D, 0x0B, 0x86, 0x22, 0xCE, 0xDF, 0x13, 0x9F, 0xC6, 0x85, \
0x4D, 0x3B, 0x11, 0xF2, 0x9A, 0xAC, 0x9B, 0x1E, 0xE5, 0x18, 0xCA, 0x4C, \
0xA6, 0x51, 0x1C, 0xC7, 0x21, 0x11, 0x82, 0xAC, 0xA1, 0xC0, 0x75, 0x08, \
0x0B, 0x07, 0x00, 0x97, 0x0A, 0x51, 0xE6, 0x33, 0xA5, 0x9A, 0x6C, 0x73, \
0xB0, 0xC7, 0x01, 0x24, 0x59, 0xE5, 0xDF, 0x73, 0x49, 0x17, 0xC2, 0x51, \
0xCF, 0x44, 0x95, 0x8A, 0x4F, 0x85, 0x95, 0xBD, 0xF4, 0xB1, 0xB0, 0xA2, \
0xDE, 0x48, 0xD0, 0xF1, 0x32, 0xAC, 0x96, 0x0E, 0x96, 0xB9, 0x90, 0xED, \
0x7A, 0x13, 0xC1, 0x95, 0x0E, 0x29, 0x8E, 0x5F, 0x07, 0x22, 0x87, 0x7A, \
0x00, 0xDC, 0xD7, 0x71, 0x79, 0x86, 0xA3, 0x18, 0xF2, 0xA4, 0xD4, 0xFD }

#define EZCONFIG_DATA_PART2 { \
0x7A, 0x38, 0xB2, 0x69, 0x5A, 0xAC, 0x02, 0x08, 0x58, 0x5C, 0x3A, 0x9C, \
0x2F, 0x60, 0x86, 0xFA, 0xEB, 0x59, 0x90, 0x15, 0xEA, 0x11, 0x2C, 0x03, \
0x43, 0x98, 0xEB, 0x63, 0xF2, 0xAB, 0xF4, 0x72, 0x1F, 0xA0, 0xA4, 0x3A, \
0xB6, 0x23, 0x5D, 0x69, 0x75, 0x7E, 0xBD, 0xC6, 0xE6, 0x4E, 0xCC, 0xA9, \
0xAA, 0x0E, 0xFA, 0x9E, 0xA9, 0xBC, 0x56, 0xD0, 0xE5, 0xBC, 0xFD, 0x2F, \
0x1D, 0xA2, 0xAA, 0x97, 0x70, 0xDC, 0xC6, 0x86, 0x77, 0x29, 0xE5, 0x37, \
0x17, 0xCE, 0xFE, 0x00, 0xFA, 0x18, 0x62, 0xBF, 0x35, 0x32, 0x30, 0xB8, \
0xEC, 0xA9, 0x76, 0x60, 0xE6, 0x13, 0x56, 0x32, 0xA3, 0x68, 0x67, 0x82, \
0xA2, 0xC8, 0x5D, 0xD7, 0xC1, 0x7F, 0xAC, 0x46 }

#define EZCONFIG_CHECKSUM 0xC538

// PA
#define RADIO_CONFIGURATION_DATA_PA_1   { \
    /*paName        */ "+10dBm", \
    /*paSelEnum     */ 6, /* PROP_PA_MODE_PA_SEL_ENUM_LP */ \
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
    { -43,   0 }, \
    { -18,   1 }, \
    { -12,   2 }, \
    {  -8,   3 }, \
    {  -6,   4 }, \
    {  -4,   5 }, \
    {  -2,   6 }, \
    {  -1,   7 }, \
    {   0,   8 }, \
    {   1,   9 }, \
    {   2,  10 }, \
    {   3,  11 }, \
    {   4,  13 }, \
    {   5,  16 }, \
    {   6,  18 }, \
    {   7,  20 }, \
    {   8,  22 }, \
    {   9,  26 }, \
    {  10,  30 }, \
    {  11,  36 }, \
    {  12,  45 }, \
    {  13,  61 }, \
    {  14,  79 }, \
    /* PA_PWR_LVL values above 79 wrap in some weird way, so avoid them! */ \
    /* The default PA_PWR_LVL of 127 should end up being same as 79. */ \
  }


//PREAMBLE_TX_LENGTH                                0x04
//PREAMBLE_CONFIG_STD_1                             0x14
//PREAMBLE_CONFIG_NSTD                              0x00
//PREAMBLE_CONFIG_STD_2                             0x0F
//PREAMBLE_CONFIG                                   0x31
//PREAMBLE_PATTERN_31_24                            0x00
//PREAMBLE_PATTERN_23_16                            0x00
//PREAMBLE_PATTERN_15_8                             0x00
//PREAMBLE_PATTERN_7_0                              0x00
//SYNC_CONFIG                                       0x01
//SYNC_BITS_31_24                                   0x67
//SYNC_BITS_23_16                                   0x0B
//SYNC_BITS_15_8                                    0x00
//SYNC_BITS_7_0                                     0x00
//MODEM_MOD_TYPE                                    0x03
//MODEM_MAP_CONTROL                                 0x00
//MODEM_DSM_CTRL                                    0x07
//MODEM_DATA_RATE_2                                 0x3D
//MODEM_DATA_RATE_1                                 0x09
//MODEM_DATA_RATE_0                                 0x00
//MODEM_TX_NCO_MODE_3                               0x09
//MODEM_TX_NCO_MODE_2                               0xC9
//MODEM_TX_NCO_MODE_1                               0xC3
//MODEM_TX_NCO_MODE_0                               0x80
//MODEM_FREQ_DEV_2                                  0x00
//MODEM_FREQ_DEV_1                                  0x0D
//MODEM_FREQ_DEV_0                                  0xA7
//MODEM_FREQ_OFFSET_1                               0x00
//MODEM_FREQ_OFFSET_0                               0x00
//MODEM_TX_RAMP_DELAY                               0x01
//MODEM_MDM_CTRL                                    0x00
//MODEM_IF_CONTROL                                  0x00
//MODEM_IF_FREQ_2                                   0x03
//MODEM_IF_FREQ_1                                   0xE0
//MODEM_IF_FREQ_0                                   0x00
//MODEM_DECIMATION_CFG1                             0x00
//MODEM_DECIMATION_CFG0                             0x20
//MODEM_RESERVED_20_20/MODEM_DECIMATION_CFG2        0x00
//MODEM_RESERVED_20_21/MODEM_IFPKD_THRESHOLDS       0x00
//MODEM_BCR_OSR_1                                   0x00
//MODEM_BCR_OSR_0                                   0x4B
//MODEM_BCR_NCO_OFFSET_2                            0x06
//MODEM_BCR_NCO_OFFSET_1                            0xD3
//MODEM_BCR_NCO_OFFSET_0                            0xA0
//MODEM_BCR_GAIN_1                                  0x06
//MODEM_BCR_GAIN_0                                  0xD4
//MODEM_BCR_GEAR                                    0x02
//MODEM_BCR_MISC1                                   0x00
//MODEM_BCR_MISC0                                   0x00
//MODEM_AFC_GEAR                                    0x00
//MODEM_AFC_WAIT                                    0x23
//MODEM_AFC_GAIN_1                                  0x86
//MODEM_AFC_GAIN_0                                  0xD4
//MODEM_AFC_LIMITER_1                               0x00
//MODEM_AFC_LIMITER_0                               0xD3
//MODEM_AFC_MISC                                    0xA0
//MODEM_AFC_ZIFOFF                                  0x00
//MODEM_ADC_CTRL                                    0x00
//MODEM_AGC_CONTROL                                 0xE2
//MODEM_RESERVED_20_36                              0x00
//MODEM_RESERVED_20_37                              0x00
//MODEM_AGC_WINDOW_SIZE                             0x11
//MODEM_AGC_RFPD_DECAY                              0x10
//MODEM_AGC_IFPD_DECAY                              0x10
//MODEM_FSK4_GAIN1                                  0x80
//MODEM_FSK4_GAIN0                                  0x1A
//MODEM_FSK4_TH1                                    0x40
//MODEM_FSK4_TH0                                    0x00
//MODEM_FSK4_MAP                                    0x00
//MODEM_OOK_PDTC                                    0x28
//MODEM_OOK_BLOPK                                   0x0C
//MODEM_OOK_CNT1                                    0xA4
//MODEM_OOK_MISC                                    0x23
//MODEM_RAW_SEARCH                                  0x56
//MODEM_RAW_CONTROL                                 0x03
//MODEM_RAW_EYE_1                                   0x01
//MODEM_RAW_EYE_0                                   0x15
//MODEM_ANT_DIV_MODE                                0x02
//MODEM_ANT_DIV_CONTROL                             0x00
//MODEM_RSSI_THRESH                                 0xFF
//MODEM_RSSI_JUMP_THRESH                            0x06
//MODEM_RSSI_CONTROL                                0x02
//MODEM_RSSI_CONTROL2                               0x1C
//MODEM_RSSI_COMP                                   0x40
//MODEM_DC_CONTROL                                  0x03
//MODEM_RESERVED_20_50/MODEM_RAW_SEARCH2            0x00
//MODEM_CLKGEN_BAND                                 0x02
//MODEM_RESERVED_20_52                              0x00
//MODEM_PLL_SETTLE_TIME                             0x28
//MODEM_CHFLT_RX1_CHFLT_COE13_7_0                   0xFF
//MODEM_CHFLT_RX1_CHFLT_COE12_7_0                   0xC4
//MODEM_CHFLT_RX1_CHFLT_COE11_7_0                   0x30
//MODEM_CHFLT_RX1_CHFLT_COE10_7_0                   0x7F
//MODEM_CHFLT_RX1_CHFLT_COE9_7_0                    0xF5
//MODEM_CHFLT_RX1_CHFLT_COE8_7_0                    0xB5
//MODEM_CHFLT_RX1_CHFLT_COE7_7_0                    0xB8
//MODEM_CHFLT_RX1_CHFLT_COE6_7_0                    0xDE
//MODEM_CHFLT_RX1_CHFLT_COE5_7_0                    0x05
//MODEM_CHFLT_RX1_CHFLT_COE4_7_0                    0x17
//MODEM_CHFLT_RX1_CHFLT_COE3_7_0                    0x16
//MODEM_CHFLT_RX1_CHFLT_COE2_7_0                    0x0C
//MODEM_CHFLT_RX1_CHFLT_COE1_7_0                    0x03
//MODEM_CHFLT_RX1_CHFLT_COE0_7_0                    0x00
//MODEM_CHFLT_RX1_CHFLT_COEM0                       0x15
//MODEM_CHFLT_RX1_CHFLT_COEM1                       0xFF
//MODEM_CHFLT_RX1_CHFLT_COEM2                       0x00
//MODEM_CHFLT_RX1_CHFLT_COEM3                       0x00
//MODEM_CHFLT_RX2_CHFLT_COE13_7_0                   0xFF
//MODEM_CHFLT_RX2_CHFLT_COE12_7_0                   0xC4
//MODEM_CHFLT_RX2_CHFLT_COE11_7_0                   0x30
//MODEM_CHFLT_RX2_CHFLT_COE10_7_0                   0x7F
//MODEM_CHFLT_RX2_CHFLT_COE9_7_0                    0xF5
//MODEM_CHFLT_RX2_CHFLT_COE8_7_0                    0xB5
//MODEM_CHFLT_RX2_CHFLT_COE7_7_0                    0xB8
//MODEM_CHFLT_RX2_CHFLT_COE6_7_0                    0xDE
//MODEM_CHFLT_RX2_CHFLT_COE5_7_0                    0x05
//MODEM_CHFLT_RX2_CHFLT_COE4_7_0                    0x17
//MODEM_CHFLT_RX2_CHFLT_COE3_7_0                    0x16
//MODEM_CHFLT_RX2_CHFLT_COE2_7_0                    0x0C
//MODEM_CHFLT_RX2_CHFLT_COE1_7_0                    0x03
//MODEM_CHFLT_RX2_CHFLT_COE0_7_0                    0x00
//MODEM_CHFLT_RX2_CHFLT_COEM0                       0x15
//MODEM_CHFLT_RX2_CHFLT_COEM1                       0xFF
//MODEM_CHFLT_RX2_CHFLT_COEM2                       0x00
//MODEM_CHFLT_RX2_CHFLT_COEM3                       0x00
//PA_MODE                                           0x18
//PA_PWR_LVL                                        0x4F
//PA_BIAS_CLKDUTY                                   0xC0
//PA_TC                                             0x1C
//PA_RAMP_EX                                        0x80
//PA_RAMP_DOWN_DELAY                                0x23
//PA_SPARE0/PA_DIG_PWR_SEQ_CONFIG                   0x03
//SYNTH_PFDCP_CPFF                                  0x01
//SYNTH_PFDCP_CPINT                                 0x05
//SYNTH_VCO_KV                                      0x0B
//SYNTH_LPFILT3                                     0x05
//SYNTH_LPFILT2                                     0x02
//SYNTH_LPFILT1                                     0x00
//SYNTH_LPFILT0                                     0x03
//SYNTH_VCO_KVCAL                                   0x05
//FREQ_CONTROL_INTE                                 0x1B
//FREQ_CONTROL_FRAC_2                               0x0F
//FREQ_CONTROL_FRAC_1                               0x77
//FREQ_CONTROL_FRAC_0                               0x77
//FREQ_CONTROL_CHANNEL_STEP_SIZE_1                  0x22
//FREQ_CONTROL_CHANNEL_STEP_SIZE_0                  0x22
//FREQ_CONTROL_W_SIZE                               0x20
//FREQ_CONTROL_VCOCNT_RX_ADJ                        0xFF
//PKT_CRC_CONFIG                                    0x05
//PKT_WHT_POLY_15_8                                 0x01
//PKT_WHT_POLY_7_0                                  0x08
//PKT_WHT_SEED_15_8                                 0xFF
//PKT_WHT_SEED_7_0                                  0xFF
//PKT_WHT_BIT_NUM                                   0x00
//PKT_CONFIG1                                       0x83
//PKT_RESERVED_18_7/PKT_CONFIG2                     0x00
//PKT_LEN                                           0x0A
//PKT_LEN_FIELD_SOURCE                              0x01
//PKT_LEN_ADJUST                                    0xFE
//PKT_TX_THRESHOLD                                  0x30
//PKT_RX_THRESHOLD                                  0x30
//PKT_FIELD_1_LENGTH_12_8                           0x00
//PKT_FIELD_1_LENGTH_7_0                            0x40
//PKT_FIELD_1_CONFIG                                0x04
//PKT_FIELD_1_CRC_CONFIG                            0x00
//PKT_FIELD_2_LENGTH_12_8                           0x00
//PKT_FIELD_2_LENGTH_7_0                            0x00
//PKT_FIELD_2_CONFIG                                0x00
//PKT_FIELD_2_CRC_CONFIG                            0x00
//PKT_FIELD_3_LENGTH_12_8                           0x00
//PKT_FIELD_3_LENGTH_7_0                            0x00
//PKT_FIELD_3_CONFIG                                0x00
//PKT_FIELD_3_CRC_CONFIG                            0x00
//PKT_FIELD_4_LENGTH_12_8                           0x00
//PKT_FIELD_4_LENGTH_7_0                            0x00
//PKT_FIELD_4_CONFIG                                0x00
//PKT_FIELD_4_CRC_CONFIG                            0x00
//PKT_FIELD_5_LENGTH_12_8                           0x00
//PKT_FIELD_5_LENGTH_7_0                            0x00
//PKT_FIELD_5_CONFIG                                0x00
//PKT_FIELD_5_CRC_CONFIG                            0x00
//PKT_RX_FIELD_1_LENGTH_12_8                        0x00
//PKT_RX_FIELD_1_LENGTH_7_0                         0x01
//PKT_RX_FIELD_1_CONFIG                             0x00
//PKT_RX_FIELD_1_CRC_CONFIG                         0x80
//PKT_RX_FIELD_2_LENGTH_12_8                        0x00
//PKT_RX_FIELD_2_LENGTH_7_0                         0x7F
//PKT_RX_FIELD_2_CONFIG                             0x00
//PKT_RX_FIELD_2_CRC_CONFIG                         0x0A
//PKT_RX_FIELD_3_LENGTH_12_8                        0x00
//PKT_RX_FIELD_3_LENGTH_7_0                         0x00
//PKT_RX_FIELD_3_CONFIG                             0x00
//PKT_RX_FIELD_3_CRC_CONFIG                         0x00
//PKT_RX_FIELD_4_LENGTH_12_8                        0x00
//PKT_RX_FIELD_4_LENGTH_7_0                         0x00
//PKT_RX_FIELD_4_CONFIG                             0x00
//PKT_RX_FIELD_4_CRC_CONFIG                         0x00
//PKT_RX_FIELD_5_LENGTH_12_8                        0x00
//PKT_RX_FIELD_5_LENGTH_7_0                         0x00
//PKT_RX_FIELD_5_CONFIG                             0x00
//PKT_RX_FIELD_5_CRC_CONFIG                         0x00
//GLOBAL_CLK_CFG                                    0x00
//GLOBAL_LOW_BATT_THRESH                            0x18
//GLOBAL_CONFIG                                     0x21
//GLOBAL_WUT_CONFIG                                 0x00
//GLOBAL_WUT_M_15_8                                 0x00
//GLOBAL_WUT_M_7_0                                  0x01
//GLOBAL_WUT_R                                      0x60
//GLOBAL_WUT_LDC                                    0x00
//GLOBAL_WUT_CAL                                    0x00

#endif //__RADIO_CONFIG_H__
