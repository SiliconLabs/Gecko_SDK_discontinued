/**************************************************************************//**
 * @file main.c
 * @brief LEUART/DMA in EM2 example for EFM32TG_STK3300 starter kit
 * @version 5.1.2
 ******************************************************************************
 * @section License
 * <b>Copyright 2016 Silicon Labs, Inc. http://www.silabs.com</b>
 ******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 *****************************************************************************/
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_leuart.h"
#include "em_dma.h"
#include "dmactrl.h"

/** LEUART Rx/Tx Port/Pin Location */
#define LEUART_LOCATION    0
#define LEUART_TXPORT      gpioPortD        /* LEUART transmission port */
#define LEUART_TXPIN       4                /* LEUART transmission pin  */
#define LEUART_RXPORT      gpioPortD        /* LEUART reception port    */
#define LEUART_RXPIN       5                /* LEUART reception pin     */

/** DMA Configurations            */
#define DMA_CHANNEL       0          /* DMA channel is 0 */
#define NUM_TRANSFER      1024       /* Number of transfers per DMA cycle */

/** DMA callback structure */
static DMA_CB_TypeDef dmaCallBack;

/**************************************************************************//**
 * @brief  DMA callback function
 * 
 * @details This function is invoked once a DMA transfer cycle is completed.
 *          It then refreshes the completed DMA descriptor.
 *****************************************************************************/
static void basicTransferComplete(unsigned int channel, bool primary, void *user)
{
  (void) user; /* Unused parameter */
  /* Refresh DMA basic transaction cycle */
  DMA_ActivateBasic(channel,
		    primary,
		    false,
		    (void *)&LEUART0->TXDATA,
		    (void *)&LEUART0->RXDATA,
		    (NUM_TRANSFER-1));
}

/**************************************************************************//**
 * @brief  Setup DMA
 * 
 * @details
 *   This function initializes DMA controller.
 *   It configures the DMA channel to be used for LEUART0 transmit and receive.
 *   The primary descriptor for channel0 is configured for N_MINUS_1 byte 
 *   transfer. For continous data reception and transmission using LEUART in
 *   basic mode DMA Callback function is configured to reactivate
 *   basic transfer cycle once it is completed. 
 *   
 *****************************************************************************/
void setupDma(void)
{
  /* DMA configuration structs */
  DMA_Init_TypeDef       dmaInit;
  DMA_CfgChannel_TypeDef channelCfg;
  DMA_CfgDescr_TypeDef   descrCfg;
  
  /* Initializing the DMA */
  dmaInit.hprot        = 0;
  dmaInit.controlBlock = dmaControlBlock;
  DMA_Init(&dmaInit);

  /* Set the interrupt callback routine */
  dmaCallBack.cbFunc = basicTransferComplete;
  /* Callback doesn't need userpointer */
  dmaCallBack.userPtr = NULL;
  
  /* Setting up channel */
  channelCfg.highPri   = false; /* Can't use with peripherals */
  channelCfg.enableInt = true;  /* Enabling interrupt to refresh DMA cycle*/

  /* Configure channel 0 */
  /*Setting up DMA transfer trigger request*/
  channelCfg.select = DMAREQ_LEUART0_RXDATAV; 
  /* Setting up callback function to refresh descriptors*/
  channelCfg.cb     = &dmaCallBack; 
  DMA_CfgChannel(0, &channelCfg);
  
  /* Setting up channel descriptor */
  /* Destination is LEUART_Tx register and doesn't move */
  descrCfg.dstInc = dmaDataIncNone;

  /* Source is LEUART_RX register and transfers 8 bits each time */
  descrCfg.srcInc = dmaDataIncNone;
  descrCfg.size   = dmaDataSize1;

  /* Default setting of DMA arbitration*/
  descrCfg.arbRate = dmaArbitrate1;
  descrCfg.hprot   = 0;

  /* Configure primary descriptor */
  DMA_CfgDescr(DMA_CHANNEL, true, &descrCfg);
  
  /* Enable Basic Transfer cycle */
  DMA_ActivateBasic(DMA_CHANNEL,
		    true,
		    false,
		    (void *)&LEUART0->TXDATA,
		    (void *)&LEUART0->RXDATA,
		    (NUM_TRANSFER-1));
}

/**************************************************************************//**
 * @brief  Setting up LEUART
 *****************************************************************************/
void setupLeuart(void)
{
  /* Enable peripheral clocks */
  CMU_ClockEnable(cmuClock_HFPER, true);
  /* Configure GPIO pins */
  CMU_ClockEnable(cmuClock_GPIO, true);
  /* To avoid false start, configure output as high */
  GPIO_PinModeSet(LEUART_TXPORT, LEUART_TXPIN, gpioModePushPull, 1);
  GPIO_PinModeSet(LEUART_RXPORT, LEUART_RXPIN, gpioModeInput, 0);

  LEUART_Init_TypeDef init = LEUART_INIT_DEFAULT;
  
  /* Enable CORE LE clock in order to access LE modules */
  CMU_ClockEnable(cmuClock_CORELE, true);  

  /* Select LFXO for LEUARTs (and wait for it to stabilize) */
  CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFXO);
  CMU_ClockEnable(cmuClock_LEUART0, true);

  /* Do not prescale clock */
  CMU_ClockDivSet(cmuClock_LEUART0, cmuClkDiv_1);

  /* Configure LEUART */
  init.enable = leuartDisable;

  LEUART_Init(LEUART0, &init);
  
  /* Enable pins at default location */
  LEUART0->ROUTE = LEUART_ROUTE_RXPEN | LEUART_ROUTE_TXPEN | LEUART_LOCATION;
  
  /* Set RXDMAWU to wake up the DMA controller in EM2 */
  LEUART_RxDmaInEM2Enable(LEUART0, true);
  
  /* Clear previous RX interrupts */
  LEUART_IntClear(LEUART0, LEUART_IF_RXDATAV);
  NVIC_ClearPendingIRQ(LEUART0_IRQn);

  /* Finally enable it */
  LEUART_Enable(LEUART0, leuartEnable);
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  /* Chip errata */
  CHIP_Init();

  /* Initialize LEUART */
  setupLeuart();
  
  /* Setup DMA */
  setupDma();
  
  while (1)
  {
    /* On every wakeup enter EM2 again */
    EMU_EnterEM2(true);
  }
}
