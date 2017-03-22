/*
 * File: adc-efr32.c
 * Description: EFR32-specific ADC HAL functions
 *
 *
 * Copyright 2008 by Ember Corporation. All rights reserved.                *80*
 */
#include PLATFORM_HEADER
#include "stack/include/error.h"
#include "hal/micro/micro-common.h"
#include "hal/micro/cortexm3/micro-common.h"
#include "plugin/adc/adc.h"

#include <limits.h>
#include "em_cmu.h"
#include "em_adc.h"
#include "em_bus.h"

#if (NUM_ADC_USERS > 8)
  #error NUM_ADC_USERS must not be greater than 8, or uint8_t variables in adc-efr32.c must be changed
#endif

#if defined(CORTEXM3_EFR32_MICRO)
// EFR-only ADC HAL

typedef struct
{
  ADC_AcqTime_TypeDef     acqTime;
  ADC_Ref_TypeDef         reference;
  uint8_t                 channel;
  bool                    diff;
} adcConfig_typeDef;

static bool              use1MHzClock = false;
static volatile uint8_t  adcPendingRequests;              /* Bitmap of pending requests */
static volatile uint8_t  adcPendingConversion;            /* ID of pending conversion */
static uint8_t           adcReadingValid;                 /* Bitmap of valid adcReadings */
static adcConfig_typeDef adcConfig[NUM_ADC_USERS];
static uint8_t           adcUserSel;
static uint16_t          adcReadings[NUM_ADC_USERS];

/* Fixed mapping of channel to APORT. Channel is 4 bits while there are 256 APORT channels available. */
const ADC_PosSel_TypeDef pChanAportPosSelMap[] =
                           {adcPosSelAVDD,        /* 0 - AVDD (single-ended only) */
                            adcPosSelAPORT3YCH27, /* 1 - PB11 (single-ended or diff) */
                            adcPosSelAPORT3XCH28, /* 2 - PB12 (single-ended or diff) */
                            adcPosSelAPORT3YCH5,  /* 3 - PB13 (single-ended or diff) */
                            adcPosSelAPORT2YCH6,  /* 4 - PC6  (single-ended or diff) */
                            adcPosSelAPORT3XCH12, /* 5 - PA4  (single-ended or diff) */
                            adcPosSelAPORT3YCH13, /* 6 - PA5  (single-ended or diff) */
                           };
const ADC_NegSel_TypeDef nChanAportNegSelMap[] =
                           {adcNegSelVSS,         /* 0 - GND  (single-ended only) */
                            adcNegSelAPORT3YCH27, /* 1 - PB11 (single-ended or diff) */
                            adcNegSelAPORT3XCH28, /* 2 - PB12 (single-ended or diff) */
                            adcNegSelAPORT3YCH5,  /* 3 - PB13 (single-ended or diff) */
                            adcNegSelAPORT2YCH6,  /* 4 - PC6  (single-ended or diff) */
                            adcNegSelAPORT3XCH12, /* 5 - PA4  (single-ended or diff) */
                            adcNegSelAPORT3YCH13, /* 6 - PA5  (single-ended or diff) */
                           };


ADC_PosSel_TypeDef pChanToPosSel(uint8_t pChan)
{
  if (pChan < sizeof(pChanAportPosSelMap))
  {
    return pChanAportPosSelMap[pChan];
  }
  else
  {
    EFM_ASSERT(false);
    return adcPosSelDEFAULT;
  }
}

ADC_NegSel_TypeDef nChanToNegSel(uint8_t nChan)
{
  if (nChan < sizeof(nChanAportNegSelMap))
  {
    return nChanAportNegSelMap[nChan];
  }
  else
  {
    EFM_ASSERT(false);
    return adcNegSelDEFAULT;
  }
}


void halAdcSetClock(bool slow)
{
  use1MHzClock = slow;
}

bool halAdcGetClock(void)
{
  return use1MHzClock;
}

/* Set user configuration and start ADC */
void adcInitStart(ADC_InitSingle_TypeDef *initSingle, uint8_t adcUser)
{
  initSingle->posSel      = pChanToPosSel(adcConfig[adcUser].channel >> 4);
  initSingle->negSel      = nChanToNegSel(adcConfig[adcUser].channel & 0xF);

  /* Configure for diff mode if negSel is not GND (VSS) */
  adcConfig[adcUser].diff = (initSingle->negSel != adcNegSelVSS);
  initSingle->diff        = adcConfig[adcUser].diff;

  initSingle->acqTime     = adcConfig[adcUser].acqTime;
  initSingle->reference   = adcConfig[adcUser].reference;

  ADC_InitSingle(ADC0, initSingle);
  ADC_Start(ADC0, adcStartSingle);
}


void ADC0_IRQHandler(void)
{
  ADC_InitSingle_TypeDef initSingle = ADC_INITSINGLE_DEFAULT;
  uint8_t i;
  uint8_t conversion = adcPendingConversion;
  uint16_t adcSample;

  /* make sure data is ready and the desired conversion is valid */
  if ( (ADC_IntGetEnabled(ADC0) & ADC_IF_SINGLE) && (conversion < NUM_ADC_USERS) )
  {
    adcSample = (uint16_t)ADC0->SINGLEDATA;
    if ((adcConfig[conversion].diff) && ((int16_t)adcSample < 0))
    {
      /* ADC data in diff mode is 2's complement, but this API does not support
         both single-ended and differential measurements. Set result to zero if
         diff mode and V_nchan > V_pchan. */
      adcReadings[conversion] = 0;
    }
    else
    {
      adcReadings[conversion] = adcSample;
    }

    /* mark the reading as valid */
    adcReadingValid |= BIT(conversion);
    /* setup the next conversion if any */
    if (adcPendingRequests)
    {
      for (i = 0; i < NUM_ADC_USERS; i++)
      {
        if (BIT(i) & adcPendingRequests)
        {
          adcPendingConversion = i;     /* set pending conversion */
          adcPendingRequests ^= BIT(i); /* clear request: conversion is starting */

          adcInitStart(&initSingle, i);

          /* conversion started, so we're done here (only one at a time) */
          break;
        }
      }
    }
    else
    {
      /* nothing pending, so go "idle" */
      adcPendingConversion = NUM_ADC_USERS;
    }
  }
}

/* An internal support routine called from functions below.
 * Returns the user number of the started conversion, or NUM_ADC_USERS
 * otherwise. */
static ADCUser startNextConversion(void)
{
  ADC_InitSingle_TypeDef initSingle = ADC_INITSINGLE_DEFAULT;
  uint8_t i;

  ATOMIC (
    /* start the next requested conversion if any */
    if (adcPendingRequests && !(ADC0->STATUS & ADC_STATUS_SINGLEACT)) {
      for (i = 0; i < NUM_ADC_USERS; i++) {
        if ( BIT(i) & adcPendingRequests) {
          adcPendingConversion = i;     /* set pending conversion */
          adcPendingRequests ^= BIT(i); /* clear request */

          adcInitStart(&initSingle, i);

          break;
        }
      }
    }
    else
    {
      i = NUM_ADC_USERS;
    }
  )
  return i;
}

void halInternalInitAdc(void)
{
  ADC_Init_TypeDef init = ADC_INIT_DEFAULT;

  if (use1MHzClock)
  {
    init.prescale = ADC_PrescaleCalc(1000000, 0);
  }
  else
  {
    /* Use MAX ADC clock */
    init.prescale = ADC_PrescaleCalc(UINT32_MAX, 0);
  }

  CMU_ClockEnable(cmuClock_ADC0, true);
  ADC_Init(ADC0, &init);

  for (adcUserSel = 0; adcUserSel < NUM_ADC_USERS; adcUserSel++)
  {
    /* pChan = 0 (VDD), nChan = 0 (GND) */
    adcConfig[adcUserSel].channel   = 0;
    adcConfig[adcUserSel].acqTime   = adcAcqTime1;
    adcConfig[adcUserSel].diff      = false;
    adcConfig[adcUserSel].reference = adcRef5VDIFF;
  }

  adcUserSel = 0;
  adcPendingRequests = 0;
  adcPendingConversion = 0;
  adcReadingValid = 0;

  /* Enable SINGLE data interrupt */
  NVIC_ClearPendingIRQ(ADC0_IRQn);
  NVIC_EnableIRQ(ADC0_IRQn);
  ADC_IntEnable(ADC0, ADC_IF_SINGLE);
}

void halInternalSleepAdc(void)
{
  ADC_Reset(ADC0);
  NVIC_DisableIRQ(ADC0_IRQn);
  ADC_IntDisable(ADC0, ADC_IF_SINGLE);
}

EmberStatus halStartAdcConversion(ADCUser id,
                                  ADCReferenceType reference,
                                  ADCChannelType channel,
                                  ADCRateType rate)
{
  /* Save config for this ID */
  adcConfig[id].channel = channel;

  if (reference <= adcRefP)
  {
    adcConfig[id].reference = (ADC_Ref_TypeDef)reference;
  }
  else
  {
    return EMBER_ERR_FATAL;
  }

  if (rate <= adcAcqTime256)
  {
    adcConfig[id].acqTime = (ADC_AcqTime_TypeDef)rate;
  }
  else
  {
    return EMBER_ERR_FATAL;
  }


  /* if the user already has a pending request, overwrite params */
  if (adcPendingRequests & BIT(id)) {
    return EMBER_ADC_CONVERSION_DEFERRED;
  }

  ATOMIC (
    /* otherwise, queue the transaction */
    adcPendingRequests |= BIT(id);
    /* try and start the conversion if there is not one happening */
    adcReadingValid &= ~BIT(id);
  )
  if (startNextConversion() == id)
  {
    return EMBER_ADC_CONVERSION_BUSY;
  }
  else
  {
    return EMBER_ADC_CONVERSION_DEFERRED;
  }
}

EmberStatus halRequestAdcData(ADCUser id, uint16_t *value)
{
  // If interrupts are disabled, the ADC ISR cannot be serviced
  bool intsAreOff = INTERRUPTS_ARE_OFF();
  EmberStatus status;

  ATOMIC (
    // If interupts are disabled but the flag is set, manually run the ISR
    if (intsAreOff && (ADC_IntGet(ADC0) & ADC_IF_SINGLE))
    {
      ADC0_IRQHandler();
    }

    /* check if we are done */
    if (BIT(id) & adcReadingValid)
    {
      *value = adcReadings[id];
      adcReadingValid ^= BIT(id);
      status = EMBER_ADC_CONVERSION_DONE;
    }
    else if (adcPendingRequests & BIT(id))
    {
      status = EMBER_ADC_CONVERSION_DEFERRED;
    }
    else if (adcPendingConversion == id)
    {
      status = EMBER_ADC_CONVERSION_BUSY;
    }
    else
    {
      status = EMBER_ADC_NO_CONVERSION_PENDING;
    }
  )
  return status;
}

EmberStatus halReadAdcBlocking(ADCUser id, uint16_t *value)
{
  EmberStatus status;

  do {
    status = halRequestAdcData(id, value);
    if (status == EMBER_ADC_NO_CONVERSION_PENDING)
      break;
  } while(status != EMBER_ADC_CONVERSION_DONE);
  return status;
}

EmberStatus halAdcCalibrate(ADCUser id)
{
  /* ADC is calibrated out of reset */
  (void)id;
  return 0;
}

int32_t halConvertValueToVolts(uint16_t value)
{
#if (ADC_REF_INT == adcRef5VDIFF) && !defined(VFS_FACTOR)
#define REF_VFS_FACTOR      50000UL
#else
#error "REF_VFS_FACTOR is udefined"
#endif

  int32_t volts;
  volts = (int32_t)(((uint32_t)value * REF_VFS_FACTOR) / 4096UL);
  return volts;
}

#endif // CORTEXM3_EFR32_MICRO
