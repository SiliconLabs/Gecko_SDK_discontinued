 /*************************************************************************//**
 * @file glib.h
 * @brief Silicon Labs Graphics Library
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


#ifndef __GLIB_H
#define __GLIB_H

/* C-header files */
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/* GLIB header files */
#include "glib_color.h"

/* Display Driver header files */
#include "dmd/dmd.h"

#include "em_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ECODE_GLIB_BASE                         0x00000000

/* Error codes */
/** Successful call */
#define GLIB_OK                                 0x00000000
/** Function did not draw */
#define GLIB_ERROR_NOTHING_TO_DRAW              (ECODE_GLIB_BASE | 0x0001)
/** Invalid char */
#define GLIB_ERROR_INVALID_CHAR                 (ECODE_GLIB_BASE | 0x0002)
/** Coordinates out of bounds */
#define GLIB_OUT_OF_BOUNDS                      (ECODE_GLIB_BASE | 0x0003)
/** Invalid coordinates (ex. xMin > xMax) */
#define GLIB_ERROR_INVALID_CLIPPINGREGION       (ECODE_GLIB_BASE | 0x0004)
/** Invalid argument */
#define GLIB_ERROR_INVALID_ARGUMENT             (ECODE_GLIB_BASE | 0x0005)
/** Out of memory */
#define GLIB_ERROR_OUT_OF_MEMORY                (ECODE_GLIB_BASE | 0x0006)
/** File not supported */
#define GLIB_ERROR_FILE_NOT_SUPPORTED           (ECODE_GLIB_BASE | 0x0007)
/** General IO Error */
#define GLIB_ERROR_IO                           (ECODE_GLIB_BASE | 0x0008)
/** Invalid file */
#define GLIB_ERROR_INVALID_FILE                 (ECODE_GLIB_BASE | 0x0009)


/** @struct __GLIB_Font_t_Class
 *  @brief Font classes
 */
typedef enum __GLIB_Font_Class
{
  InvalidFont = 0,
  FullFont,
  NumbersOnlyFont,
} GLIB_Font_Class;


/** @struct __GLIB_Font_t
 *  @brief Font definition structure
 */
typedef struct __GLIB_Font_t
{
  void *pFontPixMap;
  uint16_t cntOfMapElements;
  uint8_t sizeOfMapElement;
  uint8_t fontRowOffset;
  uint8_t fontWidth;
  uint8_t fontHeight;
  uint8_t lineSpacing;
  uint8_t charSpacing;
  GLIB_Font_Class class;
} GLIB_Font_t;


/** @struct __GLIB_Rectangle_t
 *  @brief Rectangle structure
 */
typedef struct __GLIB_Rectangle_t
{
  /** Minimum x-coordinate */
  int32_t xMin;
  /** Minimum y-coordinate */
  int32_t yMin;
  /** Maximum x-coordinate */
  int32_t xMax;
  /** Maximum y-coordinate */
  int32_t yMax;
} GLIB_Rectangle_t;


/** @struct __GLIB_Context_t
 *  @brief GLIB Drawing Context
 *  (Multiple instances of GLIB_Context_t can exist)
 */
typedef struct __GLIB_Context_t
{
  /** Pointer to the dimensions of the display */
  const DMD_DisplayGeometry *pDisplayGeometry;

  /** Background color */
  uint32_t backgroundColor;

  /** Foreground color */
  uint32_t foregroundColor;

  /** Clipping rectangle */
  GLIB_Rectangle_t clippingRegion;

  /** Font definition */
  GLIB_Font_t font;

} GLIB_Context_t;


/* Prototypes for graphics library functions */
EMSTATUS GLIB_contextInit(GLIB_Context_t *pContext);

EMSTATUS GLIB_displayWakeUp(void);

EMSTATUS GLIB_displaySleep(void);

EMSTATUS GLIB_clear(GLIB_Context_t *pContext);

EMSTATUS GLIB_resetDisplayClippingArea(GLIB_Context_t *pContext);

EMSTATUS GLIB_resetClippingRegion(GLIB_Context_t *pContext);

void GLIB_colorTranslate24bpp(uint32_t color, uint8_t *red, uint8_t *green, uint8_t *blue);

uint32_t GLIB_rgbColor(uint8_t red, uint8_t green, uint8_t blue);

bool GLIB_rectContainsPoint(const GLIB_Rectangle_t *pRect, int32_t xCenter, int32_t yCenter);

void GLIB_normalizeRect(GLIB_Rectangle_t *pRect);

EMSTATUS GLIB_setClippingRegion(GLIB_Context_t *pContext, GLIB_Rectangle_t *pRect);

EMSTATUS GLIB_drawCircle(GLIB_Context_t *pContext, int32_t x, int32_t y,
                         uint32_t radius);

EMSTATUS GLIB_drawCircleFilled(GLIB_Context_t *pContext, int32_t x, int32_t y,
                               uint32_t radius);

EMSTATUS GLIB_drawPartialCircle(GLIB_Context_t *pContext, int32_t xCenter,
                                int32_t yCenter, uint32_t radius, uint8_t bitMask);

EMSTATUS GLIB_setFont(GLIB_Context_t *pContext, GLIB_Font_t *pFont);

EMSTATUS GLIB_drawString(GLIB_Context_t *pContext, char* pString, uint32_t sLength,
                         int32_t x0, int32_t y0, bool opaque);

EMSTATUS GLIB_drawChar(GLIB_Context_t *pContext, char myChar, int32_t x,
                       int32_t y, bool opaque);

EMSTATUS GLIB_drawBitmap(GLIB_Context_t* pContext, int32_t x, int32_t y,
                         uint32_t width, uint32_t height, uint8_t *picData);

EMSTATUS GLIB_drawLine(GLIB_Context_t *pContext, int32_t x1, int32_t y1,
                       int32_t x2, int32_t y2);

EMSTATUS GLIB_drawLineH(GLIB_Context_t *pContext, int32_t x1, int32_t y1,
                        int32_t x2);

EMSTATUS GLIB_drawLineV(GLIB_Context_t *pContext, int32_t x1, int32_t y1,
                        int32_t y2);

EMSTATUS GLIB_drawRect(GLIB_Context_t *pContext, GLIB_Rectangle_t *pRect);

EMSTATUS GLIB_drawRectFilled(GLIB_Context_t *pContext,
                             GLIB_Rectangle_t *pRect);

EMSTATUS GLIB_drawPolygon(GLIB_Context_t *pContext,
                          uint32_t numPoints, int32_t *polyPoints);

EMSTATUS GLIB_drawPixelRGB(GLIB_Context_t *pContext, int32_t x, int32_t y,
                           uint8_t red, uint8_t green, uint8_t blue);

EMSTATUS GLIB_drawPixel(GLIB_Context_t *pContext, int32_t x, int32_t y);

EMSTATUS GLIB_drawPixelColor(GLIB_Context_t *pContext, int32_t x, int32_t y,
                             uint32_t color);

/* Fonts included in the library */
extern const GLIB_Font_t GLIB_FontNormal8x8; /* Default */
extern const GLIB_Font_t GLIB_FontNarrow6x8;
extern const GLIB_Font_t GLIB_FontNumber16x20;

#ifdef __cplusplus
}
#endif

#endif
