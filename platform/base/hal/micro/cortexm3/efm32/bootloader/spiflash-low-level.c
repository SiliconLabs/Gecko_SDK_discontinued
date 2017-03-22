/*
 * File: spiflash-class1.c
 * Description: This is the efm specific spiflash-class1.c that provides
 *              device specific implementation of the spiflash that
 *              changes between parts.
 *
 * Copyright 2015 Silicon Laboratories, Inc.                                *80*
 *
 */

/*
 * When SPI_FLASH_SC1 is defined, serial controller one will be used to
 * communicate with the external flash.
 * - This must be enabled in the case of app bootloading over USB, since USB
 *   uses SC2. Also ensure that the board jumpers are in the right place and
 *   the debugger is not connected to any SC1 pins.
 * - This must be enabled in the case of PRO2+-connected devices since SC2
 *   is used to interact with the PRO2+ radio.
 */

#include PLATFORM_HEADER
#include "hal/micro/micro.h"
#include "hal/micro/cortexm3/memmap.h"
#include BOARD_HEADER

#include "spiflash-low-level.h"

//// abstract away which pins and serial controller are being used
//#include "external-flash-gpio-select.h"

#include "em_cmu.h"
#include "em_usart.h"
#include "em_gpio.h"


#if !defined(EXTERNAL_FLASH_SERIAL_CONTROLLER) \
    || !defined(EXTERNAL_FLASH_SERIAL_CONTROLLER_CLOCK)

  #if defined(EXTERNAL_FLASH_SERIAL_CONTROLLER) \
      || defined(EXTERNAL_FLASH_SERIAL_CONTROLLER_CLOCK)

    #error Partial Serial controller definition. Please define \
           both EXTERNAL_FLASH_SERIAL_CONTROLLER and \
           EXTERNAL_FLASH_SERIAL_CONTROLLER_CLOCK when \
           specifying an alternate serial controller.
  #endif

  #define EXTERNAL_FLASH_SERIAL_CONTROLLER        USART1
  #define EXTERNAL_FLASH_SERIAL_CONTROLLER_CLOCK  cmuClock_USART1

#endif

//
// ~~~~~~~~~~~~~~ Device Specific SPI Routines ~~~~~~~~~~~~~~
//

// WARNING! hardware FIFO is only 2 bytes plus the shift register, so don't push
// more than 3 before popping!

void halSpiPush8(uint8_t txData)
{
  USART_Tx(EXTERNAL_FLASH_SERIAL_CONTROLLER, txData);
}

uint8_t halSpiPop8(void)
{
  // WARNING!  spiPop8 must be matched 1:1 with spiPush8 calls made first
  //  or else this could spin forever!!
  return USART_Rx(EXTERNAL_FLASH_SERIAL_CONTROLLER);
}

// Default external flash pin locations for WSTK expansion header
#ifndef EXTERNAL_FLASH_USART_TXLOC
#define EXTERNAL_FLASH_USART_TXLOC             USART_ROUTELOC0_TXLOC_LOC11
#endif
#ifndef EXTERNAL_FLASH_USART_RXLOC
#define EXTERNAL_FLASH_USART_RXLOC             USART_ROUTELOC0_RXLOC_LOC11
#endif
#ifndef EXTERNAL_FLASH_USART_CLKLOC
#define EXTERNAL_FLASH_USART_CLKLOC            USART_ROUTELOC0_CLKLOC_LOC11
#endif

#ifndef EXTERNAL_FLASH_MOSI_PORT
#define EXTERNAL_FLASH_MOSI_PORT    gpioPortC
#endif
#ifndef EXTERNAL_FLASH_MOSI_PIN
#define EXTERNAL_FLASH_MOSI_PIN     6
#endif
#ifndef EXTERNAL_FLASH_MISO_PORT
#define EXTERNAL_FLASH_MISO_PORT    gpioPortC
#endif
#ifndef EXTERNAL_FLASH_MISO_PIN
#define EXTERNAL_FLASH_MISO_PIN     7
#endif
#ifndef EXTERNAL_FLASH_CLK_PORT
#define EXTERNAL_FLASH_CLK_PORT     gpioPortC
#endif
#ifndef EXTERNAL_FLASH_CLK_PIN
#define EXTERNAL_FLASH_CLK_PIN      8
#endif
#ifndef EXTERNAL_FLASH_CS_PORT
#define EXTERNAL_FLASH_CS_PORT      gpioPortC
#endif
#ifndef EXTERNAL_FLASH_CS_PIN
#define EXTERNAL_FLASH_CS_PIN       9
#endif

#ifndef EXTERNAL_FLASH_CS_ACTIVE
#define EXTERNAL_FLASH_CS_ACTIVE()              GPIO_PinOutClear(EXTERNAL_FLASH_CS_PORT, EXTERNAL_FLASH_CS_PIN)
#endif
#ifndef EXTERNAL_FLASH_CS_INACTIVE
#define EXTERNAL_FLASH_CS_INACTIVE()            GPIO_PinOutSet(EXTERNAL_FLASH_CS_PORT, EXTERNAL_FLASH_CS_PIN)
#endif

#ifndef EXTERNAL_FLASH_BAUDRATE
  #define EXTERNAL_FLASH_BAUDRATE 6400000U
#else
  #if (EXTERNAL_FLASH_BAUDRATE > 9000000)
    #warning Baudrate over 9000000 can have issues on some chips due to MISO setup time
  #elif (EXTERNAL_FLASH_BAUDRATE <= 0)
    #error Can't have baudrate of 0 or below
  #endif
#endif

// default PA4 for power pin

#if defined(EEPROM_USES_SHUTDOWN_CONTROL)

  // Default to PA4 when board header doesn't fully define alternative
  #if    !defined(EEPROM_POWER_PORT)   \
         || !defined(EEPROM_POWER_PIN)

    // Issue warning for partial definitions
    #if    defined(EEPROM_POWER_PORT)  \
          || defined(EEPROM_POWER_PIN)

      #error Partial EEPROM enable pin definition. Please define all of \
             CONFIGURE_EEPROM_POWER_PIN, SET_EEPROM_POWER_PIN, and \
             CLR_EEPROM_POWER_PIN as funciton-like macros when specifying an \
             alternate pin.

    #endif

    #define EEPROM_POWER_PORT gpioPortA
    #define EEPROM_POWER_PIN  4

  #endif
  
  #define CONFIGURE_EEPROM_POWER_PIN()  GPIO_PinModeSet(EEPROM_POWER_PORT, EEPROM_POWER_PIN, gpioModePushPull, 0)
  #define SET_EEPROM_POWER_PIN()        GPIO_PinOutSet(EEPROM_POWER_PORT, EEPROM_POWER_PIN)
  #define CLR_EEPROM_POWER_PIN()        GPIO_PinOutClear(EEPROM_POWER_PORT, EEPROM_POWER_PIN)

#else

  // Define empty macros when shutdown control is not used
  #define CONFIGURE_EEPROM_POWER_PIN()            (void)0
  #define SET_EEPROM_POWER_PIN()                  (void)0
  #define CLR_EEPROM_POWER_PIN()                  (void)0

#endif

#if defined(SPIFLASH_CLASS1_DEBUG) && SPIFLASH_CLASS1_DEBUG
  #define SPIFLASH_CLASS1_DEBUG_SETUP()   GPIO_PinModeSet(gpioPortF, 4, gpioModePushPull,  0)
  #define SPIFLASH_CLASS1_DEBUG_TOGGLE()  GPIO_PinToggle(gpioPortF, 4)
  #define SPIFLASH_CLASS1_DEBUG_SET()     GPIO_PinOutSet(gpioPortF, 4)
  #define SPIFLASH_CLASS1_DEBUG_CLEAR()   GPIO_PinOutClear(gpioPortF, 4)
#else
  #define SPIFLASH_CLASS1_DEBUG_SETUP()   (void)0
  #define SPIFLASH_CLASS1_DEBUG_TOGGLE()  (void)0
  #define SPIFLASH_CLASS1_DEBUG_SET()     (void)0
  #define SPIFLASH_CLASS1_DEBUG_CLEAR()   (void)0
#endif


//
//  ~~~~~~~~~~~ Device Specific Eeprom Functions ~~~~~~~~~~~
//

void setFlashCSActive(void)
{
  EXTERNAL_FLASH_CS_ACTIVE();
}

void setFlashCSInactive(void)
{
  EXTERNAL_FLASH_CS_INACTIVE();
}

/** Synchronous mode init structure. */
static const USART_InitSync_TypeDef usartInit = 
{
  /** Specifies whether TX and/or RX shall be enabled when init completed. */
  //USART_Enable_TypeDef    enable;
  usartEnable,

  /**
   * USART/UART reference clock assumed when configuring baudrate setup. Set
   * it to 0 if currently configurated reference clock shall be used.
   */
  //uint32_t                refFreq;
  38400000U,

  /** Desired baudrate. */
  //uint32_t                baudrate;
  EXTERNAL_FLASH_BAUDRATE,

  /** Number of databits in frame. */
  //USART_Databits_TypeDef  databits;
  usartDatabits8,

  /** Select if to operate in master or slave mode. */
  //bool                    master;
  true,

  /** Select if to send most or least significant bit first. */
  //bool                    msbf;
  true,

  /** Clock polarity/phase mode. */
  //USART_ClockMode_TypeDef clockMode;
  usartClockMode0,

  /** Enable USART Rx via PRS. */
  //bool                    prsRxEnable;
  false,

  /** Select PRS channel for USART Rx. (Only valid if prsRxEnable is true). */
  //USART_PrsRxCh_TypeDef   prsRxCh;
  usartPrsRxCh0,

  /** Enable AUTOTX mode. Transmits as long as RX is not full.
   *  If TX is empty, underflows are generated. */
  //bool                    autoTx;
  false,

  /** Auto CS enabling */
  //bool autoCsEnable;
  false,

  /** Auto CS hold time in baud cycles */
  //uint8_t autoCsHold;
  0,

  /** Auto CS setup time in baud cycles */
  //uint8_t autoCsSetup;
  0,

} ;

static const uint32_t iterations_per_microsecond = 3;

void usecDelay( uint32_t usecs )
{
  volatile uint64_t iterations = iterations_per_microsecond * usecs;

  while(iterations--);
}

void halEepromConfigureGPIO(void){
  // MOSI
  GPIO_PinModeSet(EXTERNAL_FLASH_MOSI_PORT, EXTERNAL_FLASH_MOSI_PIN, gpioModePushPull,  0);
  // MISO
  GPIO_PinModeSet(EXTERNAL_FLASH_MISO_PORT, EXTERNAL_FLASH_MISO_PIN, gpioModeInputPull, 0);
  // CLK
  GPIO_PinModeSet(EXTERNAL_FLASH_CLK_PORT, EXTERNAL_FLASH_CLK_PIN, gpioModePushPull,  0);
  // CS#
  GPIO_PinModeSet(EXTERNAL_FLASH_CS_PORT, EXTERNAL_FLASH_CS_PIN, gpioModePushPull,  1);

#if defined(REDUCED_SLEWRATE)
  // ----- Account for Noise and Crosstalk ------ //
  // on some hardware configurations there is a lot of noise and bootloading can fail
  // due to crosstalk. to avoid this, the slewrate is lowered here from 6 to 5
  // if noise related errors still occur, the slewrate can be lowered further
  GPIO->P[EXTERNAL_FLASH_MOSI_PORT].CTRL = 0x00000000;
  GPIO->P[EXTERNAL_FLASH_MOSI_PORT].CTRL = (0x5 << _GPIO_P_CTRL_SLEWRATE_SHIFT);
  GPIO->P[EXTERNAL_FLASH_MOSI_PORT].CTRL |= (0x5 << _GPIO_P_CTRL_SLEWRATEALT_SHIFT);

  GPIO->P[EXTERNAL_FLASH_MISO_PORT].CTRL = 0x00000000;
  GPIO->P[EXTERNAL_FLASH_MISO_PORT].CTRL = (0x5 << _GPIO_P_CTRL_SLEWRATE_SHIFT);
  GPIO->P[EXTERNAL_FLASH_MISO_PORT].CTRL |= (0x5 << _GPIO_P_CTRL_SLEWRATEALT_SHIFT);

  GPIO->P[EXTERNAL_FLASH_CLK_PORT].CTRL = 0x00000000;
  GPIO->P[EXTERNAL_FLASH_CLK_PORT].CTRL = (0x5 << _GPIO_P_CTRL_SLEWRATE_SHIFT);
  GPIO->P[EXTERNAL_FLASH_CLK_PORT].CTRL |= (0x5 << _GPIO_P_CTRL_SLEWRATEALT_SHIFT);

  GPIO->P[EXTERNAL_FLASH_CS_PORT].CTRL = 0x00000000;
  GPIO->P[EXTERNAL_FLASH_CS_PORT].CTRL = (0x5 << _GPIO_P_CTRL_SLEWRATE_SHIFT);
  GPIO->P[EXTERNAL_FLASH_CS_PORT].CTRL |= (0x5 << _GPIO_P_CTRL_SLEWRATEALT_SHIFT);
#endif

#if defined(EXTERNAL_FLASH_HOLD_PORT)
  GPIO_PinModeSet(EXTERNAL_FLASH_HOLD_PORT, EXTERNAL_FLASH_HOLD_PIN, gpioModePushPull, 1);
#endif
#if defined(EXTERNAL_FLASH_WP_PORT)
  GPIO_PinModeSet(EXTERNAL_FLASH_WP_PORT, EXTERNAL_FLASH_WP_PIN, gpioModePushPull, 1);
#endif
}

void halEepromConfigureFlashController(void){
  SPIFLASH_CLASS1_DEBUG_SETUP();
  SPIFLASH_CLASS1_DEBUG_CLEAR();

  CMU_ClockEnable(cmuClock_HFPER, true);
  CMU_ClockEnable(cmuClock_GPIO, true);
  CMU_ClockEnable(EXTERNAL_FLASH_SERIAL_CONTROLLER_CLOCK, true);

  USART_InitSync(EXTERNAL_FLASH_SERIAL_CONTROLLER, &usartInit);

  // :TODO: abstract away pin selection for route location and GPIO mode setting

  EXTERNAL_FLASH_SERIAL_CONTROLLER->ROUTEPEN  =   USART_ROUTEPEN_TXPEN
                                                | USART_ROUTEPEN_RXPEN
                                                | USART_ROUTEPEN_CLKPEN;

  EXTERNAL_FLASH_SERIAL_CONTROLLER->ROUTELOC0 =   EXTERNAL_FLASH_USART_TXLOC
                                                | EXTERNAL_FLASH_USART_RXLOC
                                                | EXTERNAL_FLASH_USART_CLKLOC ;
}

void halEepromDelayMicroseconds(uint32_t timeToDelay)
{
  SPIFLASH_CLASS1_DEBUG_SET();
  usecDelay(timeToDelay);
  SPIFLASH_CLASS1_DEBUG_CLEAR();
}

void configureEepromPowerPin(void)
{
  CONFIGURE_EEPROM_POWER_PIN();
}

void setEepromPowerPin(void)
{
  SET_EEPROM_POWER_PIN();
}

void clearEepromPowerPin(void)
{
  CLR_EEPROM_POWER_PIN();
}
