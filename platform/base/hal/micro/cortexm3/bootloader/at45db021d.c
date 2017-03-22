/*
 * File: at45db021d.c
 * Description: SPI Interface to Atmel AT45DB021D Serial Flash Memory
 * containing 264kBytes of memory.
 *
 * This file provides an interface to the AT45DB021D flash memory to allow
 * writing, reading and status polling.
 *
 * The write function uses command 82 to write data to the flash buffer
 * which then erases the page and writes the memory.
 *
 * The read function uses command D2 to read directly from memory without
 * using the buffer.
 *
 * The Ember remote storage code operates using 128 byte blocks of data. This
 * interface will write two 128 byte blocks to each remote page utilizing
 * 256 of the 264 bytes available per page. This format effectively uses
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

#if !(BAT_VERSION == THIS_DRIVER_VERSION)
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

#define AT_MANUFACTURER_ID 0x1F

// Atmel Op Codes for SPI mode 0 or 3
#define AT_OP_RD_MFG_ID 0x9F //read manufacturer ID
#define AT_OP_RD_STATUS_REG 0xD7   // status register read
#define AT_OP_RD_MEM_PG 0xD2     // memory page read
#define AT_OP_WR_TO_BUF_TO_MEM_W_ERASE 0x82   // write into buffer, then to memory, erase first
#define AT_OP_RD_MEM_TO_BUF 0x53   // memory to buffer read
// other commands not used by this driver:
//#define AT_OP_RD_BUF 0xD4        // buffer read
//#define AT_OP_ERASE_PG 0x81      // page erase
//#define AT_OP_ERASE_BL 0x50      // block erase
//#define AT_OP_COMPARE_MEM_TO_BUF 0x60   // memory to buffer compare

#define CHIP_RDY 0x80   // ready bit

// could be optionally added
#define EEPROM_WP_ON()  do { ; } while (0)  // WP pin, write protection on
#define EEPROM_WP_OFF() do { ; } while (0)  // WP pin, write protection off


//This function reads the manufacturer ID to verify this driver is
//talking to the chip this driver is written for.
static uint8_t halAT45DB021DVerifyMfgId(void)
{
  uint8_t mfgId;

  EEPROM_WP_OFF();
  EXTERNAL_FLASH_CS_ACTIVE();
  halSpiWrite(AT_OP_RD_MFG_ID);
  mfgId = halSpiRead();
  EXTERNAL_FLASH_CS_INACTIVE();
  EEPROM_WP_ON();

  //If this assert triggers, this driver is being used to talk to
  //the wrong chip.
  return (mfgId==AT_MANUFACTURER_ID)? EEPROM_SUCCESS : EEPROM_ERR_INVALID_CHIP;
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
  // Set EEPROM_POWER_PIN high as part of EEPROM init
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
  //powerup/init, delay worst case of 20ms.  (I'd much rather worry about
  //time and power consumption than potentially unstable behavior).
  //The Atmel AT45DB021D datasheet says that 20ms is the max "Power-Up Device
  //Delay before Write Allowed", and 1ms is the delay "required before the
  //device can be selected in order to perform a read operation."
  halCommonDelayMicroseconds(20000);
  
  //Make sure this driver is talking to the correct chip
  return halAT45DB021DVerifyMfgId();
}

static const HalEepromInformationType partInfo = {
  EEPROM_INFO_VERSION,
  0,  // no specific capabilities
  0,  // page erase time (not suported or needed in this driver)
  0,  // part erase time (not suported or needed in this driver)
  DEVICE_PAGE_SZ,  // page size
  DEVICE_SIZE,  // device size
  "AT45DB021D",
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

// halAT45DB021DReadStatus
//
// Read the status register, return value read
//
// return: status value
//
static uint8_t halAT45DB021DReadStatus(void)
{
  uint8_t retVal;

  EEPROM_WP_OFF();
  EXTERNAL_FLASH_CS_ACTIVE();
  halSpiWrite(AT_OP_RD_STATUS_REG);  // cmd D7
  retVal = halSpiRead();
  EXTERNAL_FLASH_CS_INACTIVE();
  EEPROM_WP_ON();

  return retVal;
}

void halEepromShutdown(void)
{
  // wait for any outstanding operations to complete before pulling the plug
  while (halAT45DB021DReadStatus() < CHIP_RDY) {
    BLDEBUG_PRINT("Poll ");
  }
  BLDEBUG_PRINT("\r\n");
  CLR_EEPROM_POWER_PIN();
}

// halAT45DB021DReadBytes
//
// Reads directly from memory, non-buffered
// address: byte address within device
// data: write the data here
// len: number of bytes to read
//
// return: EEPROM_SUCCESS
//
// This routine requires [address .. address+len-1] to fit within a device
// page; callers must enforce this, we do not check here for violators.
//
static uint8_t halAT45DB021DReadBytes(uint32_t address, uint8_t *data, uint16_t len)
{
  BLDEBUG_PRINT("ReadBytes: address:len ");
//BLDEBUG(serPutHex((uint8_t)(address >> 24)));
  BLDEBUG(serPutHex((uint8_t)(address >> 16)));
  BLDEBUG(serPutHex((uint8_t)(address >>  8)));
  BLDEBUG(serPutHex((uint8_t)(address      )));
  BLDEBUG_PRINT(":");
  BLDEBUG(serPutHexInt(len));
  BLDEBUG_PRINT("\r\n");

  // make sure chip is ready
  while (halAT45DB021DReadStatus() < CHIP_RDY) {
    BLDEBUG_PRINT("Poll ");
  }
  BLDEBUG_PRINT("\r\n");

  EEPROM_WP_OFF();
  EXTERNAL_FLASH_CS_ACTIVE();

  // write opcode for main memory page read
  halSpiWrite(AT_OP_RD_MEM_PG);  // cmd D2

  // write 24 addr bits -- converting address to Atmel 264-byte page addressing
  halSpiWrite((uint8_t)(address >> 15));
  halSpiWrite((uint8_t)(address >>  8) << 1);
  halSpiWrite((uint8_t)(address      ));

  // initialize with 32 don't care bits
  halSpiWrite(0);
  halSpiWrite(0);
  halSpiWrite(0);
  halSpiWrite(0);

  // loop reading data
  BLDEBUG_PRINT("ReadBytes: data: ");
  while(len--) {
    halInternalResetWatchDog();
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

// halAT45DB021DWriteBytes
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
static uint8_t halAT45DB021DWriteBytes(uint32_t address, const uint8_t *data,
                                     uint16_t len)
{
  BLDEBUG_PRINT("WriteBytes: address:len ");
//BLDEBUG(serPutHex((uint8_t)(address >> 24)));
  BLDEBUG(serPutHex((uint8_t)(address >> 16)));
  BLDEBUG(serPutHex((uint8_t)(address >>  8)));
  BLDEBUG(serPutHex((uint8_t)(address      )));
  BLDEBUG_PRINT(":");
  BLDEBUG(serPutHexInt(len));
  BLDEBUG_PRINT("\r\n");

  // make sure chip is ready
  while (halAT45DB021DReadStatus() < CHIP_RDY) {
    BLDEBUG_PRINT("Poll ");
  }
  BLDEBUG_PRINT("\r\n");

  if( len < DEVICE_PAGE_SZ ) { // partial flash page write
    // read current contents of page into on-chip buffer
    EXTERNAL_FLASH_CS_ACTIVE();

    // write opcode
    halSpiWrite(AT_OP_RD_MEM_TO_BUF);  // cmd 53

    // write 24 addr bits -- converting address to Atmel 264-byte page addressing
    halSpiWrite((uint8_t)(address >> 15));
    halSpiWrite((uint8_t)(address >>  8) << 1);
    halSpiWrite((uint8_t)(address      ));

    EXTERNAL_FLASH_CS_INACTIVE();

    // wait for completion
    while (halAT45DB021DReadStatus() < CHIP_RDY) {
      BLDEBUG_PRINT("BufPoll\r\n");
    }
  }

  // activate memory
  EEPROM_WP_OFF();
  EXTERNAL_FLASH_CS_ACTIVE();

  // write opcode
  halSpiWrite(AT_OP_WR_TO_BUF_TO_MEM_W_ERASE);  // cmd 82

  // write 24 addr bits -- converting address to Atmel 264-byte page addressing
  halSpiWrite((uint8_t)(address >> 15));
  halSpiWrite((uint8_t)(address >>  8) << 1);
  halSpiWrite((uint8_t)(address      ));

  // loop reading data
  BLDEBUG_PRINT("WriteBytes: data: ");
  while(len--) {
    halInternalResetWatchDog();
    halSpiWrite(*data);
    BLDEBUG(serPutHex(*data));
    BLDEBUG(serPutChar(' '));
    data++;
  }
  BLDEBUG_PRINT("\r\n");

  EXTERNAL_FLASH_CS_INACTIVE();
  EEPROM_WP_ON();

  return EEPROM_SUCCESS;
}

// test writing and reading two pages
static uint8_t halAT45DB021DTest(void)
{
  uint8_t i;
  uint8_t wrData[] = {0,1,2,3,4,5,6,7,8,9};
  uint8_t rdData[] = {0,0,0,0,0,0,0,0,0,0};

  BLDEBUG_PRINT("AT test: read status\r\n");
  BLDEBUG(serPutHex(halAT45DB021DReadStatus()));
  BLDEBUG_PRINT("\r\n");

  BLDEBUG_PRINT("AT test: write data\r\n");
  halAT45DB021DWriteBytes(0x0000, wrData, 10);
  halAT45DB021DWriteBytes(0x0080, wrData, 10);
  halAT45DB021DWriteBytes(0x0300, wrData, 10);
  halAT45DB021DWriteBytes(0x0380, wrData, 10);
  halAT45DB021DReadBytes(0x0000, rdData, 10);
  halAT45DB021DReadBytes(0x0080, rdData, 10);
  halAT45DB021DReadBytes(0x0300, rdData, 10);
  halAT45DB021DReadBytes(0x0380, rdData, 10);
  BLDEBUG_PRINT("AT test: read data ");
  for (i=0;i<10;i++)
    BLDEBUG(serPutHex(rdData[i]));
  BLDEBUG_PRINT("\r\n");

  return 0;
}

void halEepromTest(void)
{
  halAT45DB021DTest();
}

//
// ~~~~~~~~~~~~~~~~~~~~~~~~~ Standard EEPROM Interface ~~~~~~~~~~~~~~~~~~~~~~~~~
//

// halEepromRead
// address: the address in EEPROM to start reading
// data: write the data here
// len: number of bytes to read
//
// return: result of halAT45DB021DReadBytes() call(s) or EEPROM_ERR_ADDR
//
uint8_t halEepromRead(uint32_t address, uint8_t *data, uint16_t totalLength)
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
    if( (status=halAT45DB021DReadBytes(address, data, len)) != EEPROM_SUCCESS) {
      return status;
    }
    totalLength -= len;
    address += len;
    data += len;
    len = (totalLength>DEVICE_PAGE_SZ)? DEVICE_PAGE_SZ : totalLength;
  }
  return EEPROM_SUCCESS;
}

// halEepromWrite
// address: the address in EEPROM to start writing
// data: pointer to the data to write
// len: number of bytes to write
//
// return: result of halAT45DB021DWriteBytes() call(s) or EEPROM_ERR_ADDR
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
    if( (status=halAT45DB021DWriteBytes(address, data, len)) != EEPROM_SUCCESS) {
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
