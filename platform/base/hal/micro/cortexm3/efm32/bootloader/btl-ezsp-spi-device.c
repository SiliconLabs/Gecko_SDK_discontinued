/***************************************************************************//**
 * @file hal/micro/cortexm3/efm32/bootloader/btl-ezsp-spi-protocol.c
 * @brief EFM32 internal SPI protocol implementation for use with the 
 *   standalone ezsp spi bootloader.
 * @version
 *******************************************************************************
 * @section License
 * <b>(C) Copyright 2015 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Silicon Labs has no
 * obligation to support this Software. Silicon Labs is providing the
 * Software "AS IS", with no express or implied warranties of any kind,
 * including, but not limited to, any implied warranties of merchantability
 * or fitness for any particular purpose or warranties against infringement
 * of any proprietary rights of a third party.
 *
 * Silicon Labs will not be liable for any consequential, incidental, or
 * special damages, or any other relief, or for any claim by any third party,
 * arising from your use of this Software.
 *
 ******************************************************************************/
#include PLATFORM_HEADER
#include "stack/include/ember.h"
#include "hal/hal.h"
#include "serial/serial.h"
#include "em_gpio.h"
#include "em_usart.h"
#include "em_ldma.h"
#include "em_cmu.h"
#include "em_bus.h"
#include "em_core.h"
#include "em_chip.h"
#include "em_emu.h"
#include "spidrv.h"
#include "gpiointerrupt.h"
#include "spi-protocol-device.h"
#include "btl-ezsp-spi-protocol.h"
#include "btl-ezsp-spi-device.h"

static const USART_InitSync_TypeDef initUsartData = SPI_BTL_USART_INIT;

SPIDRV_Init_t btlEzspSpi = SPI_NCP_USART_INIT;

// LDMA Assignments

// LDMA Rx and Tx Configurations
LDMA_TransferCfg_t periTransferRx = LDMA_TRANSFER_CFG_PERIPHERAL(ldmaPeripheralSignal_USART1_RXDATAV);
LDMA_TransferCfg_t periTransferTx = LDMA_TRANSFER_CFG_PERIPHERAL(ldmaPeripheralSignal_USART1_TXEMPTY);

// LDMA Rx and Tx Descriptors
LDMA_Descriptor_t rxDescriptor = LDMA_DESCRIPTOR_SINGLE_P2M_BYTE(&USART1->RXDATA,
                                                               (uint32_t)halHostCommandBuffer,
                                                               SPIP_BUFFER_SIZE);

LDMA_Descriptor_t txDescriptor = LDMA_DESCRIPTOR_SINGLE_M2P_BYTE((uint32_t)halHostResponseBuffer,
                                                               &USART1->TXDATA,
                                                               SPIP_BUFFER_SIZE);

const uint8_t rxChannel = 0;                // LDMA channel 0 for rx
const uint8_t txChannel = 1;                // LDMA channel 1 for tx

LDMA_Init_t initLdmaData = LDMA_INIT_DEFAULT;   // use default initialization

void ezspSetupSpiAndDma(void){
  uint32_t resetRouteloc = 0x00000000;
  // USART SPI initialization
  USART_InitSync(SPI_NCP_USART, &initUsartData);   // initialize the uart in spi mode
  
  SPI_NCP_USART->ROUTELOC0 = resetRouteloc;
  SPI_NCP_USART->ROUTELOC0 |= (btlEzspSpi.portLocationRx << _USART_ROUTELOC0_RXLOC_SHIFT);
  SPI_NCP_USART->ROUTELOC0 |= (btlEzspSpi.portLocationTx << _USART_ROUTELOC0_TXLOC_SHIFT);
  SPI_NCP_USART->ROUTELOC0 |= (btlEzspSpi.portLocationClk << _USART_ROUTELOC0_CLKLOC_SHIFT);
  SPI_NCP_USART->ROUTELOC0 |= (btlEzspSpi.portLocationCs << _USART_ROUTELOC0_CSLOC_SHIFT);

  SPI_NCP_USART->ROUTEPEN |= USART_ROUTEPEN_RXPEN;
  SPI_NCP_USART->ROUTEPEN |= USART_ROUTEPEN_TXPEN;
  SPI_NCP_USART->ROUTEPEN |= USART_ROUTEPEN_CSPEN;
  SPI_NCP_USART->ROUTEPEN |= USART_ROUTEPEN_CLKPEN;

  LDMA->CH[0].DST = (uint32_t)halHostCommandBuffer;
  LDMA->CH[1].DST = (uint32_t)halHostResponseBuffer;
 
  SPI_NCP_USART->IEN |= USART_IEN_RXDATAV;    // enable rxdatav interrupt

  LDMA_IntClear(0xFFFFFFFF);           // clear channel done interrupts

  LDMA->CHEN    = 0;    // disable all channels
  LDMA->REQDIS  = 0;    // enable all peripheral requests

  NVIC_ClearPendingIRQ( LDMA_IRQn );

  // disable rx and tx in usart just in case so LDMA transfer won't start prematurely
  USART_Enable(SPI_NCP_USART, usartDisable);

  LDMA_StartTransfer(rxChannel,  &periTransferRx, &rxDescriptor);

  BUS_RegMaskedClear(&LDMA->CHEN, 1 << rxChannel);

  USART_Enable(SPI_NCP_USART, usartEnable); // enable the rx and tx in uart

  if(nSSEL_IS_NEGATED()){
    LDMA_IntEnable((uint32_t)1 << rxChannel);    // enable transfer complete interrupt for rxChannel
    BUS_RegMaskedSet(&LDMA->CHEN, 1 << rxChannel); //Enable receive channel.
  }
}

void ezspSpiInitSpiAndDma(void){
  CMU_HFXOInit_TypeDef hfxoInit = CMU_HFXOINIT_WSTK_DEFAULT;
  CMU_HFXOInit(&hfxoInit);
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);
  CMU_ClockEnable(cmuClock_HFPER, true);
  CMU_ClockEnable(cmuClock_GPIO, true);
  CMU_ClockEnable(SPI_NCP_USART_CLOCK, true);

  LDMA_Init(&initLdmaData);

  // LDMA_Init always enables the IRQ, explicitly disable as bootloaders don't
  // handle interrupts
  NVIC_DisableIRQ( LDMA_IRQn );
}

void configureSpiGpio(void)
{
  GPIO_Port_TypeDef mosiPort, misoPort, clkPort;
  uint8_t mosiPin, misoPin, clkPin;

  if(SPI_NCP_USART == USART1){
    mosiPort = (GPIO_Port_TypeDef)AF_USART1_TX_PORT(btlEzspSpi.portLocationTx);
    misoPort = (GPIO_Port_TypeDef)AF_USART1_RX_PORT(btlEzspSpi.portLocationRx);
    clkPort  = (GPIO_Port_TypeDef)AF_USART1_CLK_PORT(btlEzspSpi.portLocationClk);
    mosiPin  = AF_USART1_TX_PIN(btlEzspSpi.portLocationTx);
    misoPin  = AF_USART1_RX_PIN(btlEzspSpi.portLocationRx);
    clkPin   = AF_USART1_CLK_PIN(btlEzspSpi.portLocationClk);
  }
  else if(SPI_NCP_USART == USART0){
    mosiPort = (GPIO_Port_TypeDef)AF_USART0_TX_PORT(btlEzspSpi.portLocationTx);
    misoPort = (GPIO_Port_TypeDef)AF_USART0_RX_PORT(btlEzspSpi.portLocationRx);
    clkPort  = (GPIO_Port_TypeDef)AF_USART0_CLK_PORT(btlEzspSpi.portLocationClk);
    mosiPin  = AF_USART0_TX_PIN(btlEzspSpi.portLocationTx);
    misoPin  = AF_USART0_RX_PIN(btlEzspSpi.portLocationRx);
    clkPin   = AF_USART0_CLK_PIN(btlEzspSpi.portLocationClk);
  }

  GPIO_PinModeSet(mosiPort, mosiPin, gpioModeInputPull, 1); // Configure MOSI
  GPIO_PinModeSet(misoPort, misoPin, gpioModePushPull, 0);  // Configure MISO
  GPIO_PinModeSet(clkPort, clkPin, gpioModeInputPull, 1);   // Configure SCK
  GPIO_PinModeSet(NSSEL_PORT, NSSEL_PIN, gpioModeInputPullFilter, 1); // configure SSEL
}

void ezspSpiConfigureInterrupts(void){
  ////---- Configure nWAKE interrupt ----////
  //start from a fresh state just in case
#ifndef DISABLE_NWAKE
  GPIO->EXTIFALL = 0 << NWAKE_PIN; // disable triggering
  // configure nwake pin
  GPIO_IntConfig(NWAKE_PORT, NWAKE_PIN, false, true, false); // falling edge
  GPIO_PinModeSet(NWAKE_PORT, NWAKE_PIN, gpioModeInputPullFilter, 1); // input with pullup
#endif

  ////---- Configure nSSEL interrupt ----////
  GPIO->EXTIRISE = 0 << NSSEL_PIN; // disable triggering
  GPIO_IntConfig(NSSEL_PORT, NSSEL_PIN, true, false, false);

  ////---- Configure nHOST_INT output ----////
  GPIO_PinModeSet(NHOST_INT_PORT, NHOST_INT_PIN, gpioModePushPull, 1);

  // ----- Account for Noise and Crosstalk ------ //
  // on some hardware configurations there is a lot of noise and bootloading can fail
  // due to crosstalk. to avoid this, the slewrate is lowered here from 5 to 4, and the
  // drivestrength is lowered from 10mA to 1mA. if noise related errors still occur, 
  // the slewrate can be lowered further
  GPIO->P[NHOST_INT_PORT].CTRL = 0x00000000;
  GPIO->P[NHOST_INT_PORT].CTRL = (0x4 << _GPIO_P_CTRL_SLEWRATE_SHIFT);
  GPIO->P[NHOST_INT_PORT].CTRL |= (0x4 << _GPIO_P_CTRL_SLEWRATEALT_SHIFT);

  // the drivestrength is lowered from 10mA to 1mA by setting DRIVESTRENGTH to 1
  GPIO->P[NHOST_INT_PORT].CTRL |= (1 << _GPIO_P_CTRL_DRIVESTRENGTH_SHIFT);  
}

void ezspSpiDisableReceptionInterrupts(void){
  // do nothing, not needed on efr32
}

void ezspSpiDisableReception(void){
  BUS_RegMaskedClear(&LDMA->CHEN, 1 << rxChannel);
}

void ezspSpiAckUnload(void){
  // do nothing, not needed on efr32
}

void ezspSpiEnableReception(void){
  LDMA_IntEnable(1 << rxChannel);    // enable transfer complete int for rxChannel
  BUS_RegMaskedSet(&LDMA->CHEN, 1 << rxChannel);  //Enable receive channel.
}

void ezspSpiStartTxTransfer(uint8_t responseLength){
  LDMA_StartTransfer(txChannel,  &periTransferTx, &txDescriptor);
}

bool ezspSpiRxActive(void){
  if(BUS_RegMaskedRead(&LDMA->CHEN, 1 << rxChannel) == 0) {
    return false;
  }
  else{
    return true;
  }
}

void ezspSpiFlushRxFifo(void){
  volatile uint8_t dummy;
  // efr32 buffer is 3 so read 3 dummy bytes
  dummy = (uint8_t)SPI_NCP_USART->RXDATA;
  dummy = (uint8_t)SPI_NCP_USART->RXDATA;
  dummy = (uint8_t)SPI_NCP_USART->RXDATA;
}

bool ezspSpiValidStartOfData(void){
  // check for valid start of data (CH[0].DST has moved from start)
  // check for unloaded buffer
  if (BUS_RegMaskedRead(&LDMA->CH[0].DST, 0xFFFFFFFF) > (uint32_t)&halHostCommandBuffer 
        || BUS_RegMaskedRead(&LDMA->CHDONE, 1 << rxChannel)){
    return true;
  }
  else{
    return false;
  }
}

bool ezspSpiCheckIfUnloaded(void){
  // efr32 doesn't need to do this
  return false;
}


bool ezspSpiHaveTwoBytes(void){
  // check that DST has moved at least 2 places
  if(BUS_RegMaskedRead(&LDMA->CH[0].DST, 0xFFFFFFFF) >= 
    ((uint32_t)&halHostCommandBuffer + 0x2)){ 
    return true;
  }
  else{
    return false;
  }
}

bool ezspSpiHaveAllData(void){
  if(BUS_RegMaskedRead(&LDMA->CH[0].DST, 0xFFFFFFFF) >= 
    ((uint32_t)&halHostCommandBuffer + halHostCommandBuffer[1] + 0x3)){
    return true;
  }
  else{
    return false;
  }
}

bool ezspSpiPollForMosi(uint8_t responseLength){
  if((BUS_RegMaskedRead(&LDMA->CH[0].DST, 0xFFFFFFFF) > (uint32_t)&halHostCommandBuffer) && 
    (responseLength <= 0)){
    return true;
  }
  else{
    return false;
  }
}

bool ezspSpiPollForNWAKE(void){
  #ifndef DISABLE_NWAKE
  if((GPIO_IntGet()&((uint32_t)1<<NWAKE_PIN))==((uint32_t)1<<NWAKE_PIN)) {
    // ack int before read to avoid potential of missing interrupt
    GPIO_IntClear((uint32_t)1 << NWAKE_PIN);  // clears the nWAKE interrupt flag
    return true;
  }
  else{
    return false;
  }
  #else
  return false;
  #endif
}

bool ezspSpiPollForNSSEL(void){
  if((GPIO_IntGet()&((uint32_t)1<<NSSEL_PIN))==((uint32_t)1<<NSSEL_PIN)) {
    // ack int before read to avoid potential of missing interrupt
    GPIO_IntClear((uint32_t)1 << NSSEL_PIN);  // clears the nSSEL interrupt flag
    return true;
  }
  else{
    return false;
  }
}

bool ezspSpiTransactionValid(uint8_t responseLength){
  // not implemented
  return true;
}
