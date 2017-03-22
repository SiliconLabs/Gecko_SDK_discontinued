/**************************************************************************//**
 * @file main.c
 * @brief USB headphone audio device example.
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
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_usart.h"

#include "bsp.h"
#include "bsp_trace.h"
#include "retargetserial.h"
#include "dmadrv.h"

#include "em_usb.h"
#include "descriptors.h"

//#define DEBUG_USB_AUDIO           // Uncomment to add debugging features

#if defined(DEBUG_USB_AUDIO)
#include <stdio.h>
#define DEBUG_PUTCHAR(c) putchar(c)
#define DEBUG_GPIO_PIN_TOGGLE(p) GPIO_PinOutToggle(gpioPortC, p)
#else
#define DEBUG_PUTCHAR(c)
#define DEBUG_GPIO_PIN_TOGGLE(p)
#endif

#define ISO_OUT_EP          1     // Endpoint address
#define AUDIO_DATA_TIMER    0     // Timer id
#define AUDIO_DATA_TIMEOUT  4     // Timeout in milliseconds

// Figure out how many bytes we will receive per USB frame (per ms) when
// accounting for up to 96 kHz sampling rate (stereo, 16 bit per sample).
#define AUDIO_BUFFER_SIZE (96000/250)
#define AUDIO_BUFFER_COUNT 8

static int setupCmd(const USB_Setup_TypeDef *setup);
static void stateChange(USBD_State_TypeDef oldState,
                        USBD_State_TypeDef newState );
static int audioDataReceived(USB_Status_TypeDef status,
                             uint32_t xferred,
                             uint32_t remaining);
static int muteSettingReceived(USB_Status_TypeDef status,
                               uint32_t xferred,
                               uint32_t remaining);
static void i2sSetup(int sampleRate);
static void dacDmaCallback(unsigned int channel,
                           bool primary,
                           void *user);
static void dacDmaSetup(void);
static void audioDataTimeout(void);
static void goToSilence(void);

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

STATIC_UBUF(audioBuffer0, AUDIO_BUFFER_SIZE);
STATIC_UBUF(audioBuffer1, AUDIO_BUFFER_SIZE);
STATIC_UBUF(audioBuffer2, AUDIO_BUFFER_SIZE);
STATIC_UBUF(audioBuffer3, AUDIO_BUFFER_SIZE);
STATIC_UBUF(audioBuffer4, AUDIO_BUFFER_SIZE);
STATIC_UBUF(audioBuffer5, AUDIO_BUFFER_SIZE);
STATIC_UBUF(audioBuffer6, AUDIO_BUFFER_SIZE);
STATIC_UBUF(audioBuffer7, AUDIO_BUFFER_SIZE);
STATIC_UBUF(silenceBuffer, AUDIO_BUFFER_SIZE);

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

static bool                   mute    = false;
static bool                   silence = true;
static bool                   audioOn = false;
static int                    audioStartCnt;

static uint32_t               smallBuffer;
static int                    usbIndex;
static unsigned int           usbFrameCnt;
static unsigned int           dacFrameCnt;
static int                    dacIndex;
static unsigned int           dacDmaId;
static DMA_CB_TypeDef         dacDmaCB;
static DMA_CfgChannel_TypeDef dacDmaChCfg;
static DMA_CfgDescr_TypeDef   dacDmaDescrCfg;
static uint16_t               altSetting = 0;

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
  // Initialize DK UART port for debugging purposes.
  RETARGET_SerialInit();        // Initialize USART
  RETARGET_SerialCrLf(1);       // Map LF to CRLF
  printf("\nEFM32 USB Headphone Audio example\n");

  // Setup GPIO to trigger scope for debugging.
  GPIO_PinModeSet(gpioPortC, 0, gpioModePushPull, 0);
  GPIO_PinModeSet(gpioPortC, 1, gpioModePushPull, 0);
  GPIO_PinModeSet(gpioPortC, 2, gpioModePushPull, 0);
#endif

  i2sSetup(44118);    // This is the closest we get to 44.100 kHz
  dacDmaSetup();

  // Start DMA, feed "silence" to dac.
  memset(silenceBuffer, 0, sizeof(silenceBuffer));
  DMA_ActivatePingPong(dacDmaId,
                       false,
                       (void*)&USART1->TXDOUBLE,
                       silenceBuffer,
                       (176/2)-1,
                       (void*)&USART1->TXDOUBLE,
                       silenceBuffer,
                       (176/2)-1);

  // Initialize and start USB device stack.
  USBD_Init(&usbInitStruct);

  for(;;)
  {
    EMU_EnterEM1();
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
    if ((setup->bRequest == SET_INTERFACE)
        && (setup->wIndex == 1)       // Interface number
        && (setup->wLength == 0)
        && (setup->Recipient == USB_SETUP_RECIPIENT_INTERFACE))
    {
      // setup->wValue contains a new Alternate Setting value
      if(setup->wValue == 0)          // The zero bandwidth interface
      {
        altSetting = setup->wValue;
        goToSilence();
        retVal = USB_STATUS_OK;
        DEBUG_PUTCHAR('Z');
      }
      else if(setup->wValue == 1)     // The normal bandwidth interface
      {
        altSetting = setup->wValue;
        retVal = USB_STATUS_OK;
        DEBUG_PUTCHAR('N');
      }
    }

    else if ((setup->bRequest == GET_INTERFACE)
             && (setup->wValue == 0)
             && (setup->wIndex == 1)  // Interface number
             && (setup->wLength == 1)
             && (setup->Recipient == USB_SETUP_RECIPIENT_INTERFACE))
    {
      *pBuffer = (uint8_t)altSetting;
      retVal = USBD_Write( 0, pBuffer, 1, NULL);
    }
  }

  return retVal;
}

/**************************************************************************//**
 * @brief Callback called when a new mute on/off setting is received.
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
    goToSilence();
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
                        USBD_State_TypeDef newState )
{
  if ( newState == USBD_STATE_CONFIGURED )
  {
    // We have been configured, start reading audio data.
    USBD_Read(ISO_OUT_EP,
              audioArray[usbIndex].buffer,
              AUDIO_BUFFER_SIZE,
              audioDataReceived );
  }

  else if ( oldState == USBD_STATE_CONFIGURED )
  {
    // We have been de-configured.
    USBTIMER_Stop(AUDIO_DATA_TIMER);
    USBD_AbortTransfer(ISO_OUT_EP);

    // DMA restart necessary ?
    // The abort function above may take more time than one DMA cycle, in which
    // case we will have to restart the DMA.
    if( DMA->IF & (1 << dacDmaId))
    {
      DMA->IFC = (1 << dacDmaId);
      DMA_ActivatePingPong(dacDmaId,
                           false,
                           (void*)&USART1->TXDOUBLE,
                           silenceBuffer,
                           (176/2)-1,
                           (void*)&USART1->TXDOUBLE,
                           silenceBuffer,
                           (176/2)-1);
    }
  }

  if ( newState == USBD_STATE_SUSPENDED )
  {
    // We have been suspended.
    // Reduce current consumption to below 2.5 mA.
  }
}

/**************************************************************************//**
 * @brief Callback called when a DMA transfer has completed.
 *****************************************************************************/
static void dacDmaCallback(unsigned int channel,
                           bool primary,
                           void *user)
{
  int16_t *pSample;
  int32_t sample;
  (void)user;

  DEBUG_GPIO_PIN_TOGGLE(1);

  if(silence || !audioOn)
  {
    DMA_RefreshPingPong(channel,
                        primary,
                        false,
                        NULL,
                        silenceBuffer,    // DMA data source
                        (176/2)-1,        // Data length
                        false);
  }
  else
  {
    dacFrameCnt++;
    dacIndex = (dacIndex + 1) % AUDIO_BUFFER_COUNT;

    if(dacFrameCnt > usbFrameCnt)
    {
      // Insert an extra sample to compensate for the sample rate skew.
      // Average last sample and next to last sample.
      // Move last sample to next sample position.
      // Replace last sample with the averaged value.
      // The above is done both for left and right channels.
      pSample = (int16_t*)
                &audioArray[dacIndex].buffer[audioArray[dacIndex].len - 8];
      pSample[4] = pSample[2];
      pSample[5] = pSample[3];
      sample     = (pSample[0] + pSample[4] + 1) / 2;
      pSample[2] = sample;
      sample     = (pSample[1] + pSample[5] + 1) / 2;
      pSample[3] = sample;

      audioArray[dacIndex].len += 4;
      DEBUG_GPIO_PIN_TOGGLE(2);
    }

    DMA_RefreshPingPong(channel,
                        primary,
                        false,
                        NULL,
                        audioArray[dacIndex].buffer,        // DMA data source
                        (audioArray[dacIndex].len/2) - 1,   // Data length
                        false);
  }
}

/**************************************************************************//**
 * @brief Callback called when a USB data transfer has completed.
 *****************************************************************************/
static int audioDataReceived(USB_Status_TypeDef status,
                             uint32_t xferred,
                             uint32_t remaining)
{
  (void)remaining;

  if(status == USB_STATUS_OK)
  {
    DEBUG_GPIO_PIN_TOGGLE(0);
    usbFrameCnt++;

    // Checkif we shall exit "silent" mode.
    if(silence && !mute)
    {
      silence       = false;
      usbIndex      = 0;
      dacIndex      = 0;
      audioStartCnt = 0;
    }

    if(!audioOn && !silence)
    {
      audioStartCnt++;

      // We let USB fill a few buffers before switching to audio data
      if(audioStartCnt == 6)
      {
        audioOn     = true;
        dacIndex    = 1;
        usbFrameCnt = 0;
        dacFrameCnt = 0;
      }
    }

    audioArray[usbIndex].len = xferred;
    usbIndex = (usbIndex + 1) % AUDIO_BUFFER_COUNT;
    USBD_Read(ISO_OUT_EP,
              audioArray[usbIndex].buffer,
              AUDIO_BUFFER_SIZE,
              audioDataReceived);
    USBTIMER_Start(AUDIO_DATA_TIMER, AUDIO_DATA_TIMEOUT, audioDataTimeout);
  }
  return USB_STATUS_OK;
}

/**************************************************************************//**
 * @brief Timeout callback on audio data reception. Used to detect when
 *        when host stop sending audio data.
 *****************************************************************************/
static void audioDataTimeout(void)
{
  // Start sending "silence" data to dac.
  goToSilence();
}

/**************************************************************************//**
 * @brief Set state variables so that we will feed dac with "silence".
 *****************************************************************************/
static void goToSilence(void)
{
  if(!silence)
  {
    silence       = true;
    audioOn       = false;
    audioStartCnt = 0;
  }
}

/**************************************************************************//**
 * @brief
 *   Setup USART1 in I2S mode.
 * @details
 *   USART1 is initialized in I2S mode to feed the DS1334 I2S dac.
 *****************************************************************************/
static void i2sSetup(int sampleRate)
{
  USART_InitI2s_TypeDef init = USART_INITI2S_DEFAULT;

  BSP_PeripheralAccess(BSP_I2S, true);
  CMU_ClockEnable(cmuClock_USART1, true);

  // Use USART location 1.
  // USART TX  = I2S DATA - Pin D0
  // USART CLK = I2S_SCLK - Pin D2
  // USART CS  = I2S_WS   - Pin D3

  GPIO_PinModeSet(gpioPortD, 0, gpioModePushPull, 0);
  GPIO_PinModeSet(gpioPortD, 2, gpioModePushPull, 0);
  GPIO_PinModeSet(gpioPortD, 3, gpioModePushPull, 0);

  // Configure USART for basic I2S operation.
  init.sync.baudrate = sampleRate * 32;
  USART_InitI2s(USART1, &init);

  // Enable pins at location 1.
  USART1->ROUTE = USART_ROUTE_TXPEN
                  | USART_ROUTE_CSPEN
                  | USART_ROUTE_CLKPEN
                  | USART_ROUTE_LOCATION_LOC1;
}

/**************************************************************************//**
 * @brief
 *   Configure a DMA channel for transferring data to the i2s dac.
 *****************************************************************************/
static void dacDmaSetup(void)
{
  DMADRV_Init();
  DMADRV_AllocateChannel(&dacDmaId,NULL);

  dacDmaCB.cbFunc  = dacDmaCallback;
  dacDmaCB.userPtr = NULL;

  dacDmaChCfg.highPri   = false;
  dacDmaChCfg.enableInt = true;
  dacDmaChCfg.select    = dmadrvPeripheralSignal_USART1_TXBL;
  dacDmaChCfg.cb        = &dacDmaCB;
  DMA_CfgChannel(dacDmaId, &dacDmaChCfg);

  dacDmaDescrCfg.dstInc  = dmaDataIncNone;
  dacDmaDescrCfg.srcInc  = dmaDataInc2;
  dacDmaDescrCfg.size    = dmaDataSize2;
  dacDmaDescrCfg.arbRate = dmaArbitrate1;
  dacDmaDescrCfg.hprot   = 0;
  DMA_CfgDescr(dacDmaId, true,  &dacDmaDescrCfg);
  DMA_CfgDescr(dacDmaId, false, &dacDmaDescrCfg);
}
