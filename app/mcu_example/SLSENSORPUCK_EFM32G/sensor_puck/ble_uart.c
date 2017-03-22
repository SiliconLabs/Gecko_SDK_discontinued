/**************************************************************************//**
* @file
* @brief Bluetooth Low Energy UART interface driver
* @version 5.1.2

******************************************************************************
* @section License
* <b>Copyright 2015 Silicon Labs, Inc. http://www.silabs.com</b>
*******************************************************************************
*
* This file is licensensed under the Silabs License Agreement. See the file
* "Silabs_License_Agreement.txt" for details. Before using this software for
* any purpose, you must agree to the terms of that agreement.
*
******************************************************************************/

#include "em_device.h"
#include "em_cmu.h"
#include "em_common.h"
#include "em_leuart.h"
#include "em_dma.h"
#include "em_gpio.h"
#include "ble.h"
#include "trace.h"


/* DEFINES */

#define RX_DMA_CHAN       0
#define TX_DMA_CHAN       1

#define MIN_FRAME_SIZE    6
#define MAX_FRAME_SIZE    64
#define MAX_MSG_SIZE      32

#define FLAG              0x7E
#define ESCAPE            0x7D

#define TX_PORT           gpioPortD
#define RX_PORT           gpioPortD
#define TX_PIN            4
#define RX_PIN            5

/* GLOBAL VARIABLES */

static uint8_t TxFrame[MAX_FRAME_SIZE];
static uint8_t RxFrame[MAX_FRAME_SIZE];

/* DMA control block, must be aligned to 256. */
SL_ALIGN(256)
DMA_DESCRIPTOR_TypeDef dmaControlBlock[2] SL_ATTRIBUTE_ALIGN(256);

/* DMA callback structure */
static DMA_CB_TypeDef cb[2];

/* Defining the LEUART0 initialization data */
static LEUART_Init_TypeDef leuart1Init =
{
  .enable   = leuartEnable,       /* Activate data reception on LEUn TX and RX pins. */
  .refFreq  = 0,                  /* Inherit the clock frequenzy from the LEUART clock source */
  .baudrate = 9600,               /* Baudrate = 9600 bps */
  .databits = leuartDatabits8,    /* Each LEUART frame containes 8 databits */
  .parity   = leuartNoParity,     /* No parity bits in use */
  .stopbits = leuartStopbits1,    /* Setting the number of stop bits in a frame to 1 bitperiods */
};

/* DMA init structure */
static DMA_Init_TypeDef dmaInit =
{
  .hprot        = 0,                  /* No descriptor protection */
  .controlBlock = dmaControlBlock,    /* DMA control block alligned to 256 */
};

/* Setting up RX DMA channel */
static DMA_CfgChannel_TypeDef rxChnlCfg =
{
  .highPri   = false,                     /* Normal priority */
  .enableInt = false,                     /* No interupt enabled for callback functions */
  .select    = DMAREQ_LEUART0_RXDATAV,    /* Set LEUART0 RX data avalible as source of DMA signals */
  .cb        = NULL,                      /* No callback funtion */
};

/* Setting up TX DMA channel */
static DMA_CfgChannel_TypeDef txChnlCfg =
{
  .highPri   = false,                   /* Normal priority */
  .enableInt = false,                   /* No interupt for callback function */
  .select    = DMAREQ_LEUART0_TXBL,     /* Set LEUART0 TX buffer empty, as source of DMA signals */
  .cb        = &(cb[TX_DMA_CHAN]),      /* Callback */
};

/* Setting up RX DMA channel descriptor */
static DMA_CfgDescr_TypeDef rxDescrCfg =
{
  .dstInc  = dmaDataInc1,       /* Increment destination address by one byte */
  .srcInc  = dmaDataIncNone,    /* Do no increment source address  */
  .size    = dmaDataSize1,      /* Data size is one byte */
  .arbRate = dmaArbitrate1,     /* Rearbitrate for each byte recieved*/
  .hprot   = 0,                 /* No read/write source protection */
};

/* Setting up TX DMA channel descriptor */
static DMA_CfgDescr_TypeDef txDescrCfg =
{
  .dstInc  = dmaDataIncNone,    /* Do not increment destination address */
  .srcInc  = dmaDataInc1,       /* Increment source address by one byte */
  .size    = dmaDataSize1,      /* Data size is one byte */
  .arbRate = dmaArbitrate1,     /* Rearbitrate for each byte recieved */
  .hprot   = 0,                 /* No read/write source protection */
};


static uint16_t CRC_Table[256] =
{
  0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
  0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
  0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
  0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
  0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
  0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
  0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
  0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
  0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
  0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
  0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
  0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
  0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
  0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
  0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
  0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
  0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
  0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
  0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
  0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
  0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
  0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
  0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
  0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
  0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
  0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
  0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
  0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
  0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
  0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
  0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
  0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};


/**************************************************************************//**
 * @brief FindCRC
 *****************************************************************************/

static uint16_t FindCRC(uint8_t *pData, int DataLength)
{
  uint16_t CRC = 0xFFFF;

  while (DataLength--)
    CRC = (CRC >> 8) ^ CRC_Table[(CRC ^ *pData++) & 0xFF];

  return CRC;
}


/**************************************************************************//**
 * @brief StuffByte
 *****************************************************************************/

static uint8_t *StuffByte(uint8_t *pFrame, uint8_t Byte)
{
  if ((Byte == FLAG) || (Byte == ESCAPE))
  {
    *pFrame++ = ESCAPE;
    *pFrame++ = Byte ^ 0x20;
  }
  else
    *pFrame++ = Byte;

  return pFrame;
}


/**************************************************************************//**
 * @brief BLE_UART_Send
 *****************************************************************************/

void BLE_UART_Send(uint8_t *pMsg, int MsgLength)
{
  uint16_t FCS;
  uint8_t  *pMsgLimit;
  uint8_t  *pFrame = TxFrame;
#ifdef TRACE
  int x;   /* Trace */
#endif
  /* Calculate the FCS of the message */
  FCS = FindCRC(pMsg, MsgLength) ^ 0xFFFF;

  /* Start the frame with a flag character */
  *pFrame++ = FLAG;

  /* Copy the message to the frame with byte stuffing */
  for (pMsgLimit = pMsg + MsgLength; pMsg < pMsgLimit; pMsg++)
    pFrame = StuffByte(pFrame, *pMsg);

  /* Add the FCS to the frame with byte stuffing */
  pFrame = StuffByte(pFrame, FCS & 0xFF);
  pFrame = StuffByte(pFrame, FCS >> 8);

  /* End the frame with a flag character */
  *pFrame = FLAG;
#ifdef TRACE
  TraceDWord( 0xAAAAAAAA );
  for ( x=0; x<20; x++ )
    TraceByte( TxFrame[x] );
#endif
  /* Tell DMA where the end of the frame is */
  dmaControlBlock[TX_DMA_CHAN].SRCEND = pFrame;

  /* Enable DMA wake-up from LEUART0 TX */
  LEUART0->CTRL |= LEUART_CTRL_TXDMAWU;

  /* Start the DMA transfer using basic mode */
  DMA_ActivateBasic(TX_DMA_CHAN,                /* Activate channel selected */
                    true,                       /* Use primary descriptor */
                    false,                      /* No DMA burst */
                    NULL,                       /* Keep destination address */
                    NULL,                       /* Keep source address*/
                    pFrame - TxFrame);          /* Size of frame minus 1 */
}


/**************************************************************************//**
* @brief  DMA Callback function
*
* When the DMA transfer is completed, disables the DMA wake-up on TX in the
* LEUART to enable the DMA to sleep even when the LEUART buffer is empty.
*
******************************************************************************/
static void TxTransferDone(unsigned int channel, bool primary, void *user)
{
  (void) primary;
  (void) user;

  /* Disable DMA wake-up from LEUART0 TX */
  if (channel == TX_DMA_CHAN)
  {
    LEUART0->CTRL &= ~LEUART_CTRL_TXDMAWU;
  }
}


/**************************************************************************//**
 * @brief OnFrameReceived
 *****************************************************************************/

static void OnFrameReceived(uint8_t *pFrameLimit)
{
  uint16_t FCS;
  uint8_t  Msg[MAX_MSG_SIZE];
  uint8_t  *pMsg;
  uint8_t  *pFrame;
  uint8_t  *pEnd;
#ifdef TRACE
  int x;   /* Trace */

  TraceDWord( 0xBBBBBBBB );
  for ( x=0; x<20; x++ )
    TraceByte( RxFrame[x] );
#endif
  /* Find the start flag */
  for (pFrame = RxFrame; pFrame < pFrameLimit; pFrame++)
    if (*pFrame == FLAG)
      break;
  if (pFrame == pFrameLimit)
    return;

  /* Skip past the start flag */
  pFrame++;

  /* Find the end flag */
  for (pEnd = pFrame; pEnd < pFrameLimit; pEnd++)
    if (*pEnd == FLAG)
      break;
  if (pEnd == pFrameLimit)
    return;

  /* Remove byte stuffing and copy the message from the frame */
  for (pMsg = Msg; pFrame < pEnd; pMsg++)
  {
    if (*pFrame == ESCAPE)
    {
      pFrame++;
      *pMsg = *pFrame++ ^ 0x20;
    }
    else
      *pMsg = *pFrame++;
  }

  /* Verify the FCS */
  FCS = FindCRC(Msg, pMsg - Msg);
  if (FCS != 0xF0B8)
    return;

  BLE_OnMsgReceived(Msg);
}


/**************************************************************************//**
 * @brief LEUART IRQ handler
 *
 * When the signal frame is detected by the LEUART, this interrupt routine will
 * zero-terminate the char array, write the received string the to the LCD, and
 * reset the DMA for new data.
 *
 *****************************************************************************/
void LEUART0_IRQHandler(void)
{
  uint32_t Interrupts;
  uint32_t Length;

  /* Store and reset pending interupts */
  Interrupts = LEUART_IntGet(LEUART0);
  LEUART_IntClear(LEUART0, Interrupts);

  /* Signal frame found. */
  if (Interrupts & LEUART_IF_SIGF)
  {
    Length = MAX_FRAME_SIZE - 1 - ((dmaControlBlock[RX_DMA_CHAN].CTRL >> 4) & 0x3FF);

    if (Length >= MIN_FRAME_SIZE)
    {
      OnFrameReceived(&RxFrame[Length]);

      /* Reactivate DMA */
      DMA_ActivateBasic(RX_DMA_CHAN,         /* Activate DMA channel 0 */
                        true,                /* Activate using primary descriptor */
                        false,               /* No DMA burst */
                        NULL,                /* Keep source */
                        NULL,                /* Keep destination */
                        MAX_FRAME_SIZE - 1); /* Number of DMA transfer elements (minus 1) */
    }
  }
}


/**************************************************************************//**
 * @brief  Setup Low Energy UART with DMA operation
 *
 * The LEUART/DMA interaction is defined, and the DMA, channel and descriptor
 * is initialized. Then DMA starts to wait for and receive data, and the LEUART0
 * is set up to generate an interrupt when it receives the defined signal frame.
 * The signal frame is set to '\r', which is the "carriage return" symbol.
 *
 *****************************************************************************/

static void BLE_DMA_Init()
{
  /* Setting call-back function */
  cb[TX_DMA_CHAN].cbFunc  = TxTransferDone;
  cb[TX_DMA_CHAN].userPtr = NULL;

  /* Initializing DMA, channel and descriptor */
  DMA_Init(&dmaInit);
  DMA_CfgChannel(RX_DMA_CHAN, &rxChnlCfg);
  DMA_CfgChannel(TX_DMA_CHAN, &txChnlCfg);
  DMA_CfgDescr(RX_DMA_CHAN, true, &rxDescrCfg);
  DMA_CfgDescr(TX_DMA_CHAN, true, &txDescrCfg);

  /* Starting the RX transfer. Using Basic Mode */
  DMA_ActivateBasic(RX_DMA_CHAN,                /* Activate channel selected */
                    true,                       /* Use primary descriptor */
                    false,                      /* No DMA burst */
                    (void *) RxFrame,           /* Destination address */
                    (void *) &LEUART0->RXDATA,  /* Source address*/
                    MAX_FRAME_SIZE - 1);        /* Size of buffer minus1 */

  /* Set LEUART signal frame */
  LEUART0->SIGFRAME = FLAG;

  /* Enable LEUART Signal Frame Interrupt */
  LEUART_IntEnable(LEUART0, LEUART_IEN_SIGF);

  /* Set new DMA destination address directly in the DMA descriptor */
  dmaControlBlock[TX_DMA_CHAN].DSTEND = &LEUART0->TXDATA;

  /* Enable DMA Transfer Complete Interrupt */
  DMA->IEN = DMA_IEN_CH1DONE;

  /* Enable LEUART0 interrupt vector */
  NVIC_EnableIRQ(LEUART0_IRQn);

  /* Enable DMA interrupt vector */
  NVIC_EnableIRQ(DMA_IRQn);

  /* Make sure the LEUART wakes up the DMA on RX data */
  LEUART0->CTRL = LEUART_CTRL_RXDMAWU;
}


/**************************************************************************//**
 * @brief  Initialize LEUART 0
 *
 * Here the LEUART is initialized with the chosen settings.
 *
 *****************************************************************************/

void BLE_UART_Init()
{
  /* Start LFXO, and use LFXO for low-energy modules */
  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFRCO);
  CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFRCO);

  /* Enabling clocks, all other remain disabled */
  CMU_ClockEnable(cmuClock_CORELE, true);     /* Enable CORELE clock */
  CMU_ClockEnable(cmuClock_DMA, true);        /* Enable DMA clock */
  CMU_ClockEnable(cmuClock_LEUART0, true);    /* Enable LEUART0 clock */
  CMU_ClockEnable(cmuClock_GPIO, true);

  /* Reseting and initializing LEUART0 */
  LEUART_Reset(LEUART0);
  LEUART_Init(LEUART0, &leuart1Init);

  /* Route LEUART0 RX pin to DMA location 0 */
  LEUART0->ROUTE = LEUART_ROUTE_RXPEN |
                   LEUART_ROUTE_TXPEN |
                   LEUART_ROUTE_LOCATION_LOC0;

  /* Enable GPIO for LEUART0. RX is on D5 */
  GPIO_PinModeSet(RX_PORT,            /* Port */
                  RX_PIN,             /* Port number */
                  gpioModeInput,      /* Pin mode is set to input only */
                  1);

  /* Enable GPIO for LEUART0. TX is on D4 */
  GPIO_PinModeSet(TX_PORT,                  /* GPIO port */
                  TX_PIN,                   /* GPIO port number */
                  gpioModePushPull,         /* Pin mode is set to push pull */
                  1);                       /* High idle state */

  BLE_DMA_Init();

  BLE_Ready       = false;
  BLE_Initialized = false;
}


