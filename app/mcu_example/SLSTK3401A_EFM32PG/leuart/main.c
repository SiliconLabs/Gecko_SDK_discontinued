/**************************************************************************//**
 * @file
 * @brief LEUART/LDMA in EM2 example for SLSTK3401A starter kit
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
#include "em_ldma.h"
#include "bspconfig.h"
#include "retargetserial.h"

/** LDMA Descriptor initialization */
static LDMA_Descriptor_t xfer =
  LDMA_DESCRIPTOR_LINKREL_P2M_BYTE(&LEUART0->RXDATA, /* Peripheral source address */
                                   &LEUART0->TXDATA, /* Peripheral destination address */
                                   1,                /* Number of bytes */
                                   0);               /* Link to same descriptor */

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
  GPIO_PinModeSet(RETARGET_TXPORT, RETARGET_TXPIN, gpioModePushPull, 1);
  GPIO_PinModeSet(RETARGET_RXPORT, RETARGET_RXPIN, gpioModeInput, 0);

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
  LEUART0->ROUTELOC0 = (LEUART0->ROUTELOC0 & ~(_LEUART_ROUTELOC0_TXLOC_MASK
                                               | _LEUART_ROUTELOC0_RXLOC_MASK))
                       | (RETARGET_TX_LOCATION << _LEUART_ROUTELOC0_TXLOC_SHIFT)
                       | (RETARGET_RX_LOCATION << _LEUART_ROUTELOC0_RXLOC_SHIFT);

  LEUART0->ROUTEPEN  = USART_ROUTEPEN_RXPEN | USART_ROUTEPEN_TXPEN;

  /* Set RXDMAWU to wake up the DMA controller in EM2 */
  LEUART_RxDmaInEM2Enable(LEUART0, true);

  /* Finally enable it */
  LEUART_Enable(LEUART0, leuartEnable);
}

/*******************************************************************************//**
 * @brief  Setting up LDMA
 *
 * @details
 *   This function configures LDMA transfer trigger to LEUART0 Rx.
 *   It also disables LDMA interrupt on transfer complete and makes the
 *   destination address increment to None. Since the we are using
 *   LDMA_DESCRIPTOR_LINKREL_P2M_BYTE as the channel descriptor and it enables
 *   LDMA interrupt and destination increment to a none-zero value
 *   ldmaCtrlDstIncOne. In the end this function initializes LDMA ch0 using
 *   channel descriptor(xfer) and the transfer trigger.
 *********************************************************************************/
void setupLdma(void)
{
  /* LDMA transfer configuration for LEUART */
  const LDMA_TransferCfg_t periTransferRx =
    LDMA_TRANSFER_CFG_PERIPHERAL(ldmaPeripheralSignal_LEUART0_RXDATAV);

  xfer.xfer.dstInc  = ldmaCtrlDstIncNone;
  xfer.xfer.doneIfs = 0;

  /* LDMA initialization mode definition */
  LDMA_Init_t init = LDMA_INIT_DEFAULT;

  /* LDMA initialization */
  LDMA_Init(&init);
  LDMA_StartTransfer(0, (LDMA_TransferCfg_t *)&periTransferRx, &xfer);
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  /* Use default settings for DCDC */
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_STK_DEFAULT;

  /* Chip errata */
  CHIP_Init();

  /* Init DCDC regulator with kit specific parameters */
  EMU_DCDCInit(&dcdcInit);

  /* Initialize LEUART */
  setupLeuart();

  /* Configure LDMA */
  setupLdma();

  while (1)
  {
    /* On every wakeup enter EM2 again */
    EMU_EnterEM2(true);
  }
}
