/**************************************************************************//**
 * @file
 * @brief Graphics routines for reading a single BMP image from the filesystem
 *        and displaying it on the TFT.
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
#include <string.h>

#include "glib/glib.h"
#include "glib/glib_color.h"
#include "glib/bmp.h"
#include "dmd/ssd2119/dmd_ssd2119.h"

#include "slides.h"

#include "diskio.h"
#include "ff.h"
#include "microsd.h"

/** Graphics context */
GLIB_Context_t gc;

/* Palette */
uint8_t palette[PALETTE_SIZE];
uint8_t rgbBuffer[RGB_BUFFER_SIZE];

/* File to read bmp data from */
FIL BMPfile;

/* Local prototypes */
EMSTATUS SLIDES_readData(uint8_t buffer[], uint32_t bufLength, uint32_t bytesToRead);

/***************************************************************************//**
 * @brief
 *   This function draws an error message and if it is fatal, loops forever.
 *   This function assumes that TFT_init has been run prior to calling this
 *   function.
 * @param fatal
 *   If true, the display will show the error message and loop forever.
 * @param fmt
 *   Format string to display.
 ******************************************************************************/
void SLIDES_showError(bool fatal, const char* fmt, ...)
{
  va_list argp;
  char    buffer[100];

  va_start(argp, fmt);
  vsnprintf(buffer, 100, fmt, argp);
  va_end(argp);

  /* Clear screen */
  GLIB_clear(&gc);
  /* Draw error string */
  GLIB_drawString(&gc, buffer, sizeof(buffer), 10, 50, 1);

  /* If it is fatal, loop forever here. */
  if (fatal)
    while (1) ;
}

/***************************************************************************//**
 * @brief
 *   Callback used by the BMP decompression library for reading data from
 *   the filesystem. Uses the global variable BMPfile to read from.
 * @param[out] buffer
 *   The buffer to write data to.
 * @param[in] bufLength
 *   Size of the buffer.
 * @param[in] bytesToRead
 *   Number of bytes to read from file.
 * @return
 *   An EMSTATUS error code.
 ******************************************************************************/
EMSTATUS SLIDES_readData(uint8_t buffer[], uint32_t bufLength, uint32_t bytesToRead)
{
  UINT bytes_read;
  (void) bufLength;      /* Unused parameter */

  if (f_read(&BMPfile, buffer, bytesToRead, &bytes_read) != FR_OK)
    return BMP_ERROR_IO;
  return BMP_OK;
}

/**************************************************************************//**
 * @brief Clears/updates entire background ready to be drawn
 *****************************************************************************/
void SLIDES_showBMP(char *fileName)
{
  int32_t  xCursor;
  int32_t  yCursor;
  uint32_t pixelsRead;
  uint32_t nPixelsPerRow;
  uint32_t nRows;

  EMSTATUS status;

  /* Open file */
  if (f_open(&BMPfile, fileName, FA_READ) != FR_OK)
  {
    SLIDES_showError(true, "Fatal:\n  Failed to open file:\n  %s", fileName);
  }

  /* Initialize BMP decoder */
  if (BMP_init(palette, 1024, &SLIDES_readData) != BMP_OK)
  {
    SLIDES_showError(true, "Fatal:\n  Failed to init BMP library.");
  }

  /* Read headers */
  if ((status = BMP_reset()) != BMP_OK)
  {
    SLIDES_showError(false, "Info:\n  %s is not a BMP file", fileName);
    goto cleanup;
  }

  /* Get important BMP data */
  nPixelsPerRow = BMP_getWidth();
  nRows         = BMP_getHeight();
  yCursor       = nRows - 1;
  xCursor       = 0;

  /* Check size of BMP */
  if ((nPixelsPerRow > 320) || (nRows > 240))
  {
    SLIDES_showError(false, "Info:\n  %s is larger than 320x240.", fileName);
  }

  /* Set clipping region */
  DMD_setClippingArea(0, 0, nPixelsPerRow, nRows);

  /* Read in and draw row for row */
  while (yCursor >= 0)
  {
    /* Read in row buffer */
    status = BMP_readRgbData(rgbBuffer, RGB_BUFFER_SIZE, &pixelsRead);
    if (status != BMP_OK || pixelsRead == 0)
      break;

    /* Draw row buffer. Remember, BMP is stored bottom-up */
    status = DMD_writeData(xCursor, yCursor, rgbBuffer, pixelsRead);
    if (status != DMD_OK)
      break;

    /* Update cursor */
    xCursor += pixelsRead;
    if ((uint32_t) xCursor >= nPixelsPerRow)
    {
      yCursor -= xCursor / nPixelsPerRow;
      xCursor  = xCursor % nPixelsPerRow;
    }
  }
  /* Reset clipping area in DMD driver */
  status = GLIB_resetDisplayClippingArea(&gc);
  if (status != 0)
    return;

 cleanup:
  /* Close the file */
  f_close(&BMPfile);
}

/**************************************************************************//**
 * @brief Initialize viewer
 *****************************************************************************/
void SLIDES_init(void)
{
  EMSTATUS status;
  GLIB_Rectangle_t rect = {
    .xMin =   0,
    .yMin =   0,
    .xMax = 319,
    .yMax = 239,
  };

  /* Init graphics context - abort on failure */
  status = GLIB_contextInit(&gc);
  if (status != GLIB_OK) while (1) ;

  /* Clear framebuffer */
  gc.foregroundColor = GLIB_rgbColor(20, 40, 20);
  GLIB_drawRectFilled(&gc, &rect);

  /* Update drawing regions of picture  */
  gc.foregroundColor = GLIB_rgbColor(200, 200, 200);
}
