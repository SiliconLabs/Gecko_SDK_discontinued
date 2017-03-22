/** @file hal/micro/cortexm3/em35x/board/ist-a30.h
 * See @ref board for detailed documentation.
 *
 * <!-- Copyright 2008-2011 by Ember Corporation. All rights reserved.   *80*-->
 */

/** @addtogroup board
 *  @brief Functions and definitions specific to the contact sensor reference
 *  design.
 *
 * @note The file ist-a30.h is intended to be copied, renamed, and
 * customized for customer-specific hardware.
 *
 * The file ist-a30.h is the default BOARD_HEADER file used with the contact
 * sensor reference design.
 *
 * The EM35x on a ist-a30 has the following example GPIO configuration.
 * This board file and the default HAL setup reflects this configuration.
 * - PA0 - SC2MOSI for optional external flash
 * - PA1 - SC2MISO for optional external flash
 * - PA2 - SC2SCLK for optional external flash
 * - PA3 - SC2nSSEL for optional external flash
 * - PA4 - Packet Trace
 * - PA5 - Packet Trace
 * - PA6 - User Interface LED
 * - PA7 - Flash power enable
 * - PB0 - Optional Reed Switch MCU Pull up
 * - PB1 - SC1SDA for temperature sensor
 * - PB2 - SC1SCL for temperature sensor
 * - PB3 - Optional Reed Switch MCU Pull down
 * - PB4 - No Connect
 * - PB5 - No Connect
 * - PB6 - User interface button
 * - PB7 - Optional Thermistor input, ADC-
 * - PC0 - JTAG (JRST)
 * - PC1 - Optional Thermistor input, ADC+
 * - PC2 - JTAG (JTDO) / SWO
 * - PC3 - JTAG (JTDI)
 * - PC4 - JTAG (JTMS)
 * - PC5 - TX Active
 * - PC6 - Reed Switch output
 * - PC7 - Optional Thermistor power enable
 *
 *@{
 */

#ifndef __BOARD_H__
#define __BOARD_H__

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
//#define EMBER_SERIAL_BAUD_CUSTOM  13
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
 * \b Note: This reference design contains only one LED, tied to PA6
 *
 * \b Note: The BOARD_ACTIVITY and HEARTBEAT leds are not used in this design,
 * which requires the NO_LED macro to be defined
 */
//@{

/**
 * @brief Assign each GPIO with an LED connected to a convenient name.
 * ::BOARD_ACTIVITY_LED and ::BOARD_HEARTBEAT_LED provide a further layer of
 * abstraction on top of the 3 LEDs for verbose coding.
 */
typedef enum HalBoardLedPins {
  BOARDLED0 = PORTA_PIN(6),
} HalBoardLedPins_t;

/**
 * @brief Inform the led-blink plugin that this board only has one LED
 */
#define MAX_LED_NUMBER        1

/**
 * @brief Inform the framework that the BOARD_ACTIVITY_LED should not be used
 * to indicate network traffic in this design.
 */
#define NO_LED                1
#define BOARD_ACTIVITY_LED    BOARDLED0

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
#define BUTTON0             PORTB_PIN(6)
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
//@} //END OF BUTTON DEFINITIONS

/** @name GPIO Sensor Definitions
 *
 * The following are used to aid in the abstraction with the GPIO Sensor
 * connections.  The microcontroller-specific sources use these
 * definitions so they are able to work across a variety of boards
 * which could have different connections.  The names and ports/pins
 * used below are intended to match with a schematic of the system to
 * provide the abstraction.
 *
 */
//@{
/**
 * @brief The GPIO pin used for the magnetic reed contact switch
 */
#define GPIO_SENSOR_PIN  6

/**
 * @brief The GPIO port used for the magnetic reed contact switch
 */
#define GPIO_SENSOR_PORT HAL_GIC_GPIO_PORTC

/**
 * @brief The IRQ used to observe changes in the magnetic reed contact switch
 */
#define GPIO_SENSOR_IRQ  HAL_GIC_IRQ_NUMC

/**
 * @brief Macro necessary to get GIC to properly tie into IRQC for reed switch
 */
#define HAL_GIC_USE_IRQC_ISR
//@} //END OF GPIO SENSOR DEFINITIONS

/** @name Battery Monitor pin definitions
 *
 * The following are used to aid in the abstraction with the battery monitor
 * connections.  The microcontroller-specific sources use these
 * definitions so they are able to work across a variety of boards
 * which could have different connections.  The names and ports/pins
 * used below are intended to match with a schematic of the system to
 * provide the abstraction.
 *
 */
//@{
/**
 * @brief The TX Active pin
 */
#define BATTERY_MONITOR_TX_ACTIVE_PIN  5

/**
 * @brief The TX Active port
 */
#define BATTERY_MONITOR_TX_ACTIVE_PORT HAL_GIC_GPIO_PORTC

/**
 * @brief The IRQ used to trigger the battery monitor event
 */
#define BATTERY_MONITOR_IRQ  HAL_GIC_IRQ_NUMD

/**
 * @brief Macro necessary to get GIC to properly tie into IRQD
 */
#define HAL_GIC_USE_IRQD_ISR
//@} //END OF BATTERY MONITOR DEFINITIONS

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
 * trigger to enable Packet Trace support on the breakout board (ist-a30).
 */
#define PACKET_TRACE  // We do have PACKET_TRACE support
//@} //END OF PACKET TRACE DEFINITIONS

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


/** @name LED2 Defines
 * Configure the pin for LED2.
 *@{
 */
/**
 * @brief Give the LED2 configuration a friendly name.
 */
#define PWRUP_CFG_LED2  GPIOCFG_OUT
#define PWRUP_OUT_LED2  1  /* LED default off */
#define PWRDN_CFG_LED2  GPIOCFG_OUT
#define PWRDN_OUT_LED2  1  /* LED default off */
//@} END OF LED2 Configuration Defines


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
//A single mask variable covers all 24 GPIO.
extern uint32_t gpioRadioPowerBoardMask;


/**
 * @brief Define the mask for GPIO relevant to the radio in the context
 * of power state.  Each bit in the mask indicates the corresponding GPIO
 * which should be affected when invoking halStackRadioPowerUpBoard() or
 * halStackRadioPowerDownBoard().
 */
#define DEFINE_GPIO_RADIO_POWER_BOARD_MASK_VARIABLE() \
uint32_t gpioRadioPowerBoardMask = 0


/**
 * @brief Initialize GPIO powerup configuration variables.
 */
#define DEFINE_POWERUP_GPIO_CFG_VARIABLES()     \
uint16_t gpioCfgPowerUp[6] = {                    \
                            ((GPIOCFG_IN_PUD     <<PA0_CFG_BIT)|              \
                             (GPIOCFG_IN_PUD     <<PA1_CFG_BIT)|              \
                             (GPIOCFG_IN_PUD     <<PA2_CFG_BIT)|              \
                             (GPIOCFG_IN_PUD     <<PA3_CFG_BIT)),             \
                            ((GPIOCFG_OUT_ALT    <<PA4_CFG_BIT)| /* PTI_EN */ \
                             (GPIOCFG_OUT_ALT    <<PA5_CFG_BIT)| /* PTI_DATA*/\
                             (GPIOCFG_OUT        <<PA6_CFG_BIT)| /* LED */    \
                             (GPIOCFG_OUT        <<PA7_CFG_BIT)),             \
                            ((GPIOCFG_IN_PUD     <<PB0_CFG_BIT)|              \
                             (GPIOCFG_OUT_ALT_OD <<PB1_CFG_BIT)| /* SC1SDA */ \
                             (GPIOCFG_OUT_ALT    <<PB2_CFG_BIT)| /* SC1SCL  */\
                             (GPIOCFG_IN_PUD     <<PB3_CFG_BIT)),/* SC1nCTS */\
                            ((GPIOCFG_IN_PUD     <<PB4_CFG_BIT)|              \
                             (GPIOCFG_IN_PUD     <<PB5_CFG_BIT)|              \
                             (GPIOCFG_IN_PUD     <<PB6_CFG_BIT)|              \
                             (GPIOCFG_IN_PUD     <<PB7_CFG_BIT)),             \
                            ((GPIOCFG_IN_PUD     <<PC0_CFG_BIT)|              \
                             (GPIOCFG_IN_PUD     <<PC1_CFG_BIT)|              \
                             (GPIOCFG_OUT_ALT    <<PC2_CFG_BIT)|              \
                             (GPIOCFG_IN         <<PC3_CFG_BIT)),             \
                            ((GPIOCFG_IN_PUD     <<PC4_CFG_BIT)|              \
                             (GPIOCFG_OUT_ALT    <<PC5_CFG_BIT)| /* TX_ACT */ \
                             (GPIOCFG_IN         <<PC6_CFG_BIT)| /* reed */   \
                             (GPIOCFG_OUT        <<PC7_CFG_BIT)) /* TEMP_EN */\
                           }


/**
 * @brief Initialize GPIO powerup output variables.
 */
#define DEFINE_POWERUP_GPIO_OUTPUT_DATA_VARIABLES()                       \
uint8_t gpioOutPowerUp[3] = {                                               \
                           ((GPIOOUT_PULLUP     <<PA0_BIT)|               \
                            (GPIOOUT_PULLUP     <<PA1_BIT)|               \
                            (GPIOOUT_PULLUP     <<PA2_BIT)|               \
                            (GPIOOUT_PULLUP     <<PA3_BIT)|               \
                            (PWRUP_OUT_PTI_EN   <<PA4_BIT)|               \
                            (PWRUP_OUT_PTI_DATA <<PA5_BIT)|               \
                            (1                  <<PA6_BIT)| /* LED */     \
                            (0                  <<PA7_BIT)),              \
                           ((GPIOOUT_PULLUP     <<PB0_BIT)|               \
                            (1                  <<PB1_BIT)| /* SC1TXD  */ \
                            (1                  <<PB2_BIT)| /* SC1RXD  */ \
                            (GPIOOUT_PULLUP     <<PB3_BIT)|               \
                            (GPIOOUT_PULLUP     <<PB4_BIT)|               \
                            (GPIOOUT_PULLUP     <<PB5_BIT)|               \
                            /* PB6 has button needing a pullup */         \
                            (GPIOOUT_PULLUP     <<PB6_BIT)|               \
                            (GPIOOUT_PULLDOWN   <<PB7_BIT)),              \
                           ((1                  <<PC0_BIT)|               \
                            (GPIOOUT_PULLDOWN   <<PC1_BIT)|               \
                            (1                  <<PC2_BIT)|               \
                            (0                  <<PC3_BIT)|               \
                            (0                  <<PC4_BIT)|               \
                            (1                  <<PC5_BIT)|               \
                            (1                  <<PC6_BIT)|  /* reed  */  \
                            (0                  <<PC7_BIT))               \
                          }


/**
 * @brief Initialize powerdown GPIO configuration variables.
 */
#define DEFINE_POWERDOWN_GPIO_CFG_VARIABLES()                                  \
uint16_t gpioCfgPowerDown[6] = {                                                 \
                              ((GPIOCFG_IN_PUD     <<PA0_CFG_BIT)|             \
                               (GPIOCFG_IN_PUD     <<PA1_CFG_BIT)|             \
                               (GPIOCFG_IN_PUD     <<PA2_CFG_BIT)|             \
                               (GPIOCFG_IN_PUD     <<PA3_CFG_BIT)),            \
                              ((PWRDN_CFG_PTI_EN   <<PA4_CFG_BIT)|             \
                               (PWRDN_CFG_PTI_DATA <<PA5_CFG_BIT)|             \
                               (GPIOCFG_OUT        <<PA6_CFG_BIT)| /* LED */   \
                               (GPIOCFG_OUT        <<PA7_CFG_BIT)),            \
                              ((GPIOCFG_IN_PUD     <<PB0_CFG_BIT)|             \
                               (GPIOCFG_OUT        <<PB1_CFG_BIT)| /* SC1TXD */\
                               (GPIOCFG_OUT        <<PB2_CFG_BIT)| /* SC1RXD */\
                               (GPIOCFG_IN_PUD     <<PB3_CFG_BIT)),            \
                              ((GPIOCFG_IN_PUD     <<PB4_CFG_BIT)|             \
                               (GPIOCFG_IN_PUD     <<PB5_CFG_BIT)|             \
                               (GPIOCFG_IN_PUD     <<PB6_CFG_BIT)|             \
                               /* need to use pulldown for sleep */            \
                               (GPIOCFG_IN_PUD     <<PB7_CFG_BIT)),            \
                              ((GPIOCFG_IN_PUD     <<PC0_CFG_BIT)|             \
                               (GPIOCFG_IN_PUD     <<PC1_CFG_BIT)|             \
                               (GPIOCFG_OUT        <<PC2_CFG_BIT)|             \
                               (GPIOCFG_IN_PUD     <<PC3_CFG_BIT)),            \
                              ((GPIOCFG_IN_PUD     <<PC4_CFG_BIT)|             \
                               (GPIOCFG_OUT_ALT    <<PC5_CFG_BIT)|             \
                               (GPIOCFG_IN         <<PC6_CFG_BIT)|  /* reed */ \
                               (GPIOCFG_OUT        <<PC7_CFG_BIT))             \
                             }


/**
 * @brief Initialize powerdown GPIO output variables.
 */
#define DEFINE_POWERDOWN_GPIO_OUTPUT_DATA_VARIABLES()                       \
uint8_t gpioOutPowerDown[3] = {                                               \
                             ((GPIOOUT_PULLUP     <<PA0_BIT)|               \
                              (GPIOOUT_PULLUP     <<PA1_BIT)|               \
                              (GPIOOUT_PULLUP     <<PA2_BIT)|               \
                              (GPIOOUT_PULLUP     <<PA3_BIT)|               \
                              (PWRDN_OUT_PTI_EN   <<PA4_BIT)|               \
                              /* data is idle high */                       \
                              (PWRDN_OUT_PTI_DATA <<PA5_BIT)|               \
                              (1                  <<PA6_BIT)| /* LED */     \
                              /* LED off */                                 \
                              (0                  <<PA7_BIT)),              \
                             ((GPIOOUT_PULLUP     <<PB0_BIT)|               \
                              (GPIOOUT_PULLUP     <<PB1_BIT)|  /* SC1TXD  */\
                              (GPIOOUT_PULLUP     <<PB2_BIT)|  /* SC1RXD  */\
                              (GPIOOUT_PULLUP     <<PB3_BIT)|               \
                              (GPIOOUT_PULLUP     <<PB4_BIT)|               \
                              (GPIOOUT_PULLUP     <<PB5_BIT)|               \
                              /* PB6 has button needing a pullup */         \
                              (GPIOOUT_PULLUP     <<PB6_BIT)|               \
                              (GPIOOUT_PULLDOWN   <<PB7_BIT)),              \
                             ((GPIOOUT_PULLUP     <<PC0_BIT)|               \
                              (GPIOOUT_PULLDOWN   <<PC1_BIT)|               \
                              (1                  <<PC2_BIT)|               \
                              (GPIOOUT_PULLDOWN   <<PC3_BIT)|               \
                              (GPIOOUT_PULLDOWN   <<PC4_BIT)|               \
                              (1                  <<PC5_BIT)|               \
                              (1                  <<PC6_BIT)|  /* reed  */  \
                              (0                  <<PC7_BIT))  /*TEMP_EN */ \
                            }


/**
 * @brief Set powerup GPIO configuration registers.
 */
#define SET_POWERUP_GPIO_CFG_REGISTERS() \
  GPIO_PACFGL = gpioCfgPowerUp[0];       \
  GPIO_PACFGH = gpioCfgPowerUp[1];       \
  GPIO_PBCFGL = gpioCfgPowerUp[2];       \
  GPIO_PBCFGH = gpioCfgPowerUp[3];       \
  GPIO_PCCFGL = gpioCfgPowerUp[4];       \
  GPIO_PCCFGH = gpioCfgPowerUp[5];


/**
 * @brief Set powerup GPIO output registers.
 */
#define SET_POWERUP_GPIO_OUTPUT_DATA_REGISTERS() \
  GPIO_PAOUT = gpioOutPowerUp[0];                \
  GPIO_PBOUT = gpioOutPowerUp[1];                \
  GPIO_PCOUT = gpioOutPowerUp[2];


/**
 * @brief Set powerdown GPIO configuration registers.
 */
#define SET_POWERDOWN_GPIO_CFG_REGISTERS() \
  GPIO_PACFGL = gpioCfgPowerDown[0];       \
  GPIO_PACFGH = gpioCfgPowerDown[1];       \
  GPIO_PBCFGL = gpioCfgPowerDown[2];       \
  GPIO_PBCFGH = gpioCfgPowerDown[3];       \
  GPIO_PCCFGL = gpioCfgPowerDown[4];       \
  GPIO_PCCFGH = gpioCfgPowerDown[5];


/**
 * @brief Set powerdown GPIO output registers.
 */
#define SET_POWERDOWN_GPIO_OUTPUT_DATA_REGISTERS() \
  GPIO_PAOUT = gpioOutPowerDown[0];                \
  GPIO_PBOUT = gpioOutPowerDown[1];                \
  GPIO_PCOUT = gpioOutPowerDown[2];



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
#define WAKE_ON_PA0   false
#define WAKE_ON_PA1   false
#define WAKE_ON_PA2   false
#define WAKE_ON_PA3   false
#define WAKE_ON_PA4   false
#define WAKE_ON_PA5   false
#define WAKE_ON_PA6   false
#define WAKE_ON_PA7   false
#define WAKE_ON_PB0   false
#define WAKE_ON_PB1   false
#define WAKE_ON_PB2   false
#define WAKE_ON_PB3   false
#define WAKE_ON_PB4   false
#define WAKE_ON_PB5   false
#define WAKE_ON_PB6   true   //BUTTON0
#define WAKE_ON_PB7   false
#define WAKE_ON_PC0   false
#define WAKE_ON_PC1   false
#define WAKE_ON_PC2   false
#define WAKE_ON_PC3   false
#define WAKE_ON_PC4   false
#define WAKE_ON_PC5   true
#define WAKE_ON_PC6   true   //BUTTON1
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
          halInternalInitButton();                              \
       } while(0)

/**
 * @brief Power down the board.  This function is called from
 * ::halPowerDown().
 */
#define halInternalPowerDownBoard()                   \
        do {                                          \
          /* Board peripheral deactivation */         \
          /* halInternalSleepAdc(); */                \
          SET_POWERDOWN_GPIO_OUTPUT_DATA_REGISTERS()  \
          SET_POWERDOWN_GPIO_CFG_REGISTERS()          \
        } while(0)

/**
 * @brief Power up the board.  This function is called from
 * ::halPowerUp().
 */
#define halInternalPowerUpBoard()                                  \
        do {                                                       \
          SET_POWERUP_GPIO_OUTPUT_DATA_REGISTERS()                 \
          SET_POWERUP_GPIO_CFG_REGISTERS()                         \
          /*The radio GPIO should remain in the powerdown state */ \
          /*until the stack specifically powers them up. */        \
          halStackRadioPowerDownBoard();                           \
          CONFIGURE_EXTERNAL_REGULATOR_ENABLE()                    \
          /* Board peripheral reactivation */                      \
          halInternalInitAdc();                                    \
        } while(0)
//@} //END OF BOARD SPECIFIC FUNCTIONS

#endif //__BOARD_H__

/** @} END Board Specific Functions */

/** @} END addtogroup */

