/**************************************************************************//**
 * @file
 * @brief TFT address mapped example application
 * @version 4.2.0
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2014 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/


#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "em_device.h"
#include "bsp.h"

#include "glib/glib.h"
#include "glib/glib_color.h"
#include "dmd/ssd2119/dmd_ssd2119.h"

#include "tftdraw.h"

/** Graphics context */
GLIB_Context_t gc;

/**************************************************************************//**
 * @brief Really simple and silly random number generator
 * @param limit Upper limit of return value
 *****************************************************************************/
static int randomGenerator(int limit)
{
  static uint32_t rnum = 0xabcd7381;

  rnum = ((rnum * 27) << 8) | (((rnum / 13) & 0xa5a7f196) + (rnum >> 13));

  return(rnum % limit);
}


/**************************************************************************//**
 * @brief Clears/updates entire background ready to be drawn
 *****************************************************************************/
void TFT_displayUpdate(void)
{
  int            i, tmp;
  GLIB_Rectangle_t rect = {
    .xMin =   0,
    .yMin =   0,
    .xMax = 319,
    .yMax = 239,
  };
  /* Set clipping region to entire image */
  GLIB_setClippingRegion(&gc, &rect);
  GLIB_resetDisplayClippingArea(&gc);

  /* Generate "wild" rectangle pattern  */
  for (i = 0; i < 20; i++)
  {
    rect.xMin = randomGenerator(320);
    rect.xMax = randomGenerator(320);
    rect.yMin = randomGenerator(230) + 10;
    rect.yMax = randomGenerator(230) + 10;
    if (rect.xMin > rect.xMax)
    {
      tmp       = rect.xMin;
      rect.xMin = rect.xMax;
      rect.xMax = tmp;
    }
    if (rect.yMin > rect.yMax)
    {
      tmp       = rect.yMin;
      rect.yMin = rect.yMax;
      rect.yMax = tmp;
    }
    gc.foregroundColor = GLIB_rgbColor(128 + randomGenerator(127),
                                       randomGenerator(200),
                                       randomGenerator(255));
    GLIB_drawRectFilled(&gc, &rect);
  }
}

/**************************************************************************//**
 * @brief Initialize viewer
 *****************************************************************************/
void TFT_init(void)
{
  static char    *efm32_hello = "EFM32 @ 32MHz / SSD2119 TFT demo\n";
  EMSTATUS       status;
  GLIB_Rectangle_t rect = {
    .xMin =   0,
    .yMin =   0,
    .xMax = 319,
    .yMax = 239,
  };

  /* Initialize graphics - abort on failure */
  status = DMDIF_init(BC_SSD2119_BASE, BC_SSD2119_BASE + 2);
  if (status == DMD_OK) status = DMD_init(0);
  if ((status != DMD_OK) && (status != DMD_ERROR_DRIVER_ALREADY_INITIALIZED)) while (1) ;

  /* Make sure display is configured with correct rotation */
  if ((status == DMD_OK)) DMD_flipDisplay(1,1);

  /* Init graphics context - abort on failure */
  status = GLIB_contextInit(&gc);
  if (status != GLIB_OK) while (1) ;

  /* Clear framebuffer */
  gc.foregroundColor = GLIB_rgbColor(20, 40, 20);
  GLIB_drawRectFilled(&gc, &rect);

  /* Update drawing regions of picture  */
  gc.foregroundColor = GLIB_rgbColor(200, 200, 200);
  GLIB_drawString(&gc, efm32_hello, strlen(efm32_hello), 0, 0, 1);

  TFT_displayUpdate();
}

