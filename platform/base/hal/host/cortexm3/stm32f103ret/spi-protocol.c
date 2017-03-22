/** @file hal/host/cortexm3/spi-protocol.c
 *  @brief Example host SPI Protocol implementation for interfacing with NCP.
 * 
 * 
 * SPI Protocol Interface:
 * 
 * void halNcpSerialInit(void)
 * void halNcpSerialPowerup(void)
 * void halNcpSerialPowerdown(void)
 * EzspStatus halNcpHardReset(void)
 * EzspStatus halNcpHardResetReqBootload(bool requestBootload)
 * void halNcpWakeUp(void)
 * void halNcpSendCommand(void)
 * void halNcpSendRawCommand(void)
 * EzspStatus halNcpPollForResponse(void)
 * bool halNcpHasData(void)
 * void halNcpIsAwakeIsr(bool isAwake)
 * uint8_t *halNcpFrame
 * uint8_t halNcpSpipErrorByte
 * bool halNcpVerifySpiProtocolVersion(void)
 * bool halNcpVerifySpiProtocolActive(void)
 * 
 * <!-- Copyright 2010 by Ember Corporation. All rights reserved.        *80*-->
 */

#include PLATFORM_HEADER
#include "stack/include/error.h"
#include "hal/hal.h"
#include "hal/host/spi-protocol-common.h"

//Define some basic SPI Protocol parameters described in the NCP Datasheet
#define DESIRED_SPIP_VERSION_RESPONSE 0x82
#define DESIRED_SPIP_STATUS_RESPONSE  0xC1
#define DESIRED_FRAME_TERMINATOR      0xA7
#define SPIP_BUFFER_SIZE              136
#define MAX_PAYLOAD_FRAME_LENGTH      133
#define EZSP_LENGTH_INDEX             1

//To save RAM usage, define a single transaction buffer.  This buffer is
//filled by the EZSP for a Command, then overwritten with a Response.
uint8_t halNcpSpipBuffer[SPIP_BUFFER_SIZE];
//This error byte is the third byte found in a special SPI Protocol error case.
//It provides more detail concerning the error.  Refer to the NCP Datasheet.
uint8_t halNcpSpipErrorByte;
//Provide easy references to the buffer for EZSP.  This handle is
//made externally available (to EZSP) via the spi protocol header.
uint8_t *halNcpFrame = halNcpSpipBuffer + EZSP_LENGTH_INDEX;

//The NCP mandates an inter-command spacing between the rising edge of nSSEL
//(end transaction) to the falling edge of nSSEL (start transaction).
//The inter-command spacing will be measured with a timer and an interrupt.
//Use a flag to indicate if OK to transmit (the interrupt will set the flag)
bool halNcpClearToTransmit = false;

//A simple flag that is either set or cleared on powerup to record if the
//NCP has data for the Host.  This flag is necessary because we cannot
//guarantee exiting sleep that the Host recorded an assertion of nHOST_INT.
bool halNcpHostIntAssertedOnPowerup = false;

//A globabl status for the SPI Protocol.  This status is used to track the
//current state of the SPI Protocol.  Specifically, we need to track state
//between sending a command and polling for a response.
EzspStatus spipStatus = EZSP_SPI_ERR_FATAL;


//// nHOST_INT CONVENIENCE MACROS
//true if nHOST_INT is asserted (low), false otherwise
#define nHOST_INT_ASSERTED \
  (GPIO_ReadInputDataBit(SPIP_nHOST_INT_PORT, SPIP_nHOST_INT_PIN) == Bit_RESET)
//true if nHOST_INT ISR is pending
#define nHOST_INT_PENDING \
  (EXTI_GetFlagStatus(EXTI_Line4) == SET)
//Disable the nHOST_INT ISR.  We'll only poll for pending status.
#define nHOST_INT_ISR_OFF()             \
  do {                                  \
    ATOMIC(                             \
      NVIC_DisableIRQ(EXTI4_IRQn);      \
      EXTI_ClearFlag(EXTI_Line4);       \
      NVIC_ClearPendingIRQ(EXTI4_IRQn); \
    )                                   \
  } while(0)
//Enable the nHOST_INT ISR.
#define nHOST_INT_ISR_ON()              \
  do {                                  \
    ATOMIC(                             \
      NVIC_EnableIRQ(EXTI4_IRQn);       \
      EXTI_ClearFlag(EXTI_Line4);       \
      NVIC_ClearPendingIRQ(EXTI4_IRQn); \
    )                                   \
  } while(0)


//// nWAKE CONVENIENCE MACROS
//Idles (deasserts high) nWAKE
#define IDLE_nWAKE()                                         \
  do {                                                       \
    GPIO_WriteBit(SPIP_nWAKE_PORT, SPIP_nWAKE_PIN, Bit_SET); \
  } while(0)
//Asserts (low) nWAKE
#define ASSERT_nWAKE()                                         \
  do {                                                         \
    GPIO_WriteBit(SPIP_nWAKE_PORT, SPIP_nWAKE_PIN, Bit_RESET); \
  } while(0)
//true if nWAKE is asserted (low), false otherwise
#define nWAKE_ASSERTED \
  (GPIO_ReadInputDataBit(SPIP_nWAKE_PORT, SPIP_nWAKE_PIN) == Bit_RESET)


//// nSSEL CONVENIENCE MACROS
//Idles (deasserts high) nSSEL
#define IDLE_nSSEL()                                         \
  do {                                                       \
    GPIO_WriteBit(SPIP_nSSEL_PORT, SPIP_nSSEL_PIN, Bit_SET); \
  } while(0)
//Asserts (low) nSSEL
#define ASSERT_nSSEL()                                         \
  do {                                                         \
    GPIO_WriteBit(SPIP_nSSEL_PORT, SPIP_nSSEL_PIN, Bit_RESET); \
  } while(0)


//// nRESET CONVENIENCE MACROS
//Idles (deasserts high) nRESET by making it an input with pullup
#define IDLE_nRESET()                                 \
  do {                                                \
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;     \
    GPIO_InitStructure.GPIO_Pin = SPIP_nRESET_PIN;    \
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; \
    GPIO_Init(SPIP_nRESET_PORT, &GPIO_InitStructure); \
  } while(0)
//Asserts (low) nSSEL by making it a low push-pull output
#define ASSERT_nRESET()                                          \
  do {                                                           \
    GPIO_WriteBit(SPIP_nRESET_PORT, SPIP_nRESET_PIN, Bit_SET);   \
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;             \
    GPIO_InitStructure.GPIO_Pin = SPIP_nRESET_PIN;               \
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;            \
    GPIO_Init(SPIP_nRESET_PORT, &GPIO_InitStructure);            \
    GPIO_WriteBit(SPIP_nRESET_PORT, SPIP_nRESET_PIN, Bit_RESET); \
  } while(0)


//// SPI PROTOCOL TIMER CONVENIENCE MACROS
//Configure the SPIP Timer to expire at a provided duration.  The onoff
//parameter is either ENABLE or DISABLE and can be used to choose if
//polling or ISR driven.
#define SET_SPIP_TIMER(duration, onoff)         \
  do {                                          \
    ATOMIC(                                     \
      TIM_SetAutoreload(TIM2, duration);        \
      TIM_SetCounter(TIM2, 0x0000);             \
      TIM_ITConfig(TIM2, TIM_IT_Update, onoff); \
      TIM_ClearFlag(TIM2, TIM_FLAG_Update);     \
      NVIC_ClearPendingIRQ(TIM2_IRQn);          \
    )                                           \
  } while(0)
//true if the SPI Protocol Timer has reached it's scheduled value
#define SPIP_TIMER_EXPIRED \
  (TIM_GetFlagStatus(TIM2, TIM_FLAG_Update) == SET)


static uint8_t spiWriteRead(uint8_t data)
{
  SPI_I2S_SendData(SPI1, data);
  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET) {}
  data = (uint8_t)SPI_I2S_ReceiveData(SPI1);
  return data;
}


void halNcpSerialInit(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  SPI_InitTypeDef SPI_InitStructure;
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  
  ////---- Configure SPI 1 for full duplex, master mode, 2.25Mhz ----////
    //Configure SPI1 pins for full duplex master:
    //  PA4/nSS as output
    IDLE_nSSEL();
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = SPIP_nSSEL_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SPIP_nSSEL_PORT, &GPIO_InitStructure);
    //  PA5/SCK as AF push-pull
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = SPIP_SCLK_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SPIP_SCLK_PORT, &GPIO_InitStructure);
    //  PA7/MOSI as AF push-pull
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = SPIP_MOSI_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SPIP_MOSI_PORT, &GPIO_InitStructure);
    //  PA6/MISO as input pull-up
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = SPIP_MISO_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SPIP_MISO_PORT, &GPIO_InitStructure);
    
    //Configure SPI (but without enabling it)
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    //PCLK2/32 = 72MHz/32 = 2.25MHz
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStructure);
    
    //Enable SPI
    SPI_Cmd(SPI1, ENABLE);
  
  
  ////---- Configure Timer 2 for calculating timeouts ----////
    //Enable TIM2 for simple upcounting
    //Choose a prescaler good for delays of 1ms-7.5s using a 16bit counter:
    //PCLK1/36000 = 72MHz/9000 = 8KHz (125us tick)
    TIM_TimeBaseInitStructure.TIM_Prescaler = 9000;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Period = 0xFFFF;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
    
    //Enable TIM2
    TIM_Cmd(TIM2, ENABLE);
    
    //Leave TIM2 (second level) interrupt disabled.  It will be enabled at
    //the appropriate time.
    TIM_ITConfig(TIM2, TIM_IT_Update, DISABLE);
    
    //Enable TIM2 (top level) NVIC interrupt and configure the priority
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    
  ////---- Configure nHOST_INT on PC4 for input, pullup, falling edge ----////
    halNcpHostIntAssertedOnPowerup = false;
    
    //Initialize PC4/nHOST_INT for input pull-up
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = SPIP_nHOST_INT_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SPIP_nHOST_INT_PORT, &GPIO_InitStructure);
    
    //Initialize EXTI4 to GPIO PC4
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource4);
    
    //Initialize EXTI4 to be a falling edge interrupt
    EXTI_InitStructure.EXTI_Line = EXTI_Line4;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    
    //Initialize EXTI4 (second level) interrupt by clearing it
    EXTI_ClearFlag(EXTI_Line4);
    
    //Initialize EXTI4 (top level) NVIC interrupt and configure the priority
    //NOTE: Start with disabled EXTI4 interrupt.  It will only be enabled in
    //      halNcpWakeUp and always cleared in the timer and nHOST_INT ISRs.
    //      This interrupt is still available as a flag, though.  EXTI4 has
    //      to be disabled at the NVIC since the EXTI mask registers prevent
    //      reading a status and we need to poll the status.
    NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
    NVIC_Init(&NVIC_InitStructure);
    
  ////---- Configure nWAKE (PC5) for general purpose push-pull output ----////
    IDLE_nWAKE();
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = SPIP_nWAKE_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SPIP_nWAKE_PORT, &GPIO_InitStructure);
    
  ////---- Configure nRESET (PB0) for input-pullup or push-pull output ----////
    IDLE_nRESET();
}


void halNcpSerialPowerup(void)
{
  //check to see if nHOST_INT is already asserted and record the state
  if(nHOST_INT_ASSERTED) {
    halNcpHostIntAssertedOnPowerup = true;
  } else {
    halNcpHostIntAssertedOnPowerup = false;
  }
  //Disable nHOST_INT ISR to restore it to normal operation
  nHOST_INT_ISR_OFF();
}


void halNcpSerialPowerdown(void)
{
  //Enable nHOST_INT ISR to come out of sleep
  nHOST_INT_ISR_ON();
}


EzspStatus halNcpHardResetReqBootload(bool requestBootload)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  //reset the NCP and wait for it to come back.
  //assert nRESET low (by changing it to push-pull output)
  ASSERT_nRESET();
  //the state of the nWAKE pin while the NCP is booting asks the NCP to
  //either enter the bootloader (nWAKE is low) or continue like normal
  if(requestBootload) {
    //request bootloader
    ASSERT_nWAKE();
  } else {
    //continue with normal boot
    IDLE_nWAKE();
  }
  halCommonDelayMicroseconds(NCP_RESET_DELAY);
  //deassert nRESET (by returning it to input, pullup)
  IDLE_nRESET();
  halNcpHostIntAssertedOnPowerup = false;
  //wait for nHOST_INT to assert to say it is fully booted
  halResetWatchdog(); //reset the watchdog since this could take some time
  
  //It is legal to hijack our inter-command spacing timer at this point
  //because the NCP is not doing anything else.
  //Configure the timer for STARTUP_TIMEOUT, but do not enable the
  //timer interrupt or nHOST_INT interrupt since we'll be polling the
  //status of the timer interrupt and the nHOST_INT flag.
  ATOMIC(
    SET_SPIP_TIMER(STARTUP_TIMEOUT, DISABLE);
    nHOST_INT_ISR_OFF();
  )
  
  //Ideally we would use a call to halNcpHasData() here to see if nHOST_INT
  //has been asserted, but the function halNcpHasData() is
  //mutually exclusive with the use of nWAKE.  Since this is a special case
  //only used for triggering the bootloader, we can directly check for the
  //nHOST_INT falling edge here.
  while(!nHOST_INT_PENDING) {
    halResetWatchdog();
    //While the NCP hasn't finished starting up, check our timeout
    if(SPIP_TIMER_EXPIRED) {
      //If we run out of time, throw the proper error and
      //make sure nWAKE is back to idle.
      IDLE_nWAKE();
      return EZSP_SPI_ERR_STARTUP_TIMEOUT;
    }
  }
  
  //make sure nWAKE is back to idle
  IDLE_nWAKE();
  
  //It's possible that the NCP wants to notify us early of more events,
  //so clear the nHOST_INT flag.
  nHOST_INT_ISR_OFF();
  
  //the first check better return false due to the NCP resetting
  if(halNcpVerifySpiProtocolActive()!=false) {
    return EZSP_SPI_ERR_STARTUP_FAIL;
  }
  //the second check better return true indicating the SPIP is active
  if(halNcpVerifySpiProtocolActive()!=true) {
    return EZSP_SPI_ERR_STARTUP_FAIL;
  }
  //the third check better return true indicating the proper SPIP version
  if(halNcpVerifySpiProtocolVersion()!=true) {
    return EZSP_SPI_ERR_STARTUP_FAIL;
  }
  
  return EZSP_SUCCESS;
}


EzspStatus halNcpHardReset(void)
{
  return halNcpHardResetReqBootload(false);
}


void halNcpWakeUp(void)
{
  //check for nHOST_INT falling edge (nHOST_INT is already asserted)
  //the edge is captured by the external interrupt
  if(nHOST_INT_PENDING || halNcpHostIntAssertedOnPowerup) {
    halNcpIsAwakeIsr(true); //NCP is already awake, inform upper layers
    halNcpClearToTransmit = true;  //a handshake means NCP can receive data
  } else {
    //it is legal to hijack our inter-command spacing timer at this point
    //because if the NCP does finish this handshake then we know the NCP
    //is ready to receive a command.
    ATOMIC(
      SET_SPIP_TIMER(WAKE_HANDSHAKE_TIMEOUT, ENABLE);
      nHOST_INT_ISR_ON();
      //enable nHOST_INT interrupt for just the handshake
      halNcpHostIntAssertedOnPowerup = false;
      ASSERT_nWAKE();
    )
  }
}


//This function makes no assumption about the data in the SpipBuffer, it will
//just faithly try to perform the transaction
void halNcpSendRawCommand(void)
{
  uint8_t i;
  uint8_t length;
  uint8_t payloadLength;
  spipStatus = EZSP_SPI_ERR_FATAL;

  //Wait until we are clear to transmit (inter-command spacing requirement).
  //We would prefer to have the timer or wake handshake set this flag,
  //but just in case neither were used we can do a local delay here
  //NOTE: We cannot use the INTER_COMMAND_SPACING parameter since that
  //      parameter holds a value designed to be used with a hardware timer
  //      running at a certain frequency.  halCommonDelayMicroseconds() already
  //      provides proper hardware abstraction, therefore we schedule a simple
  //      1000us delay divided into 8us blocks.
  i = (1000 / 8) + 1;
  while((!halNcpClearToTransmit) && i>0) {
    halCommonDelayMicroseconds(8);
    i--;
  }
  
  //To maintain continuity in the transmission of the command, we go atomic
  ATOMIC(
    //start the transaction
    ASSERT_nSSEL();
    //the next command is not clear to send
    halNcpClearToTransmit = false;
    
    //determine the length of the Command
    if(halNcpSpipBuffer[0]==0xFE) { //EZSP payload
      payloadLength = halNcpSpipBuffer[1];
      length = payloadLength+2;
    } else if(halNcpSpipBuffer[0]==0xFD) { //Bootloader payload
      payloadLength = halNcpSpipBuffer[1];
      length = payloadLength+2;
    } else {
      payloadLength = 1;
      length = 1;
    }
    //guard against oversized payloads which could cause problems
    if(payloadLength > MAX_PAYLOAD_FRAME_LENGTH) {
      spipStatus = EZSP_SPI_ERR_EZSP_COMMAND_OVERSIZED;
    } else {
      spipStatus = EZSP_SUCCESS; //the command can now be marked as successful
      for(i=0;i<length;i++) {
        spiWriteRead(halNcpSpipBuffer[i]);
      }
      //finish Command with the Frame Terminator
      spiWriteRead(0xA7);
      //schedule the timeout, "start" the timer, disable the ISR, clear
      //the timeout flag, and clear the top level timer ISR
      SET_SPIP_TIMER(WAIT_SECTION_TIMEOUT, DISABLE);
      //We'll poll for asserting nHOST_INT
      nHOST_INT_ISR_OFF();
      halNcpHostIntAssertedOnPowerup = false;
      //we do NOT enable nHOST_INT interrupt since we'll poll for it
    }
  )
}

EzspStatus halNcpPollForResponse(void)
{
  uint8_t i;
  uint8_t spipByte;

  //To maintain continuity in the reception of the response, we go atomic
  ATOMIC(
    //SendCommand failed because of an oversized command, return an error
    if(spipStatus==EZSP_SPI_ERR_EZSP_COMMAND_OVERSIZED) {
      goto kickout; //dump!
    } else {
      spipStatus = EZSP_SPI_ERR_FATAL; //start from a fatal state
    }

    //check for nHOST_INT falling edge, keep waiting if not asserted
    //the edge is captured by the external interrupt
    if(!nHOST_INT_PENDING) {
      //if we wait past our timeout period, return an error
      if(SPIP_TIMER_EXPIRED) {
        spipStatus = EZSP_SPI_ERR_WAIT_SECTION_TIMEOUT;
        goto kickout; //dump!
      }
      spipStatus = EZSP_SPI_WAITING_FOR_RESPONSE;
    } else {
      //clock the SPI until we receive the actual response (spiByte!=0xFF)
      spipByte = spiWriteRead(0xFF);
      while(spipByte==0xFF) {
        //use temp variable to preserve Command
        spipByte = spiWriteRead(0xFF);
        //if no response data past our timeout period, return an error
        //This test is purposefully here, after the second byte read from
        //the NCP, because the NCP will always send a 0xFF as its first
        //byte, and we don't want post a timeout until we're sure the
        //NCP has no response ready.
        if(spipByte==0xFF && SPIP_TIMER_EXPIRED) {
          spipStatus = EZSP_SPI_ERR_WAIT_SECTION_TIMEOUT;
          goto kickout;
        }
      }
      //after this point, don't care about SPIP_TIMER_EXPIRED because
      //retrieving rest of response is deterministic and time-bounded.
      //determine the type of response and the length, then receive the rest
      if(spipByte==0x00) {  //NCP Reset error condition
        //record the Error Byte
        halNcpSpipErrorByte = spiWriteRead(0xFF);
        //check for frame terminator
        if(spiWriteRead(0xFF)!=DESIRED_FRAME_TERMINATOR) {
          spipStatus = EZSP_SPI_ERR_NO_FRAME_TERMINATOR;
        } else {
          spipStatus = EZSP_SPI_ERR_NCP_RESET;
        }
      } else if(spipByte<0x05) { //other error conditions
        //error conditions mean there is a error byte to receive
        //record the Error Byte
        halNcpSpipErrorByte = spiWriteRead(0xFF);
        if(spipByte==0x01) {
          spipStatus = EZSP_SPI_ERR_OVERSIZED_EZSP_FRAME;
        } else if(spipByte==0x02) {
          spipStatus = EZSP_SPI_ERR_ABORTED_TRANSACTION;
        } else if(spipByte==0x03) {
          spipStatus = EZSP_SPI_ERR_MISSING_FRAME_TERMINATOR;
        } else if(spipByte==0x04) {
          spipStatus = EZSP_SPI_ERR_UNSUPPORTED_SPI_COMMAND;
        }
        //check for frame terminator
        if(spiWriteRead(0xFF)!=DESIRED_FRAME_TERMINATOR) {
          spipStatus = EZSP_SPI_ERR_NO_FRAME_TERMINATOR;
        }
      } else if((spipByte==0xFE) || //normal EZSP payload
                (spipByte==0xFD) ) { //normal Bootloader payload
        halNcpSpipBuffer[0] = spipByte; //save the spipByte into the buffer
        halNcpSpipBuffer[1] = spiWriteRead(0xFF); //rx the length byte
        //guard against oversized messages which could cause serious problems
        if(halNcpSpipBuffer[1] > MAX_PAYLOAD_FRAME_LENGTH) {
          spipStatus = EZSP_SPI_ERR_EZSP_RESPONSE_OVERSIZED;
          goto kickout; //dump!
        }
        for(i=2;i<halNcpSpipBuffer[1]+2;i++) {
          halNcpSpipBuffer[i] = spiWriteRead(0xFF); //rx the message
        }
        //check for frame terminator
        if(spiWriteRead(0xFF)!=DESIRED_FRAME_TERMINATOR) {
          spipStatus = EZSP_SPI_ERR_NO_FRAME_TERMINATOR;
        } else {
          spipStatus = EZSP_SUCCESS;
        }
      } else if( (spipByte&0xC0) ==0x80 ) { //SPI Protocol Version Response
        halNcpSpipBuffer[0] = spipByte;
        //check for frame terminator
        if(spiWriteRead(0xFF)!=DESIRED_FRAME_TERMINATOR) {
          spipStatus = EZSP_SPI_ERR_NO_FRAME_TERMINATOR;
        } else {
          spipStatus = EZSP_SUCCESS;
        }
      } else if( (spipByte&0xC0) ==0xC0 ) { //SPI Protocol Status response
        halNcpSpipBuffer[0] = spipByte;
        //check for frame terminator
        if(spiWriteRead(0xFF)!=DESIRED_FRAME_TERMINATOR) {
          spipStatus = EZSP_SPI_ERR_NO_FRAME_TERMINATOR;
        } else {
          spipStatus = EZSP_SUCCESS;
        }
      } else {
        //we can only get here due to a bad SPIP Byte!
        assert(0);
      }
      
kickout:
      //"start" the timer looking for inter-command spacing and enable timer
      //interrupts
      SET_SPIP_TIMER(INTER_COMMAND_SPACING, ENABLE);
      nHOST_INT_ISR_OFF();
      halNcpHostIntAssertedOnPowerup = false;
      //End transaction (after the SPI idles)
      while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET) {}
      IDLE_nSSEL();
    }
  )
  return spipStatus;
}


void halNcpSendCommand(void)
{
  //The function halNcpSendCommand() is used by EZSP.  All other
  //calls use halNcpSendRawCommand() since it allows specifying the SPIP Byte.
  halNcpSpipBuffer[0] = 0xFE; //mark the Command an EZSP Frame
  halNcpSendRawCommand(); //call the raw SendCommand
}


bool halNcpHasData(void)
{
  //if nWAKE is asserted (logic low), we are in the middle of a wake handshake.
  //supress HasData reporting (return false), until outside of handshake
  if(!nWAKE_ASSERTED) {
    //if nHOST_INT asserted on powerup, 260 has data
    if(halNcpHostIntAssertedOnPowerup) {
      halNcpHostIntAssertedOnPowerup = false;
      return true;
    } else {
      //if nHOST_INT asserted normally, 260 has data
      //the edge is captured by the external interrupt
      return nHOST_INT_PENDING;
    }
  } else {
    return false;
  }
}


bool halNcpVerifySpiProtocolVersion(void)
{
  EzspStatus status = EZSP_SPI_WAITING_FOR_RESPONSE;
  //send the SPI Protocol Version request byte
  //and check it against our expected version response
  halNcpSpipBuffer[0] = 0x0A;
  halNcpSendRawCommand();
  while(status == EZSP_SPI_WAITING_FOR_RESPONSE) {
    status = halNcpPollForResponse();
  }
  if( (status == EZSP_SUCCESS) &&
      (halNcpSpipBuffer[0] == DESIRED_SPIP_VERSION_RESPONSE) ) {
    return true;
  }
  return false;
}


bool halNcpVerifySpiProtocolActive(void)
{
  EzspStatus status = EZSP_SPI_WAITING_FOR_RESPONSE;
  //send the SPI Protocol Status request byte
  //and check it against the desired "alive" response
  halNcpSpipBuffer[0] = 0x0B;
  halNcpSendRawCommand();
  while(status == EZSP_SPI_WAITING_FOR_RESPONSE) {
    status = halNcpPollForResponse();
  }
  if( (status == EZSP_SUCCESS) &&
      (halNcpSpipBuffer[0] == DESIRED_SPIP_STATUS_RESPONSE) ) {
    return true;
  }
  return false;
}


//Inter-command spacing and wake handshake timeout come in here
void TIM2_IRQHandler(void)
{
  //if wake handshake timeout expired, flag failure
  if(TIM2->ARR == WAKE_HANDSHAKE_TIMEOUT) {
    nHOST_INT_ISR_OFF();
    IDLE_nWAKE();
    halNcpHostIntAssertedOnPowerup = false;
    //flag NCP as unresponsive
    halNcpIsAwakeIsr(false);
  }
  
  //Stop using the timer
  SET_SPIP_TIMER(0xFFFF, DISABLE);
  //definitely waited the inter-command spacing
  halNcpClearToTransmit = true;
}


//Wake handshake response comes in here.  this interrupt is only enabled
//for the wake handshake (as opposed to callback and response ready states)
void EXTI4_IRQHandler(void)
{
  nHOST_INT_ISR_OFF();
  halNcpHostIntAssertedOnPowerup = false;
  //finish handshake
  IDLE_nWAKE();
  //Stop using the timer
  SET_SPIP_TIMER(0xFFFF, DISABLE);
  //a handshake means NCP can receive data
  halNcpClearToTransmit = true;
  //inform upper layers that we're awake
  halNcpIsAwakeIsr(true);
}

