/** @file hal/micro/cortexm3/uart.c
 *  @brief EM3XX UART Library.
 *
 * <!-- Copyright 2014 Silicon Laboratories, Inc.                        *80*-->
 */

#include PLATFORM_HEADER
#include "stack/include/ember.h"
#include "stack/include/error.h"
#include "stack/platform/micro/debug-channel.h"
#include "hal/hal.h"
#include "hal/micro/micro-types.h"

#if (! defined(EMBER_STACK_IP))
#include "stack/include/packet-buffer.h"
#endif

#include "serial/serial.h"

// Allow some code to be disabled (and flash saved) if
//  a port is unused or in low-level driver mode
// port 0 is VUART
#if (EMBER_SERIAL0_MODE == EMBER_SERIAL_UNUSED)
  #define EM_SERIAL0_ENABLED 0
  #define EM_SER0_PORT_EN(port) (false)
#else
  #define EM_SERIAL0_ENABLED 1
  #define EM_SER0_PORT_EN(port) ((port) == 0)
#endif
#if (EMBER_SERIAL0_MODE == EMBER_SERIAL_LOWLEVEL)
  #error Serial 0 (Virtual Uart) does not support LOWLEVEL mode
#endif

// port 1 is SC1
#if !defined(EMBER_MICRO_HAS_SC1) \
    || (EMBER_SERIAL1_MODE == EMBER_SERIAL_UNUSED) \
    || (EMBER_SERIAL1_MODE == EMBER_SERIAL_LOWLEVEL)
  #define EM_SERIAL1_ENABLED 0
  #define EM_SER1_PORT_EN(port)     (false)
  #define EM_SER1_PORT_FIFO(port)   (false)
  #define EM_SER1_PORT_BUFFER(port) (false)
#else
  #define EM_SERIAL1_ENABLED 1
  #define EM_SER1_PORT_EN(port) ((port) == 1)
  #if     (EMBER_SERIAL1_MODE == EMBER_SERIAL_FIFO)
    #define EM_SER1_PORT_FIFO(port)   EM_SER1_PORT_EN(port)
    #define EM_SER1_PORT_BUFFER(port) (false)
  #else//Must be EMBER_SERIAL_BUFFER
    #define EM_SER1_PORT_FIFO(port)   (false)
    #define EM_SER1_PORT_BUFFER(port) EM_SER1_PORT_EN(port)
  #endif
  #define EM_PHYSICAL_UART
#endif

// port 2 is SC3
#if !defined(EMBER_MICRO_HAS_SC3) \
    || (EMBER_SERIAL2_MODE == EMBER_SERIAL_UNUSED) \
    || (EMBER_SERIAL2_MODE == EMBER_SERIAL_LOWLEVEL)
  #define EM_SERIAL2_ENABLED 0
  #define EM_SER2_PORT_EN(port)     (false)
  #define EM_SER2_PORT_FIFO(port)   (false)
  #define EM_SER2_PORT_BUFFER(port) (false)
#else
  #define EM_SERIAL2_ENABLED 1
  #define EM_SER2_PORT_EN(port) ((port) == 2)
  #if     (EMBER_SERIAL2_MODE == EMBER_SERIAL_FIFO)
    #define EM_SER2_PORT_FIFO(port)   EM_SER2_PORT_EN(port)
    #define EM_SER2_PORT_BUFFER(port) (false)
  #else//Must be EMBER_SERIAL_BUFFER
    #define EM_SER2_PORT_FIFO(port)   (false)
    #define EM_SER2_PORT_BUFFER(port) EM_SER2_PORT_EN(port)
  #endif
  #define EM_PHYSICAL_UART
#endif

// port 3 is USB
#if !defined(CORTEXM3_EM35X_USB) \
    || (EMBER_SERIAL3_MODE == EMBER_SERIAL_UNUSED) \
  || defined (USB_MSD) \
  || defined (USB_HID)
  #define EM_SERIAL3_ENABLED 0
  #define EM_SER3_PORT_EN(port) (false)
  #ifdef USB_HID
    #include "hal/micro/cortexm3/usb/em_usb.h"
    #include "hal/micro/cortexm3/usb/hid/usbconfig.h"
    #include "hal/micro/cortexm3/usb/hid/descriptors.h"
  #endif
  #define EM_SER3_PORT_FIFO(port)   (false)
  #define EM_SER3_PORT_BUFFER(port) (false)
#else
  #define EM_SERIAL3_ENABLED 1
  #define EM_SER3_PORT_EN(port) ((port) == 3)
  #if     (EMBER_SERIAL3_MODE == EMBER_SERIAL_FIFO)
    #define EM_SER3_PORT_FIFO(port)   EM_SER3_PORT_EN(port)
    #define EM_SER3_PORT_BUFFER(port) (false)
  #else//Must be EMBER_SERIAL_BUFFER
    #define EM_SER3_PORT_FIFO(port)   (false)
    #define EM_SER3_PORT_BUFFER(port) EM_SER3_PORT_EN(port)
  #endif

  #include "hal/micro/cortexm3/usb/em_usb.h"
  #include "hal/micro/cortexm3/usb/cdc/usbconfig.h"
  #include "hal/micro/cortexm3/usb/cdc/descriptors.h"
#endif

#if defined(EM_ENABLE_SERIAL_FIFO) && defined(EM_ENABLE_SERIAL_BUFFER)
  #define EM_SER_MULTI(expr) (expr)
#else // None/Single/Same UARTs -- no need to runtime check their mode at all
  #define EM_SER_MULTI(expr) (true)
#endif

// TODO
#if EM_SERIAL1_ENABLED && (defined(EMBER_SERIAL1_RTSCTS) || defined(EMBER_SERIAL1_XONXOFF)) && EM_SERIAL2_ENABLED
  #error Flow control is not currently supported when using both physical UARTs
#endif

#define UARTERRORMARK_NEEDED ( 0 \
        || (EM_SERIAL0_ENABLED) \
        || (EM_SERIAL3_ENABLED) \
        || (defined(EM_PHYSICAL_UART) && defined(EM_ENABLE_SERIAL_FIFO)) \
        )

//State information for RX DMA Buffer operation
typedef struct EmSerialBufferState {
  const uint16_t fifoSize;
  const uint16_t rxStartIndexB;
  uint16_t prevCountA;
  uint16_t prevCountB;
  bool waitingForTailA;
  bool waitingForTailB;
  bool waitingForInputToB;
  EmberMessageBuffer holdBuf[2];
} EmSerialBufferState;

#if defined(EZSP_ASH) && \
    !defined(EMBER_SERIAL1_RTSCTS) && \
    !defined(EMBER_SERIAL1_XONXOFF)&& \
    !defined(EZSP_USB)
  #error EZSP-UART requires either RTS/CTS or XON/XOFF flow control!
#endif

#ifdef EMBER_SERIAL1_RTSCTS
  #if EMBER_SERIAL1_MODE != EMBER_SERIAL_BUFFER
  #error "Illegal serial port 1 configuration"
  #endif
#endif

#ifdef EMBER_SERIAL2_RTSCTS
  #if EMBER_SERIAL2_MODE != EMBER_SERIAL_BUFFER
  #error "Illegal serial port 2 configuration"
  #endif
#endif

#ifdef EMBER_SERIAL1_XONXOFF
  #if EMBER_SERIAL1_MODE != EMBER_SERIAL_FIFO
  #error "Illegal serial port 1 configuration"
  #endif

  static void halInternalUart1ForceXon(void); // forward declaration

  static int8_t xcmdCount;     // num XONs sent to host, written only by tx isr
                              //-1 means an XOFF was sent last
                              // 0 means ready to rx, but no XON has been sent
                              // n>0 means ready to rx, and n XONs have been sent
  static uint8_t xonXoffTxByte; // if non-zero, an XON or XOFF byte to send ahead
                              // of tx queue - cleared when byte is sent
  static uint8_t xonTimer;      // time when last data rx'ed from host, or when
                              // an XON was sent (in 1/4 ticks)

  #define ASCII_XON         0x11  // requests host to pause sending
  #define ASCII_XOFF        0x13  // requests host to resume sending
  #define XON_REFRESH_TIME  8     // delay between repeat XONs (1/4 sec units)
  #define XON_REFRESH_COUNT 3     // max number of repeat XONs to send after 1st

  // Define thresholds for XON/XOFF flow control in terms of queue used values
  // Take into account the 4 byte transmit FIFO
  #if (EMBER_SERIAL1_RX_QUEUE_SIZE == 128)
    #define XON_LIMIT       16    // send an XON
    #define XOFF_LIMIT      96    // send an XOFF
  #elif (EMBER_SERIAL1_RX_QUEUE_SIZE == 64)
    #define XON_LIMIT       8
    #define XOFF_LIMIT      36
  #elif (EMBER_SERIAL1_RX_QUEUE_SIZE == 32)
    #define XON_LIMIT       2
    #define XOFF_LIMIT      8
  #elif (EMBER_SERIAL1_RX_QUEUE_SIZE > 32)
    #define XON_LIMIT       (EMBER_SERIAL1_RX_QUEUE_SIZE/8)
    #define XOFF_LIMIT      (EMBER_SERIAL1_RX_QUEUE_SIZE*3/4)
  #else
    #error "Serial port 1 receive buffer too small!"
  #endif
#endif  // EMBER_SERIAL1_XONXOFF

#ifdef EMBER_SERIAL2_XONXOFF
  #error "XON/XOFF is not supported on port 2"
#endif

#ifdef EMBER_SERIAL3_XONXOFF
  #error "XON/XOFF is not supported on port 3"
#endif

#if defined(EMBER_SERIAL1_RTSCTS) || defined(EMBER_SERIAL2_RTSCTS)
  void halInternalUartRxCheckRts(uint8_t port);
#else
  #define halInternalUartRxCheckRts(x) do {} while(0)
#endif

#if defined(EMBER_SERIAL1_RTSCTS)
  // define this for backwards compatibility
  void halInternalUart1RxCheckRts( void )
  {
    halInternalUartRxCheckRts(1);
  }
#endif

// Save flash if ports are undefined
#if defined(EM_PHYSICAL_UART)

  const uint8_t baudSettings[] = {
    // This table is indexed by the supported BAUD_xxx enum from serial.h.
    // The actual baud rate is encoded in a byte and converted algorithmically
    // into the needed SCx register values based on system clock frequency
    // Here each byte is divided into two 4-bit nibbles 0x<mul><exp> where:
    // baud = <mul> * 100 * 2^<exp> when <exp> is <=10
    //  and = <mul> * 100 * 10^(<exp>-10) when <exp> is >10
    // This allows all supported baud rates (and many others) to be represented.
    0x30, //  0 - BAUD_300    =  3 * 100 * 2^0
    0x60, //  1 - BAUD_600    =  6 * 100 * 2^0
    0x90, //  2 - BAUD_900    =  9 * 100 * 2^0
    0xC0, //  3 - BAUD_1200   = 12 * 100 * 2^0
    0xC1, //  4 - BAUD_2400   = 12 * 100 * 2^1
    0xC2, //  5 - BAUD_4800   = 12 * 100 * 2^2
    0xC3, //  6 - BAUD_9600   = 12 * 100 * 2^3
    0x94, //  7 - BAUD_14400  =  9 * 100 * 2^4
    0xC4, //  8 - BAUD_19200  = 12 * 100 * 2^4
    0x95, //  9 - BAUD_28800  =  9 * 100 * 2^5
    0xC5, // 10 - BAUD_38400  = 12 * 100 * 2^5
    0x5C, // 11 - BAUD_50000  =  5 * 100 * 10^2
    0x96, // 12 - BAUD_57600  =  9 * 100 * 2^6
    0xC6, // 13 - BAUD_76800  = 12 * 100 * 2^6
    0xAC, // 14 - BAUD_100000 = 10 * 100 * 10^2
    0x97, // 15 - BAUD_115200 =  9 * 100 * 2^7
    0x98, // 16 - BAUD_230400 =  9 * 100 * 2^8
    0x99, // 17 - BAUD_460800 =  9 * 100 * 2^9
   #ifdef EMBER_SERIAL_BAUD_CUSTOM
    EMBER_SERIAL_BAUD_CUSTOM, //Hook for custom baud rate, see BOARD_HEADER
   #else
    0x9A, // 18 - BAUD_921600 =  9 * 100 * 2^10
   #endif
  };

#endif // defined(EM_PHYSICAL_UART)

#if EM_SERIAL1_ENABLED
  #if (EMBER_SERIAL1_MODE == EMBER_SERIAL_BUFFER)
  #endif//(EMBER_SERIAL1_MODE == EMBER_SERIAL_BUFFER)

  #if    SLEEPY_IP_MODEM_UART
    //This macro is used to manipulate TxD to avoid glitching it across sleep
    //which can lead to spurrious data or framing errors seen by peer
    #define SC1_TXD_GPIO(gpioCfg, state) do {                                   \
      GPIO_PBCFGL = (GPIO_PBCFGL & ~PB1_CFG_MASK) | ((gpioCfg) << PB1_CFG_BIT); \
      (state) ? (GPIO_PBSET = PB1) : (GPIO_PBCLR = PB1);                        \
    } while (0)
  #else//!SLEEPY_IP_MODEM_UART
    #define SC1_TXD_GPIO(gpioCfg, state) do { } while (0)
  #endif//SLEEPY_IP_MODEM_UART
#endif // EM_SERIAL1_ENABLED

// figure out how many buffer state structs we need
#if (EM_SERIAL1_ENABLED                                                        \
    && EMBER_SERIAL1_MODE == EMBER_SERIAL_BUFFER)                              \
    && (EM_SERIAL2_ENABLED                                                     \
    && EMBER_SERIAL2_MODE == EMBER_SERIAL_BUFFER)                              \
    && (EM_SERIAL3_ENABLED                                                     \
    && EMBER_SERIAL3_MODE == EMBER_SERIAL_BUFFER)
  static EmSerialBufferState serialBufferStates[] = {
    { EMBER_SERIAL1_RX_QUEUE_SIZE,
      (EMBER_SERIAL1_RX_QUEUE_SIZE/2),
      0,
      0,
      false,
      false,
      false,
      { EMBER_NULL_MESSAGE_BUFFER, EMBER_NULL_MESSAGE_BUFFER }
    },
    { EMBER_SERIAL2_RX_QUEUE_SIZE,
      (EMBER_SERIAL2_RX_QUEUE_SIZE/2),
      0,
      0,
      false,
      false,
      false,
      { EMBER_NULL_MESSAGE_BUFFER, EMBER_NULL_MESSAGE_BUFFER }
    },
    { EMBER_SERIAL3_RX_QUEUE_SIZE,
      (EMBER_SERIAL3_RX_QUEUE_SIZE/2),
      0,
      0,
      false,
      false,
      false,
      { EMBER_NULL_MESSAGE_BUFFER, EMBER_NULL_MESSAGE_BUFFER }
    }
  };

  #define BUFSTATE(port) (serialBufferStates + (port) - 1)
#elif (EM_SERIAL1_ENABLED                                          \
       && EMBER_SERIAL1_MODE == EMBER_SERIAL_BUFFER)
  static EmSerialBufferState serialBufferState = {
    EMBER_SERIAL1_RX_QUEUE_SIZE,
    (EMBER_SERIAL1_RX_QUEUE_SIZE/2),
    0,
    0,
    false,
    false,
    false,
    { EMBER_NULL_MESSAGE_BUFFER, EMBER_NULL_MESSAGE_BUFFER }
  };

  #define BUFSTATE(port) (&serialBufferState)
#elif (EM_SERIAL2_ENABLED                                          \
       && EMBER_SERIAL2_MODE == EMBER_SERIAL_BUFFER)
  static EmSerialBufferState serialBufferState = {
    EMBER_SERIAL2_RX_QUEUE_SIZE,
    (EMBER_SERIAL2_RX_QUEUE_SIZE/2),
    0,
    0,
    false,
    false,
    false,
    { EMBER_NULL_MESSAGE_BUFFER, EMBER_NULL_MESSAGE_BUFFER }
  };

  #define BUFSTATE(port) (&serialBufferState)
#elif (EM_SERIAL3_ENABLED                                          \
       && EMBER_SERIAL3_MODE == EMBER_SERIAL_BUFFER)
  static EmSerialBufferState serialBufferState = {
    EMBER_SERIAL3_RX_QUEUE_SIZE,
    (EMBER_SERIAL3_RX_QUEUE_SIZE/2),
    0,
    0,
    false,
    false,
    false,
    { EMBER_NULL_MESSAGE_BUFFER, EMBER_NULL_MESSAGE_BUFFER }
  };

  #define BUFSTATE(port) (&serialBufferState)
#endif

// prototypes
#if defined(EM_PHYSICAL_UART)
static void halInternalUartTxIsr(uint8_t port);
#endif
#if     UARTERRORMARK_NEEDED
static void uartErrorMark(uint8_t port, uint8_t errors);
#endif//UARTERRORMARK_NEEDED
#if defined(EM_ENABLE_SERIAL_BUFFER) && (EM_SERIAL1_ENABLED || EM_SERIAL2_ENABLED)
static void halInternalRestartUartDma(uint8_t port);
#endif


#ifdef EM_PHYSICAL_UART
static void halInternalInitUartInterrupts(uint8_t port)
{
  #if defined(EM_ENABLE_SERIAL_FIFO)
    if (EM_SER_MULTI(EM_SER1_PORT_FIFO(port) || EM_SER2_PORT_FIFO(port))) {
      // Make the RX Valid interrupt level sensitive (instead of edge)
//snip-
      // SC1_INTMODE = SC_SPIRXVALMODE;
      // Enable just RX interrupts; TX interrupts are controlled separately
      EVENT_SCxCFG(port) |= (EVENT_SC12_CFG_RXVAL  |
                             EVENT_SC12_CFG_RXOVF  |
                             EVENT_SC12_CFG_FRMERR |
                             EVENT_SC12_CFG_PARERR);
      EVENT_SCxFLAG(port) = 0xFFFF; // Clear any stale interrupts
      NVIC_EnableIRQ(SCx_IRQn(port));
    }
  #endif
  #if defined(EM_ENABLE_SERIAL_BUFFER) && (EM_SERIAL1_ENABLED || EM_SERIAL2_ENABLED)
    if (EM_SER_MULTI(EM_SER1_PORT_BUFFER(port) || EM_SER2_PORT_BUFFER(port))) {
      halInternalRestartUartDma(port);

      // don't do this for port 1 if it's being used for EZSP
      #ifdef EZSP_ASH
        if (port != 1) {
      #endif
          EVENT_SCxCFG(port) |= (EVENT_SC12_CFG_RXOVF  |
                                 EVENT_SC12_CFG_FRMERR |
                                 EVENT_SC12_CFG_PARERR);
      #ifdef EZSP_ASH
        }
      #endif

      // The receive side of buffer mode does not require any interrupts.
      // The transmit side of buffer mode requires interrupts, which
      // will be configured on demand in halInternalStartUartTx(), so just
      // enable the top level interrupt for the transmit side.
      EVENT_SCxFLAG(port) = 0xFFFF; // Clear any stale interrupts
      NVIC_EnableIRQ(SCx_IRQn(port));

      #ifdef EMBER_SERIAL1_RTSCTS
        // TODO refactor this into a variable that can be queried at runtime
        if (EM_SER1_PORT_EN(port)) {
          // Software-based RTS/CTS needs interrupts on DMA buffer unloading.
          EVENT_SCxCFG(port) |= (EVENT_SC12_CFG_RXULDA | EVENT_SC12_CFG_RXULDB);
          SCx_REG(port, UARTCFG) |= (SC_UARTCFG_UARTFLOW | SC_UARTCFG_UARTRTS);
        }
      #endif
      #ifdef EMBER_SERIAL2_RTSCTS
        if (EM_SER2_PORT_EN(port)) {
          // Software-based RTS/CTS needs interrupts on DMA buffer unloading.
          EVENT_SCxCFG(port) |= (EVENT_SC12_CFG_RXULDA | EVENT_SC12_CFG_RXULDB);
          SCx_REG(port, UARTCFG) |= (SC_UARTCFG_UARTFLOW | SC_UARTCFG_UARTRTS);
        }
      #endif
    }
  #endif //defined(EM_ENABLE_SERIAL_BUFFER) && (EM_SERIAL1_ENABLED || EM_SERIAL2_ENABLED)
}

// init function for physical UART
static EmberStatus halInternalInitPhysicalUart(uint8_t port,
                                               SerialBaudRate rate,
                                               SerialParity parity,
                                               uint8_t stopBits)
{
  uint32_t tempcfg;

  // set baud rate
  // If rate is one of the BAUD_ settings from serial.h then use
  // its baudSetting[] value from above, otherwise interpret it
  // as a custom baudSetting[] encoded value.
  if (rate < sizeof(baudSettings)/sizeof(*baudSettings)) {
    rate = baudSettings[rate];
  }
  // Convert encoded rate into baud by extracting the <mul> and <exp>
  // nibbles.  <mul> is always multiplied by 100.  For <exp> <= 10,
  // that result is multipled by 2^<exp>; for <exp> > 10 that result
  // is multipled by 10^(<exp>-10).
  tempcfg = (uint32_t)(rate >> 4) * 100; // multiplier
  rate &= 0x0F; // exponent
  if (rate <= 10) {
    tempcfg <<= rate;
  } else {
    while (rate-- > 10) {
      tempcfg *= 10;
    }
  }
  EmberStatus status = halInternalUartSetBaudRate(port, tempcfg);
  if (status != EMBER_SUCCESS) {
    return status;
  }

  // Default is always 8 data bits irrespective of parity setting,
  // according to Lee, but hack overloads high-order nibble of stopBits to
  // allow user to specify desired number of data bits:  7 or 8 (default).
  if (((stopBits & 0xF0) >> 4) == 7) {
    tempcfg = 0;
  } else {
    tempcfg = SC_UARTCFG_UART8BIT;
  }

  // parity bits
  if (parity == PARITY_ODD) {
    tempcfg |= SC_UARTCFG_UARTPAR | SC_UARTCFG_UARTODD;
  } else if( parity == PARITY_EVEN ) {
    tempcfg |= SC_UARTCFG_UARTPAR;
  }

  // stop bits
  if ((stopBits & 0x0F) >= 2) {
    tempcfg |= SC_UARTCFG_UART2STP;
  }

  // set all of the above into the config register
  SCx_REG(port, UARTCFG) = tempcfg;

  // put the peripheral into UART mode
  SCx_REG(port, MODE) = SC_MODE_MODE_UART;

  if (EM_SER1_PORT_EN(port)) { // port 1 special glitch-free case 
//snip- use CMSIS GPIO->P[] stuff
    SC1_TXD_GPIO(GPIOCFG_OUT_ALT, 1); // Can Assign TxD glitch-free to UART now
  }

  halInternalInitUartInterrupts(port);

  #ifdef EMBER_SERIAL1_XONXOFF
    if (EM_SER1_PORT_EN(port)) { // port 1 XON/XOFF special case
      halInternalUart1ForceXon();
    }
  #endif

  return EMBER_SUCCESS;
}
#endif // EM_PHYSICAL_UART

// initialize USB Virtual COM Port
#if EM_SERIAL3_ENABLED
static EmberStatus halInternalInitUsbVcp(void)
{
  #if defined(CORTEXM3_EM35X_USB)
    halResetWatchdog();

    tokTypeMfgEui64 tokEui64;
    halCommonGetMfgToken((void *)&tokEui64, TOKEN_MFG_EUI_64);

    uint8_t i = 0;
    uint8_t j = 0;
    for(j = 0; j<8; j++)
    {
      USBCDC_iSerialNumber.name[i++] = nibble2Ascii((tokEui64[j]>>4)&0xF);
      USBCDC_iSerialNumber.name[i++] = nibble2Ascii((tokEui64[j]>>0)&0xF);
    }

    USBD_Init(&USBCDC_initStruct);

    // USBD_Read(EP_OUT, receiveBuffer, 50, dataReceivedCallback);

    //It is necessary to wait for the COM port on the host to become
    //active before serial port3 can be used.
    uint16_t startTime = halCommonGetInt16uMillisecondTick();
    while(USBD_GetUsbState()!=USBD_STATE_CONFIGURED) {
      #ifdef BOOTLOADER
        halUsbIsr();
      #endif
      //Give ourselves a healthy 1 second for a COM port to open.
      if(elapsedTimeInt16u(startTime,
                           halCommonGetInt16uMillisecondTick()) > 1000) {
        return EMBER_SERIAL_INVALID_PORT;
      }
    }

    return EMBER_SUCCESS;
  #else
    return EMBER_SERIAL_INVALID_PORT;
  #endif
}
#endif

#if (EM_SERIAL0_ENABLED ||\
     EM_SERIAL1_ENABLED ||\
     EM_SERIAL2_ENABLED ||\
     EM_SERIAL3_ENABLED)
EmberStatus halInternalUartInit(uint8_t port,
                                SerialBaudRate rate,
                                SerialParity parity,
                                uint8_t stopBits)
{
  #if EM_SERIAL0_ENABLED
    if (EM_SER0_PORT_EN(port)) {
      // Initialize the debug channel
      return emDebugInit();
    }
  #endif

  #ifdef EM_PHYSICAL_UART
    if (EM_SER1_PORT_EN(port) || EM_SER2_PORT_EN(port)) {
      return halInternalInitPhysicalUart(port, rate, parity, stopBits);
    }
  #endif

  #if EM_SERIAL3_ENABLED && defined(CORTEXM3_EM35X_USB)
    if (EM_SER3_PORT_EN(port)) {
      return halInternalInitUsbVcp();
    }
  #endif

  return EMBER_SERIAL_INVALID_PORT;
}
#endif//(!defined(EM_SERIAL0_DISABLED) || !defined(EM_SERIAL1_DISABLED))

#if (EMBER_SERIAL3_MODE == EMBER_SERIAL_BUFFER)
void usbReleaseBuffer( USB_Status_TypeDef status, uint32_t xferred, uint32_t remaining )
{
  assert(status == USB_STATUS_OK);
  emberReleaseMessageBuffer(BUFSTATE(3)->holdBuf[0]);
  BUFSTATE(3)->holdBuf[0] = EMBER_NULL_MESSAGE_BUFFER;
}
#endif

void halInternalStartUartTx(uint8_t port)
{
  #if EM_SERIAL0_ENABLED
    if (EM_SER0_PORT_EN(port)) {
      #if EMBER_SERIAL0_MODE == EMBER_SERIAL_FIFO
        EmSerialFifoQueue *q = (EmSerialFifoQueue *)emSerialTxQueues[0];
        assert(q->tail == 0);
        emDebugSendVuartMessage(q->fifo, q->used);
        q->used = 0;
        q->head = 0;
        return;
      #elif EMBER_SERIAL0_MODE == EMBER_SERIAL_BUFFER
        EmSerialBufferQueue *q = (EmSerialBufferQueue *)emSerialTxQueues[0];
        assert(q->nextByte == NULL);
        emSerialBufferNextMessageIsr(q);
        while (q->nextByte != NULL) {
          emDebugSendVuartMessage(q->nextByte, (q->lastByte-q->nextByte)+1);
          emSerialBufferNextBlockIsr(q,0);
        }
        return;
      #endif
    }
  #endif//!defined(EM_SERIAL0_DISABLED)

  #if defined(EM_PHYSICAL_UART)
    // If the port is configured, go ahead and start transmit
    #if defined(EM_ENABLE_SERIAL_FIFO)
      if ((EM_SER1_PORT_FIFO(port) || EM_SER2_PORT_FIFO(port)) &&
          (SCx_REG(port, MODE) == SC_MODE_MODE_UART)) {
        // Ensure UART TX interrupts are enabled,
        // and call the ISR to send any pending output
        ATOMIC(
          // Enable TX interrupts
          EVENT_SCxCFG(port) |= (EVENT_SC12_CFG_TXFREE | EVENT_SC12_CFG_TXIDLE);
          // Pretend we got a tx interrupt
          halInternalUartTxIsr(port);
        )
        return;
      }
    #endif // defined(EM_ENABLE_SERIAL_FIFO)
    #if defined(EM_ENABLE_SERIAL_BUFFER) && (EM_SERIAL1_ENABLED || EM_SERIAL2_ENABLED)
      if ((EM_SER1_PORT_BUFFER(port) || EM_SER2_PORT_BUFFER(port)) &&
          (SCx_REG(port, MODE) == SC_MODE_MODE_UART)) {
        // Ensure UART TX interrupts are enabled,
        // and call the ISR to send any pending output
        ATOMIC(
          EVENT_SCxCFG(port) |= (EVENT_SC12_CFG_TXULDA | EVENT_SC12_CFG_TXULDB | EVENT_SC12_CFG_TXIDLE);
          // Pretend we got a tx interrupt
          halInternalUartTxIsr(port);
        )
        return;
      }
    #endif // defined(EM_ENABLE_SERIAL_BUFFER) && (EM_SERIAL1_ENABLED || EM_SERIAL2_ENABLED)
  #endif // EM_PHYSICAL_UART

  #if EM_SERIAL3_ENABLED
    if (EM_SER3_PORT_EN(port)) {
      #if defined(CORTEXM3_EM35X_USB)
        #if (EMBER_SERIAL3_MODE == EMBER_SERIAL_FIFO)
        usbTxData();
        #else //Must be EMBER_SERIAL_BUFFER
          EmSerialBufferQueue *q = (EmSerialBufferQueue *)emSerialTxQueues[3];
          assert( !((q->used == 0) && (q->nextByte != NULL)) );
          while ( q->used > 0 ) {
            if ( q->nextByte == NULL ) {
              // new message pending, but nextByte not set up yet
              emSerialBufferNextMessageIsr(q);
            }
            if (!USBD_EpIsBusy(CDC_EP_IN))
            {
              USBD_Write( CDC_EP_IN, q->nextByte, (uint32_t)q->lastByte - (uint32_t)q->nextByte + 1, (USB_XferCompleteCb_TypeDef) &usbReleaseBuffer );

              if (BUFSTATE(3)->holdBuf[0] != EMBER_NULL_MESSAGE_BUFFER)
                emberReleaseMessageBuffer(BUFSTATE(3)->holdBuf[0]);
              BUFSTATE(3)->holdBuf[0] = q->currentBuffer;
              emberHoldMessageBuffer(BUFSTATE(3)->holdBuf[0]);
              emSerialBufferNextBlockIsr(q, 3);
            }
            else
            {
              break;
            }
          } // while ( q->used > 0 )
        #endif
        return;
      #endif
    }
  #endif // EM_SERIAL3_ENABLED && EMBER_SERIAL3_MODE == EMBER_SERIAL_FIFO
}

void halInternalStopUartTx(uint8_t port)
{
  // Nothing for port 0 (virtual uart)

  #if defined(EM_PHYSICAL_UART)
    #if defined(EM_ENABLE_SERIAL_FIFO)
      if (EM_SER1_PORT_FIFO(port) || EM_SER2_PORT_FIFO(port)) {
        // Disable TX Interrupts
        EVENT_SCxCFG(port) &= ~(EVENT_SC12_CFG_TXFREE | EVENT_SC12_CFG_TXIDLE);
      }
    #endif // defined(EM_ENABLE_SERIAL_FIFO)
    #if defined(EM_ENABLE_SERIAL_BUFFER) && (EM_SERIAL1_ENABLED || EM_SERIAL2_ENABLED)
      if (EM_SER1_PORT_BUFFER(port) || EM_SER2_PORT_BUFFER(port)) {
        // Ensure DMA operations are complete before shutting off interrupts,
        // otherwise we might miss an important interrupt and cause a
        // packet buffer leak, e.g.
        while (SCx_REG(port, DMACTRL) & (SC_DMACTRL_TXLODA | SC_DMACTRL_TXLODB)) {}
        while ( !(SCx_REG(port, UARTSTAT) & SC_UARTSTAT_UARTTXIDLE) ) {}
        // Disable TX Interrupts
        EVENT_SCxCFG(port) &= ~(EVENT_SC12_CFG_TXULDA | EVENT_SC12_CFG_TXULDB | EVENT_SC12_CFG_TXIDLE);
      }
    #endif // defined(EM_ENABLE_SERIAL_BUFFER) && (EM_SERIAL1_ENABLED || EM_SERIAL2_ENABLED)
  #endif // defined(EM_PHYSICAL_UART)
}


//full blocking, no queue overflow issues, can be used in or out of int context
//does not return until character is transmitted.
EmberStatus halInternalForceWriteUartData(uint8_t port, uint8_t *data, uint8_t length)
{
  #if EM_SERIAL0_ENABLED
    if (EM_SER0_PORT_EN(port)) {
      emDebugSendVuartMessage(data, length);
      return EMBER_SUCCESS;
    }
  #endif

  #if defined(EM_PHYSICAL_UART)
    //if the port is configured, go ahead and transmit
    if ((EM_SER1_PORT_EN(port) || EM_SER2_PORT_EN(port)) &&
        (SCx_REG(port, MODE) == SC_MODE_MODE_UART)) {
      while (length--) {
        //spin until data register has room for more data
        while (!(SCx_REG(port, UARTSTAT) & SC_UARTSTAT_UARTTXFREE)) {}
        SCx_REG(port, DATA) = *data;
        data++;
      }

      //spin until TX complete (TX is idle)
      while (!(SCx_REG(port, UARTSTAT) & SC_UARTSTAT_UARTTXIDLE)) {}

      return EMBER_SUCCESS;
    }
  #endif // defined(EM_PHYSICAL_UART)

  #if EM_SERIAL3_ENABLED
    if (EM_SER3_PORT_EN(port)) {
      #if defined(CORTEXM3_EM35X_USB)
        //This function will block until done sending all the data.
        usbForceTxData(data, length);
        return EMBER_SUCCESS;
      #endif
    }
  #endif // EM_SERIAL3_ENABLED

  return EMBER_SERIAL_INVALID_PORT;
}

// Useful for waiting on serial port characters before interrupts have been
// turned on.
EmberStatus halInternalForceReadUartByte(uint8_t port, uint8_t* dataByte)
{
  EmberStatus err = EMBER_SUCCESS;

  #if EM_SERIAL0_ENABLED
    if (EM_SER0_PORT_EN(port)) {
      EmSerialFifoQueue *q = emSerialRxQueues[0];
      ATOMIC(
        if (q->used == 0) {
          WAKE_CORE = WAKE_CORE_FIELD;
        }
        if (q->used > 0) {
          *dataByte = FIFO_DEQUEUE(q, emSerialRxQueueWraps[0]);
        } else {
          err = EMBER_SERIAL_RX_EMPTY;
        }
      )
    }
  #endif // EM_SERIAL0_ENABLED

  #if defined(EM_PHYSICAL_UART)
    #if defined(EM_ENABLE_SERIAL_FIFO)
      if (EM_SER1_PORT_FIFO(port) || EM_SER2_PORT_FIFO(port)) {
        if (SCx_REG(port, UARTSTAT) & SC_UARTRXVAL) {
          *dataByte = (uint8_t) SCx_REG(port, DATA);
        } else {
          err = EMBER_SERIAL_RX_EMPTY;
        }
      }
    #endif
    #if defined(EM_ENABLE_SERIAL_BUFFER) && (EM_SERIAL1_ENABLED || EM_SERIAL2_ENABLED)
      if (EM_SER1_PORT_BUFFER(port) || EM_SER2_PORT_BUFFER(port)) {
        //When in buffer mode, the DMA channel is active and the RXVALID bit (as
        //used above in FIFO mode) will never get set.  To maintain the DMA/Buffer
        //model of operation, we need to break the conceptual model in this function
        //and make a function call upwards away from the hardware.  The ReadByte
        //function calls back down into halInternalUartRxPump and forces the
        //sequencing of the serial queues and the DMA buffer, resulting in a forced
        //read byte being returned if it is there.
        if (emberSerialReadByte(port, dataByte) != EMBER_SUCCESS) {
          err = EMBER_SERIAL_RX_EMPTY;
        }
      }
    #endif // defined(EM_ENABLE_SERIAL_BUFFER) && (EM_SERIAL1_ENABLED || EM_SERIAL2_ENABLED)
  #endif // defined(EM_PHYSICAL_UART)

  return err;
}

// blocks until the text actually goes out
void halInternalWaitUartTxComplete(uint8_t port)
{
  halResetWatchdog();

  // Nothing to do for port 0 (virtual uart)

  #if defined(EM_PHYSICAL_UART)
  if (EM_SER1_PORT_EN(port) || EM_SER2_PORT_EN(port)) {
      while ( !(SCx_REG(port, UARTSTAT) & SC_UARTSTAT_UARTTXIDLE) ) {}
      return;
    }
  #endif // defined(EM_PHYSICAL_UART)
}

// Debug Channel calls this ISR to push up data it has received
void halStackReceiveVuartMessage(uint8_t *data, uint8_t length)
{
  #if EM_SERIAL0_ENABLED
    EmSerialFifoQueue *q = emSerialRxQueues[0];

    while (length--) {
      //Use (emSerialRxQueueSizes - 1) so that the FIFO never completely fills
      //and the head never wraps around to the tail
      if ((q->used < (emSerialRxQueueSizes[0] - 1))) {
        FIFO_ENQUEUE(q,*data++,emSerialRxQueueWraps[0]);
      } else {
        uartErrorMark(0, EMBER_SERIAL_RX_OVERFLOW);
        return;  // no sense in trying to enqueue the rest
      }
    }
  #endif // EM_SERIAL0_ENABLED
}

#if defined(EM_ENABLE_SERIAL_BUFFER) && (EM_SERIAL1_ENABLED || EM_SERIAL2_ENABLED)
static void halInternalRestartUartDma(uint8_t port)
{
  //Reset the DMA software and restart it.
  EmSerialFifoQueue *q = emSerialRxQueues[port];
  uint32_t startAddress = (uint32_t)q->fifo;
  uint8_t head;
  uint8_t tail;
  uint8_t loadA = 0;
  uint8_t loadB = 0;
  BUFSTATE(port)->prevCountA = 0;
  BUFSTATE(port)->prevCountB = 0;
  BUFSTATE(port)->waitingForTailA = false;
  BUFSTATE(port)->waitingForTailB = false;
  BUFSTATE(port)->waitingForInputToB = false;
  //reload all defaults addresses - they will be adjusted below if needed
  SCx_REG(port, DMACTRL) = SC_DMACTRL_RXDMARST;
  SCx_REG(port, RXBEGA) =  startAddress;
  SCx_REG(port, RXENDA) = (startAddress + BUFSTATE(port)->fifoSize/2 - 1);
  SCx_REG(port, RXBEGB) =  (startAddress + BUFSTATE(port)->fifoSize/2);
  SCx_REG(port, RXENDB) = (startAddress + BUFSTATE(port)->fifoSize - 1);

  //adjust buffer addresses as needed and reload available buffers
  if ( q->used != BUFSTATE(port)->fifoSize ) {
    //we can only reload if the FIFO isn't full!
    //the FIFO is not empty or full, figure out what to do:
    //at this point we know we always have to adjust ST_ADDR to the head
    //we need to know which buffer the head is in, and always load that buff
    if ((q->head) < BUFSTATE(port)->rxStartIndexB) {
      SCx_REG(port, RXBEGA) = startAddress + (q->head);
      loadA++;
    } else {
      SCx_REG(port, RXBEGB) = startAddress + (q->head);
      loadB++;
    }
    //check to see if the head and the tail are not in the same buffer
    if((q->tail)/(BUFSTATE(port)->rxStartIndexB)) {
      tail = true;  //Tail in B buffer
    } else {
      tail = false; //Tail in A buffer
    }

    if((q->head)/(BUFSTATE(port)->rxStartIndexB)) {
      head = true;  //Head in B buffer
    } else {
      head = false; //Head in A buffer
    }

    if ( tail != head ) {
      //the head and the tail are in different buffers
      //we need to flag the buffer the tail is in so the Pump function does
      //not try to reenable it until it has been drained like normal.
      if ((q->tail)<BUFSTATE(port)->rxStartIndexB) {
        BUFSTATE(port)->waitingForTailA = true;
      } else {
        BUFSTATE(port)->waitingForTailB = true;
      }
    } else {
      //the head and the tail are in the same buffers
      if (q->used <= BUFSTATE(port)->rxStartIndexB) {
        //The serial FIFO is less no more than half full!
        if (!loadB) {
          //the head is in B, and we're capable of loading A
          //BUT: we can't activate A because the DMA defaults to A first,
          //  and it needs to start using B first to fill from the head
          //  SO, only load A if B hasn't been marked yet for loading.
          loadA++;
        } else {
          //B is loaded and waiting for data, A is being supressed until
          //B receives at least one byte so A doesn't prematurely load and
          //steal bytes meant for B first.
          BUFSTATE(port)->waitingForTailA = true;
          BUFSTATE(port)->waitingForInputToB = true;
        }
        //We can always loadB at this point thanks to our waiting* flags.
        loadB++;
      } else {
        //The serial FIFO is more than half full!
        //Since this case requires moving an end address of a buffer, which
        //severely breaks DMA'ing into a FIFO, we cannot do anything.
        //Doing nothing is ok because we are more than half full anyways,
        //and under normal operation we would only load a buffer when our
        //used count is less than half full.
        //Configure so the Pump function takes over when the serial FIFO drains
        SCx_REG(port, RXBEGA) =  startAddress;
        SCx_REG(port, RXBEGB) =  (startAddress + BUFSTATE(port)->fifoSize/2);
        loadA = 0;
        loadB = 0;
        BUFSTATE(port)->waitingForTailA = true;
        BUFSTATE(port)->waitingForTailB = true;
      }
    }

    //Address are set, flags are set, DMA is ready, so now we load buffers
    if (loadA) {
      SCx_REG(port, DMACTRL) = SC_DMACTRL_RXLODA;
    }
    if (loadB) {
      SCx_REG(port, DMACTRL) = SC_DMACTRL_RXLODB;
    }
  } else {
    //we're full!!  doh!  have to wait for the FIFO to drain
    BUFSTATE(port)->waitingForTailA = true;
    BUFSTATE(port)->waitingForTailB = true;
  }
}
#endif // defined(EM_ENABLE_SERIAL_BUFFER)

#ifdef EM_PHYSICAL_UART
void halInternalUartRxIsr(uint8_t port, uint16_t causes)
{
  #if defined(EM_ENABLE_SERIAL_FIFO)
    if (EM_SER_MULTI(EM_SER1_PORT_FIFO(port) || EM_SER2_PORT_FIFO(port))) {
      EmSerialFifoQueue *q = emSerialRxQueues[port];

      // At present we really don't care which interrupt(s)
      // occurred, just that one did.  Loop reading RXVALID
      // data (loop is necessary for bursty data otherwise
      // we could leave with RXVALID and not get another
      // RXVALID interrupt), processing any errors noted
      // along the way.
      while ( SCx_REG(port, UARTSTAT) & SC_UARTRXVAL ) {
        uint8_t errors = SCx_REG(port, UARTSTAT) & (SC_UARTFRMERR |
                                       SC_UARTRXOVF  |
                                       SC_UARTPARERR );
        uint8_t incoming = (uint8_t) SCx_REG(port, DATA);
#if     (CORTEXM3_EM35X_GEN3 && defined(ENABLE_OSC24M) && (ENABLE_OSC24M != 24) \
         && (ENABLE_OSC24M != 24000) && (ENABLE_OSC24M != 24000000))
        // Hack workaround for EMLIPARI-101 UART HW bug on non-24 MHz SysClk
        static bool uartFixTried = false;
        if ( (errors & SC_UARTFRMERR) && (incoming != 0 /* a BRK */) ) {
          if (!uartFixTried) {
            // Try to correct for this HW bug by inverting fraction bit
            SCx_REG(port, UARTFRAC) = !SCx_REG(port, UARTFRAC);
            uartFixTried = true;
          }
          incoming >>= 1;
          errors &= ~SC_UARTFRMERR;
        }
#endif//(CORTEXM3_EM35X_GEN3 && defined(ENABLE_OSC24M) && ...)

        if ( (errors == 0) && (q->used < (emSerialRxQueueSizes[port]-1)) ) {
#ifdef EMBER_SERIAL1_XONXOFF
          if (EM_SER1_PORT_FIFO(port)) {
            // Discard any XON or XOFF bytes received
            if ( (incoming != ASCII_XON) && (incoming != ASCII_XOFF) ) {
              FIFO_ENQUEUE(q, incoming, emSerialRxQueueWraps[port]);
            }
          } else {
            FIFO_ENQUEUE(q, incoming, emSerialRxQueueWraps[port]);
          }
#else
          FIFO_ENQUEUE(q, incoming, emSerialRxQueueWraps[port]);
#endif
        } else {
          // Translate error code
          if ( errors == 0 ) {
            errors = EMBER_SERIAL_RX_OVERFLOW;
            HANDLE_ASH_ERROR(EMBER_COUNTER_ASH_OVERFLOW_ERROR);
          } else if ( errors & SC_UARTSTAT_UARTRXOVF ) {
            errors = EMBER_SERIAL_RX_OVERRUN_ERROR;
            HANDLE_ASH_ERROR(EMBER_COUNTER_ASH_OVERRUN_ERROR);
          } else if ( errors & SC_UARTSTAT_UARTFRMERR ) {
            errors = EMBER_SERIAL_RX_FRAME_ERROR;
            HANDLE_ASH_ERROR(EMBER_COUNTER_ASH_FRAMING_ERROR);
          } else if ( errors & SC_UARTSTAT_UARTPARERR ) {
            errors = EMBER_SERIAL_RX_PARITY_ERROR;
          } else { // unknown
            errors = EMBER_ERR_FATAL;
          }
          uartErrorMark(port, errors);
        }
#ifdef EMBER_SERIAL1_XONXOFF
        if (EM_SER1_PORT_FIFO(port) &&
            (q->used >= XOFF_LIMIT) && (xcmdCount >= 0))  {
          xonXoffTxByte = ASCII_XOFF;
          halInternalStartUartTx(1);
        }
#endif
      } // end of while ( SC1_UARTSTAT & SC1_UARTRXVAL )
    }
  #endif

  #if defined(EM_ENABLE_SERIAL_BUFFER) && (EM_SERIAL1_ENABLED || EM_SERIAL2_ENABLED)
    if (EM_SER_MULTI(EM_SER1_PORT_BUFFER(port) || EM_SER2_PORT_BUFFER(port))) {
    #ifdef EMBER_SERIAL1_RTSCTS
      // TODO this flow control will fail if port 2 is active
      // If RTS is controlled by sw, this ISR is called when a buffer unloads.
      if (causes & (EVENT_SC12_FLAG_RXULDA | EVENT_SC12_FLAG_RXULDB)) {
        // Deassert RTS if the rx queue tail is not in an active DMA buffer:
        // if it is, then there's at least one empty DMA buffer
        if ( !( (emSerialRxQueues[port]->tail < emSerialRxQueueSizes[port]/2) &&
               (SCx_REG(port, DMASTAT) & SC_DMASTAT_RXACTA) ) &&
             !( (emSerialRxQueues[port]->tail >= emSerialRxQueueSizes[port]/2)
                && (SCx_REG(port, DMASTAT) & SC_DMASTAT_RXACTB) ) ) {
          SCx_REG(port, UARTCFG) &= ~SC_UARTCFG_UARTRTS;        // deassert RTS
        }
      #ifdef EZSP_ASH
        if ( ( (causes & EVENT_SC12_FLAG_RXULDA) && (SC1_DMASTAT & SC_DMASTAT_RXOVFA) ) ||
             ( (causes & EVENT_SC12_FLAG_RXULDB) && (SC1_DMASTAT & SC_DMASTAT_RXOVFB) ) ) {
          HANDLE_ASH_ERROR(EMBER_COUNTER_ASH_OVERFLOW_ERROR);
        }
        if ( ( (causes & EVENT_SC12_FLAG_RXULDA) && (SC1_DMASTAT & SC_DMASTAT_RXFRMA) ) ||
             ( (causes & EVENT_SC12_FLAG_RXULDB) && (SC1_DMASTAT & SC_DMASTAT_RXFRMB) ) ) {
          HANDLE_ASH_ERROR(EMBER_COUNTER_ASH_FRAMING_ERROR);
        }
      #else//!EZSP_ASH
        causes &= ~(EVENT_SC12_FLAG_RXULDA | EVENT_SC12_FLAG_RXULDB);
        if (causes == 0) { // if no errors in addition, all done
          return;
        }
      #endif//EZSP_ASH
      }
    #endif  //#ifdef EMBER_SERIAL1_RTSCTS
    #ifndef EZSP_ASH
    //Load all of the hardware status, then immediately reset so we can process
    //what happened without worrying about new data changing these values.
    //We're in an error condition anyways, so it is ok to have the DMA disabled
    //for a while (less than 80us, while 4 bytes @ 115.2kbps is 350us)
    {
      EmSerialFifoQueue *q = emSerialRxQueues[port];
      uint16_t status  = SCx_REG(port, DMASTAT);
      uint16_t errCntA = SCx_REG(port, RXERRA);
      uint16_t errCntB = SCx_REG(port, RXERRB);
      uint32_t errorIdx = emSerialRxQueueSizes[port]*2;
      uint32_t tempIdx;
      uint32_t startAddress = (uint32_t)q->fifo;

      //interrupts acknowledged at the start of the master SC1 ISR
      uint16_t intSrc  = causes;
      uint8_t errorType = EMBER_SUCCESS;

      SCx_REG(port, DMACTRL) = SC_DMACTRL_RXDMARST;  //to clear error
      //state fully captured, DMA reset, now we process error and restart

      if ( intSrc & EVENT_SC12_FLAG_RXOVF ) {
        //Read the data register four times to clear
        //the RXOVERRUN condition and empty the FIFO, giving us 4 bytes
        //worth of time (from this point) to reenable the DMA.
        (void) SCx_REG(port, DATA);
        (void) SCx_REG(port, DATA);
        (void) SCx_REG(port, DATA);
        (void) SCx_REG(port, DATA);

        if ( status & ( SC_DMASTAT_RXFRMA
                     | SC_DMASTAT_RXFRMB
                     | SC_DMASTAT_RXPARA
                     | SC_DMASTAT_RXPARB ) ) {
          //We just emptied hardware FIFO so the overrun condition is cleared.
          //Byte errors require special handling to roll back the serial FIFO.
          goto dealWithByteError;
        }

      //record the error type
      emSerialRxError[port] = EMBER_SERIAL_RX_OVERRUN_ERROR;

      //check for a retriggering of the Rx overflow, don't advance FIFO if so
      if ( !(BUFSTATE(port)->waitingForTailA && BUFSTATE(port)->waitingForTailB) ) {
        //first, move head to end of buffer head is in
        //second, move head to end of other buffer if tail is not in other buffer
        if ((q->head)<BUFSTATE(port)->rxStartIndexB) {
          //head inside A
          q->used += (BUFSTATE(port)->rxStartIndexB - q->head);
          q->head = (BUFSTATE(port)->rxStartIndexB);
          if ((q->tail)<BUFSTATE(port)->rxStartIndexB) {
            //tail not inside of B
            q->used += BUFSTATE(port)->rxStartIndexB;
            q->head = 0;
          }
        } else {
          //head inside B
          q->used += (BUFSTATE(port)->fifoSize - q->head);
          q->head = 0;
          if ((q->tail)>=BUFSTATE(port)->rxStartIndexB) {
            //tail is not inside of A
            q->used += BUFSTATE(port)->rxStartIndexB;
            q->head = BUFSTATE(port)->rxStartIndexB;
          }
        }
      }

      //Record the error position in the serial FIFO
      if (q->used != BUFSTATE(port)->fifoSize) {
        //mark the byte at q->head as the error
        emSerialRxErrorIndex[port] = q->head;
      } else {
        //Since the FIFO is full, the error index needs special handling
        //so there is no conflict between the head and tail looking at the same
        //index which needs to be marked as an error.
        emSerialRxErrorIndex[port] = RX_FIFO_FULL;
      }

      //By now the error is accounted for and the DMA hardware is reset.
      //By definition, the overrun error means we have no room left, therefore
      //we can't reenable the DMA.  Reset the previous counter states, and set
      //the waitingForTail flags to true - this tells the Pump function we have
      //data to process.  The Pump function will reenable the buffers as they
      //become available, just like normal.
      BUFSTATE(port)->prevCountA = 0;
      BUFSTATE(port)->prevCountB = 0;
      BUFSTATE(port)->waitingForInputToB = false;
      BUFSTATE(port)->waitingForTailA = true;
      BUFSTATE(port)->waitingForTailB = true;
      //from this point we fall through to the end of the Isr and return.

      } else {
      dealWithByteError:
        //We have a byte error to deal with and possibly more than one byte error,
        //of different types in different DMA buffers, so check each error flag.
        //All four error checks translate the DMA buffer's error position to their
        //position in the serial FIFO, and compares the error locations to find
        //the first error to occur after the head of the FIFO.  This error is the
        //error condition that is stored and operated on.
        if ( status & SC_DMASTAT_RXFRMA ) {
          tempIdx = errCntA;
          if (tempIdx < q->head) {
            tempIdx += BUFSTATE(port)->fifoSize;
          }
          if (tempIdx<errorIdx) {
            errorIdx = tempIdx;
          }
          errorType = EMBER_SERIAL_RX_FRAME_ERROR;
        }
        if ( status & SC_DMASTAT_RXFRMB ) {
          tempIdx = (errCntB + SCx_REG(port, RXBEGB)) - startAddress;
          if (tempIdx < q->head) {
            tempIdx += BUFSTATE(port)->fifoSize;
          }
          if (tempIdx<errorIdx) {
            errorIdx = tempIdx;
          }
          errorType = EMBER_SERIAL_RX_FRAME_ERROR;
        }
        if ( status & SC_DMASTAT_RXPARA ) {
          tempIdx = errCntA;
          if (tempIdx < q->head) {
            tempIdx += BUFSTATE(port)->fifoSize;
          }
          if (tempIdx<errorIdx) {
            errorIdx = tempIdx;
          }
          errorType = EMBER_SERIAL_RX_PARITY_ERROR;
        }
        if ( status & SC_DMASTAT_RXPARB ) {
          tempIdx = (errCntB + SCx_REG(port, RXBEGB)) - startAddress;
          if (tempIdx < q->head) {
            tempIdx += BUFSTATE(port)->fifoSize;
          }
          if (tempIdx<errorIdx) {
            errorIdx = tempIdx;
          }
          errorType = EMBER_SERIAL_RX_PARITY_ERROR;
        }

        //We now know the type and location of the first error.
        //Move up to the error location and increase the used count.
        q->head = (errorIdx % BUFSTATE(port)->fifoSize);
        if (q->head < q->tail) {
          q->used = ((q->head + BUFSTATE(port)->fifoSize) - q->tail);
        } else {
          q->used = (q->head - q->tail);
        }

        //Mark the byte at q->head as the error
        emSerialRxError[port] = errorType;
        if (q->used != BUFSTATE(port)->fifoSize) {
          //mark the byte at q->head as the error
          emSerialRxErrorIndex[port] = q->head;
        } else {
          //Since the FIFO is full, the error index needs special handling
          //so there is no conflict between the head and tail looking at the same
          //index which needs to be marked as an error.
          emSerialRxErrorIndex[port] = RX_FIFO_FULL;
        }

        //By now the error is accounted for and the DMA hardware is reset.
        halInternalRestartUartDma(port);
      }
    }
    #endif // #ifndef EZSP_ASH
    }
  #endif // defined(EM_ENABLE_SERIAL_BUFFER) && (EM_SERIAL1_ENABLED || EM_SERIAL2_ENABLED)
}
#endif//!defined(EM_SERIAL1_DISABLED)


#if EM_SERIAL3_ENABLED
  void halInternalUart3RxIsr(uint8_t *rxData, uint8_t length)
  {
    EmSerialFifoQueue *q = emSerialRxQueues[3];
    halResetWatchdog();
    while(length--) {
      if(q->used < (EMBER_SERIAL3_RX_QUEUE_SIZE-1)) {
        FIFO_ENQUEUE(q, *rxData, emSerialRxQueueWraps[3]);
        rxData++;
      } else {
        uartErrorMark(3, EMBER_SERIAL_RX_OVERFLOW);
        return;
      }
    }
  }
#endif


void halInternalUartRxPump(uint8_t port)
{
  #if defined(EM_ENABLE_SERIAL_BUFFER) && (EM_SERIAL1_ENABLED || EM_SERIAL2_ENABLED)
    if (EM_SER1_PORT_BUFFER(port) || EM_SER2_PORT_BUFFER(port)) {
      EmSerialFifoQueue *q = emSerialRxQueues[port];
      uint8_t tail,head;
      uint16_t count=0;
      uint8_t loadA;
      uint8_t loadB;
      //Load all of the hardware status, so we can process what happened
      //without worrying about new data changing these values.
      uint8_t dmaStatus = SCx_REG(port, DMACTRL);
      uint16_t currCountA = SCx_REG(port, RXCNTA);
      uint16_t currCountB = SCx_REG(port, RXCNTB);

      //Normal check to see if A has any data
      if (BUFSTATE(port)->prevCountA != currCountA) {
        //Update the counters and head location for the new data
        count = (currCountA - BUFSTATE(port)->prevCountA);
        q->used += count;
        q->head = (q->head + count) % emSerialRxQueueSizes[port];
        BUFSTATE(port)->prevCountA = currCountA;
        BUFSTATE(port)->waitingForTailA = true;
      }
      //Normal check to see if B has any data at all
      if (BUFSTATE(port)->prevCountB != currCountB) {
        //Update the counters and head location for the new data
        count = (currCountB - BUFSTATE(port)->prevCountB);
        q->used += count;
        q->head = (q->head + count) % emSerialRxQueueSizes[port];
        BUFSTATE(port)->prevCountB = currCountB;
        BUFSTATE(port)->waitingForTailB = true;
        BUFSTATE(port)->waitingForInputToB = false;
      }


      //if the used count is greater than half the buffer size, nothing can be done
      if (q->used > BUFSTATE(port)->rxStartIndexB) {
        return;
      }
      //if nothing is in the FIFO, we can reload both if needed
      if (q->used == 0) {
        loadA = true;
        loadB = true;
        goto reloadBuffers;
      }
      //0 < used < bufferSize, so figure out where tail and head are
      if((q->tail)/(BUFSTATE(port)->rxStartIndexB)) {
        tail = true;  //Tail in B buffer
      } else {
        tail = false; //Tail in A buffer
      }

      if(((uint16_t)(q->head - 1))/(BUFSTATE(port)->rxStartIndexB)) {
        head = true;  //Head in B buffer
      } else {
        head = false; //Head in A buffer
      }

      //To load, the tail must be in the same buffer as the head so we don't
      //overwrite any bytes that haven't drained from the serial FIFO yet.
      if (tail!=head) {
        halInternalUartRxCheckRts(port);
        return;
      }
      // Recall tail true means data is inside B
      loadA = tail;
      loadB = !tail;
  reloadBuffers:
      //check if the buffers need to be reloaded
      if ( (loadA) && (!BUFSTATE(port)->waitingForInputToB) ) {
        if ( (dmaStatus&SC_DMACTRL_RXLODA)
            != SC_DMACTRL_RXLODA) {
          //An error interrupt can move the addresses of the buffer
          //during the flush/reset/reload operation.  At this point the
          //buffer is clear of any usage, so we can reset the addresses
          SCx_REG(port, RXBEGA) = (uint32_t)q->fifo;
          SCx_REG(port, RXENDA) = (uint32_t)(q->fifo + BUFSTATE(port)->fifoSize/2 - 1);
          BUFSTATE(port)->prevCountA = 0;
          BUFSTATE(port)->waitingForTailA = false;
          SCx_REG(port, DMACTRL) = SC_RXLODA;
        }
      }
      if (loadB) {
        if ( (dmaStatus&SC_DMACTRL_RXLODB)
            != SC_DMACTRL_RXLODB) {
          //An error interrupt can move the addresses of the buffer
          //during the flush/reset/reload operation.  At this point the
          //buffer is clear of any usage, so we can reset the addresses
          SCx_REG(port, RXBEGB) = (uint32_t)(q->fifo + BUFSTATE(port)->fifoSize/2);
          SCx_REG(port, RXENDB) = (uint32_t)(q->fifo + BUFSTATE(port)->fifoSize - 1);
          BUFSTATE(port)->prevCountB = 0;
          BUFSTATE(port)->waitingForTailB = false;
          SCx_REG(port, DMACTRL) = SC_RXLODB;
        }
      }
      halInternalUartRxCheckRts(port);
    }
  #endif // defined(EM_ENABLE_SERIAL_BUFFER) && (EM_SERIAL1_ENABLED || EM_SERIAL2_ENABLED)
}

#if defined(EMBER_SERIAL1_RTSCTS) || defined(EMBER_SERIAL2_RTSCTS)
void halInternalUartRxCheckRts(uint8_t port)
{
  // Verify RTS is controlled by SW (not AUTO mode), and isn't already asserted.
  // (The logic to deassert RTS is in halInternalUart1RxIsr().)
  if ((SCx_REG(port, UARTCFG) & (SC_UARTCFG_UARTFLOW | SC_UARTCFG_UARTAUTO | SC_UARTCFG_UARTRTS)) == SC_UARTCFG_UARTFLOW) {
    // Assert RTS if the rx queue tail is in an active (or pending) DMA buffer,
    // because this means the other DMA buffer is empty.
    ATOMIC (
      if ( ( (emSerialRxQueues[port]->tail < emSerialRxQueueSizes[port]/2) &&
             (SCx_REG(port, DMACTRL) & SC_DMACTRL_RXLODA) ) ||
           ( (emSerialRxQueues[port]->tail >= emSerialRxQueueSizes[port]/2)
              && (SCx_REG(port, DMACTRL) & SC_DMACTRL_RXLODB) ) ) {
          SCx_REG(port, UARTCFG) |= SC_UARTCFG_UARTRTS;          // assert RTS
      }
    )
  }
}
#endif

#ifdef EMBER_SERIAL1_RTSCTS
bool halInternalUartFlowControlRxIsEnabled(uint8_t port)
{
  return ( (SCx_REG(port, UARTCFG) & (SC_UARTCFG_UARTFLOW | SC_UARTCFG_UARTAUTO | SC_UARTCFG_UARTRTS)) ==
           (SC_UARTCFG_UARTFLOW | SC_UARTCFG_UARTRTS) );
}
#endif
#ifdef EMBER_SERIAL1_XONXOFF
bool halInternalUartFlowControlRxIsEnabled(uint8_t port)
{
  xonTimer = halCommonGetInt16uQuarterSecondTick(); //FIXME move into new func?
  return ( (xonXoffTxByte == 0) && (xcmdCount > 0) );
}

bool halInternalUartXonRefreshDone(uint8_t port)
{
  return (xcmdCount == XON_REFRESH_COUNT);
}
#endif

bool halInternalUartTxIsIdle(uint8_t port)
{
  // TODO how do we determine idle for the VUART or USB?
  #if defined(EM_PHYSICAL_UART)
    if (EM_SER1_PORT_EN(port) || EM_SER2_PORT_EN(port)) {
      return ( (SCx_REG(port, MODE) == SC_MODE_MODE_UART) &&
               ((SCx_REG(port, UARTSTAT) & SC_UARTSTAT_UARTTXIDLE) != 0) );
    }
  #endif

  return true;
}

#if defined(EM_PHYSICAL_UART)
// If called outside of an ISR, it should be from within an ATOMIC block.
static void halInternalUartTxIsr(uint8_t port)
{
  #if defined(EM_ENABLE_SERIAL_FIFO)
    if (EM_SER_MULTI(EM_SER1_PORT_FIFO(port) || EM_SER2_PORT_FIFO(port))) {
      EmSerialFifoQueue *q = (EmSerialFifoQueue *)emSerialTxQueues[port];

      // At present we really don't care which interrupt(s)
      // occurred, just that one did.  Loop while there is
      // room to send more data and we've got more data to
      // send.  For UART there is no error detection.

#ifdef EMBER_SERIAL1_XONXOFF
      // Sending an XON or XOFF takes priority over data in the tx queue.
      if (xonXoffTxByte && (SCx_REG(port, UARTSTAT) & SC_UARTSTAT_UARTTXFREE) ) {
        SCx_REG(port, DATA) = xonXoffTxByte;
        if (xonXoffTxByte == ASCII_XOFF) {
          xcmdCount = -1;
          HANDLE_ASH_ERROR(EMBER_COUNTER_ASH_XOFF);
        } else {
          xcmdCount = (xcmdCount < 0) ? 1: xcmdCount + 1;
        }
        xonXoffTxByte = 0;    // clear to indicate XON/XOFF was sent
      }
#endif
      while ( (q->used > 0) && (SCx_REG(port, UARTSTAT) & SC_UARTSTAT_UARTTXFREE) ) {
        SCx_REG(port, DATA) = FIFO_DEQUEUE(q, emSerialTxQueueWraps[port]);
      }
    }
  #endif
  #if defined(EM_ENABLE_SERIAL_BUFFER) && (EM_SERIAL1_ENABLED || EM_SERIAL2_ENABLED)
    if (EM_SER_MULTI(EM_SER1_PORT_BUFFER(port) || EM_SER2_PORT_BUFFER(port))) {
      EmSerialBufferQueue *q = (EmSerialBufferQueue *)emSerialTxQueues[port];

      // The only interrupts we care about here are UNLOAD's and IDLE.
      // Our algorithm doesn't really care which interrupt occurred,
      // or even if one really didn't.  If there is data to send and
      // a DMA channel available to send it, then out it goes.

      assert( !((q->used == 0) && (q->nextByte != NULL)) );
      while ( q->used > 0 ) {
        if ( q->nextByte == NULL ) {
          // new message pending, but nextByte not set up yet
          emSerialBufferNextMessageIsr(q);
        }

        // Something to send: do we have a DMA channel to send it on?
        // Probe for an available channel by checking the channel's
        // SC1_DMACTRL.TX_LOAD   == 0 (channel unloaded) &&
        // SC1_DMASTAT.TX_ACTIVE == 0 (channel not active)
        // The latter check should be superfluous but is a safety mechanism.
        if ( !(SCx_REG(port, DMACTRL) & SC_DMACTRL_TXLODA) &&
             !(SCx_REG(port, DMASTAT) & SC_DMASTAT_TXACTA) ) {
          // Channel A is available
          SCx_REG(port, TXBEGA)  = (uint32_t)q->nextByte;
          SCx_REG(port, TXENDA) = (uint32_t)q->lastByte;
          EVENT_SCxFLAG(port) = EVENT_SC12_FLAG_TXULDA; // Ack if pending
          SCx_REG(port, DMACTRL) = SC_TXLODA;
          // Release previously held buffer and hold the newly-loaded one
          // so we can safely use emSerialBufferNextBlockIsr() to check for
          // more data to send without the risk of reusing a buffer we're
          // in the process of DMA-ing.
          if (BUFSTATE(port)->holdBuf[0] != EMBER_NULL_MESSAGE_BUFFER)
            emberReleaseMessageBuffer(BUFSTATE(port)->holdBuf[0]);
          BUFSTATE(port)->holdBuf[0] = q->currentBuffer;
          emberHoldMessageBuffer(BUFSTATE(port)->holdBuf[0]);
          emSerialBufferNextBlockIsr(q, port);
        } else
        if ( !(SCx_REG(port, DMACTRL) & SC_DMACTRL_TXLODB) &&
             !(SCx_REG(port, DMASTAT) & SC_DMASTAT_TXACTB) ) {
          // Channel B is available
          SCx_REG(port, TXBEGB)  = (uint32_t)q->nextByte;
          SCx_REG(port, TXENDB) = (uint32_t)q->lastByte;
          EVENT_SCxFLAG(port) = EVENT_SC12_FLAG_TXULDB; // Ack if pending
          SCx_REG(port, DMACTRL) = SC_TXLODB;
          // Release previously held buffer and hold the newly-loaded one
          // so we can safely use emSerialBufferNextBlockIsr() to check for
          // more data to send without the risk of reusing a buffer we're
          // in the process of DMA-ing.
          if (BUFSTATE(port)->holdBuf[1] != EMBER_NULL_MESSAGE_BUFFER)
            emberReleaseMessageBuffer(BUFSTATE(port)->holdBuf[1]);
          BUFSTATE(port)->holdBuf[1] = q->currentBuffer;
          emberHoldMessageBuffer(BUFSTATE(port)->holdBuf[1]);
          emSerialBufferNextBlockIsr(q, port);
        } else {
          // No channels available; can't send anything now so break out of loop
          break;
        }

      } // while ( q->used > 0 )

      // Release previously-held buffer(s) from an earlier DMA operation
      // if that channel is now free (i.e. it's completed the DMA and we
      // didn't need to use that channel for more output in this call).
      if ( (BUFSTATE(port)->holdBuf[0] != EMBER_NULL_MESSAGE_BUFFER) &&
           !(SCx_REG(port, DMACTRL) & SC_DMACTRL_TXLODA) &&
           !(SCx_REG(port, DMASTAT) & SC_DMASTAT_TXACTA) ) {
        emberReleaseMessageBuffer(BUFSTATE(port)->holdBuf[0]);
        BUFSTATE(port)->holdBuf[0] = EMBER_NULL_MESSAGE_BUFFER;
      }
      if ( (BUFSTATE(port)->holdBuf[1] != EMBER_NULL_MESSAGE_BUFFER) &&
           !(SCx_REG(port, DMACTRL) & SC_DMACTRL_TXLODB) &&
           !(SCx_REG(port, DMASTAT) & SC_DMASTAT_TXACTB) ) {
        emberReleaseMessageBuffer(BUFSTATE(port)->holdBuf[1]);
        BUFSTATE(port)->holdBuf[1] = EMBER_NULL_MESSAGE_BUFFER;
      }
    }
  #endif // defined(EM_ENABLE_SERIAL_BUFFER) && (EM_SERIAL1_ENABLED || EM_SERIAL2_ENABLED)
}
#endif // defined(EM_PHYSICAL_UART)


#if EM_SERIAL1_ENABLED
  //The following registers are the only SC1-UART registers that need to be
  //saved across deep sleep cycles.  All other SC1-UART registers are
  //reenabled or restarted using more complex init or restart algorithms.
  static uint32_t  SC1_UARTPER_SAVED;
  static uint32_t  SC1_UARTFRAC_SAVED;
  static uint32_t  SC1_UARTCFG_SAVED;
#endif // EM_SERIAL1_ENABLED
#if EM_SERIAL2_ENABLED
  //The following registers are the only SC3-UART registers that need to be
  //saved across deep sleep cycles.  All other SC3-UART registers are
  //reenabled or restarted using more complex init or restart algorithms.
  static uint32_t  SC3_UARTPER_SAVED;
  static uint32_t  SC3_UARTFRAC_SAVED;
  static uint32_t  SC3_UARTCFG_SAVED;
#endif // EM_SERIAL2_ENABLED

void halInternalPowerDownUart(void)
{
  #if EM_SERIAL1_ENABLED
    SC1_UARTPER_SAVED = SC1->UARTPER;
    SC1_UARTFRAC_SAVED = SC1->UARTFRAC;
    SC1_UARTCFG_SAVED = SC1->UARTCFG;
    SC1_TXD_GPIO(GPIOCFG_OUT, 1); // Avoid gitching TxD going down
  #endif // EM_SERIAL1_ENABLED
  #if EM_SERIAL2_ENABLED
    SC3_UARTPER_SAVED = SC3->UARTPER;
    SC3_UARTFRAC_SAVED = SC3->UARTFRAC;
    SC3_UARTCFG_SAVED = SC3->UARTCFG;
    // TODO SC3_TXD_GPIO(GPIOCFG_OUT, 1); // Avoid gitching TxD going down
  #endif // EM_SERIAL1_ENABLED
  #if EM_SERIAL3_ENABLED || defined (USB_MSD) || defined (USB_HID)
    #if defined(CORTEXM3_EM35X_USB)
      #ifndef USB_SUSPEND
        USBD_Stop();
      #endif
    #endif
  #endif
}

void halInternalPowerUpUart(void)
{
  #if EM_SERIAL1_ENABLED
    SC1->UARTPER = SC1_UARTPER_SAVED;
    SC1->UARTFRAC = SC1_UARTFRAC_SAVED;
    SC1->UARTCFG = SC1_UARTCFG_SAVED;

    SC1->MODE = SC_MODE_MODE_UART;
    SC1_TXD_GPIO(GPIOCFG_OUT_ALT, 1); // Can Assign TxD glitch-free to UART now

    halInternalInitUartInterrupts(1);
  #endif

  #if EM_SERIAL2_ENABLED
    SC3->UARTPER = SC3_UARTPER_SAVED;
    SC3->UARTFRAC = SC3_UARTFRAC_SAVED;
    SC3->UARTCFG = SC3_UARTCFG_SAVED;

    SC3->MODE = SC_MODE_MODE_UART;
    //SC3_TXD_GPIO(GPIOCFG_OUT_ALT, 1); // Can Assign TxD glitch-free to UART now

    halInternalInitUartInterrupts(2);
  #endif

  #if EM_SERIAL3_ENABLED || defined (USB_MSD) || defined (USB_HID)
    //Remember, halInternalPowerUpUart does not return anything.  Powering
    //up the USB requires going through its normal configuration and
    //enumeration process.
    #if defined(CORTEXM3_EM35X_USB)
      #ifndef USB_SUSPEND
        USBD_Init(&USBCDC_initStruct);
      #endif
    #endif
  #endif
}


void halInternalRestartUart(void)
{
  // This is no longer needed and should be removed as a dinosaur --DMM
}

#if (EMBER_SERIAL1_MODE == EMBER_SERIAL_FIFO) && defined(EMBER_SERIAL1_XONXOFF)
// TODO XON/XOFF on port 2
void halInternalUartFlowControl(uint8_t port)
{
  if (EM_SER1_PORT_EN(port)) {
    uint16_t used = emSerialRxQueues[1]->used;
    uint8_t time = halCommonGetInt16uQuarterSecondTick();

    if (used) {
      xonTimer = time;
    }
    // Send an XON if the rx queue is below the XON threshold
    // and an XOFF had been sent that needs to be reversed
    ATOMIC(
      if ( (xcmdCount == -1) && (used <= XON_LIMIT) ) {
        halInternalUart1ForceXon();
      } else if ( (used == 0) &&
                  ((uint8_t)(time - xonTimer) >= XON_REFRESH_TIME) &&
                  (xcmdCount < XON_REFRESH_COUNT) ) {
        halInternalUart1ForceXon();
      }
    )
  }
}
#endif

#ifdef EMBER_SERIAL1_XONXOFF
// Must be called from within an ATOMIC block.
static void halInternalUart1ForceXon(void)
{
  if (xonXoffTxByte == ASCII_XOFF) {  // if XOFF waiting to be sent, cancel it
    xonXoffTxByte = 0;
    xcmdCount = 0;
  } else {                            // else, send XON and record the time
    xonXoffTxByte = ASCII_XON;
    halInternalStartUartTx(1);
  }
  xonTimer = halCommonGetInt16uQuarterSecondTick();
}
#endif

#if EM_SERIAL1_ENABLED
void halSc1Isr(void)
{
  uint32_t interrupt;

  //this read and mask is performed in two steps otherwise the compiler
  //will complain about undefined order of volatile access
  interrupt = EVENT_SC1->FLAG;
  interrupt &= EVENT_SC1->CFG;

  #if (EMBER_SERIAL1_MODE == EMBER_SERIAL_FIFO)
    while (interrupt != 0) {
  #endif // (EMBER_SERIAL1_MODE == EMBER_SERIAL_FIFO)

      EVENT_SC1->FLAG = interrupt; // acknowledge the interrupts early

      // RX events
      if ( interrupt & (EVENT_SC12_FLAG_RXVAL  | // RX has data
                        EVENT_SC12_FLAG_RXOVF  | // RX Overrun error
                        EVENT_SC12_FLAG_RXFIN  | // RX done [TWI]
                        EVENT_SC12_FLAG_NAK    | // RX Nack [TWI]
                        EVENT_SC12_FLAG_RXULDA | // RX DMA A has data
                        EVENT_SC12_FLAG_RXULDB | // RX DMA B has data
                        EVENT_SC12_FLAG_FRMERR | // RX Frame error
                        EVENT_SC12_FLAG_PARERR ) // RX Parity error
         ) {
        halInternalUartRxIsr(1, interrupt);
      }

      // TX events
      if ( interrupt & (EVENT_SC12_FLAG_TXFREE | // TX has room
                        EVENT_SC12_FLAG_TXIDLE | // TX idle (more room)
                        EVENT_SC12_FLAG_TXUND  | // TX Underrun [SPI/TWI]
                        EVENT_SC12_FLAG_TXFIN  | // TX complete [TWI]
                        EVENT_SC12_FLAG_CMDFIN | // TX Start/Stop done [TWI]
                        EVENT_SC12_FLAG_TXULDA | // TX DMA A has room
                        EVENT_SC12_FLAG_TXULDB ) // TX DMA B has room
         ) {
        halInternalUartTxIsr(1);
      }

  #if (EMBER_SERIAL1_MODE == EMBER_SERIAL_FIFO)
      interrupt = EVENT_SC1->FLAG;
      interrupt &= EVENT_SC1->CFG;
    }
  #endif // (EMBER_SERIAL1_MODE == EMBER_SERIAL_FIFO)
}
#endif // EM_SERIAL1_ENABLED

#if EM_SERIAL2_ENABLED
void halSc3Isr(void)
{
  uint32_t interrupt;

  //this read and mask is performed in two steps otherwise the compiler
  //will complain about undefined order of volatile access
  interrupt = EVENT_SCxFLAG(2);
  interrupt &= EVENT_SCxCFG(2);

  #if (EMBER_SERIAL2_MODE == EMBER_SERIAL_FIFO)
    while (interrupt != 0) {
  #endif // (EMBER_SERIAL2_MODE == EMBER_SERIAL_FIFO)

      EVENT_SCxFLAG(2) = interrupt; // acknowledge the interrupts early

      // RX events
      if ( interrupt & (EVENT_SC12_FLAG_RXVAL  | // RX has data
                        EVENT_SC12_FLAG_RXOVF  | // RX Overrun error
                        EVENT_SC12_FLAG_RXFIN  | // RX done [TWI]
                        EVENT_SC12_FLAG_NAK    | // RX Nack [TWI]
                        EVENT_SC12_FLAG_RXULDA | // RX DMA A has data
                        EVENT_SC12_FLAG_RXULDB | // RX DMA B has data
                        EVENT_SC12_FLAG_FRMERR | // RX Frame error
                        EVENT_SC12_FLAG_PARERR ) // RX Parity error
         ) {
        halInternalUartRxIsr(2, interrupt);
      }

      // TX events
      if ( interrupt & (EVENT_SC12_FLAG_TXFREE | // TX has room
                        EVENT_SC12_FLAG_TXIDLE | // TX idle (more room)
                        EVENT_SC12_FLAG_TXUND  | // TX Underrun [SPI/TWI]
                        EVENT_SC12_FLAG_TXFIN  | // TX complete [TWI]
                        EVENT_SC12_FLAG_CMDFIN | // TX Start/Stop done [TWI]
                        EVENT_SC12_FLAG_TXULDA | // TX DMA A has room
                        EVENT_SC12_FLAG_TXULDB ) // TX DMA B has room
         ) {
        halInternalUartTxIsr(2);
      }

  #if (EMBER_SERIAL2_MODE == EMBER_SERIAL_FIFO)
      interrupt = EVENT_SCxFLAG(2);
      interrupt &= EVENT_SCxCFG(2);
    }
  #endif // (EMBER_SERIAL2_MODE == EMBER_SERIAL_FIFO)
}
#endif // EM_SERIAL2_ENABLED

#if     UARTERRORMARK_NEEDED
static void uartErrorMark(uint8_t port, uint8_t errors)
{
  EmSerialFifoQueue *q = emSerialRxQueues[port];

  // save error code & location in queue
  if ( emSerialRxError[port] == EMBER_SUCCESS ) {
    emSerialRxErrorIndex[port] = q->head;
    emSerialRxError[port] = errors;
  } else {
    // Flush back to previous error location & update value
    q->head = emSerialRxErrorIndex[port];
    emSerialRxError[port] = errors;
    if(q->head < q->tail) {
      q->used = (emSerialRxQueueSizes[port] - q->tail) + q->head;
    } else {
      q->used = q->head - q->tail;
    }
  }
}
#endif//UARTERRORMARK_NEEDED
