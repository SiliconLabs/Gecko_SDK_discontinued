/**************************************************************************//**
 * @file
 * @brief TFT Direct Drive horizontal scroller for Leopard Gecko / Leopard Gecko
 *        EFM32LG-DK3650 development kit. This demo uses the HSYNC interrupt
 *        capability to update screen refresh pointers, and thus enabling a
 *        "stock ticker" type of scroller on the display.
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
#include <stdint.h>
#include <stdbool.h>
#include "em_device.h"
#include "em_cmu.h"
#include "em_ebi.h"
#include "em_gpio.h"
#include "bsp.h"
#include "bsp_trace.h"
#include "tftdirect.h"
#include "graphics.h"

/* Read physical display size */
#define D_WIDTH     320
#define D_HEIGHT    240

/* Virtual display size */
#define V_WIDTH     672 /* 320x2+(font width)*2 pixels */
#define V_HEIGHT    320

/* Configure TFT direct drive from EBI BANK2 */
const EBI_TFTInit_TypeDef tftInit =
{ ebiTFTBank2,                  /* Select EBI Bank 2 */
  ebiTFTWidthHalfWord,          /* Select 2-byte (16-bit RGB565) increments */
  ebiTFTColorSrcMem,            /* Use memory as source for mask/blending */
  ebiTFTInterleaveUnlimited,    /* Unlimited interleaved accesses */
  ebiTFTFrameBufTriggerHSync,   /* HSYNC as frame buffer update trigger */
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
  8,                            /* DCLK Period, ~60fps at 48MHz */
  0,                            /* DCLK Start cycles */
  0,                            /* DCLK Setup cycles */
  0,                            /* DCLK Hold cycles */
};

/** Counts 1 ms timeTicks */
static volatile uint32_t msTicks;

/** Keep track of horizontal offset */
static uint32_t          hzOffset;
static uint32_t          hPos;
static volatile uint32_t frameCtr;

/* Text to scroll over display */
const char *scrolltext = " The Silicon Labs Simplicity Team brings you the "
                         "EFM32LG-DK3650 development kit, with cool TFT direct drive features.....";
const char *scrollptr;

/* Colours in 16-bit RGB565 */
#define RED          (0x1f << 11)
#define GREEN        (0x3f << 5)
#define DARKGREEN    (0x08 << 5)
#define BLUE         (0x1f)

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
 * @brief Clear entire virtual dispaly
 *****************************************************************************/
void TFT_DisplayClear(void)
{
  uint32_t *frameBuffer = (uint32_t *) EBI_BankAddress(EBI_BANK2);
  int      i;

  /* Clear entire display using 32-bit write operations */
  for (i = 0; i < ((V_WIDTH * V_HEIGHT) / 2); i++)
  {
    *frameBuffer++ = 0x00000000;
  }
}


/**************************************************************************//**
 * @brief Draw coloured rectangle
 * @note This function does not do any boundary checking!
 * @param[in] x Horizontal start position into frame buffer in pixels
 * @param[in] y Verticasl start position into frame buffer in pixels
 * @param[in] xsize Horizontal size in pixels
 * @param[in] ysize Vertical size in pixels
 * @param[in] color 16-bit RGB565 color of rectangle
 *****************************************************************************/
void TFT_DrawRectangle(int x, int y, int xsize, int ysize, uint16_t color)
{
  uint16_t *frameBuffer = (uint16_t *) EBI_BankAddress(EBI_BANK2);
  int      i, j;

  /* Fill area with coloured pattern */
  for (j = y; j < (y + ysize); j++)
  {
    for (i = x; i < (x + xsize); i++)
    {
      frameBuffer[j * V_WIDTH + i] = color;
    }
  }
}


/**************************************************************************//**
 * @brief Draw an image of the Gecko and position x,y
 * @param[in] x Horizontal start position into frame buffer in pixels
 * @param[in] y Verticasl start position into frame buffer in pixels
 *****************************************************************************/
void TFT_DrawGecko(int x, int y)
{
  uint16_t *frameBuffer = (uint16_t *) EBI_BankAddress(EBI_BANK2);
  int      i, j;

  /* Iterate over lines and then pixels */
  for (j = 0; j < GECKO_HEIGHT; j++)
  {
    for (i = 0; i < GECKO_WIDTH; i++)
    {
      frameBuffer[(y + j) * V_WIDTH + x + i] = gecko[j * GECKO_WIDTH + i];
    }
  }
}


/**************************************************************************//**
 * @brief Draw a lot of Geckos at different positions
 *****************************************************************************/
void TFT_DrawGraphics(void)
{
  int i;

  /* Draw a number of Gecko on screen */
  for (i = 0; i <= 160; i += 20)
  {
    /* Set increasing intensity when BLEND mode is active */
    EBI_TFTAlphaBlendSet((255 * i) / 160);
    TFT_DrawGecko(i, 70);
    Delay(100);
  }
}


/**************************************************************************//**
 * @brief Draw single character into frame buffer
 * @param[in] x Horizontal start position into frame buffer in pixels
 * @param[in] y Verticasl start position into frame buffer in pixels
 * @param[in] c Character to draw
 *****************************************************************************/
void TFT_DrawFont(int x, int y, int c)
{
  uint16_t     *frameBuffer = (uint16_t *) EBI_BankAddress(EBI_BANK2);
  unsigned int i, j;
  int          fontOffset = c - 0x20;
  uint16_t     color;

  frameBuffer += (x);
  frameBuffer += (y * V_WIDTH);

  /* Draw a single character, iterate over lines and pixels */
  for (j = 0; j < font16x28.c_height; j++)
  {
    for (i = 0; i < font16x28.c_width; i++)
    {
      color          = font16x28.data[j * font16x28.width + fontOffset * font16x28.c_width + i];
      *frameBuffer++ = color;
    }
    frameBuffer += (V_WIDTH - font16x28.c_width);
  }
}


/**************************************************************************//**
 * @brief Draw single character into frame buffer
 * @param[in] x Horizontal start position into frame buffer in pixels
 * @param[in] y Verticasl start position into frame buffer in pixels
 * @param[in] str String to draw
 *****************************************************************************/
void TFT_DrawString(int x, int y, char *str)
{
  /* Draw string, no boundary checks are performed */
  while (*str)
  {
    TFT_DrawFont(x, y, *str++);
    x += 16;
  }
}


/**************************************************************************//**
 * @brief EBI interrupt routine, configures new frame buffer offset
 *****************************************************************************/
void EBI_IRQHandler(void)
{
  uint32_t flags;
  uint32_t lineNumber;
  uint32_t prevMaskBlend;

  /* Get source for interrupt */
  flags = EBI_IntGet();

  /* Clear interrupt */
  EBI_IntClear(flags);

  /* Process vertical sync interrupt */
  if (flags & EBI_IF_VFPORCH)
  {
    /* Keep tracke of number of frames drawn */
    frameCtr++;

    /* Increase this increment to 2/4/8 to increase scroll speed */
    hzOffset += 1;

    /* Wrap around when a full screen has been displayed */
    if (hzOffset == (D_WIDTH + font16x28.c_width))
    {
      hzOffset = 0;
    }

    /* We only redraw when we need to */
    if (hzOffset != hPos)
    {
      hPos = hzOffset;
      /* We only redraw when we need a new character */
      if ((hPos % 16) == 0)
      {
        /* Update frame buffer - we do this in the interrupt routine to    */
        /* ensure a smooth scroller no matter what else is being done.     */
        /* However, drawing these fonts with low optimization will take    */
        /* more than one scan line, so this _can_ result in tearing at the */
        /* top of the screen for this example.                             */
        prevMaskBlend = EBI->TFTCTRL & _EBI_TFTCTRL_MASKBLEND_MASK;
        EBI_TFTMaskBlendMode(ebiTFTMBDisabled);
        TFT_DrawFont(D_WIDTH + hPos, D_HEIGHT - font16x28.height, *scrollptr);
        if (hPos > 0)
        {
          TFT_DrawFont(hPos - font16x28.c_width, D_HEIGHT - font16x28.height, *scrollptr);
        }
        EBI->TFTCTRL |= prevMaskBlend;
        /* Update pointer into text, reset if at last character */
        scrollptr++;
        if (*scrollptr == '\0')
        {
          scrollptr = scrolltext;
        }
      }
    }
  }

  /* Process horizontal sync interrupt */
  if (flags & EBI_IF_HSYNC)
  {
    lineNumber = EBI_TFTVCount();

    /* Adjust for porch size */
    if (lineNumber >= 3) lineNumber -= 3;
    if (lineNumber < (D_HEIGHT - font16x28.c_height))
    {
      EBI_TFTFrameBaseSet(lineNumber * V_WIDTH * 2);
    }
    else
    {
      EBI_TFTFrameBaseSet(lineNumber * V_WIDTH * 2 + hzOffset * 2);
    }
  }
}


/**************************************************************************//**
 * @brief Configure EBI/TFT interrupt
 * @param[in] flags Interrupt flags for EBI to enable
 *****************************************************************************/
void TFT_IRQEnable(uint32_t flags)
{
  /* Disable interrupts */
  EBI_IntDisable(_EBI_IF_MASK);

  /* Clear previous interrupts */
  EBI_IntClear(_EBI_IF_MASK);

  /* Enable required interrupt */
  EBI_IntEnable(flags);

  NVIC_ClearPendingIRQ(EBI_IRQn);
  NVIC_EnableIRQ(EBI_IRQn);
}


/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  bool redraw;

  /* Configure for 48MHz HFXO operation of core clock */
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);

  /* Setup SysTick Timer for 1 msec interrupts  */
  if (SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 1000))
  {
    while (1) ;
  }

  /* Initialize EBI banks (Board Controller, external PSRAM, ..) */
  BSP_Init(BSP_INIT_DEFAULT);

  /* If first word of user data page is non-zero, enable eA Profiler trace */
  BSP_TraceProfilerSetup();

  /* Indicate we are waiting for AEM button state enable EFM32LG */
  BSP_LedsSet(0x8001);
  while (BSP_RegisterRead(&BC_REGISTER->UIF_AEM) != BC_UIF_AEM_EFM)
  {
    /* Show a short "strobe light" on DK LEDs, indicating wait */
    BSP_LedsSet(0x8001);
    Delay(200);
    BSP_LedsSet(0x4002);
    Delay(50);
  }

  /* Initialize scroll text pointer */
  scrollptr = scrolltext;

  /* Initial initialization, enables clocks etc */
  redraw = TFT_DirectInit(&tftInit);

  /* Configure common settings */
  EBI_TFTHStrideSet((V_WIDTH - D_WIDTH) * 2);
  TFT_IRQEnable(EBI_IF_VFPORCH | EBI_IF_HSYNC);
  TFT_DisplayClear();

  /* Loop demo forever */
  while (1)
  {
    /* If necessary, reinitalize display and report back if we are active */
    redraw = TFT_DirectInit(&tftInit);
    if (redraw)
    {
      /* Inidicate that we are in active drawing mode */
      BSP_LedsSet(0x0000);

      /* Update frame buffer - do a simple block copy */
      TFT_DrawString(0, 10, "    Regular copy    ");
      TFT_DrawRectangle(0, 50, D_WIDTH, 140, DARKGREEN);
      TFT_DrawGraphics();
      EBI_TFTMaskBlendMode(ebiTFTMBDisabled);
      Delay(1000);

      /* Update frame buffer - enable masking */
      TFT_DrawString(0, 10, "      Masking       ");
      TFT_DrawRectangle(0, 50, D_WIDTH, 140, DARKGREEN);
      EBI_TFTMaskSet(0x0000);
      EBI_TFTMaskBlendMode(ebiTFTMBEMask);
      TFT_DrawGraphics();
      EBI_TFTMaskBlendMode(ebiTFTMBDisabled);
      Delay(1000);

      /* Update frame buffer - enable masking and blending */
      TFT_DrawString(0, 10, " Masking & Blending ");
      TFT_DrawRectangle(0, 50, D_WIDTH, 140, DARKGREEN);
      EBI_TFTMaskBlendMode(ebiTFTMBEMaskAlpha);
      TFT_DrawGraphics();
      EBI_TFTMaskBlendMode(ebiTFTMBDisabled);
      Delay(1000);
    }
    else
    {
      /* Sleep - no need to update display */
      BSP_LedsSet(0x8001);

      /* Instead of this, we could go into energy mode 3 */
      /* waiting for an interrupt on GPIO/keypress */
      Delay(200);
    }
  }
}
