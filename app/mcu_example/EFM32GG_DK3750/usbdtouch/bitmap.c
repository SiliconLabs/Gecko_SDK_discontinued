/**************************************************************************//**
 * @file picture.c
 * @brief Storing TFT frame buffer into BMP file.
 * @version 5.1.2
 ******************************************************************************
 * @section License
 * <b>Copyright 2015 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include <stdio.h>
#include "em_device.h"
#include "em_assert.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_ebi.h"
#include "em_gpio.h"
#include "bsp.h"
#include "bsp_trace.h"
#include "retargetserial.h"
#include "ff.h"
#include "diskio.h"

#include "em_usb.h"
#include "msdd.h"
#include "msddmedia.h"
#include "descriptors.h"

/**************************************************************************//**
 *
 * This example shows how a TFT content could be transmitted to PC.
 *
 * There is implemented functionality which converts TFT frame buffer into BMP
 * formatted file. The file is then stored on earlier created virtual drive.
 * Created files could be read after connecting DK to PC by means of USB.
 * This project basically uses usbdmsd example and implements FAT file system on
 * created virtual drive. The drive occupies last 3MB of PSRAM, first MB could
 * still be used by TFT driver as frame buffer.
 *
 *****************************************************************************/
#define BITMAP_HEADER_SIZE      66
#define BITMAP_WIDTH            320
#define BITMAP_HEIGHT           240
#define BITMAP_PICTURE_POINTS   (BITMAP_HEIGHT*BITMAP_WIDTH)
#define BITMAP_SIZE             (2*BITMAP_PICTURE_POINTS+BITMAP_HEADER_SIZE)
#define BITMAP_LINE_SIZE        (2*BITMAP_WIDTH)

#if defined( BUSPOWERED )
#if ( ( MSD_MEDIA==MSD_PSRAM_MEDIA ) || ( MSD_MEDIA==MSD_SDCARD_MEDIA ) )
#error "Illegal combination of BUSPOWERED and MSD_MEDIA type."
#endif
#endif

FATFS fatfs;

static const USBD_Callbacks_TypeDef callbacks =
{
  .usbReset        = NULL,
  .usbStateChange  = MSDD_StateChangeEvent,
  .setupCmd        = MSDD_SetupCmd,
  .isSelfPowered   = NULL,
  .sofInt          = NULL
};

static const USBD_Init_TypeDef usbInitStruct =
{
  .deviceDescriptor    = &USBDESC_deviceDesc,
  .configDescriptor    = USBDESC_configDesc,
  .stringDescriptors   = USBDESC_strings,
  .numberOfStrings     = sizeof(USBDESC_strings)/sizeof(void*),
  .callbacks           = &callbacks,
  .bufferingMultiplier = USBDESC_bufferingMultiplier,
  .reserved            = 0
};

/**************************************************************************//**
 * @brief
 *   Create new file and writes data in it.
 *
 * @param[in] filename
 *   name of file to which data should be written
 * @param[in] pData
 *   pointer to data to be written
 * @param[in] len
 *   number of bytes to be written
 *
 * @return
 *   Number of bytes written, on sucessful operation is equal to number of bytes
 * to be written.
 *
 * @details
 *   Function writes data pointed by argument to newly created file. During entire
 * operation USB is dicsonnected. At return, filesystem is unmounted and
 * USB is reconnected.
 *****************************************************************************/
uint32_t BITMAP_CreateFileAndSaveData(const char *filename, const char *pData, uint32_t len)
{ UINT written = 0;
  FIL file;

  USBD_Disconnect();
  f_mount(0, &fatfs);

  if(f_open(&file, filename, FA_CREATE_NEW | FA_WRITE) == FR_OK)
   {
     f_write(&file, pData, len, &written);
     f_close(&file);
   };

  f_mount(0, NULL);
  USBD_Connect();

  /* Call MSDD_Handler in case user did not call BITMAP_USBHandler */
  MSDD_Handler();

  return((uint32_t)written);
}

/**************************************************************************//**
 * @brief
 *   bitmap creation module init function
 *****************************************************************************/
int BITMAP_Init( void )
{

  CMU_ClockSelectSet( cmuClock_HF, cmuSelect_HFXO );
  CMU_OscillatorEnable(cmuOsc_LFXO, true, false);

#if !defined(BUSPOWERED)
  //RETARGET_SerialInit();        /* Initialize DK UART port */
  //RETARGET_SerialCrLf( 1 );     /* Map LF to CRLF          */
  //printf( "\nEFM32 Mass Storage Device example\n" );
#endif

  /* Initialize the Mass Storage Media. */
  if ( !MSDDMEDIA_Init() )
  {
#if !defined(BUSPOWERED)
    printf( "\nMedia error !\n" );
#endif
    EFM_ASSERT( false );
    for( ;; ){}
  }

  f_mount(0, &fatfs);
#if ( MSD_MEDIA == MSD_SRAM_MEDIA ) || ( MSD_MEDIA == MSD_PSRAM_MEDIA )
  f_mkfs(0, 1, 0);
#endif
  // umount filesystem
  f_mount(0, NULL);

  /* Initialize the Mass Storage Device. */
  MSDD_Init(gpioPortE, 1);

  /* Initialize and start USB device stack. */
  USBD_Init(&usbInitStruct);

  MSDD_Handler();

  return(0);
}

/**************************************************************************//**
 * @brief
 *   dummy function returning time stamp. Currently returns always 0.
 *****************************************************************************/
DWORD get_fattime(void)
{
  return 0;
}

/**************************************************************************//**
 * @brief
 *   dummy i/o function. Does nothing.
 *****************************************************************************/
DSTATUS disk_initialize(BYTE drive)
{ (void)drive;

 return 0;
}

/**************************************************************************//**
 * @brief
 *   o/i file control function used by FAT subsystem.
 *****************************************************************************/
DRESULT disk_ioctl (BYTE drive, BYTE Command, void* buffer)
{ (void)drive;
  (void)buffer;

  switch(Command)
  { case CTRL_SYNC: MSDDMEDIA_Flush();
                    break;
    case GET_SECTOR_SIZE: *(DWORD *)buffer = 512;
                          break;
    case GET_SECTOR_COUNT: *(DWORD *)buffer = MSDDMEDIA_GetSectorCount();
                           break;
    case GET_BLOCK_SIZE:  *(DWORD *)buffer = 1;
                          break;
    default: return RES_PARERR;
  };
  return RES_OK;
};

/**************************************************************************//**
 * @brief
 *   i/o write function used by FAT subsystem.
 *****************************************************************************/
DRESULT disk_write(BYTE drive, const BYTE *buffer, DWORD sector_number, BYTE sectors)
{ MSDD_CmdStatus_TypeDef Cmd;
  (void)drive;

  if(MSDDMEDIA_CheckAccess(&Cmd, sector_number, sectors))
  {
    MSDDMEDIA_Write(&Cmd, (uint8_t *)buffer, sectors);
  } else return RES_PARERR;

  return RES_OK;
};

/**************************************************************************//**
 * @brief
 *   i/o read function used by FAT subsystem
 *****************************************************************************/
DRESULT disk_read(BYTE drive, BYTE *buffer, DWORD sector_number, BYTE sectors)
{ MSDD_CmdStatus_TypeDef Cmd;
  (void)drive;

  if(MSDDMEDIA_CheckAccess(&Cmd, sector_number, sectors))
  {
    MSDDMEDIA_Read(&Cmd, buffer, sectors);
  } else return RES_PARERR;

  return RES_OK;
};

/**************************************************************************//**
 * @brief
 *   i/o dummy function - always returns 0
 *****************************************************************************/
DSTATUS disk_status(BYTE drive)
{
  (void)drive;
  return(0);
}

/**************************************************************************//**
 * @brief
 *   Write frame buffer to file
 *
 * @param[in] pFile
 *   pointer file handler
 * @param[in] pSource
 *   pointer to framebuffer to be stored
 *
 * @details
 *   Function writes frame buffer pointed by argument to file taking care of
 *   swapping lines and adding BMP header.
 *****************************************************************************/
void BITMAP_Create(FIL *pFile, uint8_t *pSource)
{ int line;
  UINT written;
  const uint8_t bitmapHeader[BITMAP_HEADER_SIZE] =
  { 'B',  'M',  /* file size=153600+header */ BITMAP_HEADER_SIZE, 0x58, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00,
    BITMAP_HEADER_SIZE, 0x00, 0x00, 0x00, /* where picture data starts */
    40, 0, 0, 0, 0x40, 1, 0x00, 0x00, /* header size, bitmap width */
    /* bitmap height */ 240, 0x00, 0x00, 0x00, /* number of planes */0x01, 0x00,
    /* bits per pixel */ 16, 0x00,/* compression method */ 0x03, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, /* horizontal resolution */ 0x13, 0x0B, 0x00, 0x00, /* vertical resolution */ 0x13, 0x0B, 0x00, 0x00,
    /*colours used */ 0x00, 0x00, 0x00, 0x00, /*important colours*/ 0x00, 0x00, 0x00, 0x00,
    0x00, 0xF8, 0x00, 0x00, /* R channel bitmask */
    0xE0, 0x07, 0x00, 0x00, /* G channel bitmask */
    0x1F, 0x00, 0x00, 0x00, /* B channel bitmask */
  };

  f_write(pFile, bitmapHeader, sizeof(bitmapHeader), &written);
  pSource += (BITMAP_HEIGHT-1) * BITMAP_LINE_SIZE; /* move to bottom line of picture */
  for(line=0;line<BITMAP_HEIGHT;line++)
  {
    f_write(pFile, pSource, (UINT)BITMAP_LINE_SIZE, &written);
    pSource -= BITMAP_LINE_SIZE;
  }
}

/**************************************************************************//**
 * @brief
 *   Returns current frame buffer address.
 *****************************************************************************/
unsigned char *BITMAP_ReturnScreenAddress(void)
{
  return (unsigned char *)(EBI_BankAddress(EBI_BANK2) + EBI->TFTFRAMEBASE);
};

int pictureCount = 0;
/**************************************************************************//**
 * @brief
 *   Create bitmap file on file system with data taken from TFT frame buffer
 *
 * @return
 *   0 - success,
 *   1 - error.
 *
 * @details
 *   Function takes current frame buffer, disconnect USB when needed, mounts
 * filesystem, create new file and write to this file. After that unmounts FS
 * and reconnect USB again.
 *****************************************************************************/
int BITMAP_TakePicture(void)
{ char fileName[13] = "SCREENxx.BMP";
  FIL file;
  USBD_State_TypeDef usbState;

  /* Call MSDD_Handler in case user did not call BITMAP_USBHandler */
  MSDD_Handler();

  usbState = USBD_GetUsbState();
  // disconnect USB and mount filesystem
  USBD_Disconnect();
  f_mount(0, &fatfs);

  do{
      if( (pictureCount & 0x0F)< 10) fileName[7] = '0' + (pictureCount & 0x0F);
      else fileName[7] = 'a' - 10 + (pictureCount & 0x0F);
      if( ( (pictureCount>>4) & 0x0F)< 10) fileName[6] = '0' + ( (pictureCount>>4) & 0x0F);
      else fileName[6] = 'a' - 10 + ( (pictureCount>>4) & 0x0F);
      pictureCount++;
      if(f_open(&file, fileName, FA_CREATE_NEW | FA_WRITE) == FR_OK)
      {
        BITMAP_Create(&file, BITMAP_ReturnScreenAddress());
        f_close(&file);

        f_mount(0, NULL);
        if(usbState!=USBD_STATE_NONE)
          USBTIMER_DelayMs( 1500 );
        USBD_Connect();

        return(0);
      }
    }while(pictureCount<255);

  f_mount(0, NULL);
  USBD_Connect();

  return(1);
}

/**************************************************************************//**
 * @brief
 *   Simply calls MSDD_Handler. Should be called periodically from main loop.
 *****************************************************************************/
bool BITMAP_USBHandler(void)
{
  return MSDD_Handler();
}
