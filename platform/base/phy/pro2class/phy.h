/*
 * File: phy/pro2class/phy.h
 * Description: Implementation of radio, baseband, and MAC functions.
 *
 * Copyright 2014 Silicon Laboratories, Inc.                                *80*
 */

#ifndef __PHY_PRO2CLASS_PHY_H__
#define __PHY_PRO2CLASS_PHY_H__

#ifndef PRO2_FW_API
#define PRO2_FW_API 0x06000A00ul // fw_6_0_10 (on A2A otp_6_0_7)
#endif//PRO2_FW_API

#include "hal/micro/micro-types.h"

#ifdef  LABTEST
#error  pro2class PHYs lacks support for LABTEST builds
#endif//LABTEST

/*----------------------------------------------------------------------------*/
/*                                Constants                                   */
/*----------------------------------------------------------------------------*/

#include "phy-params-subGHz.h"

// These are no longer constants, but configurable:
#define RSSI_DBM_OFFSET_PRO2CLASS       (-(int16_t)emPhyConfigMHz.rssidBmOffset)
#define ED_CCA_THRESHOLD_UNINIT         0x7F
#define ED_CCA_THRESHOLD_SUBBAND        (ED_CCA_THRESHOLD_UNINIT - 1)

#define MIN_RADIO_POWER                 -43
#undef  MAX_RADIO_POWER // Replace any prior def with pro2class max
#define MAX_RADIO_POWER                 21
#define INVALID_RADIO_POWER             EMBER_PHY_INVALID_RSSI // -128

// PRO2+ needs a distinct mode for Energy Detection.
// This definition overrides enum in phy/phy.h
#define EMBER_RADIO_POWER_MODE_ED_ON (EMBER_RADIO_POWER_MODE_OFF+1)
#define EMBER_RADIO_POWER_MODE_TX_ON (EMBER_RADIO_POWER_MODE_OFF+2)

// PRO2+ emberSetTxPowerMode() value to not override per-band config settings
#define EMBER_TX_POWER_MODE_PER_BAND_CONFIG       (0xFFFFu)
#define EMBER_TX_POWER_MODE_PA_SELECTION_MASK     (0x0003u)
#define EMBER_TX_POWER_MODE_BAND_NO_LIMITS_MASK   (0x00C0u)
#define EMBER_TX_POWER_MODE_BAND_NO_LIMITS_VALUE  (0x0080u)

#define NUM_RX_BUFFER_BYTES ( EMBER_PHY_MAX_PHR_BYTES       \
                            + EMBER_PHY_MAX_PAYLOAD_BYTES   \
                            + 1 /*optional pad byte*/       \
                            + NUM_APPENDED_INFO_BYTES       \
                            )

#define RADIO_CCA_ATTEMPT_MAX_802_15_4  5 // PRO2+ supports max 5 attempts

/*----------------------------------------------------------------------------*/
/*                                Data Types                                  */
/*----------------------------------------------------------------------------*/

typedef uint8_t EmberNetworkIndex;    // This really belongs in multi-network.h

// EmPhyMode is encoded:
// [  7 | 6  | 5 | 4         |     3    |    2   |    1   |      0        ]
// [ FILTER  |FPD| MR-encode | MR-SFD   |    FSK MODE     |   PHR Format  ]
// [ 00=2003 | 0 | x (2011)  | x (2011) | 00=2011 GFSK    | 0=2003/6/11   ]
// [ 01=2006 | 0 | 0=UNCODED | 0=SFD0   | 01=2012 MR-2FSK | 1=2012 MR-FSK ]
// [ 10=2012 | 0 | 1=ENCODED | 1=SFD1   | 02=2012 MR-4FSK |               ]
// FPD is Frame Pending Default, and must-be-zero currently.
enum {
  //          PHR   FSK  SFD   CODE  FILTER
  EM_PHY_MODE_2011_GFSK______________F2003 = 0x00,
  EM_PHY_MODE_2011_GFSK______________F2006 = 0x40,
  EM_PHY_MODE_2011_GFSK______________F2012 = 0x80,
  EM_PHY_MODE_2012_2FSK_SFD0_UNCODED_F2006 = 0x43,
  EM_PHY_MODE_2012_2FSK_SFD0_UNCODED_F2012 = 0x83,
  EM_PHY_MODE_2012_2FSK_SFD0_ENCODED_F2006 = 0x53,
  EM_PHY_MODE_2012_2FSK_SFD0_ENCODED_F2012 = 0x93,
  EM_PHY_MODE_2012_2FSK_SFD1_UNCODED_F2006 = 0x4B,
  EM_PHY_MODE_2012_2FSK_SFD1_UNCODED_F2012 = 0x8B,
  EM_PHY_MODE_2012_2FSK_SFD1_ENCODED_F2006 = 0x5B,
  EM_PHY_MODE_2012_2FSK_SFD1_ENCODED_F2012 = 0x9B,
  EM_PHY_MODE_2012_4FSK_SFD0_UNCODED_F2006 = 0x45,
  EM_PHY_MODE_2012_4FSK_SFD0_UNCODED_F2012 = 0x85,
  EM_PHY_MODE_2012_4FSK_SFD0_ENCODED_F2006 = 0x55,
  EM_PHY_MODE_2012_4FSK_SFD0_ENCODED_F2012 = 0x95,
  EM_PHY_MODE_2012_4FSK_SFD1_UNCODED_F2006 = 0x4D,
  EM_PHY_MODE_2012_4FSK_SFD1_UNCODED_F2012 = 0x8D,
  EM_PHY_MODE_2012_4FSK_SFD1_ENCODED_F2006 = 0x5D,
  EM_PHY_MODE_2012_4FSK_SFD1_ENCODED_F2012 = 0x9D,
};
typedef uint8_t EmPhyMode;

enum {
  PHY_EZR2_CONFIG_MODE_TRX,
  PHY_EZR2_CONFIG_MODE_CW,
  PHY_EZR2_CONFIG_MODE_PN9,
};

#ifndef PHY_HAS_DUTY_CYCLE
#define PHY_HAS_DUTY_CYCLE 1    // This PHY has duty cycle requirements
#endif//PHY_HAS_DUTY_CYCLE

#define EM_PHY_LBT_FLAG_MODE_MASK 0x01u // Mask for the parameter's mode
#define EM_PHY_LBT_FLAG_MODE_CSMA 0x00u // CSMA-mode parameters
#define EM_PHY_LBT_FLAG_MODE_LBT  0x01u // LBT-mode parameters

enum {
  PRO2_GPIO_NONE = 0,
  PRO2_GPIO_0    = 1,
  PRO2_GPIO_1    = 2,
  PRO2_GPIO_2    = 3,
  PRO2_GPIO_3    = 4,
  PRO2_GPIO_MAX // Must be last
};
typedef int8_t Pro2Gpio; // Positive=enabled, Negative=disabled, 0=none

/*----------------------------------------------------------------------------*/
/*                             API prototypes                                 */
/*----------------------------------------------------------------------------*/
// From em250class/analogue.h
// These literals are used in the implementation of emberSetTxPowerMode().
#define EMBER_TX_POWER_MODE_BOOST_BIT       0
#define EMBER_TX_POWER_MODE_ALTERNATE_BIT   1
// This option is exposed via the PHY_CONFIG token and via emberRadioSetPower():
#define EMBER_TX_POWER_MODE_EXT_PA_ON_BI_DIR_RF_PATH 0x0004
#define EMBER_TX_POWER_MODE_EXT_PA_ON_BI_DIR_RF_PATH_BIT 2
// but a non-default Tx Step Time is only exposed via the PHY_CONFIG token.
// This option is exposed via the PHY_CONFIG token and via emberRadioSetPower():
#define EMBER_TX_POWER_BOOST_LEVEL_BIT      3
#define EMBER_TX_POWER_BOOST_LEVEL_BITS     5

EmberStatus emRadioSetChannelAndForceCalibration(uint8_t channel);

#if     (!PHY_DUAL) // Native PHY provides, so don't need dups in pro2plus
#ifdef  CORTEXM3_EMBER_MICRO

extern EmberEventControl emCalPadsEvent;
void emCalPadsEventHandler(void);

// Allow access to certain registers in em3xx radio analog section
#include "hal/micro/cortexm3/em35x/regs-indirect-analogue.h"
#define AUXADC_ID      AUXADC_REG

// emRadioReadSerialReg
// This function reads data from an analog control register.
// These registers are accessed via the serial control bus.
// Only bits specified by mask parameter are read.
// The value is right shifted as needed before being returned.
uint16_t emRadioReadSerialReg(uint16_t id,
                            uint16_t bit,
                            uint16_t mask);

// emRadioWriteSerialReg
// This function writes data to an analog control register.
// These registers are accessed via the serial control bus.
// Only bits specified by mask parameter are changed,
// other existing register contents are preserved.
void emRadioWriteSerialReg(uint16_t id,
                           uint16_t value,
                           uint16_t mask);

#endif//CORTEXM3_EMBER_MICRO
#endif//(!PHY_DUAL) // Native PHY provides, so don't need dups in pro2plus

// From em250class/phy.h
void emPhySetCoordinator(bool coordinator);
bool emPhyDeviceIsCoordinator(void);
void emRadioSetEdCcaThreshold(int8_t thresholddBm);
int8_t emRadioGetEdCcaThreshold(void);
uint16_t emRadioUnpackChipErrors(uint8_t internalChipErrors);
uint8_t emRadioCalculateLinkQuality(uint16_t unpackedChipErrors,
                                    uint16_t packetLength);
uint8_t emRadioConvertRssiToEd(int8_t rssiDbm);
RadioPowerMode emRadioGetIdleMode(void);
void emRadioOkToIdle(void);
EmberStatus emRadioSetIdleMode(RadioPowerMode mode);
int8_t emRadioEnergyDetection(void);
uint32_t emGetMacTimer(void);
void emRadioEnableOverflowNotification(bool enable);
bool emRadioOverflowNotificationEnabled(void);
void emRadioEnableSfdSentNotification(bool enable);
bool emRadioSfdSentNotificationEnabled(void);

bool emPhyIsTransmitting(void);
void emPhyCancelTransmit(void);

// From em250class/phy.c
void emRadioIdleMicroForUs(uint32_t delayUs);

// From em3xx/phy.h
void emRadioTxAckIsrCallback(void);
void emPacketReceivedInternalIsrCallback(uint8_t *packet,
                                         uint8_t length,
                                         bool framePendingSetInTxAck);
#ifdef   SUPERPHY_LIBRARY
void emberRadioOverflowIsrCallback(void);
void emberRadioSfdSentIsrCallback(uint32_t sfdSentTime);
#endif// SUPERPHY_LIBRARY

int8_t emGetTempFromAdc(void);
bool emRadioIsReceivingFrame(void);

// New for pro2plus
EmberStatus emPhyGetDutyCycleParams(uint8_t macPgChan,
                                    EmPhyDutyCycleParams* dutyCycleParams);
EmberStatus emPhySetPhyMode(EmPhyMode phyMode);
//TODO: Provide these multi-network routines for real in MAC layer
#define emberRadioDataPendingLongIdPanIdIsrCallback(longId, panId) \
        emberRadioDataPendingLongIdIsrCallback(longId)
#define emberRadioDataPendingShortIdPanIdIsrCallback(shortId, panId) \
        emberRadioDataPendingShortIdIsrCallback(shortId)
#define emberChildIndexShortIdPanId(shortId, panId) \
        emberChildIndex(shortId)
#define emberChildIndexLongIdPanId(longId, panId) \
        emberChildLongIndex(longId)
void emRadioPowerDown(void);
void emRadioPowerUp(void);
EmberStatus emPhySetRadioChannelOnNetwork(uint8_t macPgChan,
                                          EmberNetworkIndex networkIndex);
EmberStatus emPhySetRadioPowerOnNetwork(int8_t dBmPower,
                                        EmberNetworkIndex networkIndex);
extern uint32_t emPhyGetLastTxCompleteTimeMs(bool lastCsmaTx); // For Duty Cycle
extern const uint8_t emPhyCcaCsmaConfigArray[];
extern const uint8_t emPhyCcaLbtConfigArray[];
extern const EmPhyConfig emPhyConfigMHz; // Sub-GHz band PHY
extern EmPhyBandConfig* emPhyCurrentBandConfig;
// Convenience macros to select default and current band config or default
// if current is not yet set
#define EM_PHY_DEFAULT_BAND_CONFIG (emPhyConfigMHz.bandConfigs[0])
#define EM_PHY_CURRENT_BAND_CONFIG ((emPhyCurrentBandConfig == NULL) \
                                   ? EM_PHY_DEFAULT_BAND_CONFIG      \
                                   : emPhyCurrentBandConfig )

// 'Internals' exposed for tests
#if defined(PHY_EZR2)
// Compatibility macros so we don't have to create duplicate test codes
#define pro2GetChipRev                  ezr2GetChipRev
#define pro2GetDieName                  ezr2GetDieName
#define pro2GetFunc                     ezr2GetFunc
#define pro2GetFwRev                    ezr2GetFwRev
#define pro2GetFwSvn                    ezr2GetFwSvn
#define pro2GetOtpId                    ezr2GetOtpId
#define pro2GetPartNum                  ezr2GetPartNum
#define pro2GetPatchRev                 ezr2GetPatchRev
#define pro2GetRomId                    ezr2GetRomId
#define pro2Peek                        ezr2Peek
#define pro2Poke                        ezr2Poke
#define pro2ReadOneProperty             ezr2ReadOneProperty
#define pro2WriteOneProperty            ezr2WriteOneProperty
#define pro2GetPtiLogging               ezr2GetPtiLogging
#define pro2SetPtiLogging               ezr2SetPtiLogging
#define pro2SetRadioBootMode            ezr2SetRadioBootMode
#define pro2GetRadioBootMode            ezr2GetRadioBootMode
#define pro2GetRadioTxPowerMode         ezr2GetRadioTxPowerMode
#define pro2GpioConfig                  ezr2GpioConfig

uint8_t ezr2GetChipRev(void);
const uint8_t* ezr2GetDieName(void);
uint8_t ezr2GetFunc(void);
uint32_t ezr2GetFwRev(void);
uint32_t ezr2GetFwSvn(void);
uint16_t ezr2GetOtpId(void);
uint16_t ezr2GetPartNum(void);
uint16_t ezr2GetPatchRev(void);
uint8_t ezr2GetRomId(void);
uint8_t ezr2Peek(uint16_t addr);
void ezr2Poke(uint16_t addr, uint8_t value);
uint8_t ezr2ReadOneProperty(uint8_t propGrp, uint8_t prop);
void ezr2WriteOneProperty(uint8_t propGrp, uint8_t prop, uint8_t value);
uint16_t ezr2GetPtiLogging(void);
void ezr2SetPtiLogging(uint16_t newPtiLogging);
void ezr2SetRadioBootMode(uint8_t bootOptions);
uint8_t ezr2GetRadioBootMode(void);
uint16_t ezr2GetRadioTxPowerMode(void);
EmberStatus ezr2GpioConfig(Pro2Gpio gpio, uint8_t config);
#else
uint8_t pro2GetChipRev(void);
const uint8_t* pro2GetDieName(void);
uint8_t pro2GetFunc(void);
uint32_t pro2GetFwRev(void);
uint32_t pro2GetFwSvn(void);
uint16_t pro2GetOtpId(void);
uint16_t pro2GetPartNum(void);
uint16_t pro2GetPatchRev(void);
uint8_t pro2GetRomId(void);
uint8_t pro2Peek(uint16_t addr);
void pro2Poke(uint16_t addr, uint8_t value);
uint8_t pro2ReadOneProperty(uint8_t propGrp, uint8_t prop);
void pro2WriteOneProperty(uint8_t propGrp, uint8_t prop, uint8_t value);
uint16_t pro2GetPtiLogging(void);
void pro2SetPtiLogging(uint16_t newPtiLogging);
void pro2SetRadioBootMode(uint8_t bootOptions);
uint8_t pro2GetRadioBootMode(void);
uint16_t pro2GetRadioTxPowerMode(void);
EmberStatus pro2GpioConfig(Pro2Gpio gpio, uint8_t config);
#ifndef PHY_PRO2
extern uint8_t emPhyPhr154gFeatures;
#endif//PHY_PRO2
#endif//defined(PHY_EZR2)
#endif// __PHY_PRO2CLASS_PHY_H__
