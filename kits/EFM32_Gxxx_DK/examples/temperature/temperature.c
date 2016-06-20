/**************************************************************************//**
 * @file
 * @brief Temperature example for EFM32_G8xx_DK
 * @details
 *   Show temperature using sensor on DVK.
 *
 * @par Usage
 * @li Joystick Push toggles Celsius/Fahrenheit display mode.
 *
 * @note
 *   This example requires BSP version 1.0.6 or later.
 *
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
#include <stdlib.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_emu.h"
#include "em_dbg.h"
#include "em_gpio.h"
#include "bsp.h"
#include "bsp_trace.h"
#include "segmentlcd.h"
#include "rtcdriver.h"
#include "i2cspm.h"
#include "tempsens.h"

/** Flag used to indicate if displaying in Celsius or Fahrenheit */
static int showFahrenheit;

/** Timer used for bringing the system back to EM0. */
static RTCDRV_TimerID_t xTimerForWakeUp;

/* Local prototypes */
void temperatureIRQInit(void);
void temperatureUpdateLCD(TEMPSENS_Temp_TypeDef *temp);

/**************************************************************************//**
 * @brief GPIO Interrupt handler
 *****************************************************************************/
void GPIO_EVEN_IRQHandler(void)
{
  uint16_t joystick;

  /* Clear interrupt */
  GPIO_IntClear(1 << BSP_GPIO_INT_PIN);
  BSP_InterruptFlagsClear(BC_INTEN_JOYSTICK);

  /* LEDs on to indicate joystick used */
  BSP_LedsSet(0xffff);

  /* Read and store joystick activity - wait for key release */
  joystick = BSP_JoystickGet();
  while (BSP_JoystickGet());

  /* LEDs off to indicate joystick release */
  BSP_LedsSet(0x0000);

  /* Push toggles celsius/fahrenheit */
  if (joystick & BC_JOYSTICK_CENTER)
  {
    showFahrenheit ^= 1;
  }
}


/**************************************************************************//**
 * @brief Initialize GPIO interrupt for joystick
 *****************************************************************************/
void temperatureIRQInit(void)
{
  /* Configure interrupt pin as input with pull-up */
  GPIO_PinModeSet(BSP_GPIO_INT_PORT, BSP_GPIO_INT_PIN, gpioModeInputPull, 1);

  /* Set falling edge interrupt and clear/enable it */
  GPIO_IntConfig(BSP_GPIO_INT_PORT, BSP_GPIO_INT_PIN, false, true, true);

  NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
  NVIC_EnableIRQ(GPIO_EVEN_IRQn);
}


/**************************************************************************//**
 * @brief Update LCD with temperature
 * @param[in] temp Temperature to display.
 *****************************************************************************/
void temperatureUpdateLCD(TEMPSENS_Temp_TypeDef *temp)
{
  char text[8];
  TEMPSENS_Temp_TypeDef dtemp;

  /* Work with local copy in case conversion to Fahrenheit is required */
  dtemp = *temp;

  /* Show Gecko if debugger is attached. Energy modes do not behave as */
  /* expected when using the debugger. */
  if (DBG_Connected())
  {
    SegmentLCD_Symbol(LCD_SYMBOL_GECKO, 1);
  }
  else
  {
    SegmentLCD_Symbol(LCD_SYMBOL_GECKO, 0);
  }

  memset(text, ' ', sizeof(text) - 1);
  text[sizeof(text) - 1] = 0;

  if (showFahrenheit)
  {
    text[5] = 'F';
    TEMPSENS_Celsius2Fahrenheit(&dtemp);
  }
  else
  {
    text[5] = 'C';
  }

  /* Round temperature to nearest 0.5 */
  if (dtemp.f >= 0)
  {
    dtemp.i += (dtemp.f + 2500) / 10000;
    dtemp.f = (((dtemp.f + 2500) % 10000) / 5000) * 5000;
  }
  else
  {
    dtemp.i += (dtemp.f - 2500) / 10000;
    dtemp.f = (((dtemp.f - 2500) % 10000) / 5000) * 5000;
  }

  /* 100s */
  if (abs(dtemp.i) >= 100)
    text[0] = '0' + (abs(dtemp.i) / 100);

  /* 10s */
  if (abs(dtemp.i) >= 10)
    text[1] = '0' + ((abs(dtemp.i) % 100) / 10);

  /* 1s */
  text[2] = '0' + (abs(dtemp.i) % 10);

  /* 0.1s */
  text[3] = '0' + (abs(dtemp.f) / 1000);

  SegmentLCD_Write(text);
  SegmentLCD_Symbol(LCD_SYMBOL_DP4, 1);

  if ((dtemp.i < 0) || (dtemp.f < 0))
  {
    SegmentLCD_Symbol(LCD_SYMBOL_MINUS, 1);
  }
  else
  {
    SegmentLCD_Symbol(LCD_SYMBOL_MINUS, 0);
  }
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  I2CSPM_Init_TypeDef i2cInit = I2CSPM_INIT_DEFAULT;
  TEMPSENS_Temp_TypeDef temp;
  /* Define previous temp to invalid, just to ensure update first time */
  TEMPSENS_Temp_TypeDef prevTemp = { 1000, 0};
  int prevShowFahrenheit = showFahrenheit;

  /* Chip revision alignment and errata fixes */
  CHIP_Init();

  /* Initialize DVK board register access */
  BSP_Init(BSP_INIT_DEFAULT);

#if !defined( BSP_STK )
  BSP_PeripheralAccess(BSP_I2C, true);
#endif

  /* If first word of user data page is non-zero, enable eA Profiler trace */
  BSP_TraceProfilerSetup();

  /* Initialize RTC timer. */
  RTCDRV_Init();
  RTCDRV_AllocateTimer( &xTimerForWakeUp);

  /* Initialize LCD controller without boost */
  SegmentLCD_Init(false);

  SegmentLCD_AllOff();

  /* Enable board control interrupts */
  BSP_InterruptDisable(0xffff);
  BSP_InterruptFlagsClear(0xffff);
  BSP_InterruptEnable(BC_INTEN_JOYSTICK);
  temperatureIRQInit();

  /* Initialize I2C driver, using standard rate. Devices on DVK itself */
  /* supports fast mode, but in case some slower devices are added on */
  /* prototype board, we use standard mode. */
  I2CSPM_Init(&i2cInit);

  /* Main loop - just read temperature and update LCD */
  while (1)
  {
    if (TEMPSENS_TemperatureGet(i2cInit.port,
                                TEMPSENS_DK_ADDR,
                                &temp) < 0)
    {
      SegmentLCD_Write("ERROR");
      /* Enter EM2, no wakeup scheduled */
      EMU_EnterEM2(true);
    }

    /* Update LCD display if any change. This is just an example of how */
    /* to save some energy, since the temperature normally is quite static. */
    /* The compare overhead is much smaller than actually updating the display. */
    if ((prevTemp.i != temp.i) ||
        (prevTemp.f != temp.f) ||
        (prevShowFahrenheit != showFahrenheit))
    {
      temperatureUpdateLCD(&temp);
    }
    prevTemp = temp;
    prevShowFahrenheit = showFahrenheit;

    /* Read every 2 seconds which is more than it takes worstcase to */
    /* finish measurement inside sensor. */
    RTCDRV_StartTimer( xTimerForWakeUp, rtcdrvTimerTypeOneshot, 2000, NULL, NULL);
    EMU_EnterEM2(true);
  }
}
