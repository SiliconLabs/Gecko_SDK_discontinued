/**************************************************************************//**
 * @file
 * @brief Touch example for EFM32LG_DK3650 development kit
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
#include "em_device.h"
#include "em_adc.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_usart.h"
#include "em_common.h"

#include "bsp.h"
#include "bsp_trace.h"
#include "tftdirect.h"
#include "calibrate.h"
#include "graphics.h"

/* Used by delayMs() and delayUs() functions. */
#define DWT_CYCCNT      *(volatile uint32_t*)0xE0001004
#define DWT_CTRL        *(volatile uint32_t*)0xE0001000

#define EXIT_LOOP       0x10
#define LCD_TOUCH_X1_PORT    gpioPortD
#define LCD_TOUCH_X1_PIN     5
#define LCD_TOUCH_X2_PORT    gpioPortD      /* ADC0_Ch4 Loc 0 */
#define LCD_TOUCH_X2_PIN     4
#define LCD_TOUCH_Y1_PORT    gpioPortD
#define LCD_TOUCH_Y1_PIN     3
#define LCD_TOUCH_Y2_PORT    gpioPortD      /* ADC0_Ch1 Loc 0 */
#define LCD_TOUCH_Y2_PIN     1

#define ADC_X           adcSingleInpCh4
#define ADC_Y           adcSingleInpCh1

/* Display size */
#define WIDTH           320
#define HEIGHT          240
#define BLACK           0x0000
#define WHITE           0xFFFF
#define GREEN           0x07E0
#define COLOR           GREEN

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
  8,                            /* DCLK Period */
  0,                            /* DCLK Start cycles */
  0,                            /* DCLK Setup cycles */
  0,                            /* DCLK Hold cycles */
};

static const POINT lcdCalibPoints[ 3 ] =  /* Display size is 320x240 */
{
  {  32,  24 },     /* 10%, 10% */
  { 288, 120 },     /* 90%, 50% */
  { 160, 216 },     /* 50%, 90% */
};

static const POINT touchCalibPoints[ 3 ] =
{
  { 160, 210 },       /* 10%, 10% */
  { 860, 520 },       /* 90%, 50% */
  { 515-50, 805-50 }, /* 50%, 90% 50 units off to prove calibration algorithm */
};

static uint16_t                 *frameBuffer;
static ADC_InitSingle_TypeDef   sInit = ADC_INITSINGLE_DEFAULT;
static MATRIX                   calibFactors;

static void           delayMs( uint32_t msec );
static void           delayUs( uint32_t usec );
static void           displayHelpScreen( void );
static void           drawCross( uint32_t xpos, uint32_t ypos, uint16_t color );
__STATIC_INLINE void  drawPixel( uint32_t xpos, uint32_t ypos, uint16_t color );
static uint32_t       getTouchChSample12bit( ADC_SingleInput_TypeDef ch );
static POINT          getTouchSample( void );
static POINT          getTouchSample10bit( void );
static POINT          getTouchSample12bit( void );
static POINT          getTouchTapSample10bit( void );
static uint32_t       readAdc( void );
static uint32_t       readButtons( void );
static bool           touched( void );

void TFT_DrawFont(int x, int y, int c);
void TFT_DrawString(int x, int y, char *str);

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  uint32_t buttons;
  POINT touchSample, P[ 3 ];
  ADC_Init_TypeDef init = ADC_INIT_DEFAULT;

  /* Configure for 48MHz HFXO operation of core clock */
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);

  /* Initialize DK board register access */
  BSP_Init(BSP_INIT_DEFAULT);

  /* If first word of user data page is non-zero, enable eA Profiler trace */
  BSP_TraceProfilerSetup();

  CMU_ClockEnable(cmuClock_GPIO, true);

  CMU_ClockEnable( cmuClock_ADC0, true);
  /* Max ADC clock is 13MHz, use 14MHz/(1+1) or 48MHz/(5+1) */
  init.prescale = 5;
  ADC_Init(ADC0, &init);
  sInit.reference = adcRefVDD;

  /* Set frame buffer start address */
  frameBuffer = (uint16_t *) EBI_BankAddress(EBI_BANK2);

  /* Make sure CYCCNT is running, needed by delay functions. */
  DWT_CTRL |= 1;

  /* Indicate we are waiting for AEM button state enabling EFM */
  BSP_LedsSet(0x8001);
  while (BSP_RegisterRead(&BC_REGISTER->UIF_AEM) != BC_UIF_AEM_EFM)
  {
    /* Show a short "strobe light" on DK LEDs, indicating wait */
    BSP_LedsSet(0x8001);
    delayMs(200);
    BSP_LedsSet(0x4002);
    delayMs(50);
  }

  /* Initialize touch screen calibration factor matrix with approx. values  */
  setCalibrationMatrix( (POINT*)&lcdCalibPoints,   /* Display coordinates   */
                        (POINT*)&touchCalibPoints, /* Touch coordinates     */
                        &calibFactors );      /* Calibration factor matrix  */

  while (1)
  {
    if ( TFT_DirectInit(&tftInit) )
    {
      displayHelpScreen();
      BSP_LedsSet(0x0000);
      BSP_PeripheralAccess(BSP_TOUCH, true);
      GPIO_PinModeSet(LCD_TOUCH_X1_PORT, LCD_TOUCH_X1_PIN, gpioModeInput, 0);
      GPIO_PinModeSet(LCD_TOUCH_X2_PORT, LCD_TOUCH_X2_PIN, gpioModeInput, 0);
      GPIO_PinModeSet(LCD_TOUCH_Y1_PORT, LCD_TOUCH_Y1_PIN, gpioModeInput, 0);
      GPIO_PinModeSet(LCD_TOUCH_Y2_PORT, LCD_TOUCH_Y2_PIN, gpioModeInput, 0);

      do
      {
        buttons = readButtons();

        /* Draw on screen */
        if ( buttons & BC_UIF_PB1 )
        {
          memset( frameBuffer, BLACK, 2 * WIDTH * HEIGHT );   /* Clear screen */

          do
          {
            if ( touched() )
            {
              touchSample = getTouchSample();
              drawPixel( touchSample.x, touchSample.y, COLOR );
            }
            delayMs( 2 );

            buttons = readButtons() & ~BC_UIF_PB1;
          } while ( buttons == 0 );
        }

        /* Calibrate touch screen */
        else if ( buttons & BC_UIF_PB2 )
        {
          memset( frameBuffer, BLACK, 2 * WIDTH * HEIGHT );   /* Clear screen */
          drawCross( lcdCalibPoints[ 0 ].x, lcdCalibPoints[ 0 ].y, COLOR );
          TFT_DrawString(30, 35, "Tap green marker" );
          P[ 0 ] = getTouchTapSample10bit();

          memset( frameBuffer, BLACK, 2 * WIDTH * HEIGHT );   /* Clear screen */
          drawCross( lcdCalibPoints[ 1 ].x, lcdCalibPoints[ 1 ].y, COLOR );
          TFT_DrawString(40, 130, "Tap green marker" );
          P[ 1 ] = getTouchTapSample10bit();

          memset( frameBuffer, BLACK, 2 * WIDTH * HEIGHT );   /* Clear screen */
          drawCross( lcdCalibPoints[ 2 ].x, lcdCalibPoints[ 2 ].y, COLOR );
          TFT_DrawString(20, 180, "Tap green marker" );
          P[ 2 ] = getTouchTapSample10bit();

          setCalibrationMatrix( (POINT*)&lcdCalibPoints,/* Display coordinates*/
                                &P[0],                  /* Touch coordinates  */
                                &calibFactors );  /* Calibration factor matrix*/

          memset( frameBuffer, BLACK, 2 * WIDTH * HEIGHT );   /* Clear screen */
          TFT_DrawString(10, 100, "The touch screen is" );
          TFT_DrawString(30, 130, "now calibrated !" );
        }

        /* Display help screen */
        else if ( buttons & BC_UIF_PB3 )
        {
          displayHelpScreen();
          while ( readButtons() & BC_UIF_PB3 ) {}
        }

      } while ( ( buttons & EXIT_LOOP ) == 0 );
    }
    else
    {
      BSP_LedsSet(0x8001);
      delayMs(200);
    }
  }
}

/**************************************************************************//**
 * @brief Delay function, delays a number of cpu clock cycles (ticks)
 * @note  This function relies on:
 *        1. BSP_TraceProfilerSetup() which sets
 *           CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk
 *        2. DWT_CTRL.CYCCNTENA is set.
 * @param[in] usec Delay in ticks
 *****************************************************************************/
static void delayTicks( uint64_t ticks )
{
  uint32_t startTime;
  volatile uint32_t now;

  while ( ticks > 2000000000 )
  {
    startTime = DWT_CYCCNT;
    do
    {
      now = DWT_CYCCNT;
    } while ( ( now - startTime ) < 2000000000 );
    ticks -= 2000000000;
  }

  if ( ticks )
  {
    startTime = DWT_CYCCNT;
    do
    {
      now = DWT_CYCCNT;
    } while ( ( now - startTime ) < ticks );
  }
}

/**************************************************************************//**
 * @brief Millisecond delay function
 * @param[in] usec Delay in milliseconds
 *****************************************************************************/
static void delayMs( uint32_t msec )
{
  uint64_t totalTicks;

  totalTicks = (((uint64_t)CMU_ClockFreqGet(cmuClock_CORE)*msec)+500)/1000;

  delayTicks( totalTicks );
}

/**************************************************************************//**
 * @brief Microsecond delay function
 * @param[in] usec Delay in microseconds
 *****************************************************************************/
static void delayUs( uint32_t usec )
{
  uint64_t totalTicks;

  totalTicks = (((uint64_t)CMU_ClockFreqGet(cmuClock_CORE)*usec)+500000)/1000000;

  delayTicks( totalTicks );
}

/**************************************************************************//**
 * @brief Display "help" screen
 *****************************************************************************/
static void displayHelpScreen( void )
{
  memset( frameBuffer, BLACK, 2 * WIDTH * HEIGHT );         /* Clear screen */
  TFT_DrawString(120, 10, "EFM32");
  TFT_DrawString(10, 10 + font16x28.c_height, "Touchscreen example");
  TFT_DrawString(50, 100, "PB1: Draw");
  TFT_DrawString(50, 100 + font16x28.c_height, "PB2: Calibrate");
  TFT_DrawString(50, 100 + (2*font16x28.c_height), "PB3: Help");
}

/**************************************************************************//**
 * @brief Draw a 9 pixel cross
 * @param[in] xpos Horizontal position of cross center
 * @param[in] ypos Vertical position of cross center
 * @param[in] color Color to use for cross
 *****************************************************************************/
static void drawCross( uint32_t xpos, uint32_t ypos, uint16_t color )
{
  drawPixel( xpos-2, ypos,   color );
  drawPixel( xpos-1, ypos,   color );
  drawPixel( xpos,   ypos,   color );
  drawPixel( xpos+1, ypos,   color );
  drawPixel( xpos+2, ypos,   color );
  drawPixel( xpos,   ypos-2, color );
  drawPixel( xpos,   ypos-1, color );
  drawPixel( xpos,   ypos+1, color );
  drawPixel( xpos,   ypos+2, color );
}

/**************************************************************************//**
 * @brief Draw a single pixel
 * @param[in] xpos Horizontal position
 * @param[in] ypos Vertical position
 * @param[in] color Color to use for pixel
 *****************************************************************************/
__STATIC_INLINE void drawPixel( uint32_t xpos, uint32_t ypos, uint16_t color )
{
  uint32_t pixAddr;

  pixAddr = (ypos * WIDTH) + xpos;
  if ( pixAddr < ( 2 * WIDTH * HEIGHT ) )
    frameBuffer[ pixAddr ] = color;
}

/**************************************************************************//**
 * @brief  Read touchscreen X or Y position (12bit resolution)
 * @param[in] ch X (ADC_X) or Y (ADC_Y) touchscreen readout
 * @return Touchscreen X or Y position
 *****************************************************************************/
static uint32_t getTouchChSample12bit( ADC_SingleInput_TypeDef ch )
{
  int i;
  uint32_t value, min, max, acc;

  if ( ch == ADC_X )
  {
    GPIO_PinModeSet(LCD_TOUCH_Y1_PORT, LCD_TOUCH_Y1_PIN, gpioModePushPull, 0);
    GPIO_PinModeSet(LCD_TOUCH_Y2_PORT, LCD_TOUCH_Y2_PIN, gpioModePushPull, 1);
    GPIO_PinModeSet(LCD_TOUCH_X1_PORT, LCD_TOUCH_X1_PIN, gpioModeInput, 0);
    GPIO_PinModeSet(LCD_TOUCH_X2_PORT, LCD_TOUCH_X2_PIN, gpioModeInput, 0);
  }
  else
  {
    GPIO_PinModeSet(LCD_TOUCH_X1_PORT, LCD_TOUCH_X1_PIN, gpioModePushPull, 1);
    GPIO_PinModeSet(LCD_TOUCH_X2_PORT, LCD_TOUCH_X2_PIN, gpioModePushPull, 0);
    GPIO_PinModeSet(LCD_TOUCH_Y1_PORT, LCD_TOUCH_Y1_PIN, gpioModeInput, 0);
    GPIO_PinModeSet(LCD_TOUCH_Y2_PORT, LCD_TOUCH_Y2_PIN, gpioModeInput, 0);
  }

  sInit.input = ch;
  ADC_InitSingle(ADC0, &sInit);

  delayUs( 10 );
  acc = 0;
  max = 0;
  min = 4096;
  for ( i=0; i<5; i++ )
  {
    value = readAdc();
    acc += value;
    min = SL_MIN( min, value );
    max = SL_MAX( max, value );
  }
  /* Throw away largest and smallest sample */
  acc = acc - min - max;
  /* Average */
  acc = acc / 3;

  if ( ch == ADC_X )
  {
    GPIO_PinModeSet(LCD_TOUCH_Y1_PORT, LCD_TOUCH_Y1_PIN, gpioModeInput, 0);
    GPIO_PinModeSet(LCD_TOUCH_Y2_PORT, LCD_TOUCH_Y2_PIN, gpioModeInput, 0);
  }
  else
  {
    GPIO_PinModeSet(LCD_TOUCH_X1_PORT, LCD_TOUCH_X1_PIN, gpioModeInput, 0);
    GPIO_PinModeSet(LCD_TOUCH_X2_PORT, LCD_TOUCH_X2_PIN, gpioModeInput, 0);
  }

  return acc;
}

/**************************************************************************//**
 * @brief  Read normalized touchscreen XY position (10bit resolution)
 * @return Touchscreen position
 *****************************************************************************/
static POINT getTouchSample( void )
{
  POINT sample, normalizedSample;

  sample = getTouchSample10bit();
  getDisplayPoint( &normalizedSample, &sample, &calibFactors );
  return normalizedSample;
}

/**************************************************************************//**
 * @brief  Read touchscreen XY position with 10bit ADC resolution
 * @return Touchscreen position
 *****************************************************************************/
static POINT getTouchSample10bit( void )
{
  POINT sample;

  sample = getTouchSample12bit();

  sample.x = ( sample.x + 2 ) / 4;
  sample.y = ( sample.y + 2 ) / 4;
  return sample;
}

/**************************************************************************//**
 * @brief  Read touchscreen XY position with full ADC resolution
 * @return Touchscreen position
 *****************************************************************************/
static POINT getTouchSample12bit( void )
{
  POINT sample;

  sample.x = getTouchChSample12bit( ADC_X );
  sample.y = getTouchChSample12bit( ADC_Y );
  return sample;
}

/**************************************************************************//**
 * @brief  Wait for touchscreen tap
 * @return Touchscreen tap position
 *****************************************************************************/
static POINT getTouchTapSample10bit( void )
{
  POINT sample = { 0, 0 };

  for (;;)
  {
    if ( touched() )
    {
      sample = getTouchSample10bit();

      while ( touched() )
      {
        delayMs( 25 );
      }
      break;
    }
    delayMs( 25 );
  }

  return sample;
}

/**************************************************************************//**
 * @brief  Do one ADC conversion
 * @return ADC conversion result
 *****************************************************************************/
static uint32_t readAdc( void )
{
  ADC_Start(ADC0, adcStartSingle);
  while ( ( ADC0->STATUS & ADC_STATUS_SINGLEDV ) == 0 ){}
  return ADC_DataSingleGet(ADC0);
}

/**************************************************************************//**
 * @brief  Get status of PB1,...,PB4 buttons and the AEM state
 * @return Bitmap button status, AEM state in bit position EXIT_LOOP
 *****************************************************************************/
static uint32_t readButtons( void )
{
  if ( BSP_RegisterRead(&BC_REGISTER->UIF_AEM) == BC_UIF_AEM_BC )
    return EXIT_LOOP | BSP_PushButtonsGet();

  return BSP_PushButtonsGet();
}

/**************************************************************************//**
 * @brief  Check if touch screen is "touched"
 * @return true if screen touched
 *****************************************************************************/
static bool touched( void )
{
  uint32_t adcValue;

  GPIO_PinModeSet(LCD_TOUCH_X1_PORT, LCD_TOUCH_X1_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(LCD_TOUCH_X2_PORT, LCD_TOUCH_X2_PIN, gpioModePushPull, 1);
  GPIO_PinModeSet(LCD_TOUCH_Y1_PORT, LCD_TOUCH_Y1_PIN, gpioModeInput, 0);

  GPIO_PinModeSet(LCD_TOUCH_Y2_PORT, LCD_TOUCH_Y2_PIN, gpioModePushPull, 1);
  delayUs( 10 );
  GPIO_PinModeSet(LCD_TOUCH_Y2_PORT, LCD_TOUCH_Y2_PIN, gpioModeInputPull, 1);
  delayUs( 10 );

  sInit.input = ADC_Y;
  ADC_InitSingle(ADC0, &sInit);

  adcValue = readAdc();

  GPIO_PinModeSet(LCD_TOUCH_X1_PORT, LCD_TOUCH_X1_PIN, gpioModeInput, 0);
  GPIO_PinModeSet(LCD_TOUCH_X2_PORT, LCD_TOUCH_X2_PIN, gpioModeInput, 0);
  GPIO_PinModeSet(LCD_TOUCH_Y2_PORT, LCD_TOUCH_Y2_PIN, gpioModeInput, 0);

  return ( adcValue < 3800 ) ? true : false;
}

/**************************************************************************//**
 * @brief Draw single character into frame buffer
 * @param[in] x Horizontal start position into frame buffer in pixels
 * @param[in] y Vertical start position into frame buffer in pixels
 * @param[in] c Character to draw
 *****************************************************************************/
void TFT_DrawFont(int x, int y, int c)
{
  uint16_t     *frameBuffer = (uint16_t *) EBI_BankAddress(EBI_BANK2);
  unsigned int i, j;
  int          fontOffset = c - 0x20;
  uint16_t     color;

  frameBuffer += (x);
  frameBuffer += (y * WIDTH);

  /* Draw a single character, iterate over lines and pixels */
  for (j = 0; j < font16x28.c_height; j++)
  {
    for (i = 0; i < font16x28.c_width; i++)
    {
      color          = font16x28.data[j * font16x28.width + fontOffset * font16x28.c_width + i];
      *frameBuffer++ = color;
    }
    frameBuffer += (WIDTH - font16x28.c_width);
  }
}

/**************************************************************************//**
 * @brief Draw single character into frame buffer
 * @param[in] x Horizontal start position into frame buffer in pixels
 * @param[in] y Vertical start position into frame buffer in pixels
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
