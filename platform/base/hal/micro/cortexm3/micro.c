/*
 * File: micro.c
 * Description: EM3XX micro specific full HAL functions
 *
 *
 * Copyright 2008, 2009 by Ember Corporation. All rights reserved.          *80*
 */
//[[ Author(s): Brooks Barrett, Lee Taylor ]]


#include PLATFORM_HEADER
#include "stack/include/ember.h"
#include "include/error.h"

#include "hal/hal.h"
#include "serial/serial.h"
#include "hal/micro/cortexm3/diagnostic.h"
#include "hal/micro/cortexm3/memmap.h"
#include "hal/micro/cortexm3/flash.h"
#include "stack/platform/micro/debug-channel.h"

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

// halInit is called on first initial boot, not on wakeup from sleep.
void halInit(void)
{
  //[[ Strip emulator only code from official build
  #ifdef EMBER_EMU_TEST
    //On the emulator, give our fake XTAL reasonable thresholds so the cal
    //algorithm ends up at 4.
    EMU_OSC24M_CTRL =((0x8<<EMU_OSC24M_CTRL_OSC24M_THRESH_H_BIT) |
                      (0x2<<EMU_OSC24M_CTRL_OSC24M_THRESH_L_BIT) |
                      (0x0<<EMU_OSC24M_CTRL_OSC24M_THRESH_STOP_BIT));
  #endif
  //]]

  halCommonStartXtal();

  halInternalSetRegTrim(false);

  GPIO_DBGCFG |= GPIO_DBGCFGRSVD;

  #ifndef DISABLE_WATCHDOG
    halInternalEnableWatchDog();
  #endif

  halCommonCalibratePads();

  #ifdef DISABLE_INTERNAL_1V8_REGULATOR
    //Out of reset, VREG 1V8 is enabled.  Update the helper variable
    //to keep the state in agreement with what the disable API is expecting.
    halCommonVreg1v8EnableCount = 1;
    halCommonDisableVreg1v8();
  #endif

  halInternalInitBoard();

  halCommonSwitchToXtal();

  #ifndef DISABLE_RC_CALIBRATION
    halInternalCalibrateFastRc();
  #endif//DISABLE_RC_CALIBRATION

  halInternalStartSystemTimer();

  #ifdef INTERRUPT_DEBUGGING
    //When debugging interrupts/ATOMIC, ensure that our
    //debug pin is properly enabled and idle low.
    I_OUT(I_PORT, I_PIN, I_CFG_HL);
    I_CLR(I_PORT, I_PIN);
  #endif //INTERRUPT_DEBUGGING

  #ifdef USB_CERT_TESTING
  halInternalPowerDownBoard();
  #endif
}

void halReboot(void)
{
  halInternalSysReset(RESET_SOFTWARE_REBOOT);
}

void halPowerDown(void)
{
  emDebugPowerDown();

  halInternalPowerDownUart();

  halInternalPowerDownBoard();
}

// halPowerUp is called from sleep state, not from first initial boot.
void halPowerUp(void)
{
  //[[ Strip emulator only code from official build
  #ifdef EMBER_EMU_TEST
    //On the emulator, give our fake XTAL reasonable thresholds so the cal
    //algorithm ends up at 4.
    EMU_OSC24M_CTRL =((0x8<<EMU_OSC24M_CTRL_OSC24M_THRESH_H_BIT) |
                      (0x2<<EMU_OSC24M_CTRL_OSC24M_THRESH_L_BIT) |
                      (0x0<<EMU_OSC24M_CTRL_OSC24M_THRESH_STOP_BIT));
  #endif
  //]]

  halInternalPowerUpKickXtal();

  halCommonCalibratePads();

  //NOTE: The register VREG is a high voltage register that holds its
  //      state across deep sleep.  While halInit() must be sensitive
  //      to the define DISABLE_INTERNAL_1V8_REGULATOR, halPowerUp()
  //      trusts that VREG hasn't been modified and therefore doesn't
  //      need to take further action with respect to VREG.

  halInternalPowerUpBoard();

  halInternalBlockUntilXtal();

  halInternalPowerUpUart();

  emDebugPowerUp();
}

// halSuspend suspends all board activity except for USB
void halSuspend(void)
{
  halInternalPowerDownUart();

  halInternalSuspendBoard();
}

// halResume restores all board activity from a previous USB suspend
void halResume(void)
{
  //[[ Strip emulator only code from official build
  #ifdef EMBER_EMU_TEST
    //On the emulator, give our fake XTAL reasonable thresholds so the cal
    //algorithm ends up at 4.
    EMU_OSC24M_CTRL =((0x8<<EMU_OSC24M_CTRL_OSC24M_THRESH_H_BIT) |
                      (0x2<<EMU_OSC24M_CTRL_OSC24M_THRESH_L_BIT) |
                      (0x0<<EMU_OSC24M_CTRL_OSC24M_THRESH_STOP_BIT));
  #endif
  //]]

  halInternalPowerUpKickXtal();

  halCommonCalibratePads();

  halInternalResumeBoard();

  halInternalBlockUntilXtal();

  halInternalPowerUpUart();
}

//If the board file defines runtime powerup/powerdown GPIO configuration,
//instantiate the variables and implement halStackRadioPowerDownBoard/
//halStackRadioPowerUpBoard which the stack will use to control the
//power state of radio specific GPIO.  If the board file does not define
//runtime GPIO configuration, the compile time configuration will work as
//it always has.
#if defined(DEFINE_POWERUP_GPIO_CFG_VARIABLES)           && \
    defined(DEFINE_POWERUP_GPIO_OUTPUT_DATA_VARIABLES)   && \
    defined(DEFINE_POWERDOWN_GPIO_CFG_VARIABLES)         && \
    defined(DEFINE_POWERDOWN_GPIO_OUTPUT_DATA_VARIABLES)

DEFINE_POWERUP_GPIO_CFG_VARIABLES();
DEFINE_POWERUP_GPIO_OUTPUT_DATA_VARIABLES();
DEFINE_POWERDOWN_GPIO_CFG_VARIABLES();
DEFINE_POWERDOWN_GPIO_OUTPUT_DATA_VARIABLES();

#if defined(DEFINE_GPIO_RADIO_POWER_BOARD_MASK_VARIABLE)
  DEFINE_GPIO_RADIO_POWER_BOARD_MASK_VARIABLE();
#endif
#if defined(DEFINE_GPIO_RADIO2_POWER_BOARD_MASK_VARIABLE)
  DEFINE_GPIO_RADIO2_POWER_BOARD_MASK_VARIABLE();
#endif

static void rmwRadioPowerCfgReg(uint32_t gpioRadioPowerBoardMask,
                                uint16_t radioPowerCfg[],
                                uint32_t volatile * cfgReg,
                                uint8_t cfgVar)
{
  uint32_t temp = *cfgReg;
  uint8_t i;

  //don't waste time with a register that doesn't have anything to be done
  if(gpioRadioPowerBoardMask&(((GpioMaskType)0xF)<<(4*cfgVar))) {
    //loop over the 4 pins of the cfgReg
    for(i=0; i<4; i++) {
      if((gpioRadioPowerBoardMask>>((4*cfgVar)+i))&1) {
        //read-modify-write the pin's cfg if the mask says it pertains
        //to the radio's power state
        temp &= ~(0xFu<<(4*i));
        temp |= (radioPowerCfg[cfgVar]&(0xF<<(4*i)));
      }
    }
  }

  *cfgReg = temp;
}


static void rmwRadioPowerOutReg(uint32_t gpioRadioPowerBoardMask,
                                uint8_t radioPowerOut[],
                                uint32_t volatile * outReg,
                                uint8_t outVar)
{
  uint32_t temp = *outReg;
  uint8_t i;

  //don't waste time with a register that doesn't have anything to be done
  if(gpioRadioPowerBoardMask&(((GpioMaskType)0xFF)<<(8*outVar))) {
    //loop over the 8 pins of the outReg
    for(i=0; i<8; i++) {
      if((gpioRadioPowerBoardMask>>((8*outVar)+i))&1) {
        //read-modify-write the pin's out if the mask says it pertains
        //to the radio's power state
        temp &= ~(0x1u<<(1*i));
        temp |= (radioPowerOut[outVar]&(0x1<<(1*i)));
      }
    }
  }

  *outReg = temp;
}

void halStackRadioPowerDownBoard(void)
{
  (void) halPtaSetRxRequest(PTA_REQ_OFF, NULL);
  // Don't touch PTA Tx Request here
  halStackRadioHoldOffPowerDown();

 #if     defined(DEFINE_GPIO_RADIO_POWER_BOARD_MASK_VARIABLE)
  if(gpioRadioPowerBoardMask == 0) {
    //If the mask indicates there are no special GPIOs for the
    //radio that need their power state to be conrolled by the stack,
    //don't bother attempting to do anything.
    return;
  }

  rmwRadioPowerOutReg(gpioRadioPowerBoardMask, gpioOutPowerDown, &GPIO_PAOUT, 0);
  rmwRadioPowerOutReg(gpioRadioPowerBoardMask, gpioOutPowerDown, &GPIO_PBOUT, 1);
  rmwRadioPowerOutReg(gpioRadioPowerBoardMask, gpioOutPowerDown, &GPIO_PCOUT, 2);

  rmwRadioPowerCfgReg(gpioRadioPowerBoardMask, gpioCfgPowerDown, &GPIO_PACFGL, 0);
  rmwRadioPowerCfgReg(gpioRadioPowerBoardMask, gpioCfgPowerDown, &GPIO_PACFGH, 1);
  rmwRadioPowerCfgReg(gpioRadioPowerBoardMask, gpioCfgPowerDown, &GPIO_PBCFGL, 2);
  rmwRadioPowerCfgReg(gpioRadioPowerBoardMask, gpioCfgPowerDown, &GPIO_PBCFGH, 3);
  rmwRadioPowerCfgReg(gpioRadioPowerBoardMask, gpioCfgPowerDown, &GPIO_PCCFGL, 4);
  rmwRadioPowerCfgReg(gpioRadioPowerBoardMask, gpioCfgPowerDown, &GPIO_PCCFGH, 5);
 #endif//defined(DEFINE_GPIO_RADIO_POWER_BOARD_MASK_VARIABLE)
}

void halStackRadio2PowerDownBoard(void)
{
  // Neither PTA nor RHO are supported on Radio2
 #if     defined(DEFINE_GPIO_RADIO2_POWER_BOARD_MASK_VARIABLE)
  if(gpioRadio2PowerBoardMask == 0) {
    //If the mask indicates there are no special GPIOs for the
    //radio that need their power state to be conrolled by the stack,
    //don't bother attempting to do anything.
    return;
  }

  rmwRadioPowerOutReg(gpioRadio2PowerBoardMask, gpioOutPowerDown, &GPIO_PAOUT, 0);
  rmwRadioPowerOutReg(gpioRadio2PowerBoardMask, gpioOutPowerDown, &GPIO_PBOUT, 1);
  rmwRadioPowerOutReg(gpioRadio2PowerBoardMask, gpioOutPowerDown, &GPIO_PCOUT, 2);

  rmwRadioPowerCfgReg(gpioRadio2PowerBoardMask, gpioCfgPowerDown, &GPIO_PACFGL, 0);
  rmwRadioPowerCfgReg(gpioRadio2PowerBoardMask, gpioCfgPowerDown, &GPIO_PACFGH, 1);
  rmwRadioPowerCfgReg(gpioRadio2PowerBoardMask, gpioCfgPowerDown, &GPIO_PBCFGL, 2);
  rmwRadioPowerCfgReg(gpioRadio2PowerBoardMask, gpioCfgPowerDown, &GPIO_PBCFGH, 3);
  rmwRadioPowerCfgReg(gpioRadio2PowerBoardMask, gpioCfgPowerDown, &GPIO_PCCFGL, 4);
  rmwRadioPowerCfgReg(gpioRadio2PowerBoardMask, gpioCfgPowerDown, &GPIO_PCCFGH, 5);
 #endif//defined(DEFINE_GPIO_RADIO2_POWER_BOARD_MASK_VARIABLE)
}

void halStackRadioPowerUpBoard(void)
{
  halStackRadioHoldOffPowerUp();

 #if     defined(DEFINE_GPIO_RADIO_POWER_BOARD_MASK_VARIABLE)
  if(gpioRadioPowerBoardMask == 0) {
    //If the mask indicates there are no special GPIOs for the
    //radio that need their power state to be conrolled by the stack,
    //don't bother attempting to do anything.
    return;
  }

  rmwRadioPowerOutReg(gpioRadioPowerBoardMask, gpioOutPowerUp, &GPIO_PAOUT, 0);
  rmwRadioPowerOutReg(gpioRadioPowerBoardMask, gpioOutPowerUp, &GPIO_PBOUT, 1);
  rmwRadioPowerOutReg(gpioRadioPowerBoardMask, gpioOutPowerUp, &GPIO_PCOUT, 2);

  rmwRadioPowerCfgReg(gpioRadioPowerBoardMask, gpioCfgPowerUp, &GPIO_PACFGL, 0);
  rmwRadioPowerCfgReg(gpioRadioPowerBoardMask, gpioCfgPowerUp, &GPIO_PACFGH, 1);
  rmwRadioPowerCfgReg(gpioRadioPowerBoardMask, gpioCfgPowerUp, &GPIO_PBCFGL, 2);
  rmwRadioPowerCfgReg(gpioRadioPowerBoardMask, gpioCfgPowerUp, &GPIO_PBCFGH, 3);
  rmwRadioPowerCfgReg(gpioRadioPowerBoardMask, gpioCfgPowerUp, &GPIO_PCCFGL, 4);
  rmwRadioPowerCfgReg(gpioRadioPowerBoardMask, gpioCfgPowerUp, &GPIO_PCCFGH, 5);
 #endif//defined(DEFINE_GPIO_RADIO_POWER_BOARD_MASK_VARIABLE)
}

void halStackRadio2PowerUpBoard(void)
{
  // Neither PTA nor RHO are supported on Radio2
 #if     defined(DEFINE_GPIO_RADIO2_POWER_BOARD_MASK_VARIABLE)
  if(gpioRadio2PowerBoardMask == 0) {
    //If the mask indicates there are no special GPIOs for the
    //radio that need their power state to be conrolled by the stack,
    //don't bother attempting to do anything.
    return;
  }

  rmwRadioPowerOutReg(gpioRadio2PowerBoardMask, gpioOutPowerUp, &GPIO_PAOUT, 0);
  rmwRadioPowerOutReg(gpioRadio2PowerBoardMask, gpioOutPowerUp, &GPIO_PBOUT, 1);
  rmwRadioPowerOutReg(gpioRadio2PowerBoardMask, gpioOutPowerUp, &GPIO_PCOUT, 2);

  rmwRadioPowerCfgReg(gpioRadio2PowerBoardMask, gpioCfgPowerUp, &GPIO_PACFGL, 0);
  rmwRadioPowerCfgReg(gpioRadio2PowerBoardMask, gpioCfgPowerUp, &GPIO_PACFGH, 1);
  rmwRadioPowerCfgReg(gpioRadio2PowerBoardMask, gpioCfgPowerUp, &GPIO_PBCFGL, 2);
  rmwRadioPowerCfgReg(gpioRadio2PowerBoardMask, gpioCfgPowerUp, &GPIO_PBCFGH, 3);
  rmwRadioPowerCfgReg(gpioRadio2PowerBoardMask, gpioCfgPowerUp, &GPIO_PCCFGL, 4);
  rmwRadioPowerCfgReg(gpioRadio2PowerBoardMask, gpioCfgPowerUp, &GPIO_PCCFGH, 5);
 #endif//defined(DEFINE_GPIO_RADIO2_POWER_BOARD_MASK_VARIABLE)
}

#else

//If the board file uses traditional compile time powerup/powerdown GPIO
//configuration, stub halStackRadioPowerDownBoard/halStackRadioPowerUpBoard
//which the stack would have used to control the power state of radio
//relevant GPIO.  With compile time configuration, only the traditional
//halInternalPowerDownBoard and halInternalPowerUpBoard funtions configure
//the GPIO.  RHO powerdown/up still needs to be managed however.

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

#endif

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
  halCommonIncrementCounterToken(TOKEN_STACK_BOOT_COUNTER);
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
