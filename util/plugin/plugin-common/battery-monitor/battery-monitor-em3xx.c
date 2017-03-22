// *****************************************************************************
// * battery-monitor-em3xx.c
// *
// * Code to implement a battery monitor.
// *
// * This code will read the battery voltage during a transmission (in theory
// * when we are burning the most current), and update the battery voltage
// * attribute in the power configuration cluster.
// *
// *
// * Copyright 2015 Silicon Laboratories, Inc.                              *80*
// *****************************************************************************

#include PLATFORM_HEADER
#include CONFIGURATION_HEADER
#include "stack/include/ember-types.h"
#include "stack/include/event.h"
#include "hal/hal.h"
#include EMBER_AF_API_BATTERY_MONITOR
#include EMBER_AF_API_GENERIC_INTERRUPT_CONTROL

// Shorter macros for plugin options
#define FIFO_SIZE EMBER_AF_PLUGIN_BATTERY_MONITOR_SAMPLE_FIFO_SIZE
#define MS_BETWEEN_BATTERY_CHECK \
          (EMBER_AF_PLUGIN_BATTERY_MONITOR_MONITOR_TIMEOUT_M * 60 * 1000)

#define MAX_INT_MINUS_DELTA 0xe0000000

//------------------------------------------------------------------------------
// Forward Declaration
static uint16_t filterVoltageSample(uint16_t sample);
static void batteryMonitorIsr(void);

//------------------------------------------------------------------------------
// Globals

EmberEventControl emberAfPluginBatteryMonitorReadADCEventControl;

// structure used to store irq configuration from GIC plugin
static HalGenericInterruptControlIrqCfg *irqConfig;

// count used to track when the last measurement occurred
// Ticks start at 0.  We use this value to limit how frequently we make
// measurements in an effort to conserve battery power.  By setting this to an
// arbitrary value close to MAX_INT, we are going to make sure we make a
// battery measurement on the first transmission.
static uint32_t lastBatteryMeasureTick = MAX_INT_MINUS_DELTA;

// sample FIFO access variables
static uint8_t samplePtr = 0;
static uint16_t voltageFifo[FIFO_SIZE];
static bool fifoInitialized = false;

// Remember the last reported voltage value from callback, which will be the
// return value if anyone needs to manually poll for data
static uint16_t lastReportedVoltageMilliV;

//------------------------------------------------------------------------------
// Implementation of public functions

void emberAfPluginBatteryMonitorInitCallback(void)
{
  halBatteryMonitorInitialize();
}

void halBatteryMonitorInitialize(void)
{
  // Set up the generic interrupt controller to handle changes on the gpio
  // sensor.  Note that on an em35x, the TX active pin is always port C, pin 5,
  // so these values are hard coded.
  irqConfig = halGenericInterruptControlIrqCfgInitialize(
    5,
    HAL_GIC_GPIO_PORTC,
    BATTERY_MONITOR_IRQ);
  halGenericInterruptControlIrqIsrAssignFxn(irqConfig, batteryMonitorIsr);
  halGenericInterruptControlIrqEnable(irqConfig);
}

uint16_t halGetBatteryVoltageMilliV(void)
{
  return lastReportedVoltageMilliV;
}

// This event will be triggered by the TX Active ISR.  It will sample the ADC
// during a radio transmission and notify any interested parties of a new valid
// battery voltage level via the emberAfPluginBatteryMonitorDataCallback
void emberAfPluginBatteryMonitorReadADCEventHandler(void)
{
  emberEventControlSetInactive(emberAfPluginBatteryMonitorReadADCEventControl);

  uint16_t voltageMilliV;
  uint32_t currentMsTick = halCommonGetInt32uMillisecondTick();
  uint32_t timeSinceLastMeasureMS = currentMsTick - lastBatteryMeasureTick;

  if (timeSinceLastMeasureMS >= MS_BETWEEN_BATTERY_CHECK) {
    // The most common and shortest (other than the ACK) transmission is the
    // data poll.  It takes 512 uS for a data poll.  Therefore, the conversion
    // needs to be faster than 256.
    // However, I think this is ignored, and halMeasureVdd uses 256 regardless
    // of what value we give it.
    voltageMilliV = halMeasureVdd(ADC_CONVERSION_TIME_US_256);

    // filter the voltage
    voltageMilliV = filterVoltageSample(voltageMilliV);

    emberAfPluginBatteryMonitorDataCallback(voltageMilliV);
    lastReportedVoltageMilliV = voltageMilliV;
    lastBatteryMeasureTick = currentMsTick;
  }
}

//------------------------------------------------------------------------------
// Implementation of private functions

static uint16_t filterVoltageSample(uint16_t sample)
{
  uint16_t voltageSum;
  uint8_t i;

  if (fifoInitialized) {
    voltageFifo[samplePtr++] = sample;

    if (samplePtr >= FIFO_SIZE) {
      samplePtr = 0;
    }
    voltageSum = 0;
    for (i=0; i<FIFO_SIZE; i++) {
      voltageSum += voltageFifo[i];
    }
    sample = voltageSum / FIFO_SIZE;
  } else {
    for (i=0; i<FIFO_SIZE; i++) {
      voltageFifo[i] = sample;
    }
    fifoInitialized = true;
  }

  return sample;
}

//!!!CAUTION!!!!
// This function runs in interrupt context.  Take proper care when modifying or
// adding functionality.
static void batteryMonitorIsr(void)
{
  INT_MISS = irqConfig->irqMissBit;
  INT_GPIOFLAG = irqConfig->irqFlagBit;

  if (halGenericInterruptControlIrqReadGpio(irqConfig))
  {
    emberEventControlSetActive(emberAfPluginBatteryMonitorReadADCEventControl);
  }
}
