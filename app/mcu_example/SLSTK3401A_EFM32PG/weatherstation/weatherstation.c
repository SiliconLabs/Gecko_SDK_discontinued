/**************************************************************************//**
 * @file
 * @brief Weather station demo for SLSTK3402A and Sensors-EXP
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
#include "i2cspm.h"
#include "si7013.h"
#include "brd8001a_config.h"
#include "si114x_algorithm.h"
#include "rtcdriver.h"
#include "graphics.h"
#include "bspconfig.h"

/**************************************************************************//**
 * Local defines
 *****************************************************************************/
/** Time (in ms) to keep looking for gestures if none are seen. */
#define GESTURE_TIMEOUT_MS      60000
/** Time (in ms) between periodic updates of the measurements. */
#define PERIODIC_UPDATE_MS      2000
/** Time (in ms) between scrolling updates. Lower means faster scrolling
 *  between screens. */
#define ANIMATION_UPDATE_MS     15


/**************************************************************************//**
 * Local variables
 *****************************************************************************/
/* RTC callback parameters. */
static void (*rtccCallback)(void*) = NULL;
static void * rtccCallbackArg = 0;

/** This variable is set to true when the user presses PB0. */
static volatile bool startDemo = false;
/** This variable is set to true when we are in gesture control mode. */
static volatile bool demoMode =  false;
/** This flag tracks if we need to update the display
 *  (animations or measurements). */
static volatile bool updateDisplay = true;
/** This flag tracks if we need to perform a new
 *  measurement. */
static volatile bool updateMeasurement = true;
/** Flag that is used whenever we have get an gesture process interrupt. */
static volatile bool processGestures = false;
/** GUI scrolling offset. */
static volatile int  xoffset = 0;
/** GUI scrolling offset. */
static volatile int  yoffset = 0;
/** Amount to increment xoffset every ANIMATION_UPDATE_MS */
static volatile int  xinc            = 0;
/** Amount to increment yoffset every ANIMATION_UPDATE_MS */
static volatile int  yinc = 0;
/** Millisecond tick counter */
volatile uint32_t msTicks;
/** Used to track what happens when the user presses PB0. */
static int pb0Action = 1;

/** Timer used for timing out gesturemode to save power. */
RTCDRV_TimerID_t gestureTimeoutTimerId;
/** Timer used for counting milliseconds. Used for gesture detection. */
RTCDRV_TimerID_t millisecondsTimerId;
/** Timer used for periodic update of the measurements. */
RTCDRV_TimerID_t periodicUpdateTimerId;
/** Timer used for animations (swiping) */
RTCDRV_TimerID_t animationTimerId;

/**************************************************************************//**
 * Local prototypes
 *****************************************************************************/
static void gpioSetup(void);
void PB0_handler(void);
static void disableGestureMode(RTCDRV_TimerID_t id, void *user);
static void enableGestureMode(void);
static void msTicksCallback(RTCDRV_TimerID_t id, void *user);
static void handleGestures(void);
static void periodicUpdateCallback(RTCDRV_TimerID_t id, void *user);
static void animationUpdateCallback(RTCDRV_TimerID_t id, void *user);
static void memLcdCallback(RTCDRV_TimerID_t id, void *user);
static bool isBatteryLow (void);
static void emuInit(void);

/**************************************************************************//**
 * @brief  Helper function to perform data measurements.
 *****************************************************************************/
static int performMeasurements(uint32_t *rhData, int32_t *tData,
                               uint16_t *uvData, int *objectDetect, bool *lowBat)
{
  *lowBat = isBatteryLow();
  Si7013_MeasureRHAndTemp(I2C0, SI7013_ADDR, rhData, tData);
  Si1147_MeasureUVAndObjectPresent(I2C0, SI1147_ADDR, uvData, objectDetect);
  return 0;
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  I2CSPM_Init_TypeDef i2cInit = I2CSPM_INIT_DEFAULT;
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_STK_DEFAULT;
  uint32_t         rhData;
  bool             si7013_status, si1147_status;
  int32_t          tempData;
  uint16_t         uvData;
  /* objectDetect is set to true when hand (or object) is detected near
  *  Si1147. We look for rising edge on this because if the board is
  *  continuously detecting an object it may be because it is inside
  *  a briefcase or box and not in use. To prevent battery drain we
  *  do not want to detect this condition as user input.
  */
  int              objectDetect;
  int              objectDetectPrevious = 1;
  int              offset;
  bool             lowBat = false;

  /* Chip errata */
  CHIP_Init();

  /* Misc initalizations. */
  EMU_DCDCInit(&dcdcInit);
  gpioSetup();
  emuInit();

  /* Misc timers. */
  RTCDRV_Init();
  RTCDRV_AllocateTimer(&gestureTimeoutTimerId);
  RTCDRV_AllocateTimer(&millisecondsTimerId);
  RTCDRV_AllocateTimer(&periodicUpdateTimerId);
  RTCDRV_AllocateTimer(&animationTimerId);

  GRAPHICS_Init();

  /* Initialize I2C driver, using standard rate. */
  I2CSPM_Init(&i2cInit);

  si7013_status = Si7013_Detect(I2C0, SI7013_ADDR, NULL);
  si1147_status = Si1147_Detect_Device(I2C0, SI1147_ADDR);
  GRAPHICS_ShowStatus(si1147_status, si7013_status, false, false);
  /* configure prox sensor to enter low power state */
  Si1147_ConfigureDetection(I2C0, SI1147_ADDR, true);

  /* Set up periodic update of the display.
   * Note: Must update at a much lesser rate than the gesture detection
   * algorithm sample rate. */
  RTCDRV_StartTimer(periodicUpdateTimerId, rtcdrvTimerTypePeriodic,
                    PERIODIC_UPDATE_MS, periodicUpdateCallback, NULL);

  updateDisplay = true;
  demoMode = false;

  while (1)
  {
    if (updateMeasurement)
    {
      performMeasurements(&rhData, &tempData, &uvData, &objectDetect, &lowBat);
    }
    if (demoMode)
    {
      if (processGestures)
      {
        handleGestures();
        /* Check if interrupt pin still low (if it is we have another sample ready). */
        if (GPIO_PinInGet(BRD8001A_INT_INPUT_PORT, BRD8001A_INT_INPUT_PIN) == 0)
          handleGestures();
        processGestures = false;
      }
      if (updateDisplay)
      {
        offset = xoffset;
        GRAPHICS_Draw(offset, tempData, rhData, uvData, yoffset, lowBat);
        updateDisplay = false;
      }
    }
    else
    {
      if (updateDisplay)
      {
        updateDisplay = false;

        if ((objectDetect && !objectDetectPrevious) || startDemo)
        {
          demoMode = true;
          enableGestureMode();
          updateDisplay = true;
          startDemo = false;
        }

        GRAPHICS_ShowStatus(si1147_status, si7013_status, objectDetect&objectDetectPrevious, lowBat);
        objectDetectPrevious = objectDetect;
      }
    }

    EMU_EnterEM2(false);
  }
}

/**************************************************************************//**
 * @brief Check the status of the EMU voltage monitor.
 *****************************************************************************/
static bool isBatteryLow (void)
{
  return ! EMU_VmonChannelStatusGet(emuVmonChannel_AVDD);
}

/**************************************************************************//**
 * @brief Enable EMU voltage monitor (VMON) to monitor the supply voltage of
 *        the digital domain.
 *****************************************************************************/
static void emuInit(void)
{
  EMU_VmonInit_TypeDef vmonInit = EMU_VMONINIT_DEFAULT;
  vmonInit.channel   = emuVmonChannel_AVDD;
  vmonInit.threshold = 2700; /* 2.7 V */
  EMU_VmonInit(&vmonInit);
}

/**************************************************************************//**
 * @brief This function is called whenever a new gesture needs to be processed.
 *        It is responsible for setting up the animations.
 *****************************************************************************/
static void handleGestures(void)
{
  gesture_t gestureInput = NONE;

  /* get prox sensor sample */
  gestureInput = Si1147_NewSample(I2C0, SI1147_ADDR, msTicks);
  if (gestureInput != NONE)
  {
    /* Gesture detected, restart timer */
    RTCDRV_StartTimer(gestureTimeoutTimerId, rtcdrvTimerTypeOneshot,
                      GESTURE_TIMEOUT_MS, disableGestureMode, NULL);
    if ((gestureInput == UP) || (gestureInput == DOWN))
    {
      if (xoffset == 0)
      {
        if (gestureInput == UP)
          yinc = 1;
        else
          yinc = -1;
      }
    }
    if (gestureInput == RIGHT)
    {
      xinc = -1;
    }
    if (gestureInput == LEFT)
    {
      xinc = 1;
    }
    updateDisplay = true;
    /* This timer runs the animations. E.g if an animation is
     * active this will retrigger a redraw. */
    RTCDRV_StartTimer(animationTimerId, rtcdrvTimerTypePeriodic,
                      ANIMATION_UPDATE_MS, animationUpdateCallback, NULL);
  }
}

/**************************************************************************//**
 * @brief Enable gesture mode.
 *****************************************************************************/
static void enableGestureMode(void)
{
  Si1147_ConfigureDetection(I2C0, SI1147_ADDR, false);
  Si1147_SetInterruptOutputEnable(I2C0, SI1147_ADDR, true);

  /* Start timer to disable gestures */
  RTCDRV_StartTimer(gestureTimeoutTimerId, rtcdrvTimerTypeOneshot,
                    GESTURE_TIMEOUT_MS, disableGestureMode, NULL);
  /* Start timer to count milliseconds - used for gesture detection */
  RTCDRV_StartTimer(millisecondsTimerId, rtcdrvTimerTypePeriodic,
                    5, msTicksCallback, NULL);
}

/**************************************************************************//**
 * @brief Disable gesture mode.
 * @param id
 *   Timer ID that triggered this event. Not used, only there for
 *   compatability with RTC driver.
 *****************************************************************************/
static void disableGestureMode(RTCDRV_TimerID_t id, void *user)
{
  (void) id;
  (void) user;
  Si1147_ConfigureDetection(I2C0, SI1147_ADDR, true);

  /* Stop counting milliseconds */
  RTCDRV_StopTimer(millisecondsTimerId);
  /* Increment timer an arbitrary amount of time to account for timer stopping*/
  msTicks += 1000;
  demoMode = false;
  /* This timer runs the animations. E.g if an animation is
   * active this will retrigger a redraw. */
  RTCDRV_StopTimer(animationTimerId);
}

/**************************************************************************//**
* @brief Setup GPIO interrupt for pushbuttons.
*****************************************************************************/
static void gpioSetup(void)
{
  /* Enable GPIO clock */
  CMU_ClockEnable(cmuClock_GPIO, true);

  /* Configure PB0 as input and enable interrupt  */
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInputPull, 1);
  GPIO_IntConfig(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, false, true, true);

  /* Configure PB1 as input and enable interrupt */
  GPIO_PinModeSet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, gpioModeInputPull, 1);
  GPIO_IntConfig(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, false, true, true);

  NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
  NVIC_EnableIRQ(GPIO_EVEN_IRQn);

  NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
  NVIC_EnableIRQ(GPIO_ODD_IRQn);

  /* Configure power pin as pushpull. (5v enable signal) */
  GPIO_PinModeSet(BRD8001A_POWER_PORT, BRD8001A_POWER_PIN, gpioModePushPull, 0);

  /* Configure interrupt input pin and enable interrupt - proximity interrupt. */
  /* Interrupt is active low */
  GPIO_PinModeSet(BRD8001A_INT_INPUT_PORT,
                  BRD8001A_INT_INPUT_PIN, gpioModeInputPull, 1);
  GPIO_IntConfig(BRD8001A_INT_INPUT_PORT,
                 BRD8001A_INT_INPUT_PIN, false, true, true);

  /* Enable 5V supply to add-on board. */
  GPIO_PinOutSet(BRD8001A_POWER_PORT, BRD8001A_POWER_PIN);
}

/**************************************************************************//**
* @brief Unified GPIO Interrupt handler (pushbuttons)
*        PB0 Switches units within a measurement display
*        PB1 Starts the demo (quit splashscreen)
*****************************************************************************/
void GPIO_Unified_IRQ(void)
{
  /* Get and clear all pending GPIO interrupts */
  uint32_t interruptMask = GPIO_IntGet();
  GPIO_IntClear(interruptMask);

  /* Act on interrupts */
  if (interruptMask & (1 << BSP_GPIO_PB0_PIN))
  {
    /* BTN0: Switch units within display*/
    PB0_handler();
  }

  if (interruptMask & (1 << BSP_GPIO_PB1_PIN))
  {
    /* BTN1: Start the demo */
    startDemo = true;
  }

  if (interruptMask & (1 << BRD8001A_INT_INPUT_PIN))
  {
    /* Interrupt from Si1147 */
    processGestures = true;
  }
}

/**************************************************************************//**
* @brief GPIO Interrupt handler for even pins
*****************************************************************************/
void GPIO_EVEN_IRQHandler(void)
{
  GPIO_Unified_IRQ();
}

/**************************************************************************//**
* @brief GPIO Interrupt handler for odd pins
*****************************************************************************/
void GPIO_ODD_IRQHandler(void)
{
  GPIO_Unified_IRQ();
}

/**************************************************************************//**
 * @brief PB0 Interrupt handler
 *****************************************************************************/
void PB0_handler(void)
{
  if (xoffset == 0)
  {
    pb0Action = 1;
  }
  if (xoffset == 32)
  {
    pb0Action = -1;
  }
  updateDisplay = true;
  /* This timer runs the animations. E.g if an animation is
   * active this will retrigger a redraw. */
  RTCDRV_StartTimer(animationTimerId, rtcdrvTimerTypePeriodic,
                    ANIMATION_UPDATE_MS, animationUpdateCallback, NULL);
  xinc          = pb0Action;
  startDemo     = true;
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
  rtccCallback(rtccCallbackArg);
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
 *         -1 if the requested frequency does not match the RTC frequency.
 *****************************************************************************/
int rtccIntCallbackRegister(void (*pFunction)(void*),
                           void* argument,
                           unsigned int frequency)
{
  RTCDRV_TimerID_t timerId;
  rtccCallback    = pFunction;
  rtccCallbackArg = argument;

  RTCDRV_AllocateTimer(&timerId);

  RTCDRV_StartTimer(timerId, rtcdrvTimerTypePeriodic, 1000 / frequency,
                    memLcdCallback, NULL);

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

/**************************************************************************//**
 * @brief Callback used to count milliseconds using gestures
 *****************************************************************************/
static void msTicksCallback(RTCDRV_TimerID_t id, void *user)
{
  (void) id;
  (void) user;
  msTicks += 5;
}

/**************************************************************************//**
 * @brief Callback used to drive gesture animations.
 *        e.g. the sliding window effect.
 *****************************************************************************/
static void animationUpdateCallback(RTCDRV_TimerID_t id, void *user)
{
  (void) id;
  (void) user;
  int inc;
  if (!updateDisplay)
  {
    if (xinc != 0)
    {
      inc = xinc;
      xoffset += inc;
      if (xoffset < -16)
      {
        xoffset = 32;
      }
      if (xoffset > 32)
      {
        xoffset = -16;
      }
      if ((xoffset == 16) || (xoffset == 32) || (xoffset == 0))
      {
        xinc = 0;
        /* This timer runs the animations. E.g if an animation is
         * active this will retrigger a redraw. */
        RTCDRV_StopTimer(animationTimerId);
      }

      updateDisplay = true;
    }
    if (yinc != 0)
    {
      inc = yinc;
      yoffset += inc;
      if (yoffset < -16)
      {
        yoffset = 16;
      }
      if (yoffset > 16)
      {
        yoffset = -16;
      }
      if ((yoffset == 16) || (yoffset == 0))
      {
        yinc = 0;
        /* This timer runs the animations. E.g if an animation is
         * active this will retrigger a redraw. */
        RTCDRV_StopTimer(animationTimerId);
      }

      updateDisplay = true;
    }
  }
}
