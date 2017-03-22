/*
 * File: adc-em3xx.c
 * Description: EM3XX-specific ADC HAL functions
 *
 *
 * Copyright 2008 by Ember Corporation. All rights reserved.                *80*
 */
#include PLATFORM_HEADER
#include "stack/include/error.h"
#include "hal/micro/micro-common.h"
#include "hal/micro/cortexm3/micro-common.h"
#include "plugin/adc/adc.h"

#if (NUM_ADC_USERS > 8)
  #error NUM_ADC_USERS must not be greater than 8, or uint8_t variables in adc-em3xx.c must be changed
#endif

static uint16_t adcData;             // conversion result written by DMA
static uint8_t adcPendingRequests;   // bitmap of pending requests
static volatile uint8_t adcPendingConversion; // id of pending conversion
static uint8_t adcReadingValid;      // bitmap of valid adcReadings
static uint16_t adcReadings[NUM_ADC_USERS];
static uint16_t adcConfig[NUM_ADC_USERS];
static bool adcCalibrated;
static int16_t Nvss;
static int16_t Nvdd;
static uint16_t adcStaticConfig;

void halAdcSetClock(bool slow)
{
  if (slow) {
    adcStaticConfig |= ADC_1MHZCLK_MASK;
  } else {
    adcStaticConfig &= ~ADC_1MHZCLK_MASK;
  }
}

bool halAdcGetClock(void)
{
  return (bool)(adcStaticConfig & ADC_1MHZCLK_MASK);
}


// Define a channel field that combines ADC_MUXP and ADC_MUXN
#define ADC_CHAN        (ADC_MUXP | ADC_MUXN)
#define ADC_CHAN_BIT    ADC_MUXN_BIT

void halAdcIsr(void)
{
  uint8_t i;
  uint8_t conversion = adcPendingConversion; //fix 'volatile' warning; costs no flash

  // make sure data is ready and the desired conversion is valid
  if ( (EVENT_ADC->FLAG & EVENT_ADC_FLAG_ULDFULL)
        && (conversion < NUM_ADC_USERS) ) {
    adcReadings[conversion] = adcData;
    adcReadingValid |= BIT(conversion); // mark the reading as valid
    // setup the next conversion if any
    if (adcPendingRequests) {
      for (i = 0; i < NUM_ADC_USERS; i++) {
        if (BIT(i) & adcPendingRequests) {
          adcPendingConversion = i;     // set pending conversion
          adcPendingRequests ^= BIT(i); //clear request: conversion is starting
          ADC_CFG = adcConfig[i];
          break; //conversion started, so we're done here (only one at a time)
        }
      }
    } else {                                // no conversion to do
      ADC_CFG = 0;                          // disable adc
      halCommonDisableVreg1v8();
      adcPendingConversion = NUM_ADC_USERS; //nothing pending, so go "idle"
    }
  }
  EVENT_ADC->FLAG = 0xFFFF;
  asm("DMB");
}

// An internal support routine called from functions below.
// Returns the user number of the started conversion, or NUM_ADC_USERS
// otherwise.
ADCUser startNextConversion(void)
{
  uint8_t i;

  ATOMIC (
    halCommonEnableVreg1v8();
    // start the next requested conversion if any
    if (adcPendingRequests && !(ADC_CFG & ADC_ENABLE)) {
      for (i = 0; i < NUM_ADC_USERS; i++) {
        if ( BIT(i) & adcPendingRequests) {
          adcPendingConversion = i;     // set pending conversion
          adcPendingRequests ^= BIT(i); // clear request
          ADC_CFG = adcConfig[i];       // set the configuration to desired
          EVENT_ADC->FLAG = 0xFFFF;
          NVIC_EnableIRQ(ADC_IRQn);
          break;
        }
      }
    } else {
      i = NUM_ADC_USERS;
    }
  )
  return i;
}

void halInternalInitAdc(void)
{
  // reset the state variables
  adcPendingRequests = 0;
  adcPendingConversion = NUM_ADC_USERS;
  adcCalibrated = false;
  adcStaticConfig = ADC_1MHZCLK | ADC_ENABLE; // init config: 1MHz, low voltage

  // set all adcReadings as invalid
  adcReadingValid = 0;

  // turn off the ADC
  ADC_CFG = 0;                   // disable ADC, turn off HV buffers
  ADC_OFFSET = ADC_OFFSET_RESET;
  ADC_GAIN = ADC_GAIN_RESET;
  ADC_DMACFG = ADC_DMARST;
  ADC_DMABEG = (uint32_t)&adcData;
  ADC_DMASIZE = 1;
  ADC_DMACFG = (ADC_DMAAUTOWRAP | ADC_DMALOAD);

  // clear the ADC interrupts and enable
  EVENT_ADC->CFG = EVENT_ADC_CFG_ULDFULL;
  EVENT_ADC->FLAG = 0xFFFF;
  NVIC_EnableIRQ(ADC_IRQn);

  emberCalibrateVref();
}

EmberStatus halStartAdcConversion(ADCUser id,
                                  ADCReferenceType reference,
                                  ADCChannelType channel,
                                  ADCRateType rate)
{

   if(reference != ADC_REF_INT)
    return EMBER_ERR_FATAL;

  // save the chosen configuration for this user
  adcConfig[id] = ( ((rate << ADC_PERIOD_BIT) & ADC_PERIOD)
                  | ((channel << ADC_CHAN_BIT) & ADC_CHAN)
                  | adcStaticConfig);

  // if the user already has a pending request, overwrite params
  if (adcPendingRequests & BIT(id)) {
    return EMBER_ADC_CONVERSION_DEFERRED;
  }

  ATOMIC (
    // otherwise, queue the transaction
    adcPendingRequests |= BIT(id);
    // try and start the conversion if there is not one happening
    adcReadingValid &= ~BIT(id);
  )
  if (startNextConversion() == id)
    return EMBER_ADC_CONVERSION_BUSY;
  else
    return EMBER_ADC_CONVERSION_DEFERRED;
}

#define NVIC_ENABLED_IRQ(IRQn) ((NVIC->ISER[(uint32_t)(IRQn) >> 5] & (1 << ((uint32_t)(IRQn) & 0x1F)))?1:0)

EmberStatus halRequestAdcData(ADCUser id, uint16_t *value)
{
  //Both the ADC interrupt and the global interrupt need to be enabled,
  //otherwise the ADC ISR cannot be serviced.
  bool intsAreOff = ( INTERRUPTS_ARE_OFF()
                        || !NVIC_ENABLED_IRQ(ADC_IRQn)
                        || !(EVENT_ADC->CFG & EVENT_ADC_CFG_ULDFULL) );
  EmberStatus stat;

  ATOMIC (
    // If interupts are disabled but the flag is set,
    // manually run the isr...
    //FIXME -= is this valid???
    if( intsAreOff
      && ( NVIC_ENABLED_IRQ(ADC_IRQn) && (EVENT_ADC->CFG & EVENT_ADC_CFG_ULDFULL) )) {
      halAdcIsr();
    }

    // check if we are done
    if (BIT(id) & adcReadingValid) {
      *value = adcReadings[id];
      adcReadingValid ^= BIT(id);
      stat = EMBER_ADC_CONVERSION_DONE;
    } else if (adcPendingRequests & BIT(id)) {
      stat = EMBER_ADC_CONVERSION_DEFERRED;
    } else if (adcPendingConversion == id) {
      stat = EMBER_ADC_CONVERSION_BUSY;
    } else {
      stat = EMBER_ADC_NO_CONVERSION_PENDING;
    }
  )
  return stat;
}

EmberStatus halReadAdcBlocking(ADCUser id, uint16_t *value)
{
  EmberStatus stat;

  do {
    stat = halRequestAdcData(id, value);
    if (stat == EMBER_ADC_NO_CONVERSION_PENDING)
      break;
  } while(stat != EMBER_ADC_CONVERSION_DONE);
  return stat;
}

EmberStatus halAdcCalibrate(ADCUser id)
{
  EmberStatus stat;

  halStartAdcConversion(id,
                        ADC_REF_INT,
                        ADC_SOURCE_GND_VREF2,
                        ADC_CONVERSION_TIME_US_4096);
  stat = halReadAdcBlocking(id, (uint16_t *)(&Nvss));
  if (stat == EMBER_ADC_CONVERSION_DONE) {
    halStartAdcConversion(id,
                          ADC_REF_INT,
                          ADC_SOURCE_VREG2_VREF2,
                          ADC_CONVERSION_TIME_US_4096);
    stat = halReadAdcBlocking(id, (uint16_t *)(&Nvdd));
  }
  if (stat == EMBER_ADC_CONVERSION_DONE) {
    Nvdd -= Nvss;
    adcCalibrated = true;
  } else {
    adcCalibrated = false;
    stat = EMBER_ERR_FATAL;
  }
  return stat;
}

// Use the ratio of the sample reading to the of VDD_PADSA/2, a 'known'
// value (nominally 900mV in normal mode, but slightly higher in boost mode)
// to convert to 100uV units.
// FIXME: support external Vref
//        use #define of Vref, ignore VDD_PADSA
int32_t halConvertValueToVolts(uint16_t value)
{
  int32_t N;
  int32_t V;
  int32_t nvalue;

  if (!adcCalibrated) {
    halAdcCalibrate(ADC_USER_LQI);
  }
  if (adcCalibrated) {
    assert(Nvdd);
    nvalue = value - Nvss;
    // Convert input value (minus ground) to a fraction of VDD/2.
    N = ((nvalue << 16) + Nvdd/2) / Nvdd;
    // Calculate voltage with: V = (N * Vreg/2) / (2^16)
    // Mutiplying by Vreg/2*10 makes the result units of 100 uVolts
    // (in fixed point E-4 which allows for 13.5 bits vs millivolts
    // which is only 10.2 bits).
    V = (int32_t)((N*((int32_t)halInternalGetVreg())*5) >> 16);
  } else {
     V = -32768;
  }
  return V;
}
