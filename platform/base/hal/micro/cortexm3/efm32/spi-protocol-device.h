/** 
 * @file hal/micro/cortexm3/spi-protocol-device.h
 * @brief Internal SPI Protocol Device Header for USART and Pin Configuration
 *
 * <!-- Copyright 2015 by Silicon Laboratories. All rights reserved.     *80*-->
 */

#ifndef __SPI_PROTOCOL_DEVICE_H__
#define __SPI_PROTOCOL_DEVICE_H__

//The maximum SPI Protocol message size is 136 bytes. We define a buffer of
//142 specifically for error detection during the Response Section.  By using
//a buffer of 142, we can use the SCx_REG(TXCNT) register to monitor the state 
//of the transaction and know that if a DMA TX unload occurs we have an error.
#define SPIP_BUFFER_SIZE             142
#define SPIP_ERROR_RESPONSE_SIZE     2
#define MAX_PAYLOAD_FRAME_LENGTH     133
#define EZSP_LENGTH_INDEX            1
#define RX_DMA_BYTES_LEFT_THRESHOLD  4
// Timeout (ms) for SPI transfers
#ifndef SPI_NCP_TIMEOUT
#define SPI_NCP_TIMEOUT 0
#endif

#ifndef SPI_NCP_RX_BUFFERS
#define SPI_NCP_RX_BUFFERS 1
#endif

// SPI NCP USART selection
#ifndef SPI_NCP_USART_PORT
  #define SPI_NCP_USART_PORT 1
#endif

#ifndef SPI_NCP_USART1
// Configuration data for SPI NCP slave using USART1.
// Location 11 will output on EXP header on WSTK6001
// EXP    Function  Pin  Peripheral  Location  Description
// EXP4   SPI_MOSI  PC6  USART1_TX   11        Data input to NCP
// EXP6   SPI_MISO  PC7  USART1_RX   11        Data output from NCP
// EXP8   SPI_CLK   PC8  USART1_CLK  11        Clock input to NCP
// EXP10  SPI_CS    PC9  USART1_CS   11        Slave select input to NCP
#define SPI_NCP_USART1                                                    \
{                                                                         \
  USART1,                       /* USART port                       */    \
  _USART_ROUTELOC0_TXLOC_LOC11, /* USART Tx pin location number     */    \
  _USART_ROUTELOC0_RXLOC_LOC11, /* USART Rx pin location number     */    \
  _USART_ROUTELOC0_CLKLOC_LOC11,/* USART Clk pin location number    */    \
  _USART_ROUTELOC0_CSLOC_LOC11, /* USART Cs pin location number     */    \
  0,                            /* Bitrate                          */    \
  8,                            /* Frame length                     */    \
  0xFF,                         /* Dummy tx value for rx only funcs */    \
  spidrvSlave,                  /* SPI mode                         */    \
  spidrvBitOrderMsbFirst,       /* Bit order on bus                 */    \
  spidrvClockMode0,             /* SPI clock/phase mode             */    \
  spidrvCsControlAuto,          /* CS controlled by the driver      */    \
  spidrvSlaveStartImmediate     /* Slave start transfers immediately*/    \
}
#endif //SPI_NCP_USART

#ifndef SPI_BTL_USART_INIT
#define SPI_BTL_USART_INIT                                              \
{                                                                       \
  usartEnable,     /*enable*/                                           \
  0,               /*refFreq*/                                          \
  1000,            /*baudrate*/                                         \
  usartDatabits8,  /*databits*/                                         \
  false,           /*master:  false = slave mode*/                      \
  true,            /*msfb: true = msb first  false = lsb first*/        \
  usartClockMode0, /*clockMode: clock idle low, sample on rising edge*/ \
}
#endif

#if (SPI_NCP_USART_PORT == 1)
  #define SPI_NCP_USART USART1
  #define SPI_NCP_USART_IRQn USART1_RX_IRQn
  #define SPI_NCP_USART_IRQ_NAME USART1_RX_IRQHandler
  #define SPI_NCP_USART_INIT SPI_NCP_USART1
  #define SPI_NCP_USART_CLOCK cmuClock_USART1
#elif (SPI_NCP_USART_PORT == 2)
  #define SPI_NCP_USART USART2
  #define SPI_NCP_USART_IRQn USART2_RX_IRQn
  #define SPI_NCP_USART_IRQ_NAME USART2_RX_IRQHandler
  #define SPI_NCP_USART_INIT SPI_NCP_USART2
  #define SPI_NCP_USART_CLOCK cmuClock_USART2
#else
  #error EFR SPI NCP not enabled for ports other than USART1 and USART2.
#endif


// Default nHOST_INT to PA4 -> EXP7 on WSTK6001
#ifndef NHOST_INT_PORT
#define NHOST_INT_PORT gpioPortA
#endif

#ifndef NHOST_INT_PIN
#define NHOST_INT_PIN 4
#endif

// Default CS/nSSEL to PC9 -> EXP10 on WSTK6001
#ifndef NSSEL_PORT
#define NSSEL_PORT gpioPortC
#endif

#ifndef NSSEL_PIN
#define NSSEL_PIN 9
#endif

#ifndef DISABLE_NWAKE

// Default nWAKE_PORT to PB11 -> EXP9 on WSTK6001
#ifndef NWAKE_PORT
#define NWAKE_PORT gpioPortB
#endif

#ifndef NWAKE_PIN
#define NWAKE_PIN 11
#endif

static inline bool nWAKE_IS_NEGATED (void)
{
  return (GPIO_PinInGet(NWAKE_PORT, NWAKE_PIN) != 0);
}

static inline bool nWAKE_IS_ASSERTED (void)
{
  return (GPIO_PinInGet(NWAKE_PORT, NWAKE_PIN) == 0);
}
#else
#define nWAKE_IS_NEGATED() true
#define nWAKE_IS_ASSERTED() false
#endif

static inline bool nSSEL_IS_NEGATED (void)
{
  return (GPIO_PinInGet(NSSEL_PORT, NSSEL_PIN) != 0);
}

static inline bool nSSEL_IS_ASSERTED (void)
{
  return (GPIO_PinInGet(NSSEL_PORT, NSSEL_PIN) == 0);
}

static inline void SET_nHOST_INT(void)
{
  GPIO_PinOutSet(NHOST_INT_PORT, NHOST_INT_PIN);
}

static inline void CLR_nHOST_INT(void)
{
  GPIO_PinOutClear(NHOST_INT_PORT, NHOST_INT_PIN);
}

#endif // __SPI_PROTOCOL_DEVICE_H__

