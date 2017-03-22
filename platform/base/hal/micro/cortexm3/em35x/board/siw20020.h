/** @file hal/micro/cortexm3/em35x/board/siw20020.h
 * See @ref board for detailed documentation.
 *
 * <!-- Copyright 2016 Silicon Laboratories, Inc.                        *80*-->
 */

/** @addtogroup board
 *  @brief Functions and definitions specific to the breakout board.
 *
 * The pcb0663 RCM on dev0680 BoB has the following example GPIO configuration.
 * This board file and the default HAL setup reflects the *'d configuration.
 * Pin - Signal       Use               PwrUpCfg  PwrDnCfg #define option
 * PA0 * SC2MOSI(mst) PRO2_SDI          OUT_ALTv  IN_PUD^
 *    `- USBDM        USBDM             INv       INv      CORTEXM3_EM35X_USB && !EM_SERIAL3_DISABLED
 * PA1 * SC2MISO(mst) PRO2_SDO2         INv       IN_PUD^
 *    `- USBDP        USBDP             INv       INv      CORTEXM3_EM35X_USB && !EM_SERIAL3_DISABLED
 * PA2 * SC2SCLK(mst) PRO2_SCLK2        OUT_ALTv  IN_PUD^
 *    `- GPIO         ENUMCTRL          OUTv      OUTv     CORTEXM3_EM35X_USB && !EM_SERIAL3_DISABLED
 * PA3 * SC2nSEL(mst) PRO2_nSEL2        OUT^      OUT^
 *    `- GPIO         VBUSMON           INv       INv      CORTEXM3_EM35X_USB && !EM_SERIAL3_DISABLED
 * PA4 - LED0         MCM_PA4           OUT^      OUT^
 *    `* PTI_EN       MCM_PA4           OUT_ALTv  IN_PUDv  PACKET_TRACE
 * PA5 - GPIO         PRO2_GPIO3(PTI)   INv       INv
 *    `* PTI_DATA     MCM_PA5           OUT_ALT^  IN_PUD^  PACKET_TRACE
 * PA6 * SDN          PRO2_SDN2         OUTv      OUT^
 * PA7 * nIRQ         PRO2_nIRQ         IN^       IN_PUD^
 * PB0 - nWAKE/IRQA   HOST_nWAKE        IN_PUD^   IN_PUD^W
 *    `- INV_CTS      PRO2_GPIO1        IN_PUD^   IN_PUDv  EXT_DEVICE_RDY
 *    `* FEM_CSD      FEM_CSD           OUT^      OUTv     (PHY_DUAL on PCB0663)
 * PB1 * SC1TXD       HOST_TXD          OUT_ALT^  OUT^
 * PB2 * SC1RXD       HOST_RXD          IN_PUD^   IN_PUD^W
 * PB3 * SC1nCTS      HOST_nCTS         IN_PUD^   IN_PUDv
 * PB4 * SC1nRTS      HOST_nRTS         OUT_ALTv  OUT^
 * PB5 - TEMP_SENSE   MCM_PB5           ANALOGv   IN_PUDv
 *    `* FEM_CPS      FEM_CPS           OUT^      OUTv     (PHY_DUAL on PCB0663)
 * PB6 * BUTTON0/IRQB MCM_PB6           IN_PUD^   IN_PUD^W non-EZSP_SPI
 *     * nWAKE/IRQB   HOST_nWAKE        IN_PUD^   IN_PUD^W EZSP_SPI
 *    `* nRHO         nRHO              IN_PUD^   IN_PUD^W RHO_GPIO+RADIO_HOLDOFF non-EZSP_SPI
 * PB7 * Buzzer       MCM_PB7           OUT_ALTv  IN_PUDv
 *    `- EEPROM_SDN   MCM_PB7           OUTv      OUTv     EEPROM_USES_SHUTDOWN_CONTROL
 * PC0 * JRST         MCM_PC0/JRST      INv       IN_PUD^
 * PC1 * LED1         MCM_PC1           OUT^      OUT^
 * PC2 * SWO          MCM_PC2/JTDO      OUT_ALT^  OUT^
 * PC3 * JTDI         MCM_PC3/JTDI      INv       IN_PUDv
 * PC4 * SWDIO        MCM_PC4/SWDIO     INv       IN_PUDv
 * PC5 * LED2         -n/c-             OUT^      OUT^     non-PHY_DUAL
 *    `* TX_ACTIVE    FEM_CTX           OUT_ALTv  OUTv     PHY_DUAL ENABLE_ALT_FUNCTION_TX_ACTIVE
 * PC6 * BUTTON1      MCM_PC6           IN_PUD^   IN_PUD^W
 *    `- nTX_ACTIVE   FEM_CRX           OUT_ALTv  OUTv     ENABLE_ALT_FUNCTION_NTX_ACTIVE
 *    `- OSC32B       OSC32K_CRYSTAL    ANALOGv   ANALOGv  ENABLE_OSC32K=OSC32K_CRYSTAL
 * PC7 * nINT/TEMP_EN HOST_nINT/MCM_PC7 OUT^      OUT^
 *    `- OSC32A       OSC32K_CRYSTAL    ANALOGv   ANALOGv  ENABLE_OSC32K=OSC32K_CRYSTAL
 *    `- OSC32_EXT    OSC32K_DIGITAL    INv       INv      ENABLE_OSC32K=OSC32K_DIGITAL
 *
 * PRO2+ GPIOs:
 * Pin - Signal       Use               PwrUpCfg  PwrDnCfg #define option
 * GP0 * RX_STATE     T/R switch        RX_STATE  (same)
 * GP1 - CTS          CTS option        CTS       (same)
 * GP2 - TX_STATE     T/R switch        TX_STATE  (same)
 * GP3 - PTI          1-wire PTI        PKT_TRACE (same)
 * NIRQ- NIRQ         NIRQ              NIRQ      (same)
 * SDO - SDO          SDO               SDO       (same)
 * GEN_CONFIG         Drive Strength    LOW       (same)
 *@{
 */

#ifndef __BOARD_H__
#define __BOARD_H__


/** @name PRO2+ Interface Definitions
 *
 * The following provide the spi-master and ext-device driver configurations
 * for interfacing the PRO2+ transceiver.
 */
#if     SC0_SPI // Hack to permit testing SC0 bitbang using SC2 signals
#define SC0_NSEL        PORTB_PIN(5)
#define SC0_NSEL_PORT   B
#define SC0_NSEL_BIT    5
#define SC0_SCLK        PORTA_PIN(2)
#define SC0_SCLK_PORT   A
#define SC0_SCLK_BIT    2
#define SC0_MOSI        PORTA_PIN(3)
#define SC0_MOSI_PORT   A
#define SC0_MOSI_BIT    3
#define SC0_MISO        PORTA_PIN(4)
#define SC0_MISO_PORT   A
#define SC0_MISO_BIT    4
#endif//SC0_SPI

/** @brief PRO2+ SPI-Master configuration
 */
#define PRO2_SPI_PORT   2     // PRO2+ SPI  is on SC2
#define PRO2_SPI_MHZ    4     // PRO2+ SPI clock rate in MHz

#define SC2_NSEL PORTA_PIN(3) // PRO2+ nSEL is on PA3

#define ENABLE_SPI_SC PRO2_SPI_PORT // Enable SPI only for PRO2+ on SC2
//#define ENABLE_SPI_SC 10 // Enable SPI for ports in addition to PRO2+ on SC2

/** @brief PRO2+ Ext-Device configuration
 */
#define ENABLE_EXT_DEVICE       1
// SDN configuration
#define EXT_DEVICE_PWR          PORTA_PIN(6)
#define EXT_DEVICE_PWR_PORT     A
#define EXT_DEVICE_PWR_BIT      6
#define EXT_DEVICE_PWR_TRUE     0 // 0=low true, 1=high true
#if 0 //FIXME: NOTYET
// INV_CTS configuration
#define EXT_DEVICE_RDY          PORTB_PIN(0)
#define EXT_DEVICE_RDY_PORT     B
#define EXT_DEVICE_RDY_BIT      0
#define EXT_DEVICE_RDY_TRUE     0 // 0=low true, 1=high true
#define EXT_DEVICE_RDY_IRQ      A // A,B,C,D are the valid IRQs
#define EXT_DEVICE_RDY_IRQCFG   (GPIOINTMOD_FALLING_EDGE << GPIO_INTMOD_BIT)
#define EXT_DEVICE_RDY_IRQPRI   INTERRUPTS_DISABLED_PRIORITY
//FIXME: If enabled for CTS, need to subvert BUTTON1 definition
#endif//FIXME
// INT configuration
#define EXT_DEVICE_INT          PORTA_PIN(7)
#define EXT_DEVICE_INT_PORT     A
#define EXT_DEVICE_INT_BIT      7
#define EXT_DEVICE_INT_TRUE     0 // 0=low true, 1=high true
#define EXT_DEVICE_INT_IRQ      D // A,B,C,D are the valid IRQs
#define EXT_DEVICE_INT_IRQCFG   (GPIOINTMOD_LOW_LEVEL << GPIO_INTMOD_BIT)
#define EXT_DEVICE_INT_IRQPRI   INTERRUPTS_DISABLED_PRIORITY

// App Bootloader dataflash SPI/I2C assignment must not be same as PRO2+'s
#if     (PRO2_SPI_PORT == 2)
 #define SPI_FLASH_SC1 1 // Ensure dataflash drivers don't use SC2
#else//!(PRO2_SPI_PORT == 2)
 #undef  SPI_FLASH_SC1   // Ensure dataflash drivers use SC2
#endif//(PRO2_SPI_PORT == 2)
//@} //END OF PRO2+ INTERFACE DEFINITIONS


/** @name PRO2+ GPIO Configuration
 */
//TODO: Currently no public PRO2+ API headers exist, so extract useful
//TODO: CMD_GPIO_ definitions to allow for custom GPIO configurations.
#ifndef CMD_GPIO_PIN_CFG
#define CMD_GPIO_PIN_CFG 0x13
#define CMD_GPIO_PIN_CFG_ARG_GPIO_PULL_CTL_MASK 0x40
#define CMD_GPIO_PIN_CFG_ARG_GPIO_PULL_CTL_LSB 6
#define CMD_GPIO_PIN_CFG_ARG_GPIO_PULL_CTL_ENUM_PULL_DIS 0
#define CMD_GPIO_PIN_CFG_ARG_GPIO_PULL_CTL_ENUM_PULL_EN 1
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_MASK 0x3f
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_LSB 0
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_DONOTHING 0
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_TRISTATE 1
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_DRIVE0 2
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_DRIVE1 3
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_INPUT 4
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_32K_CLK 5
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_BOOT_CLK 6
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_DIV_CLK 7
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_CTS 8
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_INV_CTS 9
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_CMD_OVERLAP 10
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_SDO 11
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_POR 12
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_CAL_WUT 13
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_WUT 14
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_EN_PA 15
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_TX_DATA_CLK 16
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_RX_DATA_CLK 17
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_EN_LNA 18
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_TX_DATA 19
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_RX_DATA 20
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_RX_RAW_DATA 21
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_ANTENNA_1_SW 22
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_ANTENNA_2_SW 23
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_VALID_PREAMBLE 24
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_INVALID_PREAMBLE 25
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_SYNC_WORD_DETECT 26
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_CCA 27
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_IN_SLEEP 28
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_PKT_TRACE 29
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_BUFDIV_CLK 30
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_TX_RX_DATA_CLK 31
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_TX_STATE 32
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_RX_STATE 33
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_RX_FIFO_FULL 34
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_TX_FIFO_EMPTY 35
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_LOW_BATT 36
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_CCA_LATCH 37
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_HOPPED 38
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_HOP_TABLE_WRAP 39
#define CMD_GPIO_PIN_CFG_ARG_GEN_CONFIG_DRV_STRENGTH_MASK 0x60
#define CMD_GPIO_PIN_CFG_ARG_GEN_CONFIG_DRV_STRENGTH_LSB 5
#define CMD_GPIO_PIN_CFG_ARG_GEN_CONFIG_DRV_STRENGTH_ENUM_HIGH 0
#define CMD_GPIO_PIN_CFG_ARG_GEN_CONFIG_DRV_STRENGTH_ENUM_MED_HIGH 1
#define CMD_GPIO_PIN_CFG_ARG_GEN_CONFIG_DRV_STRENGTH_ENUM_MED_LOW 2
#define CMD_GPIO_PIN_CFG_ARG_GEN_CONFIG_DRV_STRENGTH_ENUM_LOW 3
#endif//CMD_GPIO_PIN_CFG

//TODO: Want to use PRO2_GPIO_3 from phy.h here but too painful to #include it
//TODO: So 0=none, 1=GPIO_0, 2=GPIO_1, 3=GPIO_2, 4=GPIO_3
#define PRO2_PTI_GPIO (1+3) // GPIO3 for PTI data
#define PRO2_POWERUP_GPIO_CFG()                                               \
                   8, /*Number of bytes in following PRO2+ command*/          \
                   CMD_GPIO_PIN_CFG,                                          \
        /*GPIO0*/  CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_RX_STATE,         \
        /*GPIO1*/  CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_CTS,              \
        /*GPIO2*/  CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_TX_STATE,         \
        /*GPIO3*/  CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_PKT_TRACE,        \
        /*NIRQ */  CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_DONOTHING,        \
        /*SDO  */  CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_DONOTHING,        \
        /*GEN  */ (CMD_GPIO_PIN_CFG_ARG_GEN_CONFIG_DRV_STRENGTH_ENUM_LOW      \
                   << CMD_GPIO_PIN_CFG_ARG_GEN_CONFIG_DRV_STRENGTH_LSB),      \
                   0, /*Number of bytes in following PRO2+ command*/          \
        /*(end of list)*/

// There's no need for a PRO2_POWERDOWN_GPIO_CFG because SDN will kill the PRO2+
//@} //END OF PRO2+ GPIO CONFIGURATION


/** @name ENABLE_OSC24M
 *
 * ENABLE_OSC24M specifies the system clock's high-speed crystal frequency,
 * in MHz, on em35x pins OSCA and OSCB.  If negated, it indicates that an
 * external digital or sine-wave signal is provided rather than being a real
 * crystal.  Default if not specified is 24 MHz, and that is the only
 * frequency valid for proper operation of the em35x's 2.4 GHz radio.
 * A value of 0 may be specified forcing the em35x to run only off its
 * internal high-speed RC (nominally 12 MHz, uncalibrated).
 */
//@{
/**
 * @brief This define specifies the system clock frequency in MHz.  The
 * default is 24 MHz.
 */
#ifndef ENABLE_OSC24M
#if     (defined(CORTEXM3_EM317) || defined(EMU))
#define ENABLE_OSC24M -1 // Sentinal non-Xtal value to select based on PRO2+ radio config
#endif//(defined(CORTEXM3_EM317) || defined(EMU))
#endif//ENABLE_OSC24M

/**
 * @brief Once the system clock has switched over at boot or wakeup
 * from the high-speed oscillator to the stable OSC24M, the high-speed
 * oscillator is normally calibrated to a nominal 12 MHz frequency.
 * Defining OSCHF_TUNE_VALUE will override that calibration.  The
 * high-speed oscillator can be tuned from nominal 12 MHz in roughly
 * 0.3 Mhz increments, ranging from (lowest-frequency ~7 Mhz) +15 to -16
 * (~17 MHz highest frequency).
 * @note WARNING: this setting should only be used if you really know
 * what you're doing!  Please leave it undefined otherwise!
 */
//#define OSCHF_TUNE_VALUE 0
//@} //END OF ENABLE_OSC24M DEFINITIONS


/** @name USE_BUTTON_RECOVERY and related Bootloader GPIO Definitions
 *
 * The following defines are used when building bootloaders to optionally
 * specify a recovery button / GPIO.
 *
 * @note These definitions are ignored for non-bootloader applications.
 */
//@{
/**
 * @brief This define does not equate to anything.  It is used as a
 * trigger to enable use of a button / GPIO to tell the bootloader
 * to enter recovery upon bootup.
 * Default is to not enable recovery bootloader functionality.
 */
#define USE_BUTTON_RECOVERY

/**
 * @brief The actual GPIO used to invoke the recovery bootloader.
 */
#define BUTTON_RECOVERY_PORT_PIN  PORTA_PIN(5)
/**
 * @brief The GPIO set-output method, used briefly to drive the signal
 * before sensing its true state.  Must match the port-pin assignment
 * above.
 */
#define BUTTON_RECOVERY_SET()     (GPIO_PASET = PA5)
/**
 * @brief The GPIO input method to sense whether the button is pressed.
 * Must match the port-pin assignment above.
 */
#define BUTTON_RECOVERY_PRESSED() ((GPIO_PAIN & PA5) ? false : true)
//@} //END OF USE_BUTTON_RECOVERY AND RELATED BOOTLOADER GPIO DEFINITIONS


/** @name Custom Baud Rate Definitions
 *
 * The following define is used with defining a custom baud rate for the UART.
 * This define provides a simple hook into the definition of
 * the baud rates used with the UART.  The baudSettings[] array in uart.c
 * links the BAUD_* defines with the actual register values needed for
 * operating the UART.  The array baudSettings[] can be edited directly for a
 * custom baud rate or another entry (the register settings) can be provided
 * here with this define.
 */
//@{
/**
 * @brief This define is the register setting for generating a baud of
 * 921600.  Refer to the EM35x datasheet's discussion on UART baud rates for
 * the equation used to derive this value.
 */
#define EMBER_SERIAL_BAUD_CUSTOM  13
//@} //END OF CUSTOM BAUD DEFINITIONS


/** @name LED Definitions
 *
 * The following are used to aid in the abstraction with the LED
 * connections.  The microcontroller-specific sources use these
 * definitions so they are able to work across a variety of boards
 * which could have different connections.  The names and ports/pins
 * used below are intended to match with a schematic of the system to
 * provide the abstraction.
 *
 * The ::HalBoardLedPins enum values should always be used when manipulating the
 * state of LEDs, as they directly refer to the GPIOs to which the LEDs are
 * connected.
 */
//@{

/**
 * @brief Assign each GPIO with an LED connected to a convenient name.
 * ::BOARD_ACTIVITY_LED and ::BOARD_HEARTBEAT_LED provide a further layer of
 * abstraction on top of the 3 LEDs for verbose coding.
 */
enum HalBoardLedPins {
  BOARDLED0 = PORTA_PIN(4),
  BOARDLED1 = PORTC_PIN(1),
  BOARDLED2 = PORTC_PIN(5),
  BOARDLED3 = BOARDLED2,
  BOARD_ACTIVITY_LED  = BOARDLED0,
  BOARD_HEARTBEAT_LED = BOARDLED1
};
/** @} END OF LED DEFINITIONS  */


/** @name Button Definitions
 *
 * The following are used to aid in the abstraction with the Button
 * connections.  The microcontroller-specific sources use these
 * definitions so they are able to work across a variety of boards
 * which could have different connections.  The names and ports/pins
 * used below are intended to match with a schematic of the system to
 * provide the abstraction.
 *
 * The BUTTONn macros should always be used with manipulating the buttons
 * as they directly refer to the GPIOs to which the buttons are connected.
 *
 * @note The GPIO number must match the IRQ letter
 */
//@{
/**
 * @brief The actual GPIO BUTTON0 is connected to.  This define should
 * be used whenever referencing BUTTON0.
 */
#ifdef  EZSP_SPI
// Cannot use BUTTON0 due to conflict with HOST_nWAKE
#undef  BUTTON0
#else//!EZSP_SPI
#define BUTTON0             PORTB_PIN(6)
#endif//EZSP_SPI
/**
 * @brief The GPIO input register for BUTTON0.
 */
#define BUTTON0_IN          GPIO_PBIN
/**
 * @brief Point the proper IRQ at the desired pin for BUTTON0.
 * @note IRQB is fixed and as such does not need any selection operation.
 */
#define BUTTON0_SEL()       do { } while(0)
/**
 * @brief The interrupt service routine for BUTTON0.
 */
#define BUTTON0_ISR         halIrqBIsr
/**
 * @brief The interrupt configuration register for BUTTON0.
 */
#define BUTTON0_INTCFG      GPIO_INTCFGB
/**
 * @brief The interrupt enable bit for BUTTON0.
 */
#define BUTTON0_INT_EN_BIT  INT_IRQB
/**
 * @brief The interrupt flag bit for BUTTON0.
 */
#define BUTTON0_FLAG_BIT    INT_IRQBFLAG
/**
 * @brief The missed interrupt bit for BUTTON0.
 */
#define BUTTON0_MISS_BIT    INT_MISSIRQB

/**
 * @brief The actual GPIO BUTTON1 is connected to.  This define should
 * be used whenever referencing BUTTON1, such as controlling if pieces
 * are compiled in.
 * Remember there may be other things that might want to use IRQC.
 */
#define BUTTON1             PORTC_PIN(6)
/**
 * @brief The GPIO input register for BUTTON1.
 */
#define BUTTON1_IN          GPIO_PCIN
/**
 * @brief Point the proper IRQ at the desired pin for BUTTON1.
 * Remember there may be other things that might want to use IRQC.
 * @note For this board, IRQC is pointed at PC6
 */
#define BUTTON1_SEL()       do { GPIO_IRQCSEL = PORTC_PIN(6); } while(0)
/**
 * @brief The interrupt service routine for BUTTON1.
 * Remember there may be other things that might want to use IRQC.
 */
#define BUTTON1_ISR         halIrqCIsr
/**
 * @brief The interrupt configuration register for BUTTON1.
 */
#define BUTTON1_INTCFG      GPIO_INTCFGC
/**
 * @brief The interrupt enable bit for BUTTON1.
 */
#define BUTTON1_INT_EN_BIT  INT_IRQC
/**
 * @brief The interrupt flag bit for BUTTON1.
 */
#define BUTTON1_FLAG_BIT    INT_IRQCFLAG
/**
 * @brief The missed interrupt bit for BUTTON1.
 */
#define BUTTON1_MISS_BIT    INT_MISSIRQC
//@} //END OF BUTTON DEFINITIONS


/** @name USB Power State
 *
 * Define if the USB is self powered or bus powered since the configuration
 * descriptor needs to report to the host the powered state.
 *
 * @note VBUS Monitoring is required for USB to function when the EM358 device
 * is configured as self-powered.
 */
//@{
/**
 * @brief The USB power state.
 *
 * Set the define USB_SELFPWRD_STATE:
 *   0 if the device is bus powered.
 *   1 if the device self powered.
 */
#ifdef USB_BUSPWRD
#define USB_SELFPWRD_STATE (0)
#else
#define USB_SELFPWRD_STATE (1)
#endif
//@}


/** @name USB Remote Wakeup Enable
 *
 * If the USB device needs to awake the host from suspend, then it needs
 * to have remote wakeup enable.
 *
 * @note The host can deny remote wakeup, keeping the device in suspend.
 *
 * If the device has remote wakeup enabled the configuration descriptor
 * needs to report this fact to the host.  Additionally, the USB core
 * in the chip needs to be directly told.  Set the define
 * USB_REMOTEWKUPEN_STATE to 0 if remote wake is disabled or 1 if enabled.
 */
//@{
/**
 * @brief USB Remote Wakeup Enable
 *
 * Set the define USB_REMOTEWKUPEN_STATE:
 *   0 remote wakeup is disabled.
 *   1 remote wakeup is enabled.
 */
#define USB_REMOTEWKUPEN_STATE (1)
//@}


/** @name USB Maximum Power Consumption
 *
 * The USB device must report the maximum power it will draw from the bus.
 * This is done via the bMaxPower parameter in the Configuration Descriptor
 * reported to the host.  The value used is in units of 2mA.
 *
 * Self-powered devices are low power devices and must draw less than 100mA.
 *
 * Systems that have components such as a FEM are likely to consume more than
 * 100mA and are considered high power and therefore must be bus-powered.
 */
//@{
/**
 * @brief USB Max Power parameter (bMaxPower) the driver will report to the
 * host in the Configuration Descriptor.
 */
#define USB_MAX_POWER (50)
//@}


/** @name USB Enumeration Control
 *
 * The following are used to aid in the abstraction of which GPIO is
 * used for controlloing the pull-up resistor for enumeation.
 *
 * The hardware setup connects the D+ signal to a GPIO via a 1.5kOhm
 * pull-up resistor.  Any GPIO can be used since it just needs to be a
 * simple push-pull output configuration.
 */
//@{
/**
 * @brief The actual GPIO ENUMCTRL is connected to.  The GPIO only needs to
 * be a simple push-pull output or input.
 */
#define ENUMCTRL  PORTA_PIN(2)
/**
 * @brief Set the GPIO's configuration to the provided state.  The two
 * states used are GPIOCFG_OUT when the device is enumerated and GPIOCFG_IN
 * when the device is not enumerated.
 */
#define ENUMCTRL_SETCFG(cfg) do { SET_REG_FIELD(GPIO_PACFGL, PA2_CFG, cfg); } while(0)
/**
 * @brief When the GPIO used for enumeration is configured as push-pull, this
 * macro makes it easy to set the output state high.
 */
#define ENUMCTRL_SET()       do { GPIO_PASET = PA2; } while(0)
/**
 * @brief When the GPIO used for enumeration is configured as push-pull, this
 * macro makes it easy to clear the output state low.
 */
#define ENUMCTRL_CLR()       do { GPIO_PACLR = PA2; } while(0)
//@} //USB Enumeration Control


/** @name USB VBUS Monitoring Support
 *
 * @note VBUS Monitoring is required for USB to function when the EM358 device
 * is configured as self-powered.
 *
 * The following are used to aid in the abstraction of which
 * GPIO and IRQ is used for VBUS Monitoring.
 *
 * Remember that IRQA and IRQB are fixed to GPIO PB0 and PB6 respectively
 * while IRQC and IRQD can be assigned to any GPIO.  Since USB's D- and D+
 * data pins are fixed to PA0 and PA1 respectively, SC2 can't be used so it
 * makes sense to allocate PA2 for enumeration control and PA3 for VBUS
 * monitoring.  Therefore, using PA3 for VBUS monitoring requires IRQC or IRQD.
 *
 * The driver will only try to use VBUSMON functionality if USB_SELFPWRD_STATE
 * is set to 1.
 */
//@{
/**
 * @brief The actual GPIO VBUSMON is connected to.  Remember that other
 * pieces might want to use PA3.
 *
 * Leaving VBUSMON undefined will keep VBUS Monitoring functionality
 * from being compiled in and not conflict with other pieces that
 * might want to use the GPIO or IRQ that VBUS Monitoring needs.
 */
#define VBUSMON  PA3
/**
 * @brief The GPIO input register for VBUSMON.
 */
#define VBUSMON_IN          GPIO_PAIN
/**
 * @brief The GPIO configuration needed for VBUSMON.  The configuration
 * needs to be a simple input that will monitor for edge tansitions.
 */
#define VBUSMON_SETCFG() do { SET_REG_FIELD(GPIO_PACFGL, PA3_CFG, GPIOCFG_IN); } while(0)
/**
 * @brief Point the proper IRQ at the desired pin for VBUSMON.  Remember that
 * other pieces that might want to use IRQC.
 * @note For this board, IRQC is pointed at PA3.
 */
#define VBUSMON_SEL()       do { GPIO_IRQDSEL = PORTA_PIN(3); } while(0)
/**
 * @brief The interrupt service routine for VBUSMON.  Remember that
 * other pieces that might want to use IRQC.
 */
#define VBUSMON_ISR         halIrqDIsr
/**
 * @brief The interrupt configuration register for VBUSMON.
 */
#define VBUSMON_INTCFG      GPIO_INTCFGD
/**
 * @brief The interrupt enable bit for VBUSMON.
 */
#define VBUSMON_INT_EN_BIT  INT_IRQD
/**
 * @brief The interrupt flag bit for VBUSMON.
 */
#define VBUSMON_FLAG_BIT    INT_IRQDFLAG
/**
 * @brief The missed interrupt bit for VBUSMON.
 */
#define VBUSMON_MISS_BIT    INT_MISSIRQD
//@} //USB VBUS Monitoring Support


/** @name Radio HoldOff
 *
 * When ::RADIO_HOLDOFF is defined, the GPIO configuration
 * will be initially setup by halInit() to enable Radio HoldOff
 * support on the designated RHO_GPIO as an input, replacing use
 * of that pin's default configuration.
 *
 * @note This define will override other settings for the RHO_GPIO.
 *
 * @note Radio HoldOff can also be enabled/disabled at runtime
 * via halSetRadioHoldOff().
 * The ::RADIO_HOLDOFF definition just controls the default
 * configuration at boot.
 */
//@{

/**
 * @brief This define does not equate to anything.  It is used as a
 * trigger to enable Radio HoldOff support.
 */
//#define RADIO_HOLDOFF  // Configure Radio HoldOff at bootup
//@} //END OF RADIO HOLDOFF

/** @name Radio HoldOff Configuration Definitions
 *
 * The following are used to aid in the abstraction with Radio
 * HoldOff (RHO).  The microcontroller-specific sources use these
 * definitions so they are able to work across a variety of boards
 * which could have different connections.  The names and ports/pins
 * used below are intended to match with a schematic of the system to
 * provide the abstraction.
 *
 * The Radio HoldOff input GPIO is abstracted like BUTTON0/1.
 */
//@{

/**
 * @brief The actual GPIO used to control Radio HoldOff.
 *
 * @note If ::RHO_GPIO is not defined, then Radio HoldOff
 * support will not be built in even for runtime use.
 */
#ifdef  EZSP_SPI
#undef  RHO_GPIO              // RHO conflicts with HOST_nWAKE on SPI NCP
#else//!EZSP_SPI
#define RHO_GPIO              PORTB_PIN(6) // RHO configured except for SPI NCP
#endif//EZSP_SPI
/**
 * @brief The GPIO signal level to assert Radio HoldOff (1=high, 0=low).
 */
#define RHO_ASSERTED          0
/**
 * @brief The GPIO configuration register for Radio HoldOff.
 */
#define RHO_CFG               GPIO_PBCFGH
/**
 * @brief The GPIO input register for Radio HoldOff.
 */
#define RHO_IN                GPIO_PBIN
/**
 * @brief The GPIO output register for Radio HoldOff.
 */
#define RHO_OUT               GPIO_PBOUT
/**
 * @brief Point the proper IRQ at the desired pin for Radio HoldOff.
 * Remember there may be other things that might want to use this IRQ.
 */
#define RHO_SEL()             do { /* IRQB fixed on PB6 */ } while(0)
/**
 * @brief The interrupt service routine for Radio HoldOff.
 * Remember there may be other things that might want to use this IRQ.
 */
#define RHO_ISR               halIrqBIsr
/**
 * @brief The interrupt configuration register for Radio HoldOff.
 */
#define RHO_INTCFG            GPIO_INTCFGB
/**
 * @brief The interrupt enable bit for Radio HoldOff.
 */
#define RHO_INT_EN_BIT        INT_IRQB
/**
 * @brief The interrupt flag bit for Radio HoldOff.
 */
#define RHO_FLAG_BIT          INT_IRQBFLAG
/**
 * @brief The missed interrupt bit for Radio HoldOff.
 */
#define RHO_MISS_BIT          INT_MISSIRQB

/** @brief Configuration of GPIO for Radio HoldOff operation
 */
#define WAKE_ON_DFL_RHO_FOR_RHO     false

/** @brief Configuration of GPIO for default behavior
 */
#define WAKE_ON_DFL_RHO_FOR_DFL     true

/** @brief The following definitions are helpers for managing
 *  Radio HoldOff and should not be modified.
 */
// GPIO configuration is the same for RHO and BUTTON0, simplifying things
  #define PWRUP_CFG_DFL_RHO           GPIOCFG_IN_PUD
  #define PWRUP_OUT_DFL_RHO           GPIOOUT_PULLUP
  #define PWRDN_CFG_DFL_RHO           GPIOCFG_IN_PUD
  #define PWRDN_OUT_DFL_RHO           GPIOOUT_PULLUP
#if     (defined(RADIO_HOLDOFF) && defined(RHO_GPIO))
  // Initial bootup configuration is for Radio HoldOff
  #define WAKE_ON_DFL_RHO             WAKE_ON_DFL_RHO_FOR_RHO
  #define halInternalInitRadioHoldOff() halSetRadioHoldOff(true)
#else//!(defined(RADIO_HOLDOFF) && defined(RHO_GPIO))
  // Initial bootup configuration is for default
  #define WAKE_ON_DFL_RHO             WAKE_ON_DFL_RHO_FOR_DFL
  #define halInternalInitRadioHoldOff() /* no-op */
#endif//(defined(RADIO_HOLDOFF) && defined(RHO_GPIO))

#ifdef  RHO_GPIO

 #ifdef  EZSP_SPI
  // EZSP_SPI does not build with button driver, so there is no RHO default
  #define RHO_INIT_FOR_DFL()          /* no-op */
 #else//!EZSP_SPI
  // Comment out RHO_ISR_FOR_DFL if not used, DO NOT #define it to be a no-op!
  // It will be called from RHO_ISR when RHO is not enabled and is expected to
  // acknowledge the interrupt instead of RHO_ISR acknowledging the interrupt.
  #define RHO_ISR_FOR_DFL             halIrq_FromRho
  #define RHO_INIT_FOR_DFL()          halInternalInitButton()
  #undef  BUTTON0_ISR   // Redefine to be invoked from RHO's ISR
  #define BUTTON0_ISR                 RHO_ISR_FOR_DFL
 #endif//EZSP_SPI

  #define WAKE_ON_DFL_RHO_VAR         halInternalWakeOnDflOrRho
  extern uint8_t WAKE_ON_DFL_RHO_VAR;
  #define ADJUST_GPIO_CONFIG_DFL_RHO(enableRadioHoldOff)  do {                \
          ATOMIC( /* Must read-modify-write so to be safe, use ATOMIC() */    \
            if (enableRadioHoldOff) { /* Radio HoldOff */                     \
              WAKE_ON_DFL_RHO_VAR = WAKE_ON_DFL_RHO_FOR_RHO;                  \
              RHO_INTCFG  = (0 << GPIO_INTFILT_BIT) /* 0 = no filter  */      \
                          | (3 << GPIO_INTMOD_BIT); /* 3 = both edges */      \
            } else { /* default */                                            \
              WAKE_ON_DFL_RHO_VAR = WAKE_ON_DFL_RHO_FOR_DFL;                  \
              RHO_INTCFG  = 0; /* disabled */                                 \
              RHO_INIT_FOR_DFL();                                             \
            }                                                                 \
            RHO_SEL(); /* Point IRQ at the desired pin */                     \
          )} while (0)

#else//!RHO_GPIO

  #define WAKE_ON_DFL_RHO_VAR         WAKE_ON_DFL_RHO_FOR_DFL

#endif//RHO_GPIO
//@} //END OF RADIO HOLDOFF CONFIGURATION DEFINITIONS


/** @name Temperature sensor ADC channel
 *
 * Define the analog input channel connected to the LM-20 temperature sensor.
 * The scale factor compensates for different platform input ranges.
 * PB5/ADC0 must be an analog input.
 * PC7 must be an output and set to a high level to power the sensor.
 *
 *@{
 */
/**
 * @brief The analog input channel to use for the temperature sensor.
 */
#define TEMP_SENSOR_ADC_CHANNEL ADC_SOURCE_ADC0_VREF2
/**
 * @brief The scale factor to compensate for different input ranges.
 */
#define TEMP_SENSOR_SCALE_FACTOR 1
/** @} END OF TEMPERATURE SENSOR ADC CHANNEL DEFINITIONS */


/** @name Packet Trace
 *
 * When ::PACKET_TRACE is defined, ::GPIO_PACFGH will automatically be setup by
 * halInit() to enable Packet Trace support on PA4 and PA5,
 * in addition to the configuration specified below.
 *
 * @note This define will override any settings for PA4 and PA5.
 */
//@{
/**
 * @brief This define does not equate to anything.  It is used as a
 * trigger to enable Packet Trace support.
 */
#define PACKET_TRACE  // We do have PACKET_TRACE support
//@} //END OF PACKET TRACE DEFINITIONS


/** @name ENABLE_OSC32K
 *
 * When ENABLE_OSC32K is defined to one of the below values, halInit()
 * will configure system timekeeping to utilize the chosen clock source:
 *   OSC32K_DISABLE - Use internal 10 kHz RC oscillator
 *       (This is the default if ENABLE_OSC32K is not defined at all)
 *   OSC32K_CRYSTAL - 32.768 kHz Crystal oscillator on PC6 and PC7
 *   OSC32K_SINE_1V - 32.768 kHz Sine wave 0-1V analog on PC7
 *   OSC32K_DIGITAL - 32.768 kHz Digital clock (0-Vdd square wave) on PC7
 *
 * @note ENABLE_OSC32K set to OSC32K_CRYSTAL is mutually exclusive with
 * ENABLE_ALT_FUNCTION_NTX_ACTIVE since they define conflicting
 * usage of GPIO PC6.
 */
//@{
/**
 * @brief This define specifies the external 32.768 kHz clock mode to use
 * for system time-keeping; e.g. OSC32K_CRYSTAL would specify using the
 * XTAL oscillator on the RCM plugged into the Breakout board (dev0680).
 * Default is to disable 32.768 kHz XTAL and use 1 kHz RC oscillator instead.
 */
#define ENABLE_OSC32K OSC32K_DISABLE

#ifndef OSC32K_CHOICES
  //Oops!  micro.h definitions needed for BOARD_HEADER option checks below
  #error Missing OSC32K_CHOICES definition -- include micro.h before BOARD_HEADER
#endif//OSC32K_CHOICES

/**
 * @brief On initial powerup the 32.768 kHz external clock clock could take
 * a little while to start stable oscillation. This only happens on initial
 * powerup, not on wake-from-sleep, since the clock usually stays running in
 * deep sleep mode.  After the clock is started, we delay for
 * OSC32K_STARTUP_DELAY_MS (time in milliseconds).  This delay allows the
 * external clock or crystal oscillator to stabilize before we start using it
 * for system timing.
 *
 * If you set OSC32K_STARTUP_DELAY_MS to less than the clock's startup time:
 *   - The system timer won't produce a reliable one millisecond tick before
 *     the clock is stable.
 *   - You may see some number of ticks of unknown period occur before the
 *     clock is stable.
 *   - halInit() will complete and application code will begin running, but
 *     any events based on the system timer will not be accurate until the
 *     clock is stable.
 *   - An unstable system timer will only affect the APIs in system-timer.h.
 *
 * Typical 32.768 kHz crystals measured by Ember take about 400 milliseconds
 * to stabilize. Be sure to characterize your particular crystal's or clock's
 * stabilization time since crystal behavior can vary.
 */
#define OSC32K_STARTUP_DELAY_MS (0)

#if OSC32K_STARTUP_DELAY_MS > MAX_INT16U_VALUE
  #error "OSC32K_STARTUP_DELAY_MS must fit in 16 bits."
#endif
//@} //END OF ENABLE OSC32K DEFINITIONS


/** @name ENABLE_ALT_FUNCTION_REG_EN
 *
 * When ENABLE_ALT_FUNCTION_REG_EN is defined, halInit() will enable the REG_EN
 * alternate functionality on PA7.  REG_EN is the special signal provided
 * by the EM35x's internal power controller which can be used to tell an
 * external power regulator when the EM35x is in deep sleep or not and as such
 * signal the external regulator to cut power.  This signal will override all
 * GPIO configuration and use of PA7.  When the alternate functionality is
 * not enabled, PA7 can be operated as a standard GPIO.
 */
//@{
/**
 * @brief This define does not equate to anything.  It is used as a
 * trigger to enable the REG_EN alternate function on PA7.
 * Default is to not enable REG_EN functionality on PA7.
 */
//#define ENABLE_ALT_FUNCTION_REG_EN
//@} //END OF ENABLE_ALT_FUNCTION_REG_EN DEFINITIONS


/** @name DISABLE_INTERNAL_1V8_REGULATOR
 *
 * When DISABLE_INTERNAL_1V8_REGULATOR is defined, the internal regulator for
 * the 1.8 V supply (VREG_1V8) is disabled.  Disabling of VREG_1V8 will
 * prevent excess current draw.
 *
 * The disabling occurs early in the halInit() sequence so that current
 * consumption is always minimized and the configuration will be applied
 * across all power modes.
 *
 * @note Disabling VREG_1V8 on devices that are not externally powered will
 * prevent the device from operating normally.
 *
 * @note Disabling the regulator will cause ADC readings of external signals
 * to be wrong.  These exteranl signals include analog sources ADC0 thru ADC5
 * and VDD_PADS/4.  Internal signals such as radio calibration will not be
 * affected.  Using the ADC requires re-enabling VREG_1V8.
 * 
 * Refer to the file micro-common.h for further description. 
 *
 */
//@{
/**
 * @brief This define does not equate to anything.  It is used as a
 * trigger for other code to disable the 1.8V regulator.
 */
//#define DISABLE_INTERNAL_1V8_REGULATOR
//@} //END OF DISABLE_INTERNAL_1V8_REGULATOR DEFINITION


/** @name ENABLE_ALT_FUNCTION_TX_ACTIVE
 *
 * When ENABLE_ALT_FUNCTION_TX_ACTIVE is defined, halInit() and halPowerUp()
 * will enable the TX_ACTIVE alternate functionality of PC5.  halPowerDown()
 * will configure PC5 to be a low output.  TX_ACTIVE can be used for
 * external PA power management and RF switching logic.  In transmit mode
 * the Tx baseband drives TX_ACTIVE high.  In receive mode the TX_ACTIVE
 * signal is low.  This define will override any settings for PC5.
 */
//@{
/**
 * @brief This define does not equate to anything.  It is used as a
 * trigger to enable the TX_ACTIVE alternate function on PC5.
 * Default is to not enable TX_ACTIVE functionality on PC5.
 */
#if     PHY_DUAL
  #define ENABLE_ALT_FUNCTION_TX_ACTIVE
#else//!PHY_DUAL
//#define ENABLE_ALT_FUNCTION_TX_ACTIVE
#endif//PHY_DUAL
//@} //END OF ENABLE_ALT_FUNCTION_TX_ACTIVE DEFINITIONS


/** @name ENABLE_ALT_FUNCTION_NTX_ACTIVE
 *
 * When ENABLE_ALT_FUNCTION_NTX_ACTIVE is defined, halInit() and halPowerUp()
 * will enable the nTX_ACTIVE alternate functionality of PC6.  halPowerDown()
 * will configure PC6 to be a low output.  nTX_ACTIVE can be used for
 * external PA power management and RF switching logic.  In transmit mode
 * the Tx baseband drives nTX_ACTIVE low.  In receive mode the nTX_ACTIVE
 * signal is high.  This define will override any settings for PC6.
 *
 * @note ENABLE_ALT_FUNCTION_NTX_ACTIVE is mutually exclusive with
 * ENABLE_OSC32K set to OSC32K_CRYSTAL since they define conflicting usage
 * of GPIO PC6.
 */
//@{
/**
 * @brief This define does not equate to anything.  It is used as a
 * trigger to enable the nTX_ACTIVE alternate function on PC6.
 * Default is to not enable nTX_ACTIVE functionality on PC6.
 */
//#define ENABLE_ALT_FUNCTION_NTX_ACTIVE
//@} //END OF ENABLE_ALT_FUNCTION_NTX_ACTIVE DEFINITIONS


/** @name EEPROM_USES_SHUTDOWN_CONTROL
 *
 * When EEPROM_USES_SHUTDOWN_CONTROL is defined, logic is enabled in the
 * EEPROM driver which drives PB7 high upon EEPROM initialization.  In
 * Ember reference designs, PB7 acts as an EEPROM enable pin and therefore
 * must be driven high in order to use the EEPROM.  This option is intended
 * to be enabled when running app-bootloader on designs based on current
 * Ember reference designs.
 */
//@{
/**
 * @brief This define does not equate to anything.  It is used as a
 * trigger to enable the logic that drives PB7 high in the EEPROM driver.
 * Default is to leave this logic disabled.
 */
//#define EEPROM_USES_SHUTDOWN_CONTROL

/**
 * @brief Give GPIO PB7 configuration a friendly name.
 */
#ifdef  EEPROM_USES_SHUTDOWN_CONTROL
  // Configure PB7 for EEPROM shutdown
  #define PWRUP_CFG_EEPROM  GPIOCFG_OUT
  #define PWRUP_OUT_EEPROM  0
  #define PWRDN_CFG_EEPROM  GPIOCFG_OUT
  #define PWRDN_OUT_EEPROM  0
#else//!EEPROM_USES_SHUTDOWN_CONTROL
  // Configure PB7 for buzzer
  #define PWRUP_CFG_EEPROM  GPIOCFG_OUT_ALT
  #define PWRUP_OUT_EEPROM  0
  #define PWRDN_CFG_EEPROM  GPIOCFG_IN_PUD
  #define PWRDN_OUT_EEPROM  GPIOOUT_PULLDOWN
#endif//EEPROM_USES_SHUTDOWN_CONTROL
//@} //END OF EEPROM_USES_SHUTDOWN_CONTROL DEFINITIONS


/** @name GPIO Configuration Definitions
 *
 * The following are used to specify the GPIO configuration to establish
 * when Powered (POWERUP_), and when Deep Sleeping (POWERDOWN_).  The reason
 * for separate Deep Sleep settings is to allow for a slightly different
 * configuration that minimizes power consumption during Deep Sleep.  For
 * example, inputs that might float could be pulled up or down, and output
 * states chosen with care, e.g. to turn off LEDs or other devices that might
 * consume power or be unnecessary when Deep Sleeping.
 */
//@{


/** @name Packet Trace Configuration Defines
 *
 * Provide the proper set of pin configuration for when the Packet
 * Trace is enabled (look above for the define which enables it).  When Packet
 * Trace is not enabled, leave the two PTI pins in their default configuration.
 * If Packet Trace is not being used, feel free to set the pin configurations
 * as desired.  The config shown here is simply the Power On Reset defaults.
 *@{
 */
/**
 * @brief Give the packet trace configuration a friendly name.
 */
#ifdef  PACKET_TRACE
  // Configure PA4 and PA5 for PTI
  #define PWRUP_CFG_PTI_EN    GPIOCFG_OUT_ALT
  #define PWRUP_OUT_PTI_EN    0
  #define PWRDN_CFG_PTI_EN    GPIOCFG_IN_PUD
  #define PWRDN_OUT_PTI_EN    GPIOOUT_PULLDOWN
  #define PWRUP_CFG_PTI_DATA  GPIOCFG_OUT_ALT
  #define PWRUP_OUT_PTI_DATA  1
  #define PWRDN_CFG_PTI_DATA  GPIOCFG_IN_PUD
  #define PWRDN_OUT_PTI_DATA  GPIOOUT_PULLUP
#else//!PACKET_TRACE
  // Configure PA4 and PA5 for LED, GPIO
  #define PWRUP_CFG_PTI_EN    GPIOCFG_OUT
  #define PWRUP_OUT_PTI_EN    1
  #define PWRDN_CFG_PTI_EN    GPIOCFG_OUT
  #define PWRDN_OUT_PTI_EN    1
  #define PWRUP_CFG_PTI_DATA  GPIOCFG_IN
  #define PWRUP_OUT_PTI_DATA  0
  #define PWRDN_CFG_PTI_DATA  GPIOCFG_IN
  #define PWRDN_OUT_PTI_DATA  0
#endif//PACKET_TRACE
//@} END OF Packet Trace Configuration Defines


/** @name 32kHz Oscillator and nTX_ACTIVE Configuration Defines
 *
 * Since the 32kHz Oscillator and nTX_ACTIVE both share PC6, their
 * configuration defines are linked and instantiated together.  Look above
 * for the defines that enable the 32kHz Oscillator and nTX_ACTIVE.
 *
 * @note ENABLE_OSC32K set to OSC32K_CRYSTAL is mutually exclusive with
 * ENABLE_ALT_FUNCTION_NTX_ACTIVE since they define conflicting
 * usage of GPIO PC6.
 *
 * When using the 32kHz, configure PC6 and PC7 according to the
 * ENABLE_OSC32K mode value.
 *
 * When using nTX_ACTIVE, configure PC6 for alternate output while awake
 * and a low output when deepsleeping.
 *
 * When not using the 32kHz or nTX_ACTIVE, configure PC6 and PC7 for
 * Button1 and TEMP_EN.
 *@{
 */
/**
 * @brief Give GPIO PC6 configuration a friendly name.
 */
#if (ENABLE_OSC32K == OSC32K_CRYSTAL) && defined(ENABLE_ALT_FUNCTION_NTX_ACTIVE)
  //Oops!  Only one of these can be used at a time!
  #error PC6 conflict: ENABLE_OSC32K set to OSC32K_CRYSTAL and\
 ENABLE_ALT_FUNCTION_NTX_ACTIVE are mutually exclusive.

#elif (ENABLE_OSC32K == OSC32K_CRYSTAL) && !defined(ENABLE_ALT_FUNCTION_NTX_ACTIVE)
  //Use OCS32K configuration
  #define PWRUP_CFG_BUTTON1  GPIOCFG_ANALOG
  #define PWRUP_OUT_BUTTON1  0
  #define PWRDN_CFG_BUTTON1  GPIOCFG_ANALOG
  #define PWRDN_OUT_BUTTON1  0
  #define WAKE_ON_BUTTON1    false

#elif (ENABLE_OSC32K != OSC32K_CRYSTAL) && defined(ENABLE_ALT_FUNCTION_NTX_ACTIVE)
  //Use nTX_ACTIVE configuration
  #define PWRUP_CFG_BUTTON1  GPIOCFG_OUT_ALT
  #define PWRUP_OUT_BUTTON1  0
  #define PWRDN_CFG_BUTTON1  GPIOCFG_OUT
  #define PWRDN_OUT_BUTTON1  0
  #define WAKE_ON_BUTTON1    false

#else
  //Use Button1 configuration
  #define PWRUP_CFG_BUTTON1  GPIOCFG_IN_PUD
  #define PWRUP_OUT_BUTTON1  GPIOOUT_PULLUP /* Button needs a pullup */
  #define PWRDN_CFG_BUTTON1  GPIOCFG_IN_PUD
  #define PWRDN_OUT_BUTTON1  GPIOOUT_PULLUP /* Button needs a pullup */
  #ifdef  BUTTON1
    #define WAKE_ON_BUTTON1    true
  #else//!BUTTON1
    #define WAKE_ON_BUTTON1    false
  #endif//BUTTON1
#endif

/**
 * @brief Give GPIO PC7 configuration a friendly name.
 */
#if   (ENABLE_OSC32K == OSC32K_CRYSTAL) || (ENABLE_OSC32K == OSC32K_SINE_1V)
  // Configure PC7 for OSC32K Crystal or analog sine wave
  #define CFG_OSC32K         GPIOCFG_ANALOG
#elif (ENABLE_OSC32K == OSC32K_DIGITAL)
  // Configure PC7 for OSC32K digital signal
  #define CFG_OSC32K         GPIOCFG_IN
#else // Generic GPIO usage
  // Configure PC7 for TEMP_EN
  #define CFG_OSC32K         GPIOCFG_OUT
#endif
//@} END OF 32kHz Oscillator and nTX_ACTIVE Configuration Defines


/** @name TX_ACTIVE Configuration Defines
 *
 * Provide the proper set of pin (PC5) configurations for when TX_ACTIVE is
 * enabled (look above for the define which enables it).  When TX_ACTIVE is
 * not enabled, configure the pin for LED2.
 *@{
 */
/**
 * @brief Give the TX_ACTIVE configuration a friendly name.
 */
#ifdef  ENABLE_ALT_FUNCTION_TX_ACTIVE
  #define PWRUP_CFG_LED2  GPIOCFG_OUT_ALT
  #define PWRUP_OUT_LED2  0
  #define PWRDN_CFG_LED2  GPIOCFG_OUT
  #define PWRDN_OUT_LED2  0
#else //!ENABLE_ALT_FUNCTION_TX_ACTIVE
  #define PWRUP_CFG_LED2  GPIOCFG_OUT
  #define PWRUP_OUT_LED2  1  /* LED default off */
  #define PWRDN_CFG_LED2  GPIOCFG_OUT
  #define PWRDN_OUT_LED2  1  /* LED default off */
#endif//ENABLE_ALT_FUNCTION_TX_ACTIVE
//@} END OF TX_ACTIVE Configuration Defines


/** @name USB Configuration Defines
 *
 * Provide the proper set of pin configuration for when USB is not enumerated.
 * Not enumerated primarily refers to the driver not being configured or deep
 * sleep.  The configuration used here is only for keeping the USB off the bus.
 * The GPIO configuration used when active is controlled by the USB driver
 * since the driver needs to control the enumeration process (which affects
 * GPIO state.)
 *
 * @note: Using USB requires Serial port 3 to be defined and is only possible
 * on EM3582/EM3586/EM3588/EM359 chips.
 *@{
 */
/**
 * @brief Give the USB configuration a friendly name.
 */
#if (!defined(EM_SERIAL3_DISABLED) && defined(CORTEXM3_EM35X_USB))
  #define PWRUP_CFG_USBDM      GPIOCFG_IN
  #define PWRUP_OUT_USBDM      0
  #define PWRUP_CFG_USBDP      GPIOCFG_IN
  #define PWRUP_OUT_USBDP      0
  #define PWRUP_CFG_ENUMCTRL   GPIOCFG_OUT
  #define PWRUP_OUT_ENUMCTRL   0
  #define PWRUP_CFG_VBUSMON    GPIOCFG_IN
  #define PWRUP_OUT_VBUSMON    0
  #define PWRDN_CFG_USBDM      GPIOCFG_IN
  #define PWRDN_OUT_USBDM      0
  #define PWRDN_CFG_USBDP      GPIOCFG_IN
  #define PWRDN_OUT_USBDP      0
  #define PWRDN_CFG_ENUMCTRL   GPIOCFG_OUT
  #define PWRDN_OUT_ENUMCTRL   0
  #define PWRDN_CFG_VBUSMON    GPIOCFG_IN
  #define PWRDN_OUT_VBUSMON    0
#else
  #define PWRUP_CFG_USBDM      GPIOCFG_OUT_ALT
  #define PWRUP_OUT_USBDM      0
  #define PWRUP_CFG_USBDP      GPIOCFG_IN
  #define PWRUP_OUT_USBDP      0
  #define PWRUP_CFG_ENUMCTRL   GPIOCFG_OUT_ALT
  #define PWRUP_OUT_ENUMCTRL   0
  #define PWRUP_CFG_VBUSMON    GPIOCFG_OUT
  #define PWRUP_OUT_VBUSMON    1
  #define PWRDN_CFG_USBDM      GPIOCFG_IN_PUD
  #define PWRDN_OUT_USBDM      GPIOOUT_PULLUP
  #define PWRDN_CFG_USBDP      GPIOCFG_IN_PUD
  #define PWRDN_OUT_USBDP      GPIOOUT_PULLUP
  #define PWRDN_CFG_ENUMCTRL   GPIOCFG_IN_PUD
  #define PWRDN_OUT_ENUMCTRL   GPIOOUT_PULLUP
  #define PWRDN_CFG_VBUSMON    GPIOCFG_OUT
  #define PWRDN_OUT_VBUSMON    1
#endif//
//@} END OF USB Configuration Defines

/**
 * @brief Give GPIO SC1 TXD and nRTS configurations friendly names.
 */
#if     SLEEPY_IP_MODEM_UART
  #define PWRUP_CFG_SC1_TXD  GPIOCFG_OUT     // Let UART driver manage OUT_ALT
  #define PWRDN_OUT_SC1_nRTS 0               // Let peer send data to wake
#else//!SLEEPY_IP_MODEM_UART
  #define PWRUP_CFG_SC1_TXD  GPIOCFG_OUT_ALT // Pre-set for UART operation
  #define PWRDN_OUT_SC1_nRTS 1               // Deassert nRTS when sleeping
#endif//SLEEPY_IP_MODEM_UART

/** @name GPIO Configuration Macros
 *
 * These macros define the GPIO configuration and initial state of the output
 * registers for all the GPIO in the powerup and powerdown modes.
 *@{
 */


//Each pin has 4 cfg bits.  There are 3 ports with 2 cfg registers per
//port since the cfg register only holds 2 pins (16bits).  Therefore,
//the cfg arrays need to be 6 entries of 16bits.
extern uint16_t gpioCfgPowerUp[6];
extern uint16_t gpioCfgPowerDown[6];
//Each pin has 1 out bit.  There are 3 ports with 1 out register per
//port (8bits).  Therefore, the out arrays need to be 3 entries of 8bits.
extern uint8_t gpioOutPowerUp[3];
extern uint8_t gpioOutPowerDown[3];
//A single mask variable covers all GPIO.
extern GpioMaskType gpioRadioPowerBoardMask;


/**
 * @brief Define the pins crucial to host-NCP communications.  This
 * mask is used to prevent the host from changing pin configurations
 * necessary to maintain host-NCP communications (otherwise changes would
 * break the link!).
 *
 * For MCUs connected to PRO2+, blackout both the PRO2+ SPI interface and
 * the NCP interface.
 */
#define EZSP_PIN_BLACKOUT_MASK  ((((GpioMaskType)1)<<PORTA_PIN(0))|  /*PRO2_SDI       */ \
                                 (((GpioMaskType)1)<<PORTA_PIN(1))|  /*PRO2_SDO       */ \
                                 (((GpioMaskType)1)<<PORTA_PIN(2))|  /*PRO2_SCLK      */ \
                                 (((GpioMaskType)1)<<PORTA_PIN(3))|  /*PRO2_nSEL      */ \
                                 (((GpioMaskType)1)<<PORTA_PIN(6))|  /*PRO2_SDN       */ \
                                 (((GpioMaskType)1)<<PORTA_PIN(7))|  /*PRO2_nIRQ      */ \
                                 (((GpioMaskType)1)<<PORTB_PIN(6))|  /*HOST_nWAKE     */ \
                                 (((GpioMaskType)1)<<PORTB_PIN(1))|  /*HOST_MISO/TXD  */ \
                                 (((GpioMaskType)1)<<PORTB_PIN(2))|  /*HOST_MOSI/RXD  */ \
                                 (((GpioMaskType)1)<<PORTB_PIN(3))|  /*HOST_SCLK/nCTS */ \
                                 (((GpioMaskType)1)<<PORTB_PIN(4))|  /*HOST_nSEL/nRTS */ \
                                 (((GpioMaskType)1)<<PORTC_PIN(7)))  /*HOST_nINT      */


// Set up FEM control signals based on ENABLE_ALT_FUNCTION_xxx defines above
// in preparation for GPIO Radio Power Board Mask below:
// -PA control line (Tx_Active)
#ifdef ENABLE_ALT_FUNCTION_TX_ACTIVE
  #define FEM_CTX_BIT (BIT32(PORTC_PIN(5)))
#else
  #define FEM_CTX_BIT (0)
#endif
// -LNA control line (!Tx_Active)
#ifdef ENABLE_ALT_FUNCTION_NTX_ACTIVE
  #define FEM_CRX_BIT (BIT32(PORTC_PIN(6)))
#else
  #define FEM_CRX_BIT (0)
#endif

/**
 * @brief Define the mask for GPIO relevant to the radio in the context
 * of power state.  Each bit in the mask indicates the corresponding GPIO
 * which should be affected when invoking halStackRadioPowerUpBoard() or
 * halStackRadioPowerDownBoard().
 */
#define DEFINE_GPIO_RADIO_POWER_BOARD_MASK_VARIABLE() \
GpioMaskType gpioRadioPowerBoardMask = {( BIT32(PORTB_PIN(0)) /* FEM_CSD */    \
                                        | BIT32(PORTB_PIN(5)) /* FEM_CPS */    \
                                        | FEM_CTX_BIT                          \
                                        | FEM_CRX_BIT                          \
                                        )}

/**
 * @brief Initialize GPIO powerup configuration variables.
 */
#define DEFINE_POWERUP_GPIO_CFG_VARIABLES()                                   \
const uint8_t pro2SpiPort = PRO2_SPI_PORT;                                      \
const uint8_t pro2SpiClockMHz = PRO2_SPI_MHZ;                                   \
const uint8_t pro2GpioCfgPowerUp[] = { PRO2_POWERUP_GPIO_CFG() };               \
int8_t phy2PtiGpio = PRO2_PTI_GPIO;                                            \
uint16_t gpioCfgPowerUp[6] = {                                                  \
                            ((PWRUP_CFG_USBDM    <<PA0_CFG_BIT)|              \
                             (PWRUP_CFG_USBDP    <<PA1_CFG_BIT)|              \
                             (PWRUP_CFG_ENUMCTRL <<PA2_CFG_BIT)|              \
                             (PWRUP_CFG_VBUSMON  <<PA3_CFG_BIT)),             \
                            ((PWRUP_CFG_PTI_EN   <<PA4_CFG_BIT)|              \
                             (PWRUP_CFG_PTI_DATA <<PA5_CFG_BIT)|              \
                             (GPIOCFG_OUT        <<PA6_CFG_BIT)|              \
                             (GPIOCFG_IN         <<PA7_CFG_BIT)),             \
                            ((GPIOCFG_OUT        <<PB0_CFG_BIT)|              \
                             (PWRUP_CFG_SC1_TXD  <<PB1_CFG_BIT)|              \
                             (GPIOCFG_IN_PUD     <<PB2_CFG_BIT)|              \
                             (GPIOCFG_IN_PUD     <<PB3_CFG_BIT)),             \
                            ((GPIOCFG_OUT_ALT    <<PB4_CFG_BIT)|              \
                             (GPIOCFG_OUT        <<PB5_CFG_BIT)|              \
                             (PWRUP_CFG_DFL_RHO  <<PB6_CFG_BIT)|              \
                             (PWRUP_CFG_EEPROM   <<PB7_CFG_BIT)),             \
                            ((GPIOCFG_IN         <<PC0_CFG_BIT)|              \
                             (GPIOCFG_OUT        <<PC1_CFG_BIT)|              \
                             (GPIOCFG_OUT_ALT    <<PC2_CFG_BIT)|              \
                             (GPIOCFG_IN         <<PC3_CFG_BIT)),             \
                            ((GPIOCFG_IN         <<PC4_CFG_BIT)|              \
                             (PWRUP_CFG_LED2     <<PC5_CFG_BIT)|              \
                             (PWRUP_CFG_BUTTON1  <<PC6_CFG_BIT)|              \
                             (CFG_OSC32K         <<PC7_CFG_BIT))              \
                           }


/**
 * @brief Initialize GPIO powerup output variables.
 */
#define DEFINE_POWERUP_GPIO_OUTPUT_DATA_VARIABLES()                           \
uint8_t gpioOutPowerUp[3] = {                                                   \
                           ((PWRUP_OUT_USBDM    <<PA0_BIT)|                   \
                            (PWRUP_OUT_USBDP    <<PA1_BIT)|                   \
                            (PWRUP_OUT_ENUMCTRL <<PA2_BIT)|                   \
                            (PWRUP_OUT_VBUSMON  <<PA3_BIT)|                   \
                            (PWRUP_OUT_PTI_EN   <<PA4_BIT)|                   \
                            (PWRUP_OUT_PTI_DATA <<PA5_BIT)|                   \
                            (0                  <<PA6_BIT)|                   \
                            (1                  <<PA7_BIT)),                  \
                           ((1                  <<PB0_BIT)|                   \
                            (1                  <<PB1_BIT)|                   \
                            (GPIOOUT_PULLUP     <<PB2_BIT)|                   \
                            (GPIOOUT_PULLUP     <<PB3_BIT)|                   \
                            (0                  <<PB4_BIT)|                   \
                            (1                  <<PB5_BIT)|                   \
                            (PWRUP_OUT_DFL_RHO  <<PB6_BIT)|                   \
                            (PWRUP_OUT_EEPROM   <<PB7_BIT)),                  \
                           ((0                  <<PC0_BIT)|                   \
                            (1                  <<PC1_BIT)|                   \
                            (1                  <<PC2_BIT)|                   \
                            (0                  <<PC3_BIT)|                   \
                            (0                  <<PC4_BIT)|                   \
                            (PWRUP_OUT_LED2     <<PC5_BIT)|                   \
                            (PWRUP_OUT_BUTTON1  <<PC6_BIT)|                   \
                            (0                  <<PC7_BIT))                   \
                          }


/**
 * @brief Initialize powerdown GPIO configuration variables.
 */
#define DEFINE_POWERDOWN_GPIO_CFG_VARIABLES()                                 \
uint16_t gpioCfgPowerDown[6] = {                                                \
                              ((PWRDN_CFG_USBDM    <<PA0_CFG_BIT)|            \
                               (PWRDN_CFG_USBDP    <<PA1_CFG_BIT)|            \
                               (PWRDN_CFG_ENUMCTRL <<PA2_CFG_BIT)|            \
                               (PWRDN_CFG_VBUSMON  <<PA3_CFG_BIT)),           \
                              ((PWRDN_CFG_PTI_EN   <<PA4_CFG_BIT)|            \
                               (PWRDN_CFG_PTI_DATA <<PA5_CFG_BIT)|            \
                               (GPIOCFG_OUT        <<PA6_CFG_BIT)|            \
                               (GPIOCFG_IN_PUD     <<PA7_CFG_BIT)),           \
                              ((GPIOCFG_OUT        <<PB0_CFG_BIT)|            \
                               (GPIOCFG_OUT        <<PB1_CFG_BIT)|            \
                               (GPIOCFG_IN_PUD     <<PB2_CFG_BIT)|            \
                               (GPIOCFG_IN_PUD     <<PB3_CFG_BIT)),           \
                              ((GPIOCFG_OUT        <<PB4_CFG_BIT)|            \
                               (GPIOCFG_OUT        <<PB5_CFG_BIT)|            \
                               (PWRDN_CFG_DFL_RHO  <<PB6_CFG_BIT)|            \
                               (PWRDN_CFG_EEPROM   <<PB7_CFG_BIT)),           \
                              ((GPIOCFG_IN_PUD     <<PC0_CFG_BIT)|            \
                               (GPIOCFG_OUT        <<PC1_CFG_BIT)|            \
                               (GPIOCFG_OUT        <<PC2_CFG_BIT)|            \
                               (GPIOCFG_IN_PUD     <<PC3_CFG_BIT)),           \
                              ((GPIOCFG_IN_PUD     <<PC4_CFG_BIT)|            \
                               (PWRDN_CFG_LED2     <<PC5_CFG_BIT)|            \
                               (PWRDN_CFG_BUTTON1  <<PC6_CFG_BIT)|            \
                               (CFG_OSC32K         <<PC7_CFG_BIT))            \
                             }


/**
 * @brief Initialize powerdown GPIO output variables.
 */
#define DEFINE_POWERDOWN_GPIO_OUTPUT_DATA_VARIABLES()                         \
uint8_t gpioOutPowerDown[3] = {                                                 \
                             ((PWRDN_OUT_USBDM    <<PA0_BIT)|                 \
                              (PWRDN_OUT_USBDP    <<PA1_BIT)|                 \
                              (PWRDN_OUT_ENUMCTRL <<PA2_BIT)|                 \
                              (PWRDN_OUT_VBUSMON  <<PA3_BIT)|                 \
                              (PWRDN_OUT_PTI_EN   <<PA4_BIT)|                 \
                              (PWRDN_OUT_PTI_DATA <<PA5_BIT)|                 \
                              (1                  <<PA6_BIT)|                 \
                              (GPIOOUT_PULLUP     <<PA7_BIT)),                \
                             ((0                  <<PB0_BIT)|                 \
                              (1                  <<PB1_BIT)|                 \
                              (GPIOOUT_PULLUP     <<PB2_BIT)|                 \
                              (GPIOOUT_PULLDOWN   <<PB3_BIT)|                 \
                              (PWRDN_OUT_SC1_nRTS <<PB4_BIT)|                 \
                              (0                  <<PB5_BIT)|                 \
                              (PWRDN_OUT_DFL_RHO  <<PB6_BIT)|                 \
                              (PWRDN_OUT_EEPROM   <<PB7_BIT)),                \
                             ((GPIOOUT_PULLUP     <<PC0_BIT)|                 \
                              (1                  <<PC1_BIT)|                 \
                              (1                  <<PC2_BIT)|                 \
                              (GPIOOUT_PULLDOWN   <<PC3_BIT)|                 \
                              (GPIOOUT_PULLDOWN   <<PC4_BIT)|                 \
                              (PWRDN_OUT_LED2     <<PC5_BIT)|                 \
                              (PWRDN_OUT_BUTTON1  <<PC6_BIT)|                 \
                              (0                  <<PC7_BIT))                 \
                            }


/**
 * @brief Set powerup GPIO configuration registers.
 */
#define SET_POWERUP_GPIO_CFG_REGISTERS() do { \
    GPIO_PACFGL = gpioCfgPowerUp[0];          \
    GPIO_PACFGH = gpioCfgPowerUp[1];          \
    GPIO_PBCFGL = gpioCfgPowerUp[2];          \
    GPIO_PBCFGH = gpioCfgPowerUp[3];          \
    GPIO_PCCFGL = gpioCfgPowerUp[4];          \
    GPIO_PCCFGH = gpioCfgPowerUp[5];          \
  } while (0)


/**
 * @brief Set powerup GPIO output registers.
 */
#define SET_POWERUP_GPIO_OUTPUT_DATA_REGISTERS() do { \
    GPIO_PAOUT = gpioOutPowerUp[0];                   \
    GPIO_PBOUT = gpioOutPowerUp[1];                   \
    GPIO_PCOUT = gpioOutPowerUp[2];                   \
  } while(0)


/**
 * @brief Set powerdown GPIO configuration registers.
 */
#define SET_POWERDOWN_GPIO_CFG_REGISTERS() do { \
    GPIO_PACFGL = gpioCfgPowerDown[0];          \
    GPIO_PACFGH = gpioCfgPowerDown[1];          \
    GPIO_PBCFGL = gpioCfgPowerDown[2];          \
    GPIO_PBCFGH = gpioCfgPowerDown[3];          \
    GPIO_PCCFGL = gpioCfgPowerDown[4];          \
    GPIO_PCCFGH = gpioCfgPowerDown[5];          \
  } while(0)


/**
 * @brief Set powerdown GPIO output registers.
 */
#define SET_POWERDOWN_GPIO_OUTPUT_DATA_REGISTERS() do { \
    GPIO_PAOUT = gpioOutPowerDown[0];                   \
    GPIO_PBOUT = gpioOutPowerDown[1];                   \
    GPIO_PCOUT = gpioOutPowerDown[2];                   \
  } while(0)


/**
 * @brief Set resume GPIO configuration registers. Identical to SET_POWERUP
 */
#define SET_RESUME_GPIO_CFG_REGISTERS() \
        SET_POWERUP_GPIO_CFG_REGISTERS()


/**
 * @brief Set resume GPIO output registers. Identical to SET_POWERUP
 */
#define SET_RESUME_GPIO_OUTPUT_DATA_REGISTERS() \
        SET_POWERUP_GPIO_OUTPUT_DATA_REGISTERS()


/**
 * @brief Set suspend GPIO configuration registers. SET_POWERDOWN minus USB regs
 */
#define SET_SUSPEND_GPIO_CFG_REGISTERS() do { \
    /* GPIO_PACFGL USB untouched! */          \
    GPIO_PACFGH = gpioCfgPowerDown[1];        \
    GPIO_PBCFGL = gpioCfgPowerDown[2];        \
    GPIO_PBCFGH = gpioCfgPowerDown[3];        \
    GPIO_PCCFGL = gpioCfgPowerDown[4];        \
    GPIO_PCCFGH = gpioCfgPowerDown[5];        \
  } while(0)


/**
 * @brief Set suspend GPIO output registers. SET_POWERDOWN minus USB regs
 */
#define SET_SUSPEND_GPIO_OUTPUT_DATA_REGISTERS() do { \
    /* Avoid touching the USB signals */              \
    enum { PA_USB_MASK = (PA0 | PA1 | PA2 | PA3) } ;  \
    GPIO_PAOUT = (GPIO_PAOUT & PA_USB_MASK)           \
               |(gpioOutPowerDown[0] & ~PA_USB_MASK); \
    GPIO_PBOUT = gpioOutPowerDown[1];                 \
    GPIO_PCOUT = gpioOutPowerDown[2];                 \
  } while (0)


/**
 * @brief External regulator enable/disable macro.
 */
#ifdef ENABLE_ALT_FUNCTION_REG_EN
  #error  "PA7 conflict: REG_EN is not supported as that pin is used for PRO2+ nIRQ"
  #define CONFIGURE_EXTERNAL_REGULATOR_ENABLE()  (GPIO_DBGCFG |= GPIO_EXTREGEN)
#else
  #define CONFIGURE_EXTERNAL_REGULATOR_ENABLE()  (GPIO_DBGCFG &= ~GPIO_EXTREGEN)
#endif
//@} END OF GPIO Configuration Macros


/** @name GPIO Wake Source Definitions
 *
 * A convenient define that chooses if this external signal can
 * be used as source to wake from deep sleep.  Any change in the state of the
 * signal will wake up the CPU.
 */
 //@{
/**
 * @brief true if this GPIO can wake the chip from deep sleep, false if not.
 */
#define WAKE_ON_PA0   false
#define WAKE_ON_PA1   false
#define WAKE_ON_PA2   false
#define WAKE_ON_PA3   false
#define WAKE_ON_PA4   false
#define WAKE_ON_PA5   false
#define WAKE_ON_PA6   false
#define WAKE_ON_PA7   false // Currently when em3xx sleeps, PRO2+ is shut down
#define WAKE_ON_PB0   true  // HOST_nWAKE
#define WAKE_ON_PB1   false
#if defined(SLEEPY_EZSP_UART) || SLEEPY_IP_MODEM_UART  // SC1RXD
  #define WAKE_ON_PB2   true
#else
  #define WAKE_ON_PB2   false
#endif
#define WAKE_ON_PB3   false
#define WAKE_ON_PB4   false
#define WAKE_ON_PB5   false
#define WAKE_ON_PB6   WAKE_ON_DFL_RHO_VAR
#define WAKE_ON_PB7   false
#define WAKE_ON_PC0   false
#define WAKE_ON_PC1   false
#define WAKE_ON_PC2   false
#define WAKE_ON_PC3   false
#define WAKE_ON_PC4   false
#define WAKE_ON_PC5   false
#define WAKE_ON_PC6   WAKE_ON_BUTTON1
#define WAKE_ON_PC7   false
//@} //END OF GPIO Wake Source Definitions


//@} //END OF GPIO Configuration Definitions


/** @name Board Specific Functions
 *
 * The following macros exist to aid in the initialization, power up from sleep,
 * and power down to sleep operations.  These macros are responsible for
 * either initializing directly, or calling initialization functions for any
 * peripherals that are specific to this board implementation.  These
 * macros are called from halInit, halPowerDown, and halPowerUp respectively.
 */
 //@{
/**
 * @brief Initialize the board.  This function is called from ::halInit().
 */
#if     (defined(EZSP_SPI) || defined(EZSP_ASH))
  #define halInternalInitBoard()                                    \
          do {                                                      \
            halInternalPowerUpBoardNoRadio();                       \
            halInternalInitRadioHoldOff();                          \
         } while(0)
#else//!(defined(EZSP_SPI) || defined(EZSP_ASH))
  #define halInternalInitBoard()                                    \
          do {                                                      \
            halInternalPowerUpBoardNoRadio();                       \
            halInternalInitButton();                                \
            halInternalInitRadioHoldOff();                          \
         } while(0)
#endif//(defined(EZSP_SPI) || defined(EZSP_ASH))

/**
 * @brief Power down the board.  This function is called from
 * ::halPowerDown().
 */
#define halInternalPowerDownBoard()                                 \
        do {                                                        \
          /* Board peripheral deactivation */                       \
          /* halInternalSleepAdc(); */                              \
          extern void emRadioPowerDown(void);                       \
          emRadioPowerDown();                                       \
          SET_POWERDOWN_GPIO_OUTPUT_DATA_REGISTERS();               \
          SET_POWERDOWN_GPIO_CFG_REGISTERS();                       \
        } while(0)

/**
 * @brief Suspend the board.  This function is called from
 * ::halSuspend().
 */
#define halInternalSuspendBoard()                                   \
        do {                                                        \
          /* Board peripheral deactivation */                       \
          /* halInternalSleepAdc(); */                              \
          extern void emRadioPowerDown(void);                       \
          emRadioPowerDown();                                       \
          SET_SUSPEND_GPIO_OUTPUT_DATA_REGISTERS();                 \
          SET_SUSPEND_GPIO_CFG_REGISTERS();                         \
        } while(0)

/**
 * @brief Power up the board and radio.  This function is used internally.
 */
#define halInternalPowerUpBoardNoRadio()                            \
        do {                                                        \
          SET_POWERUP_GPIO_OUTPUT_DATA_REGISTERS();                 \
          SET_POWERUP_GPIO_CFG_REGISTERS();                         \
          /*The radio GPIO should remain in the powerdown state */  \
          /*until the stack specifically powers them up. */         \
          halStackRadioPowerDownBoard();                            \
          CONFIGURE_EXTERNAL_REGULATOR_ENABLE();                    \
          /* Board peripheral reactivation */                       \
          halInternalInitAdc();                                     \
        } while(0)

/**
 * @brief Power up the board.  This function is called from
 * ::halPowerUp().
 */
#define halInternalPowerUpBoard()                                   \
        do {                                                        \
          halInternalPowerUpBoardNoRadio();                         \
          extern void emRadioPowerUp(void);                         \
          emRadioPowerUp();                                         \
        } while(0)

/**
 * @brief Resume the board.  This function is called from
 * ::halResume().
 */
#define halInternalResumeBoard()                                    \
        do {                                                        \
          SET_RESUME_GPIO_OUTPUT_DATA_REGISTERS();                  \
          SET_RESUME_GPIO_CFG_REGISTERS();                          \
          /*The radio GPIO should remain in the powerdown state */  \
          /*until the stack specifically powers them up. */         \
          halStackRadioPowerDownBoard();                            \
          CONFIGURE_EXTERNAL_REGULATOR_ENABLE();                    \
          /* Board peripheral reactivation */                       \
          halInternalInitAdc();                                     \
          extern void emRadioPowerUp(void);                         \
          emRadioPowerUp();                                         \
        } while(0)
//@} //END OF BOARD SPECIFIC FUNCTIONS

#endif //__BOARD_H__

/** @} END Board Specific Functions */

/** @} END addtogroup */
