/**************************************************************************//**
 * @file
 * @brief Wav Player, requires FAT32 formatted micro-SD card with .wav file
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

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>

#include "em_device.h"
#include "em_common.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_dac.h"
#include "em_prs.h"
#include "em_timer.h"
#include "em_dma.h"
#include "em_usart.h"
#include "dmactrl.h"
#include "ff.h"
#include "microsd.h"
#include "diskio.h"
#include "bsp.h"

/** Filename to open from SD-card */
#define WAV_FILENAME             "sweet1.wav"

/** Ram buffers
 * BUFFERSIZE should be between 512 and 1024, depending on available ram on efm32
 */
#define BUFFERSIZE               512

/** Max number of output volume steps. */
#define VOLUME_MAX               13

/** Number of 5ms ticks between each volume pushbutton check. */
#define TICKS_PER_VOLUMECHECK    5

/** Volume pushbutton check mask. */
#define PB_MASK                  (BC_UIF_PB1 | BC_UIF_PB2) /* Check PB1 and PB2 */

/** DMA callback structure */
static DMA_CB_TypeDef DMAcallBack;

/* Temporary buffer for use when source is mono, can't put samples directly in
 * stereo DMA buffer with f_read(). */
static int16_t ramBufferTemporaryMono[BUFFERSIZE];

/* Buffers for DMA transfer, 32 bits are transfered at a time with DMA.
 * The buffers are twice as large as BUFFERSIZE to hold both left and right
 * channel samples. */
static int16_t ramBufferDacData0Stereo[2 * BUFFERSIZE];
static int16_t ramBufferDacData1Stereo[2 * BUFFERSIZE];

/** Bytecounter, need to stop DMA when finished reading file */
static uint32_t ByteCounter;

/** File system specific */
static FATFS Fatfs;

/** File to read WAV audio data from */
static FIL WAVfile;

/** WAV header structure */
typedef struct
{
  uint8_t  id[4];                   /** should always contain "RIFF"      */
  uint32_t totallength;             /** total file length minus 8         */
  uint8_t  wavefmt[8];              /** should be "WAVEfmt "              */
  uint32_t format;                  /** Sample format. 16 for PCM format. */
  uint16_t pcm;                     /** 1 for PCM format                  */
  uint16_t channels;                /** Channels                          */
  uint32_t frequency;               /** sampling frequency                */
  uint32_t bytes_per_second;        /** Bytes per second                  */
  uint16_t bytes_per_capture;       /** Bytes per capture                 */
  uint16_t bits_per_sample;         /** Bits per sample                   */
  uint8_t  data[4];                 /** should always contain "data"      */
  uint32_t bytes_in_data;           /** No. bytes in data                 */
} WAV_Header_TypeDef;

/** Wav header. Global as it is used in callbacks. */
static WAV_Header_TypeDef wavHeader;

/*
 *   DK push buttons PB1 and PB2 are used to decrease/increase output volume.
 *   Push the AEM button on the kit until "EFM" shows in the upper right
 *   corner of the TFT display to activate PB1 and PB2 pushbuttons.
 */

/** Variables for volume control. */
static int           volume;
static volatile bool checkVolume;
static int           checkVolumeCount;
static uint16_t      last_buttons = 0;

/** Table with approx. 3dB per step adjust factors. */
static const uint32_t Volume[ VOLUME_MAX + 1 ] =
{
  0, 1, 2, 3, 4, 6, 9, 13, 18, 25, 35, 50, 71, 100
};

/** Adjustment factor used for adjusting volume, range is 0..100 */
static volatile uint32_t volumeAdjustFactor;

/***************************************************************************//**
 * @brief
 *   Initialize MicroSD driver.
 * @return
 *   Returns 0 if initialization succeded, non-zero otherwise.
 ******************************************************************************/
int initFatFS(void)
{
  MICROSD_Init();
  if (f_mount(0, &Fatfs) != FR_OK)
    return -1;
  return 0;
}

/***************************************************************************//**
 * @brief
 *   This function is required by the FAT file system in order to provide
 *   timestamps for created files. Since we do not have a reliable clock we
 *   hardcode a value here.
 *
 *   Refer to reptile/fatfs/doc/en/fattime.html for the format of this DWORD.
 * @return
 *    A DWORD containing the current time and date as a packed datastructure.
 ******************************************************************************/
DWORD get_fattime(void)
{
  return (28 << 25) | (2 << 21) | (1 << 16);
}

/**************************************************************************//**
 * @brief SysTick_Handler
 * Interrupt Service Routine for system tick counter.
 *****************************************************************************/
void SysTick_Handler(void)
{
  if (checkVolumeCount < TICKS_PER_VOLUMECHECK)
    checkVolumeCount++;

  if ((checkVolumeCount == TICKS_PER_VOLUMECHECK) && (checkVolume == false))
  {
    checkVolumeCount = 0;
    checkVolume      = true;
  }
}

/**************************************************************************//**
 * @brief
 *   This function fills up the memory buffers with data from SD card.
 * @param stereo
 *   Input is in stereo.
 * @param primary
 *   Fill primary or alternate DMA buffer with data.
 *****************************************************************************/
void FillBufferFromSDcard(bool stereo, bool primary)
{
  UINT     bytes_read;
  int16_t  * buffer;
  int      i, j;
  uint16_t tmp;

  /* Set buffer pointer correct ram buffer */
  if (primary)
  {
    buffer = ramBufferDacData0Stereo;
  }
  else /* Alternate */
  {
    buffer = ramBufferDacData1Stereo;
  }

  if (stereo)
  {
    /* Stereo, Store Left and Right data interlaced as in wavfile */
    /* DMA is writing the data to the combined register as interlaced data*/

    /* First buffer is filled from SD-card */
    f_read(&WAVfile, buffer, 4 * BUFFERSIZE, &bytes_read);
    ByteCounter += bytes_read;

    for (i = 0; i < 2 * BUFFERSIZE; i++)
    {
      /* Adjust volume */
      buffer[i] = (buffer[i] * (int32_t) volumeAdjustFactor) / 100;

      /* Convert from signed to unsigned */
      tmp = buffer[i] + 0x8000;

      /* Convert to 12 bits */
      tmp >>= 4;

      buffer[i] = tmp;
    }
  }
  else /* Mono */
  {
    /* Read data into temporary buffer. */
    f_read(&WAVfile, ramBufferTemporaryMono, BUFFERSIZE, &bytes_read);
    ByteCounter += bytes_read;

    j = 0;
    for (i = 0; i < (2 * BUFFERSIZE) - 1; i += 2)
    {
      /* Adjust volume */
      ramBufferTemporaryMono[j] = (ramBufferTemporaryMono[j] *
                                   (int32_t) volumeAdjustFactor) / 100;

      /* Convert from signed to unsigned */
      tmp = ramBufferTemporaryMono[j] + 0x8000;

      /* Convert to 12 bits */
      tmp >>= 4;

      /* Make sample 12 bits and unsigned */
      buffer[ i     ] = tmp;
      buffer[ i + 1 ] = tmp;
      j++;
    }
  }
}

/**************************************************************************//**
 * @brief
 *   Callback function called when the DMA finishes a transfer.
 * @param channel
 *   The DMA channel that finished.
 * @param primary
 *   Primary or Alternate DMA descriptor
 * @param user
 *   User defined pointer (Not used in this example.)
 *****************************************************************************/
void PingPongTransferComplete(unsigned int channel, bool primary, void *user)
{
  (void) channel;              /* Unused parameter */
  (void) user;                 /* Unused parameter */

  FillBufferFromSDcard((bool) wavHeader.channels, primary);

  if ( DMA->IF & DMA_IF_CH0DONE )           /* Did a DMA complete while   */
  {                                         /* reading from the SD Card ? */
    /* If FillBufferFromSDcard() takes too much time, we need to restart  */
    /* the pingpong machinery. This results in an audible click, which is */
    /* acceptable once in a while...                                      */
    DMA->IFC = DMA_IFC_CH0DONE;
    DMA_ActivatePingPong( 0,
                          false,
                          (void *) &(DAC0->COMBDATA),
                          (void *) &ramBufferDacData0Stereo,
                          BUFFERSIZE - 1,
                          (void *) &(DAC0->COMBDATA),
                          (void *) &ramBufferDacData1Stereo,
                          BUFFERSIZE - 1);
    return;
  }

  /* Stop DMA if bytecounter is equal to datasize or larger */
  bool stop = false;
  if (ByteCounter >= wavHeader.bytes_in_data)
  {
    stop = true;
  }

  /* Refresh the DMA control structure */
  DMA_RefreshPingPong(0,
                      primary,
                      false,
                      NULL,
                      NULL,
                      BUFFERSIZE - 1,
                      stop);
}

/**************************************************************************//**
 * @brief
 *   DAC Setup.
 * @details
 *   Setup DAC in stereo mode and triggered by PRS.
 *****************************************************************************/
void DAC_setup(void)
{
  DAC_Init_TypeDef        init        = DAC_INIT_DEFAULT;
  DAC_InitChannel_TypeDef initChannel = DAC_INITCHANNEL_DEFAULT;

  /* Calculate the DAC clock prescaler value that will result in a DAC clock
   * close to 1 MHz. Second parameter is zero, if the HFPERCLK value is 0, the
   * function will check what the HFPERCLK actually is. */
  init.prescale = DAC_PrescaleCalc(1000000, 0);

  /* Initialize the DAC. */
  DAC_Init(DAC0, &init);

  /* Enable prs to trigger samples at the right time with the timer */
  initChannel.prsEnable = true;
  initChannel.prsSel    = dacPRSSELCh0;

  /* Both channels can be configured the same
   * and be triggered by the same prs-signal. */
  DAC_InitChannel(DAC0, &initChannel, 0);
  DAC_InitChannel(DAC0, &initChannel, 1);

  DAC_Enable(DAC0, 0, true);
  DAC_Enable(DAC0, 1, true);
}

/**************************************************************************//**
 * @brief
 *   Setup TIMER for prs triggering of DAC conversion
 * @details
 *   Timer is set up to tick at the same frequency as the frequency described
 *   in the global .wav header. This will also cause a PRS trigger.
 *****************************************************************************/
void TIMER_setup(void)
{
  uint32_t timerTopValue;
  /* Use default timer configuration, overflow on counter top and start counting
   * from 0 again. */
  TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;

  TIMER_Init(TIMER0, &timerInit);

  /* PRS setup */
  /* Select TIMER0 as source and TIMER0OF (Timer0 overflow) as signal (rising edge) */
  PRS_SourceSignalSet(0, PRS_CH_CTRL_SOURCESEL_TIMER0, PRS_CH_CTRL_SIGSEL_TIMER0OF, prsEdgePos);

  /* Calculate the proper overflow value */
  timerTopValue = CMU_ClockFreqGet(cmuClock_TIMER0) / wavHeader.frequency;

  /* Write new topValue */
  TIMER_TopBufSet(TIMER0, timerTopValue);
}

/**************************************************************************//**
 * @brief
 *   Setup DMA in ping pong mode
 * @details
 *   The DMA is set up to transfer data from memory to the DAC, triggered by
 *   PRS (which in turn is triggered by the TIMER). When the DMA finishes,
 *   it will trigger the callback (PingPongTransferComplete).
 *****************************************************************************/
void DMA_setup(void)
{
  /* DMA configuration structs */
  DMA_Init_TypeDef       dmaInit;
  DMA_CfgChannel_TypeDef chnlCfg;
  DMA_CfgDescr_TypeDef   descrCfg;

  /* Initializing the DMA */
  dmaInit.hprot        = 0;
  dmaInit.controlBlock = dmaControlBlock;
  DMA_Init(&dmaInit);

  /* Set the interrupt callback routine */
  DMAcallBack.cbFunc = PingPongTransferComplete;

  /* Callback doesn't need userpointer */
  DMAcallBack.userPtr = NULL;

  /* Setting up channel */
  chnlCfg.highPri   = false; /* Can't use with peripherals */
  chnlCfg.enableInt = true;  /* Interrupt needed when buffers are used */

  /* channel 0 and 1 will need data at the same time,
   * can use channel 0 as trigger */

  chnlCfg.select = DMAREQ_DAC0_CH0;

  chnlCfg.cb = &DMAcallBack;
  DMA_CfgChannel(0, &chnlCfg);

  /* Setting up channel descriptor */
  /* Destination is DAC/USART register and doesn't move */
  descrCfg.dstInc = dmaDataIncNone;

  /* Transfer 32/16 bit each time to DAC_COMBDATA/USART_TXDOUBLE register*/
  descrCfg.srcInc = dmaDataInc4;
  descrCfg.size   = dmaDataSize4;

  /* We have time to arbitrate again for each sample */
  descrCfg.arbRate = dmaArbitrate1;
  descrCfg.hprot   = 0;

  /* Configure both primary and secondary descriptor alike */
  DMA_CfgDescr(0, true, &descrCfg);
  DMA_CfgDescr(0, false, &descrCfg);

  /* Enabling PingPong Transfer*/
  DMA_ActivatePingPong(0,
                       false,
                       (void *) &(DAC0->COMBDATA),
                       (void *) &ramBufferDacData0Stereo,
                       BUFFERSIZE - 1,
                       (void *) &(DAC0->COMBDATA),
                       (void *) &ramBufferDacData1Stereo,
                       BUFFERSIZE - 1);
}


/**************************************************************************//**
 * @brief
 *   Check if volume control pushbuttons are pressed.
 *****************************************************************************/
static void CheckVolume(void)
{
  uint16_t leds, buttons;

  if (checkVolume)
  {
    checkVolume = false;

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
      volumeAdjustFactor = Volume[ volume ];
      last_buttons       = buttons;
    }

    /* Use 14 leftmost leds for volume control indication. */
    leds = 0x00003FFF << (15 - volume);
    BSP_LedsSet((uint16_t) leds);
  }
}

/**************************************************************************//**
 * @brief
 *   Main function.
 * @details
 *   Configures the DK for sound output, reads the wav header and fills the data
 *   buffers. After the DAC, DMA, Timer and PRS are set up to perform playback
 *   the mainloop just enters em1 continuously.
 *****************************************************************************/
int main(void)
{
  UINT    bytes_read;
  FRESULT res;

  ByteCounter = 0;

  /* Use 32MHZ HFXO as core clock frequency, need high speed for 44.1kHz stereo */
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);

  /* Initialize DK board register access */
  BSP_Init(BSP_INIT_DEFAULT);

  volume             = 7;
  volumeAdjustFactor = Volume[ volume ];
  BSP_LedsSet((uint16_t)(0x00003FFF << (15 - volume)));

  /* Setup SysTick Timer for 10 msec interrupts  */
  if (SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 100))
  {
    while (1) ;
  }

  /* Enable Audio out driver */
  BSP_PeripheralAccess(BSP_AUDIO_OUT, true);

  /* Enable SPI access to MicroSD card */
  BSP_PeripheralAccess(BSP_MICROSD, true);

  /* Initialize filesystem */
  MICROSD_Init();
  res = f_mount(0, &Fatfs);
  if (res != FR_OK)
  {
    /* No micro-SD with FAT32 is present */
    while (1) ;
  }

  /* Open wav file from SD-card */
  if (f_open(&WAVfile, WAV_FILENAME, FA_READ) != FR_OK)
  {
    /* No micro-SD with FAT32, or no WAV_FILENAME found */
    while (1) ;
  }

  /* Read header and place in header struct */
  f_read(&WAVfile, &wavHeader, sizeof(wavHeader), &bytes_read);

  /* Start clocks */
  CMU_ClockEnable(cmuClock_DMA, true);
  CMU_ClockEnable(cmuClock_DAC0, true);
  CMU_ClockEnable(cmuClock_TIMER0, true);
  CMU_ClockEnable(cmuClock_PRS, true);

  /* Fill both primary and alternate RAM-buffer before start */
  FillBufferFromSDcard((bool) wavHeader.channels, true);
  FillBufferFromSDcard((bool) wavHeader.channels, false);

  /* Setup DMA and peripherals */
  DMA_setup();

  DAC_setup();

  /* Start timer which will trig DMA ... */
  TIMER_setup();

  while (1)
  {
    /* Enter EM1 while the DAC, Timer, PRS and DMA is working */
    EMU_EnterEM1();

    /* Check if time to check volume pushbuttons. */
    CheckVolume();
  }
}
