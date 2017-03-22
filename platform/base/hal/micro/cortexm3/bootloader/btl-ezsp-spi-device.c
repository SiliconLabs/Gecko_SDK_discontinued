/***************************************************************************//**
 * @file hal/micro/cortexm3/efm32/bootloader/btl-ezsp-spi-protocol.c
 * @brief EFM32 internal SPI protocol implementation for use with the 
 *   standalone ezsp spi bootloader.
 * @version
 *******************************************************************************
 * @section License
 * <b>(C) Copyright 2016 Silicon Labs, http://www.silabs.com</b>
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
#include "btl-ezsp-spi-protocol.h"
#include "hal/micro/cortexm3/ezsp-spi-serial-controller-autoselect.h"
#include "btl-ezsp-spi-device.h"


void ezspSetupSpiAndDma(void){
  // Make SPI peripheral clean and start a-new
  INT_SCxCFG &= ~INT_SCRXVAL;   //disable byte received interrupt
  INT_SCxCFG &= ~INT_SCRXULDA;  //disable buffer A unload interrupt
  SCx_REG(DMACTRL) = SC_RXDMARST;    //reset DMA just in case
  SCx_REG(DMACTRL) = SC_TXDMARST;    //reset DMA just in case
  SCx_REG(MODE) = SCx_MODE(DISABLED); //be safe, make sure we start from disabled
  SCx_REG(RATELIN) =  0; //no effect in slave mode
  SCx_REG(RATEEXP) =  0; //no effect in slave mode
  SCx_REG(SPICFG)  =  (0 << SC_SPIMST_BIT)  | //slave mode
                      (0 << SC_SPIPHA_BIT)  | //SPI Mode 0 - sample leading edge
                      (0 << SC_SPIPOL_BIT)  | //SPI Mode 0 - rising leading edge
                      (0 << SC_SPIORD_BIT)  | //MSB first
                      (0 << SC_SPIRXDRV_BIT)| //no effect in slave mode
                      (0 << SC_SPIRPT_BIT)  ; //transmit 0xFF when no data to send
  SCx_REG(MODE)   =  SCx_MODE(SPI); //activate SPI mode
  //Configure DMA RX channel to point to the command buffer
  SCx_REG(RXBEGA) = (uint32_t) halHostCommandBuffer;
  SCx_REG(RXENDA) = (uint32_t) halHostCommandBuffer + SPIP_BUFFER_SIZE -1;
  //Configure DMA TX channel to point to the response buffer
  SCx_REG(TXBEGA) = (uint32_t) halHostResponseBuffer;
  SCx_REG(TXENDA) = (uint32_t) halHostResponseBuffer + SPIP_BUFFER_SIZE -1;
  if(nSSEL_IS_NEGATED()) { //only activate DMA if nSSEL is idle
    //since bootloader is polling driven, do not enable ISRs!
    INT_SCxCFG |= INT_SCRXVAL; //enable byte received interrupt
    INT_SCxCFG |= INT_SCRXULDA;//enable RX buffer A unload interrupt
    SCx_REG(DMACTRL) = SC_RXLODA;  //activate RX DMA for first command
  }
  INT_SCxFLAG = 0xFFFF;     //clear any stale interrupts
  //since bootloader is polling driven, do not enable top-level SCx interrupts!
  //  INT_CFGSET = INT_SCx;     // no interrupts in bootloader!
}

void ezspSpiInitSpiAndDma(void){
  // do nothing, no additional code needed for em3xx
}

void configureSpiGpio(void){
  ////---- Configure basic SPI Pins: MOSI, MISO, SCLK and nSSEL ----////
  CFG_SPI_GPIO();
  PULLUP_nSSEL();
}

void ezspSpiConfigureInterrupts(void){
  ////---- Configure nWAKE interrupt ----////
  //start from a fresh state just in case
  INT_CFGCLR = nWAKE_INT;               //disable triggering
  nWAKE_INTCFG = (GPIOINTMOD_DISABLED << GPIO_INTMOD_BIT);
  //Configure nWAKE pin
  CFG_nWAKE(GPIOCFG_IN_PUD);            //input with pullup
  PULLUP_nWAKE();
  //Enable Interrupts
  INT_GPIOFLAG = nWAKE_GPIOFLAG;        //clear stale interrupts
  INT_PENDCLR = nWAKE_INT;
  //    INT_CFGSET = nWAKE_INT;             // no interrupts in bootloader!
  nWAKE_INTCFG =  (0 << GPIO_INTFILT_BIT) |   //no filter
                    (GPIOINTMOD_FALLING_EDGE << GPIO_INTMOD_BIT);
  
  ////---- Configure nSSEL_INT for compatibility with EM260 ----////
  CFG_nSSEL_INT(GPIOCFG_IN);            //input floating - not used

  ////---- Configure nSSEL interrupt (IRQC) ----////
  INT_CFGCLR = nSSEL_INT;               //disable triggering
  nSSEL_INTCFG = (GPIOINTMOD_DISABLED << GPIO_INTMOD_BIT);
  nSSEL_IRQSEL = nSSEL_IRQSEL_MASK;     //assign nSSEL pin to IRQC
  //Enable Interrupts
  INT_GPIOFLAG = nSSEL_GPIOFLAG;        //clear stale interrupts
  INT_PENDCLR = nSSEL_INT;
  //    INT_CFGSET = nSSEL_INT;             // no interrupts in bootloader!
  nSSEL_INTCFG = (0 << GPIO_INTFILT_BIT) |  //no filter
                   (GPIOINTMOD_RISING_EDGE << GPIO_INTMOD_BIT);

  ////---- Configure nHOST_INT output ----////
  SET_nHOST_INT();
  CFG_nHOST_INT(GPIOCFG_OUT);
}

void ezspSpiDisableReceptionInterrupts(void){
  INT_SCxCFG &= ~INT_SCRXVAL;//disable byte received interrupt
  INT_SCxCFG &= ~INT_SCRXULDA;
}

void ezspSpiDisableReception(void){
  SCx_REG(DMACTRL) = SC_RXDMARST; //disable reception while processing
}

void ezspSpiAckUnload(void){
  //ack command unload --BugzId:14622
  INT_SCxFLAG = INT_SCRXULDA;
}

void ezspSpiEnableReception(void){
  INT_SCxFLAG = INT_SCRXVAL; //clear byte received interrupt
  INT_SCxFLAG = INT_SCRXULDA;//clear buffer A unload interrupt
  INT_SCxCFG |= INT_SCRXVAL; //enable byte received interrupt
  INT_SCxCFG |= INT_SCRXULDA;//enable buffer A unload interrupt
  SCx_REG(DMACTRL) = SC_RXLODA; //we are inter-command, activate RX DMA for next
}

void ezspSpiStartTxTransfer(uint8_t responseLength){
  SCx_REG(TXENDA) = (uint32_t)halHostResponseBuffer + responseLength - 1;
  SCx_REG(DATA) = 0xFF; // emlipari-183: Prepend sacrificial Tx pad byte
  INT_SCxFLAG = INT_SCRXVAL; //clear byte received interrupt
  SCx_REG(DMACTRL) = SC_TXLODA;   //enable response for TX
  INT_SCxCFG |= INT_SCRXVAL; //enable byte received interrupt
}

bool ezspSpiRxActive(void){
  if(((SCx_REG(DMASTAT)&SC_RXACTA)!=SC_RXACTA)) {
    return false;
  }
  else{
    return true;
  }
}

void ezspSpiFlushRxFifo(void){
  volatile uint8_t dummy;
  // em3xx buffer is 4 so read 4 dummy bytes
  dummy = SCx_REG(DATA);
  dummy = SCx_REG(DATA);
  dummy = SCx_REG(DATA);
  dummy = SCx_REG(DATA);
}

bool ezspSpiValidStartOfData(void){
  //check for valid start of data (counter!=0)
  //check for unloaded buffer
  if( (SCx_REG(RXCNTA)!=0) || (INT_SCxFLAG & INT_SCRXULDA) ) {
    return true;
  }
  else{
    return false;
  }
}

bool ezspSpiCheckIfUnloaded(void){
  //if we have unloaded, know command arrived so jump directly there
  //bypassing RXCNT checks.  On em2xx this is needed because unload
  //clears RXCNT; on em3xx it is simply a convenience.
  if(INT_SCxFLAG & INT_SCRXULDA){
    //While em2xx could get away with ACKing unload interrupt here,
    //because unload clears RXCNT, em3xx *must* do it below otherwise
    //a just-missed unload leaving RXCNT intact could mistakenly come
    //back to haunt us as a ghost command. -- BugzId:14622.
    return true;
  }
  else{
    return false;
  }
}


bool ezspSpiHaveTwoBytes(void){
  if(SCx_REG(RXCNTA)>1){
    return true;
  }
  else{
    return false;
  }
}

bool ezspSpiHaveAllData(void){
  if(SCx_REG(RXCNTA) >= halHostCommandBuffer[1]+3) {
    return true;
  }
  else{
    return false;
  }
}

bool ezspSpiPollForMosi(uint8_t responseLength){
  //this fakes out SCx_ISR() in the normal SPI Protocol
  if(INT_SCxFLAG & INT_SCRXVAL) {
    return true;
  }
  else{
    return false;
  }
}

bool ezspSpiPollForNWAKE(void){
  if(INT_GPIOFLAG & nWAKE_GPIOFLAG) {
    // ack int before read to avoid potential of missing interrupt
    INT_GPIOFLAG = nWAKE_GPIOFLAG;
    return true;
  }
  else{
    // falling edge not detected
    return false;
  }
}

bool ezspSpiPollForNSSEL(void){
  //nSSEL signal comes in on IRQC
  if(INT_GPIOFLAG & nSSEL_GPIOFLAG) {
    // ack int before read to avoid potential of missing interrupt
    INT_GPIOFLAG = nSSEL_GPIOFLAG;
    return true;
  }
  else{
    // rising edge not detected
    return false;
  }
}

bool ezspSpiTransactionValid(uint8_t responseLength){
  if((SCx_REG(TXCNT) != responseLength)){
    return false;
  }
  else{
    return true;
  }
}
