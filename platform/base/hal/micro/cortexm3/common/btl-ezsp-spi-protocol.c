/***************************************************************************//**
 * @file hal/micro/cortexm3/common/btl-ezsp-spi-protocol.c
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
#include "btl-ezsp-spi-protocol.h"
#include "btl-ezsp-spi-device.h"

#if defined(CORTEXM3_EFM32_MICRO)
  #include "spi-protocol-device.h"
#else
  #include "hal/micro/cortexm3/ezsp-spi-serial-controller-autoselect.h"
#endif

#if 0
  #define DEBUG_SET_LED()   halSetLed(BOARD_ACTIVITY_LED)
  #define DEBUG_CLEAR_LED() halClearLed(BOARD_ACTIVITY_LED)
#else
  #define DEBUG_SET_LED()
  #define DEBUG_CLEAR_LED()
#endif

uint8_t halHostCommandBuffer[SPIP_BUFFER_SIZE];
uint8_t halHostResponseBuffer[SPIP_BUFFER_SIZE];
uint8_t spipErrorResponseBuffer[SPIP_ERROR_RESPONSE_SIZE];
//Provide easy references the buffers for EZSP
uint8_t *halHostCommandFrame = halHostCommandBuffer + EZSP_LENGTH_INDEX;
uint8_t *halHostResponseFrame = halHostResponseBuffer + EZSP_LENGTH_INDEX;

bool spipFlagWakeFallingEdge;    // detected a falling edge on nWAKE
bool spipFlagIdleHostInt;        // idle nHOST_INT at proper time
bool spipFlagOverrideResponse;   // just booted, or have an error to report
bool spipFlagTransactionActive;  // transaction is active
uint8_t spipResponseLength;      // true length of the Response Section

static void wipeAndRestartSpi(void);
static void halHostSerialPowerup(void);
static void setSpipErrorBuffer(uint8_t spiByte);
static void processSpipCommandAndRespond(uint8_t spipResponse);
static bool halInternalHostSerialTick(bool responseReady);
static void halGpioPolling(void);

static void wipeAndRestartSpi(void)
{ 
  spipFlagTransactionActive = false;
  spipResponseLength = 0;  

  ezspSetupSpiAndDma();
}

static void halHostSerialPowerup(void)
{
  ezspSpiInitSpiAndDma();

  ////---- Configure SPI ----////
  wipeAndRestartSpi();

  ////---- Configure basic SPI Pins: MOSI, MISO, SCLK and nSSEL ----////
  configureSpiGpio();

  ezspSpiConfigureInterrupts();
}

void halHostSerialInit(void)
{
  ////---- Initialize Flags ----////
  spipFlagWakeFallingEdge = false; //start with no edge on nWAKE
  spipFlagIdleHostInt = true;      //idle nHOST_INT after telling host we booted
  //load error response buffer with the "EM260 Reset" message + reset cause
  //we do not use the setSpipErrorBuffer() function here since that function
  //assumes the second byte in the buffer is reserved (0)
  spipFlagOverrideResponse = true; //set a flag indicating we just booted
  spipErrorResponseBuffer[0] = SPIP_RESET;
  spipErrorResponseBuffer[1] = EM2XX_RESET_BOOTLOADER; //reset is always bootloader

  halHostSerialPowerup();
}

void halHostCallback(bool haveData)
{
  if(haveData) {
    //only assert nHOST_INT if we are outside a wake handshake (wake==1)
    //and outside of a current transaction (nSSEL=1)
    //if inside a wake handshake or transaction, delay asserting nHOST_INT
    //until the SerialTick
    if( nWAKE_IS_NEGATED() && nSSEL_IS_NEGATED() ) {
      CLR_nHOST_INT();
    }
    spipFlagIdleHostInt = false;
  } else {
    spipFlagIdleHostInt = true;
  }
}

static void setSpipErrorBuffer(uint8_t spiByte)
{
  if(!spipFlagOverrideResponse) {
    //load error response buffer with the error supplied in spiByte
    spipFlagOverrideResponse = true;      //set a flag indicating override
    spipErrorResponseBuffer[0] = spiByte; //set the SPI Byte with the error
    spipErrorResponseBuffer[1] = 0;       //this byte is currently reserved
  }
}

static void processSpipCommandAndRespond(uint8_t spipResponse)
{
  DEBUG_SET_LED();
  ezspSpiDisableReception(); // Disable reception while processing
  DEBUG_CLEAR_LED();
  //check for Frame Terminator, it must be there!
  if(halHostCommandBuffer[1]==FRAME_TERMINATOR) {
    //override with the supplied spipResponse
    halHostResponseBuffer[0] = spipResponse;
  } else {
    //no frame terminator found!  report missing F.T.
    setSpipErrorBuffer(SPIP_MISSING_FT);
  }
  halHostSerialTick(true); //respond immediately!
}

uint8_t getSpipResponseLength(void){
  return spipResponseLength;
}

// One layer of indirection is used so calling the public function will actually
// result in the real Tick function (this internal one) being wrapped in an
// ATOMIC() block to prevent potential corruption from the nSSEL interrupt.
static bool halInternalHostSerialTick(bool responseReady)
{
  //assert nHOST_INT if need to tell host something immediately and nSSEL=idle
  if(spipFlagOverrideResponse && nSSEL_IS_NEGATED()) {
    CLR_nHOST_INT();  //tell the host we just got into bootloader
  }

  if(spipFlagWakeFallingEdge){ // detected falling edge on nWAKE, handshake
    CLR_nHOST_INT();
    while(nWAKE_IS_ASSERTED()) {halResetWatchdog(); /*EMHAL-1074*/}
    SET_nHOST_INT();
    spipFlagWakeFallingEdge = false;
    // The wake handshake is complete, but spipFlagIdleHostInt is saying
    // that there is a callback pending.
    if(!spipFlagIdleHostInt){
      halCommonDelayMicroseconds(50); // delay 50us so host can get ready
      CLR_nHOST_INT(); // indicate the pending callback
    }
  } else if(responseReady && spipFlagTransactionActive) { // OK to transmit
    DEBUG_SET_LED();
    if(spipFlagOverrideResponse){
      spipFlagOverrideResponse = false; // we no longer need to override
      // override whatever was sent with the error response message
      MEMCOPY(halHostResponseBuffer,
              spipErrorResponseBuffer,
              SPIP_ERROR_RESPONSE_SIZE);
    }
    if(spipFlagIdleHostInt) {
      SET_nHOST_INT(); // the nHOST_INT signal can be asynchronously
    }
    // add Frame terminator and record true response length
    if(halHostResponseBuffer[0]<0x05){
      halHostResponseBuffer[1+1] = FRAME_TERMINATOR;
      spipResponseLength = 3; // true response length
    } else if((halHostResponseBuffer[0]==0xFE) || //EZSP payload
            (halHostResponseBuffer[0]==0xFD)) { // bootloader payload
      halHostResponseBuffer[halHostResponseBuffer[1] +1 +1] = FRAME_TERMINATOR;
      spipResponseLength = halHostResponseBuffer[1]+3; // true response length
    } else {
      halHostResponseBuffer[1] = FRAME_TERMINATOR;
      spipResponseLength = 2; // true response length
    }
    ezspSpiStartTxTransfer(spipResponseLength);
    CLR_nHOST_INT(); // tell host to get the response
    DEBUG_CLEAR_LED();
  } else if(spipResponseLength == 0){ // no data to transmit, pump receive side
    // activate receive if not already and nSSEL is inactive
    if( (!ezspSpiRxActive()) && nSSEL_IS_NEGATED()) {
      // flush RX FIFO since the wait and response section overflowed it
      ezspSpiFlushRxFifo();

      ezspSpiEnableReception();
    }
    // check for valid start of data
    if ( ezspSpiValidStartOfData() ){      
      spipFlagTransactionActive = true; // RX'ed, definitely in a transaction
      SET_nHOST_INT(); // by clocking a byte, the Host ack'ed nHOST_INT
      
      //if we have unloaded, know command arrived so jump directly there
      //bypassing RXCNT checks.  On em2xx this is needed because unload
      //clears RXCNT; on em3xx it is simply a convenience.
      if(ezspSpiCheckIfUnloaded()) {
        //While em2xx could get away with ACKing unload interrupt here,
        //because unload clears RXCNT, em3xx *must* do it below otherwise
        //a just-missed unload leaving RXCNT intact could mistakenly come
        //back to haunt us as a ghost command. -- BugzId:14622.
        goto dmaUnloadOnBootloaderFrame;
      }
      // we need at least 2 bytes before processing the command
      if(ezspSpiHaveTwoBytes()){ 
        // take action depending on the command
        switch(halHostCommandBuffer[0]) {
          case 0x0A:
            processSpipCommandAndRespond(SPIP_VERSION);
            break;
          case 0x0B:
            processSpipCommandAndRespond(SPIP_ALIVE);
            break;
          case 0xFE: // the command is an EZSP frame
            // fall into bootloader frame since processing the rest of the
            // command is the same. The only difference is responding with the 
            // unsupported SPI command error
          case 0xFD: // the command is a bootloader frame
            // guard against oversized message which could cause serious problems
            if(halHostCommandBuffer[1] > MAX_PAYLOAD_FRAME_LENGTH) {
              wipeAndRestartSpi();
              setSpipErrorBuffer(SPIP_OVERSIZED_EZSP);
              return false;
            }
            // check for all data before responding that we have a valid buffer
            if(ezspSpiHaveAllData()){
dmaUnloadOnBootloaderFrame:
              DEBUG_SET_LED();
              ezspSpiDisableReceptionInterrupts();
              ezspSpiDisableReception();  //disable reception while processing
              ezspSpiAckUnload(); // ack command unload --BugzId:14622 (em3xx only)
              DEBUG_CLEAR_LED();
              // check for frame terminator, it must be there!
              if(spipFlagOverrideResponse) {
                halHostSerialTick(true); // respond immediately!
                return false; // we overrode the command
              } else if(halHostCommandBuffer[halHostCommandBuffer[1]+2] != FRAME_TERMINATOR) {
                // no frame terminator found! report missing frame terminator
                setSpipErrorBuffer(SPIP_MISSING_FT);
                halHostSerialTick(true); // resond immediately!
                return false; // we overrode the command
              } else if(halHostCommandBuffer[0]==0xFE){
                // load error response buffer with unsupported SPI command error
                setSpipErrorBuffer(SPIP_UNSUPPORTED_COMMAND);
                halHostSerialTick(true); // respond immediately!
                return false; // we overrode the command
              } else {
                halHostResponseBuffer[0] = 0xFD; // mark respond bootloader frame
                return true; // there is a valid command
              }
            }
            break;
          default:
            break;
        }
      }
    }
  }

  return false;
}

//Since the bootloader does not use interrupts, the upper layer must use this
//function to poll for the status of the SPI Protocol pins.
static void halGpioPolling(void)
{
  //polling for the first byte on MOSI
  if(ezspSpiPollForMosi(spipResponseLength)){
    SET_nHOST_INT(); //by clocking a byte, the Host ack'ed nHOST_INT
    spipFlagTransactionActive = true; //RX'ed, definitely in a transaction
  }
  
#ifndef DISABLE_NWAKE
  //polling for the falling edge of the nWAKE
  //this fakes out halGpioIsr() in the normal SPI Protocol
  if(ezspSpiPollForNWAKE()) {
    //A wakeup handshake should be performed in response to a falling edge on
    //the WAKE line. The handshake should only be performed on a SerialTick.
    spipFlagWakeFallingEdge = true;
  }
#endif
  
  //polling for the rising edge of nSSEL
  //this fakes out halTimerIsr() in the normal SPI Protocol
  if(ezspSpiPollForNSSEL()) {
    //normally nHOST_INT is idled in the RXVALID Isr, but with short and fast
    //Responses, it's possible to service nSSEL before RXVALID, but we
    //still need to idle nHOST_INT.  If there is a pending callback, it will be
    //alerted via nHOST_INT at the end of this function.
    SET_nHOST_INT();

    //if we have not sent the exact right number of bytes, Transaction is corrupt
    if(!ezspSpiTransactionValid(spipResponseLength)) {
      setSpipErrorBuffer(SPIP_ABORTED_TRANSACTION);
    }

    //It's always safer to wipe the SPI clean and restart between transactions
    wipeAndRestartSpi();
    
    if(!spipFlagIdleHostInt) {
      CLR_nHOST_INT(); //we still have more to tell the Host
    }
  }
}

bool halHostSerialTick(bool responseReady)
{
  //Processing a potential premature nSSEL deactivation inside of the Tick
  //function will generate a lot of strange conditions that are best prevented
  //insteaded of handled.
  //Normal calls to halInternalHostSerialTick are <10us.  Worst case is <30us.
  //NOTE - The normal SPI Protocol wraps this call with an ATOMIC due to the
  //       above reason.  Since the bootloader does not use interrupts, we do
  //       not need to wrap this call in an ATOMIC.
  halGpioPolling();
  return halInternalHostSerialTick(responseReady);
}
