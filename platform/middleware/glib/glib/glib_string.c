 /*************************************************************************//**
 * @file glib_string.c
 * @brief Silicon Labs Graphics Library: String Drawing Routines
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
#include <string.h>
#include <stdbool.h>
/* EM types */
#include "em_types.h"

/* GLIB header files */
#include "glib.h"
#include "glib_color.h"

/**************************************************************************//**
*  @brief
*  Draws a char using the font supplied with the library.
*
*  @param pContext
*  Pointer to the GLIB_Context_t
*
*  @param myChar
*  Char to be drawn
*
*  @param x0
*  Start x-coordinate for the char (Upper left corner)
*
*  @param y0
*  Start y-coordinate for the char (Upper left corner)
*
*  @param opaque
*  Determines whether to show the background or color it with the background
*  color specified by the GLIB_Context_t. If opaque == true, the background color is used.
*
*  @return
*  Returns GLIB_OK on success, or else error code
******************************************************************************/

EMSTATUS GLIB_drawChar(GLIB_Context_t *pContext, char myChar, int32_t x, int32_t y,
                       bool opaque)
{
  EMSTATUS status;
  uint16_t fontIdx;
  uint8_t *pPixMap8;
  uint16_t *pPixMap16;
  uint32_t *pPixMap32;
  uint16_t row;
  uint16_t currentRow;
  uint16_t xOffset;
  uint32_t drawnElements = 0;

  /* Check arguments */
  if (pContext == NULL) return GLIB_ERROR_INVALID_ARGUMENT;

  /* Check input char */
  if ((myChar < ' ') || (myChar > '~')) return GLIB_ERROR_INVALID_CHAR;

  /* Sets the index in the font array */
  if (pContext->font.class == NumbersOnlyFont) {
    fontIdx = (myChar - '0');
    if (myChar == ':') {
      fontIdx = 10;
    }
    if (myChar == ' ') {
      fontIdx = 11;
    }
  } else { /* FullFont class */
    fontIdx = myChar - ' ';
  }

  if (fontIdx > (pContext->font.cntOfMapElements - 1)) {
    return GLIB_ERROR_INVALID_CHAR;
  }

  /* Loop through the rows and draw the font */
  pPixMap8 = (uint8_t *)pContext->font.pFontPixMap;
  pPixMap16 = (uint16_t *)pContext->font.pFontPixMap;
  pPixMap32 = (uint32_t *)pContext->font.pFontPixMap;

  for (row = 0; row < pContext->font.fontHeight; row++) {

    switch (pContext->font.sizeOfMapElement) {
      case 1:
        currentRow = pPixMap8[fontIdx];
        break;

      case 2:
        currentRow = pPixMap16[fontIdx];
        break;

      default:
        currentRow = pPixMap32[fontIdx];
    }

    for (xOffset = 0; xOffset < pContext->font.fontWidth; ++xOffset) {
      /* Bit 1 means draw, Bit 0 means do not draw */
      if (currentRow & 0x1) {
        status = GLIB_drawPixel(pContext, x + xOffset, y + row);
        if (status > GLIB_ERROR_NOTHING_TO_DRAW) return status;
        if (status == GLIB_OK) drawnElements++;
      }
      else if (opaque) {
        /* Draw background pixel */
        status = GLIB_drawPixelColor(pContext, x + xOffset, y + row, pContext->backgroundColor);
        if (status > GLIB_ERROR_NOTHING_TO_DRAW) return status;
        if (status == GLIB_OK) drawnElements++;
      }
      currentRow >>= 1;
    }

    /* Handle character spacing */
    for (; xOffset < pContext->font.fontWidth + pContext->font.charSpacing; ++xOffset) {
      if (opaque) {
        /* Draw background pixel */
        status = GLIB_drawPixelColor(pContext, x + xOffset, y + row, pContext->backgroundColor);
        if (status > GLIB_ERROR_NOTHING_TO_DRAW) return status;
        if (status == GLIB_OK) drawnElements++;
      }
    }

    /* fontIdx offset for a new row */
    fontIdx += pContext->font.fontRowOffset;
  }
  return ((drawnElements == 0) ? GLIB_ERROR_NOTHING_TO_DRAW : GLIB_OK);
}

/**************************************************************************//**
*  @brief
*  Draws a string using the font supplied with the library.
*
*  @param pContext
*  Pointer to a GLIB_Context_t
*
*  @param pString
*  Pointer to the string that is drawn
*
*  @param x0
*  Start x-coordinate for the string (Upper left corner)
*
*  @param y0
*  Start y-coordinate for the string (Upper left corner)
*
*  @param opaque
*  Determines whether to show the background or color it with the background
*  color specified by the GLIB_Context_t. If opaque == 1, the background color is used.
*
*  @return
*  Returns GLIB_OK on success, or else error code
******************************************************************************/

EMSTATUS GLIB_drawString(GLIB_Context_t *pContext, char* pString, uint32_t sLength,
                         int32_t x0, int32_t y0, bool opaque)
{
  EMSTATUS status;
  uint32_t drawnElements = 0;
  uint32_t stringIndex;
  int32_t x, y;

  /* Check arguments */
  if (pContext == NULL || pString == NULL) {
    return GLIB_ERROR_INVALID_ARGUMENT;
  }

  if (pContext->font.class == InvalidFont) {
    return GLIB_ERROR_INVALID_CHAR;
  }

  x = x0;
  y = y0;

  /* Loops through the string and prints char for char */
  for (stringIndex = 0; stringIndex < sLength; stringIndex++)
  {
    /* Newline char */
    if (pString[stringIndex] == '\n') {
      x = x0;
      y = y + pContext->font.fontHeight + pContext->font.lineSpacing;
      continue;
    }

    /* Draw the current char */
    status = GLIB_drawChar(pContext, pString[stringIndex], x, y, opaque);
    if (status > GLIB_ERROR_NOTHING_TO_DRAW) return status;
    if (status == GLIB_OK) drawnElements++;

    /* Adjust x and y coordinate */
    x += (pContext->font.fontWidth + pContext->font.charSpacing);
  }
  return ((drawnElements == 0) ? GLIB_ERROR_NOTHING_TO_DRAW : GLIB_OK);
}


/**************************************************************************//**
*  @brief
*  Set new font for the library. Note that GLIB defines a default font in glib.c.
*  Redefine GLIB_DEFAULT_FONT to change the default font.
*
*  @param pContext
*  Pointer to the GLIB_Context_t
*
*  @param pFont
*  Pointer to the new font
*
*  @return
*  Returns GLIB_OK on success, or else error code
******************************************************************************/

EMSTATUS GLIB_setFont(GLIB_Context_t *pContext, GLIB_Font_t *pFont)
{
  /* Check arguments */
  if (pContext == NULL) {
    return GLIB_ERROR_INVALID_ARGUMENT;
  }

  if (pFont == NULL)
  {
    memset(&pContext->font, 0, sizeof(GLIB_Font_t));
    return GLIB_ERROR_INVALID_ARGUMENT;
  }
  else
  {
    memcpy(&pContext->font, pFont, sizeof(GLIB_Font_t));
    return GLIB_OK;
  }
}
