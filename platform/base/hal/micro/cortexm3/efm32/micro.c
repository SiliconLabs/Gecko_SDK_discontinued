// -----------------------------------------------------------------------------
// @file
// @brief EFM micro specific full HAL functions
//
// @author Silicon Laboratories Inc.
// @version 1.0.0
//
// @section License
// <b>(C) Copyright 2014 Silicon Laboratories, http://www.silabs.com</b>
//
// This file is licensed under the Silabs License Agreement. See the file
// "Silabs_License_Agreement.txt" for details. Before using this software for
// any purpose, you must agree to the terms of that agreement.
//
// -----------------------------------------------------------------------------
#include PLATFORM_HEADER
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_core.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_prs.h"
#include "rtcdriver.h"
#include "em_chip.h"
#include "gpiointerrupt.h"

#include "stack/include/ember.h"
#include "include/error.h"
#include "hal/hal.h"
#include "serial/serial.h"
#include "hal/micro/cortexm3/diagnostic.h"
#include "hal/micro/cortexm3/memmap.h"
#include "hal/micro/cortexm3/flash.h"

#ifdef _EFR_DEVICE
#include "hal/plugin/glib/graphics.h"
#include "pti.h"
#include "pa.h"
#include "tempdrv.h"
#include "sleep-efm32.h"
#endif

#ifdef BSP_STK
#include "bsp.h"
#endif

#ifdef RTOS
  #include "rtos/rtos.h"
#endif

extern void halStackRadioHoldOffPowerDown(void); // fwd ref
extern void halStackRadioHoldOffPowerUp(void);   // fwd ref

#ifndef RHO_GPIO
WEAK(void halStackRadioHoldOffPowerDown(void)) {}
WEAK(void halStackRadioHoldOffPowerUp(void)) {}
WEAK(bool halGetRadioHoldOff(void)) { return false; }
WEAK(EmberStatus halSetRadioHoldOff(bool enabled)) { return (enabled ? EMBER_BAD_ARGUMENT : EMBER_SUCCESS); }
#endif //!RHO_GPIO

#if !defined(PTA_REQ_GPIO) && !defined(PTA_GNT_GPIO)
WEAK(HalPtaOptions halPtaGetOptions(void)) { return 0; }
WEAK(EmberStatus halPtaSetOptions(HalPtaOptions options)) { return EMBER_ERR_FATAL; }
WEAK(EmberStatus halPtaSetBool(HalPtaOptions option, bool value)) { return EMBER_ERR_FATAL; }
WEAK(EmberStatus halPtaSetEnable(bool enabled)) { return EMBER_ERR_FATAL; }
WEAK(bool halPtaIsEnabled(void)) { return false; }
WEAK(EmberStatus halPtaSetTxRequest(halPtaReq_t ptaReq, halPtaCb_t ptaCb)) { return EMBER_ERR_FATAL; }
WEAK(EmberStatus halPtaSetRxRequest(halPtaReq_t ptaReq, halPtaCb_t ptaCb)) { return EMBER_ERR_FATAL; }
WEAK(halPtaReq_t halPtaFrameDetectReq(void)) { return 0; }
WEAK(halPtaReq_t halPtaFilterPassReq(void)) { return 0; }
#endif //!defined(PTA_REQ_GPIO) && !defined(PTA_GNT_GPIO)

#ifdef _EFR_DEVICE
bool halInternalGetCtuneToken(uint16_t *startupCtune, uint16_t *steadyCtune)
{
  tokTypeMfgCTune ctuneMfgTok=0xFFFF;
  halCommonGetToken(&ctuneMfgTok, TOKEN_MFG_CTUNE);
  if(ctuneMfgTok!=0xFFFF) {
    *startupCtune = ctuneMfgTok;
    *steadyCtune = ctuneMfgTok;
    return true;
  }
  return false;
}

void halInternalEnableHFXO(void)
{
  #ifndef EMBER_AF_USE_HWCONF
    CMU_HFXOInit_TypeDef hfxoInit = CMU_HFXOINIT_WSTK_DEFAULT;
  #else
    CMU_HFXOInit_TypeDef hfxoInit = CMU_HFXOINIT_DEFAULT;
  #endif
  uint16_t customStartupCtune;
  uint16_t customSteadyCtune;
  if (halInternalGetCtuneToken(&customStartupCtune, &customSteadyCtune))
  {
    hfxoInit.ctuneStartup = customStartupCtune;
    hfxoInit.ctuneSteadyState = customSteadyCtune;
  }
  CMU_HFXOInit(&hfxoInit);

  /* Enable HFXO oscillator, and wait for it to be stable */
  CMU_OscillatorEnable(cmuOsc_HFXO, true, true);

  /* Setting system HFXO frequency */
  SystemHFXOClockSet(38400000);

  /* Using HFXO as high frequency clock, HFCLK */
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);

  /* HFRCO not needed when using HFXO */
  CMU_OscillatorEnable(cmuOsc_HFRCO, false, false);
}

// Provide HAL pointers to board-header-defined PA configuration(s)
// for use by App, RAIL, or PHY library.
#ifdef  RADIO_PA_2P4_INIT
 static const RADIO_PAInit_t paInit2p4 = RADIO_PA_2P4_INIT;
 const RADIO_PAInit_t* halInternalPa2p4GHzInit = &paInit2p4;
#else//!RADIO_PA_2P4_INIT
 const RADIO_PAInit_t* halInternalPa2p4GHzInit = NULL;
#endif//RADIO_PA_2P4_INIT

#ifdef  RADIO_PA_SUBGIG_INIT
 static const RADIO_PAInit_t paInitSub = RADIO_PA_SUBGIG_INIT;
 const RADIO_PAInit_t* halInternalPaSubGHzInit = &paInitSub;
#else//!RADIO_PA_SUBGIG_INIT
 const RADIO_PAInit_t* halInternalPaSubGHzInit = NULL;
#endif//RADIO_PA_SUBGIG_INIT

#endif//_EFR_DEVICE

// halInit is called on first initial boot, not on wakeup from sleep.
void halInit(void)
{
  /* Configure board. Select either EBI or SPI mode. */
#ifdef _EFR_DEVICE
  EMU_UnlatchPinRetention();
  // HWCONF will handle clock initialization
  #ifndef EMBER_AF_USE_HWCONF
    CHIP_Init();
    halInternalEnableDCDC();
    halInternalEnableHFXO();
    CMU_OscillatorEnable(cmuOsc_LFRCO, true, true);
    CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFRCO);
    CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFRCO);
    CMU_ClockEnable(cmuClock_CORELE, true);
  #endif
  halInternalInitBoard();
 #ifdef  EMBER_STACK_CONNECT
  // In Connect we need to initialize the PA with the subGHz parameters.
  // But, only in case where we are not using the board in the 2.4 GHz band.
 #ifdef  CONNECT_2_4_PHY
  RADIO_PA_Init((RADIO_PAInit_t*)halInternalPa2p4GHzInit);
 #else//!CONNECT_2_4_PHY
  RADIO_PA_Init((RADIO_PAInit_t*)halInternalPaSubGHzInit);
 #endif//CONNECT_2_4_PHY
 #else//!EMBER_STACK_CONNECT
  // Other stacks init their PA later
 #endif//EMBER_STACK_CONNECT
  RTCDRV_Init();
  TEMPDRV_Init();
 #ifndef DISABLE_WATCHDOG
  halInternalEnableWatchDog();
 #endif
  EMU_EM4Init_TypeDef em4Init = EMU_EM4INIT_DEFAULT;
  em4Init.em4State = emuEM4Hibernate;
  EMU_EM4Init(&em4Init);
  halInternalEm4Wakeup();
#elif defined(_EZR_DEVICE)
    CHIP_Init();
    halInternalInitButton();
    halInternalInitLed();
    halInternalInitRadioHoldOff();
    halInternalInitVCPPins();
    CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);
    CMU_OscillatorEnable(cmuOsc_LFRCO, true, true);
    CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFRCO);
    CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFRCO);
    RTCDRV_Init();
    halInternalStartSymbolTimer();    // TODO: move to macInit or emRadioInit
#endif

  halInternalStartSystemTimer();
}

void halReboot(void)
{
  halInternalSysReset(RESET_SOFTWARE_REBOOT);
}

void halPowerDown(void)
{
  halInternalPowerDownBoard();
}

// halPowerUp is called from sleep state, not from first initial boot.
void halPowerUp(void)
{
  halInternalPowerUpBoard();
}

//If the board file defines runtime powerup/powerdown GPIO configuration,
//instantiate the variables and implement halStackRadioPowerDownBoard/
//halStackRadioPowerUpBoard which the stack will use to control the
//power state of radio specific GPIO.  If the board file does not define
//runtime GPIO configuration, the compile time configuration will work as
//it always has.
#ifdef DEFINE_SPI_CONFIG_VARIABLES
 DEFINE_SPI_CONFIG_VARIABLES();
#endif

// Instantiate global variables needed by the board header file
#ifdef DEFINE_BOARD_GPIO_CFG_VARIABLES
 DEFINE_BOARD_GPIO_CFG_VARIABLES();
#endif

void halStackRadioPowerDownBoard(void)
{
  (void) halPtaSetRxRequest(PTA_REQ_OFF, NULL);
  // Don't touch PTA Tx Request here
  halStackRadioHoldOffPowerDown();
}

void halStackRadio2PowerDownBoard(void)
{
  // Neither PTA nor RHO are supported on Radio2
}

void halStackRadioPowerUpBoard(void)
{
  halStackRadioHoldOffPowerUp();
}

void halStackRadio2PowerUpBoard(void)
{
  // Neither PTA nor RHO are supported on Radio2
}

void halStackRadioPowerMainControl(bool powerUp)
{
  if (powerUp) {
    halRadioPowerUpHandler();
  } else {
    halRadioPowerDownHandler();
  }
}

void halStackProcessBootCount(void)
{
  //Note: Because this always counts up at every boot (called from emberInit),
  //and non-volatile storage has a finite number of write cycles, this will
  //eventually stop working.  Disable this token call if non-volatile write
  //cycles need to be used sparingly.
#if defined(CREATOR_STACK_BOOT_COUNTER)
  halCommonIncrementCounterToken(TOKEN_STACK_BOOT_COUNTER);
#endif
}

PGM_P halGetResetString(void)
{
  // Table used to convert from reset types to reset strings.
  #define RESET_BASE_DEF(basename, value, string)  string,
  #define RESET_EXT_DEF(basename, extname, extvalue, string)  /*nothing*/
  static PGM char resetStringTable[][4] = {
    #include "reset-def.h"
  };
  #undef RESET_BASE_DEF
  #undef RESET_EXT_DEF
  uint8_t resetInfo = halGetResetInfo();
  if (resetInfo >= (sizeof(resetStringTable)/sizeof(resetStringTable[0]))){
      return resetStringTable[0x00]; // return unknown
  }
  else{
    return resetStringTable[resetInfo];
  }
}

// Note that this API should be used in conjunction with halGetResetString
//  to get the full information, as this API does not provide a string for
//  the base reset type
PGM_P halGetExtendedResetString(void)
{

  // Create a table of reset strings for each extended reset type
  typedef PGM char ResetStringTableType[][4];
  #define RESET_BASE_DEF(basename, value, string)   \
                         }; static ResetStringTableType basename##ResetStringTable = {
  #define RESET_EXT_DEF(basename, extname, extvalue, string)  string,
  {
    #include "reset-def.h"
  };
  #undef RESET_BASE_DEF
  #undef RESET_EXT_DEF

  // Create a table of pointers to each of the above tables
  #define RESET_BASE_DEF(basename, value, string)  (ResetStringTableType *)basename##ResetStringTable,
  #define RESET_EXT_DEF(basename, extname, extvalue, string)  /*nothing*/
  static ResetStringTableType * PGM extendedResetStringTablePtrs[] = {
    #include "reset-def.h"
  };
  #undef RESET_BASE_DEF
  #undef RESET_EXT_DEF

  uint16_t extResetInfo = halGetExtendedResetInfo();
  // access the particular table of extended strings we are interested in
  ResetStringTableType *extendedResetStringTable =
                    extendedResetStringTablePtrs[RESET_BASE_TYPE(extResetInfo)];

  // return the string from within the proper table
  return (*extendedResetStringTable)[((extResetInfo)&0xFF)];
}

// Translate EM3xx reset codes to the codes previously used by the EM2xx.
// If there is no corresponding code, return the EM3xx base code with bit 7 set.
uint8_t halGetEm2xxResetInfo(void)
{
  uint8_t reset = halGetResetInfo();

  // Any reset with an extended value field of zero is considered an unknown
  // reset, except for FIB resets.
  if ( (RESET_EXTENDED_FIELD(halGetExtendedResetInfo()) == 0) &&
       (reset != RESET_FIB) ) {
     return EM2XX_RESET_UNKNOWN;
  }

 switch (reset) {
  case RESET_UNKNOWN:
    return EM2XX_RESET_UNKNOWN;
  case RESET_BOOTLOADER:
    return EM2XX_RESET_BOOTLOADER;
  case RESET_EXTERNAL:      // map pin resets to poweron for EM2xx compatibility
//    return EM2XX_RESET_EXTERNAL;
  case RESET_POWERON:
    return EM2XX_RESET_POWERON;
  case RESET_WATCHDOG:
    return EM2XX_RESET_WATCHDOG;
  case RESET_SOFTWARE:
    return EM2XX_RESET_SOFTWARE;
  case RESET_CRASH:
    return EM2XX_RESET_ASSERT;
  default:
    return (reset | 0x80);      // set B7 for all other reset codes
  }
}
