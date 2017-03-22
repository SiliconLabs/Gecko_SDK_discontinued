/**************************************************************************//**
 * @file main.c
 * @brief LEUART/DMA in EM2 example for EFM32LG_DK3650 development kit
 * @version 5.1.2
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
#include "em_emu.h"
#include "em_leuart.h"
#include "em_dma.h"
#include "dmactrl.h"
#include "bsp.h"
#include "bsp_trace.h"

/** LEUART Rx/Tx Port/Pin Location */
#define LEUART_LOCATION    0
#define LEUART_TXPORT      gpioPortC            /* LEUART transmission port */
#define LEUART_TXPIN       6                    /* LEUART transmission pin  */
#define LEUART_RXPORT      gpioPortC            /* LEUART reception port    */
#define LEUART_RXPIN       7                    /* LEUART reception pin     */

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

  /* Enable LEUART peripheral access on DK board */
  BSP_PeripheralAccess(BSP_RS232_LEUART, true);

  LEUART_Init_TypeDef init = LEUART_INIT_DEFAULT;
  
  /* Enable CORE LE clock in order to access LE modules */
  CMU_ClockEnable(cmuClock_CORELE, true);  

  /* Select LFXO for LEUARTs (and wait for it to stabilize) */
  CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFXO);
  CMU_ClockEnable(cmuClock_LEUART1, true);

  /* Do not prescale clock */
  CMU_ClockDivSet(cmuClock_LEUART1, cmuClkDiv_1);

  /* Configure LEUART */
  init.enable = leuartDisable;

  LEUART_Init(LEUART1, &init);
  
  /* Enable pins at default location */
  LEUART1->ROUTE = LEUART_ROUTE_RXPEN | LEUART_ROUTE_TXPEN | LEUART_LOCATION;
  
  /* Set RXDMAWU to wake up the DMA controller in EM2 */
  LEUART_RxDmaInEM2Enable(LEUART1, true);
  
  /* Finally enable it */
  LEUART_Enable(LEUART1, leuartEnable);
}

/**************************************************************************//**
 * @brief  Setup DMA
 * 
 * @details
 *   This function initializes DMA controller.
 *   It configures the DMA channel to be used for LEUART0 transmit 
 *   and receive. The primary descriptor for channel0 is configured for 
 *   a single data byte transfer. For continous data reception and transmission 
 *   using LEUART DMA loopmode is enabled for channel0. 
 *   In the end DMA transfer cycle is configured to basicMode where 
 *   the channel source address, destination address, and 
 *   the transfercount per dma cycle have been specified.
 *   
 *****************************************************************************/
 void setupDma(void)
{
  /* DMA configuration structs */
  DMA_Init_TypeDef       dmaInit;
  DMA_CfgChannel_TypeDef channelCfg;
  DMA_CfgDescr_TypeDef   descrCfg;
  DMA_CfgLoop_TypeDef    loopCfg;
  
  /* Initializing the DMA */
  dmaInit.hprot        = 0;
  dmaInit.controlBlock = dmaControlBlock;
  DMA_Init(&dmaInit);

  /* Setting up channel */
  channelCfg.highPri   = false; /* Can't use with peripherals */
  channelCfg.enableInt = false; /* Interrupt not needed in loop transfer mode */

  /* Configure channel 0 */
  /* Configure DMA transfer trigger to reception of LEUART Rx_DATA */
  channelCfg.select = DMAREQ_LEUART1_RXDATAV;
  channelCfg.cb     = NULL;
  DMA_CfgChannel(0, &channelCfg);
  
  /* Setting up channel descriptor */
  /* Destination is LEUART_Tx register and doesn't move */
  descrCfg.dstInc = dmaDataIncNone;

  /* Source is LEUART_RX register and transfers 8 bits each time */
  descrCfg.srcInc = dmaDataIncNone;
  descrCfg.size   = dmaDataSize1;

  /* We have time to arbitrate again for each sample */
  descrCfg.arbRate = dmaArbitrate1;
  descrCfg.hprot   = 0;

  /* Configure primary descriptor  */
  DMA_CfgDescr(0, true, &descrCfg);
  
  /* Configure loop transfer mode */
  loopCfg.enable = true;
  loopCfg.nMinus1 = 0;  /* Single transfer/DMAcycle*/
  DMA_CfgLoop(0, &loopCfg);
  
  /* Activate basic dma cycle using channel0 */
  DMA_ActivateBasic(0,
		    true,
		    false,
		    (void *)&LEUART1->TXDATA,
		    (void *)&LEUART1->RXDATA,
		    0);
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  /* Chip errata */
  CHIP_Init();
  
  /* Initialize DK board register access */
  BSP_Init(BSP_INIT_DEFAULT);

  /* If first word of user data page is non-zero, enable eA Profiler trace */
  BSP_TraceProfilerSetup();

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
