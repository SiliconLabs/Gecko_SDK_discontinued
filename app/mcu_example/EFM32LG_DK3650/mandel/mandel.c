/**************************************************************************//**
 * @file
 * @brief Mandelbrot example for EFM32LG_DK3650 development kit
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

#include <string.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_ebi.h"
#include "bsp.h"
#include "bsp_trace.h"
#include "tftdirect.h"

/* Configure TFT direct drive from EBI BANK2 */
static const EBI_TFTInit_TypeDef tftInit =
{ ebiTFTBank2,                  /* Select EBI Bank 2 */
  ebiTFTWidthHalfWord,          /* Select 2-byte (16-bit RGB565) increments */
  ebiTFTColorSrcMem,            /* Use memory as source for mask/blending */
  ebiTFTInterleaveUnlimited,    /* Unlimited interleaved accesses */
  ebiTFTFrameBufTriggerVSync,   /* VSYNC as frame buffer update trigger */
  false,                        /* Drive DCLK from negative edge of internal clock */
  ebiTFTMBDisabled,             /* No masking and alpha blending enabled */
  ebiTFTDDModeExternal,         /* Drive from external memory */
  ebiActiveLow,                 /* CS Active Low polarity */
  ebiActiveHigh,                /* DCLK Active High polarity */
  ebiActiveLow,                 /* DATAEN Active Low polarity */
  ebiActiveLow,                 /* HSYNC Active Low polarity */
  ebiActiveLow,                 /* VSYNC Active Low polarity */
  320,                          /* Horizontal size in pixels */
  1,                            /* Horizontal Front Porch */
  30,                           /* Horizontal Back Porch */
  2,                            /* Horizontal Synchronization Pulse Width */
  240,                          /* Vertical size in pixels */
  1,                            /* Vertical Front Porch */
  4,                            /* Vertical Back Porch */
  2,                            /* Vertical Synchronization Pulse Width */
  0x0000,                       /* Frame Address pointer offset to EBI memory base */
  4,                            /* DCLK Period */
  0,                            /* DCLK Start cycles */
  0,                            /* DCLK Setup cycles */
  0,                            /* DCLK Hold cycles */
};

/* Pixel color space */
typedef struct {
    uint8_t y, u, v;
} PIXEL_TypeDef;

static bool upCount = true;
static uint32_t frameNumber = 0;
static uint32_t frameCount = 0;
static uint32_t irqCounter = 0;
static uint32_t skipFrames = 4;

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
 * @brief EBI interrupt routine, configures new frame buffer offset
 *****************************************************************************/
void EBI_IRQHandler(void)
{
  uint32_t flags;
  uint32_t button;

  /* Get cause of interrupt */
  flags = EBI_IntGet();
  /* Clear interrupt */
  EBI_IntClear(flags);

  /* Keep track of number of interrupts to control frame update rate */
  irqCounter++;

  button = BSP_PushButtonsGet();
  /* Adjust frame rate with buttons */
  if(button)
  {
    skipFrames = button;
  }

  /* Process VSYNC interrupt */
  if (flags & EBI_IF_VFPORCH)
  {
    /* Swap buffers if new frame is ready */
    if (frameCount <= 1)
    {
      EBI_TFTFrameBaseSet(0);
    } else {
      /* Update screen at ~15 fps */
      if ((irqCounter % skipFrames) != 0) return;

      EBI_TFTFrameBaseSet(0+(320*240*sizeof(uint16_t))*frameNumber);
      /* Ping pong next buffers */
      if(upCount)
      {
        frameNumber = frameNumber + 1;
        if(frameNumber >= frameCount)
        {
          upCount = false;
          frameNumber = frameNumber - 1;
        }
      }
      else
      {
        frameNumber = frameNumber - 1;
        if(frameNumber <= 0)
        {
          upCount = true;
        }
      }
    }
  }
}

/**************************************************************************//**
 * @brief Simple YUV to RGB color space conversion
 * @param[in] y
 * @param[in] u
 * @param[in] v
 * @return color in RGB565 16-bit format
 *****************************************************************************/
__STATIC_INLINE uint16_t YUV2RGB565( uint8_t y, uint8_t u, uint8_t v)
{
    int32_t  rr,gg,bb,yy;
    uint8_t  r,g,b;
    uint16_t color;
    yy =  y << 16;

    bb = yy + 91947*v;
    gg = yy - 22544*u - 46792*v;
    rr = yy + 115998*u;

    r = (rr >> 19);
    g = (gg >> 18);
    b = (bb >> 19);

    color = (r<<11)|(g<5)|b;
    return color;
}


/**************************************************************************//**
 * @brief Generate a pixel with color, using mandelbrot calculation
 *   This is very close to a text book algorithm
 * @note
 *   To increase speed, this routine uses 8.24 fixed point arithmetics.
 * @param[in] frameCount Frame number for animation purposes
 * @param[in] resx Horizontal resolution of final image
 * @param[in] resy Vertical resolution of final image
 * @param[in] x Horizontal position of pixel to render
 * @param[in] y Vertical position of pixel to render
 * @param pixel Resulting pixel value in YUV format
 *****************************************************************************/
#define PREC 12
void Mandelbrot(int frameCount, int resx, int resy, int x, int y, PIXEL_TypeDef *result)
{
    int32_t x0, y0;
    int32_t xn, yn;
    int32_t x2, y2;
    int i;
#if 0
    /* position of our mandelbrot image */
    unsigned int maxiterations = 100;
    int32_t startx = -2l << PREC, lengthx = 3l << PREC, starty = -1l << PREC, lengthy = 2l << PREC;

    startx += (frameCount * 300);
    starty += (frameCount * 40);
    lengthx -= (frameCount * 300);
    lengthy -= (frameCount * 200);
#else
    /* another nice area */
    unsigned int maxiterations = 100 + frameCount * 2;
    int32_t startx = 1500, lengthx = 400, starty = -1500, lengthy = 300;

    startx += (frameCount * 3);
    starty -= (frameCount * 2);
    lengthx -= (frameCount * 12);
    lengthy -= (frameCount * 10);
#endif

    /* start position for iterations */
    xn = startx + x * lengthx / resx;
    x0 = xn;
    yn = starty + y * lengthy / resy;
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
    if ( i == 0 ) i=1;
    if ( i == (int)maxiterations ) {
      result->y = 0;
      result->u = 0;
      result->v = 0;
    } else {
      result->y = 0xff-(i%0xff);
      result->u = (0xff*i/maxiterations);
      result->v = (i+0x40)%0xff;
    }
}

/**************************************************************************//**
 * @brief Draw pixel with given color at thie framebuffer location
 *****************************************************************************/
void PixelSet(uint32_t frame, uint32_t x, uint32_t y, uint16_t color)
{
  uint16_t *frameBuffer = (uint16_t *) EBI_BankAddress(EBI_BANK2)+(320*240*frame);

  frameBuffer += (y*320+x);

  *frameBuffer = color;
}


/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  PIXEL_TypeDef pix;
  int x, y;

  /* Chip revision alignment and errata fixes */
  CHIP_Init();

  /* Use 48MHZ HFXO as core clock frequency */
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);

  /* Initialize DK board register access */
  BSP_Init(BSP_INIT_DEFAULT);

  /* If first word of user data page is non-zero, enable eA Profiler trace */
  BSP_TraceProfilerSetup();

  /* Setup SysTick Timer for 10 msec interrupts  */
  if (SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 100))
  {
    while (1) ;
  }

  /* Disable interrupts */
  EBI_IntDisable(_EBI_IF_MASK);

  /* Clear previous interrupts */
  EBI_IntClear(_EBI_IF_MASK);

  /* Enable required interrupt */
  EBI_IntEnable(EBI_IF_VFPORCH);

  NVIC_ClearPendingIRQ(EBI_IRQn);
  NVIC_EnableIRQ(EBI_IRQn);

  /* Update TFT display forever */
  while (1)
  {
    /* Initialize or reinitialize display if necessary */
    TFT_DirectInit(&tftInit);

    /* Generate new mandelbrot image */
    /* iterate over  lines */
    for ( y=0; y<240; y++ )
    {
      /* iterate over pixels on each line */
      for ( x=0; x<320; x++ )
      {
        /* calculate mandelbrot at this _pixel_ position */
        Mandelbrot(frameCount, 320, 240, x, y, &pix);
        PixelSet(frameCount, x, y, YUV2RGB565(pix.y, pix.u, pix.v));
      }
    }
    /* All images are done, EBI interrupts does the rest */
    if(frameCount < 26) {
      frameCount++;
    } else {
      while(1)
      {
        /* Go to sleep */
        EMU_EnterEM1();
        /* Check for reinitialization */
        TFT_DirectInit(&tftInit);
      }
    }
  }
}
