/** @file hal/micro/cortexm3/spi-protocol.c
 *  @brief  EM3XX internal SPI Protocol implementation
 * 
 * 
 * SPI Protocol Interface:
 * 
 * void halHostSerialInit(void)
 * void halHostSerialPowerup(void)
 * void halHostSerialPowerdown(void)
 * void halHostCallback(bool haveData)
 * bool halHostSerialTick(bool responseReady)
 * uint8_t *halHostFrame
 * 
 * 
 * <!-- Author(s): Brooks Barrett -->
 * <!-- Copyright 2014 Silicon Labs, Inc.                                *80*-->
 */

#include PLATFORM_HEADER
#include "stack/include/ember.h"
#include "hal/hal.h"
#include "serial/serial.h"
#include "hal/micro/cortexm3/spi-protocol.h"
#include "hal/micro/cortexm3/ezsp-spi-serial-controller-autoselect.h"

#if 0
  #define DEBUG_SET_LED()   halSetLed(BOARD_ACTIVITY_LED)
  #define DEBUG_CLEAR_LED() halClearLed(BOARD_ACTIVITY_LED)
#else
  #define DEBUG_SET_LED()
  #define DEBUG_CLEAR_LED()
#endif

#if 0     // LED2 on PC5
  #define DEBUG_SET_LED2()   halSetLed(BOARDLED2)
  #define DEBUG_CLEAR_LED2() halClearLed(BOARDLED2)
#else
  #define DEBUG_SET_LED2()
  #define DEBUG_CLEAR_LED2()
#endif

#ifdef HALTEST
////////////////////////////
/////  TEST VARIABLES  /////
////////////////////////////

  uint8_t TM_EZSP_FRAME1[] = {0x00};
  uint8_t TM_EZSP_FRAME2[] = {0xFE,0x00};
  uint8_t TM_EZSP_FRAME3[] = {0xFE,0x01,0x10};
  uint8_t TM_EZSP_FRAME0[] = {0xFE,0x08,0x10,0x20,0x30,0x40,0x50,0x60,0x70,0x80};
  uint8_t TM_EZSP_FRAME8[] = {0xFE,0x7D,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,
                            0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x10,0x11,0x12,
                            0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,
                            0x1D,0x1E,0x1F,0x20,0x21,0x22,0x23,0x24,0x25,0x26,
                            0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,0x30,
                            0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,
                            0x3B,0x3C,0x3D,0x3E,0x3F,0x40,0x41,0x42,0x43,0x44,
                            0x45,0x46,0x47,0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,
                            0x4F,0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,
                            0x59,0x5A,0x5B,0x5C,0x5D,0x5E,0x5F,0x60,0x61,0x62,
                            0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x6B,0x6C,
                            0x6D,0x6E,0x6F,0x70,0x71,0x72,0x73,0x74,0x75,0x76,
                            0x77,0x78,0x79,0x7A,0x7B,0x7C,0x7D};
#endif //HALTEST

//The maximum SPI Protocol message size is 136 bytes. We define a buffer of
//142 specifically for error detection during the Response Section.  By using
//a buffer of 142, we can use the SCx_REG(TXCNT) register to monitor the state of
//the transaction and know that if a DMA TX unload occurs we have an error.
#define SPIP_BUFFER_SIZE             142
#define SPIP_ERROR_RESPONSE_SIZE     2
#define MAX_PAYLOAD_FRAME_LENGTH     133
#define EZSP_LENGTH_INDEX            1
#define RX_DMA_BYTES_LEFT_THRESHOLD  4
uint8_t halHostBuffer[SPIP_BUFFER_SIZE];
uint8_t spipErrorResponseBuffer[SPIP_ERROR_RESPONSE_SIZE];
//Provide easy references the buffers for EZSP
uint8_t *halHostFrame = halHostBuffer + EZSP_LENGTH_INDEX;

bool spipFlagWakeFallingEdge;  //flag for detecting a falling edge on nWAKE
bool spipFlagIdleHostInt;      //flag for idling nHOST_INT at proper time
bool spipFlagOverrideResponse; //flag for indicating the SPIP has just booted
bool spipFlagTransactionActive;//flag for indicating an active transaction
uint8_t spipResponseLength;         //The true length of the Response Section

static void wipeAndRestartSpi(void)
{
  spipFlagTransactionActive = false;//we're definitely outside a Transaction now
  spipResponseLength = 0;           //default length of zero
  
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
  SCx_REG(RXBEGA) = (uint32_t) halHostBuffer;
  SCx_REG(RXENDA) = (uint32_t) halHostBuffer + SPIP_BUFFER_SIZE -1;
  //Configure DMA TX channel to point to the response buffer
  SCx_REG(TXBEGA) = (uint32_t) halHostBuffer;
  SCx_REG(TXENDA) = (uint32_t) halHostBuffer + SPIP_BUFFER_SIZE -1;
  if(nSSEL_IS_NEGATED()) { //only activate DMA if nSSEL is idle
    INT_SCxCFG |= INT_SCRXVAL; //enable byte received interrupt
    INT_SCxCFG |= INT_SCRXULDA;//enable RX buffer A unload interrupt
    SCx_REG(DMACTRL) = SC_RXLODA;  //activate RX DMA for first command
  }
  INT_SCxFLAG = 0xFFFF;     //clear any stale interrupts
  INT_CFGSET = INT_SCx;     //enable top-level interrupt
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
    spipErrorResponseBuffer[1] = halGetEm2xxResetInfo(); //inject reset cause
  halHostSerialPowerup();
}

void halHostSerialPowerup(void)
{
  //On the next Tick call, the SPIP can assume we are fully booted and ready.
  
  ////---- Configure SPI ----////
    wipeAndRestartSpi();

  ////---- Configure basic SPI Pins: MOSI, MISO, SCLK and nSSEL ----////
    CFG_SPI_GPIO();
    PULLUP_nSSEL();
                    
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
    INT_CFGSET = nWAKE_INT;
    nWAKE_INTCFG =  (0 << GPIO_INTFILT_BIT) |   //no filter
                    (GPIOINTMOD_FALLING_EDGE << GPIO_INTMOD_BIT);
  
  ////---- Configure nSSEL_INT for compatibility with EM260 ----////
    CFG_nSSEL_INT(GPIOCFG_IN);            //input floating - not used

  ////---- Configure nSSEL interrupt ----////
    INT_CFGCLR = nSSEL_INT;               //disable triggering
    nSSEL_INTCFG = (GPIOINTMOD_DISABLED << GPIO_INTMOD_BIT);
    nSSEL_IRQSEL = nSSEL_IRQSEL_MASK;
    //Enable Interrupts
    INT_GPIOFLAG = nSSEL_GPIOFLAG;        //clear stale interrupts
    INT_PENDCLR = nSSEL_INT;
    INT_CFGSET = nSSEL_INT;
    nSSEL_INTCFG = (0 << GPIO_INTFILT_BIT) |  //no filter
                   (GPIOINTMOD_RISING_EDGE << GPIO_INTMOD_BIT);

  ////---- Configure nHOST_INT output ----////
    SET_nHOST_INT();
    CFG_nHOST_INT(GPIOCFG_OUT);

#ifdef ENABLE_NSIMRST
  // For debugging, configure nSIMRST //
    INT_CFGCLR = nSIMRST_INT;             //disable triggering
    nSIMRST_INTCFG = (GPIOINTMOD_DISABLED << GPIO_INTMOD_BIT);
    CFG_nSIMRST(GPIOCFG_IN_PUD);          //input with pull-up
    PULLUP_nSIMRST();
    INT_GPIOFLAG = nSIMRST_GPIOFLAG;      //clear stale interrupts
    INT_PENDCLR = nSIMRST_INT;
    INT_CFGSET = nSIMRST_INT;             //enable interrupt
    nSIMRST_INTCFG =(0 << GPIO_INTFILT_BIT) |   //no filter
                    (GPIOINTMOD_FALLING_EDGE << GPIO_INTMOD_BIT);
#endif
}

#ifdef ENABLE_NSIMRST
// nSIMRST falling edge interrupt replaces nRESET
void halIrqAIsr(void)
{
//  halInternalSysReset(RESET_BOOTLOADER_BOOTLOAD);
  halReboot();
}
#endif

void halHostSerialPowerdown(void)
{
  //we need to block (interrupts are off), until transmission is done
while(nSSEL_IS_ASSERTED()) { halResetWatchdog(); }
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

bool halHostSerialBusy(void)
{
  return ((nSSEL_IS_ASSERTED()) || spipFlagTransactionActive);
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



static bool halInternalHostSerialTick(bool responseReady);

bool halHostSerialTick(bool responseReady)
{
  bool validCommand;
  //Processing a potential premature nSSEL deactivation inside of the Tick
  //function will generate a lot of strange conditions that are best prevented
  //insteaded of handled.
  //Normal calls to halInternalHostSerialTick are <10us.  Worst case is <30us.
  ATOMIC(
    validCommand = halInternalHostSerialTick(responseReady);
  )
  return validCommand;
}


static void processSpipCommandAndRespond(uint8_t spipResponse)
{
  DEBUG_SET_LED();//show me when stopped receiving
  SCx_REG(DMACTRL) = SC_RXDMARST; //disable reception while processing
  DEBUG_CLEAR_LED();
  //check for Frame Terminator, it must be there!
  if(halHostBuffer[1]==FRAME_TERMINATOR) {
    //override with the supplied spipResponse
    halHostBuffer[0] = spipResponse;
  } else {
    //no frame terminator found!  report missing F.T.
    setSpipErrorBuffer(SPIP_MISSING_FT);
  }
  halHostSerialTick(true); //respond immediately!
}

//One layer of indirection is used so calling the public function will actually
//result in the real Tick function (this internal one) being wrapped in an
//ATOMIC() block to prevent potential corruption from the nSSEL interrupt.
static bool halInternalHostSerialTick(bool responseReady)
{
  //assert nHOST_INT if need to tell host something immediately and nSSEL=idle
  if(spipFlagOverrideResponse && nSSEL_IS_NEGATED()) {
    CLR_nHOST_INT();  //tell the host we just booted
  }
  
  if(spipFlagWakeFallingEdge) { //detected falling edge on nWAKE, handshake
    CLR_nHOST_INT();
    while( nWAKE_IS_ASSERTED() ) { halResetWatchdog(); /*EMHAL-1074*/ }
    SET_nHOST_INT();
    spipFlagWakeFallingEdge = false;
    //The wake handshake is complete, but spipFlagIdleHostInt is saying
    //that there is a callback pending.
    if(!spipFlagIdleHostInt) {
      halCommonDelayMicroseconds(50); //delay 50us so Host can get ready
      CLR_nHOST_INT();  //indicate the pending callback
    }
  } else if(responseReady && spipFlagTransactionActive) {  //OK to transmit
    DEBUG_SET_LED();
    if(spipFlagOverrideResponse) {
      spipFlagOverrideResponse = false; //we no longer need to override
      //override whatever was sent with the error response message
      MEMCOPY(halHostBuffer,
              spipErrorResponseBuffer,
              SPIP_ERROR_RESPONSE_SIZE);
    }
    if(spipFlagIdleHostInt) {
      SET_nHOST_INT();  //the nHOST_INT signal can be asynchronously
    }
    //add Frame Terminator and record true Response length
    if( halHostBuffer[0]<0x05 ) {
      halHostBuffer[1 +1] = FRAME_TERMINATOR;
      spipResponseLength = 3;  //true Response length
    } else if((halHostBuffer[0]==0xFE) || //EZSP Payload
              (halHostBuffer[0]==0xFD)) { //Bootloader Payload
      //guard against oversized messages which could cause serious problems
      assert(halHostBuffer[1] <= MAX_PAYLOAD_FRAME_LENGTH);
      halHostBuffer[halHostBuffer[1] +1 +1] = FRAME_TERMINATOR;
      halHostBuffer[halHostBuffer[1] +1 +2] = 0xFF; // pad so MISO stays high
      spipResponseLength = halHostBuffer[1]+3;  //true Response length
    } else {
      halHostBuffer[1] = FRAME_TERMINATOR;
      spipResponseLength = 2;  //true Response length
    }
    SCx_REG(DATA) = 0xFF; // emlipari-183: Prepend sacrificial Tx pad byte
    INT_SCxFLAG = INT_SCRXVAL; //clear byte received interrupt
    SCx_REG(DMACTRL) = SC_TXLODA;   //enable response for TX
    INT_SCxCFG |= INT_SCRXVAL; //enable byte received interrupt
    CLR_nHOST_INT();           //tell the Host to get the response
    DEBUG_CLEAR_LED();
  } else { //no data to transmit, pump receive side
    //activate receive if not already and nSSEL is inactive
    if( ((SCx_REG(DMASTAT)&SC_RXACTA)!=SC_RXACTA) && nSSEL_IS_NEGATED() ) {
      volatile uint8_t dummy;
      //flush RX FIFO since the Wait and Response section overflowed it
      dummy = SCx_REG(DATA);
      dummy = SCx_REG(DATA);
      dummy = SCx_REG(DATA);
      dummy = SCx_REG(DATA);
      INT_SCxFLAG = INT_SCRXVAL; //clear byte received interrupt
      INT_SCxFLAG = INT_SCRXULDA;//clear buffer A unload interrupt
      INT_SCxCFG |= INT_SCRXVAL; //enable byte received interrupt
      INT_SCxCFG |= INT_SCRXULDA;//enable buffer A unload interrupt
      SCx_REG(DMACTRL) = SC_RXLODA; //we are inter-command, activate RX DMA for next
    }
    //check for valid start of data (counter!=0)
    //check for unloaded buffer
    if( (SCx_REG(RXCNTA)!=0) || (INT_SCxFLAG & INT_SCRXULDA) ) {
      spipFlagTransactionActive = true; //RX'ed, definitly in a transaction
      SET_nHOST_INT();  //by clocking a byte, the Host ack'ed nHOST_INT
      //if we have unloaded, know command arrived so jump directly there
      //bypassing RXCNT checks.  On em2xx this is needed because unload
      //clears RXCNT; on em3xx it is simply a convenience.
      if(INT_SCxFLAG & INT_SCRXULDA) {
        //While em2xx could get away with ACKing unload interrupt here,
        //because unload clears RXCNT, em3xx *must* do it below otherwise
        //a just-missed unload leaving RXCNT intact could mistakenly come
        //back to haunt us as a ghost command. -- BugzId:14622.
        goto dmaUnloadOnEzspFrame;
      }
      //we need at least 2 bytes before processing the Command
      if(SCx_REG(RXCNTA)>1) {
        //take action depending on the Command
        switch(halHostBuffer[0]) {
          //case 0x0A: break; //SPI Bytes 0x0A and 0x0B are handled exclusively
          //case 0x0B: break; //inside of interrupt context (RXVALID)
          case 0xFD: //The Command is a Bootloader Frame
            //Fall into EZSP Frame since processing the rest of the command is
            //the same. The only difference is responding with the Unsupported
            //SPI Command error
          case 0xFE: //The Command is an EZSP Frame
            //guard against oversized messages which could cause serious problems
            if(halHostBuffer[1] > MAX_PAYLOAD_FRAME_LENGTH) {
              wipeAndRestartSpi();
              setSpipErrorBuffer(SPIP_OVERSIZED_EZSP);
              return false; //dump!
            }
            //check for  all data before responding that we have a valid buffer
            if(SCx_REG(RXCNTA) >= halHostBuffer[1]+3) {
dmaUnloadOnEzspFrame:
              DEBUG_SET_LED();//show me when stopped receiving
              INT_SCxCFG &= ~INT_SCRXVAL;//disable byte received interrupt
              INT_SCxCFG &= ~INT_SCRXULDA;//disable buffer A unload interrupt
              SCx_REG(DMACTRL) = SC_RXDMARST; //disable reception while processing
              INT_SCxFLAG = INT_SCRXULDA; //ack command unload --BugzId:14622
              DEBUG_CLEAR_LED();
              //check for Frame Terminator, it must be there!
              if(spipFlagOverrideResponse) {
                halHostSerialTick(true); //respond immediately!
                return false; //we overrode the command
              } else if(halHostBuffer[halHostBuffer[1]+2]
                        !=FRAME_TERMINATOR) {
                //no frame terminator found!  report missing F.T.
                setSpipErrorBuffer(SPIP_MISSING_FT);
                halHostSerialTick(true); //respond immediately!
                return false; //we overrode the command
              } else if(halHostBuffer[0]==0xFD) {
                //load error response buffer with Unsupported SPI Command error
                setSpipErrorBuffer(SPIP_UNSUPPORTED_COMMAND);
                halHostSerialTick(true); //respond immediately!
                return false; //we overrode the command
              } else {
                halHostBuffer[0] = 0xFE; //mark the response EZSP Frame
                return true; //there is a valid command
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

//All SPI operation interrupts come in here
void SCx_ISR(void)
{
  INT_SCxFLAG = INT_SCRXVAL;   //clear byte received interrupt
  if( (INT_SCxFLAG&INT_SCRXULDA)==INT_SCRXULDA ) {
    INT_SCxCFG &= ~INT_SCRXULDA;  //disable RX buffer A unload interrupt
    //N.B. Just disable, but don't Acknowledge the unload interupt here,
    //     letting it be detected and acknowledged in halHostSerialTick().
    //     The unload interrupt is primarily used to ensure the NCP wakes
    //     when host's command fully arrives, rather than waking on every
    //     byte during command receipt.
  } else { //INT_SCRXVAL bit gets us here
    SET_nHOST_INT();  //by clocking a byte, the Host ack'ed nHOST_INT
    if( (SCx_REG(DMASTAT)&SC_TXACTA)==SC_TXACTA ) { //TX'ing a response right now
      INT_SCxCFG &= ~INT_SCRXVAL;   //disable byte received interrupt
      return; //no more processing needed
    }
    spipFlagTransactionActive = true; //RX'ed, definitly in a transaction
  
    //we need at least 2 bytes before processing the Command
    if(SCx_REG(RXCNTA)>1) {
      //take action depending on the Command
      switch(halHostBuffer[0]) {
        case 0x0A:
          INT_SCxCFG &= ~INT_SCRXVAL;   //disable byte received interrupt
          processSpipCommandAndRespond(SPIP_VERSION);
          break;
        case 0x0B:
          INT_SCxCFG &= ~INT_SCRXVAL;   //disable byte received interrupt
          processSpipCommandAndRespond(SPIP_ALIVE);
          break;
        case 0xFD: //The Command is a Bootloader Frame
          //Fall into EZSP Frame since processing the rest of the command is
          //the same. The only difference is responding with the Unsupported
          //SPI Command error
        case 0xFE: //The Command is an EZSP Frame
          //guard against oversized messages which could cause serious problems
          if(halHostBuffer[1] > MAX_PAYLOAD_FRAME_LENGTH) {
            wipeAndRestartSpi();
            setSpipErrorBuffer(SPIP_OVERSIZED_EZSP);
            return; //dump! (interrupt flags are cleared above)
          }
          //time from reading SCx_REG(RXCNTA) to setting SCx_REG(RXENDA) should be < 3us
          //at max speed (5MHz), two bytes are 3.2us
          if( ((int8_t)((halHostBuffer[1]+3)-SCx_REG(RXCNTA))) > RX_DMA_BYTES_LEFT_THRESHOLD ) {
            SCx_REG(RXENDA) = (uint32_t) (halHostBuffer + halHostBuffer[1]+2);
            INT_SCxCFG &= ~INT_SCRXVAL;   //disable byte received interrupt
          } else if(SCx_REG(RXCNTA) >= halHostBuffer[1]+3) {
            INT_SCxCFG &= ~INT_SCRXVAL;   //disable byte received interrupt
          }
          break;
        default:
          break;
      }
    }
  }
}


//nWAKE signal (falling edge-triggered)
void nWAKE_ISR(void)
{
  // ack int before read to avoid potential of missing interrupt
  INT_GPIOFLAG = nWAKE_GPIOFLAG;

  //A wakeup handshake should be performed in response to a falling edge on
  //the WAKE line. The handshake should only be performed on a SerialTick.
  spipFlagWakeFallingEdge = true;
  
}

//nSSEL signal (rising edge-triggered)
//When using SC2, may be jumpered to PB3 for compatibility with the EM260, but
//the input at PB3 is not used by the spi protocol.
void nSSEL_ISR(void)
{
  INT_GPIOFLAG = nSSEL_GPIOFLAG;

  //normally nHOST_INT is idled in the RXVALID Isr, but with short and fast
  //Responses, it's possible to service nSSEL before RXVALID, but we
  //still need to idle nHOST_INT.  If there is a pending callback, it will be
  //alerted via nHOST_INT at the end of this Isr about 20us after this line.
  SET_nHOST_INT();

  //if we have not sent the exact right number of bytes, Transaction is corrupt
  if((SCx_REG(TXCNT)!=spipResponseLength+4)) {
    setSpipErrorBuffer(SPIP_ABORTED_TRANSACTION);
  }
  
  //It's always safer to wipe the SPI clean and restart between transactions
  wipeAndRestartSpi();
  
  if(!spipFlagIdleHostInt) {
    CLR_nHOST_INT();  //we still have more to tell the Host
  }
 
}

#ifdef HALTEST
///////////////////////
/////  TEST CODE  /////
///////////////////////

void spipTest(uint16_t test, uint16_t params )
{
  uint8_t ch=0;
  
  if(test==0) { //simulate a main program loop
    emberSerialPrintf(0,"Simulating EZSP main loop...\r\n");
    while (ch != 'e') {
      if(ch == 'b') {
        uint8_t i;
        emberSerialPrintf(0, "RXCNTA = %d; ",SCx_REG(RXCNTA));
        for(i=0;i<SPIP_BUFFER_SIZE;i++) {
          emberSerialPrintf(0, "%X ",halHostBuffer[i]);
        }
        emberSerialPrintf(0, "\r\n");
      }
      if(ch == 'd') {
        halHostCallback(true);
        emberSerialPrintf(0,"haveData=true\r\n");
      }
      if(ch == 'n') {
        halHostCallback(false);
        emberSerialPrintf(0,"haveData=false\r\n");
      }
      if(ch == 'c') {
        //clear buffer with markers to see used space
        MEMSET(halHostBuffer,0x55,SPIP_BUFFER_SIZE);
      }
      if(ch == '1') {
        MEMCOPY(halHostBuffer,TM_EZSP_FRAME1,sizeof(TM_EZSP_FRAME1));
      }
      if(ch == '2') {
        MEMCOPY(halHostBuffer,TM_EZSP_FRAME2,sizeof(TM_EZSP_FRAME2));
      }
      if(ch == '3') {
        MEMCOPY(halHostBuffer,TM_EZSP_FRAME3,sizeof(TM_EZSP_FRAME3));
      }
      if(ch == '0') {
        MEMCOPY(halHostBuffer,TM_EZSP_FRAME0,sizeof(TM_EZSP_FRAME0));
      }
      if(ch == '8') {
        MEMCOPY(halHostBuffer,TM_EZSP_FRAME8,sizeof(TM_EZSP_FRAME8));
      }
      if(halHostSerialTick(false)) {
        halHostSerialTick(false); //keep ourselves honest with tracking state
        //stuff ResponseBuffer with the command if we saw a NOP
        if((halHostBuffer[0]==0xFE)&&(halHostBuffer[3]==0x05)) {
          MEMMOVE(halHostBuffer,halHostBuffer,SPIP_BUFFER_SIZE);
        }
        halHostSerialTick(true);
      }
      
      emberSerialReadByte(0,&ch);
    }
  } else if(test==1) { //print the halHostBuffer
    uint8_t i;
    emberSerialPrintf(0, "RXCNTA = %d; ",SCx_REG(RXCNTA));
    for(i=0;i<SPIP_BUFFER_SIZE;i++) {
      emberSerialPrintf(0, "%X ",halHostBuffer[i]);
    }
    emberSerialPrintf(0, "\r\n");
  } else if(test==2) {
    halHostSerialInit();
  } else if(test==3) {
  } else if(test==4) {
  } else if(test==0xF) { //simulate a main program loop for deep sleep testing
    emberSerialPrintf(0,"Waiting for sleep command from host...\r\n");
    while (ch != 'e') {
      if(halHostSerialTick(false)) {
        //we're looking for a deep sleep NOP
        if( (halHostBuffer[0]==0xFE) &&
            (halHostBuffer[2]==0x02) &&
            (halHostBuffer[3]==0x05)) {
          void shutdownTest(uint16_t param1, uint16_t param2);
          shutdownTest(0,0);
        }
        halHostSerialTick(true);
      }
      emberSerialReadByte(0,&ch);
    }
  }
}

// Haltest Stubs
void halSc2SpiInit(bool master,bool cpha,bool cpol,bool dord,bool rxmode,bool resend,uint8_t prescaler,uint8_t baud) {}
void halSc2SpiStartTest(void) {}
void halSc2SpiStopTest(bool beForceful) {}
void halSc2SpiDmaReset(void) {}
void halSc2SpiDisable(void) {}
void halSc2I2cInit(void) {}
void halSc2I2cDisable(void) {}
void halSc2I2cRead(void) {}
void halSc2I2cWrite(void) {}
#endif // HALTEST
