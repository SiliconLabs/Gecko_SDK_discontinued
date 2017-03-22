/**************************************************************************//**
 * @file main.c
 * @brief USB microphone audio device example.
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

#include <math.h>

#include "em_device.h"
#include "em_adc.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_prs.h"
#include "em_timer.h"

#include "bsp.h"
#include "bsp_trace.h"
#include "retargetserial.h"
#include "dmadrv.h"

#include "em_usb.h"
#include "descriptors.h"

/*
 *  Audio handling:
 *  ---------------
 *
 *                      The audio path explained.
 *                      =========================
 *
 *                                | Audio in
 *                                V
 *  +------+     +-----+       +------+       +-----+         +--------+
 *  |TIMER1|---->| PRS |------>| ADC0 |------>| DMA |-------->| Buffer |
 *  +------+     +-----+       +------+       +-----+         +--------+
 *                                               |                 |
 *                                               V                 |
 *                                            +-----+              |
 *                                            | DMA |              |
 *                                            | IRQ |              V
 *                                            +-----+         +--------+
 *                                               |            | PendSV | Process
 *                                               +----------->|   IRQ  | audio
 *                                               Trigger      +--------+
 *                                               PendSV            |
 *                                                                 V
 *                                            +-----+         +--------+
 *                          To USB host <-----| USB |<--------| Buffer |
 *                                            +-----+         +--------+
 *
 *
 *  1. TIMER1 is set to overflow sample rate times per second,
 *     triggering a pulse to the PRS module on each overflow.
 *
 *  2. ADC0 is configured to trigger a scan sequence (left+right channels) when
 *     PRS pulse occurs.
 *
 *  3. The DMA is using a ping-pong transfer type to receive sampled data from
 *     ADC0 into a pool of buffers (right and left channel interleaved). When
 *     the specified number of samples has been received, the DMA will trigger
 *     an interrupt and switch sampling into the next buffer. The DMA interrupt
 *     handler then fires the PendSV interrupt which will process audio data.
 *
 */

//#define DEBUG_USB_AUDIO           // Uncomment to add debugging features.
//#define TONE_GENERATOR            // Uncomment to operate as tonegenerator.

#if defined(DEBUG_USB_AUDIO)
#include <stdio.h>
#define DEBUG_PUTCHAR(c) putchar(c)
#define DEBUG_GPIO_PIN_TOGGLE(p) GPIO_PinOutToggle(gpioPortC, p)
#else
#define DEBUG_PUTCHAR(c)
#define DEBUG_GPIO_PIN_TOGGLE(p)
#endif

#define ISO_IN_EP           0x81    // Endpoint address
#define ADC_PRS_CHANNEL     0

// Bytes per USB frame (per ms) when audio sampling rate is 44.100Hz
// This number does not add up, so in every 10th frame we need to transfer an
// additional 4 bytes.
#define BYTES_PER_FRAME     176

// Configure one out of four possible ADC oversampling rates.
// ADC_OVS_RATE | ADC_OVS_SHIFT | ADC_OVS_VALUE  | ADC_OVS_RESOLUTION
// -------------|---------------|----------------|-------------------
//        1     |       4       | adcOvsRateSel2 |    adcRes12Bit
//        2     |       3       | adcOvsRateSel2 |    adcResOVS
//        4     |       2       | adcOvsRateSel4 |    adcResOVS
//        8     |       1       | adcOvsRateSel8 |    adcResOVS

#define ADC_OVS_RATE        8
#define ADC_OVS_SHIFT       1
#define ADC_OVS_VALUE       adcOvsRateSel8
#define ADC_OVS_RESOLUTION  adcResOVS

#define AUDIO_BUFFER_SIZE   MAX_BUFFER_SIZE
#define AUDIO_BUFFER_COUNT  8

static int setupCmd(const USB_Setup_TypeDef *setup);
static void stateChange(USBD_State_TypeDef oldState,
                        USBD_State_TypeDef newState);

#if defined(TONE_GENERATOR)
static double WFGEN_sinec_i16iq(uint32_t buffer[], int32_t bufferLength,
                                double f, double Fs, double amplitude,
                                double offset);
#else
static void timerSetup(uint32_t rate);
static void adcSetup(void);
static void adcDmaCallback(unsigned int channel,
                           bool primary,
                           void *user);
#endif

static int audioDataSent(USB_Status_TypeDef status,
                         uint32_t xferred,
                         uint32_t remaining);
static int muteSettingReceived(USB_Status_TypeDef status,
                               uint32_t xferred,
                               uint32_t remaining);

static const USBD_Callbacks_TypeDef callbacks =
{
  .usbReset        = NULL,
  .usbStateChange  = stateChange,
  .setupCmd        = setupCmd,
  .isSelfPowered   = NULL,
  .sofInt          = NULL
};

static const USBD_Init_TypeDef usbInitStruct =
{
  .deviceDescriptor    = &USBDESC_deviceDesc,
  .configDescriptor    = USBDESC_configDesc,
  .stringDescriptors   = USBDESC_strings,
  .numberOfStrings     = sizeof(USBDESC_strings)/sizeof(void*),
  .callbacks           = &callbacks,
  .bufferingMultiplier = USBDESC_bufferingMultiplier,
  .reserved            = 0
};

#if defined(TONE_GENERATOR)
STATIC_UBUF(sineBuffer1, AUDIO_BUFFER_SIZE);
STATIC_UBUF(sineBuffer2, AUDIO_BUFFER_SIZE);

#else
STATIC_UBUF(audioBuffer0, AUDIO_BUFFER_SIZE);
STATIC_UBUF(audioBuffer1, AUDIO_BUFFER_SIZE);
STATIC_UBUF(audioBuffer2, AUDIO_BUFFER_SIZE);
STATIC_UBUF(audioBuffer3, AUDIO_BUFFER_SIZE);
STATIC_UBUF(audioBuffer4, AUDIO_BUFFER_SIZE);
STATIC_UBUF(audioBuffer5, AUDIO_BUFFER_SIZE);
STATIC_UBUF(audioBuffer6, AUDIO_BUFFER_SIZE);
STATIC_UBUF(audioBuffer7, AUDIO_BUFFER_SIZE);

static struct
{
  int     len;
  uint8_t *buffer;
} audioArray[ AUDIO_BUFFER_COUNT ] =
{
  {0, audioBuffer0},
  {0, audioBuffer1},
  {0, audioBuffer2},
  {0, audioBuffer3},
  {0, audioBuffer4},
  {0, audioBuffer5},
  {0, audioBuffer6},
  {0, audioBuffer7},
};

static DMA_CB_TypeDef         adcDmaCB;
static int                    adcIndex;
static int                    audioProcessIndex;
static unsigned int           adcDmaId;
static unsigned int           adcFrameCnt;
#endif

STATIC_UBUF(silenceBuffer, AUDIO_BUFFER_SIZE);

static bool                   mute = false;
static uint32_t               smallBuffer;
static uint16_t               altSetting = 0;
static unsigned int           usbFrameCnt;
static int                    usbTxCnt;

/**************************************************************************//**
 * @brief main - the entrypoint after reset.
 *****************************************************************************/
int main(void)
{
  CHIP_Init();                  // Handle chip errata.
  BSP_Init(BSP_INIT_DEFAULT);   // Initialize DK board register access.

  // If first word of user data page is non-zero, enable eA Profiler trace.
  BSP_TraceProfilerSetup();

  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);

#if defined(DEBUG_USB_AUDIO)
  // Initialize DK USART port for debugging purposes.
  RETARGET_SerialInit();        // Initialize USART
  RETARGET_SerialCrLf(1);       // Map LF to CRLF
  printf("\nEFM32 USB Microphone Audio example\n");

  // Setup GPIO to trigger scope for debugging.
  GPIO_PinModeSet(gpioPortC, 0, gpioModePushPull, 0);
  GPIO_PinModeSet(gpioPortC, 1, gpioModePushPull, 0);
  GPIO_PinModeSet(gpioPortC, 2, gpioModePushPull, 0);
#endif

  memset(silenceBuffer, 0, sizeof(silenceBuffer));

#if defined(TONE_GENERATOR)
  // Generate sine wave data patterns for use in tone generator mode.
  WFGEN_sinec_i16iq((uint32_t*)sineBuffer1,   // uint32_t buffer[]
                    BYTES_PER_FRAME/4,        // int32_t bufferLength
                    1000.0,                   // double frequency
                    44100.0,                  // double sample rate
                    750.0,                    // double amplitude
                    0.0);                     // double offset
  WFGEN_sinec_i16iq((uint32_t*)sineBuffer2,   // uint32_t buffer[]
                    (BYTES_PER_FRAME+4)/4,    // int32_t bufferLength
                    1000.0,                   // double frequency
                    44100.0,                  // double sample rate
                    750.0,                    // double amplitude
                    0.0);                     // double offset

#else
  // Configure ADC DMA, PRS etc.
  adcSetup();

  // Setup timer, this will start ADC sampling.
  timerSetup(44118);    // This is the closest we get to 44.100 kHz
#endif

  // Initialize and start USB device stack.
  USBD_Init(&usbInitStruct);

  for(;;)
  {
  }
}

/**************************************************************************//**
 * @brief
 *   Handle USB setup commands.
 *
 * @param[in] setup Pointer to the setup packet received.
 *
 * @return USB_STATUS_OK if command accepted.
 *         USB_STATUS_REQ_UNHANDLED when command is unknown, the USB device
 *         stack will handle the request.
 *****************************************************************************/
static int setupCmd(const USB_Setup_TypeDef *setup)
{
  int       retVal;
  uint8_t   *pBuffer = (uint8_t*) &smallBuffer;

  retVal = USB_STATUS_REQ_UNHANDLED;

  if(setup->Type == USB_SETUP_TYPE_CLASS)
  {
    switch(setup->bRequest)
    {
      case USB_AUDIO_GET_CUR:
      /********************/
        if((setup->Direction == USB_SETUP_DIR_IN)
           && (setup->Recipient == USB_SETUP_RECIPIENT_INTERFACE)
           && (setup->wLength == 1))
        {
          if((setup->wIndex == 0x0200)
             && (setup->wValue == 0x0100))
          {
            // wIndex LSB is interface no, must be 0
            // wIndex MSB is entityID, must be 2 ("Feature Unit ID2")
            // wValue LSB is channel number, must be 0 (master)
            // wValue MSB is control selector, must be 1 (first control is mute)
            *pBuffer = mute;
            retVal = USBD_Write(0, pBuffer, setup->wLength, NULL);
          }
        }
        break;

      case USB_AUDIO_SET_CUR:
      /********************/
        if((setup->Direction == USB_SETUP_DIR_OUT)
           && (setup->Recipient == USB_SETUP_RECIPIENT_INTERFACE)
           && (setup->wLength == 1))
        {
          if((setup->wIndex == 0x0200)
             && (setup->wValue == 0x0100))
          {
            // wIndex LSB is interface no, must be 0
            // wIndex MSB is entityID, must be 2 ("Feature Unit ID2")
            // wValue LSB is channel number, must be 0 (master)
            // wValue MSB is control selector, must be 1 (first control is mute)
            retVal = USBD_Read(0, pBuffer, setup->wLength, muteSettingReceived);
          }
        }
        break;
    }
  }

  // Re-implement standard SET/GET_INTERFACE commands.
  else if(setup->Type == USB_SETUP_TYPE_STANDARD)
  {
    if((setup->bRequest == SET_INTERFACE)
       && (setup->wIndex == 1)        // Interface number
       && (setup->wLength == 0)
       && (setup->Recipient == USB_SETUP_RECIPIENT_INTERFACE))
    {
      // setup->wValue contains a new Alternate Setting value
      if(setup->wValue == 0)          // The zero bandwidth interface
      {
        altSetting = setup->wValue;
        retVal = USB_STATUS_OK;
        DEBUG_PUTCHAR('Z');
      }
      else if(setup->wValue == 1)     // The normal bandwidth interface
      {
        altSetting  = setup->wValue;
#if !defined(TONE_GENERATOR)
        adcFrameCnt = 0;
#endif
        usbTxCnt    = 1;
        usbFrameCnt = 0;
        retVal = USB_STATUS_OK;
        DEBUG_PUTCHAR('N');
      }
    }

    else if((setup->bRequest == GET_INTERFACE)
            && (setup->wValue == 0)
            && (setup->wIndex == 1)   // Interface number
            && (setup->wLength == 1)
            && (setup->Recipient == USB_SETUP_RECIPIENT_INTERFACE))
    {
      *pBuffer = (uint8_t)altSetting;
      retVal = USBD_Write(0, pBuffer, 1, NULL);
    }
  }

  return retVal;
}

/**************************************************************************//**
 * @brief Callback called when a new mute on/off setting is received.
 *
 * @param[in] status
 *   The transfer status.
 *
 * @param[in] xferred
 *   Number of bytes actually transferred.
 *
 * @param[in] remaining
 *   Number of bytes not transferred.
 *
 * @return
 *   @ref USB_STATUS_OK on success, else an appropriate error code.
 *****************************************************************************/
static int muteSettingReceived(USB_Status_TypeDef status,
                               uint32_t xferred,
                               uint32_t remaining)
{
  (void)status;
  (void)xferred;
  (void)remaining;

  mute = (bool)(smallBuffer & 0xFF);

  if(mute)
  {
    DEBUG_PUTCHAR('M');
  }
  else
  {
    DEBUG_PUTCHAR('m');
  }

  return USB_STATUS_OK;
}

/**************************************************************************//**
 * @brief
 *   Called whenever the USB device has changed its device state.
 *
 * @param[in] oldState
 *   The device USB state just leaved. See USBD_State_TypeDef.
 *
 * @param[in] newState
 *   New (the current) USB device state. See USBD_State_TypeDef.
 *****************************************************************************/
static void stateChange(USBD_State_TypeDef oldState,
                        USBD_State_TypeDef newState)
{
  if(newState == USBD_STATE_CONFIGURED)
  {
    // We have been configured, start sending audio data.
    usbTxCnt    = 1;
    usbFrameCnt = 0;
#if defined(TONE_GENERATOR)
    USBD_Write(ISO_IN_EP, sineBuffer1, BYTES_PER_FRAME, audioDataSent);
#else
    adcFrameCnt = 0;
    USBD_Write(ISO_IN_EP, silenceBuffer, BYTES_PER_FRAME, audioDataSent);
#endif
  }

  else if(oldState == USBD_STATE_CONFIGURED)
  {
    // We have been de-configured.
    USBD_AbortTransfer(ISO_IN_EP);

#if !defined(TONE_GENERATOR)
    // DMA restart necessary ?
    // The abort function above may take more time than one DMA cycle, in which
    // case we will have to restart the DMA.
    if(DMA->IF & (1 << adcDmaId))
    {
      DMA->IFC = (1 << adcDmaId);
      adcIndex          = 1;
      audioProcessIndex = 0;
      DMA_ActivatePingPong(adcDmaId,
                           false,
                           audioBuffer0,
                           (void *)((uint32_t) &(ADC0->SCANDATA)),
                           (BYTES_PER_FRAME/2) - 1,               // DMA lenght
                           audioBuffer1,
                           (void *)((uint32_t) &(ADC0->SCANDATA)),
                           (BYTES_PER_FRAME/2) - 1);              // DMA lenght
    }
#endif
  }

  if(newState == USBD_STATE_SUSPENDED)
  {
    // We have been suspended.
    // Reduce current consumption to below 2.5 mA.
  }
}

#if !defined(TONE_GENERATOR)
/**************************************************************************//**
 * @brief Callback called when a DMA transfer has completed.

 * @param[in] channel - DMA channel no. the callback function is invoked for.
 * @param[in] primary - Indicates if callback is invoked for completion of
 *                      primary (true) or alternate (false) descriptor.
 * @param[in] user - User definable reference (not used here).
 *****************************************************************************/
static void adcDmaCallback(unsigned int channel,
                           bool primary,
                           void *user)
{
  (void)user;

  DEBUG_GPIO_PIN_TOGGLE(1);

  adcIndex = (adcIndex + 1) % AUDIO_BUFFER_COUNT;
  audioArray[adcIndex].len = BYTES_PER_FRAME;

  if(altSetting == 1)   // Is normal bandwidth interface active ?
  {
    adcFrameCnt++;

    if(adcFrameCnt > usbFrameCnt)
    {
      audioArray[adcIndex].len += 4;
      DEBUG_GPIO_PIN_TOGGLE(2);
    }
  }

  DMA_RefreshPingPong(channel,
                      primary,
                      false,
                      audioArray[adcIndex].buffer,      // DMA data destination
                      NULL,
                      (audioArray[adcIndex].len/2) - 1, // DMA lenght
                      false);

  // Trigger lower priority interrupt which will process audio data.
  SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
}
#endif

/**************************************************************************//**
 * @brief Callback called when a USB data transfer has completed.
 *        Fire off a new USB transfer.
 *        Keeps track of number of samples sent vs. expected, and do sample
 *        insertion removal as needed.
 *
 * @param[in] status
 *   The transfer status.
 *
 * @param[in] xferred
 *   Number of bytes actually transferred.
 *
 * @param[in] remaining
 *   Number of bytes not transferred.
 *
 * @return
 *   @ref USB_STATUS_OK on success, else an appropriate error code.
 *****************************************************************************/
static int audioDataSent(USB_Status_TypeDef status,
                         uint32_t xferred,
                         uint32_t remaining)
{
#if !defined(TONE_GENERATOR)
  int index, cnt;
  int16_t *pSample;
  int32_t sample;
#endif
  int expected;
  uint8_t *pBuffer;

  (void)xferred;
  (void)remaining;

  if(status == USB_STATUS_OK)
  {
    DEBUG_GPIO_PIN_TOGGLE(0);
    usbFrameCnt++;
    usbTxCnt++;

    if(usbTxCnt == 10)
    {
      // Send extra sample on every 10th USB frame to ensure samples add
      // up to 44100 per second.
      usbTxCnt = 0;
      expected = BYTES_PER_FRAME + 4;
    }
    else
    {
      expected = BYTES_PER_FRAME;
    }

#if defined(TONE_GENERATOR)
    if(mute)
    {
      pBuffer = silenceBuffer;
    }
    else if(expected == BYTES_PER_FRAME + 4)
    {
      pBuffer = sineBuffer2;
    }
    else
    {
      pBuffer = sineBuffer1;
    }

#else
    if(mute)
    {
      pBuffer = silenceBuffer;
    }
    else
    {
      // Lag 2 buffers behind the audioProcessIndex
      index   = (audioProcessIndex + AUDIO_BUFFER_COUNT - 2)
                % AUDIO_BUFFER_COUNT;
      pBuffer = audioArray[index].buffer;
      cnt     = audioArray[index].len;

      if(cnt != expected)
      {
        if (expected == BYTES_PER_FRAME + 4)
        {
          // Insert a sample in each channel.
          // Average last sample and next to last sample and insert the new
          // sample between last and next to last sample.
          pSample    = (int16_t*)&pBuffer[cnt - 8];
          pSample[4] = pSample[2];
          pSample[5] = pSample[3];
          sample     = (pSample[0] + pSample[4] + 1) / 2;
          pSample[2] = sample;
          sample     = (pSample[1] + pSample[5] + 1) / 2;
          pSample[3] = sample;
        }
        else
        {
          // Remove a sample from each channel.
          // Average last 4 samples (N,N-1,N-2,N-3),
          // replace sample N-2 with the average,
          // replace sample N-1 with N
          pSample    = (int16_t*)&pBuffer[cnt - 16];
          sample     = (pSample[0] + pSample[2] + pSample[4] + pSample[6] + 2)
                        / 4;
          pSample[2] = sample;
          sample     = (pSample[1] + pSample[3] + pSample[5] + pSample[7] + 2)
                        / 4;
          pSample[3] = sample;
          pSample[4] = pSample[6];
          pSample[5] = pSample[7];
        }
      }
    }
#endif

    USBD_Write(ISO_IN_EP, pBuffer, expected, audioDataSent);
  }
  return USB_STATUS_OK;
}

#if !defined(TONE_GENERATOR)
/**************************************************************************//**
 * @brief Do audio processing.
 *        Compensate for oversampling, convert to 2nd complement numbers
 *        (amplitude Zero is at adcRefVDD/2).
 *****************************************************************************/
void PendSV_Handler(void)
{
  int       i;
  int16_t   signedInt16;
  int16_t   *pAudio = (int16_t*)audioArray[audioProcessIndex].buffer;
  int       len     = audioArray[audioProcessIndex].len / 2;

  DEBUG_GPIO_PIN_TOGGLE(2);

  for(i=0; i<len; i++)
  {
    // Get sign bit in correct place and convert to 2nd compl. form.
    signedInt16 = (*pAudio << ADC_OVS_SHIFT) + 0x8000;

    // Divide to compensate for ADC oversampling.
    *pAudio++ = signedInt16 / ADC_OVS_RATE;
  }

  audioProcessIndex = (audioProcessIndex + 1) % AUDIO_BUFFER_COUNT;

  DEBUG_GPIO_PIN_TOGGLE(2);
}

/**************************************************************************//**
 * @brief Configure ADC, DMA and PRS audio sampling.
 *****************************************************************************/
static void adcSetup(void)
{
  DMA_CfgDescr_TypeDef    adcDescrCfg;
  DMA_CfgChannel_TypeDef  adcChCfg;
  ADC_Init_TypeDef        adcInit     = ADC_INIT_DEFAULT;
  ADC_InitScan_TypeDef    adcScanInit = ADC_INITSCAN_DEFAULT;

  // Route AUDIO IN jack to ADC.
  BSP_PeripheralAccess(BSP_AUDIO_IN, true);

  // Enable clocks and PRS
  CMU_ClockEnable(cmuClock_ADC0, true);
  CMU_ClockEnable(cmuClock_DMA, true);
  CMU_ClockEnable(cmuClock_PRS, true);
  PRS_LevelSet(0, 1 << (ADC_PRS_CHANNEL + _PRS_SWLEVEL_CH0LEVEL_SHIFT));
  PRS_SourceSignalSet(ADC_PRS_CHANNEL,
                      PRS_CH_CTRL_SOURCESEL_TIMER1,
                      PRS_CH_CTRL_SIGSEL_TIMER1OF,
                      prsEdgePos);

  // Prepare DMA
  DMADRV_Init();
  DMADRV_AllocateChannel(&adcDmaId,NULL);

  // Set PendSV priority to lowest.
  NVIC_SetPriority(PendSV_IRQn, (1 << __NVIC_PRIO_BITS) - 1);

  // Configure DMA
  adcDmaCB.cbFunc  = adcDmaCallback;
  adcDmaCB.userPtr = NULL;

  adcChCfg.highPri   = true;
  adcChCfg.enableInt = true;
  adcChCfg.select    = dmadrvPeripheralSignal_ADC0_SCAN;
  adcChCfg.cb        = &adcDmaCB;
  DMA_CfgChannel(adcDmaId, &adcChCfg);

  adcDescrCfg.dstInc  = dmaDataInc2;
  adcDescrCfg.srcInc  = dmaDataIncNone;
  adcDescrCfg.size    = dmaDataSize2;
  adcDescrCfg.arbRate = dmaArbitrate1;
  adcDescrCfg.hprot   = 0;
  DMA_CfgDescr(adcDmaId, true,  &adcDescrCfg);
  DMA_CfgDescr(adcDmaId, false, &adcDescrCfg);

  adcIndex          = 1;
  audioProcessIndex = 0;
  DMA_ActivatePingPong(adcDmaId,
                       false,
                       audioBuffer0,
                       (void *)((uint32_t) &(ADC0->SCANDATA)),
                       (BYTES_PER_FRAME/2) - 1,                 // DMA lenght
                       audioBuffer1,
                       (void *)((uint32_t) &(ADC0->SCANDATA)),
                       (BYTES_PER_FRAME/2) - 1);                // DMA lenght

  // Configure ADC.
  // Keep warm due to "high" frequency sampling.
  adcInit.warmUpMode = adcWarmupKeepADCWarm;
  adcInit.timebase   = ADC_TimebaseCalc(0);
  adcInit.prescale   = ADC_PrescaleCalc(12000000, 0);
  adcInit.tailgate   = true;
  adcInit.ovsRateSel = ADC_OVS_VALUE;
  ADC_Init(ADC0, &adcInit);

  // Init for scan sequence use (audio interleaved in left/right channels).
  adcScanInit.prsSel     = adcPRSSELCh0;
  adcScanInit.prsEnable  = true;
  adcScanInit.reference  = adcRefVDD;
  adcScanInit.input      = ADC_SCANCTRL_INPUTMASK_CH6
                           | ADC_SCANCTRL_INPUTMASK_CH7;
  adcScanInit.resolution = ADC_OVS_RESOLUTION;
  ADC_InitScan(ADC0, &adcScanInit);
}

/**************************************************************************//**
 * @brief Setup TIMER1 to support selected sample rate.
 * @param[in] rate - Rate in Hz.
 *****************************************************************************/
static void timerSetup(uint32_t rate)
{
  TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;

  CMU_ClockEnable(cmuClock_TIMER1, true);
  // Trigger sampling according to configured sample rate.
  TIMER_TopSet(TIMER1, ((CMU_ClockFreqGet(cmuClock_HFPER) + (rate/2)) / rate)
                       - 1);
  TIMER_Init(TIMER1, &timerInit);
}
#endif

#if defined(TONE_GENERATOR)
/**************************************************************************//**
 * @brief Generate a sine test tone.
 * @param[in] buffer          Pointer to data buffer
 * @param[in] bufferLength    Lenght of data buffer
 * @param[in] f               Frequency
 * @param[in] Fs              Sample rate
 * @param[in] amplitude       Amplitude
 * @param[in] offset          Offset
 *****************************************************************************/
static double WFGEN_sinec_i16iq(uint32_t buffer[], int32_t bufferLength,
                                double f, double Fs, double amplitude,
                                double offset)
{
  #define WFGEN_CONST_TWOPI (2.0*3.14159265358979)

  int n;
  double fs;            // Normalised frequency.
  double spp;           // Samples per period.
  int periods;          // Integer number of periods.
  int16_t *buf;
  double temp;
  double c;

  //
  // Calculate a normalised frequency so that the number of discrete-time
  // samples available in the buffer matches exactly an integer
  // number of periods.
  //

  spp     = Fs/f;
  temp    = (double)bufferLength / spp;
  periods = (int)(temp + 0.5);
  fs      = (double)periods / (double)bufferLength;

  // Calculate sine samples.
  c = WFGEN_CONST_TWOPI*fs;
  buf = (int16_t *)&buffer[0];
  for(n=0; n<bufferLength; n++)
  {
    *buf++ = (int16_t)(floor(offset + amplitude*sin(c*(double)n) + 0.5));
    *buf++ = (int16_t)(floor(offset + amplitude*sin(c*(double)n) + 0.5));
  }

  return fs;
}
#endif
