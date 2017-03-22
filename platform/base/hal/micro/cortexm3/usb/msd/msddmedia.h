/***************************************************************************//**
 * @file  msddmedia.h
 * @brief Media interface for Mass Storage class Device (MSD).
 * @author Nathaniel Ting
 * @version 3.20.1
 *******************************************************************************
 * @section License
 * Copyright 2013 by Silicon Labs. All rights reserved.                     *80*
 *****************************************************************************/
#ifndef __MSDDMEDIA_H
#define __MSDDMEDIA_H

#define MSD_SRAM_MEDIA          0   /* 48K "disk" in internal SRAM     */
#define MSD_BOOTLOAD_MEDIA      5   /* point to bootloader media        */

//not implemented. do not use!
#define MSD_PSRAM_MEDIA         1   /* 4M "disk" in external PSRAM     */
#define MSD_SDCARD_MEDIA        2   /* External micro SD-Card "disk"   */
#define MSD_FLASH_MEDIA         3   /* SECTOR_SIZE K "disk" in internal FLASH   */
#define MSD_NORFLASH_MEDIA      4   /* 16M "disk" in external NORFLASH */


#if !defined( MSD_MEDIA )
 #define MSD_MEDIA  MSD_BOOTLOAD_MEDIA  /* Select media type */
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*** MSD Media Function prototypes ***/

bool     MSDDMEDIA_CheckAccess( MSDD_CmdStatus_TypeDef *pCmd, uint32_t lba, uint32_t sectors );
uint32_t MSDDMEDIA_GetSectorCount( void );
bool     MSDDMEDIA_Init( void );
void     MSDDMEDIA_Read(  MSDD_CmdStatus_TypeDef *pCmd, uint8_t *data, uint32_t sectors );
void     MSDDMEDIA_Write( MSDD_CmdStatus_TypeDef *pCmd, uint8_t *data, uint32_t sectors );

#ifdef __cplusplus
}
#endif

#endif /* __MSDDMEDIA_H */
