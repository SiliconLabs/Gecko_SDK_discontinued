 /*************************************************************************//**
 * @file bmp.h
 * @brief BMP Module
 ******************************************************************************
 * @section License
 * <b>Copyright 2015 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/


#ifndef __BMP_H_
#define __BMP_H_

/* C Standard header files */
#include <stdlib.h>
#include <stdint.h>

/* EM types */
#include "em_types.h"

#include "bmp_conf.h"

#define ECODE_BMP_BASE    0x00000000

/* Error codes */
/** Successful call */
#define BMP_OK    0x00000000
/** End of file has been reached */
#define BMP_END_OF_FILE
/** General IO error */
#define BMP_ERROR_IO                        (ECODE_BMP_BASE | 0x0001)
/** BMP_Header size in bytes is different from BMP_HEADER_SIZE */
#define BMP_ERROR_HEADER_SIZE_MISMATCH      (ECODE_BMP_BASE | 0x0002)
/** Endian mismatch */
#define BMP_ERROR_ENDIAN_MISMATCH           (ECODE_BMP_BASE | 0x0003)
/** BMP file is not supported */
#define BMP_ERROR_FILE_NOT_SUPPORTED        (ECODE_BMP_BASE | 0x0004)
/** BMP "file" is not a BMP file */
#define BMP_ERROR_FILE_INVALID              (ECODE_BMP_BASE | 0x0005)
/** Argument passed to function is invalid */
#define BMP_ERROR_INVALID_ARGUMENT          (ECODE_BMP_BASE | 0x0006)
/** BMP module is not initialized. Call BMP_init() */
#define BMP_ERROR_MODULE_NOT_INITIALIZED    (ECODE_BMP_BASE | 0x0007)
/** Invalid palette size */
#define BMP_ERROR_INVALID_PALETTE_SIZE      (ECODE_BMP_BASE | 0x0008)
/** File not reset. Call BMP_reset() */
#define BMP_ERROR_FILE_NOT_RESET            (ECODE_BMP_BASE | 0x0009)
/** End of bmp file is reached */
#define BMP_ERROR_END_OF_FILE               (ECODE_BMP_BASE | 0x0010)
/** Buffer provided is too small */
#define BMP_ERROR_BUFFER_TOO_SMALL          (ECODE_BMP_BASE | 0x0020)
/** Bmp palette is not read */
#define BMP_ERROR_PALETTE_NOT_READ          (ECODE_BMP_BASE | 0x0030)

/* Palette size in bytes */
#define BMP_PALETTE_8BIT_SIZE               (256 * 4)
#define BMP_HEADER_SIZE                     (54)
#define BMP_LOCAL_CACHE_LIMIT               (3)

#define RLE8_COMPRESSION                    (1)
#define NO_COMPRESSION                      (0)

#define BMP_LOCAL_CACHE_SIZE                (BMP_CONFIG_LOCAL_CACHE_SIZE)

/** @struct __BMP_Header
 *  @brief BMP Module header structure. Must be packed to exact 54 bytes.
 */

#if defined ( __GNUC__ )
struct __BMP_Header
#else
__packed struct __BMP_Header
#endif
{
  /** Magic identifier: "BM" (for bmp files)*/
  uint16_t magic;
  /** Size of the bmp file in bytes */
  uint32_t fileSize;
  /** Reserved 1 */
  uint16_t reserved1;
  /** Reserved 2 */
  uint16_t reserved2;
  /** Data offset relative to the start of the bmp data */
  uint32_t dataOffset;
  /** Header size in bytes */
  uint32_t headerSize;
  /** Width of bmp */
  uint32_t width;
  /** Height of bmp */
  uint32_t height;
  /** Color planes */
  uint16_t colorPlanes;
  /** Color depth */
  uint16_t bitsPerPixel;
  /** Compression type */
  uint32_t compressionType;
  /** Size of raw BMP data */
  uint32_t imageDataSize;
  /** Horizontal resolution (pixels per meter) */
  uint32_t hPixelsPerMeter;
  /** Vertical resolution (pixels per meter) */
  uint32_t vPixelsPerMeter;
  /** Number of color indices in the color table that are actually used by the bitmap */
  uint32_t colorsUsed;
  /** Number of color indices that are required for displaying the bitmap */
  uint32_t colorsRequired;
#if defined ( __GNUC__ )
} __attribute__ ((__packed__));
#else
};
#endif

typedef struct __BMP_Header   BMP_Header;

/** @struct __BMP_Palette
 *  @brief BMP palette structure to hold palette pointer and size
 */
typedef struct __BMP_Palette
{
  /** Palette data pointer */
  uint8_t  *data;
  /** Size of palette data */
  uint32_t size;
} BMP_Palette;

/** @struct __BMP_DataType
 *  @brief BMP Data type structure to hold information about the bmp data returned
 */
typedef struct __BMP_DataType
{
  /** Color depth of the data returned from function. */
  uint16_t bitsPerPixel;
  /** Compression type */
  uint32_t compressionType;
  /** Byte length of returned data */
  uint32_t size;
  /** Marks whether this data is at the end of the current row. endOfRow == 1, if end of row is reached. endOfRow == 0, if there is still unread data left in the row */
  uint32_t endOfRow;
} BMP_DataType;


/* Module prototypes */
EMSTATUS BMP_init(uint8_t *palette, uint32_t paletteSize, EMSTATUS (*fp)(uint8_t buffer[], uint32_t bufLength, uint32_t bytesToRead));
EMSTATUS BMP_reset(void);
EMSTATUS BMP_readRgbData(uint8_t buffer[], uint32_t bufLength, uint32_t *pixelsRead);
EMSTATUS BMP_readRawData(BMP_DataType *dataType, uint8_t buffer[], uint32_t bufLength);

/* Accessor functions */
int32_t BMP_getWidth(void);
int32_t BMP_getHeight(void);
int16_t BMP_getBitsPerPixel(void);
int32_t BMP_getCompressionType(void);
int32_t BMP_getImageDataSize(void);
int32_t BMP_getDataOffset(void);
int32_t BMP_getFileSize(void);

#endif /* __BMP_H_ */
