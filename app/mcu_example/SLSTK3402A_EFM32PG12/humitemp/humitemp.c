/**************************************************************************//**
 * @file
 * @brief Relative humidity and temperature sensor demo for SLSTK3402A
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

#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "cpt112s_config.h"
#include "i2cspm.h"
#include "si7013.h"
#include "rtcdriver.h"
#include "graphics.h"
#include "em_adc.h"
#include "bspconfig.h"

/**************************************************************************//**
 * Local defines
 *****************************************************************************/

/** Time (in ms) between periodic updates of the measurements. */
#define PERIODIC_UPDATE_MS      2000

/**************************************************************************//**
 * Local variables
 *****************************************************************************/
/* RTC callback parameters. */
static void (*rtcCallback)(void*) = 0;
static void * rtcCallbackArg = 0;

/** This flag tracks if we need to update the display
 *  (animations or measurements). */
static volatile bool updateDisplay = true;
/** This flag tracks if we need to perform a new
 *  measurement. */
static volatile bool updateMeasurement = true;

/** Timer used for periodic update of the measurements. */
RTCDRV_TimerID_t periodicUpdateTimerId;


/**************************************************************************//**
 * Local prototypes
 *****************************************************************************/
static void gpioSetup(void);
static void periodicUpdateCallback(RTCDRV_TimerID_t id, void *user);
static void memLcdCallback(RTCDRV_TimerID_t id, void *user);

/**************************************************************************//**
 * @brief  Helper function to perform data measurements.
 *****************************************************************************/
static int performMeasurements(I2C_TypeDef *i2c, uint32_t *rhData, int32_t *tData)
{
  Si7013_MeasureRHAndTemp(i2c, SI7021_ADDR, rhData, tData);
  return 0;
}


/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  I2CSPM_Init_TypeDef i2cInit = I2CSPM_INIT_DEFAULT;
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_STK_DEFAULT;
  CMU_HFXOInit_TypeDef hfxoInit = CMU_HFXOINIT_DEFAULT;
  uint32_t         rhData = 0;
  bool             si7013_status;
  int32_t          tempData = 0;

  /* Chip errata */
  CHIP_Init();

  /* Initalize hardware */
  EMU_DCDCInit(&dcdcInit);
  CMU_HFXOInit(&hfxoInit);

  /* Switch HFCLK to HFXO and disable HFRCO */
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);
  CMU_OscillatorEnable(cmuOsc_HFRCO, false, false);

  /* Initalize other peripherals and drivers */
  gpioSetup();
  RTCDRV_Init();
  GRAPHICS_Init();
  I2CSPM_Init(&i2cInit);

  /* Get initial sensor status */
  si7013_status = Si7013_Detect(i2cInit.port, SI7021_ADDR, 0);

  /* Set up periodic update of the display. */
  RTCDRV_AllocateTimer(&periodicUpdateTimerId);
  RTCDRV_StartTimer(periodicUpdateTimerId, rtcdrvTimerTypePeriodic,
                    PERIODIC_UPDATE_MS, periodicUpdateCallback, 0);

  GRAPHICS_ShowStatus(si7013_status);
  EMU_EnterEM2(false);

  updateDisplay = true;

  while (true)
  {
    if (updateMeasurement)
    {
      performMeasurements(i2cInit.port, &rhData, &tempData);
      updateMeasurement = false;
    }

    if (updateDisplay)
    {
      updateDisplay = false;
      GRAPHICS_Draw(tempData, rhData);
    }
    EMU_EnterEM2(false);
  }
}


/**************************************************************************//**
* @brief Setup GPIO, enable sensor isolation switch
*****************************************************************************/
static void gpioSetup(void)
{
  /* Enable GPIO clock */
  CMU_ClockEnable(cmuClock_GPIO, true);

  /* Enable Si7021 sensor isolation switch */
  GPIO_PinModeSet(CS0_SENSOR_EN_PORT, CS0_SENSOR_EN_PIN, gpioModePushPull, 1);
}


/**************************************************************************//**
 * @brief   The actual callback for Memory LCD toggling
 * @param[in] id
 *   The id of the RTC timer (not used)
 *****************************************************************************/
static void memLcdCallback(RTCDRV_TimerID_t id, void *user)
{
  (void) id;
  (void) user;
  rtcCallback(rtcCallbackArg);
}


/**************************************************************************//**
 * @brief   Register a callback function at the given frequency.
 *
 * @param[in] pFunction  Pointer to function that should be called at the
 *                       given frequency.
 * @param[in] argument   Argument to be given to the function.
 * @param[in] frequency  Frequency at which to call function at.
 *
 * @return  0 for successful or
 *         -1 if the requested frequency does not match the RTCC frequency.
 *****************************************************************************/
int rtccIntCallbackRegister(void (*pFunction)(void*),
                           void* argument,
                           unsigned int frequency)
{
  RTCDRV_TimerID_t timerId;
  rtcCallback    = pFunction;
  rtcCallbackArg = argument;

  RTCDRV_AllocateTimer(&timerId);

  RTCDRV_StartTimer(timerId, rtcdrvTimerTypePeriodic, 1000 / frequency,
                    memLcdCallback, 0);

  return 0;
}


/**************************************************************************//**
 * @brief Callback used to count between measurement updates
 *****************************************************************************/
static void periodicUpdateCallback(RTCDRV_TimerID_t id, void *user)
{
  (void) id;
  (void) user;
  updateDisplay = true;
  updateMeasurement = true;
}
