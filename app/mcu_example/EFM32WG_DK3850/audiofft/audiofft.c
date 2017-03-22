/**************************************************************************//**
 * @file   audiofft.c
 * @brief  Audio FFT example using emWin for plotting of FFT result.
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

#include "plot.h"
#include "GUI.h"
#include <stdint.h>
#include <stdbool.h>
#include "em_adc.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_device.h"
#include "em_dma.h"
#include "em_ebi.h"
#include "em_gpio.h"
#include "em_prs.h"
#include "em_timer.h"
#include "bsp.h"
#include "bsp_trace.h"
#include "dmactrl.h"
#include "tftdirect.h"
#include "arm_math.h"

/*
 * Audio handling:
 * ---------------
 *
 *
 *                                | Audio in
 *                                V
 * +------+     +-----+       +------+       +-----+         +--------+
 * |TIMER0|---->| PRS |--+--->| ADC0 |------>| DMA |-------->| Buffer |
 * +------+     +-----+       +------+       +-----+         +--------+
 *
 *
 * 1. TIMER0 is set to overflow appr.AUDIO_SAMPLE_RATE times per seconds,
 *    triggering a pulse to the PRS module on each overflow.
 *
 * 2. ADC0 is configured to trigger a scan sequence (left+right channels) when
 *    a PRS pulse occurs.
 *
 * 3. The DMA is using a ping-pong transfer type to receive sampled data from
 *    the ADC0 into a buffer (right and left channel interleaved). When the
 *    specified number of samples has been received, the DMA will trigger an
 *    interrupt and switch sampling into an alternate buffer. The DMA interrupt
 *    handler only refreshes the use of the current buffer, and transfers the
 *    data into an array for the main function to process with FFT.
 *
 * Energy mode usage:
 * ------------------
 *
 * Due to the relatively high sampling rate, we need to keep the ADC warmed,
 * and only EM1 may be used when idle.
 */

/**
 * Number of samples for each channel processed at a time. This number has to be
 * equal to one of the accepted input sizes of the rfft transform of the CMSIS
 * DSP library. Increasing it gives better resolution in the frequency, but also
 * a longer sampling time.
 */
#define AUDIO_BUFFER_SAMPLES    512

/** (Approximate) sample rate used for processing audio data. */
#define AUDIO_SAMPLE_RATE       (1024*8)

/** DMA channel used for audio in scan sequence (both right and left channel) */
#define DMA_CHANNEL             0

/** PRS channel used by TIMER to trigger ADC activity. */
#define PRS_CHANNEL             0

/** Primary audio in buffer, holding both left and right channel (interleaved) */
static uint16_t audioInBuffer1[AUDIO_BUFFER_SAMPLES * 2];
/** Alternate audio in buffer, holding both left and right channel (interleaved) */
static uint16_t audioInBuffer2[AUDIO_BUFFER_SAMPLES * 2];

/** Buffer of uint16_t audio input values ready to be FFT-ed */
static uint16_t audioToFFTBuffer[AUDIO_BUFFER_SAMPLES * 2];

/** Buffer of righ-left-averaged float samples ready for FFT */
static float32_t floatBuf1[AUDIO_BUFFER_SAMPLES];

/** Complex (interleaved) output from FFT */
static float32_t fftOutputComplex[AUDIO_BUFFER_SAMPLES * 2];

/** Magnitude of complex numbers in FFT output */
static float32_t fftOutputMag[AUDIO_BUFFER_SAMPLES];

/** Callback config for audio-in DMA handling, must remain 'live' */
static DMA_CB_TypeDef cbInData;

/** Flag used to indicate whether data is ready for processing */
static volatile bool dataReadyForFFT;
/** Indicate whether we are currently processing data through FFT */
static volatile bool processingFFT;

/** Instance structure for float32_t RFFT */
static arm_rfft_instance_f32 rfft_instance;
/** Instance structure for float32_t CFFT used by the RFFT */
static arm_cfft_radix4_instance_f32 cfft_instance;

static void Plot( float32_t *data );

/***************************************************************************//**
* @brief
*   Callback invoked from DMA interrupt handler when DMA transfer has filled
*   an audio in buffer. (I.e. received from ADC.)
*******************************************************************************/
static void DMAInCb(unsigned int channel, bool primary, void *user)
{
  (void) user; /* Unused parameter */

  uint16_t *inBuf;

  inBuf = primary ? audioInBuffer1 : audioInBuffer2;
  GPIO_PinOutToggle(gpioPortA,12);

  /* Copy the recieved samples unless we are currently processing */
  if (!processingFFT)
  {
    /* Two channels, and each sample is 2 bytes */
    memcpy(audioToFFTBuffer, inBuf, AUDIO_BUFFER_SAMPLES * 2 * 2);
    dataReadyForFFT = true;
  }

  /* Refresh DMA for using this buffer. DMA ping-pong will */
  /* halt if buffer not refreshed in time. */
  DMA_RefreshPingPong(channel,
                      primary,
                      false,
                      NULL,
                      NULL,
                      (AUDIO_BUFFER_SAMPLES * 2) - 1,
                      false);
}

/***************************************************************************//**
* @brief
*   Configure ADC usage for this application.
*******************************************************************************/
static void ADCConfig(void)
{
  DMA_CfgDescr_TypeDef   descrCfg;
  DMA_CfgChannel_TypeDef chnlCfg;
  ADC_Init_TypeDef       init     = ADC_INIT_DEFAULT;
  ADC_InitScan_TypeDef   scanInit = ADC_INITSCAN_DEFAULT;

  /* Configure DMA usage by ADC */

  cbInData.cbFunc  = DMAInCb;
  cbInData.userPtr = NULL;

  chnlCfg.highPri   = true;
  chnlCfg.enableInt = true;
  chnlCfg.select    = DMAREQ_ADC0_SCAN;
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
                       audioInBuffer1,
                       (void *)((uint32_t) &(ADC0->SCANDATA)),
                       (AUDIO_BUFFER_SAMPLES * 2) - 1,
                       audioInBuffer2,
                       (void *)((uint32_t) &(ADC0->SCANDATA)),
                       (AUDIO_BUFFER_SAMPLES * 2) - 1);

  /* Configure ADC */

  /* Keep warm due to "high" frequency sampling */
  init.warmUpMode = adcWarmupKeepADCWarm;
  /* Init common issues for both single conversion and scan mode */
  init.timebase = ADC_TimebaseCalc(0);
  init.prescale = ADC_PrescaleCalc(4000000, 0);
  /* Sample potentiometer by tailgating in order to not disturb fixed rate */
  /* audio sampling. */
  init.tailgate = true;
  ADC_Init(ADC0, &init);

  /* Init for scan sequence use (audio in right/left channels). */
  scanInit.prsSel    = adcPRSSELCh0;
  scanInit.prsEnable = true;
  scanInit.reference = adcRefVDD;
  scanInit.input     = ADC_SCANCTRL_INPUTMASK_CH6 | ADC_SCANCTRL_INPUTMASK_CH7;
  ADC_InitScan(ADC0, &scanInit);
}

/***************************************************************************//**
* @brief
*   Configure PRS usage for this application.
*
* @param[in] prsChannel
*   PRS channel to use.
*******************************************************************************/
static void PRSConfig(unsigned int prsChannel)
{
  PRS_LevelSet(0, 1 << (prsChannel + _PRS_SWLEVEL_CH0LEVEL_SHIFT));
  PRS_SourceSignalSet(prsChannel,
                      PRS_CH_CTRL_SOURCESEL_TIMER0,
                      PRS_CH_CTRL_SIGSEL_TIMER0OF,
                      prsEdgePos);
}

/***************************************************************************//**
* @brief
*   Process the sampled audio data through FFT.
*******************************************************************************/
void processFFT(void)
{
  uint16_t        *inBuf;
  int32_t         right;
  int32_t         left;
  int             i;

  inBuf = audioToFFTBuffer;

  /*
   * Average the left and right channels into one combined floating point buffer
   */
  for (i = 0; i < AUDIO_BUFFER_SAMPLES; ++i)
  {
    right = (int32_t) *inBuf++;
    left  = (int32_t) *inBuf++;

    floatBuf1[i] = (float32_t)(((float32_t)right + (float32_t)left) / 2.0f);
  }

  /* Process the data through the RFFT module, resulting complex output is
   * stored in fftOutputComplex
   */
  arm_rfft_f32(&rfft_instance, floatBuf1, fftOutputComplex);

  /* Compute the magnitude of all the resulting complex numbers */
  arm_cmplx_mag_f32(fftOutputComplex,
                    fftOutputMag,
                    AUDIO_BUFFER_SAMPLES);
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  int                 i;
  char                buf[80];
  uint32_t            time;
  arm_status          status;
  DMA_Init_TypeDef    dmaInit;
  TIMER_Init_TypeDef  timerInit = TIMER_INIT_DEFAULT;

  dataReadyForFFT = false;
  processingFFT   = false;

  /* Chip errata */
  CHIP_Init();

  /* Configure for 48MHz HFXO operation of core clock */
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);

  /* Enable SysTick interrupt, used by GUI software timer */
  if (SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 1000)) while (1);

  /* Initialize EBI configuration for external RAM and display controller */
  BSP_Init(BSP_INIT_DK_EBI);

  /* Connect audio in to ADC */
  BSP_PeripheralAccess(BSP_AUDIO_IN, true);

  /* Initialize the CFFT/CIFFT module */
  status = arm_rfft_init_f32(&rfft_instance,
                             &cfft_instance,
                             AUDIO_BUFFER_SAMPLES,
                             0,  /* forward transform */
                             1); /* normal, not bitreversed, order */

  if (status != ARM_MATH_SUCCESS) {
    /* Error initializing RFFT module. */
    while (1) ;
  }

  /* Enable clocks required */
  CMU_ClockEnable(cmuClock_HFPER, true);
  CMU_ClockEnable(cmuClock_ADC0, true);
  CMU_ClockEnable(cmuClock_PRS, true);
  CMU_ClockEnable(cmuClock_DMA, true);
  CMU_ClockEnable(cmuClock_TIMER0, true);
  CMU_ClockEnable(cmuClock_GPIO, true);

  GPIO_PinModeSet(gpioPortA,12, gpioModePushPull, 0);

  NVIC_SetPriority(DMA_IRQn, 0); /* Highest priority */

  /* Configure peripheral reflex system used by TIMER to trigger ADC/DAC */
  PRSConfig(PRS_CHANNEL);

  /* Configure DMA */
  dmaInit.hprot        = 0;
  dmaInit.controlBlock = dmaControlBlock;
  DMA_Init(&dmaInit);

  /* Configure ADC used for audio-in */
  ADCConfig();

  /* Trigger sampling according to configured sample rate */
  TIMER_TopSet(TIMER0, CMU_ClockFreqGet(cmuClock_HFPER) / AUDIO_SAMPLE_RATE);
  TIMER_Init(TIMER0, &timerInit);

  /* Enable DWT */
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  /* Make sure CYCCNT is running */
  DWT->CTRL |= 1;

  /* Indicate we are waiting for AEM button state "EFM". */
  BSP_LedsSet(0x8001);
  while (BSP_RegisterRead(&BC_REGISTER->UIF_AEM) != BC_UIF_AEM_EFM)
  {
    /* Show a short "strobe light" on DK LED's, indicating wait. */
    BSP_LedsSet(0x8001);
    GUI_X_Delay(200);
    BSP_LedsSet(0x4002);
    GUI_X_Delay(50);
  }

  /* Initialize emWin Library. */
  GUI_Init();

  /* Initialize PLOT module. */
  PLOT_Init(AUDIO_BUFFER_SAMPLES / 2);
  for (i=0; i<GUI_MULTIBUF_GetNumBuffers(); i++)
  {
    /* Make sure that static text is written to all buffers. */
    GUI_MULTIBUF_Begin();
    PLOT_Puts( "512 pt FFT calculation time", 160, 5 );
    PLOT_Puts( "in cpu-cycles:", 160, 20 );
    GUI_MULTIBUF_End();
  }

  while (1)
  {
    GUI_X_Delay(0);           /* Takes care of AEM button switching. */

    if (dataReadyForFFT)
    {
      processingFFT = true;

      time = DWT->CYCCNT;
      processFFT();
      time = DWT->CYCCNT - time;

      dataReadyForFFT = false;
      processingFFT = false;

      GPIO_PinOutToggle(gpioPortA,12);

      sprintf(buf, "%7ld", time);

      GUI_MULTIBUF_Begin();
      PLOT_Puts( buf, 160, 38 );
      Plot( fftOutputMag );
      GUI_MULTIBUF_End();
    }
  }
}

/**************************************************************************//**
 * @brief  Plots data using PLOT module.
 *****************************************************************************/
static void Plot( float32_t *data )
{
  data[0] = 0;    /* Zero first samples, i.e. ignore DC level. */
  data[1] = 0;

  GPIO_PinOutToggle(gpioPortA,12);
  PLOT_Plot( data );
  GPIO_PinOutToggle(gpioPortA,12);
}
