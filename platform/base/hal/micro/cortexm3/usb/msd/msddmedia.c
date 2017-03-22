/**************************************************************************//**
 * @file  msddmedia.c
 * @brief Media interface for Mass Storage class Device (MSD).
 * @author Nathaniel Ting
 * @version 3.20.1
 *******************************************************************************
 * @section License
 * Copyright 2013 by Silicon Labs. All rights reserved.                     *80*
 *****************************************************************************/

#include PLATFORM_HEADER
#include "stack/include/ember.h"
#include "hal/hal.h"
#include "hal/micro/cortexm3/usb/em_usb.h"
#include "msdd.h"
#include "msddmedia.h"

#if ( MSD_MEDIA == MSD_SRAM_MEDIA )

  /* Figure out if the SRAM is large enough */
  // #if ( SRAM_SIZE < (128*1024) )
  // #error "SRAM based media can only be used on devices with 128K SRAM size."
  // #endif

  #define MEDIA_SIZE (40*1024)
  EFM32_ALIGN(4)
  static uint8_t storage[ MEDIA_SIZE ];
#elif ( MSD_MEDIA == MSD_BOOTLOAD_MEDIA )
  // #include "hal/micro/bootloader-interface.h"
  HalEepromInformationType const *eepromInfo;

  static uint32_t eeprom_pagesize;
  static uint16_t eeprom_capabilities;
    struct
    {
      uint8_t *pPageBase;
      bool    pendingWrite;
    } flashStatus;

  uint32_t addr;
  uint32_t writesize;
  uint32_t pageAddr;
  uint32_t erasesize;

  #define FLASH_PAGESIZE 4096
  STATIC_UBUF( flashPageBuf, FLASH_PAGESIZE );
#else
  #error "Unsupported media definition."

#endif

//FAT offsets
#define ROOT_DIRECTORY_ADDRESS    0xE00
#define DIR_ENTRY_SIZE            32
#define DIR_ENTRY_ATTRIBUTE_OFFS  11
#define DIR_ENTRY_EXTENSION_OFFS  8
#define DIR_ENTRY_FILESIZE_OFFS   0x1C
#define DIR_ENTRY_STARTCLUST_OFFS 0x1A
#define CLUST_2_ADDR( x )         (0x1800 + ( 512 * x ))

static uint32_t numSectors;

static uint8_t minimalMBR[] __attribute__ ((aligned(4))) =
{
  0xeb, 0x3c, 0x90, 0x6d, 0x6b, 0x64, 0x6f, 0x73,
  0x66, 0x73, 0x00, 0x00, 0x02, 0x01, 0x01, 0x00,
  0x01, 0x70, 0x00, 0x00, 0x08, 0xf8, 0x06, 0x00,///19-20 = # of sectors
  0x20, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x29, 0xe8,
  0x96, 0x3d, 0x18, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x46, 0x41,
  0x54, 0x31, 0x32, 0x20, 0x20, 0x20, 0xff, 0xff
};

/**************************************************************************//**
 * @brief
 *   Check if a media access is legal, prepare for later data transmissions.
 *
 * @param[in] pCmd
 *   Points to a MSDD_CmdStatus_TypeDef structure which holds info about the
 *   current transfer.
 *
 * @param[in] lba
 *   Media "Logical Block Address".
 *
 * @param[in] sectors
 *   Number of SECTOR_SIZE byte sectors to transfer.
 *
 * @return
 *   True if legal access, false otherwise.
 *****************************************************************************/
bool MSDDMEDIA_CheckAccess( MSDD_CmdStatus_TypeDef *pCmd,
                            uint32_t lba, uint32_t sectors )
{
  if ( ( lba + sectors ) > numSectors )
    return false;

  #if ( MSD_MEDIA == MSD_SRAM_MEDIA )
    pCmd->pData    = &storage[ lba * SECTOR_SIZE ];
    pCmd->xferType = XFER_MEMORYMAPPED;
  #endif

  #if ( MSD_MEDIA == MSD_BOOTLOAD_MEDIA )
    pCmd->lba   = lba;
    pCmd->xferType = XFER_INDIRECT;
    pCmd->maxBurst = MEDIA_BUFSIZ;
  #endif

  pCmd->xferLen = sectors * SECTOR_SIZE;

  return true;
}

/**************************************************************************//**
 * @brief
 *   Format storage device as FAT12
 *****************************************************************************/
void MSDDMEDIA_FATFormat( void )
{

  /* Prepare FAT12 image */
  
  #if ( MSD_MEDIA == MSD_BOOTLOAD_MEDIA )
  // format could take a long time (especially errasing data flash).
  // disable watchdog to prevent reset while formatting
  halInternalDisableWatchDog(MICRO_DISABLE_WATCH_DOG_KEY);

  // Erase data flash
  halEepromErase(0,eepromInfo->partSize);
  
  // uint16_t *MBRsectors = (uint16_t *)&minimalMBR[19];
  minimalMBR[19]=(numSectors&0xFF);
  minimalMBR[20]=((numSectors>>8)&0xFF);

  // Write Master Boot Record
  halEepromWrite( 0x0, minimalMBR, sizeof( minimalMBR ));
  flashPageBuf[0] = 0xFF;
  flashPageBuf[1] = 0xFF;
  flashPageBuf[2] = 0x55;
  flashPageBuf[3] = 0xAA;
  flashPageBuf[4] = 0xF8;
  flashPageBuf[5] = 0xFF;
  flashPageBuf[6] = 0xFF;
  flashPageBuf[7] = 0x00;
  halEepromWrite( 0x01FC, flashPageBuf, 8 );
  memset( flashPageBuf, 0, sizeof(flashPageBuf) );
  halEepromWrite( 0x0204, flashPageBuf, 0x9FC );
  halEepromWrite( 0x0C00, flashPageBuf, 0x1000 );

  // re-enable watchdog
  halInternalEnableWatchDog();
  #endif
}

/**************************************************************************//**
 * @brief
 *   Returns filesize if image can be found, else returns 0
 *
 * @param[in] pSrc
 *   Pointer to image
 *
 * @return
 *   Image size
 *****************************************************************************/
uint32_t MSDDMEDIA_FindImage( int *pSrc )
{
  uint32_t  fileSize;
  int       i, cluster;
  uint8_t buffer [512];
  uint8_t *p;
  p = buffer;
  #if ( MSD_MEDIA == MSD_BOOTLOAD_MEDIA )

  halEepromInit();
  halEepromRead(ROOT_DIRECTORY_ADDRESS, buffer, 512);

  for ( i=0; i<=112; i++ )
  {
    if ( i == 112 )
      return 0;           /* Fatal error, no file found. */

    //reject if file directory entry is invalid/empty
    if ( (p[ DIR_ENTRY_ATTRIBUTE_OFFS ] != 0x0F) && *p != 0xE5  )
    {
      fileSize  = p[ DIR_ENTRY_FILESIZE_OFFS ];
      fileSize |= p[ DIR_ENTRY_FILESIZE_OFFS + 1 ] << 8;
      fileSize |= p[ DIR_ENTRY_FILESIZE_OFFS + 2 ] << 16;
      fileSize |= p[ DIR_ENTRY_FILESIZE_OFFS + 3 ] << 24;

      // Look for file with a nonzero filesize and "EBL" extension
      if ((p[DIR_ENTRY_EXTENSION_OFFS  ] == 'E')   &&
          (p[DIR_ENTRY_EXTENSION_OFFS + 1] == 'B') &&
          (p[DIR_ENTRY_EXTENSION_OFFS + 2] == 'L') &&
          fileSize > 0)
      {
       break;
      }
    }

    p += DIR_ENTRY_SIZE;
  }

  cluster   = p[ DIR_ENTRY_STARTCLUST_OFFS ];
  cluster  |= p[ DIR_ENTRY_STARTCLUST_OFFS + 1 ] << 8;

  *pSrc = CLUST_2_ADDR( cluster );

  #endif
  return fileSize;
}

/**************************************************************************//**
 * @brief
 *   Get number of SECTOR_SIZE byte sectors on the media.
 *
 * @return
 *   Number of sectors on media.
 *****************************************************************************/
uint32_t MSDDMEDIA_GetSectorCount( void )
{
  return numSectors;
}

/**************************************************************************//**
 * @brief
 *   Initialize the storage media interface.
 *****************************************************************************/
bool MSDDMEDIA_Init( void )
{
  #if ( MSD_MEDIA == MSD_SRAM_MEDIA )
    numSectors = MEDIA_SIZE / SECTOR_SIZE;
  #endif

  #if ( MSD_MEDIA == MSD_BOOTLOAD_MEDIA )
    assert ( halEepromInit() == EEPROM_SUCCESS );
    eepromInfo = halEepromInfo();

    eeprom_pagesize = (eepromInfo == NULL)? 0 : eepromInfo->pageSize;
    eeprom_capabilities = (eepromInfo == NULL)? 0 : eepromInfo->capabilitiesMask;

    // numSectors = eepromInfo->partSize / SECTOR_SIZE;
    numSectors = halEepromSize() / SECTOR_SIZE;
  #endif // MSD_MEDIA == MSD_BOOTLOAD_MEDIA

  return true;
}

/**************************************************************************//**
 * @brief
 *   Read from indirectly accessed media.
 *
 * @param[in] pCmd
 *   Points to a MSDD_CmdStatus_TypeDef structure which holds info about the
 *   current transfer.
 *
 * @param[in] data
 *   Pointer to data buffer.
 *
 * @param[in] sectors
 *   Number of SECTOR_SIZE byte sectors to read from media.
 *****************************************************************************/
void MSDDMEDIA_Read( MSDD_CmdStatus_TypeDef *pCmd, uint8_t *data, uint32_t sectors )
{
  #if ( MSD_MEDIA == MSD_SRAM_MEDIA )
    (void)pCmd;
    (void)data;
    (void)sectors;
  #endif

  #if ( MSD_MEDIA == MSD_BOOTLOAD_MEDIA )
    halEepromRead(pCmd->lba * SECTOR_SIZE, data, sectors * SECTOR_SIZE );
  #endif
}

/**************************************************************************//**
 * @brief
 *   Write to indirectly accessed media.
 *
 * @param[in] pCmd
 *   Points to a MSDD_CmdStatus_TypeDef structure which holds info about the
 *   current transfer.
 *
 * @param[in] data
 *   Pointer to data buffer.
 *
 * @param[in] sectors
 *   Number of SECTOR_SIZE byte sectors to write to media.
 *****************************************************************************/
void MSDDMEDIA_Write( MSDD_CmdStatus_TypeDef *pCmd, uint8_t *data, uint32_t sectors )
{
  #if ( MSD_MEDIA == MSD_SRAM_MEDIA )
  (void)pCmd;
  (void)data;
  (void)sectors;
  #endif

  #if ( MSD_MEDIA == MSD_BOOTLOAD_MEDIA )
  uint32_t offset=0;
  uint16_t remaining;
  
  // split into pages to erase/write
  addr = pCmd->lba*SECTOR_SIZE;
  writesize = sectors * SECTOR_SIZE;

  // Try writing first. Will fail if any pages need to be erased
  if (halEepromWrite(addr, data, writesize) == EEPROM_SUCCESS)
    return;

  if((eeprom_capabilities&EEPROM_CAPABILITIES_PAGE_ERASE_REQD) && eeprom_pagesize) 
  {    
    // align addr with page divisions
    pageAddr = (addr & ~(eeprom_pagesize - 1));

    // check to see if address lines up with page, if so, then erase freely
    // if not, save page first in flashPageBuf, then erase
    if (pageAddr!=addr)
    {
      offset = addr - pageAddr;
      remaining = EFM32_MIN(writesize, eeprom_pagesize-offset);
      // save page
      halEepromRead(pageAddr,flashPageBuf,eeprom_pagesize);
      // erase page
      halEepromErase(pageAddr,eeprom_pagesize);
      // combine new + old data
      MEMCOPY (flashPageBuf + offset, data, remaining);
      // write old data prefix
      // halEepromWrite(pageAddr,flashPageBuf,offset);
      // write new + old data
      halEepromWrite(pageAddr,flashPageBuf,eeprom_pagesize);
      // adjust writesize
      writesize -= remaining;
      // adjust data
      data += remaining;
      // adjust pageAddr
      pageAddr+=eeprom_pagesize;
    }

    while (writesize>0)
    {
      // try writing again
      if (halEepromWrite(pageAddr, data, writesize)==EEPROM_SUCCESS)
        return; 

      // still failure erase another mage
      if (writesize>eeprom_pagesize)
      {
        // erase page
        while (halEepromBusy());
        halEepromErase(pageAddr,eeprom_pagesize);
        // write page
        while (halEepromBusy());
        if (halEepromWrite(pageAddr,data,eeprom_pagesize) != EEPROM_SUCCESS)
          {
            while(1);
          }
        // adjust write size
        writesize -= eeprom_pagesize;
        // adjust data
        data += eeprom_pagesize;
        // adjust pageAddr
        pageAddr+=eeprom_pagesize;
      }
      else
      {
        // save page(there might be trailing data)
        while (halEepromBusy());
        halEepromRead(pageAddr+writesize,flashPageBuf,eeprom_pagesize-writesize);
        // erase page
        while (halEepromBusy());
        halEepromErase(pageAddr,eeprom_pagesize);
        // write new data
        while (halEepromBusy());
        if (halEepromWrite(pageAddr,data,writesize)!=EEPROM_SUCCESS)
        {
          while(1);
        }
        // write old data
        while (halEepromBusy());
        if (halEepromWrite(pageAddr+writesize,flashPageBuf,eeprom_pagesize-writesize) != EEPROM_SUCCESS)
          {
            while(1);
          }
        // adjust write size
        writesize = 0;
      }
    }
  }
  #endif // MSD_MEDIA == MSD_BOOTLOAD_MEDIA
}

