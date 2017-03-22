/** @file hal/micro/cortexm3/em35x/board/ncp_spi_16gpio.h
 * See @ref board for detailed documentation.
 *
 * <!-- Copyright 2014 Silicon Laboratories, Inc.                        *80*-->
 */

/** @addtogroup board
 *  @brief Functions and definitions specific to the breakout board.
 *
 * A 16-gpio variant on a dev0680 BoB has the following example GPIO config.
 * This board file and the default HAL setup reflects this configuration.
 * - PA0 - UNUSED
 * - PA1 - UNUSED
 * - PA2 - UNUSED
 * - PA3 - UNUSED
 * - PA4 - PTI_EN
 * - PA5 - PTI_DATA
 * - PA6 - UNUSED
 * - PA7 - LED/RHO/REG_EN
 * - PB0 - nWAKE
 * - PB1 - SC1MISO
 * - PB2 - SC1MOSI
 * - PB3 - SC1SCLK
 * - PB4 - SC1nSSEL
 * - PB5 - UNUSED
 * - PB6 - UNUSED
 * - PB7 - UNUSED
 * - PC0 - JTAG (JRST) / TRACEDATA1
 * - PC1 - FEM_CPS/FEM_CSD
 * - PC2 - JTAG (JTDO) / SWO / TRACEDATA0
 * - PC3 - JTAG (JTDI) / TRACECLK
 * - PC4 - JTAG (JTMS) / SWDIO
 * - PC5 - FEM_CTX
 * - PC6 - FEM_CRX
 * - PC7 - nHOST_INT
 *
 *@{
 */

#ifndef __BOARD_H__
#define __BOARD_H__

#ifndef DOXYGEN_SHOULD_SKIP_THIS
  #include "hal/micro/cortexm3/spi-protocol.h"
#endif //DOXYGEN_SHOULD_SKIP_THIS

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
 *
 * \b Note: LEDs 0 and 1 are on the RCM.
 *
 * \b Note: LED 2 is on the breakout board (dev0680).
 *
 * \b Note: LED 3 simply redirects to LED 2.
 */
//@{

/**
 * @brief Assign each GPIO with an LED connected to a convenient name.
 * ::BOARD_ACTIVITY_LED and ::BOARD_HEARTBEAT_LED provide a further layer of
 * abstraction on top of the 3 LEDs for verbose coding.
 */
enum HalBoardLedPins {
  BOARDLED1 = PORTA_PIN(7),
  BOARDLED2 = PORTC_PIN(5),
  BOARDLED0 = BOARDLED2,
  BOARDLED3 = BOARDLED2,
  BOARD_ACTIVITY_LED  = BOARDLED0,
  BOARD_HEARTBEAT_LED = BOARDLED1
};

/** @} END OF LED DEFINITIONS  */

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
#ifdef RADIO_HOLDOFF
  #error Radio HoldOff not supported a 16-gpio variant on a dev0680
#endif
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
#define RHO_GPIO              PORTA_PIN(7)
/**
 * @brief The GPIO signal level to assert Radio HoldOff (1=high, 0=low).
 */
#define RHO_ASSERTED          1
/**
 * @brief The GPIO configuration register for Radio HoldOff.
 */
#define RHO_CFG               GPIO_PACFGH
/**
 * @brief The GPIO input register for Radio HoldOff.
 */
#define RHO_IN                GPIO_PAIN
/**
 * @brief The GPIO output register for Radio HoldOff.
 */
#define RHO_OUT               GPIO_PAOUT
/**
 * @brief Point the proper IRQ at the desired pin for Radio HoldOff.
 * Remember there may be other things that might want to use this IRQ.
 */
#define RHO_SEL()             do { GPIO_IRQDSEL = RHO_GPIO; } while(0)
/**
 * @brief The interrupt service routine for Radio HoldOff.
 * Remember there may be other things that might want to use this IRQ.
 */
#define RHO_ISR               halIrqDIsr
/**
 * @brief The interrupt configuration register for Radio HoldOff.
 */
#define RHO_INTCFG            GPIO_INTCFGD
/**
 * @brief The interrupt enable bit for Radio HoldOff.
 */
#define RHO_INT_EN_BIT        INT_IRQD
/**
 * @brief The interrupt flag bit for Radio HoldOff.
 */
#define RHO_FLAG_BIT          INT_IRQDFLAG
/**
 * @brief The missed interrupt bit for Radio HoldOff.
 */
#define RHO_MISS_BIT          INT_MISSIRQD

/** @brief Configuration of GPIO for Radio HoldOff operation
 */
#define PWRUP_CFG_DFL_RHO_FOR_RHO   GPIOCFG_IN_PUD
#define PWRUP_OUT_DFL_RHO_FOR_RHO   GPIOOUT_PULLDOWN /* Deassert */
#define PWRDN_CFG_DFL_RHO_FOR_RHO   GPIOCFG_IN_PUD
#define PWRDN_OUT_DFL_RHO_FOR_RHO   GPIOOUT_PULLDOWN /* Deassert */

/** @brief Configuration of GPIO for default behavior
 */
#define PWRUP_CFG_DFL_RHO_FOR_DFL   GPIOCFG_OUT
#define PWRUP_OUT_DFL_RHO_FOR_DFL   1 /* LED default off */
#define PWRDN_CFG_DFL_RHO_FOR_DFL   GPIOCFG_OUT
#define PWRDN_OUT_DFL_RHO_FOR_DFL   1 /* LED off */

/** @brief The following definitions are helpers for managing
 *  Radio HoldOff and should not be modified.
 */
#if     (defined(RADIO_HOLDOFF) && defined(RHO_GPIO))
  // Initial bootup configuration is for Radio HoldOff
  #define PWRUP_CFG_DFL_RHO           PWRUP_CFG_DFL_RHO_FOR_RHO
  #define PWRUP_OUT_DFL_RHO           PWRUP_OUT_DFL_RHO_FOR_RHO
  #define PWRDN_CFG_DFL_RHO           PWRDN_CFG_DFL_RHO_FOR_RHO
  #define PWRDN_OUT_DFL_RHO           PWRDN_OUT_DFL_RHO_FOR_RHO
  #define halInternalInitRadioHoldOff() halSetRadioHoldOff(true)
#else//!(defined(RADIO_HOLDOFF) && defined(RHO_GPIO))
  // Initial bootup configuration is for default
  #define PWRUP_CFG_DFL_RHO           PWRUP_CFG_DFL_RHO_FOR_DFL
  #define PWRUP_OUT_DFL_RHO           PWRUP_OUT_DFL_RHO_FOR_DFL
  #define PWRDN_CFG_DFL_RHO           PWRDN_CFG_DFL_RHO_FOR_DFL
  #define PWRDN_OUT_DFL_RHO           PWRDN_OUT_DFL_RHO_FOR_DFL
  #define halInternalInitRadioHoldOff() /* no-op */
#endif//(defined(RADIO_HOLDOFF) && defined(RHO_GPIO))

#ifdef  RHO_GPIO

  #define ADJUST_GPIO_CONFIG_DFL_RHO(enableRadioHoldOff)  do {                \
          ATOMIC( /* Must read-modify-write so to be safe, use ATOMIC() */    \
            if (enableRadioHoldOff) { /* Radio HoldOff */                     \
              /* Actual register state */                                     \
              /*halGpioSetConfig(RHO_CFG, PWRUP_CFG_DFL_RHO_FOR_RHO);*/       \
              RHO_CFG = RHO_CFG                                               \
                & ~(0x000F                   << ((RHO_GPIO&3)*4))             \
                | (PWRUP_CFG_DFL_RHO_FOR_RHO << ((RHO_GPIO&3)*4));            \
              RHO_OUT = RHO_OUT                                               \
                & ~(0x0001                   << ((RHO_GPIO&7)  ))             \
                | (PWRUP_OUT_DFL_RHO_FOR_RHO << ((RHO_GPIO&7)  ));            \
              /* Shadow register state */                                     \
              gpioCfgPowerUp[RHO_GPIO>>2] = gpioCfgPowerUp[RHO_GPIO>>2]       \
                & ~(0x000F                   << ((RHO_GPIO&3)*4))             \
                | (PWRUP_CFG_DFL_RHO_FOR_RHO << ((RHO_GPIO&3)*4));            \
              gpioOutPowerUp[RHO_GPIO>>3] = gpioOutPowerUp[RHO_GPIO>>3]       \
                & ~(0x0001                   << ((RHO_GPIO&7)  ))             \
                | (PWRUP_OUT_DFL_RHO_FOR_RHO << ((RHO_GPIO&7)  ));            \
              gpioCfgPowerDown[RHO_GPIO>>2] = gpioCfgPowerDown[RHO_GPIO>>2]   \
                & ~(0x000F                   << ((RHO_GPIO&3)*4))             \
                | (PWRDN_CFG_DFL_RHO_FOR_RHO << ((RHO_GPIO&3)*4));            \
              gpioOutPowerDown[RHO_GPIO>>3] = gpioOutPowerDown[RHO_GPIO>>3]   \
                & ~(0x0001                   << ((RHO_GPIO&7)  ))             \
                | (PWRDN_OUT_DFL_RHO_FOR_RHO << ((RHO_GPIO&7)  ));            \
              RHO_INTCFG  = (0 << GPIO_INTFILT_BIT) /* 0 = no filter  */      \
                          | (3 << GPIO_INTMOD_BIT); /* 3 = both edges */      \
            } else { /* default */                                            \
              /* Actual register state */                                     \
              /*halGpioSetConfig(RHO_CFG, PWRUP_CFG_DFL_RHO_FOR_DFL);*/       \
              RHO_CFG = RHO_CFG                                               \
                & ~(0x000F                   << ((RHO_GPIO&3)*4))             \
                | (PWRUP_CFG_DFL_RHO_FOR_DFL << ((RHO_GPIO&3)*4));            \
              RHO_OUT = RHO_OUT                                               \
                & ~(0x0001                   << ((RHO_GPIO&7)  ))             \
                | (PWRUP_OUT_DFL_RHO_FOR_DFL << ((RHO_GPIO&7)  ));            \
              /* Shadow register state */                                     \
              gpioCfgPowerUp[RHO_GPIO>>2] = gpioCfgPowerUp[RHO_GPIO>>2]       \
                & ~(0x000F                   << ((RHO_GPIO&3)*4))             \
                | (PWRUP_CFG_DFL_RHO_FOR_DFL << ((RHO_GPIO&3)*4));            \
              gpioOutPowerUp[RHO_GPIO>>3] = gpioOutPowerUp[RHO_GPIO>>3]       \
                & ~(0x0001                   << ((RHO_GPIO&7)  ))             \
                | (PWRUP_OUT_DFL_RHO_FOR_DFL << ((RHO_GPIO&7)  ));            \
              gpioCfgPowerDown[RHO_GPIO>>2] = gpioCfgPowerDown[RHO_GPIO>>2]   \
                & ~(0x000F                   << ((RHO_GPIO&3)*4))             \
                | (PWRDN_CFG_DFL_RHO_FOR_DFL << ((RHO_GPIO&3)*4));            \
              gpioOutPowerDown[RHO_GPIO>>3] = gpioOutPowerDown[RHO_GPIO>>3]   \
                & ~(0x0001                   << ((RHO_GPIO&7)  ))             \
                | (PWRDN_OUT_DFL_RHO_FOR_DFL << ((RHO_GPIO&7)  ));            \
              RHO_INTCFG  = 0; /* disabled */                                 \
            }                                                                 \
            RHO_SEL(); /* Point IRQ at the desired pin */                     \
          )} while (0)

#endif//RHO_GPIO
//@} //END OF RADIO HOLDOFF CONFIGURATION DEFINITIONS

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
 * trigger to enable Packet Trace support on the breakout board (dev0680).
 */
#define PACKET_TRACE  // We do have PACKET_TRACE support
//@} //END OF PACKET TRACE DEFINITIONS

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
 * Default enable TX_ACTIVE functionality on PC5 when using
 * the EM35x as an NCP for EZSP-SPI.
 */
#define ENABLE_ALT_FUNCTION_TX_ACTIVE
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
 * ENABLE_OSC32K since they define conflicting usage of GPIO PC6.
 */
//@{
/**
 * @brief This define does not equate to anything.  It is used as a
 * trigger to enable the nTX_ACTIVE alternate function on PC6.
 * Default enable nTX_ACTIVE functionality on PC6 when using
 * the EM35x as an NCP for EZSP-SPI.
 */
  #define ENABLE_ALT_FUNCTION_NTX_ACTIVE
//@} //END OF ENABLE_ALT_FUNCTION_NTX_ACTIVE DEFINITIONS

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
  #define PWRUP_CFG_PTI_EN    GPIOCFG_OUT_ALT
  #define PWRUP_OUT_PTI_EN    0
  #define PWRDN_CFG_PTI_EN    GPIOCFG_IN_PUD
  #define PWRDN_OUT_PTI_EN    GPIOOUT_PULLDOWN
  #define PWRUP_CFG_PTI_DATA  GPIOCFG_OUT_ALT
  #define PWRUP_OUT_PTI_DATA  1
  #define PWRDN_CFG_PTI_DATA  GPIOCFG_IN_PUD
  #define PWRDN_OUT_PTI_DATA  GPIOOUT_PULLUP
#else//!PACKET_TRACE
  #define PWRUP_CFG_PTI_EN    GPIOCFG_IN
  #define PWRUP_OUT_PTI_EN    0
  #define PWRDN_CFG_PTI_EN    GPIOCFG_IN
  #define PWRDN_OUT_PTI_EN    0
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
 * @note ENABLE_OSC32K is mutually exclusive with
 * ENABLE_ALT_FUNCTION_NTX_ACTIVE since they define conflicting
 * usage of GPIO PC6.
 *
 * When using the 32kHz, configure PC6 and PC7 for analog for the XTAL.
 *
 * When using nTX_ACTIVE, configure PC6 for alternate output while awake
 * and a low output when deepsleeping.  Also, configure PC7 for TEMP_EN.
 *
 * When not using the 32kHz or nTX_ACTIVE, configure PC6 and PC7 for
 * Button1 and TEMP_EN.
 *@{
 */
/**
 * @brief Give GPIO PC6 configuration a friendly name.
 */
#if defined(ENABLE_OSC32K) && defined(ENABLE_ALT_FUNCTION_NTX_ACTIVE)
  //Oops!  Only one of these can be used at a time!
  #error ENABLE_OSC32K and ENABLE_ALT_FUNCTION_NTX_ACTIVE are mutually\
 exclusive.  They define conflicting usage for GPIO PC6.

#elif defined(ENABLE_OSC32K) && !defined(ENABLE_ALT_FUNCTION_NTX_ACTIVE)
  //Use OCS32K configuration
  #define PWRUP_CFG_BUTTON1  GPIOCFG_ANALOG
  #define PWRUP_OUT_BUTTON1  0
  #define PWRDN_CFG_BUTTON1  GPIOCFG_ANALOG
  #define PWRDN_OUT_BUTTON1  0

#elif !defined(ENABLE_OSC32K) && defined(ENABLE_ALT_FUNCTION_NTX_ACTIVE)
  //Use nTX_ACTIVE configuration
  #define PWRUP_CFG_BUTTON1  GPIOCFG_OUT_ALT
  #define PWRUP_OUT_BUTTON1  0
  #define PWRDN_CFG_BUTTON1  GPIOCFG_OUT
  #define PWRDN_OUT_BUTTON1  0

#else
  //Use Button1 configuration
  #define PWRUP_CFG_BUTTON1  GPIOCFG_IN_PUD
  #define PWRUP_OUT_BUTTON1  GPIOOUT_PULLUP /* Button needs a pullup */
  #define PWRDN_CFG_BUTTON1  GPIOCFG_IN_PUD
  #define PWRDN_OUT_BUTTON1  GPIOOUT_PULLUP /* Button needs a pullup */

#endif

/**
 * @brief Give GPIO PC7 configuration a friendly name.
 */
#ifdef  ENABLE_OSC32K
  #define CFG_TEMPEN         GPIOCFG_ANALOG
#else//!ENABLE_OSC32K
  #define CFG_TEMPEN         GPIOCFG_OUT
#endif//ENABLE_OSC32K
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


/** @name GPIO Configuration Macros
 *
 * These macros define the GPIO configuration and initial state of the output
 * registers for all the GPIO in the powerup and powerdown modes.
 *@{
 */


//Each pin has 4 cfg bits.  There are 3 ports with 2 cfg registers per
//port since each cfg register only holds 4 pins (16bits). Entries are retained
//for the unused pins on the 16-gpio variants, so the cfg arrays need to be 6
//entries of 16bits.
extern uint16_t gpioCfgPowerUp[6];
extern uint16_t gpioCfgPowerDown[6];
//Each pin has 1 out bit.  There are 3 ports with 1 out register per
//port (8bits).  Therefore, the out arrays need to be 3 entries of 8bits.
extern uint8_t gpioOutPowerUp[3];
extern uint8_t gpioOutPowerDown[3];
//A single mask variable covers all GPIO.
extern GpioMaskType gpioRadioPowerBoardMask;

/**
 * @brief Define the pins crucial to host-NCP communications over SPI.  This
 * mask is used to prevent the host from changing pin configurations
 * necessary to maintain host-NCP communications (otherwise changes would
 * break the link!).
 */
#define EZSP_PIN_BLACKOUT_MASK  ((((GpioMaskType)1)<<PORTB_PIN(2))|  /*HOST_MOSI */ \
                                 (((GpioMaskType)1)<<PORTB_PIN(1))|  /*HOST_MISO */ \
                                 (((GpioMaskType)1)<<PORTB_PIN(3))|  /*HOST_SCLK */ \
                                 (((GpioMaskType)1)<<PORTB_PIN(4))|  /*HOST_nSEL */ \
                                 (((GpioMaskType)1)<<PORTC_PIN(7))|  /*HOST_nINT */ \
                                 (((GpioMaskType)1)<<PORTB_PIN(0)))  /*HOST_nWAKE*/

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
GpioMaskType gpioRadioPowerBoardMask = 0

#define GPIOCFG_UNUSED      GPIOCFG_OUT
#define GPIOOUT_UNUSED      0

/**
 * @brief Initialize GPIO powerup configuration variables.
 */
#define DEFINE_POWERUP_GPIO_CFG_VARIABLES()                                   \
uint16_t gpioCfgPowerUp[6] = {                                                  \
                            ((GPIOCFG_UNUSED     <<PA0_CFG_BIT)|              \
                             (GPIOCFG_UNUSED     <<PA1_CFG_BIT)|              \
                             (GPIOCFG_UNUSED     <<PA2_CFG_BIT)|              \
                             (GPIOCFG_UNUSED     <<PA3_CFG_BIT)),             \
                            ((PWRUP_CFG_PTI_EN   <<PA4_CFG_BIT)|              \
                             (PWRUP_CFG_PTI_DATA <<PA5_CFG_BIT)|              \
                             (GPIOCFG_UNUSED     <<PA6_CFG_BIT)|              \
                             (PWRUP_CFG_DFL_RHO  <<PA7_CFG_BIT)),             \
                            ((GPIOCFG_IN_PUD     <<PB0_CFG_BIT)|              \
                             (GPIOCFG_OUT_ALT    <<PB1_CFG_BIT)| /* SC1MISO */\
                             (GPIOCFG_IN         <<PB2_CFG_BIT)| /* SC1MOSI */\
                             (GPIOCFG_IN         <<PB3_CFG_BIT)),/* SC1SCLK */\
                            ((GPIOCFG_IN         <<PB4_CFG_BIT)| /* SC1nSSEL*/\
                             (GPIOCFG_UNUSED     <<PB5_CFG_BIT)|              \
                             (GPIOCFG_UNUSED     <<PB6_CFG_BIT)|              \
                             (GPIOCFG_UNUSED     <<PB7_CFG_BIT)),             \
                            ((GPIOCFG_IN         <<PC0_CFG_BIT)|              \
                             (GPIOCFG_OUT        <<PC1_CFG_BIT)|              \
                             (GPIOCFG_OUT_ALT    <<PC2_CFG_BIT)|              \
                             (GPIOCFG_IN         <<PC3_CFG_BIT)),             \
                            ((GPIOCFG_IN         <<PC4_CFG_BIT)|              \
                             (PWRUP_CFG_LED2     <<PC5_CFG_BIT)|              \
                             (PWRUP_CFG_BUTTON1  <<PC6_CFG_BIT)|              \
                             (GPIOCFG_OUT        <<PC7_CFG_BIT))              \
                           }


/**
 * @brief Initialize GPIO powerup output variables.
 */
#define DEFINE_POWERUP_GPIO_OUTPUT_DATA_VARIABLES()                       \
uint8_t gpioOutPowerUp[3] = {                                               \
                            ((GPIOOUT_UNUSED     <<PA0_BIT)|              \
                             (GPIOOUT_UNUSED     <<PA1_BIT)|              \
                             (GPIOOUT_UNUSED     <<PA2_BIT)|              \
                             (GPIOOUT_UNUSED     <<PA3_BIT)|              \
                             (PWRUP_OUT_PTI_EN   <<PA4_BIT)|              \
                             (PWRUP_OUT_PTI_DATA <<PA5_BIT)|              \
                             (GPIOOUT_UNUSED     <<PA6_BIT)|              \
                             (PWRUP_OUT_DFL_RHO  <<PA7_BIT)),             \
                            ((1                  <<PB0_BIT)|              \
                             (1                  <<PB1_BIT)| /* SC1MISO */\
                             (1                  <<PB2_BIT)| /* SC1MOSI */\
                             (1                  <<PB3_BIT)| /* SC1SCLK */\
                             (1                  <<PB4_BIT)| /* SC1nSSEL*/\
                             (GPIOOUT_UNUSED     <<PB5_BIT)|              \
                             (GPIOOUT_UNUSED     <<PB6_BIT)|              \
                             (GPIOOUT_UNUSED     <<PB7_BIT)),             \
                            ((0                  <<PC0_BIT)|              \
                             (1                  <<PC1_BIT)|              \
                             (1                  <<PC2_BIT)|              \
                             (0                  <<PC3_BIT)|              \
                             (0                  <<PC4_BIT)|              \
                             (1                  <<PC5_BIT)|              \
                             (1                  <<PC6_BIT)|              \
                             (1                  <<PC7_BIT))              \
                           }


/**
 * @brief Initialize powerdown GPIO configuration variables.
 */
#define DEFINE_POWERDOWN_GPIO_CFG_VARIABLES()                                   \
uint16_t gpioCfgPowerDown[6] = {                                                  \
                              ((GPIOCFG_UNUSED     <<PA0_CFG_BIT)|              \
                               (GPIOCFG_UNUSED     <<PA1_CFG_BIT)|              \
                               (GPIOCFG_UNUSED     <<PA2_CFG_BIT)|              \
                               (GPIOCFG_UNUSED     <<PA3_CFG_BIT)),             \
                              ((PWRDN_CFG_PTI_EN   <<PA4_CFG_BIT)|              \
                               (PWRDN_CFG_PTI_DATA <<PA5_CFG_BIT)|              \
                               (GPIOCFG_UNUSED     <<PA6_CFG_BIT)|              \
                               (PWRDN_CFG_DFL_RHO  <<PA7_CFG_BIT)),             \
                              ((GPIOCFG_IN_PUD     <<PB0_CFG_BIT)|              \
                               (GPIOCFG_OUT_ALT    <<PB1_CFG_BIT)| /* SC1MISO */\
                               (GPIOCFG_IN         <<PB2_CFG_BIT)| /* SC1MOSI */\
                               (GPIOCFG_IN         <<PB3_CFG_BIT)),/* SC1SCLK */\
                              ((GPIOCFG_IN         <<PB4_CFG_BIT)| /* SC1nSSEL*/\
                               (GPIOCFG_UNUSED     <<PB5_CFG_BIT)|              \
                               (GPIOCFG_UNUSED     <<PB6_CFG_BIT)|              \
                               (GPIOCFG_UNUSED     <<PB7_CFG_BIT)),             \
                              ((GPIOCFG_IN         <<PC0_CFG_BIT)|              \
                               (GPIOCFG_OUT        <<PC1_CFG_BIT)|              \
                               (GPIOCFG_OUT_ALT    <<PC2_CFG_BIT)|              \
                               (GPIOCFG_IN         <<PC3_CFG_BIT)),             \
                              ((GPIOCFG_IN         <<PC4_CFG_BIT)|              \
                               (PWRDN_CFG_LED2     <<PC5_CFG_BIT)|              \
                               (PWRDN_CFG_BUTTON1  <<PC6_CFG_BIT)|              \
                               (GPIOCFG_OUT        <<PC7_CFG_BIT))              \
                             }

/**
 * @brief Initialize powerdown GPIO output variables.
 */
#define DEFINE_POWERDOWN_GPIO_OUTPUT_DATA_VARIABLES()                       \
uint8_t gpioOutPowerDown[3] = {                                               \
                              ((GPIOOUT_UNUSED     <<PA0_BIT)|              \
                               (GPIOOUT_UNUSED     <<PA1_BIT)|              \
                               (GPIOOUT_UNUSED     <<PA2_BIT)|              \
                               (GPIOOUT_UNUSED     <<PA3_BIT)|              \
                               (PWRDN_OUT_PTI_EN   <<PA4_BIT)|              \
                               (PWRDN_OUT_PTI_DATA <<PA5_BIT)|              \
                               (GPIOOUT_UNUSED     <<PA6_BIT)|              \
                               (PWRDN_OUT_DFL_RHO  <<PA7_BIT)),             \
                              ((1                  <<PB0_BIT)|              \
                               (1                  <<PB1_BIT)| /* SC1MISO */\
                               (1                  <<PB2_BIT)| /* SC1MOSI */\
                               (1                  <<PB3_BIT)| /* SC1SCLK */\
                               (1                  <<PB4_BIT)| /* SC1nSSEL*/\
                               (GPIOOUT_UNUSED     <<PB5_BIT)|              \
                               (GPIOOUT_UNUSED     <<PB6_BIT)|              \
                               (GPIOOUT_UNUSED     <<PB7_BIT)),             \
                              ((0                  <<PC0_BIT)|              \
                               (0                  <<PC1_BIT)|              \
                               (1                  <<PC2_BIT)|              \
                               (0                  <<PC3_BIT)|              \
                               (0                  <<PC4_BIT)|              \
                               (1                  <<PC5_BIT)|              \
                               (1                  <<PC6_BIT)|              \
                               (1                  <<PC7_BIT))              \
                            }

/**
 * @brief Set powerup GPIO configuration registers.
 */
#define SET_POWERUP_GPIO_CFG_REGISTERS()    \
        do {                                \
          GPIO_PACFGL = gpioCfgPowerUp[0];  \
          GPIO_PACFGH = gpioCfgPowerUp[1];  \
          GPIO_PBCFGL = gpioCfgPowerUp[2];  \
          GPIO_PBCFGH = gpioCfgPowerUp[3];  \
          GPIO_PCCFGL = gpioCfgPowerUp[4];  \
          GPIO_PCCFGH = gpioCfgPowerUp[5];  \
        } while (0)


/**
 * @brief Set powerup GPIO output registers.
 */
#define SET_POWERUP_GPIO_OUTPUT_DATA_REGISTERS()  \
        do {                                      \
          GPIO_PAOUT = gpioOutPowerUp[0];         \
          GPIO_PBOUT = gpioOutPowerUp[1];         \
          GPIO_PCOUT = gpioOutPowerUp[2];         \
        } while (0)


/**
 * @brief Set powerdown GPIO configuration registers.
 */
#define SET_POWERDOWN_GPIO_CFG_REGISTERS()    \
        do {                                  \
          GPIO_PACFGL = gpioCfgPowerDown[0];  \
          GPIO_PACFGH = gpioCfgPowerDown[1];  \
          GPIO_PBCFGL = gpioCfgPowerDown[2];  \
          GPIO_PBCFGH = gpioCfgPowerDown[3];  \
          GPIO_PCCFGL = gpioCfgPowerDown[4];  \
          GPIO_PCCFGH = gpioCfgPowerDown[5];  \
        } while (0)


/**
 * @brief Set powerdown GPIO output registers.
 */
#define SET_POWERDOWN_GPIO_OUTPUT_DATA_REGISTERS()  \
        do {                                        \
          GPIO_PAOUT = gpioOutPowerDown[0];         \
          GPIO_PBOUT = gpioOutPowerDown[1];         \
          GPIO_PCOUT = gpioOutPowerDown[2];         \
        } while (0)


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
#define SET_SUSPEND_GPIO_CFG_REGISTERS()      \
        do {                                  \
          /* GPIO_PACFGL USB untouched! */    \
          GPIO_PACFGH = gpioCfgPowerDown[1];  \
          GPIO_PBCFGL = gpioCfgPowerDown[2];  \
          GPIO_PBCFGH = gpioCfgPowerDown[3];  \
          GPIO_PCCFGL = gpioCfgPowerDown[4];  \
          GPIO_PCCFGH = gpioCfgPowerDown[5];  \
        } while (0)


/**
 * @brief Set suspend GPIO output registers. SET_POWERDOWN minus USB regs
 */
#define SET_SUSPEND_GPIO_OUTPUT_DATA_REGISTERS()            \
        do {                                                \
          GPIO_PAOUT = (GPIO_PAOUT & 0x0F) /*USB untouched*/\
                     |(gpioOutPowerDown[0] & 0xF0);         \
          GPIO_PBOUT = gpioOutPowerDown[1];                 \
          GPIO_PCOUT = gpioOutPowerDown[2];                 \
        } while (0);


/**
 * @brief External regulator enable/disable macro.
 */
#ifdef ENABLE_ALT_FUNCTION_REG_EN
  #define CONFIGURE_EXTERNAL_REGULATOR_ENABLE()  GPIO_DBGCFG |= GPIO_EXTREGEN;
#else
  #define CONFIGURE_EXTERNAL_REGULATOR_ENABLE()  GPIO_DBGCFG &= ~GPIO_EXTREGEN;
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
#define WAKE_ON_PA4   false
#define WAKE_ON_PA5   false
#define WAKE_ON_PA7   false
#define WAKE_ON_PB0   true
#define WAKE_ON_PB1   false
#define WAKE_ON_PB2   false
#define WAKE_ON_PB3   false
#define WAKE_ON_PB4   false
#define WAKE_ON_PC0   false
#define WAKE_ON_PC1   false
#define WAKE_ON_PC2   false
#define WAKE_ON_PC3   false
#define WAKE_ON_PC4   false
#define WAKE_ON_PC5   false
#define WAKE_ON_PC6   false
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
#define halInternalInitBoard()                                  \
        do {                                                    \
          halInternalPowerUpBoard();                            \
          /* halInternalInitButton(); */                        \
          halHostSerialInit(); /*initialize the SPI Protocol*/  \
          halInternalInitRadioHoldOff();                        \
       } while(0)

/**
 * @brief Power down the board.  This function is called from
 * ::halPowerDown().
 */
#define halInternalPowerDownBoard()                             \
        do {                                                    \
          /* Board peripheral deactivation */                   \
          /* halInternalSleepAdc(); */                          \
          halHostSerialPowerdown(); /*powerdown SPI Protocol*/  \
          SET_POWERDOWN_GPIO_OUTPUT_DATA_REGISTERS();           \
          SET_POWERDOWN_GPIO_CFG_REGISTERS();                   \
        } while(0)

/**
 * @brief Power up the board.  This function is called from
 * ::halPowerUp().
 */
#define halInternalPowerUpBoard()                                  \
        do {                                                       \
          SET_POWERUP_GPIO_OUTPUT_DATA_REGISTERS();                \
          SET_POWERUP_GPIO_CFG_REGISTERS();                        \
          /*The radio GPIO should remain in the powerdown state */ \
          /*until the stack specifically powers them up. */        \
          halStackRadioPowerDownBoard();                           \
          CONFIGURE_EXTERNAL_REGULATOR_ENABLE();                   \
          /* Board peripheral reactivation */                      \
          halHostSerialPowerup(); /*powerup the SPI Protocol*/     \
          halInternalInitAdc();                                    \
        } while(0)
/**
 * @brief Suspend the board.  This function is called from
 * ::halSuspend().
 */
#define halInternalSuspendBoard()                     \
        do {                                          \
          /* Board peripheral deactivation */         \
          /* halInternalSleepAdc(); */                \
          SET_SUSPEND_GPIO_OUTPUT_DATA_REGISTERS();   \
          SET_SUSPEND_GPIO_CFG_REGISTERS();           \
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
        } while(0)
//@} //END OF BOARD SPECIFIC FUNCTIONS

#endif //__BOARD_H__

/** @} END Board Specific Functions */

/** @} END addtogroup */
