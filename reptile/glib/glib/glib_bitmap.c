 /*************************************************************************//**
 * @file glib_bitmap.c
 * @brief Silicon Labs Graphics Library: Bitmap Drawing Routines
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


/* Standard C header files */
#include <stdint.h>

/* EM types */
#include "em_types.h"

/* GLIB header files */
#include "glib.h"

/**************************************************************************//**
*  @brief
*  Draws a bitmap
*
*  Sets up a bitmap that starts at x0,y0 and draws bitmap.
*  The picture is a 24-bit bitmap. It is organized as 8-bit
*  per color per pixel, and the color order is RGB. So picData is supposed to contain
*  at least (width * height * 3) entries. picData has be organized like this:
*  picData = { R, G, B, R, G, B, R, G, B ... }
*
*  @param pContext
*  Pointer to a GLIB_Context_t in which the bitmap is drawn.
*  @param x
*  Start x-coordinate for bitmap
*  @param y
*  Start y-coordinate for bitmap
*  @param width
*  Width of picture
*  @param height
*  Height of picture
*  @param pixData
*  Bitmap data 24-bit RGB
*
*  @return
*  Returns GLIB_OK on success, or else error code
******************************************************************************/

EMSTATUS GLIB_drawBitmap(GLIB_Context_t *pContext, int32_t x, int32_t y,
                         uint32_t width, uint32_t height, uint8_t *picData)
{
  EMSTATUS status;

  /* Set display clipping area for bitmap */
  status = DMD_setClippingArea(x, y, width, height);
  if (status != DMD_OK) return status;

  /* Write bitmap to display */
  status = DMD_writeData(0, 0, picData, width * height);
  if (status != DMD_OK) return status;

  /* Reset driver clipping area to GLIB clipping region */
  return DMD_setClippingArea(pContext->clippingRegion.xMin,
                               pContext->clippingRegion.yMin,
                               pContext->clippingRegion.xMin + pContext->clippingRegion.xMax + 1,
                               pContext->clippingRegion.yMin + pContext->clippingRegion.yMax + 1);
}
