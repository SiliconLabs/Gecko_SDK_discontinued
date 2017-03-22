/** @file hal/micro/cortexm3/ezsp-spi-serial-controller-autoselect.h
 *  @brief  Automatic serial controller selection for EZSP SPI EM3XX NCP
 *
 * The old default for an EZSP SPI NCP was to use SC2, however some newer
 * variants don't have SC2 (or some of the extra required GPIO). When this is
 * the case, this file will define EZSP_SPI_PORT as 1 so the implementation will
 * use SC1.
 *
 * <!-- Author(s): Jerome Issa -->
 * <!-- Copyright 2014 Silicon Labs, Inc.                                *80*-->
 */
//The maximum SPI Protocol message size is 136 bytes. We define a buffer of
//142 specifically for error detection during the Response Section.  By using
//a buffer of 142, we can use the SCx_TXCNT register to monitor the state of
//the transaction and know that if a DMA TX unload occurs we have an error.
#define SPIP_BUFFER_SIZE             142
#define SPIP_ERROR_RESPONSE_SIZE     2
#define MAX_PAYLOAD_FRAME_LENGTH     133
#define EZSP_LENGTH_INDEX            1
#define RX_DMA_BYTES_LEFT_THRESHOLD  4

 #ifndef EZSP_SPI_PORT
  #if EMBER_MICRO_SERIAL_CONTROLLERS_MASK == 1 // only has SC1
    #define EZSP_SPI_PORT 1
  #elif EMBER_MICRO_SERIAL_CONTROLLERS_MASK == 2 // only has SC2
    #define EZSP_SPI_PORT 2
  #else // has multiple serial controllers (or only 3 or 4, which we don't handle)
    // test that the micro has all of the GPIO required for port 2.  this is
    // the default for backwards compatibility reasons
    #if (EMBER_MICRO_PORT_A_GPIO & PA0)    \
        && (EMBER_MICRO_PORT_A_GPIO & PA1) \
        && (EMBER_MICRO_PORT_A_GPIO & PA2) \
        && (EMBER_MICRO_PORT_A_GPIO & PA3) \
        && (EMBER_MICRO_PORT_B_GPIO & PB2) \
        && (EMBER_MICRO_PORT_B_GPIO & PB3) \
        && (EMBER_MICRO_PORT_B_GPIO & PB6) \
        && (!PHY_DUAL)     /* DUAL  reserves SC2 so EZSP must use SC1 */ \
        && (!PHY_PRO2PLUS) /* PRO2+ reserves SC2 so EZSP must use SC1 */
      #define EZSP_SPI_PORT 2
    #else
      #define EZSP_SPI_PORT 1
    #endif
  #endif
#endif
// only serial controllers 1 through 4 are valid
#if EZSP_SPI_PORT < 1 || EZSP_SPI_PORT > 4
  #error "EZSP_SPI_PORT out of range"
#endif
// only serial controllers actually present on the micro are valid
#if (EMBER_MICRO_SERIAL_CONTROLLERS_MASK & (1 << (EZSP_SPI_PORT - 1))) == 0
  #error "EZSP_SPI_PORT specifies an invalid serial controller for this micro"
#endif
#undef  SCx_REG // Need to obliterate the micro-common.h UART-focused one
#if EZSP_SPI_PORT == 1
  // Pin assignments - uses SC1, IRQA or IRQB, IRQC
  // PB0 - nWAKE (IRQA) [except PHY_DUAL / PHY_PRO2PLUS]
  // PB1 - MISO
  // PB2 - MOSI
  // PB3 - SCLK
  // PB4 - nSSEL (IRQC)
  // PB6 - nWAKE (IRQB) [only PHY_DUAL / PHY_PRO2PLUS]
  // PC7 - nHOST_INT
  // serial controller abstraction
  #define SCx_REG(reg) SC1_##reg
  #define SCx_MODE(mode) SC1_MODE_##mode
  #define INT_SCxCFG INT_SC1CFG
  #define INT_SCxFLAG INT_SC1FLAG
  #define INT_SCx INT_SC1
  #define SCx_ISR halSc1Isr
  // serial controller GPIO configuration
  // configure all the pins in one register at once to avoid glitches as much
  // as possible
  #define CFG_SPI_GPIO()                                              \
    do {                                                              \
      GPIO_PBCFGL = ( (GPIO_PBCFGL & PB0_CFG_MASK)     |              \
                      (GPIOCFG_OUT_ALT << PB1_CFG_BIT) |  /* MISO */  \
                      (GPIOCFG_IN      << PB2_CFG_BIT) |  /* MOSI */  \
                      (GPIOCFG_IN      << PB3_CFG_BIT) ); /* SCLK */  \
      GPIO_PBCFGH = ( (GPIO_PBCFGH & (~PB4_CFG_MASK))  |              \
                      (GPIOCFG_IN      << PB4_CFG_BIT) ); /* nSSEL */ \
    } while(0)
  // SC1nSSEL:  PB4, configured as a input with pullup
  // nSSEL interrupt: IRQC (assigned to pin PB4, rising edge with no filtering)
  #define PULLUP_nSSEL()      (GPIO_PBSET = PB4)
  #define nSSEL_IS_NEGATED()  ((GPIO_PBIN & PB4) != 0)
  #define nSSEL_IS_ASSERTED() ((GPIO_PBIN & PB4) == 0)
  #define nSSEL_IRQSEL        GPIO_IRQCSEL
  #define nSSEL_IRQSEL_MASK   12
  #define nSSEL_INTCFG        GPIO_INTCFGC
  #define nSSEL_INT           INT_IRQC
  #define nSSEL_GPIOFLAG      INT_IRQCFLAG
  #define nSSEL_ISR           halIrqCIsr
  // nHOST_INT: PC7, configured as a push-pull output
  #define CFG_nHOST_INT(cfg)  do { uint32_t temp;                         \
                                   temp = GPIO_PCCFGH & ~PC7_CFG_MASK;  \
                                   temp |= cfg << PC7_CFG_BIT;          \
                                   GPIO_PCCFGH = temp; } while (0)
  #define SET_nHOST_INT()     do {GPIO_PCSET = PC7;} while (0)
  #define CLR_nHOST_INT()     do {GPIO_PCCLR = PC7;} while (0)
 
  // nSSEL_INT is not used on this serial controller
  #define CFG_nSSEL_INT(cfg)  do {} while (0)
 
 #if     (PHY_DUAL || PHY_PRO2PLUS)
  // nWAKE: PB6, configured as input with a pull-up
  // nWAKE interrupt: IRQB (fixed at pin PB6, falling edge with no filtering)
  #define CFG_nWAKE(cfg)      do { uint32_t temp;                         \
                                   temp = GPIO_PBCFGH & ~PB6_CFG_MASK;  \
                                   temp |= cfg << PB6_CFG_BIT;          \
                                   GPIO_PBCFGH = temp; } while (0)
  #define PULLUP_nWAKE()      (GPIO_PBSET = PB6)
  #define nWAKE_IS_NEGATED()  ((GPIO_PBIN & PB6) != 0)
  #define nWAKE_IS_ASSERTED() ((GPIO_PBIN & PB6) == 0)
  #define nWAKE_INTCFG        GPIO_INTCFGB
  #define nWAKE_INT           INT_IRQB
  #define nWAKE_GPIOFLAG      INT_IRQBFLAG
  #define nWAKE_ISR           halIrqBIsr
 #else//!(PHY_DUAL || PHY_PRO2PLUS)
  // nWAKE: PB0, configured as input with a pull-up
  // nWAKE interrupt: IRQA (fixed at pin PB0, falling edge with no filtering)
  #define CFG_nWAKE(cfg)      do { uint32_t temp;                         \
                                   temp = GPIO_PBCFGL & ~PB0_CFG_MASK;  \
                                   temp |= cfg << PB0_CFG_BIT;          \
                                   GPIO_PBCFGL = temp; } while (0)
  #define PULLUP_nWAKE()      (GPIO_PBSET = PB0)
  #define nWAKE_IS_NEGATED()  ((GPIO_PBIN & PB0) != 0)
  #define nWAKE_IS_ASSERTED() ((GPIO_PBIN & PB0) == 0)
  #define nWAKE_INTCFG        GPIO_INTCFGA
  #define nWAKE_INT           INT_IRQA
  #define nWAKE_GPIOFLAG      INT_IRQAFLAG
  #define nWAKE_ISR           halIrqAIsr
 #endif//(PHY_DUAL || PHY_PRO2PLUS)
  // ENABLE_NSIMRST is not compatible with SC1
#elif EZSP_SPI_PORT == 2
  // Pin assignments - uses SC2, IRQB, IRQC
  // PA0 - MOSI
  // PA1 - MISO
  // PA2 - SCLK
  // PA3 - nSSEL (IRQC)
  // PB2 - nHOST_INT
  // PB3 - nSSEL_INT (for EM260 compatibility - not used by EM35X)
  // PB6 - nWAKE (IRQB)
  // PB0 - nSIMRST (IRQA replaces nRESET for debugging only)
  // serial controller abstraction
  #define SCx_REG(reg) SC2_##reg
  #define SCx_MODE(mode) SC2_MODE_##mode
  #define INT_SCxCFG INT_SC2CFG
  #define INT_SCxFLAG INT_SC2FLAG
  #define INT_SCx INT_SC2
  #define SCx_ISR halSc2Isr
  // serial controller GPIO configuration
  // To avoid glitches, refresh configuration for all 4 pins in PACFGL at once.
  #define CFG_SPI_GPIO()                                              \
    do {                                                              \
      GPIO_PACFGL = ( (GPIOCFG_IN      << PA0_CFG_BIT) |  /* MOSI */  \
                      (GPIOCFG_OUT_ALT << PA1_CFG_BIT) |  /* MISO */  \
                      (GPIOCFG_IN      << PA2_CFG_BIT) |  /* SCLK */  \
                      (GPIOCFG_IN_PUD  << PA3_CFG_BIT) ); /* nSSEL */ \
    } while(0)
  // SC2nSSEL:  PA3, configured as a input with pullup
  // nSSEL interrupt: IRQC (assigned to pin PA3, rising edge with no filtering)
  #define nSSEL_BIT           3
  #define PULLUP_nSSEL()      (GPIO_PASET = (1 << nSSEL_BIT))
  #define nSSEL_IS_NEGATED()  ((GPIO_PAIN & (1 << nSSEL_BIT)) != 0)
  #define nSSEL_IS_ASSERTED() ((GPIO_PAIN & (1 << nSSEL_BIT)) == 0)
  #define nSSEL_IRQSEL        GPIO_IRQCSEL
  #define nSSEL_IRQSEL_MASK   3
  #define nSSEL_INTCFG        GPIO_INTCFGC
  #define nSSEL_INT           INT_IRQC
  #define nSSEL_GPIOFLAG      INT_IRQCFLAG
  #define nSSEL_ISR           halIrqCIsr
  // nHOST_INT: PB2, configured as a push-pull output
  #define nHOST_INT_BIT       2
  #define CFG_nHOST_INT(cfg)  do { uint32_t temp;                         \
                                   temp = GPIO_PBCFGL & ~PB2_CFG_MASK;  \
                                   temp |= cfg << PB2_CFG_BIT;          \
                                   GPIO_PBCFGL = temp; } while (0)
  #define SET_nHOST_INT()     do {GPIO_PBSET = (1 << nHOST_INT_BIT);} while (0)
  #define CLR_nHOST_INT()     do {GPIO_PBCLR = (1 << nHOST_INT_BIT);} while (0)
 
  // nSSEL_INT: PB3, configured as a floating input
  // For EM260 compatibility, can be connected to nSSEL; it is unused on the 35x
  #define nSSEL_INT_BIT       3
  #define CFG_nSSEL_INT(cfg)  do { uint32_t temp;                         \
                                   temp = GPIO_PBCFGL & ~PB3_CFG_MASK;  \
                                   temp |= cfg << PB3_CFG_BIT;          \
                                   GPIO_PBCFGL = temp; } while (0)
 
  // nWAKE: PB6, configured as input with a pull-up
  // nWAKE interrupt: IRQB (fixed at pin PB6, falling edge with no filtering)
  #define nWAKE_BIT           6
  #define CFG_nWAKE(cfg)      do { uint32_t temp;                         \
                                   temp = GPIO_PBCFGH & ~PB6_CFG_MASK;  \
                                   temp |= cfg << PB6_CFG_BIT;          \
                                   GPIO_PBCFGH = temp; } while (0)
  #define PULLUP_nWAKE()      (GPIO_PBSET = (1 << nWAKE_BIT))
  #define nWAKE_IS_NEGATED()  ((GPIO_PBIN & (1 << nWAKE_BIT)) != 0)
  #define nWAKE_IS_ASSERTED() ((GPIO_PBIN & (1 << nWAKE_BIT)) == 0)
  #define nWAKE_INTCFG        GPIO_INTCFGB
  #define nWAKE_INT           INT_IRQB
  #define nWAKE_GPIOFLAG      INT_IRQBFLAG
  #define nWAKE_ISR           halIrqBIsr
  // For debugging, use PB0/IRQA in place of nRESET.  Use the the board file
  // dev0680spi-test.h to enable this functionality (the board file configures
  // the gpio and defines ENABLE_NSIMRST).
  #ifdef ENABLE_NSIMRST
    // nSIMRST: PB0, configured as input with a pull-up
    // nSIMRST interrupt: IRQA (fixed at pin PB0, falling edge with no filtering)
    #define nSIMRST_BIT         0
    #define CFG_nSIMRST(cfg)    do { uint32_t temp;                         \
                                     temp = GPIO_PBCFGL & ~PB0_CFG_MASK;  \
                                     temp |= cfg << PB0_CFG_BIT;          \
                                     GPIO_PBCFGL = temp; } while (0)
    #define PULLUP_nSIMRST()    (GPIO_PBSET = (1 << nSIMRST_BIT))
    #define nSIMRST_IS_HIGH     ((GPIO_PBIN & (1 << nSIMRST_BIT)) != 0)
    #define nSIMRST_IS_LOW      ((GPIO_PBIN & (1 << nSIMRST_BIT)) == 0)
    #define nSIMRST_INTCFG      GPIO_INTCFGA
    #define nSIMRST_INT         INT_IRQA
    #define nSIMRST_GPIOFLAG    INT_IRQAFLAG
  #endif
#elif EZSP_SPI_PORT == 3
  // TODO
  // serial controller abstraction
  #define SCx_REG(reg) SC3_##reg
  #define SCx_MODE(mode) SC3_MODE_##mode
  #define INT_SCxCFG INT_SC3CFG
  #define INT_SCxFLAG INT_SC3FLAG
  #define INT_SCx INT_SC3
 
  #error "ESPI_SPI_PORT == 3 is not fully implemented yet"
#elif EZSP_SPI_PORT == 4
  // TODO
  // serial controller abstraction
  #define SCx_REG(reg) SC4_##reg
  #define SCx_MODE(mode) SC4_MODE_##mode
  #define INT_SCxCFG INT_SC4CFG
  #define INT_SCxFLAG INT_SC4FLAG
  #define INT_SCx INT_SC4
  #error "ESPI_SPI_PORT == 4 is not fully implemented yet"
#endif
