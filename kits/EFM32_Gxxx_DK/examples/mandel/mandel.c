/**************************************************************************//**
 * @file
 * @brief mandelbrot example for EFM32_G2xx_DK using EBI or SPI access
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

#include <string.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "bsp.h"

#include "glib/glib.h"
#include "glib/glib_color.h"
#include "dmd/ssd2119/dmd_ssd2119.h"

/* Pixel color space */
typedef struct {
    uint8_t y, u, v;
} PIXEL_TypeDef;

/** Graphics context */
GLIB_Context_t gc;

/* Local prototypes */
void Delay(uint32_t dlyTicks);
void yuv2rgb(uint8_t y, uint8_t u, uint8_t v, uint8_t *r, uint8_t *g, uint8_t *b);
void mandelbrot(int resx, int resy, int x, int y, PIXEL_TypeDef *result);
void TFT_init(void);

volatile uint32_t msTicks; /* counts 1ms timeTicks */
/**************************************************************************//**
 * @brief SysTick_Handler
 * Interrupt Service Routine for system tick counter
 *****************************************************************************/
void SysTick_Handler(void)
{
  msTicks++;       /* increment counter necessary in Delay()*/
}

/**************************************************************************//**
 * @brief Delays number of msTick Systicks (typically 1 ms)
 * @param dlyTicks Number of ticks to delay
 *****************************************************************************/
void Delay(uint32_t dlyTicks)
{
  uint32_t curTicks;

  curTicks = msTicks;
  while ((msTicks - curTicks) < dlyTicks) ;
}


/**************************************************************************//**
 * @brief Simple YUV to RGB color space conversion
 * @param[in] y
 * @param[in] u
 * @param[in] v
 * @param r Red component value, in range 0-255
 * @param g Green component value, in range 0-255
 * @param b Blue component value, in range 0-255
 *****************************************************************************/
void yuv2rgb( uint8_t y, uint8_t u, uint8_t v,
              uint8_t *r, uint8_t *g, uint8_t *b)
{
    int32_t rr,gg,bb,yy;

    yy =  y << 16;

    bb = yy + 91947*v;
    gg = yy - 22544*u - 46792*v;
    rr = yy + 115998*u;

    *r = (uint8_t) (rr >> 16);
    *g = (uint8_t) (gg >> 16);
    *b = (uint8_t) (bb >> 16);
}


/**************************************************************************//**
 * @brief Generate a pixel with color, using mandelbrot calculation
 *   This is using an "allmost" text book algorithm
 *
 * @note
 *   To increase speed, this routine uses 8.24 fixed point arithmetics.
 *
 * @param[in] resx Horizontal resolution of final image
 * @param[in] resy Vertical resolution of final image
 * @param[in] x Horizontal position of pixel to render
 * @param[in] y Vertical position of pixel to render
 * @param pixel Resulting pixel value in YUV format
 *****************************************************************************/
#define PREC 12
void mandelbrot(int resx, int resy, int x, int y, PIXEL_TypeDef *result)
{
    int32_t x0, y0;
    int32_t xn, yn;
    int32_t x2, y2;
#if 1
    /* position of our mandelbrot image */
    int32_t startx = -2l << PREC, lengthx = 3l << PREC, starty = -1l << PREC, lengthy = 2l << PREC;
#else
    /* another "nice area" */
    //const float startx = -1.25, lengthx = 0.00625, starty = -0.0925, lengthy = 0.0075;
    int32_t startx = -5320, lengthx = 320, starty = -378, lengthy = 200;
#endif

    const unsigned int maxiterations = 100;
    int i;

    /* start position for iterations */
    xn = startx+ x * lengthx / resx;
    x0 = xn;
    yn = starty+ y * lengthy / resy;
    y0 = yn;
    /* xn^2, yn^2 */
    x2 = (x0 * x0);
    y2 = (y0 * y0);
    x2 = x2 >> PREC;
    y2 = y2 >> PREC;

    /* F(n) = F(n-1)^2 + F0 */
    for ( i=0; i < (int)maxiterations; i++) {

      /* Examine limit */
      if ( (x2 + y2) > (4l << PREC) ) break;

      yn = (xn*yn >> (PREC-1)) + y0;
      xn = x2 - y2 + x0;

      x2 = xn * xn;
      x2 = x2 >> PREC;
      y2 = yn * yn;
      y2 = y2 >> PREC;
    }

    /* Black, or add nice color */
    if ( i == (int)maxiterations ) {
      result->y = 0;
      result->u = 0;
      result->v = 0;
    } else {
      result->y = 255-(i%255);
      result->u = (255*i/maxiterations)/2;
      result->v = (i+50)%255;
    }
}

/**************************************************************************//**
 * @brief Initialize TFT display
 *****************************************************************************/
void TFT_init(void)
{
  static char *mandeltext = "EFM32 mandelbrot image";
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
  GLIB_drawString(&gc, mandeltext, strlen(mandeltext), 0, 230, 1);
}


/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  uint16_t aemState  = 0;
  int      firstRun  = 1;
  int      toggleLED = 0;
  PIXEL_TypeDef pix;
  int x, y;
  uint8_t r,g,b;

  /* Chip revision alignment and errata fixes */
  CHIP_Init();

  /* Use 32MHZ HFXO as core clock frequency */
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);

  /* Initialize DK board register access */
  BSP_Init(BSP_INIT_DEFAULT);

  /* Setup SysTick Timer for 10 msec interrupts  */
  if (SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 100))
  {
    while (1) ;
  }

  /* Update TFT display forever */
  while (1)
  {
    /* Check if we should control TFT display instead of AEM/board */
    /* control application. Read state of AEM pushbutton */
    aemState = BSP_RegisterRead(BC_AEMSTATE);
    if (aemState == 1)
    {
      /* Make sure TFT init is run at least once */
      if ((BSP_RegisterRead(BC_BUS_CFG) == BC_BUS_CFG_FSMC) || firstRun)
      {
        TFT_init();
        firstRun = 0;
      }
      /* Update display */
      /* iterate over  lines */
      for ( y=0; y < 240; y++ ) {
        /* iterate over pixels on each line */
        for ( x=0; x < 320; x++ ) {
          /* calculate mandelbrot at this _pixel_ position */
          mandelbrot(320, 240, x, y, &pix);
          yuv2rgb(pix.y, pix.u, pix.v, &r, &g, &b);
          GLIB_drawPixelRGB(&gc, x, y, r, g, b);
        }
      }
    }

    /* Toggle led after each TFT_displayUpdate iteration */
    if (toggleLED)
    {
      BSP_LedsSet(0x0000);
      toggleLED = 0;
    }
    else
    {
      BSP_LedsSet(0x000f);
      toggleLED = 1;
    }
  }
}
