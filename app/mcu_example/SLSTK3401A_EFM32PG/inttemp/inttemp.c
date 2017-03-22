/**************************************************************************//**
 * @file
 * @brief Internal temperature sensor demo for SLSTK3401A_EFM32PG
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
#include "em_adc.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "rtcdriver.h"
#include "graphics.h"
#include "bspconfig.h"
#include <stddef.h>

/**************************************************************************//**
 * Local defines
 *****************************************************************************/

/** Time (in ms) between periodic updates of the measurements. */
#define PERIODIC_UPDATE_MS      1000

/**************************************************************************//**
 * Local variables
 *****************************************************************************/
/* RTC callback parameters. */
static void (*rtcCallback)(void*) = NULL;
static void * rtcCallbackArg = 0;
volatile uint32_t rtcCallbacks = 0;

/** This flag tracks if we need to update the display. */
static volatile bool updateDisplay = true;

/** Timer used for periodic update of the measurements. */
RTCDRV_TimerID_t periodicUpdateTimerId;


/**************************************************************************//**
 * Local prototypes
 *****************************************************************************/
static void periodicUpdateCallback(RTCDRV_TimerID_t id, void *user);
static void memLcdCallback(RTCDRV_TimerID_t id, void *user);

/**************************************************************************//**
 * @brief Initialize ADC for temperature sensor readings in single poin
 *****************************************************************************/
static void AdcSetup(void)
{
  /* Enable ADC clock */
  CMU_ClockEnable(cmuClock_ADC0, true);

  /* Base the ADC configuration on the default setup. */
  ADC_Init_TypeDef       init  = ADC_INIT_DEFAULT;
  ADC_InitSingle_TypeDef sInit = ADC_INITSINGLE_DEFAULT;

  /* Initialize timebases */
  init.timebase = ADC_TimebaseCalc(0);
  init.prescale = ADC_PrescaleCalc(400000, 0);
  ADC_Init(ADC0, &init);

  /* Set input to temperature sensor. Reference must be 1.25V */
  sInit.reference   = adcRef1V25;
  sInit.acqTime     = adcAcqTime8; /* Minimum time for temperature sensor */
  sInit.posSel      = adcPosSelTEMP;
  ADC_InitSingle(ADC0, &sInit);
}

/**************************************************************************//**
 * @brief  Do one ADC conversion
 * @return ADC conversion result
 *****************************************************************************/
static uint32_t AdcRead(void)
{
  ADC_Start(ADC0, adcStartSingle);
  while ( ( ADC0->STATUS & ADC_STATUS_SINGLEDV ) == 0 ){}
  return ADC_DataSingleGet(ADC0);
}

/**************************************************************************//**
 * @brief Convert ADC sample values to celsius.
 * @detail See section 25.3.4.1 in the reference manual for detail on
 *   temperature measurement and conversion.
 * @param adcSample Raw value from ADC to be converted to celsius
 * @return The temperature in degrees celsius.
 *****************************************************************************/
static float ConvertToCelsius(int32_t adcSample)
{
  uint32_t calTemp0;
  uint32_t calValue0;
  int32_t readDiff;
  float temp;

  /* Factory calibration temperature from device information page. */
  calTemp0 = ((DEVINFO->CAL & _DEVINFO_CAL_TEMP_MASK)
              >> _DEVINFO_CAL_TEMP_SHIFT);

  calValue0 = ((DEVINFO->ADC0CAL3
                /* _DEVINFO_ADC0CAL3_TEMPREAD1V25_MASK is not correct in
                    current CMSIS. This is a 12-bit value, not 16-bit. */
                & 0xFFF0)
               >> _DEVINFO_ADC0CAL3_TEMPREAD1V25_SHIFT);

  if ((calTemp0 == 0xFF) || (calValue0 == 0xFFF))
  {
    /* The temperature sensor is not calibrated */
    return -100.0;
  }

  /* Vref = 1250mV
     TGRAD_ADCTH = 1.835 mV/degC (from datasheet)
  */
  readDiff = calValue0 - adcSample;
  temp     = ((float)readDiff * 1250);
  temp    /= (4096 * -1.835);

  /* Calculate offset from calibration temperature */
  temp     = (float)calTemp0 - temp;
  return temp;
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_STK_DEFAULT;
  CMU_HFXOInit_TypeDef hfxoInit = CMU_HFXOINIT_STK_DEFAULT;
  uint32_t tempRead;
  float celsius;

  /* Chip errata */
  CHIP_Init();

  /* Init DCDC regulator and HFXO with WSTK radio board specific parameters
     kits\SLSTK3401A_EFM32PG\config\bspconfig.h. */
  EMU_DCDCInit(&dcdcInit);
  CMU_HFXOInit(&hfxoInit);

  /* Switch HFCLK to HFXO and disable HFRCO */
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);
  CMU_OscillatorEnable(cmuOsc_HFRCO, false, false);

  /* Setup ADC */
  AdcSetup();

  RTCDRV_Init();
  GRAPHICS_Init();

  /* Set up periodic update of the display. */
  RTCDRV_AllocateTimer(&periodicUpdateTimerId);
  RTCDRV_StartTimer(periodicUpdateTimerId, rtcdrvTimerTypePeriodic,
                    PERIODIC_UPDATE_MS, periodicUpdateCallback, NULL);

  updateDisplay = true;

  while (true)
  {
    if (updateDisplay)
    {
      updateDisplay = false;
      tempRead = AdcRead();
      celsius = ConvertToCelsius(tempRead);
      GRAPHICS_ShowTemp(celsius);
    }
    EMU_EnterEM2(false);
  }
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

  rtcCallbacks ++;
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
int rtcIntCallbackRegister(void (*pFunction)(void*),
                           void* argument,
                           unsigned int frequency)
{
  RTCDRV_TimerID_t timerId;
  rtcCallback    = pFunction;
  rtcCallbackArg = argument;

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
}
