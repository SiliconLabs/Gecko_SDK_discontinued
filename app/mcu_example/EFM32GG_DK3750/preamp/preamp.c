/***************************************************************************//**
 * @file
 * @brief Preamplifier example for EFM32GG_DK3750
 * @details
 *   Use ADC/DAC/TIMER/DMA/PRS in order to amplify input signal and loop it
 *   back to audio out. The main point with this example is to show the use
 *   of the above peripherals, only simple audio processing is used.
 *
 * @note
 *   WARNING: Do not attach or use headphones with this example. Use small
 *   loadspeakers with built in amplification, ensuring volume is at an
 *   acceptable level. Exposure to loud noises from any source for extended
 *   periods of time may temporarily or permanently affect your hearing. The
 *   louder the volume sound level, the less time is required before your
 *   hearing could be affected. Hearing damage from loud noise is sometimes
 *   undetectable at first and can have a cumulative effect.
 *
 * @par Usage
 *   Use DK push buttons PB1 and PB2 to decrease/increase output volume.
 *   Push the AEM button on the kit until "EFM" shows in the upper right
 *   corner of the TFT display to activate PB1 and PB2 pushbuttons.
 *   The volume level is indicated by the 14 leftmost user LEDs.
 *   Audio out clipping may be done due to too high input signal and/or to high
 *   volume setting. Clipping is indicated by the rigthmost user LED.
 *   Reduce volume level or audio input level to avoid clipping.
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
#include "em_emu.h"
#include "em_cmu.h"
#include "em_adc.h"
#include "em_dac.h"
#include "em_prs.h"
#include "em_timer.h"
#include "em_dma.h"
#include "dmactrl.h"
#include "rtcdriver.h"

/*
 *  Audio in/out handling:
 *  ----------------------
 *
 *                    EFM32 DK preamp implementation overview
 *
 *                                | Audio in
 *                                V
 *  +------+     +-----+       +------+       +-----+         +--------+
 *  |TIMER0|---->| PRS |--+--->| ADC0 |------>| DMA |-------->| Buffer |
 *  +------+     +-----+  |    +------+       +-----+         +--------+
 *                        |                      |                 |
 *                        |                      V                 |
 *                        |                   +-----+              |
 *                        |                   | DMA |              |
 *                        |                   | IRQ |              V
 *                        |                   +-----+         +--------+
 *                        |                      |            | PendSV |Process
 *                        |                      +----------->|   IRQ  |data
 *                        |                      Trigger      +--------+
 *                        |                      PendSV            |
 *                        |                                        V
 *                        |    +------+       +-----+         +--------+
 *                        +--->| DAC0 |<------| DMA |<--------| Buffer |
 *                             +------+       +-----+         +--------+
 *                                |
 *                                V Audio out
 *
 *  1. TIMER0 is set to overflow appr. PREAMP_AUDIO_SAMPLE_RATE times per seconds,
 *     triggering a pulse to the PRS module on each overflow.
 *
 *  2. ADC0 is configured to trigger a scan sequence (left+right channels) when
 *     PRS pulse occurs.
 *
 *  3. The DMA is using a ping-pong transfer type to receive sampled data from the
 *     ADC0 into a buffer (right and left channel interleaved). When the specified
 *     number of samples has been received, the DMA will trigger an interrupt and
 *     switch sampling into an alternate buffer. The DMA interrupt handler only
 *     refreshes the use of the current buffer, and triggers a lower priority
 *     handler which processes the received audio stream.
 *
 *  4. The DAC0 has been configured to trigger request for data from DMA when same
 *     PRS pulse as used for ADC0 occurs. This ensures synchronization between
 *     sampled data on input and converted signal and output. The data transferred
 *     to DAC0 will be taken from already received and processed data, thus a
 *     slight delay occurs, proportional with the buffer size.
 *
 *  5. The DMA channel used by DAC0 is also using a ping-pong transfer type.
 *
 *  Potentiometer (volume) handling:
 *  --------------------------------
 *
 *  - The volume level (PB1 and PB2 pushbuttons) is sampled less frequently
 *    than the audio processing.
 *    (Push the AEM button on the kit until "EFM" shows in the upper right
 *     corner of the TFT display to activate PB1 and PB2 pushbuttons).
 *
 *  - The triggering of the volume level sampling is issued by
 *    the service call interrupt handler which in turn is synchronized with the
 *    audio-in interrupt handler, providing a regular sampling trigger.
 *
 *  - When audio output volume is computed, LEDs indicating the current level
 *    are updated from the main routine.
 *
 *  Task priority issues:
 *  ---------------------
 *
 *  The most time critical task in this application is to keep the DMA ping-pong
 *  sequence going. At all times, the next descriptor must be refreshed prior
 *  to being used, otherwise the ping-pong DMA sequence will halt. This example
 *  application only have three tasks to handle, listed in order of priority:
 *
 *  1. Keep DMA ping-pong sequence refreshed for both audio in and out (ADC/DAC)
 *  2. Process audio-in data for use by audio-out
 *  3. Update volume level/LEDs indicating volume level
 *
 *  Task 1 is handled by interrupt handlers. Task 3 is done from the main function.
 *  Task 2 has higher priority than updating volume level, but is the actual
 *  task requiring some amount of processing. If a buffer of audio-in samples is
 *  not processed in time before being sent to audio-out, audio artifacts will occur.
 *  This may for instance occur if core frequency is set too low, not being able
 *  to complete processing of buffers in time.
 *
 *  Since there is no OS available in this example, we implement a prioroity scheme
 *  by letting the ping-pong refresh be done in a high priority interrupt handler,
 *  and a SW triggered lower priority interrupt (PendSV) handler process the audio
 *  data.
 *
 *  Energy mode usage:
 *  ------------------
 *
 *  Due to the relatively high sampling rate, we need to keep the ADC warmed,
 *  and only EM1 may be used when idle.
 */

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

/* Defines according to DK design */

/**
 * Number of samples for each channel processed at a time. By increasing
 * this number, less average interrupt handler overhead is added per sample, but
 * more memory is required, and increased delay added. If setting too low,
 * interrupt overhead will become too large, causing loss of data.
 */
#define PREAMP_AUDIO_BUFFER_SIZE     64     /* 64/44100 = appr 1.5 msec delay */

/** (Approximate) sample rate used for processing audio data. */
#define PREAMP_AUDIO_SAMPLE_RATE     44100

/** Number of times to sample volume (potentiometer) per second. */
#define PREAMP_VOLUME_SAMPLE_RATE    20

/** DMA channel used for audio in scan sequence (both right and left channel) */
#define PREAMP_DMA_AUDIO_IN          0

/** DMA channel used for audio out channels */
#define PREAMP_DMA_AUDIO_OUT         1

/** PRS channel used by TIMER to trigger ADC/DAC activity. */
#define PREAMP_PRS_CHANNEL           0

/** Max number of output volume steps. */
#define VOLUME_MAX                   13

/** Pushbutton check mask. */
#define PB_MASK                      (BC_UIF_PB1 | BC_UIF_PB2) /* Check PB1 and PB2 */

/*******************************************************************************
 ***************************   LOCAL VARIABLES   *******************************
 ******************************************************************************/

/** Variable indicate if primary or alternate audio buffer shall be processed */
static volatile bool preampProcessPrimary;

/** Flag used to trigger volume check */
static volatile bool preampCheckVolume;

/** Flag used to indicate that audio-out level has been clipped to limit volume */
static volatile bool preampAudioOutClipped;

/** Adjustment factor used for adjusting volume, range is 0..100 */
static volatile uint32_t preampAdjustFactor;

/** Primary audio in buffer, holding both left and right channel (interleaved) */
static uint16_t preampAudioInBuffer1[PREAMP_AUDIO_BUFFER_SIZE * 2];
/** Alternate audio in buffer, holding both left and right channel (interleaved) */
static uint16_t preampAudioInBuffer2[PREAMP_AUDIO_BUFFER_SIZE * 2];

/** Primary audio out buffer, combined right/left channel in one uint32_t. */
static uint32_t preampAudioOutBuffer1[PREAMP_AUDIO_BUFFER_SIZE];
/** Alternate audio out buffer, combined right/left channel in one uint32_t. */
static uint32_t preampAudioOutBuffer2[PREAMP_AUDIO_BUFFER_SIZE];

/** Callback config for audio-in DMA handling, must remain 'live' */
static DMA_CB_TypeDef cbInData;
/** Callback config for audio-out DMA handling, must remain 'live' */
static DMA_CB_TypeDef cbOutData;

/* Counters used to monitor IRQ handling, they should all be equal. If */
/* preampMonProcessCount falls behind, it is an indication that the */
/* system is unable to process incoming data with sufficient speed, and */
/* audio out artifacts will occur. */

/** Count number of interrupts on input buffer filled. */
static uint32_t preampMonInCount;
/** Count number of interrupts on output buffer sent. */
static uint32_t preampMonOutCount;
/** Count number of times buffer has been processed. */
static uint32_t preampMonProcessCount;

/** Volume values for preampAdjustFactor in approx. 3dB steps. */
static const uint32_t preampVolume[ VOLUME_MAX + 1 ] =
{
  0, 1, 2, 3, 4, 6, 9, 13, 18, 25, 35, 50, 71, 100
};
static int            volume;

/** Timer used for bringing the system back to EM0. */
static RTCDRV_TimerID_t xTimerForWakeUp;

/*******************************************************************************
 ************************   INTERRUPT FUNCTIONS   ******************************
 ******************************************************************************/

/***************************************************************************//**
* @brief
*   Callback invoked from DMA interrupt handler when DMA transfer has filled
*   an audio in buffer. (I.e. received from ADC.)
*******************************************************************************/
static void preampDMAInCb(unsigned int channel, bool primary, void *user)
{
  (void) user; /* Unused parameter */

  /* Refresh DMA for using this buffer. DMA ping-pong will */
  /* halt if buffer not refreshed in time. */
  DMA_RefreshPingPong(channel,
                      primary,
                      false,
                      NULL,
                      NULL,
                      (PREAMP_AUDIO_BUFFER_SIZE * 2) - 1,
                      false);

  preampMonInCount++;

  /* Indicate buffer to be processed next */
  preampProcessPrimary = primary;

  /* Trigger lower priority interrupt which will process data */
  SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
}


/***************************************************************************//**
* @brief
*   Callback invoked from DMA interrupt handler when DMA transfer has used
*   an audio out buffer. (I.e. transferred to DAC.)
*******************************************************************************/
static void preampDMAOutCb(unsigned int channel, bool primary, void *user)
{
  (void) user; /* Unused parameter */

  /* Refresh DMA for using this buffer. DMA ping-pong will */
  /* halt if buffer not refreshed in time. */
  DMA_RefreshPingPong(channel,
                      primary,
                      false,
                      NULL,
                      NULL,
                      PREAMP_AUDIO_BUFFER_SIZE - 1,
                      false);

  preampMonOutCount++;
}

/***************************************************************************//**
* @brief
*   SW triggered interrupt used to process sampled data.
*******************************************************************************/
void PendSV_Handler(void)
{
  /* Define max DAC output value, clipping is done if audio-out signal is outside */
  /* defined range. This is partly to limit max output volume, and partly to */
  /* add as low DC offset to DAC output as possible. A smaller DC offset reduces */
  /* the startup crack sound, although that could be avoided by more advanced */
  /* init features. */
  #define OUTPUT_RANGE              0xA0

  /**
   * Need to figure out average DC component in input signal (appr 1.65V
   * according to DK design), since DC offset needs to be removed before adjusting
   * volume. If adjusting directly on input signal including DC offset, one would
   * also scale (the significant) DC offset, causing sharp transissions (and audio
   * out cracking noise) when adjusting volume. We only sample DC once during startup,
   * one could make more advanced monitoring features.
   */
  #define SAMPLE_DC_OFFSET_COUNT    2048
  static uint32_t dcOffsetSampleCount;
  static uint32_t dcAccRight;
  static uint32_t dcAccLeft;
  static uint32_t dcRight;
  static uint32_t dcLeft;

  static uint32_t volumeSampleCount;

  uint16_t        *inBuf;
  uint32_t        *outBuf;
  int32_t         right;
  int32_t         left;
  int             i;

  preampMonProcessCount++;

  if (preampProcessPrimary)
  {
    inBuf  = preampAudioInBuffer1;
    outBuf = preampAudioOutBuffer1;
  }
  else
  {
    inBuf  = preampAudioInBuffer2;
    outBuf = preampAudioOutBuffer2;
  }

  /* Set start sample index */
  i = 0;

  /* Are we measuring DC offset average? Only done initially. */
  if (dcOffsetSampleCount < SAMPLE_DC_OFFSET_COUNT)
  {
    while (i < PREAMP_AUDIO_BUFFER_SIZE)
    {
      i++;

      /* Right channel */
      right = (int32_t) *inBuf++;

      /* Left channel */
      left = (int32_t) *inBuf++;

      /* Avoid using input signal until first DC measurement done */
      outBuf++;

      /* Measure DC component of audio-in channels? */
      dcAccRight += right;
      dcAccLeft  += left;

      /* Finished measuring DC offset* */
      if (++dcOffsetSampleCount == SAMPLE_DC_OFFSET_COUNT)
      {
        dcRight = dcAccRight / SAMPLE_DC_OFFSET_COUNT;
        dcLeft  = dcAccLeft / SAMPLE_DC_OFFSET_COUNT;
        break;
      }
    }
  }

  /* Process input data */
  while (i < PREAMP_AUDIO_BUFFER_SIZE)
  {
    i++;

    /* Right channel */
    right = (int32_t) *inBuf++;

    /* Left channel */
    left = (int32_t) *inBuf++;

    /* Remove DC component of input signal */
    right -= dcRight;
    left  -= dcLeft;

    /* Volume adjustment, using integer arithmetic */
    right = (right * (int32_t) preampAdjustFactor) / 100;
    left  = (left * (int32_t) preampAdjustFactor) / 100;

    /* Add midpoint DC offset of allowed output range */
    right += OUTPUT_RANGE / 2;
    if (right < 0)
    {
      right                 = 0;
      preampAudioOutClipped = true;
    }
    else if (right > OUTPUT_RANGE)
    {
      right                 = OUTPUT_RANGE;
      preampAudioOutClipped = true;
    }

    left += OUTPUT_RANGE / 2;
    if (left < 0)
    {
      left                  = 0;
      preampAudioOutClipped = true;
    }
    else if (left > OUTPUT_RANGE)
    {
      left                  = OUTPUT_RANGE;
      preampAudioOutClipped = true;
    }

    /* Encode for use with DAC COMBDATA accessed by DMA */
    *(outBuf++) = ((uint32_t) left << 16) | (uint32_t) right;
  }

  /* Trigger sampling of potentiometer used for volume control? */
  volumeSampleCount += PREAMP_AUDIO_BUFFER_SIZE;
  if (volumeSampleCount >= (PREAMP_AUDIO_SAMPLE_RATE / PREAMP_VOLUME_SAMPLE_RATE))
  {
    volumeSampleCount = 0;
    preampCheckVolume = true;
  }
}


/*******************************************************************************
 ***************************   LOCAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
* @brief
*   Configure ADC usage for this application.
*******************************************************************************/
static void preampADCConfig(void)
{
  DMA_CfgDescr_TypeDef   descrCfg;
  DMA_CfgChannel_TypeDef chnlCfg;
  ADC_Init_TypeDef       init     = ADC_INIT_DEFAULT;
  ADC_InitScan_TypeDef   scanInit = ADC_INITSCAN_DEFAULT;

  /* Configure DMA usage by ADC */

  cbInData.cbFunc  = preampDMAInCb;
  cbInData.userPtr = NULL;

  chnlCfg.highPri   = true;
  chnlCfg.enableInt = true;
  chnlCfg.select    = DMAREQ_ADC0_SCAN;
  chnlCfg.cb        = &cbInData;
  DMA_CfgChannel(PREAMP_DMA_AUDIO_IN, &chnlCfg);

  descrCfg.dstInc  = dmaDataInc2;
  descrCfg.srcInc  = dmaDataIncNone;
  descrCfg.size    = dmaDataSize2;
  descrCfg.arbRate = dmaArbitrate1;
  descrCfg.hprot   = 0;
  DMA_CfgDescr(PREAMP_DMA_AUDIO_IN, true, &descrCfg);
  DMA_CfgDescr(PREAMP_DMA_AUDIO_IN, false, &descrCfg);

  DMA_ActivatePingPong(PREAMP_DMA_AUDIO_IN,
                       false,
                       preampAudioInBuffer1,
                       (void *)((uint32_t) &(ADC0->SCANDATA)),
                       (PREAMP_AUDIO_BUFFER_SIZE * 2) - 1,
                       preampAudioInBuffer2,
                       (void *)((uint32_t) &(ADC0->SCANDATA)),
                       (PREAMP_AUDIO_BUFFER_SIZE * 2) - 1);

  /* Indicate starting with primary in-buffer (according to above DMA setup) */
  preampProcessPrimary = true;

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
*   Configure DAC usage for this application.
*******************************************************************************/
static void preampDACConfig(void)
{
  DMA_CfgDescr_TypeDef    descrCfg;
  DMA_CfgChannel_TypeDef  chnlCfg;
  DAC_Init_TypeDef        dacInit   = DAC_INIT_DEFAULT;
  DAC_InitChannel_TypeDef dacChInit = DAC_INITCHANNEL_DEFAULT;

  /* Notice: Audio out buffers are by default filled with 0, since */
  /* uninitialized data; no need to clear explicitly. */

  /* Configure DAC */

  /* Init common DAC issues */
  dacInit.reference = dacRefVDD;
  DAC_Init(DAC0, &dacInit);

  /* Start with "no" signal out */
  DAC0->COMBDATA = 0x0;

  /* Init channels, equal config for both channels. */
  dacChInit.enable    = true;
  dacChInit.prsSel    = dacPRSSELCh0;
  dacChInit.prsEnable = true;
  DAC_InitChannel(DAC0, &dacChInit, 0); /* Right channel */
  DAC_InitChannel(DAC0, &dacChInit, 1); /* Left channel */

  /* Configure DMA usage by DAC */

  cbOutData.cbFunc  = preampDMAOutCb;
  cbOutData.userPtr = NULL;

  chnlCfg.highPri   = true;
  chnlCfg.enableInt = true;
  chnlCfg.select    = DMAREQ_DAC0_CH0;
  chnlCfg.cb        = &cbOutData;
  DMA_CfgChannel(PREAMP_DMA_AUDIO_OUT, &chnlCfg);

  descrCfg.dstInc  = dmaDataIncNone;
  descrCfg.srcInc  = dmaDataInc4;
  descrCfg.size    = dmaDataSize4;
  descrCfg.arbRate = dmaArbitrate1;
  descrCfg.hprot   = 0;
  DMA_CfgDescr(PREAMP_DMA_AUDIO_OUT, true, &descrCfg);
  DMA_CfgDescr(PREAMP_DMA_AUDIO_OUT, false, &descrCfg);

  DMA_ActivatePingPong(PREAMP_DMA_AUDIO_OUT,
                       false,
                       (void *)((uint32_t) &(DAC0->COMBDATA)),
                       preampAudioOutBuffer1,
                       PREAMP_AUDIO_BUFFER_SIZE - 1,
                       (void *)((uint32_t) &(DAC0->COMBDATA)),
                       preampAudioOutBuffer2,
                       PREAMP_AUDIO_BUFFER_SIZE - 1);
}


/***************************************************************************//**
* @brief
*   Configure PRS usage for this application.
*
* @param[in] prsChannel
*   PRS channel to use.
*******************************************************************************/
static void preampPRSConfig(unsigned int prsChannel)
{
  PRS_LevelSet(0, 1 << (prsChannel + _PRS_SWLEVEL_CH0LEVEL_SHIFT));
  PRS_SourceSignalSet(prsChannel,
                      PRS_CH_CTRL_SOURCESEL_TIMER0,
                      PRS_CH_CTRL_SIGSEL_TIMER0OF,
                      prsEdgePos);
}

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

int main(void)
{
  DMA_Init_TypeDef   dmaInit;
  TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;
  uint32_t           leds;
  static uint16_t    last_buttons = 0;
  uint16_t           buttons;

  /* Initialize DK board register access */
  BSP_Init(BSP_INIT_DEFAULT);

  /* If first word of user data page is non-zero, enable eA Profiler trace */
  BSP_TraceProfilerSetup();

  /* Initialize RTC timer. */
  RTCDRV_Init();
  RTCDRV_AllocateTimer( &xTimerForWakeUp);

  volume             = 7;
  preampAdjustFactor = preampVolume[ volume ];
  BSP_LedsSet((uint16_t)(0x00003FFF << (15 - volume)));

  /* Connect audio in/out to ADC/DAC */
  BSP_PeripheralAccess(BSP_AUDIO_IN, true);
  BSP_PeripheralAccess(BSP_AUDIO_OUT, true);

  /* Wait a while in order to let signal from audio-in stabilize after */
  /* enabling audio-in peripheral. */
  RTCDRV_StartTimer( xTimerForWakeUp, rtcdrvTimerTypeOneshot, 1000, NULL, NULL);
  EMU_EnterEM2(true);

  /* Current example gets by at 14MHz core clock (also with low level of compiler */
  /* optimization). That may however change if modified, consider changing to */
  /* higher HFRCO band or HFXO. */
  /*
   * Use for instance one of below statements to increase core clock.
   * CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);
   * CMU_HFRCOBandSet(cmuHFRCOBand_28MHz);
   */

  /* Enable clocks required */
  CMU_ClockEnable(cmuClock_HFPER, true);
  CMU_ClockEnable(cmuClock_ADC0, true);
  CMU_ClockEnable(cmuClock_DAC0, true);
  CMU_ClockEnable(cmuClock_PRS, true);
  CMU_ClockEnable(cmuClock_DMA, true);
  CMU_ClockEnable(cmuClock_TIMER0, true);

  /* Ensure DMA interrupt at higher priority than PendSV. */
  /* (PendSV used to process sampled audio). */
  NVIC_SetPriority(DMA_IRQn, 0);                              /* Highest priority */
  NVIC_SetPriority(PendSV_IRQn, (1 << __NVIC_PRIO_BITS) - 1); /* Lowest priority */

  /* Configure peripheral reflex system used by TIMER to trigger ADC/DAC */
  preampPRSConfig(PREAMP_PRS_CHANNEL);

  /* Configure general DMA issues */
  dmaInit.hprot        = 0;
  dmaInit.controlBlock = dmaControlBlock;
  DMA_Init(&dmaInit);

  /* Configure DAC used for audio-out */
  preampDACConfig();

  /* Configure ADC used for audio-in */
  preampADCConfig();

  /* Trigger sampling according to configured sample rate */
  TIMER_TopSet(TIMER0, CMU_ClockFreqGet(cmuClock_HFPER) / PREAMP_AUDIO_SAMPLE_RATE);
  TIMER_Init(TIMER0, &timerInit);

  /* Main loop, only responsible for checking volume */
  while (1)
  {
    /* Triggered to check volume setting? */
    if (preampCheckVolume)
    {
      preampCheckVolume = false;

      /* Calculate new output volume. */

      buttons = BSP_PushButtonsGet() & PB_MASK; /* Check pushbuttons */
      if (buttons != last_buttons)
      {
        if (buttons & BC_UIF_PB2)               /* Increase volume */
        {
          if (volume < VOLUME_MAX)
            volume++;
        }
        else if (buttons & BC_UIF_PB1)          /* Decrease volume */
        {
          if (volume)
            volume--;
        }
        preampAdjustFactor = preampVolume[ volume ];
        last_buttons       = buttons;
      }

      /* Use 14 leftmost leds for volume control indication. Rightmost led is */
      /* used to indicate clipping of audio out signal.                       */

      leds = 0x00003FFF << (15 - volume);

      /* Audio out clipped? */
      if (preampAudioOutClipped)
      {
        preampAudioOutClipped = false;
        leds                 |= 0x0001;
      }
      BSP_LedsSet((uint16_t) leds);
    }

    EMU_EnterEM1();
  }
}
