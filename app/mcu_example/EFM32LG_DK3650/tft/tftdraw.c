/**************************************************************************//**
 * @file
 * @brief EFM32LG_DK3650 TFT address mapped example, drawing random rectangles
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
#include <stdint.h>
#include "em_device.h"
#include "bsp.h"

#include "glib/glib.h"
#include "glib/glib_color.h"
#include "dmd/ssd2119/dmd_ssd2119.h"

#include "tftdraw.h"

extern void Delay(uint32_t dlyTicks);

static uint32_t rnum = 0xabcd7381;

/**************************************************************************//**
 * @brief Really simple and silly random number generator
 * @param limit Upper limit of return value
 *****************************************************************************/
static int randomGenerator(int limit)
{
  rnum = ((rnum * 27) << 3) | (((rnum / 13) & 0xa5a7f196) + (rnum >> 13));

  return(rnum % limit);
}


/**************************************************************************//**
 * @brief Clears/updates entire background ready to be drawn
 *****************************************************************************/
void TFT_displayUpdate(GLIB_Context_t *gcPtr)
{
  int            i, tmp;
  GLIB_Rectangle_t rect = {
    .xMin =   0,
    .yMin =   0,
    .xMax = 319,
    .yMax = 239,
  };
  static char    *efm32_hello = "48MHz / SSD2119 address mapped TFT\n";

  /* Set clipping region to entire image */
  GLIB_setClippingRegion(gcPtr, &rect);
  GLIB_resetDisplayClippingArea(gcPtr);

  /* Update text on top of picture  */
  gcPtr->foregroundColor = GLIB_rgbColor(200, 200, 200);
  GLIB_drawString(gcPtr, efm32_hello, strlen(efm32_hello), 0, 0, 1);

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
    gcPtr->foregroundColor = GLIB_rgbColor(128 + randomGenerator(127),
                                       randomGenerator(200),
                                       randomGenerator(255));
    GLIB_drawRectFilled(gcPtr, &rect);
  }
}
