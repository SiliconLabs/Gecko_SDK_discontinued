/** @file hal/micro/cortexm3/spi-master.c
 *  @brief EM3XX SPI Master Driver API
 *
 * <!-- Copyright 2013 Silicon Laboratories, Inc.                        *80*-->
 */

#include PLATFORM_HEADER
#include "stack/include/ember-types.h"
#include "stack/include/error.h"
#include "hal/hal.h"
#include "hal/micro/cortexm3/memmap.h"  // For RAM_BOTTOM
#include "serial/serial.h" // To assess SERIAL1 & SERIAL3 modes
#include "spi-master.h"         // Implement this API


//== DRIVER CONFIGURATION ==

/**
 * @brief SPI Master Driver configuration options:
 * - ENABLE_SPI_SC:  Specifies the SPI port(s) to be supported:
 *   undefined = driver neutered
 *    0        = SC0 only "bitbang" (see next item for add'l requirements)
 *    1        = SC1 only
 *    2        = SC2 only
 *    3        = SC3 only (if platform supports it)
 *    4        = SC4 only (if platform supports it)
 *   10        = SC0,SC1,SC2 ports all supported along with SC3 and SC4
 *               (if platform supports them)
 * - For SC0 "bitbang" driver to be supported, the BOARD_HEADER must provide
 *   definitions for SC0_SCLK, SC0_MOSI, SC0_MISO and (optionally) SC0_NSEL
 *   GPIO assignments to PORT{A-F}x_PIN{0..7}, along with corresponding
 *   convenience definitions _PORT (A..F) and _BIT {0..7}.  E.g.
 *      #define SC0_SCLK        PORTA_PIN(2)
 *      #define SC0_SCLK_PORT   A
 *      #define SC0_SCLK_BIT    2
 *      #define SC0_MOSI        PORTA_PIN(3)
 *      #define SC0_MOSI_PORT   A
 *      #define SC0_MOSI_BIT    3
 *      #define SC0_MISO        PORTA_PIN(4)
 *      #define SC0_MISO_PORT   A
 *      #define SC0_MISO_BIT    4
 *      #define SC0_NSEL        PORTB_PIN(5)
 *      #define SC0_NSEL_PORT   B
 *      #define SC0_NSEL_BIT    5
 * - SC1 can be configured and initialized for SPI, but requires its
 *   compile-time EMBER_SERIAL1_MODE definition to be EMBER_SERIAL_UNUSED
 *   or EMBER_SERIAL_LOWLEVEL in order to be able to specify asynchronous
 *   operations with a callback (which requires interrupt handling).  This
 *   is necessary to avoid an ISR naming conflict with the UART driver.
 * - SC3 has this similar limitation as SC1 on platforms that support it.
 * - ENABLE_SPI_INTERNAL_ISRS: If this is #defined as 1 when ENABLE_SPI_SC
 *   is defined as 10, the above limitation is eliminated by defining
 *   internal ISRs and activating them via RAM interrupt vectors.
 *   This can noticeably increase the RAM footprint of the application.
 * - The BOARD_HEADER can override default choice of SCx_NSEL, and can
 *   define it to 0xFF to disable the driver's use of NSEL on that port
 *   (e.g. if user prefers to control NSEL itself or through the ext-device
 *   driver).
 */
//#define ENABLE_SPI_SC 10          // Enable to activate SC0,1,2,3,4 support
//#define ENABLE_SPI_INTERNAL_ISRS 1 // Enable to override normal SCx ISRs

#define SPIDBG(blah)  // blah       // Uncomment blah to enable debugging
SPIDBG(extern uint8_t serialPort;)

#ifdef  ENABLE_SPI_SC // Driver is enabled

//== DEFINITIONS ==

//-- SPI Port custom clock rate -- can override in BOARD_HEADER
#ifndef EMBER_SPI_CLK_CUSTOM_SETTING
#define EMBER_SPI_CLK_CUSTOM_SETTING    0xE5  // 25 kHz
#define EMBER_SPI_CLK_CUSTOM_PERIOD_NS  40000 // 25 kHz = 40000 ns period
#endif//EMBER_SPI_CLK_CUSTOM_SETTING

//-- SPI Port Configuration
// SC0 parameters, if used, must be defined in BOARD_HEADER with _PORT and _BIT
// helper definitions.
//TODO: Currently only support one slave select per port.
#ifndef SC0_NSEL
#define SC0_NSEL SPI_NSEL_EXTERNAL
#endif//SC0_NSEL
#ifndef SC1_NSEL
#define SC1_NSEL PORTB_PIN(4)
#endif//SC1_NSEL
#ifndef SC2_NSEL
#define SC2_NSEL PORTA_PIN(3)
#endif//SC2_NSEL
#if     SC3_AND_SC4_EXIST
#ifndef SC3_NSEL
#define SC3_NSEL ((3<<3)|4) //FIXME: PORTD_PIN(4)
#endif//SC3_NSEL
#ifndef SC4_NSEL
#define SC4_NSEL ((4<<3)|3) //FIXME: PORTE_PIN(3)
#endif//SC4_NSEL
#endif//SC3_AND_SC4_EXIST

#if (EMBER_SERIAL1_MODE == EMBER_SERIAL_UNUSED) || \
    (EMBER_SERIAL1_MODE == EMBER_SERIAL_LOWLEVEL)
  #define CAN_USE_SC1_INT 1 // This driver can provide halSc1Isr()
#endif
#if (EMBER_SERIAL3_MODE == EMBER_SERIAL_UNUSED) || \
    (EMBER_SERIAL3_MODE == EMBER_SERIAL_LOWLEVEL)
  #define CAN_USE_SC3_INT 1 // This driver can provide halSc3Isr()
#endif

#define SPI_USE_DMA_THRESHOLD  2 // Number of bytes at which DMA is better

// Some handy-dandy preprocessor magic
#define PASTE(a,b,c)       a##b##c
#define EVAL3(a,b,c)       PASTE(a,b,c)

#undef  SCx_REG // Need to obliterate the micro-common.h UART-focused one

#if     (ENABLE_SPI_SC > 4) && (ENABLE_SPI_SC != 10) //----------------------

  #error "ENABLE_SPI_SC must be in range 0..4 or 10"

#elif   (ENABLE_SPI_SC == 0) //-----------------------------------------------

  // Simplify the code by supporting SPI only on SC0 bitbang
 #ifndef SC0_SCLK
  #error "ENABLE_SPI_SC on SC0, but SC0 GPIOs are not defined in BOARD_HEADER"
 #endif//SC0_SCLK
  #define SCx_NSEL(port)                (SC0_NSEL)
  #define SCx_OPFLAGS(port)             (scxOpFlags)

  static uint8_t scxOpFlags;

#elif   (ENABLE_SPI_SC < 10) //-----------------------------------------------

  // Simplify the code by supporting SPI only on SCx
  #undef  SC0_SCLK                      //Disable SC0 support in these builds
  #define SCx_REG(port, reg)            (EVAL3(SC,ENABLE_SPI_SC,_##reg))
  #define SCx_NSEL(port)                (SCx_REG(port, NSEL))
  #define SCx_OPFLAGS(port)             (scxOpFlags)
  #define SCx_CB(port)                  (scxCB)
  #define INT_SCx(port)                 (EVAL3(INT_SC,ENABLE_SPI_SC,))
  #define INT_SCxCFG(port)              (EVAL3(INT_SC,ENABLE_SPI_SC,CFG))
  #define INT_SCxFLAG(port)             (EVAL3(INT_SC,ENABLE_SPI_SC,FLAG))
  #define halRegisterRamVector(a, b)    /* no-op */

  static uint8_t scxOpFlags;
  static EmberSpiTransferCallback scxCB = NULL;

  static void scxIsrHandler(EmberSpiPort port); // Forward reference
  void EVAL3(halSc,ENABLE_SPI_SC,Isr)(void) { scxIsrHandler(ENABLE_SPI_SC); }

#else//(ENABLE_SPI_SC == 10) //-----------------------------------------------

  // Support SPI on any/all SCx controllers
  #define SCx_REG(port, reg)            (*((volatile uint32_t*)                 \
                 (SCx_BASE_ADDR(port) + (SC2_##reg##_ADDR - SC2_RXBEGA_ADDR))))
  #define SCx_BASE_ADDR(port)           (scxBaseAddr[port])
  #define SCx_NSEL(port)                (scxNSel[port])
  #define SCx_OPFLAGS(port)             (scxOpFlags[port])
  #define SCx_CB(port)                  (scxCB[port])
  #define SCx_ISR(port)                 (scxIsr[port])
  #define SCx_VECTOR_INDEX(port)        (scxVectorIndex[port])
  #define INT_SCx(port)                 (intScx[port])
  #define INT_SCxCFG(port)              (*((volatile uint32_t*)(intScxCfg[port])))
  #define INT_SCxFLAG(port)             (*((volatile uint32_t*)(intScxFlag[port])))


  //== GLOBALS ==

  static const uint32_t scxBaseAddr[] = {
    0,                  // SC0 "bitbang"
    SC1_RXBEGA_ADDR,    // SC1 em35x
    SC2_RXBEGA_ADDR,    // SC2 em35x
   #if     SC3_AND_SC4_EXIST
    SC3_RXBEGA_ADDR,    // SC3 em35y
    SC4_RXBEGA_ADDR,    // SC4 em35y
   #endif//SC3_AND_SC4_EXIST
  };

  static const uint8_t scxNSel[] = {
    SC0_NSEL,
    SC1_NSEL,
    SC2_NSEL,
   #if     SC3_AND_SC4_EXIST
    SC3_NSEL,
    SC4_NSEL,
   #endif//SC3_AND_SC4_EXIST
  };

  static const uint32_t intScx[] = {
    0,                  // SC0 "bitbang"
    INT_SC1,            // SC1 em35x
    INT_SC2,            // SC2 em35x
   #if     SC3_AND_SC4_EXIST
    INT_SC3,            // SC3 em35y
    INT_SC4,            // SC4 em35y
   #endif//SC3_AND_SC4_EXIST
  };

  static const uint32_t intScxCfg[] = {
   0,                  // SC0 "bitbang"
   INT_SC1CFG_ADDR,    // SC1 em35x
   INT_SC2CFG_ADDR,    // SC2 em35x
  #if     SC3_AND_SC4_EXIST
   INT_SC3CFG_ADDR,    // SC3 em35y
   INT_SC4CFG_ADDR,    // SC4 em35y
  #endif//SC3_AND_SC4_EXIST
  };

  static const uint32_t intScxFlag[] = {
    0,                  // SC0 "bitbang"
    INT_SC1FLAG_ADDR,   // SC1 em35x
    INT_SC2FLAG_ADDR,   // SC2 em35x
   #if     SC3_AND_SC4_EXIST
    INT_SC3FLAG_ADDR,   // SC3 em35y
    INT_SC4FLAG_ADDR,   // SC4 em35y
   #endif//SC3_AND_SC4_EXIST
  };

  static uint8_t scxOpFlags[] = {
    0,
    0,
    0,
   #if     SC3_AND_SC4_EXIST
    0,
    0,
   #endif//SC3_AND_SC4_EXIST
  };

  static EmberSpiTransferCallback scxCB[] = {
    NULL,
    NULL,
    NULL,
   #if     SC3_AND_SC4_EXIST
    NULL,
    NULL,
   #endif//SC3_AND_SC4_EXIST
  };


  //== ISRs

 #if     ENABLE_SPI_INTERNAL_ISRS
  // Set up internal ISR handlers for the SCx peripherals using RAM vectors
  #define ISR_CLASS static

  #define INT_SC1_ISR intSc1Isr
  #define INT_SC2_ISR intSc2Isr
 #if     SC3_AND_SC4_EXIST
  #define INT_SC3_ISR intSc3Isr
  #define INT_SC4_ISR intSc4Isr
 #endif//SC3_AND_SC4_EXIST

 #else//!ENABLE_SPI_INTERNAL_ISRS
  // Set up normal ISR handlers for the SCx peripherals using Flash vectors
  #define ISR_CLASS /*global*/

 #if     CAN_USE_SC1_INT
  #define INT_SC1_ISR halSc1Isr
 #endif//CAN_USE_SC1_INT
  #define INT_SC2_ISR halSc2Isr
 #if     SC3_AND_SC4_EXIST
 #if     CAN_USE_SC3_INT
  #define INT_SC3_ISR halSc3Isr
 #endif//CAN_USE_SC3_INT
  #define INT_SC4_ISR halSc4Isr
 #endif//SC3_AND_SC4_EXIST

 #endif//ENABLE_SPI_INTERNAL_ISRS

  static void scxIsrHandler(EmberSpiPort port); // Forward reference
 #ifdef  INT_SC1_ISR
  ISR_CLASS void INT_SC1_ISR(void) { scxIsrHandler(1); }
 #endif//INT_SC1_ISR
 #ifdef  INT_SC2_ISR
  ISR_CLASS void INT_SC2_ISR(void) { scxIsrHandler(2); }
 #endif//INT_SC2_ISR
 #ifdef  INT_SC3_ISR
  ISR_CLASS void INT_SC3_ISR(void) { scxIsrHandler(3); }
 #endif//INT_SC3_ISR
 #ifdef  INT_SC4_ISR
  ISR_CLASS void INT_SC4_ISR(void) { scxIsrHandler(4); }
 #endif//INT_SC4_ISR

 #if     ENABLE_SPI_INTERNAL_ISRS
  static const uint32_t scxVectorIndex[] = {
    0,                  // SC0 "bitbang"
    SC1_VECTOR_INDEX,   // SC1 em35x
    SC2_VECTOR_INDEX,   // SC2 em35x
   #if     SC3_AND_SC4_EXIST
    SC3_VECTOR_INDEX,   // SC3 em35y
    SC4_VECTOR_INDEX,   // SC4 em35y
   #endif//SC3_AND_SC4_EXIST
  };

  typedef void (*isrHandler)(void);
  static const isrHandler scxIsr[] = {
    NULL,
    &intSc1Isr,
    &intSc2Isr,
   #if     SC3_AND_SC4_EXIST
    &intSc3Isr,
    &intSc4Isr,
   #endif//SC3_AND_SC4_EXIST
  };
 #endif//ENABLE_SPI_INTERNAL_ISRS

#endif//ENABLE_SPI_SC //------------------------------------------------------

#if     (ENABLE_SPI_SC > 0)
  static void scxIsrHandler(EmberSpiPort port)
  {
    // One-shot transfer-completed interrupt fired
    //assert((INT_SCxFLAG(port) & INT_SCxCFG(port)) != 0)
    INT_SCxCFG(port)  = 0x0000; // Disable all 2nd-level interrupts
    INT_SCxFLAG(port) = 0xFFFF; // Ack all 2nd-level interrupts
    INT_CFGCLR = INT_SCx(port); // Disable top-level
    if (halSpiMasterIsBusy(port)) { // Deassert nSEL if operation requested it
      assert(false); // SPI still busy at end of transfer?? Yipes!
    }
    uint16_t rxLen = SCx_REG(port, RXCNTA); // Use RXCNTA for how much received
    uint32_t rxDma = (rxLen > 0) ? SCx_REG(port, RXBEGA) : 0;
    // Callback uses TXCNT for how much was sent, not received.
    // Note this works for Master mode, since it clocks out all
    // it is given, but it would not work for Slave mode due to
    // FIFO sucking up to 4 DMA bytes which the slave might not
    // clock out.  For Slave mode, RXCNT is therefore king.
    (*(SCx_CB(port)))(port, SCx_REG(port, TXCNT), (uint8_t*) rxDma);
  }

  /** @brief Reset the SPI controller for a fresh transfer
   * @param  port     serial controller port to reset (> 0)
   */
  static void resetSpi(EmberSpiPort port)
  {
    // Reset both Rx and Tx DMA side; SPI is not independently full-duplex
    // N.B. The sequence of the next 5 operations is VERY IMPORTANT to not
    //      leave a phantom interrupt pending which could fire inopportunely.
    SCx_REG(port, DMACTRL) = (SC_RXDMARST | SC_TXDMARST);
    INT_CFGCLR  = INT_SCx(port); // Disable top-level interrupt
    INT_SCxCFG(port)  = 0;       // Disable 2nd-level interrupts
    INT_SCxFLAG(port) = 0xFFFF;  // Clear stale 2nd-level interrupts
    INT_PENDCLR = INT_SCx(port); // Clear stale top-level interrupt
  }
#endif//(ENABLE_SPI_SC > 0)


//== GPIO OPERATIONS ==

//-- GPIO access for nSEL signals
#define GPIO_PxCLR_BASE       (GPIO_PACLR_ADDR)
#define GPIO_PxSET_BASE       (GPIO_PASET_ADDR)
#define GPIO_PxOUT_BASE       (GPIO_PAOUT_ADDR)
#define GPIO_PxCFG_BASE       (GPIO_PACFGL_ADDR)
// Each port is offset from the previous port by the same amount
#define GPIO_Px_OFFSET        (GPIO_PBCFGL_ADDR - GPIO_PACFGL_ADDR)

#ifdef  SC0_SCLK
  //-- Handy SC0 BitBang support -- only used if enabled
  #define GPIO_INP(bank,bit)  ( !!(EVAL3(GPIO_P,bank,IN)  & EVAL3(PA,bit,_MASK)) )
  #define GPIO_CLR(bank,bit)  (    EVAL3(GPIO_P,bank,CLR) = EVAL3(PA,bit,_MASK)  )
  #define GPIO_SET(bank,bit)  (    EVAL3(GPIO_P,bank,SET) = EVAL3(PA,bit,_MASK)  )

  #define SDI_x()             GPIO_INP(SC0_MISO_PORT, SC0_MISO_BIT) // SDI in
  #define SDO_0()             GPIO_CLR(SC0_MOSI_PORT, SC0_MOSI_BIT) // SDO=0
  #define SDO_1()             GPIO_SET(SC0_MOSI_PORT, SC0_MOSI_BIT) // SDO=1
  #define CLK_LO()            GPIO_CLR(SC0_SCLK_PORT, SC0_SCLK_BIT) // SCLK=lo
  #define CLK_HI()            GPIO_SET(SC0_SCLK_PORT, SC0_SCLK_BIT) // SCLK=hi

  static uint8_t  spiBbCfg;
  static uint16_t spiBbCycleNs;

  static void   spiBbDelayNs(uint16_t ns)
  {
    //FIXME: Just run as fast as we can in bitbang mode, ignoring actual speed
  }
#endif//SC0_SCLK

static void halInternalGpioClr(uint8_t gpio)
{
  *((volatile uint32_t *)(GPIO_PxCLR_BASE+(GPIO_Px_OFFSET*(gpio/8)))) = BIT(gpio&7);
}

static void halInternalGpioSet(uint8_t gpio)
{
  *((volatile uint32_t *)(GPIO_PxSET_BASE+(GPIO_Px_OFFSET*(gpio/8)))) = BIT(gpio&7);
}

#if 0 //FIXME: Make BOARD_HEADER configure GPIOs appropriately
static void halInternalGpioCfg(uint8_t gpio, uint32_t cfg)
{
  volatile uint32_t* reg = (GPIO_PxCFG_BASE+(GPIO_Px_OFFSET*(gpio/8))+((gpio/4)&1));
  uint8_t shift = 4*(gpio%4);
  *reg = ((*reg) & ~(0x0000000fu << shift)) | (cfg << shift);
}
#endif//FIXME: Make BOARD_HEADER configure GPIOs appropriately

/** @brief Select the SPI port's slave device
 * @note Currently only one slave per port is supported
 */
static void halInternalSpiMasterSelectSlave(EmberSpiPort port)
{
  if (SCx_NSEL(port) != SPI_NSEL_EXTERNAL) {
    halInternalGpioClr(SCx_NSEL(port));
  }
}

/** @brief Deselect the SPI port's slave device
 * @note Currently only one slave per port is supported
 */
static void halInternalSpiMasterDeselectSlave(EmberSpiPort port)
{
  if (SCx_NSEL(port) != SPI_NSEL_EXTERNAL) {
    halInternalGpioSet(SCx_NSEL(port));
  }
}

/** @brief Transfer single byte over SPI, synchronously
 * @param  port     serial controller port to init (0=bitbang, 1=SC1, ...)
 * @param  txByte   byte to transmit
 * @return byte received
 */
static uint8_t halSpiMasterTransferByte(EmberSpiPort port, uint8_t txByte)
{
  //assert(port < EMBER_SPI_PORT_MAX);
 #ifdef  SC0_SCLK
  if (port == EMBER_SPI_PORT_BITBANG) {
    uint8_t bits = 8;
    uint8_t bitmask = (spiBbCfg & SC_SPIORD) ? 0x01 : 0x80;

    // Bitbang the bits out, respecting bit order, phase, polarity
    switch (spiBbCfg & (SC_SPIPHA | SC_SPIPOL)) {
    case 0:  // Sample on rising, change on falling, CLK lo idle
    case 3:  // Sample on rising, change on falling, CLK hi idle
      while (bits-- > 0) {
        ATOMIC(
          // Falling edge, change data
          CLK_LO();
          if (txByte & bitmask) {
            SDO_1();
          } else {
            SDO_0();
          }
        )
        spiBbDelayNs(spiBbCycleNs/2);
        ATOMIC(
          // Rising edge, sample data
          CLK_HI();
          // Use txByte for Rx too
          if (spiBbCfg & SC_SPIORD) {
            txByte = (txByte >> 1) | (SDI_x() << 7); // LSB-to-MSB
          } else {
            txByte = (txByte << 1) | SDI_x();        // MSB-to-LSB
          }
        )
        spiBbDelayNs(spiBbCycleNs/2);
      }
      if ((spiBbCfg & SC_SPIPOL) != SC_SPIPOL) {
        CLK_LO(); // Mode 0: Leave with SCLK low
      }
      break;

    case 1:  // Sample on falling, change on rising, CLK hi idle
    case 2:  // Sample on falling, change on rising, CLK lo idle
      while (bits-- > 0) {
        ATOMIC(
          // Rising edge, change data
          CLK_HI();
          if (txByte & bitmask) {
            SDO_1();
          } else {
            SDO_0();
          }
        )
        spiBbDelayNs(spiBbCycleNs/2);
        ATOMIC(
          // Falling edge, sample data
          CLK_LO();
          // Use txByte for Rx too
          if (spiBbCfg & SC_SPIORD) {
            txByte = (txByte >> 1) | (SDI_x() << 7); // LSB-to-MSB
          } else {
            txByte = (txByte << 1) | SDI_x();        // MSB-to-LSB
          }
        )
        spiBbDelayNs(spiBbCycleNs/2);
      }
      if ((spiBbCfg & SC_SPIPOL) == SC_SPIPOL) {
        CLK_HI(); // Mode 1: Leave with SCLK high
      }
      break;
    }
    return txByte;
  }
 #endif//SC0_SCLK

 #if    (ENABLE_SPI_SC > 0)
  SCx_REG(port, DATA) = txByte;
  while ((SCx_REG(port, SPISTAT) & SC_SPITXIDLE) != SC_SPITXIDLE) {
    // wait for Tx to finish -- this should be pretty quick
  }
  while ((SCx_REG(port, SPISTAT) & SC_SPIRXVAL) != SC_SPIRXVAL) {
    // wait for Rx to finish -- this should be instantaneous
  }
  txByte = SCx_REG(port, DATA);
 #endif//(ENABLE_SPI_SC > 0)
  return txByte;
}


//== API FUNCTIONS ==

/** @brief Initialize SPI Master port
 * @param  port     serial controller port to init (0=bitbang, 1=SC1, ...)
 * @param  clkRate  SPI clock frequency to use per EmberSpiClkRate choices
 * @param  clkMode  SPI clocking mode to use per Emb erSpiClkMode choices
 * @param  lsbFirst true to send bytes LSB-first, false for MSB-first
 * @return EMBER_SUCCESS if everything goes well, something else if not
 */
EmberStatus halSpiMasterInit(EmberSpiPort port,
                             EmberSpiClkRate clkRate,
                             EmberSpiClkMode clkMode,
                             bool lsbFirst)
{
  if (port >= EMBER_SPI_PORT_MAX) {
    return EMBER_SERIAL_INVALID_PORT;
  }
  if (port == EMBER_SPI_PORT_BITBANG) {
   #ifdef  SC0_SCLK
   #if 0 //FIXME: Make BOARD_HEADER configure GPIOs appropriately
    if (SC0_NSEL != SPI_NSEL_EXTERNAL) {
      halInternalGpioCfg(SC0_NSEL, GPIOCFG_OUT);
    }
    halInternalGpioCfg(SC0_SCLK, GPIOCFG_OUT);
    halInternalGpioCfg(SC0_MOSI, GPIOCFG_OUT);
    halInternalGpioCfg(SC0_MISO, GPIOCFG_IN );
   #endif//FIXME: Make BOARD_HEADER configure GPIOs appropriately
    halInternalSpiMasterDeselectSlave(port);
    spiBbCfg = ((!!lsbFirst) << SC_SPIORD_BIT) | clkMode;
    (spiBbCfg & SC_SPIPOL) ? CLK_HI() : CLK_LO();
    switch (clkRate) {
    case EMBER_SPI_CLK_12_MHZ:
      spiBbCycleNs = 83;
      break;
    case EMBER_SPI_CLK_6_MHZ:
      spiBbCycleNs = 166;
      break;
    case EMBER_SPI_CLK_4_MHZ:
      spiBbCycleNs = 250;
      break;
    case EMBER_SPI_CLK_3_MHZ:
      spiBbCycleNs = 333;
      break;
    case EMBER_SPI_CLK_2_MHZ:
      spiBbCycleNs = 500;
      break;
    case EMBER_SPI_CLK_1_MHZ:
      spiBbCycleNs = 1000;
      break;
    case EMBER_SPI_CLK_750_KHZ:
      spiBbCycleNs = 1333;
      break;
    case EMBER_SPI_CLK_500_KHZ:
      spiBbCycleNs = 2000;
      break;
    case EMBER_SPI_CLK_250_KHZ:
      spiBbCycleNs = 4000;
      break;
    case EMBER_SPI_CLK_100_KHZ:
      spiBbCycleNs = 10000;
      break;
    case EMBER_SPI_CLK_50_KHZ:
      spiBbCycleNs = 20000;
      break;
   #ifdef  EMBER_SPI_CLK_CUSTOM_PERIOD_NS
    case EMBER_SPI_CLK_CUSTOM:
      spiBbCycleNs = EMBER_SPI_CLK_CUSTOM_PERIOD_NS;
      break;
   #endif//EMBER_SPI_CLK_CUSTOM_PERIOD_NS
    default:
      return EMBER_SERIAL_INVALID_BAUD_RATE;
    }
   #else//!SC0_SCLK
    return EMBER_SERIAL_INVALID_PORT; // No BitBang support specified
   #endif//SC0_SCLK
  } else {
   #if     (ENABLE_SPI_SC > 0)
    halInternalSpiMasterDeselectSlave(port);
   #if 0 //FIXME: Make BOARD_HEADER configure GPIOs appropriately
    //FIXME: If need to configure GPIOs here, work needs to be done!
   #endif//FIXME: Make BOARD_HEADER configure GPIOs appropriately
    if (clkRate == EMBER_SPI_CLK_CUSTOM) {
     #ifdef  EMBER_SPI_CLK_CUSTOM_SETTING
      clkRate = EMBER_SPI_CLK_CUSTOM_SETTING;
     #else//!EMBER_SPI_CLK_CUSTOM_SETTING
      return EMBER_SERIAL_INVALID_BAUD_RATE;
     #endif//EMBER_SPI_CLK_CUSTOM_SETTING
    }
    SCx_REG(port, MODE)    = 0; // Disable the port
    SCx_REG(port, SPICFG)  = 0; // Reset all SPI CFG
    SCx_REG(port, RATELIN) = (clkRate >> 4);   // RATELIN in upper nibble
    SCx_REG(port, RATEEXP) = (clkRate & 0x0F); // RATEEXP in lower nibble
    SCx_REG(port, SPICFG)  = ( 0
                             | (  1        << SC_SPIMST_BIT) // Master only here
                             | (!!lsbFirst << SC_SPIORD_BIT)
                             | (  clkMode /*Is already shfited*/)
                             );
    SCx_REG(port, MODE)    = SC2_MODE_SPI; // SC2_MODE_SPI value works on all SC#s
    resetSpi(port);
   #if     ENABLE_SPI_INTERNAL_ISRS
    halRegisterRamVector(SCx_VECTOR_INDEX(port), (uint32_t) SCx_ISR(port));
   #endif//ENABLE_SPI_INTERNAL_ISRS
   #else//!(ENABLE_SPI_SC > 0)
    return EMBER_SERIAL_INVALID_PORT; // Only BitBang support specified
   #endif//(ENABLE_SPI_SC > 0)
  }
  SCx_OPFLAGS(port) = 0; // Clear out any prior transaction knowledge
  return EMBER_SUCCESS;
}

/** @brief Returns SPI port busy (transfer in progress)
 * @param  port     serial controller port to init (0=bitbang, 1=SC1, ...)
 * @return true if SPI port is busy, false if idle (and will complete prior
 *         transaction per its opFlags.
 */
bool halSpiMasterIsBusy(EmberSpiPort port)
{
  bool isBusy = false; // Assume not busy, always the case for BitBang
  //assert(port < EMBER_SPI_PORT_MAX);
 #if    (ENABLE_SPI_SC > 0)
  if (port != EMBER_SPI_PORT_BITBANG) {
    isBusy = !(SCx_REG(port, SPISTAT) & SC_SPITXIDLE);
  }
 #endif//(ENABLE_SPI_SC > 0)
  // Deselect slave when no longer busy and transfer's opFlags says we can.
  // This covers asynchronous transfers upon completion.
  if ( (!isBusy) && ((SCx_OPFLAGS(port) & EMBER_SPI_OP_KEEP_SEL) == 0) ) {
    halInternalSpiMasterDeselectSlave(port);
  }
  return isBusy;
}

/** @brief Runtime-tunable DMA threshold
 */
uint8_t halSpiMasterDmaThreshold = SPI_USE_DMA_THRESHOLD;

/** @brief Transfer buffer over SPI
 * @param  port     serial controller port to init (0=bitbang, 1=SC1, ...)
 * @param  trLen    number of bytes to transfer
 * @param  txBuf    buffer to transmit; may be NULL for Rx-only
 * @param  rxBuf    buffer for receive; may be NULL for Tx-only, or same
 *                  as txBuf to (safely) overwrite Tx data with Rx data
 * @param  opFlags  flags (bitmask) for the transfer per EmberSpiOpFlags choices
 * @param  opDoneCB operation complete callback (NULL if don't want callback)
 * @return EMBER_SUCCESS if everything goes well, something else if not (and
 *                  callback will not be called)
 * @note A zero-len transfer will still manipulate nSEL per opFlags, even
 * though no data will be moved.
 */
EmberStatus halSpiMasterTransferBuf(EmberSpiPort port,
                                    uint16_t trLen,
                                    uint8_t* txBuf,
                                    uint8_t* rxBuf,
                                    EmberSpiOpFlags opFlags,
                                    const EmberSpiTransferCallback opDoneCB)
{
  //assert(port < EMBER_SPI_PORT_MAX);
  assert(trLen == 0 || txBuf != NULL || rxBuf != NULL);
  uint8_t spLen = (opFlags & EMBER_SPI_OP_SPLIT_LEN);
  assert( (spLen == 0)
        ||((spLen <= trLen) && (txBuf != NULL) && (rxBuf != NULL)) );

  // Verify port is configured for SPI
  if ( 0
    #ifdef  SC0_SCLK
     ||((port == EMBER_SPI_PORT_BITBANG) && (spiBbCycleNs == 0))
    #else//!SC0_SCLK
     ||(port == EMBER_SPI_PORT_BITBANG)
    #endif//SC0_SCLK
    #if     (ENABLE_SPI_SC > 0)
     ||((port  > EMBER_SPI_PORT_BITBANG) && (SCx_REG(port, MODE) != SC2_MODE_SPI))
    #endif//(ENABLE_SPI_SC > 0)
     ) {
    return EMBER_ERR_FATAL;
  }

 #if     !ENABLE_SPI_INTERNAL_ISRS
  // Fail if no ISR exists for an ASYNC_IO callback
  if (((opFlags & EMBER_SPI_OP_ASYNC_IO) != 0) && (opDoneCB != NULL)) {
   #if     !CAN_USE_SC1_INT
    if (port == 1) { return EMBER_ERR_FATAL; }
   #endif//!CAN_USE_SC1_INT
   #if     !CAN_USE_SC3_INT
    if (port == 3) { return EMBER_ERR_FATAL; }
   #endif//!CAN_USE_SC3_INT
  }
 #endif//!ENABLE_SPI_INTERNAL_ISRS

  // Wait until prior transfer completes before starting new one.
  // N.B. If prior transaction was KEEP_SEL, that information is still
  // available in SCx_OPFLAGS(port) and respected by halSpiMasterIsBusy().
  // For an async operation, tell caller we can't start new transfer now.
  halResetWatchdog();
  while (halSpiMasterIsBusy(port)) {
    if ((opFlags & EMBER_SPI_OP_ASYNC_IO) != 0) {
      return EMBER_PHY_TX_BUSY; // Closest error to indicate busy device
    }
    // spin -- this should be quick enough to finish long before watchdog barks
  }

  SCx_OPFLAGS(port) = opFlags; // New transaction

  // If only care about Rx, then use Rx buffer for Tx too
  if (txBuf == NULL) {
    txBuf = rxBuf;
    //FIXME: Need to clear the rxBuf or allow exposure of stale data?
    //MEMSET(txBuf, SPI_PAD_BYTE, trLen);
  }

  SPIDBG(
    emberSerialGuaranteedPrintf(SER232, "{SpiM-Tx:");
    if (spLen > 0) {
      uint16_t i;
      for (i = 0; i < spLen; i++) {
        emberSerialGuaranteedPrintf(SER232, " %x", txBuf[i]);
      }
      emberSerialGuaranteedPrintf(SER232, " |");
      for (i = 0; i < trLen - spLen; i++) {
        emberSerialGuaranteedPrintf(SER232, " %x", rxBuf[i]);
      }
    } else {
      uint16_t i;
      for (i = 0; i < trLen; i++) {
        emberSerialGuaranteedPrintf(SER232, " %x", txBuf[i]);
      }
    }
    emberSerialGuaranteedPrintf(SER232, "}\r\n");
  )

  halInternalSpiMasterSelectSlave(port); // No-op if prev transfer left slave selected

 #if     (ENABLE_SPI_SC > 0)
  if (port != EMBER_SPI_PORT_BITBANG) {
    // Flush any junk a Tx-only transfer(s) left behind in the 4-byte Rx FIFO
    // This also gives the slave a bit of time after being selected before we
    // start clocking data.
   #if 1 //FIXME: Is it better to do this:
    (void) SCx_REG(port, DATA);
    (void) SCx_REG(port, DATA);
    (void) SCx_REG(port, DATA);
    (void) SCx_REG(port, DATA);
   #else //FIXME: or would this:
    SCx_REG(port, MODE) = SC2_MODE_DISABLED; // Disable the port
    SCx_REG(port, MODE) = SC2_MODE_SPI;      // Re-enable the port
   #endif//FIXME: be as effective and faster with no other side effects?
  }
 #endif//(ENABLE_SPI_SC > 0)

  if ( (trLen < halSpiMasterDmaThreshold)   // Small transfer
     ||(port == EMBER_SPI_PORT_BITBANG)     // SC0 bitbang
     ||(opFlags & EMBER_SPI_OP_NO_DMA)      // Specified non-DMA
     ||(((uint32_t) txBuf) < RAM_BOTTOM)      // txBuf is outside
     ||(((uint32_t) txBuf) > RAM_TOP)         //   of RAM bounds
     ||( (rxBuf != NULL)                    // rxBuf exists and
       &&( (((uint32_t) rxBuf) < RAM_BOTTOM)  // rxBuf is outside
         ||(((uint32_t) rxBuf) > RAM_TOP) ) ) //   of RAM bounds
     ) {
    // For efficiency with small transfers, or when using BitBang port,
    // use non-DMA synchronous approach, regardless of EMBER_SPI_OP_ASYNC_IO
    uint16_t i;
    if (spLen > 0) {
      // For a split transaction:
      // First use txBuf for Tx+Rx of spLen
      // Then  use rxBuf for Tx+Rx of remainder
      for (i = 0; i < spLen; i++) {
        txBuf[i] = halSpiMasterTransferByte(port, txBuf[i]);
      }
      for (i = 0; i < trLen - spLen; i++) {
        if (opFlags & EMBER_SPI_OP_SPLIT_TRX) {
          (void) halSpiMasterTransferByte(port, rxBuf[i]);
        } else {
          rxBuf[i] = halSpiMasterTransferByte(port, rxBuf[i]);
        }
      }
    } else {
      // For a normal transaction:
      // Use txBuf for Tx and optionally rxBuf for Rx
      for (i = 0; i < trLen; i++) {
        if (rxBuf == NULL) {
          (void) halSpiMasterTransferByte(port, txBuf[i]);
        } else {
          rxBuf[i] = halSpiMasterTransferByte(port, txBuf[i]);
        }
      }
    }
  } else {
   #if     (ENABLE_SPI_SC > 0)
    resetSpi(port);

    if (spLen > 0) {
      // For a split transaction:
      // First use txBuf for  Tx+ Rx of spLen     as DMA A-buffers
      // Then  use rxBuf for [Tx+]Rx of remainder as DMA B-buffers
      SCx_REG(port, TXBEGA)  = (uint32_t)  txBuf;
      SCx_REG(port, RXBEGA)  = (uint32_t)  txBuf;
      SCx_REG(port, TXENDA)  = (uint32_t) (txBuf + spLen - 1); // END is inclusive
      SCx_REG(port, RXENDA)  = (uint32_t) (txBuf + spLen - 1); // END is inclusive
      SCx_REG(port, TXBEGB)  = (uint32_t)  rxBuf;
      SCx_REG(port, RXBEGB)  = (uint32_t)  rxBuf;
      SCx_REG(port, TXENDB)  = (uint32_t) (rxBuf + trLen - spLen - 1); // END is inclusive
      SCx_REG(port, RXENDB)  = (uint32_t) (rxBuf + trLen - spLen - 1); // END is inclusive
      SCx_REG(port, DMACTRL) = ( SC_TXLODA | SC_RXLODA | SC_TXLODB
                               | ((opFlags & EMBER_SPI_OP_SPLIT_TRX) ? 0 : SC_RXLODB) );
    } else {
      // For a normal transaction:
      // Use txBuf for Tx and optionally rxBuf for Rx as DMA A-buffers
      SCx_REG(port, TXBEGA) = (uint32_t)  txBuf;
      SCx_REG(port, TXENDA) = (uint32_t) (txBuf + trLen - 1); // END is inclusive
      if (rxBuf == NULL) { // Tx only
        // To do a Tx, can just do a Tx DMA and let Rx side overrun
        // We'll clear out the overrun when done.
        SCx_REG(port, DMACTRL) = (SC_TXLODA);
      } else { // Rx only or Tx+Rx
        // To do a Rx, need to actually do both a Tx and Rx DMA
        // in order to get the right number of SCLK pulses sent.
        // Can use the same buffer for both as the SC's Tx and Rx
        // FIFO should avoid contention concerns.
        SCx_REG(port, RXBEGA)  = (uint32_t)  rxBuf;
        SCx_REG(port, RXENDA)  = (uint32_t) (rxBuf + trLen - 1); // END is inclusive
        SCx_REG(port, DMACTRL) = (SC_TXLODA | SC_RXLODA);
      }
    }

    // If caller requested an ASYNC_IO transfer, we're done -- it's up to them
    // (or us in next transfer) to poll halSpiMasterIsBusy() for completion, or
    // be notified via the (*opDoneCB)() callback (which requires interrupts).
    if ((opFlags & EMBER_SPI_OP_ASYNC_IO) != 0) {
      if (opDoneCB != NULL) {
        SCx_CB(port) = opDoneCB;
        INT_SCxCFG(port) = INT_SCTXIDLE; // interrupt that signals completion
        INT_CFGSET = INT_SCx(port); // Enable top-level interrupt
      }
      return EMBER_SUCCESS;
    }
   #endif//(ENABLE_SPI_SC > 0)
  }

  // Synchronous transfer -- wait for it to complete
  halResetWatchdog();
  while (halSpiMasterIsBusy(port)) {
    // spin -- this should be quick enough to finish long before watchdog barks
  }
  SPIDBG(
    if (spLen > 0) {
      uint16_t i;
      emberSerialGuaranteedPrintf(SER232, "{SpiM-Rx:");
      for (i = 0; i < spLen; i++) {
        emberSerialGuaranteedPrintf(SER232, " %x", txBuf[i]);
      }
      emberSerialGuaranteedPrintf(SER232, " |");
      if ((opFlags & EMBER_SPI_OP_SPLIT_TRX) == 0) {
        for (i = 0; i < trLen - spLen; i++) {
          emberSerialGuaranteedPrintf(SER232, " %x", rxBuf[i]);
        }
      }
      emberSerialGuaranteedPrintf(SER232, "}\r\n");
    } else
    if (rxBuf != NULL) {
      uint16_t i;
      emberSerialGuaranteedPrintf(SER232, "{SpiM-Rx:");
      for (i = 0; i < trLen; i++) {
        emberSerialGuaranteedPrintf(SER232, " %x", rxBuf[i]);
      }
      emberSerialGuaranteedPrintf(SER232, "}\r\n");
    }
  )
  // halSpiMasterIsBusy() has deselected slave if opFlags allowed
  if (opDoneCB != NULL) {
    (*opDoneCB)(port, trLen, rxBuf);
  }

  return EMBER_SUCCESS;
}

#endif//ENABLE_SPI_SC // Driver is enabled
