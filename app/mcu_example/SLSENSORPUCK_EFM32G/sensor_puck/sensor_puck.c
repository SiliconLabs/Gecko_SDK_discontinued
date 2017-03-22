/**************************************************************************//**
* @file
* @brief Sensor Puck demo for EFM32G
* @version 5.1.2
******************************************************************************
* @section License
* <b>Copyright 2015 Silicon Labs, Inc. http://www.silabs.com</b>
*******************************************************************************
*
* This file is licensensed under the Silabs License Agreement. See the file
* "Silabs_License_Agreement.txt" for details. Before using this software for
* any purpose, you must agree to the terms of that agreement.
*
******************************************************************************/

#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "i2cspm.h"
#include "si7013.h"
#include "heart_rate_monitor.h"
#include "rtcdriver.h"
#include "em_adc.h"
#include "sensor_puck.h"
#include "ble.h"
#include "trace.h"
#include "si114x_functions.h"
#include "si114xhrm_user_functions.h"
#include "si114x_environmental.h"

// These are for LFRCO calibration
// LF_COUNT = HF_COUNT * LFRCO     / HFXO
// LF_COUNT = 0xFFFFF  * 32.768kHz / 24MHz
#define HF_COUNT    1048575
#define LF_COUNT    1432

RTCDRV_TimerID_t periodicUpdateTimerId;
RTCDRV_TimerID_t hrmTimeoutTimerId;

// Used to keep track of what mode we are in
Meas_Mode_TypeDef measurementMode = ENVIRONMENTAL_MODE;

// Flag to indicate bluetooth data needs to be updated
bool bluetoothDataPending = false;

// Flag used to indicate ADC is finished
static volatile bool adcConversionComplete = false;
// This flag tracks if we need to perform a new measurement.
static bool          updateMeasurement = true;
// This flag tracks if we need to terminate HRM because its been running too long.
static bool          hrmTimeout = false;


// Measurement data
uint32_t       rhData       = 50000; /* milliperecent */
int32_t        tempData     = 25000; /* millidegree C */
uint16_t       uvData       = 0;     /* index */
uint16_t       alsData      = 1000;  /* lux */
uint32_t       vBat         = 3100;  /* millivolt */
HRMSpO2State_t HRMState     = HRM_STATE_IDLE;
HRMSpO2State_t SpO2State    = HRM_STATE_IDLE;
int32_t        HRMRate      = 100;
int32_t        HRMSpO2Value = 100;
uint16_t       HRMSample[BLE_SAMPLE_COUNT];
int            HRMSampleCount = 0;
char           hrmVersion[10];
static HANDLE  si114xHandle;

/**************************************************************************//**
 * Local prototypes
 *****************************************************************************/
static void gpioSetup(void);
static void periodicUpdateCallback(RTCDRV_TimerID_t id, void *user);
static void hrmTimeoutCallback(RTCDRV_TimerID_t id, void *user);
static uint32_t checkBattery(void);
static void adcInit(void);
void CalibrateLFRCO(void);

/**************************************************************************//**
 * @brief  Helper function to perform data measurements.
 *****************************************************************************/
bool performMeasurements()
{
  uint16_t ps1Data = 0;
  vBat = checkBattery();
  Si7013_ReadNoHoldRHAndTemp(I2C0, 0x80, &rhData, &tempData);
  Si7013_StartNoHoldMeasureRHAndTemp(I2C0, 0x80);
  Si114x_MeasureEnvironmental(si114xHandle, &uvData, &ps1Data, &alsData);
  if (ps1Data > PS1_SKIN_CONTACT_THRESHOLD)
  {
	if (hrmTimeout == false)
	{
      GPIO_PinOutSet(GREEN_LED_PORT, GREEN_LED_PIN);
      GPIO_PinOutSet(RED_LED_PORT, RED_LED_PIN);
      Si114x_MeasureDarkOffset(si114xHandle);
	}
    return true;
  }
  return false;
}

/**************************************************************************//**
 * @brief Callback used to count between measurement updates
 *****************************************************************************/
void periodicUpdateCallback(RTCDRV_TimerID_t id, void *user)
{
  (void) user;
  (void) id;
  updateMeasurement = true;
}

/**************************************************************************//**
 * @brief Callback used to trigger biometric mode timeout
 *****************************************************************************/
void hrmTimeoutCallback(RTCDRV_TimerID_t id, void *user)
{
  (void) user;
  (void) id;
  hrmTimeout = true;
}

/**************************************************************************//**
 * @brief Callback used to provide sample data to Bluetooth
 *****************************************************************************/
void hrmSampleCallback(uint16_t sample)
{
  HRMSample[HRMSampleCount] = sample;
  if (++HRMSampleCount == BLE_SAMPLE_COUNT)
  {
    HRMSampleCount       = 0;
    bluetoothDataPending = true;
  }
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  I2CSPM_Init_TypeDef i2cInit = {
		    I2C0,                       /* Use I2C instance 0 */                        \
		    gpioPortD,                  /* SCL port */                                  \
		    7,                          /* SCL pin */                                   \
		    gpioPortD,                  /* SDA port */                                  \
		    6,                          /* SDA pin */                                   \
		    1,                          /* Location */                                  \
		    0,                          /* Use currently configured reference clock */  \
		    250000,      /* Set to standard rate  */                     \
		    i2cClockHLRStandard,        /* Set to use 4:4 low/high duty cycle */        \
		  };
  Si114xPortConfig_t  si114xI2C;
  bool                si7013_status = true, si114x_status = true;
  bool                skinContact   = false;
  si114xI2C.i2cAddress = SI114X_I2C_ADDR;
  si114xI2C.i2cPort    = I2C0;
  si114xI2C.irqPin     = SI114X_IRQ_PIN;
  si114xI2C.irqPort    = SI114X_IRQ_PORT;

  /* Chip errata */
  CHIP_Init();

  TraceInit();


  CMU_OscillatorEnable(cmuOsc_HFXO, true, true);
  /* Enable HFXO as the main clock */
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);
  CMU_ClockDivSet(cmuClock_HF, cmuClkDiv_2);

  /* Misc initalizations. */
  gpioSetup();
  adcInit();

  /* Misc timers. */
  RTCDRV_Init();

  /* Allow time for BT module to initialize */
  RTCDRV_Delay(100);

  /* Calibrate the LF RC oscillator */
  CalibrateLFRCO();

  /* Initialize Bluetooth */
  BLE_Init();

  /* If BLE is waiting for OTA FW update, then turn on the green LED */
  if (BLE_Update_Mode)
    GPIO_PinOutClear(GREEN_LED_PORT, GREEN_LED_PIN);
  else
    GPIO_PinOutSet(GREEN_LED_PORT, GREEN_LED_PIN);

  RTCDRV_AllocateTimer(&periodicUpdateTimerId);
  RTCDRV_AllocateTimer(&hrmTimeoutTimerId);



  /* Initialize I2C driver, using standard rate. */
  I2CSPM_Init(&i2cInit);
  HeartRateMonitor_GetVersion(hrmVersion);
  si7013_status = Si7013_Detect(I2C0, 0x80, 0);
  /*check for Si114x*/
  if (Si114xInit(&si114xI2C, 0, &si114xHandle) < 0)
  {
    si114x_status = false;
  }
  else
  {
    si114x_status = true;
  }

  if ((si7013_status == false) || (si114x_status == false))
  {
    HRMState = HRM_STATE_ERROR;
    while (1)
    {
      EMU_EnterEM2(true);
    }
  }
  HeartRateMonitor_Init(&si114xI2C);
  Si114x_ConfigureEnvironmental(si114xHandle);

  RTCDRV_StartTimer(periodicUpdateTimerId, rtcdrvTimerTypePeriodic,
                      PERIODIC_UPDATE_MS, periodicUpdateCallback, NULL);
  Si7013_StartNoHoldMeasureRHAndTemp(I2C0, 0x80);
  /* Infinite loop */
  while (1)
  {
    if (updateMeasurement)
    {
      if (measurementMode == ENVIRONMENTAL_MODE)
      {
        GPIO_PinOutToggle(GREEN_LED_PORT, GREEN_LED_PIN);
        skinContact = performMeasurements();
        GPIO_PinOutToggle(GREEN_LED_PORT, GREEN_LED_PIN);
        if (skinContact && (hrmTimeout == false) && !BLE_Update_Mode)
        {
          measurementMode = BIOMETRIC_MODE;
          BLE_OnMeasurementModeChange();
          GPIO_PinOutClear(gpioPortC, 0);
          RTCDRV_StartTimer(hrmTimeoutTimerId, rtcdrvTimerTypeOneshot,
                            HRM_TIMEOUT_MS, hrmTimeoutCallback, NULL);
          HeartRateMonitor_Init(&si114xI2C);
          Si114x_ConfigureHRM(si114xHandle);
        }
        else
        {
            bluetoothDataPending = true;
        }
        if (skinContact == false)
        {
          hrmTimeout = false;
        }
      }
      updateMeasurement = false;
    }

    if (measurementMode == BIOMETRIC_MODE)
    {
      if ((HeartRateMonitor_Loop(false, skinContact) == false) || hrmTimeout)
      {
        HRMState        = HRM_STATE_IDLE;
        measurementMode = ENVIRONMENTAL_MODE;
        BLE_OnMeasurementModeChange();
        GPIO_PinOutSet(GREEN_LED_PORT, GREEN_LED_PIN);
        GPIO_PinOutSet(RED_LED_PORT, RED_LED_PIN);
        RTCDRV_StopTimer(hrmTimeoutTimerId);
        if (hrmTimeout)
        {
          HeartRateMonitor_Loop(true, false);
        }
        HeartRateMonitor_Init(&si114xI2C);
        Si114x_ConfigureEnvironmental(si114xHandle);
      }
      skinContact = false;
    }

    /* Update Bluetooth */
    BLE_Update();
    if (measurementMode == ENVIRONMENTAL_MODE)
    {
      /* If BLE is waiting for OTA FW update, then turn on the green LED */
      if (BLE_Update_Mode)
      {
        GPIO_PinOutClear(GREEN_LED_PORT, GREEN_LED_PIN);
      }
      else
      {
        GPIO_PinOutSet(GREEN_LED_PORT, GREEN_LED_PIN);

        /* Enter deep sleep and wait for interupts */
        if (!BLE_Chg_Pending)
          EMU_EnterEM2(true);
      }
    }
    else if (HeartRateMonitor_SamplesPending() == false)
    {
      if (!BLE_Chg_Pending)
        EMU_EnterEM1();
    }
  }
}

/**************************************************************************//**
 * @brief This function is called whenever we want to measure the supply v.
 *        It is reponsible for starting the ADC and reading the result.
 *****************************************************************************/
static uint32_t checkBattery(void)
{
  uint32_t vData;
  /* Sample ADC */
  adcConversionComplete = false;
  ADC_Start(ADC0, adcStartSingle);
  while (!adcConversionComplete)
  {
    if (!BLE_Update_Mode)
    {
      EMU_EnterEM1();
    }
  }
  vData = ADC_DataSingleGet(ADC0);
  return vData;
}

/**************************************************************************//**
 * @brief ADC Interrupt handler (ADC0)
 *****************************************************************************/
void ADC0_IRQHandler(void)
{
  uint32_t flags;

  /* Clear interrupt flags */
  flags = ADC_IntGet(ADC0);
  ADC_IntClear(ADC0, flags);

  adcConversionComplete = true;
}

/**************************************************************************//**
 * @brief ADC Initialization
 *****************************************************************************/
static void adcInit(void)
{
  ADC_Init_TypeDef       init       = ADC_INIT_DEFAULT;
  ADC_InitSingle_TypeDef initSingle = ADC_INITSINGLE_DEFAULT;

  /* Enable ADC clock */
  CMU_ClockEnable(cmuClock_ADC0, true);

  /* Initiate ADC peripheral */
  ADC_Init(ADC0, &init);

  /* Setup single conversions for internal VDD/3 */
  initSingle.acqTime = adcAcqTime16;
  initSingle.input   = adcSingleInpVDDDiv3;
  ADC_InitSingle(ADC0, &initSingle);

  /* Manually set some calibration values */
  ADC0->CAL = (0x7C << _ADC_CAL_SINGLEOFFSET_SHIFT) | (0x1F << _ADC_CAL_SINGLEGAIN_SHIFT);

  /* Enable interrupt on completed conversion */
  ADC_IntEnable(ADC0, ADC_IEN_SINGLE);
  NVIC_ClearPendingIRQ(ADC0_IRQn);
  NVIC_EnableIRQ(ADC0_IRQn);
}

/**************************************************************************//**
 * @brief Setup GPIO interrupt for pushbuttons.
 *****************************************************************************/
static void gpioSetup(void)
{
  /* Enable GPIO clock. */
  CMU_ClockEnable(cmuClock_GPIO, true);

  NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
  NVIC_EnableIRQ(GPIO_ODD_IRQn);

  NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
  NVIC_EnableIRQ(GPIO_EVEN_IRQn);

  /* Configure PC14 as input and enable interrupt - si114x interrupt. */
  /* Interrupt is active low */
  GPIO_PinModeSet(SI114X_IRQ_PORT, SI114X_IRQ_PIN, gpioModeInputPull, 1);
  GPIO_IntConfig(SI114X_IRQ_PORT, SI114X_IRQ_PIN, false, true, true);

  /* Enable GPIO for LEDs */
  GPIO_PinModeSet(gpioPortC,                 /* GPIO port */
                  0,                         /* GPIO port number */
                  gpioModePushPull,          /* Pin mode is set to push pull */
                  1);                        /* High idle state */

  GPIO_PinModeSet(gpioPortC,                 /* GPIO port */
                  1,                         /* GPIO port number */
                  gpioModePushPull,          /* Pin mode is set to push pull */
                  1);                        /* High idle state */
  GPIO_PinOutClear(gpioPortC, 0);
  NVIC_SetPriority(GPIO_EVEN_IRQn,1);
}

/**************************************************************************//**
 * @brief GPIO Interrupt handler (si1147)
 *****************************************************************************/
void GPIO_EVEN_IRQHandler(void)
{
  uint32_t flags;
  flags = GPIO_IntGet();
  /* Acknowledge interrupt */
  if (flags & (1 << SI114X_IRQ_PIN))
  {
    /* Si114x IRQ line */
    HeartRateMonitor_Interrupt();
  }
  GPIO_IntClear(flags);
}

/**************************************************************************//**
 * @brief GPIO Interrupt handler (BLE)
 *****************************************************************************/
void GPIO_ODD_IRQHandler(void)
{
  uint32_t flags;
  flags = GPIO_IntGet();


  if (flags & (1 << BLE_HOST_WAKEUP_PIN))
  {
    BLE_OnHostWakeup();
  }
  GPIO_IntClear(flags);
}


/************************************************************
*	@brief  SYSTICK interrupt handler
************************************************************/
void SysTick_Handler(void)
{
  HeartRateMonitor_TimerEventHandler();
}

/**************************************************************************//**
 * @brief CalibrateLFRC0
 *****************************************************************************/
void CalibrateLFRCO()
{
  uint32_t Count;
  uint32_t PreviousCount;
  uint8_t  Tuning;

  /* Get the initial count and tuning from the LFRCO */
  Count  = CMU_Calibrate(HF_COUNT, cmuOsc_LFRCO);
  Tuning = CMU_OscillatorTuningGet(cmuOsc_LFRCO);

  /* If the inital count is less than the desired count */
  if (Count < LF_COUNT)
  {
    /* While the current count is less than the desired count */
    while (Count < LF_COUNT)
    {
      /* Increment the tuning of the LFRCO and get the count again */
      PreviousCount = Count;
      CMU_OscillatorTuningSet(cmuOsc_LFRCO, ++Tuning);
      Count = CMU_Calibrate(HF_COUNT, cmuOsc_LFRCO);
    }

    /* If the previous count is closer than the current count */
    if ((Count - LF_COUNT) > (LF_COUNT - PreviousCount))
      /* Adjust the tuning for the previous count */
      CMU_OscillatorTuningSet(cmuOsc_LFRCO, --Tuning);
  }
  else if (Count > LF_COUNT)
  {
    /* While the current count is greater than the desired count */
    while (Count > LF_COUNT)
    {
      /* Decrement the tuning of the LFRCO and get the count again */
      PreviousCount = Count;
      CMU_OscillatorTuningSet(cmuOsc_LFRCO, --Tuning);
      Count = CMU_Calibrate(HF_COUNT, cmuOsc_LFRCO);
    }

    /* If the previous count is closer than the current count */
    if ((LF_COUNT - Count) > (PreviousCount - LF_COUNT))
      /* Adjust the tuning for the previous count */
      CMU_OscillatorTuningSet(cmuOsc_LFRCO, ++Tuning);
  }
}

