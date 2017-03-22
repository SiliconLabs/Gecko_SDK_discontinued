/*
 * File: spiflash-class1.c
 * Description: This is the EMxx specific spiflash-class1.c that provides
 *              device specific implementations of the spiflash that 
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

#include "hal/micro/cortexm3/common/spiflash-low-level.h"

// abstract away which pins and serial controller are being used
#include "external-flash-gpio-select.h"




//
// ~~~~~~~~~~~~~~~~ Device Specific SPI Routines ~~~~~~~~~~~~~~~~~~~
//

// WARNING! hardware FIFO is only 4 bytes, so don't push more than 4 before popping!
void halSpiPush8(uint8_t txData)
{
  while( (EXTERNAL_FLASH_SCx_SPISTAT&SC_SPITXFREE) != SC_SPITXFREE ) {
    // wait for space available
  }
  EXTERNAL_FLASH_SCx_DATA = txData;
}

uint8_t halSpiPop8(void)
{
  // WARNING!  spiPop8 must be matched 1:1 with spiPush8 calls made first
  //  or else this could spin forever!!
  while( (EXTERNAL_FLASH_SCx_SPISTAT&SC_SPIRXVAL) != SC_SPIRXVAL ) {
    // wait for byte to be avail
  }
  return EXTERNAL_FLASH_SCx_DATA;
}


// Initialization constants.  For more detail on the resulting waveforms,
// see the EM35x datasheet.
#define SPI_ORD_MSB_FIRST (0<<SC_SPIORD_BIT) // Send the MSB first
#define SPI_ORD_LSB_FIRST (1<<SC_SPIORD_BIT) // Send the LSB first

#define SPI_PHA_FIRST_EDGE (0<<SC_SPIPHA_BIT)  // Sample on first edge
#define SPI_PHA_SECOND_EDGE (1<<SC_SPIPHA_BIT) // Sample on second edge

#define SPI_POL_RISING_LEAD  (0<<SC_SPIPOL_BIT) // Leading edge is rising
#define SPI_POL_FALLING_LEAD (1<<SC_SPIPOL_BIT) // Leading edge is falling

#if    !defined(EXTERNAL_FLASH_RATE_LINEAR)       \
    || !defined(EXTERNAL_FLASH_RATE_EXPONENTIAL)

  #if    defined(EXTERNAL_FLASH_RATE_LINEAR)      \
      || defined(EXTERNAL_FLASH_RATE_EXPONENTIAL)

    #error Partial Flash serial rate definition. Please define both \

  #endif

  // configure for fastest allowable rate
  // rate = 12 MHz / ((LIN + 1) * (2^EXP))
  #define EXTERNAL_FLASH_RATE_LINEAR  (0)     // 12Mhz - FOR EM35x

  #define EXTERNAL_FLASH_RATE_EXPONENTIAL  (0)
#endif

#if EXTERNAL_FLASH_RATE_LINEAR < 0 || EXTERNAL_FLASH_RATE_LINEAR > 15
  #error EXTERNAL_FLASH_RATE_LINEAR must be between 0 and 15 (inclusive)
#endif

#if EXTERNAL_FLASH_RATE_EXPONENTIAL < 0 || EXTERNAL_FLASH_RATE_EXPONENTIAL > 15
  #error EXTERNAL_FLASH_RATE_EXPONENTIAL must be between 0 and 15 (inclusive)
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

void halEepromConfigureGPIO(void)
{
  //-----SPI Master GPIO configuration
  halGpioSetConfig(EXTERNAL_FLASH_MOSI_PIN, GPIOCFG_OUT_ALT);
  halGpioSetConfig(EXTERNAL_FLASH_MISO_PIN, GPIOCFG_IN);
  halGpioSetConfig(EXTERNAL_FLASH_SCLK_PIN, GPIOCFG_OUT_ALT);
  halGpioSetConfig(EXTERNAL_FLASH_nCS_PIN , GPIOCFG_OUT);
}


void halEepromConfigureFlashController(void)
{
  //-----SPI Master SCx configuration
  EXTERNAL_FLASH_SCx_RATELIN = EXTERNAL_FLASH_RATE_LINEAR;
  EXTERNAL_FLASH_SCx_RATEEXP = EXTERNAL_FLASH_RATE_EXPONENTIAL;
  EXTERNAL_FLASH_SCx_SPICFG  = 0;
  EXTERNAL_FLASH_SCx_SPICFG  = (1 << SC_SPIMST_BIT)|  // 4; master control bit
                          (SPI_ORD_MSB_FIRST | SPI_PHA_FIRST_EDGE | SPI_POL_RISING_LEAD);
  EXTERNAL_FLASH_SCx_MODE    = EXTERNAL_FLASH_SCx_MODE_SPI;
}

void halEepromDelayMicroseconds(uint32_t timeToDelay)
{
  halCommonDelayMicroseconds(timeToDelay);
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
