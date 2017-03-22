/***************************************************************************//**
 * @file  lightsensefft.c
 * @brief FFT transform example
 * @details
 *   Use ADC/TIMER/DMA/PRS in order to capture and analyze input from the
 *   light sensor on the STK. Runs floating point FFT algorithm from the CMSIS
 *   DSP Library, and estimate the frequency of the most luminous light source
 *   using sinc interpolation. The main point with this example is to show the
 *   use of the CMSIS DSP library and the floating point capability of the cpu.
 *
 * @par Usage
 *   Connect the light sensor output to the ADC input by shorting pins
 *   15 and 14 on the EXP_HEADER of the STK.
 *   Direct various light sources to the light sensor. Expect no specific
 *   frequency from daylight or from a flashlight. Mains powered incandescent
 *   bulbs should give twice the mains frequency. Using another STK running the
 *   "blink" example modified to various blink rates is an excellent signal
 *   source. The frequency bandwidth is approximately 10-500 Hz.
 *   The frequency shows in the 4 digit numerical display upper right on
 *   the LCD. The LCD also displays the number of cpu cycles used to do
 *   the FFT transform.
 *
 * @version 5.1.2
 *******************************************************************************
 * @section License
 * <b>Copyright 2015 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/


#include "bsp.h"
#include "bsp_trace.h"
#include "em_common.h"
#include "em_emu.h"
#include "em_cmu.h"
#include "em_chip.h"
#include "em_adc.h"
#include "em_gpio.h"
#include "em_prs.h"
#include "em_timer.h"
#include "em_dma.h"
#include "dmactrl.h"
#include "segmentlcd.h"
#include "arm_math.h"
#include "math.h"

/*
 * Light sensor sampling:
 * ----------------------
 *
 *
 *                                | Light sensor
 *                                V
 * +------+     +-----+       +------+       +-----+         +--------+
 * |TIMER0|---->| PRS |--+--->| ADC0 |------>| DMA |-------->| Buffer |
 * +------+     +-----+       +------+       +-----+         +--------+
 *
 *
 * 1. TIMER0 is set to overflow appr. SAMPLE_RATE times per
 *    second, triggering a pulse to the PRS module on each overflow.
 *
 * 2. ADC0 is configured to trigger a sample when a PRS pulse occurs.
 *
 * 3. The DMA is using a ping-pong transfer type to receive sampled data from the
 *    ADC0 into a buffer. When the specified number of samples has been
 *    received, the DMA will trigger an interrupt and switch sampling into an
 *    alternate buffer. The DMA interrupt handler only refreshes the use of
 *    the current buffer, and transfers the data into an array for the main
 *    function to process through FFT.
 *
 * Energy mode usage:
 * ------------------
 *
 * Due to the relatively high sampling rate, we need to keep the ADC warmed,
 * and only EM1 may be used when idle.
 */

/**
 * Number of samples processed at a time. This number has to be equal to one
 * of the accepted input sizes of the rfft transform of the CMSIS DSP library.
 * Increasing it gives better resolution in the frequency, but also a longer
 * sampling time.
 */
#define BUFFER_SAMPLES                  512

/** (Approximate) sample rate used for sampling data. */
#define SAMPLE_RATE                     (1024)

/** DMA channel used for ADC samples. */
#define DMA_CHANNEL                     0

/** PRS channel used by TIMER to trigger ADC activity. */
#define PRS_CHANNEL                     0

/** The GPIO pin used to power the light sensor. */
#define EXCITE_PORT   gpioPortD
#define EXCITE_PIN    6

/** Primary sample buffer. */
static uint16_t lightInBuffer1[BUFFER_SAMPLES];
/** Alternate sample buffer. */
static uint16_t lightInBuffer2[BUFFER_SAMPLES];

/** Buffer of uint16_t sample values ready to be FFT-ed. */
static uint16_t lightToFFTBuffer[BUFFER_SAMPLES];

/** Buffer of float samples ready for FFT. */
static float32_t floatBuf[BUFFER_SAMPLES];

/** Complex (interleaved) output from FFT. */
static float32_t fftOutputComplex[BUFFER_SAMPLES * 2];

/** Magnitude of complex numbers in FFT output. */
static float32_t fftOutputMag[BUFFER_SAMPLES];

/** Callback config for DMA handling, must remain 'live'. */
static DMA_CB_TypeDef cbInData;

/** Flag used to indicate whether data is ready for processing */
static volatile bool dataReadyForFFT;
/** Indicate whether we are currently processing data through FFT */
static volatile bool processingFFT;

/** Instance structures for float32_t RFFT */
static arm_rfft_instance_f32 rfft_instance;
/** Instance structure for float32_t CFFT used by the RFFT */
static arm_cfft_radix4_instance_f32 cfft_instance;

/***************************************************************************//**
* @brief
*   Callback invoked from DMA interrupt handler when DMA transfer has filled
*   a input buffer. (I.e. received from ADC.)
*******************************************************************************/
static void LightDMAInCb(unsigned int channel, bool primary, void *user)
{
  (void) user; /* Unused parameter */

  uint16_t *inBuf;

  inBuf = primary ? lightInBuffer1 : lightInBuffer2;

  /* Copy the recieved samples unless we are currently processing */
  if (!processingFFT)
  {
    memcpy(lightToFFTBuffer, inBuf, BUFFER_SAMPLES * 2);
    dataReadyForFFT = true;
  }

  /* Refresh DMA for using this buffer. DMA ping-pong will */
  /* halt if buffer not refreshed in time. */
  DMA_RefreshPingPong(channel,
                      primary,
                      false,
                      NULL,
                      NULL,
                      (BUFFER_SAMPLES) - 1,
                      false);
}

/***************************************************************************//**
* @brief
*   Configure ADC usage for this application.
*******************************************************************************/
static void LightADCConfig(void)
{
  DMA_CfgDescr_TypeDef   descrCfg;
  DMA_CfgChannel_TypeDef chnlCfg;
  ADC_Init_TypeDef       init       = ADC_INIT_DEFAULT;
  ADC_InitSingle_TypeDef singleInit = ADC_INITSINGLE_DEFAULT;

  /* Configure DMA usage by ADC */

  cbInData.cbFunc  = LightDMAInCb;
  cbInData.userPtr = NULL;

  chnlCfg.highPri   = true;
  chnlCfg.enableInt = true;
  chnlCfg.select    = DMAREQ_ADC0_SINGLE;
  chnlCfg.cb        = &cbInData;
  DMA_CfgChannel(DMA_CHANNEL, &chnlCfg);

  descrCfg.dstInc  = dmaDataInc2;
  descrCfg.srcInc  = dmaDataIncNone;
  descrCfg.size    = dmaDataSize2;
  descrCfg.arbRate = dmaArbitrate1;
  descrCfg.hprot   = 0;
  DMA_CfgDescr(DMA_CHANNEL, true, &descrCfg);
  DMA_CfgDescr(DMA_CHANNEL, false, &descrCfg);

  DMA_ActivatePingPong(DMA_CHANNEL,
                       false,
                       lightInBuffer1,
                       (void *)((uint32_t) &(ADC0->SINGLEDATA)),
                       (BUFFER_SAMPLES) - 1,
                       lightInBuffer2,
                       (void *)((uint32_t) &(ADC0->SINGLEDATA)),
                       (BUFFER_SAMPLES) - 1);

  /* Configure ADC */

  /* Keep warm due to "high" frequency sampling */
  init.warmUpMode = adcWarmupKeepADCWarm;
  /* Init common issues for both single conversion and scan mode */
  init.timebase = ADC_TimebaseCalc(0);
  init.prescale = ADC_PrescaleCalc(4000000, 0);
  ADC_Init(ADC0, &init);

  /* Init for single conversion. */
  singleInit.prsSel     = adcPRSSELCh0;
  singleInit.reference  = adcRefVDD;
  singleInit.input      = adcSingleInpCh5;
  singleInit.prsEnable  = true;
  ADC_InitSingle(ADC0, &singleInit);
}

/***************************************************************************//**
* @brief
*   Configure PRS usage for this application.
*
* @param[in] prsChannel
*   PRS channel to use.
*******************************************************************************/
static void LightPRSConfig(unsigned int prsChannel)
{
  PRS_LevelSet(0, 1 << (prsChannel + _PRS_SWLEVEL_CH0LEVEL_SHIFT));
  PRS_SourceSignalSet(prsChannel,
                      PRS_CH_CTRL_SOURCESEL_TIMER0,
                      PRS_CH_CTRL_SIGSEL_TIMER0OF,
                      prsEdgePos);
}

/***************************************************************************//**
* @brief
*   Process the sampled data through FFT.
*******************************************************************************/
void ProcessFFT(void)
{
  uint16_t        *inBuf;
  int32_t         value;
  int             i;

  inBuf = lightToFFTBuffer;

  /*
   * Convert to float values.
   */
  for (i = 0; i < BUFFER_SAMPLES; ++i)
  {
    value = (int32_t)*inBuf++;
    floatBuf[i] = (float32_t)value;
  }

  /* Process the data through the RFFT module, resulting complex output is
   * stored in fftOutputComplex
   */
  arm_rfft_f32(&rfft_instance, floatBuf, fftOutputComplex);

  /* Compute the magnitude of all the resulting complex numbers */
  arm_cmplx_mag_f32(fftOutputComplex,
                    fftOutputMag,
                    BUFFER_SAMPLES);
}

/***************************************************************************//**
* @brief
*   Find the maximal bin and estimate the frequency using sinc interpolation.
* @return
*   Frequency of maximal peak
*******************************************************************************/
float32_t GetFreq(void)
{
  float32_t maxVal;
  uint32_t maxIndex;

  /* Real and imag components of maximal bin and bins on each side */
  float32_t rz_p, iz_p, rz_n, iz_n, rz_0, iz_0;
  /* Small correction to the "index" of the maximal bin */
  float32_t deltaIndex;
  /* Real and imag components of the intermediate result */
  float32_t a, b, c, d;

  #define START_INDEX 4
  /* Find the biggest bin, disregarding the first bins because of DC offset and
   * low frequency noise.
   */
  arm_max_f32(&fftOutputMag[START_INDEX],
              BUFFER_SAMPLES / 2 - START_INDEX,
              &maxVal,
              &maxIndex);

  maxIndex += START_INDEX;

  /* Perform sinc() interpolation using the two bins on each side of the
   * maximal bin. For more information see page 113 of
   * http://tmo.jpl.nasa.gov/progress_report/42-118/118I.pdf
   */

  /* z_{peak} */
  rz_0 = fftOutputComplex[maxIndex * 2];
  iz_0 = fftOutputComplex[maxIndex * 2 + 1];

  /* z_{peak+1} */
  rz_p = fftOutputComplex[maxIndex * 2 + 2];
  iz_p = fftOutputComplex[maxIndex * 2 + 2 + 1];

  /* z_{peak-1} */
  rz_n = fftOutputComplex[maxIndex * 2 - 2];
  iz_n = fftOutputComplex[maxIndex * 2 - 2 + 1];

  /* z_{peak+1} - z_{peak-1} */
  a = rz_p - rz_n;
  b = iz_p - iz_n;
  /* z_{peak+1} + z_{peak-1} - 2*z_{peak} */
  c = rz_p + rz_n - (float32_t)2.0 * rz_0;
  d = iz_p + iz_n - (float32_t)2.0 * iz_0;

  /* Re (z_{peak+1} - z_{peak-1}) / (z_{peak+1} + z_{peak-1} - 2*z_{peak}) */
  deltaIndex = (a*c + b*d) / (c*c + d*d);

  return ((float32_t)maxIndex + deltaIndex)
          * (float32_t)SAMPLE_RATE
          / (float32_t)BUFFER_SAMPLES;
}

/***************************************************************************//**
* @brief
*   Main function. Setup ADC, FFT, clocks, PRS, DMA, Timer,
*   and process FFT forever.
*******************************************************************************/
int main(void)
{
  uint32_t    time;
  arm_status  status;

  DMA_Init_TypeDef   dmaInit;
  TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;

  /* Chip errata */
  CHIP_Init();

  /* If first word of user data page is non-zero, enable eA Profiler trace */
  BSP_TraceProfilerSetup();

  /* Enable LCD without voltage boost */
  SegmentLCD_Init(false);
  SegmentLCD_Symbol(LCD_SYMBOL_GECKO, 1);
  SegmentLCD_Symbol(LCD_SYMBOL_EFM32, 1);

  /* Initialize the CFFT/CIFFT module */
  status = arm_rfft_init_f32(&rfft_instance,
                             &cfft_instance,
                             BUFFER_SAMPLES,
                             0,  /* forward transform */
                             1); /* normal, not bitreversed, order */

  if (status != ARM_MATH_SUCCESS) {
    /* Error initializing RFFT module. */
    SegmentLCD_Write(" Error ");
    while (1) ;
  }

  dataReadyForFFT = false;
  processingFFT   = false;

  /* Use the HFXO to maximize processing power. */
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);

  /* Enable clocks required */
  CMU_ClockEnable(cmuClock_HFPER, true);
  CMU_ClockEnable(cmuClock_ADC0, true);
  CMU_ClockEnable(cmuClock_PRS, true);
  CMU_ClockEnable(cmuClock_DMA, true);
  CMU_ClockEnable(cmuClock_TIMER0, true);
  CMU_ClockEnable(cmuClock_GPIO, true);

  /* Power the light sensor with GPIO. */
  GPIO_PinModeSet( EXCITE_PORT, EXCITE_PIN, gpioModePushPull, 1);

  NVIC_SetPriority(DMA_IRQn, 0); /* Highest priority */

  /* Configure peripheral reflex system used by TIMER to trigger ADC. */
  LightPRSConfig(PRS_CHANNEL);

  /* Configure general DMA issues */
  dmaInit.hprot        = 0;
  dmaInit.controlBlock = dmaControlBlock;
  DMA_Init(&dmaInit);

  /* Configure ADC. */
  LightADCConfig();

  /* Trigger sampling according to configured sample rate */
  TIMER_TopSet(TIMER0, CMU_ClockFreqGet(cmuClock_HFPER) / SAMPLE_RATE);
  TIMER_Init(TIMER0, &timerInit);

  /* Enable DWT */
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  /* Make sure CYCCNT is running */
  DWT->CTRL |= 1;

  while (1)
  {
    while (dataReadyForFFT)
    {
      processingFFT = true;

      /* Do FFT, measure number of cpu cycles used. */
      time = DWT->CYCCNT;
      ProcessFFT();
      time = DWT->CYCCNT - time;

      dataReadyForFFT = false;
      processingFFT = false;

      /* Display dominant frequency. */
      SegmentLCD_Number( (int)GetFreq() );

      /* Display cpu cycle count used to do FFT. */
      SegmentLCD_LowerNumber( (int)time );
    }
    EMU_EnterEM1();
  }
}
