/*
 * File: mc24aa1025.c
 * Description: I2C Interface to Microchip 24AA1025/24LC2025 Serial EEPROM
 * containing 128kBytes of memory.
 *
 * Author(s): David Iacobone, diacobone@ember.com
 *
 * Copyright 2010 by Ember Corporation. All rights reserved.                *80*
 *
 */

#include PLATFORM_HEADER
#include "stack/include/ember-types.h"
#include "hal/hal.h"
#include "hal/micro/cortexm3/memmap.h"

// abstract away which pins and serial controller are being used
#include "external-flash-gpio-select.h"

#define THIS_DRIVER_VERSION (0x010A)

#if BAT_VERSION != THIS_DRIVER_VERSION
  #error External Flash Driver must be updated to support new API requirements
#endif

//
// ~~~~~~~~~~~~~~~~~~~~~~~~ Device Specific Interface ~~~~~~~~~~~~~~~~~~~~~~~~~~
//

#define DEVICE_SIZE       (128ul * 1024ul)   // 128 kBytes
#define DEVICE_BLOCK_SZ    65536ul
#define DEVICE_BLOCK_MASK  65535ul
#define DEVICE_PAGE_SZ     128
#define DEVICE_PAGE_MASK   127
#define DEVICE_BLOCK_ADDR(addr)     ((addr) / DEVICE_BLOCK_SZ)
#define DEVICE_WORD_SIZE   (1)

#define MC_CTL_CODE        0xA0     // Control code + zero'd chip select bits
#define MC_CTL_A16_MASK    0x08     // Mask of A16 in control
#define MC_CTL_A1A0_MASK   0x06     // Mask of A1,A0 chip selects in control
#define MC_CTL_RDWR_MASK   0x01     // Mask of read/write in control
#define MC_CTL_OP_RD       0x01     // Read operation bit value
#define MC_CTL_OP_WR       0x00     // Write operation bit value

#if    !defined(EXTERNAL_FLASH_RATE_LINEAR)       \
    || !defined(EXTERNAL_FLASH_RATE_EXPONENTIAL)

  #if    defined(EXTERNAL_FLASH_RATE_LINEAR)      \
      || defined(EXTERNAL_FLASH_RATE_EXPONENTIAL)

    #error Partial Flash serial rate definition. Please define both \
           EXTERNAL_FLASH_RATE_LINEAR and EXTERNAL_FLASH_RATE_EXPONENTIAL when \
           specifying a custom rate.

  #endif
  // default to 400kbps operation
  #define EXTERNAL_FLASH_RATE_LINEAR        14
  #define EXTERNAL_FLASH_RATE_EXPONENTIAL   1

#endif

#if EXTERNAL_FLASH_RATE_LINEAR < 0 || EXTERNAL_FLASH_RATE_LINEAR > 15
  #error EXTERNAL_FLASH_RATE_LINEAR must be between 0 and 15 (inclusive)
#endif

uint8_t halEepromInit(void)
{
  //-----SCx I2C Master GPIO configuration
  halGpioSetConfig(EXTERNAL_FLASH_SDA_PIN, GPIOCFG_OUT_ALT_OD);
  halGpioSetConfig(EXTERNAL_FLASH_SCL_PIN, GPIOCFG_OUT_ALT_OD);

  // Initialize serial controller SCx for I2C operation.
  EXTERNAL_FLASH_SCx_RATELIN =  EXTERNAL_FLASH_RATE_LINEAR;
  EXTERNAL_FLASH_SCx_RATEEXP =  EXTERNAL_FLASH_RATE_EXPONENTIAL;
  EXTERNAL_FLASH_SCx_TWICTRL1 =  0;   // start from a clean state
  EXTERNAL_FLASH_SCx_TWICTRL2 =  0;   // start from a clean state
  EXTERNAL_FLASH_SCx_MODE =  EXTERNAL_FLASH_SCx_MODE_I2C;

  return EEPROM_SUCCESS;
}

static const HalEepromInformationType partInfo = {
  EEPROM_INFO_VERSION,
  0,  // no specific capabilities
  0,  // page erase time (not suported or needed in this driver)
  0,  // part erase time (not suported or needed in this driver)
  DEVICE_PAGE_SZ,  // page size
  DEVICE_SIZE,  // device size
  "MC24AA1025",
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

void halEepromShutdown(void)
{
  //not implemented
}

bool halEepromBusy(void)
{
  // This driver doesn't support busy detection
  return false;
}

#define SEND_BYTE(data) do{ EXTERNAL_FLASH_SCx_DATA=(data); EXTERNAL_FLASH_SCx_TWICTRL1 |= SC_TWISEND; }while(0)

#define WAIT_CMD_FIN()  do{}while((EXTERNAL_FLASH_SCx_TWISTAT&SC_TWICMDFIN)!=SC_TWICMDFIN)
#define WAIT_TX_FIN()   do{}while((EXTERNAL_FLASH_SCx_TWISTAT&SC_TWITXFIN)!=SC_TWITXFIN)
#define WAIT_RX_FIN()   do{}while((EXTERNAL_FLASH_SCx_TWISTAT&SC_TWIRXFIN)!=SC_TWIRXFIN)

// The 24AA1025 has two addressable blocks from 0x00000 to 0x0FFFF and
// 0x10000 to 0x1FFFF.  Reads must occur within a single block.
// Hence this routine requires [address .. address+len-1] to fit within a
// device block; callers must enforce this, we do not check here for violators.
//
static uint8_t halMc24AA1025ReadBytes(uint32_t address, uint8_t *data, uint16_t len)
{
  uint16_t i;
  uint8_t control = MC_CTL_CODE;

  control |= ((uint8_t)DEVICE_BLOCK_ADDR(address) * MC_CTL_A16_MASK);

  // send start cmd; loop if rx'ing nak 'cause slave is busy
  do {
    EXTERNAL_FLASH_SCx_TWICTRL1 |= SC_TWISTART;            // send start
    WAIT_CMD_FIN();

    SEND_BYTE(control | MC_CTL_OP_WR);      // send the control byte to set address
    WAIT_TX_FIN();
  }while((EXTERNAL_FLASH_SCx_TWISTAT&SC_TWIRXNAK) == SC_TWIRXNAK);

  SEND_BYTE((uint8_t)(address >> 8));         // send the address high byte
  WAIT_TX_FIN();

  SEND_BYTE((uint8_t)address);                // send the address low byte
  WAIT_TX_FIN();

  EXTERNAL_FLASH_SCx_TWICTRL1 |= SC_TWISTART;              // send start
  WAIT_CMD_FIN();

  SEND_BYTE(control | MC_CTL_OP_RD);        // send control byte for read
  WAIT_TX_FIN();

  // loop receiving the data
  for (i=0; i<len; i++) {
    halInternalResetWatchDog();

    if (i < (len - 1))
      EXTERNAL_FLASH_SCx_TWICTRL2 |= SC_TWIACK;            // ack on receipt of data
    else
      EXTERNAL_FLASH_SCx_TWICTRL2 &= ~SC_TWIACK;           // don't ack if last one

    EXTERNAL_FLASH_SCx_TWICTRL1 |= SC_TWIRECV;             // set to receive
    WAIT_RX_FIN();
    data[i] = EXTERNAL_FLASH_SCx_DATA;                     // receive data
  }

  EXTERNAL_FLASH_SCx_TWICTRL1 |= SC_TWISTOP;               // send STOP
  WAIT_CMD_FIN();

  return EEPROM_SUCCESS;
}

// The 24AA1025 has two addressable blocks from 0x00000 to 0x0FFFF and
// 0x10000 to 0x1FFFF, but writes must occur entirely within a single
// 128 byte page.
// Hence this routine requires [address .. address+len-1] to fit within a
// device page; callers must enforce this, we do not check here for violators.
//
static uint8_t halMc24AA1025WriteBytes(uint32_t address, const uint8_t *data,
                                     uint16_t len)
{
  uint16_t i;
  uint8_t control = MC_CTL_CODE;

  control |= ((uint8_t)DEVICE_BLOCK_ADDR(address) * MC_CTL_A16_MASK);

  // send start cmd; loop if rx'ing nak 'cause slave is busy
  do {
    EXTERNAL_FLASH_SCx_TWICTRL1 |= SC_TWISTART;            // send start
    WAIT_CMD_FIN();

    SEND_BYTE(control | MC_CTL_OP_WR);      // send the control byte to set address
    WAIT_TX_FIN();
  }while((EXTERNAL_FLASH_SCx_TWISTAT&SC_TWIRXNAK) == SC_TWIRXNAK);

  SEND_BYTE((uint8_t)(address >> 8));         // send the address high byte
  WAIT_TX_FIN();

  SEND_BYTE((uint8_t)address);                // send the address low byte
  WAIT_TX_FIN();

  // loop sending the data
  for (i=0; i<len; i++) {
    halInternalResetWatchDog();
    SEND_BYTE(data[i]);                     // write data
    WAIT_TX_FIN();
  }

  EXTERNAL_FLASH_SCx_TWICTRL1 |= SC_TWISTOP;               // send STOP
  WAIT_CMD_FIN();

  return EEPROM_SUCCESS;
}

void halEepromTest(void)
{
  // No test function for this driver.
}

//
// ~~~~~~~~~~~~~~~~~ Standard EEPROM Interface ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//

// halEepromRead
// address: the address in EEPROM to start reading
// data: pointer to the data buffer to read into
// len: number of bytes to read
//
// return: result of halMc24AA1025ReadBytes() call(s) or EEPROM_ERR_ADDR
//
// This function handles unaligned reads across device block boundaries
//
uint8_t halEepromRead(uint32_t address, uint8_t *data, uint16_t totalLength)
{
  uint32_t nextBlockAddr;
  uint16_t len;
  uint8_t status;

  if( address > DEVICE_SIZE || (address + totalLength) > DEVICE_SIZE)
    return EEPROM_ERR_ADDR;

  if( address & DEVICE_BLOCK_MASK) {
    // handle unaligned first block
    nextBlockAddr = (address & (~DEVICE_BLOCK_MASK)) + DEVICE_BLOCK_SZ;
    if((address + totalLength) < nextBlockAddr){
      // fits all within same block
      len = totalLength;
    } else {
      len = (uint16_t) (nextBlockAddr - address);
    }
  } else {
//  len = (totalLength>DEVICE_BLOCK_SZ)? DEVICE_BLOCK_SZ : totalLength;
    len = totalLength; // on this device, uint16_t totalLength cannot exceed DEVICE_BLOCK_SZ
  }
  while(totalLength) {
    if( (status=halMc24AA1025ReadBytes(address, data, len)) != EEPROM_SUCCESS) {
      return status;
    }
    totalLength -= len;
    address += len;
    data += len;
//  len = (totalLength>DEVICE_BLOCK_SZ)? DEVICE_BLOCK_SZ : totalLength;
    len = totalLength; // on this device, uint16_t totalLength cannot exceed DEVICE_BLOCK_SZ
  }
  return EEPROM_SUCCESS;
}

// halEepromWrite
// address: the address in EEPROM to start writing
// data: pointer to the data to write
// len: number of bytes to write
//
// return: result of halMc24AA1025WriteBytes() call(s) or EEPROM_ERR_ADDR
//
// This function handles unaligned writes across device page boundaries
//
uint8_t halEepromWrite(uint32_t address, const uint8_t *data, uint16_t totalLength)
{
  uint32_t nextPageAddr;
  uint16_t len;
  uint8_t status;

  if( address > DEVICE_SIZE || (address + totalLength) > DEVICE_SIZE)
    return EEPROM_ERR_ADDR;

  if( address & DEVICE_PAGE_MASK) {
    // handle unaligned first page
    nextPageAddr = (address & (~DEVICE_PAGE_MASK)) + DEVICE_PAGE_SZ;
    if((address + totalLength) < nextPageAddr){
      // fits all within first page
      len = totalLength;
    } else {
      len = (uint16_t) (nextPageAddr - address);
    }
  } else {
    len = (totalLength>DEVICE_PAGE_SZ)? DEVICE_PAGE_SZ : totalLength;
  }
  while(totalLength) {
    if( (status=halMc24AA1025WriteBytes(address, data, len)) != EEPROM_SUCCESS) {
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
