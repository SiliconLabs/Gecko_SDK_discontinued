/**************************************************************************//**
 * @file
 * @brief Temperature example for EFM32LG_DK3650
 * @details
 *   Show temperature using I2C sensor on DK.
 *
 * @par Usage
 * @li Joystick Push toggles Celsius/Fahrenheit display mode.
 *
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

/* Temperature sensor modes */
#define TEMP_SENSOR_POWERDOWN_MODE   0x63
#define TEMP_SENSOR_SAMPLE_MODE      0x62

/* Flag used to indicate if displaying in Celsius or Fahrenheit */
static volatile bool showFahrenheit;
static volatile bool sensorDataReady;
static volatile bool sensorSleepWakeup;

/** Timer used for bringing the system back to EM0. */
RTCDRV_TimerID_t xTimerForWakeUp;

/* Local prototypes */
void temperatureIRQInit(void);
void temperatureUpdate(TEMPSENS_Temp_TypeDef *temp);

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

  /* Disable and clear BSP interrupts */
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
 * @brief Format temperature and update TFT display
 * @param[in] tempFromSensor Temperature from sensor.
 *****************************************************************************/
void formatAndDisplayTemperature(TEMPSENS_Temp_TypeDef *tempFromSensor)
{
  TEMPSENS_Temp_TypeDef tempDisplay;
  static TEMPSENS_Temp_TypeDef prevTempDisplay;

  /* Round temperature to nearest 0.1 */
  tempDisplay.i = tempFromSensor->i;
  if (tempFromSensor->f >= 0)
  {
    tempDisplay.i += (tempFromSensor->f + 500) / 10000;
    tempDisplay.f  = (tempFromSensor->f + 500) % 10000;
  }
  else
  {
    tempDisplay.i += (tempFromSensor->f - 500) / 10000;
    tempDisplay.f  = (tempFromSensor->f - 500) % 10000;
  }

  if (showFahrenheit)
  {
    TEMPSENS_Celsius2Fahrenheit(&tempDisplay);
  }

  /* Divide down fractional part */
  tempDisplay.f = tempDisplay.f / 1000;

  /* Update the display only when the output changes by more than
     0.1 degrees to save energy */
  if ((prevTempDisplay.i != tempDisplay.i) ||
      (abs(prevTempDisplay.f - tempDisplay.f) > 1))
  {
    /* Set sign */
    putchar(((tempDisplay.i < 0) || (tempDisplay.f < 0)) ? '-' : '+');

    /* Output temperature */
    printf("%d.%d %c\n", abs(tempDisplay.i), abs(tempDisplay.f),
           showFahrenheit ? 'F' : 'C');

    prevTempDisplay.i = tempDisplay.i;
    prevTempDisplay.f = tempDisplay.f;
  }
}


/* Callback for temperature sensor conversion timing */
void setSensorDataReady( RTCDRV_TimerID_t id, void *user)
{
  ( void)id;
  ( void)user;
  sensorDataReady = true;
}


/* Callback for sensor sleep wakeup */
void setSensorSleepWakeup( RTCDRV_TimerID_t id, void *user)
{
  ( void)id;
  ( void)user;
  sensorSleepWakeup = true;
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  I2CSPM_Init_TypeDef i2cInit = I2CSPM_INIT_DEFAULT;
  TEMPSENS_Temp_TypeDef tempFromSensor;

  showFahrenheit = false;
  sensorDataReady = false;
  sensorSleepWakeup = true;

  /* Initialize DK board register access */
  BSP_Init(BSP_INIT_DEFAULT);

  /* If first word of user data page is non-zero, enable eA Profiler trace */
  BSP_TraceProfilerSetup();

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

  /* Initialize RTC timer. */
  RTCDRV_Init();
  RTCDRV_AllocateTimer( &xTimerForWakeUp);

  /* Main loop - just read temperature and update LCD */
  while (true)
  {
    /* Read sensor every 3 seconds. Sensor conversion time is 1.2 seconds. Ie,
       a 100 ms timing margin is added. */
    /* Power up sensor, sleep EFM32 until conversion is done, read temperature.
       If setSensorSleepWakeup is false, then the EM2 exit above was generated by a
       Fahrenheit/Celsius conversion interrupt. In that case go to
       formatAndDisplayTemperature and output the current temperature with the
       new unit. */
    if (sensorSleepWakeup)
    {
      TEMPSENS_RegisterSet(i2cInit.port, TEMPSENS_DK_ADDR, tempsensRegConfig, TEMP_SENSOR_SAMPLE_MODE);
      RTCDRV_StartTimer( xTimerForWakeUp, rtcdrvTimerTypeOneshot, 1300, setSensorDataReady, NULL);
      EMU_EnterEM2(true);
      sensorSleepWakeup = false;
    }

    if (sensorDataReady)
    {
      if (TEMPSENS_TemperatureGet(i2cInit.port,
                                  TEMPSENS_DK_ADDR,
                                  &tempFromSensor) < 0)
      {
        printf("Sensor error\n");
        /* Enter EM2 on error, no wakeup scheduled */
        EMU_EnterEM2(true);
      }
      sensorDataReady = false;
    }

    formatAndDisplayTemperature( &tempFromSensor);

    /* Power down sensor */
    TEMPSENS_RegisterSet(i2cInit.port, TEMPSENS_DK_ADDR, tempsensRegConfig, TEMP_SENSOR_POWERDOWN_MODE);
    RTCDRV_StartTimer( xTimerForWakeUp, rtcdrvTimerTypeOneshot, 1700, setSensorSleepWakeup, NULL);
    EMU_EnterEM2(true);
  }
}
