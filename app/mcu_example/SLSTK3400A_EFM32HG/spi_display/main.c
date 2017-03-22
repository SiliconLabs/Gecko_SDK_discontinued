/**************************************************************************//**
 * @file main.c
 * @brief Low energy SPI transfer example for STK3200
 * @version 5.1.2
 *
 ******************************************************************************
 * @section License
 * <b>Copyright 2016 Silicon Labs, Inc. http://www.silabs.com</b>
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
#include "em_dma.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_rtc.h"
#include "em_usart.h"
#include "dmactrl.h"
#include "frames.h"
#include "displayls013b7dh03config.h"
#include "displaypalconfig.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#define POLARITY_INVERSION_FREQ     60
#define DISPLAY_TOGGLE_FREQ         2
#define DISPLAY_COMMAND_CLEAR       0x4
#define DMA_CHANNEL                 0
#define DMA_MAX_UNIT_TRANSFERS      1024
#define TRANSFERS_PER_FRAME         3

static volatile bool dmaRunning = false;
static volatile bool displayOn  = true;
static volatile uint8_t extcominToggles = 0;

static DMA_CB_TypeDef callback;

static DMA_DESCRIPTOR_TypeDef frameTransferBlock0[TRANSFERS_PER_FRAME];
static DMA_DESCRIPTOR_TypeDef frameTransferBlock1[TRANSFERS_PER_FRAME];

static void spiStartTransfer(DMA_DESCRIPTOR_TypeDef *tasks);
static void spiEndTransfer(unsigned int channel, bool primary, void *user);
static void setScatterGatherBlock(void);

/***************************************************************************//**
 * @brief
 *   Setup clocks required to run the example.
 ******************************************************************************/
static void setupClocks(void)
{
  // Choose the ULFRCO for the LFA clock domain (RTC)
  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_ULFRCO);

  // Enable low frequency peripherals (RTC and DMA)
  CMU_ClockEnable(cmuClock_HFLE, true);
  CMU_ClockEnable(cmuClock_RTC,  true);
  CMU_ClockEnable(cmuClock_DMA,  true);

  // Enable high frequency peripherals (GPIO and USART)
  CMU_ClockEnable(cmuClock_HFPER,      true);
  CMU_ClockEnable(cmuClock_GPIO,       true);
  CMU_ClockEnable(PAL_SPI_USART_CLOCK, true);
}

/***************************************************************************//**
 * @brief
 *   Setup RTC channel 0 for toggling the EXTCOMIN pin and the display.
 ******************************************************************************/
static void setupRtc(void)
{
  // Initialize RTC
  RTC_Init_TypeDef rtcInit = RTC_INIT_DEFAULT;
  rtcInit.comp0Top = true;
  rtcInit.debugRun = false;
  rtcInit.enable   = true;
  RTC_Init(&rtcInit);
  const uint32_t rtcFreq = CMU_ClockFreqGet(cmuClock_RTC);

  // Setup RTC channels for toggling the display and for polarity inversion
  RTC_CompareSet(0, rtcFreq / POLARITY_INVERSION_FREQ);

  // Enable RTC Interrupt for toggling the display
  RTC_IntEnable(RTC_IEN_COMP0);
  NVIC_EnableIRQ(RTC_IRQn);

  // Enable GPIO pin for EXTCOMIN
  GPIO_PinModeSet((GPIO_Port_TypeDef)LCD_PORT_EXTCOMIN, LCD_PIN_EXTCOMIN, gpioModePushPull, 1);
}

/***************************************************************************//**
 * @brief
 *   RTCC Interrupt handler, triggered on RTC channel 0 compare match. Toggles
 *   the EXTCOMIN pin 60 times per second, and transfers a frame 2 times per
 *   second.
 ******************************************************************************/
void RTC_IRQHandler(void)
{
  // Get and clear interrupt flags
  uint32_t pending = RTC_IntGet() & RTC_IF_COMP0;
  RTC_IntClear(pending);

  if (pending)
  {
    // Toggle the EXTCOMIN pin
    GPIO_PinOutToggle((GPIO_Port_TypeDef)LCD_PORT_EXTCOMIN, LCD_PIN_EXTCOMIN);
    ++extcominToggles;

    // Transfer a frame every half second
    if (extcominToggles >= (POLARITY_INVERSION_FREQ/DISPLAY_TOGGLE_FREQ))
    {
      // Reset EXTCOMIN pin toggles
      extcominToggles = 0;

      // Determine which frame to transfer
      DMA_DESCRIPTOR_TypeDef *tasks;
      if (displayOn)
      {
        tasks = frameTransferBlock1;
      }
      else
      {
        tasks = frameTransferBlock0;
      }

      // Start transfer
      spiStartTransfer(tasks);
    }
  }
}

/***************************************************************************//**
 * @brief
 *   Initiate transfer of a frame buffer over DMA channel 0 with SPI.
 ******************************************************************************/
static void spiStartTransfer(DMA_DESCRIPTOR_TypeDef *tasks)
{
  // Set SCS to high to begin SPI transfer
  GPIO_PinOutSet((GPIO_Port_TypeDef)LCD_PORT_SCS, LCD_PIN_SCS);

  // Transfer data
  DMA_ActivateScatterGather(DMA_CHANNEL,
                            false,
                            tasks,
                            TRANSFERS_PER_FRAME);

  // Indicate that DMA transfer has started
  dmaRunning = true;
}

/***************************************************************************//**
 * @brief
 *   Call-back function triggered when transfer is complete. End the SPI
 *   transfer and toggle the internal state.
 ******************************************************************************/
static void spiEndTransfer(unsigned int channel, bool primary, void *user)
{
  // Set SCS to low to end SPI transfer
  GPIO_PinOutClear((GPIO_Port_TypeDef)LCD_PORT_SCS, LCD_PIN_SCS);

  // Indicate that the DMA transfer is complete and the display has been toggled
  dmaRunning = false;
  displayOn  = !displayOn;

  // Silence compiler warnings from unused parameters
  (void) channel;
  (void) primary;
  (void) user;
}

/***************************************************************************//**
 * @brief
 *   Setup USART for SPI communication.
 ******************************************************************************/
static void setupSpi(void)
{
  // Configure USART for synchronous mode
  USART_InitSync_TypeDef initSync = USART_INITSYNC_DEFAULT;
  initSync.baudrate = PAL_SPI_BAUDRATE;
  USART_InitSync(PAL_SPI_USART_UNIT, &initSync);

  // Route USART clock to display clock and USART TX to display SI
  PAL_SPI_USART_UNIT->ROUTE = PAL_SPI_USART_LOCATION
                              | USART_ROUTE_CLKPEN
                              | USART_ROUTE_TXPEN;

  // Set GPIO config to master for SPI communication
  GPIO_PinModeSet((GPIO_Port_TypeDef)LCD_PORT_DISP_SEL, LCD_PIN_DISP_SEL, gpioModePushPull, 1);
  GPIO_PinModeSet((GPIO_Port_TypeDef)LCD_PORT_SI,       LCD_PIN_SI,       gpioModePushPull, 0);
  GPIO_PinModeSet((GPIO_Port_TypeDef)LCD_PORT_SCLK,     LCD_PIN_SCLK,     gpioModePushPull, 0);
  GPIO_PinModeSet((GPIO_Port_TypeDef)LCD_PORT_SCS,      LCD_PIN_SCS,      gpioModePushPull, 0);

#ifdef LCD_PIN_DISP_PWR
    // On the Happy Gecko, enable EXTCOMIN to be toggled with GPIO
    GPIO_PinModeSet((GPIO_Port_TypeDef)LCD_PORT_DISP_PWR, LCD_PIN_DISP_PWR, gpioModePushPull, 1);
#endif

  // Send "CLEAR ALL" command to display over SPI
  GPIO_PinOutSet((GPIO_Port_TypeDef)LCD_PORT_SCS, LCD_PIN_SCS);
  USART_SpiTransfer(PAL_SPI_USART_UNIT, DISPLAY_COMMAND_CLEAR);
  GPIO_PinOutClear((GPIO_Port_TypeDef)LCD_PORT_SCS, LCD_PIN_SCS);
}

/***************************************************************************//**
 * @brief
 *   Setup DMA and descriptors.
 ******************************************************************************/
static void setupDma(void)
{
  // Initialize the DMA controller and enable interrupts
  DMA_Init_TypeDef init;
  init.hprot = 0;
  init.controlBlock = dmaControlBlock;
  DMA_Init(&init);
  NVIC_EnableIRQ(DMA_IRQn);

  // Setup callback to trigger when transfer is complete
  callback.cbFunc  = &spiEndTransfer;
  callback.userPtr = NULL;
  callback.primary = 0;

  // Setup DMA channel
  DMA_CfgChannel_TypeDef channelConfig;
  channelConfig.highPri   = false;
  channelConfig.enableInt = true;
  channelConfig.select    = PAL_SPI_USART_DMAREQ_TXBL;
  channelConfig.cb        = &callback;
  DMA_CfgChannel(DMA_CHANNEL, &channelConfig);

  // Setup alternate descriptor block for scatter-gather transfer
  setScatterGatherBlock();
}

/***************************************************************************//**
 * @brief
 *   Configure the alternate descriptor blocks.
 *
 * @detailed
 *   Configure the alternate descriptor blocks to facilitate a scatter-gather
 *   transfer of frame0 (the empty frame) and frame1 (the filled frame). The DMA
 *   can perform up to 1023 transfers when executing the task given by a
 *   descriptor. One frame buffer consists of 2306 bytes (see frames.c), so to
 *   transfer the whole frame buffer we set up a scatter-gather sequence with 3
 *   descriptors per frame.
 ******************************************************************************/
static void setScatterGatherBlock(void)
{
  static DMA_CfgDescrSGAlt_TypeDef descriptor;
  unsigned int frameSegment;

  for(frameSegment = 0; frameSegment < TRANSFERS_PER_FRAME; ++frameSegment)
  {
    // Set configuration common to all of the alternate descriptors
    descriptor.srcInc     = dmaDataInc1;
    descriptor.dst        = (void*)&PAL_SPI_USART_UNIT->TXDATA;
    descriptor.dstInc     = dmaDataIncNone;
    descriptor.arbRate    = dmaArbitrate1;
    descriptor.size       = dmaDataSize1;
    descriptor.peripheral = true;
    descriptor.hprot      = false;

    // Set correct number of DMA transfers
    if (frameSegment != TRANSFERS_PER_FRAME - 1)
    {
      // This is not the final chunk of a frame. Transfer maximum data.
      descriptor.nMinus1 = DMA_MAX_UNIT_TRANSFERS - 1;
    }
    else
    {
      // This is the final chunk of a frame. Transfer the rest of the frame.
      descriptor.nMinus1 = (DATASIZE % DMA_MAX_UNIT_TRANSFERS) - 1;
    }

    // Configure alternate descriptors for scatter-gather transfer of frame0
    descriptor.src = (void*)(frame0 + frameSegment*DMA_MAX_UNIT_TRANSFERS);
    DMA_CfgDescrScatterGather(frameTransferBlock0,
                              frameSegment,
                              &descriptor);

    // Configure alternate descriptors for scatter-gather transfer of frame1
    descriptor.src = (void*)(frame1 + frameSegment*DMA_MAX_UNIT_TRANSFERS);
    DMA_CfgDescrScatterGather(frameTransferBlock1,
                              frameSegment,
                              &descriptor);
  }
}

int main(void)
{
  // Chip errata
  CHIP_Init();

  // Setup
  setupClocks();
  setupSpi();
  setupRtc();
  setupDma();

  // If data is being transferred, wait in EM1 for LDMA "DONE" interrupt.
  // Otherwise wait in EM3 for an RTC interrupt.
  while (1)
  {
    if (dmaRunning)
    {
      EMU_EnterEM1();
    }
    else
    {
      EMU_EnterEM3(true);
    }
  }
}
