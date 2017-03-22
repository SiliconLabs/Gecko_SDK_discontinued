/*
 * File: m45pe20.c
 * Description: SPI Interface to Micron/Numonyx M45PE20 Serial Flash Memory
 * containing 256kBytes of memory.
 *
 * This file provides an interface to the M45PE20 flash memory to allow
 * writing, reading and status polling.
 *
 * The write function uses command 0x0A to write data to the flash buffer
 * which then erases the page and writes the memory.
 *
 * The read function uses command 0x03 to read directly from memory without
 * using the buffer.
 *
 * The Ember remote storage code operates using 256 byte blocks of data. This
 * interface will write one 256 byte block to each remote page utilizing
 * 256 of the 256 bytes available per page. This format effectively uses
 * 256kBytes of memory.
 *
 * Copyright 2010 by Ember Corporation. All rights reserved.                *80*
 *
 */

#include PLATFORM_HEADER
#include "hal/micro/bootloader-eeprom.h"
#include "bootloader-common.h"
#include "bootloader-serial.h"
#include "hal/micro/micro.h"
#include BOARD_HEADER
#include "hal/micro/cortexm3/memmap.h"

// abstract away which pins and serial controller are being used
#include "external-flash-gpio-select.h"

#define THIS_DRIVER_VERSION (0x010A)

#if BAT_VERSION != THIS_DRIVER_VERSION
  #error External Flash Driver must be updated to support new API requirements
#endif

//
// ~~~~~~~~~~~~~~~~~~~~~~~~~~ Generic SPI Routines ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
static uint8_t halSpiReadWrite(uint8_t txData)
{
  uint8_t rxData;

  EXTERNAL_FLASH_SCx_DATA = txData;
  while( (EXTERNAL_FLASH_SCx_SPISTAT&SC_SPITXIDLE) != SC_SPITXIDLE) {} //wait to finish
  if ((EXTERNAL_FLASH_SCx_SPISTAT&SC_SPIRXVAL) != SC_SPIRXVAL)
    rxData = 0xff;
  else
    rxData = EXTERNAL_FLASH_SCx_DATA;

  return rxData;
}

static void halSpiWrite(uint8_t txData)
{
  EXTERNAL_FLASH_SCx_DATA = txData;
  while( (EXTERNAL_FLASH_SCx_SPISTAT&SC_SPITXIDLE) != SC_SPITXIDLE) {} //wait to finish
  (void) EXTERNAL_FLASH_SCx_DATA;
}

static uint8_t halSpiRead(void)
{
  return halSpiReadWrite(0xFF);
}

//
// ~~~~~~~~~~~~~~~~~~~~~~~~ Device Specific Interface ~~~~~~~~~~~~~~~~~~~~~~~~~~
//

#define DEVICE_SIZE       (256ul * 1024ul)   // 256 kBytes

#define DEVICE_PAGE_SZ     256
#define DEVICE_PAGE_MASK   255
#define DEVICE_WORD_SIZE   (1)

#define MICRON_MANUFACTURER_ID 0x20

// Micron Op Codes for SPI mode 0 or 3
#define MICRON_OP_RD_MFG_ID 0x9F //read manufacturer ID
#define MICRON_OP_WRITE_ENABLE 0x06     // write enable
#define MICRON_OP_RD_STATUS_REG 0x05   // status register read
#define MICRON_OP_RD_MEM 0x03     // memory read
#define MICRON_OP_PAGE_WRITE 0x0A     // page write
// other commands not used by this driver:
//#define MICRON_OP_FAST_RD_MEM 0x0B     // fast memory read
//#define MICRON_OP_PAGE_ERASE 0xDB     // page erase
//#define MICRON_OP_SECTOR_ERASE 0xD8     // sector erase

// Write in progress bit mask for status register
#define MICRON_SR_WIP_MASK 0x01

// could be optionally added
#define EEPROM_WP_ON()  do { ; } while (0)  // WP pin, write protection on
#define EEPROM_WP_OFF() do { ; } while (0)  // WP pin, write protection off


//This function reads the manufacturer ID to verify this driver is
//talking to the chip this driver is written for.
static uint8_t halM45PE20VerifyMfgId(void)
{
  uint8_t mfgId;

  EEPROM_WP_OFF();
  EXTERNAL_FLASH_CS_ACTIVE();
  halSpiWrite(MICRON_OP_RD_MFG_ID);
  mfgId = halSpiRead();
  EXTERNAL_FLASH_CS_INACTIVE();
  EEPROM_WP_ON();

  //If this assert triggers, this driver is being used to talk to
  //the wrong chip.
  return (mfgId==MICRON_MANUFACTURER_ID)?EEPROM_SUCCESS:EEPROM_ERR_INVALID_CHIP;
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
           EXTERNAL_FLASH_RATE_LINEAR and EXTERNAL_FLASH_RATE_EXPONENTIAL when \
           specifying a custom rate.

  #endif
  // configure for fastest allowable rate
  // rate = 12 MHz / ((LIN + 1) * (2^EXP))
  #define EXTERNAL_FLASH_RATE_LINEAR  (0)     // 12Mhz - FOR EM35x

  #define EXTERNAL_FLASH_RATE_EXPONENTIAL  (0)

#endif

#if EXTERNAL_FLASH_RATE_LINEAR < 0 || EXTERNAL_FLASH_RATE_LINEAR > 15
  #error EXTERNAL_FLASH_RATE_LINEAR must be between 0 and 15 (inclusive)
#endif

uint8_t halEepromInit(void)
{
  // Set EEPROM_ENABLE_PIN high as part of EEPROM init
  CONFIGURE_EEPROM_POWER_PIN();
  SET_EEPROM_POWER_PIN();

  //-----SCx SPI Master GPIO configuration
  halGpioSetConfig(EXTERNAL_FLASH_MOSI_PIN, GPIOCFG_OUT_ALT);
  halGpioSetConfig(EXTERNAL_FLASH_MISO_PIN, GPIOCFG_IN);
  halGpioSetConfig(EXTERNAL_FLASH_SCLK_PIN, GPIOCFG_OUT_ALT);
  halGpioSetConfig(EXTERNAL_FLASH_nCS_PIN , GPIOCFG_OUT);

  EXTERNAL_FLASH_SCx_RATELIN = EXTERNAL_FLASH_RATE_LINEAR;
  EXTERNAL_FLASH_SCx_RATEEXP = EXTERNAL_FLASH_RATE_EXPONENTIAL;
  EXTERNAL_FLASH_SCx_SPICFG  =  0;
  EXTERNAL_FLASH_SCx_SPICFG =  (1 << SC_SPIMST_BIT)|  // 4; master control bit
                               (SPI_ORD_MSB_FIRST | SPI_PHA_FIRST_EDGE | SPI_POL_RISING_LEAD);
  EXTERNAL_FLASH_SCx_MODE   =  EXTERNAL_FLASH_SCx_MODE_SPI;

  //The datasheet describes timing parameters for powerup.  To be
  //the safest and impose no restrictions on further operations after
  //powerup/init, delay worst case of 10ms.  (I'd much rather worry about
  //time and power consumption than potentially unstable behavior).
  halCommonDelayMicroseconds(10000);
  
  //Make sure this driver is talking to the correct chip
  return halM45PE20VerifyMfgId();
}

static const HalEepromInformationType partInfo = {
  EEPROM_INFO_VERSION,
  0,  // no specific capabilities
  0,  // page erase time (not suported or needed in this driver)
  0,  // part erase time (not suported or needed in this driver)
  DEVICE_PAGE_SZ,  // page size
  DEVICE_SIZE,  // device size
  "M45PE20",
  DEVICE_WORD_SIZE // word size in bytes
};

const HalEepromInformationType *halEepromInfo(void)
{
  return &partInfo;
}

uint32_t halEepromSize(void)
{
  return halEepromInfo()->partSize;
}

bool halEepromBusy(void)
{
  // This driver doesn't support busy detection
  return false;
}


// halM45PE20ReadStatus
//
// Read the status register, return value read
//
// return: status value
//
static uint8_t halM45PE20ReadStatus(void)
{
  uint8_t retVal;

  EEPROM_WP_OFF();
  EXTERNAL_FLASH_CS_ACTIVE();
  halSpiWrite(MICRON_OP_RD_STATUS_REG);  // cmd 0x05
  retVal = halSpiRead();
  EXTERNAL_FLASH_CS_INACTIVE();
  EEPROM_WP_ON();

  return retVal;
}

void halEepromShutdown(void)
{
  // wait for any outstanding operations to complete before pulling the plug
  while(halM45PE20ReadStatus() & MICRON_SR_WIP_MASK) {
    BLDEBUG_PRINT("Poll ");
  }
  BLDEBUG_PRINT("\r\n");

  CLR_EEPROM_POWER_PIN();
}

// halM45PE20ReadBytes
//
// Reads directly from memory, non-buffered
// address: byte address within device
// data: write the data here
// len: number of bytes to read
//
// return: EEPROM_SUCCESS
//
// This routine requires [address .. address+len-1] to fit within the device;
// callers must enforce this, we do not check here for violators.
// This device handles reading unaligned blocks
//
static uint8_t halM45PE20ReadBytes(uint32_t address, uint8_t *data, uint16_t len)
{
  BLDEBUG_PRINT("ReadBytes: address:len ");
//BLDEBUG(serPutHex((uint8_t)(address >> 24)));
  BLDEBUG(serPutHex((uint8_t)(address >> 16)));
  BLDEBUG(serPutHex((uint8_t)(address >>  8)));
  BLDEBUG(serPutHex((uint8_t)(address      )));
  BLDEBUG_PRINT(":");
  BLDEBUG(serPutHexInt(len));
  BLDEBUG_PRINT("\r\n");

  // Make sure EEPROM is not in a write cycle
  while(halM45PE20ReadStatus() & MICRON_SR_WIP_MASK) {
    BLDEBUG_PRINT("Poll ");
  }
  BLDEBUG_PRINT("\r\n");

  EEPROM_WP_OFF();
  EXTERNAL_FLASH_CS_ACTIVE();

  // write opcode for main memory read
  halSpiWrite(MICRON_OP_RD_MEM);  // cmd 0x03

  // write 24 addr bits
  halSpiWrite((uint8_t)(address >> 16));
  halSpiWrite((uint8_t)(address >>  8));
  halSpiWrite((uint8_t)(address      ));

  // loop reading data
  BLDEBUG_PRINT("ReadBytes: data: ");
  while(len--) {
    halResetWatchdog();
    *data = halSpiRead();
    BLDEBUG(serPutHex(*data));
    BLDEBUG(serPutChar(' '));
    data++;
  }
  BLDEBUG_PRINT("\r\n");
  EXTERNAL_FLASH_CS_INACTIVE();
  EEPROM_WP_ON();

  return EEPROM_SUCCESS;
}

// halM45PE20WriteBytes
// address: byte address within device
// data: data to write
// len: number of bytes to write
//
// Create the flash address from page and byte address params. Writes to
// memory thru buffer with page erase.
// This routine requires [address .. address+len-1] to fit within a device
// page; callers must enforce this, we do not check here for violators.
//
// return: EEPROM_SUCCESS
//
static uint8_t halM45PE20WriteBytes(uint32_t address, const uint8_t *data, uint16_t len)
{
  BLDEBUG_PRINT("WriteBytes: address:len ");
//BLDEBUG(serPutHex((uint8_t)(address >> 24)));
  BLDEBUG(serPutHex((uint8_t)(address >> 16)));
  BLDEBUG(serPutHex((uint8_t)(address >>  8)));
  BLDEBUG(serPutHex((uint8_t)(address      )));
  BLDEBUG_PRINT(":");
  BLDEBUG(serPutHexInt(len));
  BLDEBUG_PRINT("\r\n");

  // Make sure EEPROM is not in a write cycle
  while(halM45PE20ReadStatus() & MICRON_SR_WIP_MASK) {
    BLDEBUG_PRINT("Poll ");
  }
  BLDEBUG_PRINT("\r\n");

  // activate memory
  EEPROM_WP_OFF();
  EXTERNAL_FLASH_CS_ACTIVE();

  // Send write enable op
  halSpiWrite(MICRON_OP_WRITE_ENABLE);

  // Toggle CS (required for write enable to take effect)
  EXTERNAL_FLASH_CS_INACTIVE();
  EXTERNAL_FLASH_CS_ACTIVE();

  // send page write command
  halSpiWrite(MICRON_OP_PAGE_WRITE);  // cmd 0x0A

  // write 24 addr bits
  halSpiWrite((uint8_t)(address >> 16));
  halSpiWrite((uint8_t)(address >>  8));
  halSpiWrite((uint8_t)(address      ));

  // loop reading data
  BLDEBUG_PRINT("WriteBytes: data: ");
  while(len--) {
    halResetWatchdog();
    halSpiWrite(*data);
    BLDEBUG(serPutHex(*data));
    BLDEBUG(serPutChar(' '));
    data++;
  }
  BLDEBUG_PRINT("\r\n");

  EXTERNAL_FLASH_CS_INACTIVE();

  //Wait until EEPROM finishes write cycle
  while(halM45PE20ReadStatus() & MICRON_SR_WIP_MASK);

  EEPROM_WP_ON();

  return EEPROM_SUCCESS;
}

void halEepromTest(void)
{
  // No test function for this driver.
}

//
// ~~~~~~~~~~~~~~~~~~~~~~~~~ Standard EEPROM Interface ~~~~~~~~~~~~~~~~~~~~~~~~~
//

// halEepromRead
// address: the address in EEPROM to start reading
// data: write the data here
// len: number of bytes to read
//
// return: result of halM45PE20ReadBytes() call(s) or EEPROM_ERR_ADDR
//
// NOTE: We don't need to worry about handling unaligned blocks in this
// function since the M45PE20 can read continuously through its memory.
//
uint8_t halEepromRead(uint32_t address, uint8_t *data, uint16_t totalLength)
{
  if( address > DEVICE_SIZE || (address + totalLength) > DEVICE_SIZE)
    return EEPROM_ERR_ADDR;

  return halM45PE20ReadBytes( address, data, totalLength);
}

// halEepromWrite
// address: the address in EEPROM to start writing
// data: pointer to the data to write
// len: number of bytes to write
//
// return: result of halM45PE20WriteBytes() call(s) or EEPROM_ERR_ADDR
//
uint8_t halEepromWrite(uint32_t address, const uint8_t *data, uint16_t totalLength)
{
  uint32_t nextPageAddr;
  uint16_t len;
  uint8_t status;

  if( address > DEVICE_SIZE || (address + totalLength) > DEVICE_SIZE)
    return EEPROM_ERR_ADDR;

  if( address & DEVICE_PAGE_MASK) {
    // handle unaligned first block
    nextPageAddr = (address & (~DEVICE_PAGE_MASK)) + DEVICE_PAGE_SZ;
    if((address + totalLength) < nextPageAddr){
      // fits all within first block
      len = totalLength;
    } else {
      len = (uint16_t) (nextPageAddr - address);
    }
  } else {
    len = (totalLength>DEVICE_PAGE_SZ)? DEVICE_PAGE_SZ : totalLength;
  }
  while(totalLength) {
    if( (status=halM45PE20WriteBytes(address, data, len)) != EEPROM_SUCCESS) {
      return status;
    }
    totalLength -= len;
    address += len;
    data += len;
    len = (totalLength>DEVICE_PAGE_SZ)? DEVICE_PAGE_SZ : totalLength;
  }
  return EEPROM_SUCCESS;
}

uint8_t halEepromErase(uint32_t address, uint32_t totalLength)
{
  // This driver doesn't support or need erasing
  return EEPROM_ERR_NO_ERASE_SUPPORT;
}
