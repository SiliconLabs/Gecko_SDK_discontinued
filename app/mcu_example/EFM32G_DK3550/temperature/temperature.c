/**************************************************************************//**
 * @file
 * @brief I2C Temperature example for EFM32G_DK3550
 * @details
 *   Show temperature using sensor on kit. Display results on TFT-LCD,
 *   remember to enable by pressing AEM-button to KEYS: EFM.
 * @par Usage
 * @li Joystick Push toggles Celsius/Fahrenheit display mode.
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

#include <stdlib.h>
#include <stdio.h>
#include "em_device.h"
#include "em_emu.h"
#include "em_dbg.h"
#include "em_gpio.h"
#include "bsp.h"
#include "bsp_trace.h"
#include "rtcdriver.h"
#include "i2cspm.h"
#include "tempsens.h"
#include "retargettft.h"

/** Flag used to indicate if displaying in Celsius or Fahrenheit */
static int showFahrenheit;

/** Timer used for bringing the system back to EM0. */
static RTCDRV_TimerID_t xTimerForWakeUp;

/* Local prototypes */
void temperatureIRQInit(void);
void temperatureUpdate(TEMPSENS_Temp_TypeDef *temp);

/**************************************************************************//**
 * @brief GPIO Interrupt handler
 *****************************************************************************/
void GPIO_EVEN_IRQHandler(void)
{
  uint16_t joystick;

  /* Clear GPIO interrupt first, then the BSP interrupts. */
  GPIO_IntClear(1 << BSP_GPIO_INT_PIN);
  BSP_InterruptFlagsClear(BC_INTEN_JOYSTICK);

  /* LEDs on to indicate joystick used */
  BSP_LedsSet(0xffff);

  /* Read and store joystick activity - wait for key release */
  joystick = BSP_JoystickGet();
  while (BSP_JoystickGet()) ;

  /* LEDs off to indicate joystick release */
  BSP_LedsSet(0x0000);

  /* Push toggles celsius/fahrenheit */
  if (joystick & BC_UIF_JOYSTICK_CENTER)
  {
    showFahrenheit ^= 1;
  }
}


/**************************************************************************//**
 * @brief Initialize GPIO interrupt for joystick
 *****************************************************************************/
void temperatureIRQInit(void)
{
  BSP_InterruptDisable(0xffff);
  BSP_InterruptFlagsClear(0xffff);

  /* Configure interrupt pin as input with pull-up */
  GPIO_PinModeSet(BSP_GPIO_INT_PORT, BSP_GPIO_INT_PIN, gpioModeInputPull, 1);

  /* Set falling edge interrupt and clear/enable it */
  GPIO_IntConfig(BSP_GPIO_INT_PORT, BSP_GPIO_INT_PIN, false, true, true);

  NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
  NVIC_EnableIRQ(GPIO_EVEN_IRQn);

  BSP_InterruptEnable(BC_INTEN_JOYSTICK);
}


/**************************************************************************//**
 * @brief Update TFT with temperature
 * @param[in] temp Temperature to display.
 *****************************************************************************/
void temperatureUpdate(TEMPSENS_Temp_TypeDef *temp)
{
  TEMPSENS_Temp_TypeDef dtemp;

  /* Work with local copy in case conversion to Fahrenheit is required */
  dtemp = *temp;

  if (showFahrenheit)
  {
    TEMPSENS_Celsius2Fahrenheit(&dtemp);
  }

  /* Round temperature to nearest 0.5 */
  if (dtemp.f >= 0)
  {
    dtemp.i += (dtemp.f + 2500) / 10000;
    dtemp.f  = (((dtemp.f + 2500) % 10000) / 5000) * 5000;
  }
  else
  {
    dtemp.i += (dtemp.f - 2500) / 10000;
    dtemp.f  = (((dtemp.f - 2500) % 10000) / 5000) * 5000;
  }

  if ((dtemp.i < 0) || (dtemp.f < 0))
  {
    putchar('-');
  }
  else
  {
    putchar('+');
  }
  printf("%d.%d %c\n", abs(dtemp.i), (int)(abs(dtemp.f) / 1000),
         showFahrenheit ? 'F' : 'C');
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  I2CSPM_Init_TypeDef   i2cInit = I2CSPM_INIT_DEFAULT;
  TEMPSENS_Temp_TypeDef temp;
  /* Define previous temp to invalid, just to ensure update first time */
  TEMPSENS_Temp_TypeDef prevTemp = {1000, 0};
  int                   prevShowFahrenheit = showFahrenheit;

  /* Initialize DK board register access */
  BSP_Init(BSP_INIT_DEFAULT);

  /* If first word of user data page is non-zero, enable eA Profiler trace */
  BSP_TraceProfilerSetup();

  /* Initialize RTC timer. */
  RTCDRV_Init();
  RTCDRV_AllocateTimer( &xTimerForWakeUp);

  /* Initialize the TFT stdio retarget module. */
  RETARGET_TftInit();

  printf("\nEFM32 I2C temperature sensor example\n\n");

  /* Enable board control interrupts */
  temperatureIRQInit();

#if !defined( BSP_STK )
  BSP_PeripheralAccess(BSP_I2C, true);
#endif

  /* Initialize I2C driver, using standard rate. Devices on DK itself */
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
      printf("ERROR\n");
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
      temperatureUpdate(&temp);
    }
    prevTemp           = temp;
    prevShowFahrenheit = showFahrenheit;

    /* Read every 2 seconds which is more than it takes worstcase to */
    /* finish measurement inside sensor. */
    RTCDRV_StartTimer( xTimerForWakeUp, rtcdrvTimerTypeOneshot, 2000, NULL, NULL);
    EMU_EnterEM2(true);
  }
}
