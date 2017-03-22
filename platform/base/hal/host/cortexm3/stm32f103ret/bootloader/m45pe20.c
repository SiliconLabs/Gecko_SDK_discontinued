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
#include "stack/include/error.h"
#include "hal.h"


//If you need debug output, define these.
#define BLDEBUG(x)
#define BLDEBUG_PRINT(str)


//
// ~~~~~~~~~~~~~~~~~~~~~~~~~~ Generic SPI Routines ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
static uint8_t halSpiReadWrite(uint8_t txData)
{
  uint8_t rxData;

  SPI_I2S_SendData(SPI2, txData);
  while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET) {}
  rxData = (uint8_t)SPI_I2S_ReceiveData(SPI2);

  return rxData;
}

static void halSpiWrite(uint8_t txData)
{
  (uint8_t)halSpiReadWrite(txData);
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

#define MICRON_MANUFACTURER_ID 0x20

// Micron Op Codes for SPI mode 0 or 3
#define MICRON_OP_RD_MFG_ID 0x9F //read manufacturer ID
#define MICRON_OP_WRITE_ENABLE 0x06     // write enable
#define MICRON_OP_RD_STATUS_REG 0x05   // status register read
#define MICRON_OP_RD_MEM 0x03     // memory read
#define MICRON_OP_PAGE_WRITE 0x0A     // page write
#define MICRON_OP_POWERDOWN 0xB9
#define MICRON_OP_POWERUP 0xAB
// other commands not used by this driver:
//#define MICRON_OP_FAST_RD_MEM 0x0B     // fast memory read
//#define MICRON_OP_PAGE_ERASE 0xDB     // page erase
//#define MICRON_OP_SECTOR_ERASE 0xD8     // sector erase

// Write in progress bit mask for status register
#define MICRON_SR_WIP_MASK 0x01

#define EEPROM_CS_ACTIVE()                        \
  do {                                            \
    GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_RESET); \
  } while(0)
#define EEPROM_CS_INACTIVE()                    \
  do {                                          \
    GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_SET); \
  } while(0)

// could be optionally added
#define EEPROM_WP_ON()  do { ; } while (0)  // WP pin, write protection on
#define EEPROM_WP_OFF() do { ; } while (0)  // WP pin, write protection off


//This function reads the manufacturer ID to verify this driver is
//talking to the chip this driver is written for.
static uint8_t halM45PE20VerifyMfgId(void)
{
  uint8_t mfgId;

  EEPROM_WP_OFF();
  EEPROM_CS_ACTIVE();
  halSpiWrite(MICRON_OP_RD_MFG_ID);
  mfgId = halSpiRead();
  EEPROM_CS_INACTIVE();
  EEPROM_WP_ON();

  //If this assert triggers, this driver is being used to talk to
  //the wrong chip.
  return (mfgId==MICRON_MANUFACTURER_ID)?EEPROM_SUCCESS:EEPROM_ERR_INVALID_CHIP;
}

static void halM45PE20EepromPowerUp(void)
{
  //The datasheet describes timing parameters for powerup.  To be
  //the safest and impose no restrictions on further operations after
  //powerup/init, delay worst case of 10ms.  (I'd much rather worry about
  //time and power consumption than potentially unstable behavior).
  halCommonDelayMicroseconds(10000);
  
  EEPROM_WP_OFF();
  EEPROM_CS_ACTIVE();
  (uint8_t)halSpiReadWrite(MICRON_OP_POWERUP);
  EEPROM_CS_INACTIVE();
  EEPROM_WP_ON();
}

//The M45PE20 Serial Flash Memory is connected to SPI2 of the STM32 which is:
//  PB7  = DF_nSD (shutdown)
//  PB12 = DF_nCS
//  PB13 = DF_SCK
//  PB14 = DF_SO
//  PB15 = DF_SI
//SPI2 is configured for full duplex, master mode, 9MHz.
uint8_t halEepromInit(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  SPI_InitTypeDef SPI_InitStructure;

  //PB7/DF_nSD (shutdown) as output, hi, push-pull
  GPIO_WriteBit(GPIOB, GPIO_Pin_7, Bit_SET);
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  //PB12/DF_nCS as output, hi, push-pull
  GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_SET);
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  //PB13/DF_SCK as AF push-pull
  //PB15/DF_SI as AF push-pull
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  //PB14/DF_SO as input pull-up
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  //Configure SPI (but without enabling it)
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  //PCLK2/8 = 72MHz/8 = 9MHz
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI2, &SPI_InitStructure);

  //Enable SPI
  SPI_Cmd(SPI2, ENABLE);

  halM45PE20EepromPowerUp();

  //Make sure this driver is talking to the correct chip
  return halM45PE20VerifyMfgId();
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
  EEPROM_CS_ACTIVE();
  halSpiWrite(MICRON_OP_RD_STATUS_REG);  // cmd 0x05
  retVal = halSpiRead();
  EEPROM_CS_INACTIVE();
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
  //todo: toggle shutdown IO instead of just activating low power mode
  EEPROM_WP_OFF();
  EEPROM_CS_ACTIVE();
  (uint8_t)halSpiReadWrite(MICRON_OP_POWERDOWN);
  EEPROM_CS_INACTIVE();
  EEPROM_WP_ON();
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
  EEPROM_CS_ACTIVE();

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
  EEPROM_CS_INACTIVE();
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
  EEPROM_CS_ACTIVE();

  // Send write enable op
  halSpiWrite(MICRON_OP_WRITE_ENABLE);

  // Toggle CS (required for write enable to take effect)
  EEPROM_CS_INACTIVE();
  EEPROM_CS_ACTIVE();

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

  EEPROM_CS_INACTIVE();

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

const HalEepromInformationType *halEepromInfo(void)
{
  // This driver doesn't support this API
  return NULL;
}

bool halEepromBusy(void)
{
  // This driver doesn't support busy detection
  return false;
}

uint8_t halEepromErase(uint32_t address, uint32_t totalLength)
{
  // This driver doesn't support or need erasing
  return EEPROM_ERR_NO_ERASE_SUPPORT;
}
