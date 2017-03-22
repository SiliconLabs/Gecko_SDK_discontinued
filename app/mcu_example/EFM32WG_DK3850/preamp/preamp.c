/**************************************************************************//**
 * @file  preamp.c
 * @brief Preamp example project.
 * @version 5.1.2
 * @note
 *   WARNING: Do not attach or use headphones with this example. Use small
 *   loadspeakers with built in amplification, ensuring volume is at an
 *   acceptable level. Exposure to loud noises from any source for extended
 *   periods of time may temporarily or permanently affect your hearing. The
 *   louder the volume sound level, the less time is required before your
 *   hearing could be affected. Hearing damage from loud noise is sometimes
 *   undetectable at first and can have a cumulative effect.
 *
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

#include <stdint.h>
#include <limits.h>
#include <math.h>

#include "plot.h"
#include "GUI.h"
#include "preamp.h"

#include "em_device.h"
#include "em_common.h"
#include "em_adc.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_dma.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_prs.h"
#include "em_timer.h"
#include "em_usart.h"

#include "bsp.h"
#include "bsp_trace.h"
#include "dmactrl.h"
#include "arm_math.h"

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
 *  +------+     +-----+       +------+       +-----+         +--------+
 *                                               |                 |
 *                                               V                 |
 *                                            +-----+              |
 *                                            | DMA |              |
 *                                            | IRQ |              V
 *                                            +-----+         +--------+
 *                                               |            | PendSV |Process
 *                                               +----------->|   IRQ  |data
 *                                               Trigger      +--------+
 *                                               PendSV            |
 *                                                                 V
 *                           +---------+      +-----+         +--------+
 *                           | I2S DAC |<-----| DMA |<--------| Buffer |
 *                           +---------+      +-----+         +--------+
 *                                |
 *                                V Audio out
 *
 *  1. TIMER0 is set to overflow SAMPLE_RATE times per seconds,
 *     triggering a pulse to the PRS module on each overflow.
 *
 *  2. ADC0 is configured to trigger a scan sequence (left+right channels) when
 *     PRS pulse occurs.
 *
 *  3. The DMA is using a ping-pong transfer type to receive sampled data from
 *     ADC0 into a buffer (right and left channel interleaved). When the specified
 *     number of samples has been received, the DMA will trigger an interrupt and
 *     switch sampling into an alternate buffer. The DMA interrupt handler then
 *     fires the PendSV interrupt which will process the audio data.
 *
 *  4. The I2S DAC has been configured to trigger request for data from DMA when
 *     at a rate given by the I2S baudrate. This baudrate MUST match TIMER0
 *     exactly.
 *
 *  5. The DMA channel used by the I2S DAC is also using a ping-pong transfer type.
 *
 *  Tone controls:
 *  --------------
 *
 *  - All controls are checked in the main loop at a rate determined by the
 *    systick timer.
 */

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

//#define GPIO_SCOPE_TRIGGERS /* Make scope trigger signals on PA12 and PA13. */
//#define SKIP_TONE_CONTROLS  /* Omit bass/treble filters in signal chain.    */

/* Configure one out of four possible ADC oversampling rates.                             */
/* ADC_OVERSAMPLE_RATE | ADC_OVERSAMPLE_SHIFT | ADC_OVERSAMPLE_VALUE | ADC_OVERSAMPLE_RES */
/* --------------------|----------------------|----------------------|------------------- */
/*          1          |           4          |    adcOvsRateSel2    |      adcRes12Bit   */
/*          2          |           3          |    adcOvsRateSel2    |      adcResOVS     */
/*          4          |           2          |    adcOvsRateSel4    |      adcResOVS     */
/*          8          |           1          |    adcOvsRateSel8    |      adcResOVS     */
#define ADC_OVERSAMPLE_RATE       8
#define ADC_OVERSAMPLE_SHIFT      1
#define ADC_OVERSAMPLE_VALUE      adcOvsRateSel8
#define ADC_OVERSAMPLE_RES        adcResOVS

/* Define the samplerate, this value is manually tuned, not all values will
   do ! The value chosen is directly related to the selected I2S baudrate.    */
#define SAMPLE_RATE               50000

#define BUFFERSIZE                512
#define VOLUME_MAX                10
#define TICKS_PR_CONTROLS_CHECK   1
#define DAC_DMA_CH                0
#define ADC_DMA_CH                1
#define ADC_PRS_CHANNEL           0

/*******************************************************************************
 ***************************   LOCAL VARIABLES   *******************************
 ******************************************************************************/

/* DMA callback structures. */
static DMA_CB_TypeDef dacDmaCallback, adcDmaCallback;

static int16_t  dacData0[BUFFERSIZE], dacData1[BUFFERSIZE];
static uint16_t adcData0[BUFFERSIZE], adcData1[BUFFERSIZE];

static int16_t  sineBuf[BUFFERSIZE];
static float    filterBuf1L[ BUFFERSIZE/2 ], filterBuf1R[ BUFFERSIZE/2 ];
static float    filterBuf2L[ BUFFERSIZE/2 ], filterBuf2R[ BUFFERSIZE/2 ];

static volatile bool checkControls = false;
static int16_t  *audioDst, *audioSrc;

static uint16_t joystickStatus = 0;
static uint16_t buttonStatus = 0;
static int      checkControlsTickCount = 0;
static int      currentBass, currentTreble;
static bool     testMode;
int             volume, balance, bass, treble;

/* Tone control filter instances. */
static arm_biquad_casd_df1_inst_f32 bassLeftFilter, bassRightFilter;
static arm_biquad_casd_df1_inst_f32 trebleLeftFilter, trebleRightFilter;

/* Filter coefficient arrays. */
static float32_t bassCoeff[5], trebleCoeff[5];

/* Filter state arrays. */
static float32_t bassLeftState[4], bassRightState[4];
static float32_t trebleLeftState[4], trebleRightState[4];

/* Array of bass filter coefficient arrays. */
static const float32_t bassCoeffs[11][5] =
{
  /*  Gain              b0                     b1                     b2                   a1                      a2        */
  /* -10dB */ { 0.9896816725117257f , -1.9529826375649526f , 0.9636479694136102f , 1.952607477676734f  , -0.9537048018135545f },
  /* -8dB  */ { 0.992235390556079f  , -1.95802200238334f   , 0.9661345115778704f , 1.957759009927104f  , -0.9586328945901855f },
  /* -6dB  */ { 0.9945170616260989f , -1.9625245248692562f , 0.9683561629982855f , 1.9623510010064973f , -0.9630467484871436f },
  /* -4dB  */ { 0.9965551061299993f , -1.9665462882718263f , 0.9703405964805928f , 1.9664441032377802f , -0.9669978876446386f },
  /* -2dB  */ { 0.9983750928406495f , -1.9701377485819662f , 0.972112708207818f  , 1.9700924298424471f , -0.9705331197879867f },
  /* 0dB   */ { 1.0f                , -1.9733442497812992f , 0.9736948719763152f , 1.9733442497812992f , -0.9736948719763152f },
  /* +2dB  */ { 1.0016275517798898f , -1.9732988572831849f , 0.9721127126945397f , 1.9733442497812992f , -0.9736948719763152f },
  /* +4dB  */ { 1.0034568021866632f , -1.9732417115138037f , 0.9703406080571474f , 1.9733442497812992f , -0.9736948719763152f },
  /* +6dB  */ { 1.005513166727312f  , -1.9731697692525536f , 0.9683561857777488f , 1.9733442497812992f , -0.9736948719763152f },
  /* +8dB  */ { 1.0078253703887436f , -1.9730791993116839f , 0.9661345520571872f , 1.9733442497812992f , -0.9736948719763152f },
  /* +10dB */ { 1.0104259053944966f , -1.972965178511578f  , 0.9636480378515397f , 1.9733442497812992f , -0.9736948719763152f }
};

/* Array of treble filter coefficient arrays. */
static const float32_t trebleCoeffs[11][5] =
{
  /*  Gain              b0                     b1                     b2                   a1                      a2        */
  /* -10dB */ { 0.34835104585189497f , -0.5437822792988928f , 0.22341547904107353f , 1.7498454083646984f , -0.777829653958774f  },
  /* -8dB  */ { 0.43191127868232804f , -0.6742213131653154f , 0.27700696288731985f , 1.7197962760093122f , -0.7544932044136448f },
  /* -6dB  */ { 0.5345371318354201f  , -0.8344221249817622f , 0.34282621165152505f , 1.686259909020889f  , -0.7292011275260719f },
  /* -4dB  */ { 0.6601999434891688f  , -1.030584045429205f  , 0.42342024918229465f , 1.6488758785979634f , -0.7019120258402215f },
  /* -2dB  */ { 0.8135540822635373f  , -1.2699726280548462f , 0.5217741619527289f  , 1.6072619286550969f , -0.6726175448165166f },
  /* 0dB   */ { 1.0f                 , -1.5610180758007182f , 0.6413515380575632f  , 1.5610180758007182f , -0.6413515380575632f },
  /* +2dB  */ { 1.2291745832283425f  , -1.9756055112934103f , 0.8267643903219126f  , 1.5610180758007182f , -0.6413515380575632f },
  /* +4dB  */ { 1.5146926470714037f  , -2.4975401692457355f , 1.0631809844311764f  , 1.5610180758007182f , -0.6413515380575632f },
  /* +6dB  */ { 1.870777426754878f   , -3.1546169734380136f , 1.3641730089399804f  , 1.5610180758007182f , -0.6413515380575632f },
  /* +8dB  */ { 2.315290313906118f   , -3.981827659736173f  , 1.7468708080869002f  , 1.5610180758007182f , -0.6413515380575632f },
  /* +10dB */ { 2.870667425597914f   , -5.023224213624618f  , 2.23289025028355f    , 1.5610180758007182f , -0.6413515380575632f }
};

/* Table with 3dB per step volume adjust factors. */
const float volumeTable[ VOLUME_MAX + 1 ] =
{
  0.0f,  /* Mute, should have been 0.031622777f */
  0.044668359f, 0.063095734f, 0.089125094f, 0.125892541f, 0.177827941f,
  0.251188643f, 0.354813389f, 0.501187234f, 0.707945784f, 1.0f
};

#ifdef USE_GUI
const float volumeTable_dB[ VOLUME_MAX + 1 ] =
{
  -1000.0f,  /* Mute */
  -27.0f, -24.0f, -21.0f, -18.0f, -15.0f, -12.0f,
  -9.0f,  -6.0f,  -3.0f, 0.0f
};
#endif

/* Table with balance ratio values. */
static const float balanceTable[ BALANCE_MAX + 1 ] =
{
  0.2f,         0.284090909f, 0.450980392f, 0.5625f,
  0.644067797f, 0.708333333f, 0.762711864f, 0.8125f,
  0.862745098f, 0.920454545f, 1.0f
};

#ifdef USE_GUI
/* Table with balance dB values. */
const float balanceTable_dB[ BALANCE_MAX + 1 ] =
{
  -13.97940009f, -10.93085327f, -6.916846802f, -4.997549464f,
  -3.821368301f, -2.995246407f, -2.352789957f, -1.803532607f,
  -1.282349992f, -0.719953065f, 0.0f
};
#endif

/*******************************************************************************
 ***************************   LOCAL PROTOTYPES   ******************************
 ******************************************************************************/

static void AdcDmaCb(unsigned int channel, bool primary, void *user);
static void AdcDmaSetup( void );
static void AdcPrsSetup( void );
static void ControlsCheck( void );
static void DacDmaCb(unsigned int channel, bool primary, void *user);
static void DacDmaSetup(void);
static void DmaSetup(void);
static void I2sSetup(void);
static void TimerSetup( void );
static double WFGEN_sinec_i16iq( uint32_t buffer[], int32_t bufferLength,
                                 double f, double Fs, double amplitude,
                                 double offset );

/**************************************************************************//**
 * @brief Main function.
 *****************************************************************************/
int main(void)
{
  /* Initial preamp "state". */
  volume   = VOLUME_MAX - 2;
  balance  = BALANCE_CENTER;
  bass     = TONE_CENTER;
  treble   = TONE_CENTER;
  testMode = false;

  currentTreble  = treble;
  trebleCoeff[0] = trebleCoeffs[TONE_CENTER][0];
  trebleCoeff[1] = trebleCoeffs[TONE_CENTER][1];
  trebleCoeff[2] = trebleCoeffs[TONE_CENTER][2];
  trebleCoeff[3] = trebleCoeffs[TONE_CENTER][3];
  trebleCoeff[4] = trebleCoeffs[TONE_CENTER][4];

  currentBass  = bass;
  bassCoeff[0] = bassCoeffs[TONE_CENTER][0];
  bassCoeff[1] = bassCoeffs[TONE_CENTER][1];
  bassCoeff[2] = bassCoeffs[TONE_CENTER][2];
  bassCoeff[3] = bassCoeffs[TONE_CENTER][3];
  bassCoeff[4] = bassCoeffs[TONE_CENTER][4];

  /* Chip errata */
  CHIP_Init();

  /* Use 48MHz HFXO as core clock frequency, need high speed high sample rate.*/
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);

  /* Setup SysTick Timer for 50 msec interrupts. */
  if (SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 20)) while (1);

  /* Initialize EBI configuration for external RAM and display controller. */
  BSP_Init(BSP_INIT_DK_EBI);

  /* If first word of user data page is non-zero, enable eA Profiler trace. */
  BSP_TraceProfilerSetup();

  /* Start clocks. */
  CMU_ClockEnable(cmuClock_HFPER, true);
  CMU_ClockEnable(cmuClock_ADC0, true);
  CMU_ClockEnable(cmuClock_DMA, true);
  CMU_ClockEnable(cmuClock_GPIO, true);
  CMU_ClockEnable(cmuClock_PRS, true);
  CMU_ClockEnable(cmuClock_TIMER0, true);

#ifdef GPIO_SCOPE_TRIGGERS
  GPIO_PinModeSet(gpioPortA,12, gpioModePushPull, 0);
  GPIO_PinModeSet(gpioPortA,13, gpioModePushPull, 0);
#endif

#ifdef USE_GUI
  if ( BSP_RegisterRead(&BC_REGISTER->UIF_AEM) == BC_UIF_AEM_EFM)
  {
    PLOT_DisplayInit();
  }
  else
  {
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
  }
#endif

  /* Connect audio in/out to ADC/DAC. */
  BSP_PeripheralAccess(BSP_AUDIO_IN, true);
  BSP_PeripheralAccess(BSP_I2S, true);

  /* Generate a sine wave data pattern for use in test mode. */
  WFGEN_sinec_i16iq(  (uint32_t*)sineBuf,    /* uint32_t buffer[]    */
                      BUFFERSIZE/2,          /* int32_t bufferLength */
                      390.0,                 /* double frequency     */
                      50000.0,               /* double sample rate   */
                      2000.0,                /* double amplitude     */
                      0.0 );                 /* double offset        */

  /* Initialize the floating-point Biquad cascade filters. */
  arm_biquad_cascade_df1_init_f32(
    &bassLeftFilter,    /* Floating-point Biquad cascade structure instance. */
    1,                  /* Number of 2nd order stages in the filter. */
    bassCoeff,          /* Filter coefficient array. */
    bassLeftState );    /* Filter state array. */

  arm_biquad_cascade_df1_init_f32(
    &bassRightFilter,   /* Floating-point Biquad cascade structure instance. */
    1,                  /* Number of 2nd order stages in the filter. */
    bassCoeff,          /* Filter coefficient array. */
    bassRightState );   /* Filter state array. */

  arm_biquad_cascade_df1_init_f32(
    &trebleLeftFilter,  /* Floating-point Biquad cascade structure instance. */
    1,                  /* Number of 2nd order stages in the filter. */
    trebleCoeff,        /* Filter coefficient array. */
    trebleLeftState );  /* Filter state array. */

  arm_biquad_cascade_df1_init_f32(
    &trebleRightFilter, /* Floating-point Biquad cascade structure instance. */
    1,                  /* Number of 2nd order stages in the filter. */
    trebleCoeff,        /* Filter coefficient array. */
    trebleRightState ); /* Filter state array. */

  /* Init DMA. */
  DmaSetup();

  /* Configure peripheral reflex system (PRS) used by TIMER to trigger ADC. */
  AdcPrsSetup();

  /* Configure ADC DMA. */
  AdcDmaSetup();

  /* Setup timer, this will start ADC sampling. */
  TimerSetup();

  /* Setup USART1 as I2S master. */
  I2sSetup();

  /* Setup (and start) DAC DMA. */
  DacDmaSetup();

  while (1)
  {
    GUI_X_Delay(0);             /* Takes care of AEM button switching. */

    if ( checkControls )        /* Pushbuttons/joystick pressed ? */
    {
      checkControls = false;
      ControlsCheck();
    }
  }
}

/**************************************************************************//**
 * @brief Interrupt handler for system tick. Used to signal main loop to
 *        check pushbuttons etc.
 *****************************************************************************/
void SysTick_Handler(void)
{
  if ( ++checkControlsTickCount == TICKS_PR_CONTROLS_CHECK )
  {
    checkControlsTickCount = 0;
    checkControls = true;
  }
}

/**************************************************************************//**
 * @brief
 *   Callback function called when an ADC DMA finishes a transfer.
 * @param channel
 *   The DMA channel that finished.
 * @param primary
 *   Primary or Alternate DMA descriptor
 * @param user
 *   User defined pointer (Not used in this example.)
 *****************************************************************************/
static void AdcDmaCb(unsigned int channel, bool primary, void *user)
{
  (void)user;   /* Unused parameter. */

#ifdef GPIO_SCOPE_TRIGGERS
  if ( primary )
    GPIO_PinOutSet(gpioPortA,13);
  else
    GPIO_PinOutClear(gpioPortA,13);
#endif

  /* Refresh DMA control structure. */
  DMA_RefreshPingPong( channel,
                       primary,
                       false,
                       NULL,
                       NULL,
                       BUFFERSIZE - 1,
                       false );
}

/**************************************************************************//**
 * @brief Initialize ADC and ADC DMA.
 *****************************************************************************/
static void AdcDmaSetup( void )
{
  DMA_CfgDescr_TypeDef   descrCfg;
  DMA_CfgChannel_TypeDef chnlCfg;
  ADC_Init_TypeDef       init     = ADC_INIT_DEFAULT;
  ADC_InitScan_TypeDef   scanInit = ADC_INITSCAN_DEFAULT;

  adcDmaCallback.cbFunc  = AdcDmaCb;
  adcDmaCallback.userPtr = NULL;

  chnlCfg.highPri   = true;
  chnlCfg.enableInt = true;
  chnlCfg.select    = DMAREQ_ADC0_SCAN;
  chnlCfg.cb        = &adcDmaCallback;
  DMA_CfgChannel(ADC_DMA_CH, &chnlCfg);

  descrCfg.dstInc  = dmaDataInc2;
  descrCfg.srcInc  = dmaDataIncNone;
  descrCfg.size    = dmaDataSize2;
  descrCfg.arbRate = dmaArbitrate1;
  descrCfg.hprot   = 0;
  DMA_CfgDescr(ADC_DMA_CH, true, &descrCfg);
  DMA_CfgDescr(ADC_DMA_CH, false, &descrCfg);

  DMA_ActivatePingPong( ADC_DMA_CH,
                        false,
                        adcData0,
                        (void *)((uint32_t) &(ADC0->SCANDATA)),
                        BUFFERSIZE - 1,
                        adcData1,
                        (void *)((uint32_t) &(ADC0->SCANDATA)),
                        BUFFERSIZE - 1);

  /* Configure ADC, keep warm due to "high" frequency sampling. */
  init.warmUpMode = adcWarmupKeepADCWarm;
  /* Init common issues for both single conversion and scan mode. */
  init.timebase   = ADC_TimebaseCalc(0);
  init.prescale   = ADC_PrescaleCalc(12000000, 0);
  init.tailgate   = true;
  init.ovsRateSel = ADC_OVERSAMPLE_VALUE;
  ADC_Init(ADC0, &init);

  /* Init for scan sequence use (audio in left/right channels). */
  scanInit.prsSel     = adcPRSSELCh0;
  scanInit.prsEnable  = true;
  scanInit.reference  = adcRefVDD;
  scanInit.input      = ADC_SCANCTRL_INPUTMASK_CH6 | ADC_SCANCTRL_INPUTMASK_CH7;
  scanInit.resolution = ADC_OVERSAMPLE_RES;
  ADC_InitScan(ADC0, &scanInit);
}

/**************************************************************************//**
 * @brief Initialize PRS system used used by TIMER to trigger ADC.
 *****************************************************************************/
static void AdcPrsSetup( void )
{
  PRS_LevelSet(0, 1 << (ADC_PRS_CHANNEL + _PRS_SWLEVEL_CH0LEVEL_SHIFT));
  PRS_SourceSignalSet( ADC_PRS_CHANNEL,
                       PRS_CH_CTRL_SOURCESEL_TIMER0,
                       PRS_CH_CTRL_SIGSEL_TIMER0OF,
                       prsEdgePos);
}

/**************************************************************************//**
 * @brief Check pushbuttons and joystick status.
 *****************************************************************************/
static void ControlsCheck( void )
{
  uint16_t stick, buttons;
  int oldVolume, oldBalance, oldBass, oldTreble;

  if ( BSP_RegisterRead( &BC_REGISTER->UIF_AEM) == BC_UIF_AEM_EFM)
  {
    oldVolume  = volume;
    oldBalance = balance;
    oldBass    = bass;
    oldTreble  = treble;

    stick   = BSP_JoystickGet();
    if ( stick != joystickStatus )
    {
      if ( stick != 0 )
      {
        if (stick & BC_UIF_JOYSTICK_UP)         /* Increase volume. */
        {
          if (volume < VOLUME_MAX)
            volume++;
        }

        else if (stick & BC_UIF_JOYSTICK_DOWN)  /* Decrease volume. */
        {
          if (volume)
            volume--;
        }

        else if (stick & BC_UIF_JOYSTICK_LEFT)  /* Balance towards left. */
        {
          if (balance)
            balance--;
        }

        else if (stick & BC_UIF_JOYSTICK_RIGHT) /* Balance towards right. */
        {
          if (balance < BALANCE_MAX)
            balance++;
        }

        else if (stick & BC_UIF_JOYSTICK_CENTER) /* Enter/Exit test mode. */
        {
          DMA_Reset();                      /* Stop ongoing DMA's.        */
          DmaSetup();                       /* Prepare for new DMA setup. */
          I2sSetup();

          if ( testMode == false )
          {
            testMode = true;

            DacDmaSetup();                  /* Start audio output dac DMA.  */
          }
          else
          {
            testMode = false;

            DacDmaSetup();                  /* Start audio output dac DMA.  */
            AdcDmaSetup();                  /* Start audio input adc DMA.   */
          }
        }
      }
      joystickStatus = stick;
    }

    buttons = BSP_PushButtonsGet();
    if ( buttons != buttonStatus )
    {
      if ( buttons != 0 )
      {
        if (buttons & BC_UIF_PB2)                 /* Increase bass. */
        {
          if (bass < TONE_MAX)
            bass++;
        }

        else if (buttons & BC_UIF_PB1)            /* Decrease bass. */
        {
          if (bass)
            bass--;
        }

        else if (buttons & BC_UIF_PB4)            /* Increase treble. */
        {
          if (treble < TONE_MAX)
            treble++;
        }

        else if (buttons & BC_UIF_PB3)            /* Decrease treble. */
        {
          if (treble)
            treble--;
        }
      }
      buttonStatus = buttons;
    }

    if ( ( oldVolume  != volume  ) ||
         ( oldBalance != balance ) ||
         ( oldBass    != bass    ) ||
         ( oldTreble  != treble  )    )
      PLOT_DisplayUpdate();
  }
}

/**************************************************************************//**
 * @brief
 *   Callback function called when the DAC DMA finishes a transfer.
 * @param channel
 *   The DMA channel that finished.
 * @param primary
 *   Primary or Alternate DMA descriptor
 * @param user
 *   User defined pointer (Not used in this example.)
 *****************************************************************************/
static void DacDmaCb(unsigned int channel, bool primary, void *user)
{
  (void)user;   /* Unused parameter. */

#ifdef GPIO_SCOPE_TRIGGERS
  if ( primary )
    GPIO_PinOutSet(gpioPortA,12);
  else
    GPIO_PinOutClear(gpioPortA,12);
#endif

  /* Refresh DMA control structure. */
  DMA_RefreshPingPong( channel,
                       primary,
                       false,
                       NULL,
                       NULL,
                       BUFFERSIZE - 1,
                       false );

  if ( testMode == true )
  {
    audioSrc = sineBuf;

    if ( primary )
      audioDst = dacData0;
    else
      audioDst = dacData1;
  }
  else
  {
    if ( primary )
    {
      audioSrc = (int16_t*)adcData0;
      audioDst = dacData0;
    }
    else
    {
      audioSrc = (int16_t*)adcData1;
      audioDst = dacData1;
    }
  }

  /* Trigger lower priority interrupt which will process audio data. */
  SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
}

/**************************************************************************//**
 * @brief
 *   Setup DAC DMA in ping pong mode
 * @details
 *   The DMA is set up to transfer data from memory to the I2S DAC,
 *   triggered by USART TXBL. When the DMA finishes, it will trigger the
 *   callback (DacDmaCb).
 *****************************************************************************/
static void DacDmaSetup(void)
{
  /* DMA configuration structs. */
  DMA_CfgChannel_TypeDef chnlCfg;
  DMA_CfgDescr_TypeDef   descrCfg;

  /* Set the interrupt callback routine. */
  dacDmaCallback.cbFunc = DacDmaCb;

  /* Callback doesn't need userpointer. */
  dacDmaCallback.userPtr = NULL;

  /* Setting up channel */
  chnlCfg.highPri   = false; /* Can't use with peripherals. */
  chnlCfg.enableInt = true;  /* Interrupt needed when buffers are used. */

  chnlCfg.select = DMAREQ_USART1_TXBL;
  chnlCfg.cb = &dacDmaCallback;
  DMA_CfgChannel(DAC_DMA_CH, &chnlCfg);

  /* Setting up channel descriptor. */
  /* Destination is DAC/USART register and doesn't move. */
  descrCfg.dstInc = dmaDataIncNone;

  /* Transfer 16 bit each time to USART_TXDOUBLE register. */
  descrCfg.srcInc = dmaDataInc2;
  descrCfg.size   = dmaDataSize2;

  /* We have time to arbitrate again for each sample. */
  descrCfg.arbRate = dmaArbitrate1;
  descrCfg.hprot   = 0;

  /* Configure both primary and secondary descriptor alike. */
  DMA_CfgDescr(DAC_DMA_CH, true, &descrCfg);
  DMA_CfgDescr(DAC_DMA_CH, false, &descrCfg);

  /* Enabling PingPong Transfer. */
  DMA_ActivatePingPong( DAC_DMA_CH,
                        false,
                        (void *) & (USART1->TXDOUBLE),
                        (void *) &dacData0,
                        BUFFERSIZE - 1,
                        (void *) &(USART1->TXDOUBLE),
                        (void *) &dacData1,
                        BUFFERSIZE - 1);
}

/**************************************************************************//**
 * @brief Initialize DMA.
 *****************************************************************************/
static void DmaSetup(void)
{
  /* DMA configuration struct. */
  DMA_Init_TypeDef  dmaInit;

  NVIC_SetPriority(DMA_IRQn, 0);                              /* Highest priority. */
  NVIC_SetPriority(PendSV_IRQn, (1 << __NVIC_PRIO_BITS) - 1); /* Lowest priority.  */

  /* Initialize DMA controller. */
  dmaInit.hprot        = 0;
  dmaInit.controlBlock = dmaControlBlock;
  DMA_Init(&dmaInit);
}

/**************************************************************************//**
 * @brief
 *   Setup USART1 in I2S mode.
 * @details
 *   USART1 is initialized in I2S mode to feed the DS1334 I2S dac.
 *****************************************************************************/
static void I2sSetup(void)
{
  USART_InitI2s_TypeDef init = USART_INITI2S_DEFAULT;

  CMU_ClockEnable(cmuClock_USART1, true);

  /* Use USART location 1.           */
  /* USART TX  = I2S DATA - Pin D0   */
  /* USART CLK = I2S_SCLK - Pin D2   */
  /* USART CS  = I2S_WS   - Pin D3   */

  GPIO_PinModeSet(gpioPortD, 0, gpioModePushPull, 0);
  GPIO_PinModeSet(gpioPortD, 2, gpioModePushPull, 0);
  GPIO_PinModeSet(gpioPortD, 3, gpioModePushPull, 0);

  /* Configure USART for basic I2S operation */
  init.sync.baudrate = SAMPLE_RATE * 32;
  USART_InitI2s(USART1, &init);

  /* Enable pins at location 1 */
  USART1->ROUTE = USART_ROUTE_TXPEN |
                  USART_ROUTE_CSPEN |
                  USART_ROUTE_CLKPEN |
                  USART_ROUTE_LOCATION_LOC1;
}

/**************************************************************************//**
 * @brief Do all audio processing.
 *        This function is called from within the ADC DMA transfer
 *        complete interrupt.
 *****************************************************************************/
void PendSV_Handler(void)
{
  int i;
  float f, *fL, *fR;
  int16_t  signedInt16;
  int32_t  signedInt32;

  /* Process interleaved audio channels, split left and right channel data. */
  fL = filterBuf1L;
  fR = filterBuf1R;
  for ( i=0; i<BUFFERSIZE; i++ )
  {
    if ( testMode == false )
    {
      /* Get sign bit in correct place and convert to 2nd compl. form. */
      signedInt16 = (*audioSrc++ << ADC_OVERSAMPLE_SHIFT ) + 0x8000;

      /* Divide to to compensate for ADC oversampling. */
      f = ( signedInt16 * 1.0f ) / ADC_OVERSAMPLE_RATE;
    }
    else
    {
      signedInt16 = *audioSrc++;
      f = signedInt16;
    }

    if ( i & 1 )
    {
      *fR++ = f;      /* Right channel. */
    }
    else
    {
      *fL++ = f;      /* Left channel */
    }
  }

  /* We now have separate float arrays for each channel. */

  /* Any change in bass or treble filter setting ? */
  if ( currentBass != bass )
  {
    /* Switch filter coefficients. */
    currentBass  = bass;
    bassCoeff[0] = bassCoeffs[bass][0];
    bassCoeff[1] = bassCoeffs[bass][1];
    bassCoeff[2] = bassCoeffs[bass][2];
    bassCoeff[3] = bassCoeffs[bass][3];
    bassCoeff[4] = bassCoeffs[bass][4];
  }

  if ( currentTreble != treble )
  {
    /* Switch filter coefficients. */
    currentTreble  = treble;
    trebleCoeff[0] = trebleCoeffs[treble][0];
    trebleCoeff[1] = trebleCoeffs[treble][1];
    trebleCoeff[2] = trebleCoeffs[treble][2];
    trebleCoeff[3] = trebleCoeffs[treble][3];
    trebleCoeff[4] = trebleCoeffs[treble][4];
  }

#ifndef SKIP_TONE_CONTROLS
  /* Perform bass and treble filter calculations.    */
  /* Note that the filters could have been cascaded. */
  arm_biquad_cascade_df1_f32(
    &bassLeftFilter,    /* Floating-point Biquad cascade structure instance. */
    filterBuf1L,        /* Block of input data. */
    filterBuf2L,        /* Block of output data. */
    BUFFERSIZE/2 );     /* Number of samples to process per call. */

  arm_biquad_cascade_df1_f32(
    &bassRightFilter,   /* Floating-point Biquad cascade structure instance. */
    filterBuf1R,        /* Block of input data. */
    filterBuf2R,        /* Block of output data. */
    BUFFERSIZE/2 );     /* Number of samples to process per call. */

  arm_biquad_cascade_df1_f32(
    &trebleLeftFilter,  /* Floating-point Biquad cascade structure instance. */
    filterBuf2L,        /* Block of input data. */
    filterBuf1L,        /* Block of output data. */
    BUFFERSIZE/2 );     /* Number of samples to process per call. */

  arm_biquad_cascade_df1_f32(
    &trebleRightFilter, /* Floating-point Biquad cascade structure instance. */
    filterBuf2R,        /* Block of input data. */
    filterBuf1R,        /* Block of output data. */
    BUFFERSIZE/2 );     /* Number of samples to process per call. */
#endif

  /* Process separate channels, join left and right (interleave) channels. */
  fL = filterBuf1L;
  fR = filterBuf1R;
  for ( i=0; i<BUFFERSIZE/2; i++ )
  {
    /* Start with left channel. */
    f = *fL++;

    /* Compute volume. */
    f *= volumeTable[ volume ];

    /* Compute balance.                                           */
    /* The balance step attenuates by approx 3dB at center pos.   */
    f *= balanceTable[ BALANCE_MAX - balance ];

    /* Convert back to integer, cast will take care of overflow/saturation. */
    signedInt32 = (int32_t)f;

    /* Convert to 16bit integer, make sure that overflow/saturation is done. */
    signedInt16 = __SSAT( signedInt32, 16 );
    *audioDst++ = signedInt16;

    /* Ditto for right channel. */
    f = *fR++;
    f *= volumeTable[ volume ];
    f *= balanceTable[ balance ];
    signedInt32 = (int32_t)f;
    signedInt16 = __SSAT( signedInt32, 16 );
    *audioDst++ = signedInt16;
  }

#ifdef GPIO_SCOPE_TRIGGERS
  GPIO_PinOutToggle(gpioPortA,13);
  GPIO_PinOutToggle(gpioPortA,13);
#endif
}

/**************************************************************************//**
 * @brief Setup TIMER0 to support selected sample rate.
 *****************************************************************************/
static void TimerSetup( void )
{
  TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;

  /* Trigger sampling according to configured sample rate */
  TIMER_TopSet(TIMER0, (CMU_ClockFreqGet(cmuClock_HFPER) / SAMPLE_RATE) - 1);
  TIMER_Init(TIMER0, &timerInit);
}

/**************************************************************************//**
 * @brief Generate a sine test tone.
 *****************************************************************************/
static double WFGEN_sinec_i16iq( uint32_t buffer[], int32_t bufferLength,
                                 double f, double Fs, double amplitude,
                                 double offset )
{
  #define WFGEN_CONST_TWOPI ( 2.0*3.14159265358979 )

  int n;
  double fs;            /* Normalised frequency */
  double spp;           /* Samples per period   */
  int periods;          /* Integer number of periods */
  int16_t *buf;
  double temp;
  double c;

  /*
   * Calculate a normalised frequency so that the number of discrete-time
   * samples available in the buffer matches exactly an integer
   * number of periods.
   */

  spp     = Fs/f;
  temp    = (double)bufferLength / spp;
  periods = (int) ( temp + 0.5 );
  fs      = (double)periods / (double)bufferLength;

  /* Calculate sine samples */
  c = WFGEN_CONST_TWOPI*fs;
  buf = (int16_t *)&buffer[0];
  for( n=0; n<bufferLength; n++ )
  {
    *buf++ = (int16_t)( floor( offset + amplitude*sin( c*(double)n ) + 0.5 ) );
    *buf++ = (int16_t)( floor( offset + amplitude*sin( c*(double)n ) + 0.5 ) );
  }

  return fs;
}
