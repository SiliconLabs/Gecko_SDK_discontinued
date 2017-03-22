/**************************************************************************//**
* @file
* @brief Heart Rate and SpO2 state machine
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
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_chip.h"
#include "em_timer.h"
#include "heart_rate_monitor_config.h"
#include "si114xhrm.h"
#include "si114x_types.h"
#include "Si114x_functions.h"
#include "heart_rate_monitor.h"
#include "sensor_puck.h"
#include "si114xhrm_user_functions.h"
#include "si114x_sys_out_puck.h"

s16 Si114xProcessIrq2(HANDLE si114x_handle, u16 timestamp, u16 *p_sample);

static Si114xhrmConfiguration_t *currentHRMConfig;
static Si114xhrmHandle_t        hrmHandle = 0;

// Variables for inactive timeout
static volatile bool          enableInactiveTimer  = false;
static volatile bool          inactiveTimeout      = false;
static volatile int           inactiveTimerCounter = 0;
// Variables for invalid timeout
static volatile bool          enableInvalidTimer  = false;
static volatile bool          invalidTimeout      = false;
static volatile int           invalidTimerCounter = 0;
// Variables for red LED flash timeout
static volatile bool          redLEDTimeout      = false;
static volatile int           redLEDTimerCounter = 0;
static volatile bool          enableRedLEDTimer  = false;
// Variables for sample throdeling
static volatile int           CurrentSampleTime = 0;
static volatile int           LastSampleTime    = 0;
// Memory allocation for HRM/SpO2 algorithm
static Si114xhrmDataStorage_t hrmDataStorage;
//static Si114xSpO2DataStorage_t spo2Data;
static Si114xDataStorage_t    dataStorage;

// Timeout values
#define RED_LED_TIMEOUT     10
#define INVALID_TIMEOUT     2000
#define INACTIVE_TIMEOUT    50

// Heart Rate value stored for LCD display
static int16_t       displayHeartRateValue = 0;
// Error flag indicates sample buffer full
static volatile bool bufferOverrunError = false;

// Si1147 postage stamp module config
static Si114xhrmConfiguration_t Si1147PsHRMConfig = {
  0,      // ps_select
  0xff,   // current_led
  0x1,    // tasklist
  0x421,  // psled_select
  0xa4,   // measrate
  0,      // adcgain
  0x24,   // adcmisc
  0x0333, // adcmux
  0       // ps_align
};



/**************************************************************************//**
 * Local prototypes
 *****************************************************************************/
static void HeartRateMonitor_UpdateDisplay(HRMSpO2State_t state, bool displaySpO2);

/**************************************************************************//**
 * @brief  Helper function to set green LED on.
 *****************************************************************************/
static inline void enableGreenLED (void)
{
  GPIO_PinOutClear(GREEN_LED_PORT,GREEN_LED_PIN);
}

/**************************************************************************//**
 * @brief  Helper function to set green LED off.
 *****************************************************************************/
static inline void disableGreenLED (void)
{
  GPIO_PinOutSet(GREEN_LED_PORT,GREEN_LED_PIN);
}

/**************************************************************************//**
 * @brief  Helper function to set red LED on.
 *****************************************************************************/
static inline void enableRedLED (void)
{
  GPIO_PinOutClear(RED_LED_PORT,RED_LED_PIN);
}

/**************************************************************************//**
 * @brief  Helper function to set red LED off.
 *****************************************************************************/
static inline void disableRedLED (void)
{
  GPIO_PinOutSet(RED_LED_PORT,RED_LED_PIN);
}

/**************************************************************************//**
 * @brief  Helper function to set green LED state.
 *****************************************************************************/
static void setGreenLED(bool enable)
{
  if (enable)
  {
    enableGreenLED();
  }
  else
  {
    disableGreenLED();
  }
}

/**************************************************************************//**
 * @brief  Helper function to set red LED state.
 *****************************************************************************/
static void setRedLED(bool enable)
{
  if (enable)
  {
    enableRedLED();
    enableRedLEDTimer = true;
  }
  else
  {
    disableRedLED();
  }
}

/**************************************************************************//**
 * @brief This function returns the algorithm version string.
 *****************************************************************************/
void HeartRateMonitor_GetVersion(char *hrmVersion)
{
  si114xhrm_QuerySoftwareRevision((int8_t *) hrmVersion);
}

/**************************************************************************//**
 * @brief This function enables the 100us timer for HRM.
 *****************************************************************************/
static void startTimer()
{
  // Enable clock for TIMER module
  CMU_ClockEnable(CLK_HRM_TIMER1, true);
  CMU_ClockEnable(CLK_HRM_TIMER2, true);
  TIMER_Reset(HRM_TIMER1);
  TIMER_Reset(HRM_TIMER2);
  // Select TIMER parameters
  TIMER_Init_TypeDef timerInit1 =
  {
    .enable     = true,
    .debugRun   = true,
    .prescale   = timerPrescale1,
    .clkSel     = timerClkSelHFPerClk,
    .fallAction = timerInputActionNone,
    .riseAction = timerInputActionNone,
    .mode       = timerModeUp,
    .dmaClrAct  = false,
    .quadModeX4 = false,
    .oneShot    = false,
    .sync       = false,
  };
  TIMER_Init_TypeDef timerInit2 =
  {
    .enable     = true,
    .debugRun   = true,
    .prescale   = timerPrescale1,
    .clkSel     = timerClkSelCascade,
    .fallAction = timerInputActionNone,
    .riseAction = timerInputActionNone,
    .mode       = timerModeUp,
    .dmaClrAct  = false,
    .quadModeX4 = false,
    .oneShot    = false,
    .sync       = false,
  };
  // Set TIMER Top value
  TIMER_TopSet(HRM_TIMER1, CMU_ClockFreqGet(cmuClock_HF) / 1 / 10000); /*overflow every 100us*/
  TIMER_TopSet(HRM_TIMER2, 0xffff);                                    /*max 16 bits*/
  // Configure TIMER
  TIMER_Init(HRM_TIMER1, &timerInit1);
  TIMER_Init(HRM_TIMER2, &timerInit2);
}

/**************************************************************************//**
 * @brief Initiate the Heart Rate Monitor
 *****************************************************************************/
void HeartRateMonitor_Init(Si114xPortConfig_t* i2c)
{
  // Setup SysTick Timer for 10msec interrupts.
  if (SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 100))
    while (1)
      ;
  // Setup Timer as 100 usec counter.  The HRM API requires a 100usec timestamp.
  startTimer();
  dataStorage.spo2 = NULL;
  dataStorage.hrm  = &hrmDataStorage;
  hrmHandle        = &dataStorage;
  si114xhrm_Initialize(i2c, 0, &hrmHandle);

  si114xhrm_Configure(hrmHandle, &Si1147PsHRMConfig);
  currentHRMConfig = &Si1147PsHRMConfig;

  //turn off LEDs
  setRedLED(false);
  setGreenLED(false);
  bufferOverrunError    = false;
  displayHeartRateValue = 0;
  HeartRateMonitor_UpdateDisplay(HRM_STATE_NOSIGNAL, false);
}

/**************************************************************************//**
 * @brief Check for samples in irq queue
 *****************************************************************************/
bool HeartRateMonitor_SamplesPending()
{
  HANDLE si114xHandle;
  si114xhrm_GetLowLevelHandle(hrmHandle, &si114xHandle);
  return(Si114xIrqQueueNumentries(si114xHandle) > 0);
}

/**************************************************************************//**
 * @brief Reset inactive timer values
 *****************************************************************************/
static void resetInactiveTimer()
{
  enableInactiveTimer  = false;
  inactiveTimeout      = false;
  inactiveTimerCounter = 0;
}

/**************************************************************************//**
 * @brief Reset invalid timer values
 *****************************************************************************/
static void resetInvalidTimer()
{
  invalidTimeout      = false;
  enableInvalidTimer  = false;
  invalidTimerCounter = 0;
}

/**************************************************************************//**
 * @brief Stop HRM Processing and interrupts
 *****************************************************************************/
static void stopHRM()
{
  HANDLE si114xHandle;
  si114xhrm_GetLowLevelHandle(hrmHandle, &si114xHandle);
  si114xhrm_Pause(hrmHandle);
  displayHeartRateValue = 0;
  Si114xWriteToRegister(si114xHandle, REG_PS_LED21, currentHRMConfig->ledCurrent & 0xff);
  Si114xWriteToRegister(si114xHandle, REG_PS_LED3, currentHRMConfig->ledCurrent >> 8);
}

/**************************************************************************//**
 * @brief State machine for heart rate measurement
 *****************************************************************************/
static void heartRateStateMachine(HRMSpO2State_t *state, int hrmStatus)
{
  *state = HRM_STATE_INVALID;
  if ((hrmStatus & SI114xHRM_STATUS_HRM_MASK) == SI114xHRM_STATUS_SUCCESS)
  {
    *state = HRM_STATE_ACTIVE;
    resetInactiveTimer();
  }
  else
  {
    if (hrmStatus & SI114xHRM_STATUS_FINGER_OFF)
    {
      *state              = HRM_STATE_NOSIGNAL;
      enableInactiveTimer = true;
      if (inactiveTimeout == true)
      {
        *state = HRM_STATE_IDLE;
        stopHRM();
      }
    }
    else
    {
      resetInactiveTimer();
      if (hrmStatus & SI114xHRM_STATUS_FINGER_ON)
        *state = HRM_STATE_ACQUIRING;
      if (hrmStatus & SI114xHRM_STATUS_ZERO_CROSSING_INVALID)
        *state = HRM_STATE_INVALID;
      if (hrmStatus & SI114xHRM_STATUS_BPF_PS_VPP_OFF_RANGE)
        *state = HRM_STATE_INVALID;
      if (hrmStatus & SI114xHRM_STATUS_AUTO_CORR_TOO_LOW)
        *state = HRM_STATE_INVALID;
      if (hrmStatus & SI114xHRM_STATUS_CREST_FACTOR_TOO_HIGH)
        *state = HRM_STATE_INVALID;
    }
  }
}

/**************************************************************************//**
 * @brief State machine for spO2 measurement
 *****************************************************************************/
static void spo2StateMachine(HRMSpO2State_t *state, int hrmStatus)
{
  *state = HRM_STATE_INVALID;
  if ((hrmStatus & SI114xHRM_STATUS_SPO2_FINGER_OFF) || (hrmStatus & SI114xHRM_STATUS_FINGER_OFF))
  {
    *state              = HRM_STATE_NOSIGNAL;
    enableInactiveTimer = 1;
    if (inactiveTimeout == true)
    {
      *state = HRM_STATE_IDLE;
      stopHRM();
    }
  }
  else
  {
    resetInactiveTimer();
    if ((hrmStatus & SI114xHRM_STATUS_SPO2_FINGER_ON) || (hrmStatus & SI114xHRM_STATUS_FINGER_ON))
    {
      *state = HRM_STATE_ACQUIRING;
    }
    if (hrmStatus & SI114xHRM_STATUS_SPO2_TOO_LOW_AC)
      *state = HRM_STATE_INVALID;
    else if (hrmStatus & SI114xHRM_STATUS_SPO2_TOO_HIGH_AC)
      *state = HRM_STATE_INVALID;
    else if (hrmStatus & SI114xHRM_STATUS_SPO2_EXCEPTION)
      *state = HRM_STATE_INVALID;
    else if (hrmStatus & SI114xHRM_STATUS_SPO2_CREST_FACTOR_OFF)
      *state = HRM_STATE_INVALID;
    else if (((hrmStatus & SI114xHRM_STATUS_HRM_MASK) != SI114xHRM_STATUS_SUCCESS) && !((hrmStatus & SI114xHRM_STATUS_FINGER_OFF)))
      *state = HRM_STATE_INVALID;
    else
      *state = HRM_STATE_ACTIVE;
  }
}

/**************************************************************************//**
 * @brief The Heart Rate Monitor loop
 *        It implements the main state machine.
 *****************************************************************************/
bool HeartRateMonitor_Loop(bool forceStop, bool checkSkinContact)
{
  static int32_t        hrmStatus;
  int16_t               heartRate;
  int16_t               spo2;
  static HRMSpO2State_t state = HRM_STATE_IDLE; // stores the main state of the state machine
  HANDLE                si114xHandle;
  int32_t               skinDetect = 0;
  si114xhrm_GetLowLevelHandle(hrmHandle, &si114xHandle);
  static bool           updateDisplay = false; // flag to save when display needs to update
  bool                  displaySpO2   = false;

  if (forceStop && state != HRM_STATE_IDLE)
  {
    // main application sets forceStop to true to force the HRM algo to quit*/
    state = HRM_STATE_IDLE;
    stopHRM();
    checkSkinContact = false;
  }

  if (state == HRM_STATE_IDLE)
  {
    /*
     * in the idle state we periodically check for skin contact
     * and if detected start the algorithm
     */
    resetInactiveTimer();
    if (checkSkinContact)
      si114xhrm_DetectSkinContact(hrmHandle, &skinDetect);
    if (skinDetect)
    {
      state = HRM_STATE_NOSIGNAL;
      // start the algorithm
      si114xhrm_Run(hrmHandle);
    }
    else
      return false;
  }
  if (bufferOverrunError)
  {
    state              = HRM_STATE_IDLE;
    bufferOverrunError = false;
    stopHRM();
    Si114xIrqQueue_Clear(si114xHandle);
  }
  // call the main HRM algorithm processing function
  if (si114xhrm_Process(hrmHandle, &heartRate, &spo2, &hrmStatus, 0) == SI114xHRM_SUCCESS)
  {
    // when STATUS_FRAME_PROCESSED is set new data is available
    if (hrmStatus & SI114xHRM_STATUS_FRAME_PROCESSED)
    {
      hrmStatus &= ~SI114xHRM_STATUS_FRAME_PROCESSED;
      // process error codes
      heartRateStateMachine(&state, hrmStatus);
      // if heart rate is valid save the value
      if (state == HRM_STATE_ACTIVE)
        displayHeartRateValue = heartRate;
      // if we want to display SpO2 check the SpO2 status too*/
      if (displaySpO2)
      {
        spo2StateMachine(&state, hrmStatus);
        // if spo2 value is valid save the value
      }

      // if data is invalid start a timeout
      if (state == HRM_STATE_INVALID)
      {
        enableInvalidTimer = true;
        // when timeout expires clear old data
        if (invalidTimeout == true)
        {
          state = HRM_STATE_IDLE;
          stopHRM();
          resetInvalidTimer();
        }
      }
      // update the display when we get new data*/
      updateDisplay = true;
    }
  }
  if ((updateDisplay) && (HeartRateMonitor_SamplesPending() == false))
  {
    HeartRateMonitor_UpdateDisplay(state, displaySpO2);
    updateDisplay = false;
  }
  // this timeout turns off the red led to produce the flash effect
  if (redLEDTimeout)
  {
    redLEDTimeout = false;
    setRedLED(false);
  }

  return(state != HRM_STATE_IDLE);
}



/**************************************************************************//**
 * @brief Update the data based on the HRM state
 *****************************************************************************/
static void HeartRateMonitor_UpdateDisplay(HRMSpO2State_t state, bool displaySpO2)
{
  (void) displaySpO2;
  switch (state)
  {
  case HRM_STATE_IDLE:
  case HRM_STATE_NOSIGNAL:
    setRedLED(false);
    setGreenLED(false);
    resetInvalidTimer();
    break;
  case HRM_STATE_ACQUIRING:
    // flash red led
    setRedLED(true);
    enableInvalidTimer = true;
    break;
  case HRM_STATE_ACTIVE:
    setGreenLED(true);
    resetInvalidTimer();
    break;
  case HRM_STATE_INVALID:
    if (displayHeartRateValue == 0)
    {
      setRedLED(true);
      setGreenLED(false);
    }
    break;
  case HRM_STATE_ERROR:
    setRedLED(true);
    break;
  }
  if (displayHeartRateValue == 0)
    HRMState = state;
  else
    HRMState = HRM_STATE_ACTIVE;
  HRMRate = displayHeartRateValue;
}

/**************************************************************************//**
 * @brief GPIO Interrupt handler (Si114x INT)
 *****************************************************************************/
void HeartRateMonitor_Interrupt(void)
{
  int16_t  error;
  HANDLE   si114xHandle;
  uint16_t sample;
  si114xhrm_GetLowLevelHandle(hrmHandle, &si114xHandle);
  error = Si114xProcessIrq2(si114xHandle, TIMER_CounterGet(HRM_TIMER2), &sample);
  int lastSample = LastSampleTime;

  /* If it has been at least 30ms since the last sample */
  if ((CurrentSampleTime - lastSample) > 3)
  {
    /* Provide the sample to Bluetooth */
    hrmSampleCallback(sample);
    LastSampleTime = CurrentSampleTime;
  }

  if (error != 0)
    bufferOverrunError = true;
}

/**************************************************************************//**
 * @brief Timer Event Handler function for the Heart Rate Monitor.
 *        This function must be called from within the 10ms systick interrupt
 *****************************************************************************/
void HeartRateMonitor_TimerEventHandler(void)
{
  if (enableRedLEDTimer)
  {
    redLEDTimerCounter++;
    if (redLEDTimerCounter == RED_LED_TIMEOUT)
    {
      redLEDTimerCounter = 0;
      redLEDTimeout      = true;
      enableRedLEDTimer  = false;
    }
  }
  if (enableInactiveTimer == 1)
  {
    inactiveTimerCounter++;
    if (inactiveTimerCounter == INACTIVE_TIMEOUT)
    {
      inactiveTimeout      = true;
      inactiveTimerCounter = 0;
      enableInactiveTimer  = false;
    }
  }
  if (enableInvalidTimer == true)
  {
    invalidTimerCounter++;
    if (invalidTimerCounter == INVALID_TIMEOUT)
    {
      //invalidTimeout = true;
      invalidTimerCounter = 0;
      enableInvalidTimer  = false;
    }
  }
  /* Increment every 10ms */
  CurrentSampleTime++;
}

