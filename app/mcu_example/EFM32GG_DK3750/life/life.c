/**************************************************************************//**
 * @file
 * @brief Double buffering TFT Direct drive example for EFM32GG990F1024
 *        EFM32GG_DK3750. Implements Conway's Game of Life by using the
 *        double buffering capability of the EFM32GG990F1024 devices.
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
#include "glib/glib.h"
#include "tftdirect.h"

/* Color component shifts for RGB-565 */
#define RED_SHIFT      11
#define GREEN_SHIFT    5
#define BLUE_SHIFT     0

/* Display size */
#define WIDTH          320                      /** Width of display in pixels (16-bit per pixel) */
#define HEIGHT         240                      /** Height of display in pixels */
#define FRAME_A        (WIDTH * HEIGHT * 2 * 0) /** Offset to frame buffer A */
#define FRAME_B        (WIDTH * HEIGHT * 2 * 1) /** Offset to frame buffer B */
#define FRAME_C        (WIDTH * HEIGHT * 2 * 2) /** Offset to frame buffer C */


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

/* Green 5x5 green pixel dot image */
static const uint16_t greendot[] = {
  0x0000, 0x0040, 0x1246, 0x0881, 0x0040, 0x0040, 0x2389, 0x252e,
  0x9672, 0x0040, 0x0a26, 0x254f, 0x25b0, 0x25d0, 0x0a46, 0x00c2,
  0x246c, 0x254f, 0x248d, 0x00c2, 0x0020, 0x11a4, 0x13aa, 0x11c5,
  0x0000,
};

/* Game of Life size of playing field */
#define LIFE_WIDTH     (WIDTH / 5)  /** 320 pixels divided by 5 for each "dot" */
#define LIFE_HEIGHT    (HEIGHT / 5) /** 240 pixels divided by 5 for each "dot" */

/* Board containers */
static uint8_t life1[LIFE_WIDTH * LIFE_HEIGHT];
static uint8_t life2[LIFE_WIDTH * LIFE_HEIGHT];

/** Counts 1 ms timeTicks */
static volatile uint32_t msTicks;

/** Small, simple random generator seed */
static uint32_t randomNumber = 0xcafebabe;

/** Offset into frame buffer memory */
static volatile int nextOffset = -1;

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
 * @brief Draw a WIDTH*HEIGHT frame buffer with green dots represeting a life
 *        cycle
 * @param[in] offset Offset into frame buffer memory to update
 * @param[in] board Game of Life board
 *****************************************************************************/
void TFT_DrawLife(int offset, uint8_t *board)
{
  int               x, y;
  int               i;
  volatile uint16_t *frameBuffer = (uint16_t *)(EBI_BankAddress(EBI_BANK2) + offset);
  volatile uint32_t *clearBuffer = (uint32_t *) frameBuffer;


  /* Clear frame buffer, set to black background  */
  for (i = 0; i < ((WIDTH * HEIGHT) / 2); i++)
  {
    *clearBuffer++ = 0x00000000;
  }

  /* Iterate over Game Of Life world and draw "life" */
  for (y = 0; y < LIFE_HEIGHT; y++)
  {
    for (x = 0; x < LIFE_WIDTH; x++)
    {
      /* Check for life, if present, draw it */
      if (board[y * LIFE_WIDTH + x] != 0)
      {
        /* Display green dot */
        for (i = 0; i < 25; i++)
        {
          frameBuffer[(y * 5 + i / 5) * WIDTH + x * 5 + i % 5] = greendot[i];
        }
      }
    }
  }
}


/**************************************************************************//**
 * @brief EBI interrupt routine, configures new frame buffer offset
 *****************************************************************************/
void EBI_IRQHandler(void)
{
  uint32_t flags;

  /* Get cause of interrupt */
  flags = EBI_IntGet();
  /* Clear interrupt */
  EBI_IntClear(flags);

  /* Process VSYNC interrupt */
  if (flags & EBI_IF_VFPORCH)
  {
    /* Swap buffers if new frame is ready */
    if (nextOffset != -1)
    {
      EBI_TFTFrameBaseSet(nextOffset);
      nextOffset = -1;
    }
  }
}

/**************************************************************************//**
 * @brief Sets up a frame to be updated in the next screen refresh, return
 *        pointer to next update offset
 *****************************************************************************/
int TFT_SwapBuffer(int frameDone)
{
  /* If nextOffset != -1, the previous screen has not been displayed yet, so wait */
  while (nextOffset != -1) ;

  /* Set this buffer to be displayed on next update, draw graphics into next */
  switch (frameDone)
  {
  case FRAME_A:
    nextOffset = FRAME_A;
    return(FRAME_B);
  case FRAME_B:
    nextOffset = FRAME_B;
    return(FRAME_C);
  case FRAME_C:
    nextOffset = FRAME_C;
    return(FRAME_A);
  default:
    nextOffset = FRAME_A;
    return(FRAME_A);
  }
}

/**************************************************************************//**
 * @brief Makes a new Game of Life board from a previous generation
 *        See wikipedia article for details
 *        http://en.wikipedia.org/wiki/Conway's_Game_of_Life
 * @param[out] new New life cycle to be updated
 * @param[in] previous Previous life cycle
 *****************************************************************************/
void GameOfLife(uint8_t *new, const uint8_t *previous)
{
  int x, y;
  int neighbours, cell;

  /* Keep all life within one pixel boundary, in practice a 62*46 board */
  for (y = 1; y < (LIFE_HEIGHT - 1); y++)
  {
    for (x = 1; x < (LIFE_WIDTH - 1); x++)
    {
      neighbours = 0;
      /* live cell */
      if (previous[y * LIFE_WIDTH + x] == 1)
      {
        if (previous[y * LIFE_WIDTH + x - 1] == 1) neighbours++;
        if (previous[y * LIFE_WIDTH + x + 1] == 1) neighbours++;
        if (previous[(y - 1) * LIFE_WIDTH + x] == 1) neighbours++;
        if (previous[(y - 1) * LIFE_WIDTH + x - 1] == 1) neighbours++;
        if (previous[(y - 1) * LIFE_WIDTH + x + 1] == 1) neighbours++;
        if (previous[(y + 1) * LIFE_WIDTH + x] == 1) neighbours++;
        if (previous[(y + 1) * LIFE_WIDTH + x - 1] == 1) neighbours++;
        if (previous[(y + 1) * LIFE_WIDTH + x + 1] == 1) neighbours++;
        /* assume living */
        cell = 1;
        if (neighbours < 2) cell = 0;
        if (neighbours > 3) cell = 0;
        new[y * LIFE_WIDTH + x] = cell;
      }
      /* dead cell */
      else
      {
        if (previous[y * LIFE_WIDTH + x - 1] == 1) neighbours++;
        if (previous[y * LIFE_WIDTH + x + 1] == 1) neighbours++;
        if (previous[(y - 1) * LIFE_WIDTH + x] == 1) neighbours++;
        if (previous[(y - 1) * LIFE_WIDTH + x - 1] == 1) neighbours++;
        if (previous[(y - 1) * LIFE_WIDTH + x + 1] == 1) neighbours++;
        if (previous[(y + 1) * LIFE_WIDTH + x] == 1) neighbours++;
        if (previous[(y + 1) * LIFE_WIDTH + x - 1] == 1) neighbours++;
        if (previous[(y + 1) * LIFE_WIDTH + x + 1] == 1) neighbours++;
        /* assume dead */
        cell = 0;
        if (neighbours == 3) cell = 1;
        new[y * LIFE_WIDTH + x] = cell;
      }
    }
  }
}


/**************************************************************************//**
 * @brief Initialize a new Game of Life board
 *        See wikipedia article for details
 *        http://en.wikipedia.org/wiki/Conway's_Game_of_Life
 * @param[in] board Pointer to game of life state area
 *****************************************************************************/
void GameOfLifeInit(uint8_t *board)
{
  int x, y;

  /* Keep all life within one pixel boundary, in practice a 62*46 board */
  for (y = 1; y < (LIFE_HEIGHT - 1); y++)
  {
    for (x = 1; x < (LIFE_WIDTH - 1); x++)
    {
      randomNumber = (((randomNumber * 27) << 8) | (((randomNumber / 13) & 0xa5a7f196) + (randomNumber >> 13))) - DEVINFO->UNIQUEL;
      if (randomNumber & 1)
      {
        board[y * LIFE_WIDTH + x] = 0;
      }
      else
      {
        board[y * LIFE_WIDTH + x] = 1;
      }
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
  int  frameOffset = FRAME_B;
  int  lifeCycles  = 0;
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

  /* Indicate we are waiting for AEM button state enable EFM32GG */
  BSP_LedsSet(0x8001);
  while (BSP_RegisterRead(&BC_REGISTER->UIF_AEM) != BC_UIF_AEM_EFM)
  {
    /* Show a short "strobe light" on DK LEDs, indicating wait */
    BSP_LedsSet(0x8001);
    Delay(200);
    BSP_LedsSet(0x4002);
    Delay(50);
  }

  /* Initialize game of life */
  GameOfLifeInit(life2);

  /* Enable VSYNC interrupts */
  TFT_IRQEnable(EBI_IF_VFPORCH);

  /* Loop demo forever */
  while (1)
  {
    /* Initialize or reinitialize display if necessary */
    redraw = TFT_DirectInit(&tftInit);

    /* If redraw is true, we have control over display */
    if (redraw)
    {
      /* Run life cycle */
      lifeCycles++;
      GameOfLife(life1, life2);
      TFT_DrawLife(frameOffset, life1);

      /* On next VSYNC, update frame buffer */
      frameOffset = TFT_SwapBuffer(frameOffset);

      /* Run life cycle */
      lifeCycles++;
      GameOfLife(life2, life1);
      TFT_DrawLife(frameOffset, life2);

      /* On next VSYNC, update frame buffer */
      frameOffset = TFT_SwapBuffer(frameOffset);

      /* Restart afer a set number of generations */
      if (lifeCycles > 500)
      {
        lifeCycles = 0;
        GameOfLifeInit(life2);
      }
    }
    else
    {
      /* Sleep - no need to update display */
      BSP_LedsSet(0x8001);
      Delay(200);
    }
  }
}
