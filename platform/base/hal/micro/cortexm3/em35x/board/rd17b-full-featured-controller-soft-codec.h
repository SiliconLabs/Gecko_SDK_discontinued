/** @file hal/micro/cortexm3/em35x/board/rd17b-full-featured-controller-soft-codec.h
 *
 * <!-- Copyright 2013 Silicon Laboratories, Inc.                        *80*-->
 */

/** @addtogroup board
 *  @brief Functions and definitions specific to the full featured remote
 *  without a hardware codec
 *
 * The EM341 on the Full Featured Remote board (no hardware codec) has the 
 * following GPIO configuration. This board file and the default HAL setup 
 * reflects this configuration.
 *
 * - PA0 - NOT USED
 * - PA1 - KEY_COL6 / AUDIO_MCLK        (Shared pin)
 * - PA2 - KEY_COL4 / AUDIO_BCLK        (Shared pin)
 * - PA3 - KEY_COL2 / AUDIO_WCLK        (Shared pin)
 * - PA4 - KEY_BACKLIGHT
 * - PA5 - PWR_BACKLIGHT
 * - PA6 - MICROPHONE ENABLE
 * - PA7 - BUZZER
 * - PB0 - KEY_ROW4
 * - PB1 - SC1TXD / SDA
 * - PB2 - SC1RXD / SCL
 * - PB3 - IR2
 * - PB4 - KEY_ROW1
 * - PB5 - ACC_INT
 * - PB6 - IR1
 * - PB7 - KEY_ROW6
 * - PC0 - KEY_ROW5 / JTAG_JRST         (Shared pin)
 * - PC1 - KEY_ROW2
 * - PC2 - KEY_COL3 / JTAG_JTDO / SWO   (Shared pin)
 * - PC3 - KEY_COL5 / JTAG_JTDI         (Shared pin)
 * - PC4 - KEY_ROW3 / JTAG_JTMS / SWDIO (Shared pin)
 * - PC5 - KEY_ROW0
 * - PC6 - KEY_COL0
 * - PC7 - KEY_COL1
 *
 *@{
 */

#ifndef __BOARD_H__
#define __BOARD_H__

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
 */
enum HalBoardLedPins {
  KEY_BACKLIGHT     = PORTA_PIN(4),
  POWER_LED         = PORTA_PIN(5),
  IR_LED            = POWER_LED,
  ACCELEROMETER_LED = KEY_BACKLIGHT,
  BOARDLED2         = KEY_BACKLIGHT
};

// LEDs are active high

#define LED_ACTIVE_HIGH

// No activity LED
#define NO_LED true

// No Radio Hold Off
#define halInternalInitRadioHoldOff()

//@} //END LED DEFINITIONS

/** @name Button Definitions
 *
 * The following definitions are only included because they are needed by the
 * button-stub plug-in.
 */
//@{
#define BUTTON0  PORTB_PIN(5)
#define BUTTON1  PORTB_PIN(6)

//@} //END OF BUTTON DEFINITIONS

/** @name Buzzer Definitions
 */
//@{

/** @name Microphone Definitions
 *
 * Definitions for the microphone
 */
//@{

#define microphoneEnable()  do {GPIO_PASET = PA6_MASK;} while(0)
#define microphoneDisable() do {GPIO_PACLR = PA6_MASK;} while(0)

//@} //END OF MICROPHONE DEFINITIONS

/** @name Buzzer Definitions
 */
//@{

// User TIMER1 Channel 4 for buzzer output. Redefining the following:
#define BUZZER_OUTPUT_MODE            (0x3 << TIM_OC4M_BIT)
#define BUZZER_OUTPUT_ENABLE_CHANNEL  TIM_CC4E
#define BUZZER_OUTPUT_CFG             TIM1_CCMR2

//@} //END OF BUZZER DEFINITIONS

/** @name Accelerometer Definitions
 *
 * The following are used to aid in the abstraction with the accelerometer
 * interrupts via GPIOs.
 *
 * @note The GPIO number must match the IRQ letter
 */
//@{
/**
 * @brief The number of seconds the accelerometer should be inactive for before
 * the backlight LEDs are turned off.
 */
#define ACC_INACTIVITY_TIMOUT_SEC 5
/**
 * @brief The actual GPIO ACC_INT is connected to.  This define should
 * be used whenever referencing ACC_INT.
 */
#define ACC_INT_PIN         PORTB_PIN(5)
/**
 * @brief The GPIO pin mask for ACC_INT
 */
#define ACC_INT_PINMASK     PB5_MASK
/**
* @brief The GPIO input register for ACC_INT.
 */
#define ACC_INT_IN          GPIO_PBIN
/**
 * @brief Point the proper IRQ at the desired pin for ACC_INT.
 * Remember that other pieces that might want to use IRQC.
 */
#define ACC_INT_SEL()       do { GPIO_IRQCSEL = PORTB_PIN(5); } while(0)
/**
 * @brief The interrupt service routine for ACC_INT.
 */
#define ACC_INT_ISR         halIrqCIsr
/**
 * @brief The interrupt configuration register for ACC_INT.
 */
#define ACC_INT_INTCFG      GPIO_INTCFGC
/**
 * @brief The interrupt enable bit for ACC_INT.
 */
#define ACC_INT_INT_EN_BIT  INT_IRQC
/**
 * @brief The interrupt flag bit for ACC_INT.
 */
#define ACC_INT_FLAG_BIT    INT_IRQCFLAG
/**
 * @brief The missed interrupt bit for ACC_INT.
 */
#define ACC_INT_MISS_BIT    INT_MISSIRQC

//@} //END OF ACCELEROMETER DEFINITIONS

/** @name Key Matrix Definitions
 *
 *  Key matrix rows    [0..6]: PC5, PB4, PC1, PC4, PB0, PC0, PB7 (inputs)
 *  Key matrix columms [0..6]: PC6, PC7, PA3, PC2, PA2, PC3, PA1 (outputs)
 *  When debug is enabled, pins PC0, PC2, PC3, and PC4 cannot be used
 */

// Key matrix is scanned using columns as outputs and active low
#define KEY_MATRIX_COLUMNS_AS_OUTPUTS 1
#define KEY_MATRIX_PINS_ACTIVE_HIGH   0

/* Period between key matrix scans in active mode.
 * In deep sleep, the keys are not scanned but will wake-up on presses.
 */
#define KEY_MATRIX_IO_WAIT_TIME_US 20	// Wait time before input is stable
#define KEY_SCAN_PERIOD_MS         50  // Key scanning interval
#define KEY_MATRIX_NUM_ROWS        7
#define KEY_MATRIX_NUM_COLUMNS     7
#define KEY_MATRIX_NUM_KEYS        (KEY_MATRIX_NUM_COLUMNS * KEY_MATRIX_NUM_ROWS)

// Key row and column GPIO pins
#define KEY_ROW_PINS { \
  PORTC_PIN(5), PORTB_PIN(4), PORTC_PIN(1), PORTC_PIN(4), PORTB_PIN(0), PORTC_PIN(0), PORTB_PIN(7) \
}
#define KEY_COLUMN_PINS { \
  PORTC_PIN(6), PORTC_PIN(7), PORTA_PIN(3), PORTC_PIN(2), PORTA_PIN(2), PORTC_PIN(3), PORTA_PIN(1) \
}
// Pins that are used for debug are marked with (*)
// 7x7 matrix	7     6     5     4     3     2     1     0
//     Rows:   ---   PB7, *PC0,  PB0, *PC4,  PC1,  PB4,  PC5 (inputs)
//  Columns:   ---   PA1, *PC3,  PA2, *PC2,  PA3,  PC7,  PC6 (outputs)
#define KEY_ROW_PINS_MASK      0x7F // 7 key row pins
#define KEY_COLUMN_PINS_MASK   0x7F // 7 key column pins
#define KEY_ROW_DEBUG_PINS     0x28 // KR3(JTMS), KR5(JRST) are debug pins
#define KEY_COLUMN_DEBUG_PINS  0x28 // KC3(JTDO), KC3(JTDI) are debug pins


  // 7 x 7 matrix
#define SET_KEY_ROW_PINS const uint32_t keyRowPins[KEY_MATRIX_NUM_ROWS] = \
  { PORTC_PIN(5), PORTB_PIN(4), PORTC_PIN(1), PORTC_PIN(4), PORTB_PIN(0), PORTC_PIN(0), PORTB_PIN(7) };
#define SET_KEY_COLUMN_PINS  const uint32_t keyColumnPins[KEY_MATRIX_NUM_COLUMNS] = \
  { PORTC_PIN(6), PORTC_PIN(7), PORTA_PIN(3), PORTC_PIN(2), PORTA_PIN(2), PORTC_PIN(3), PORTA_PIN(1) };

// Key matrix scan code definition: range from 1 to (Num_Columns x Num_Rows)
// K[column,row] scancode mappings (Note: 0 means no key mapped)
/*  K[0,0]    K[0,1]    K[0,2]    K[0,3]    K[0,4]    K[0,5]    K[0,6]
 *  K_REWIND, K_VOICE,  K_PLAY,   K_FSTFRWD,K_LIST,   K_0,      K_ENTER,
 *  K[1,0]    K[1,1]    K[1,2]    K[1,3]    K[1,4]    K[1,5]    K[1,6]
 *  K_STOP,   K_B,      K_C,      K_RECORD, K_7,      K_8,      K_9,
 *  K[2,0]    K[2,1]    K[2,2]    K[2,3]    K[2,4]    K[2,5]    K[2,6]
 *  K_A,      0,        K_DOWN,   K_D,      K_4,      K_5,      K_6,
 *  K[3,0]    K[3,1]    K[3,2]    K[3,3]    K[3,4]    K[3,5]    K[3,6]
 *  K_GUIDE,  LEFT,     K_OK,     K_EXIT,   K_1,      K_2,      K_3,
 *  K[4,0]    K[4,1]    K[4,2]    K[4,3]    K[4,4]    K[4,5]    K[4,6]
 *  K_MENU,   0,        K_RIGHT,  K_INFO,   K_VOLDOWN,K_MUTE,   K_CHLDOWN,
 *  K[5,0]    K[5,1]    K[5,2]    K[5,3]    K[5,4]    K[5,5]    K[5,6]
 *  K_SETUP,  K_UP,     0,        K_INPUT,  K_VOLUP,  K_LAST,   K_CHLUP,
 *  K[6,0]    K[6,1]    K[6,2]    K[6,3]    K[6,4]    K[6,5]    K[6,6]
 *  K_PAIRING,K_TV,     K_STB,    K_POWER,  K_REPLAY, K_PAUSE,  K_ADVANCE
 */

// Index defines for special non-mappable keys
#define KEY_MATRIX_SET_KEY      6
#define KEY_MATRIX_VOICE_KEY    7
#define KEY_MATRIX_TV_KEY      13
#define KEY_MATRIX_STB_KEY     20

// If VOICE key is sticky, voice streaming starts when VOICE button is pressed
// and continues after it is released. It is stopped when the VOICE button is
// pressed a second time. If the VOICE key is not sticky it will only stream
// voice while the VOICE key is held down.
#define VOICE_KEY_IS_STICKY false

//@} //END OF KEY MATRIX DEFINITIONS

//@{
/**
 * @brief This define does not equate to anything.  It is used as a
 * trigger to enable Packet Trace support on the breakout board (dev0680).
 */
#define PACKET_TRACE  // We do have PACKET_TRACE support
//@} //END OF PACKET TRACE DEFINITIONS

/** @name DISABLE_INTERNAL_1V8_REGULATOR
 *
 * When DISABLE_INTERNAL_1V8_REGULATOR is defined, the internal regulator for
 * the 1.8 V supply (VREG_1V8) is disabled.  Disabling of VREG_1V8 will
 * prevent excess current draw.
 *
 * @note Disabling VREG_1V8 on devices that are not externally powered will
 * prevent the device from operating normally.
 *
 * The disabling occurs early in the board power up sequence so that current
 * consumption is always minimized and the configuration will be applied
 * across all power modes.
 *
 */
//@{
/**
 * @brief This define does not equate to anything.  It is used as a
 * trigger to disable the 1.8V regulator.
 */
//#define DISABLE_INTERNAL_1V8_REGULATOR
//@} //END OF DISABLE_INTERNAL_1V8_REGULATOR DEFINITION

/**
 * @brief External regulator enable/disable macro.
 */
#ifdef DISABLE_INTERNAL_1V8_REGULATOR
  #define CONFIGURE_VREG_1V8_DISABLE()                                         \
    VREG = ( ( VREG & (~(VREG_VREG_1V8_EN_MASK | VREG_VREG_1V8_TEST_MASK)) ) | \
           (0 << VREG_VREG_1V8_EN_BIT)                                       | \
           (1 << VREG_VREG_1V8_TEST_BIT)                                     )
#else
  #define CONFIGURE_VREG_1V8_DISABLE()
#endif

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
#define DEFINE_POWERUP_GPIO_CFG_VARIABLES()                                   \
uint16_t gpioCfgPowerUp[6] = {                                                  \
      ((GPIOCFG_IN_PUD     <<PA0_CFG_BIT)|  /* NOT USED */                    \
       (GPIOCFG_OUT        <<PA1_CFG_BIT)|  /* KEY_COL6 / AUDIO_MCLK  */      \
       (GPIOCFG_OUT        <<PA2_CFG_BIT)|  /* KEY_COL4 / AUDIO_BCLK */       \
       (GPIOCFG_OUT        <<PA3_CFG_BIT)), /* KEY_COL2 / AUDIO_WCLK  */      \
      ((GPIOCFG_OUT        <<PA4_CFG_BIT)|  /* KEY_BACKLIGHT */               \
       (GPIOCFG_OUT        <<PA5_CFG_BIT)|  /* PWR_BACKLIGHT */               \
       (GPIOCFG_OUT        <<PA6_CFG_BIT)|  /* MICROPHONE ENABLE */           \
       (GPIOCFG_OUT_ALT    <<PA7_CFG_BIT)), /* BUZZER */                      \
      ((GPIOCFG_IN_PUD     <<PB0_CFG_BIT)|  /* KEY_ROW4 */                    \
       (GPIOCFG_IN         <<PB1_CFG_BIT)|  /* SC1TXD / SDA */                \
       (GPIOCFG_IN         <<PB2_CFG_BIT)|  /* SC1RXD / SCL */                \
       (GPIOCFG_OUT        <<PB3_CFG_BIT)), /* IR2 */                         \
      ((GPIOCFG_IN_PUD     <<PB4_CFG_BIT)|  /* KEY_ROW1 */                    \
       (GPIOCFG_IN         <<PB5_CFG_BIT)|  /* ACC_INT */                     \
       (GPIOCFG_OUT        <<PB6_CFG_BIT)|  /* IR1 */                         \
       (GPIOCFG_IN_PUD     <<PB7_CFG_BIT)), /* KEY_ROW6 */                    \
      ((GPIOCFG_IN         <<PC0_CFG_BIT)|  /* KEY_ROW5 / JTAG_JRST */        \
       (GPIOCFG_IN_PUD     <<PC1_CFG_BIT)|  /* KEY_ROW2 */                    \
       (GPIOCFG_OUT_ALT    <<PC2_CFG_BIT)|  /* KEY_COL3 / JTAG_JTDO / SWO */  \
       (GPIOCFG_IN         <<PC3_CFG_BIT)), /* KEY_COL5 / JTAG_JTDI */        \
      ((GPIOCFG_IN         <<PC4_CFG_BIT)|  /* KEY_ROW3 / JTAG_JTMS / SWDIO */\
       (GPIOCFG_IN_PUD     <<PC5_CFG_BIT)|  /* KEY_ROW0 */                    \
       (GPIOCFG_OUT        <<PC6_CFG_BIT)|  /* KEY_COL0 */                    \
       (GPIOCFG_OUT        <<PC7_CFG_BIT))  /* KEY_COL1 */                    \
       }

/**
 * @brief Initialize GPIO powerup output variables.
 */
#define DEFINE_POWERUP_GPIO_OUTPUT_DATA_VARIABLES()                           \
uint8_t gpioOutPowerUp[3] = {                                                   \
     ((GPIOOUT_PULLDOWN   <<PA0_BIT)| /* NOT USED */                          \
      (0                  <<PA1_BIT)| /* KEY_COL6 / AUDIO_MCLK  */            \
      (0                  <<PA2_BIT)| /* KEY_COL4 / AUDIO_BCLK */             \
      (0                  <<PA3_BIT)| /* KEY_COL2 / AUDIO_WCLK  */            \
      (0                  <<PA4_BIT)| /* KEY_BACKLIGHT */                     \
      (0                  <<PA5_BIT)| /* PWR_BACKLIGHT */                     \
      (0                  <<PA6_BIT)| /* MICROPHONE ENABLE*/                  \
      (0                  <<PA7_BIT)),/* BUZZER */                            \
     ((GPIOOUT_PULLUP     <<PB0_BIT)| /* KEY_ROW4 */                          \
      (1                  <<PB1_BIT)| /* SC1TXD / SDA */                      \
      (1                  <<PB2_BIT)| /* SC1RXD / SCL */                      \
      (0                  <<PB3_BIT)| /* IR2 */                               \
      (GPIOOUT_PULLUP     <<PB4_BIT)| /* KEY_ROW1 */                          \
      (0                  <<PB5_BIT)| /* ACC_INT */                           \
      (0                  <<PB6_BIT)| /* IR1 */                               \
      (GPIOOUT_PULLUP     <<PB7_BIT)),/* KEY_ROW6 */                          \
     ((0                  <<PC0_BIT)| /* KEY_ROW5 / JTAG_JRST */              \
      (GPIOOUT_PULLUP     <<PC1_BIT)| /* KEY_ROW2 */                    \
      (1                  <<PC2_BIT)| /* KEY_COL3 / JTAG_JTDO / SWO */        \
      (0                  <<PC3_BIT)| /* KEY_COL5 / JTAG_JTDI */              \
      (0                  <<PC4_BIT)| /* KEY_ROW3 / JTAG_JTMS / SWDIO */      \
      (GPIOOUT_PULLUP     <<PC5_BIT)| /* KEY_ROW0 */                          \
      (0                  <<PC6_BIT)| /* KEY_COL0 */                          \
      (0                  <<PC7_BIT)) /* KEY_COL1 */                          \
      }


/**
 * @brief Initialize powerdown GPIO configuration variables.
 */
#define DEFINE_POWERDOWN_GPIO_CFG_VARIABLES()                                 \
uint16_t gpioCfgPowerDown[6] = {                                                \
      ((GPIOCFG_IN_PUD     <<PA0_CFG_BIT)| /* NOT USED */                     \
       (GPIOCFG_OUT        <<PA1_CFG_BIT)| /* KEY_COL6 / AUDIO_MCLK  */       \
       (GPIOCFG_OUT        <<PA2_CFG_BIT)| /* KEY_COL4 / AUDIO_BCLK */        \
       (GPIOCFG_OUT        <<PA3_CFG_BIT)),/* KEY_COL2 / AUDIO_WCLK  */       \
      ((GPIOCFG_OUT        <<PA4_CFG_BIT)| /* KEY_BACKLIGHT */                \
       (GPIOCFG_OUT        <<PA5_CFG_BIT)| /* PWR_BACKLIGHT */                \
       (GPIOCFG_OUT        <<PA6_CFG_BIT)| /* MICROPHONE ENABLE */            \
       (GPIOCFG_OUT        <<PA7_CFG_BIT)),/* BUZZER */                       \
      ((GPIOCFG_IN_PUD     <<PB0_CFG_BIT)| /* KEY_ROW4 */                     \
       (GPIOCFG_IN         <<PB1_CFG_BIT)| /* SC1TXD / SDA */                 \
       (GPIOCFG_IN         <<PB2_CFG_BIT)| /* SC1RXD / SCL */                 \
       (GPIOCFG_OUT        <<PB3_CFG_BIT)),/* IR2 */                          \
      ((GPIOCFG_IN_PUD     <<PB4_CFG_BIT)| /* KEY_ROW1 */                     \
       (GPIOCFG_IN         <<PB5_CFG_BIT)| /* ACC_INT */                      \
       (GPIOCFG_OUT        <<PB6_CFG_BIT)| /* IR1 */                          \
       (GPIOCFG_IN_PUD     <<PB7_CFG_BIT)),/* KEY_ROW6 */                     \
      ((GPIOCFG_IN_PUD     <<PC0_CFG_BIT)| /* KEY_ROW5 / JTAG_JRST */         \
       (GPIOCFG_IN_PUD     <<PC1_CFG_BIT)| /* KEY_ROW2 */                     \
       (GPIOCFG_OUT        <<PC2_CFG_BIT)| /* KEY_COL3 / JTAG_JTDO / SWO */   \
       (GPIOCFG_OUT        <<PC3_CFG_BIT)),/* KEY_COL5 / JTAG_JTDI */         \
      ((GPIOCFG_IN_PUD     <<PC4_CFG_BIT)| /* KEY_ROW3 / JTAG_JTMS / SWDIO */ \
       (GPIOCFG_IN_PUD     <<PC5_CFG_BIT)| /* KEY_ROW0 */                     \
       (GPIOCFG_OUT        <<PC6_CFG_BIT)| /* KEY_COL0 */                     \
       (GPIOCFG_OUT        <<PC7_CFG_BIT)) /* KEY_COL1 */                     \
       }

/**
 * @brief Initialize powerdown GPIO output variables.
 */
#define DEFINE_POWERDOWN_GPIO_OUTPUT_DATA_VARIABLES()                         \
uint8_t gpioOutPowerDown[3] = {                                                 \
        ((GPIOOUT_PULLDOWN    <<PA0_BIT)| /* NOT USED */                      \
         (0                   <<PA1_BIT)| /* KEY_COL6 / AUDIO_MCLK  */        \
         (0                   <<PA2_BIT)| /* KEY_COL4 / AUDIO_BCLK */         \
         (0                   <<PA3_BIT)| /* KEY_COL2 / AUDIO_WCLK  */        \
         (0                   <<PA4_BIT)| /* KEY_BACKLIGHT */                 \
         (0                   <<PA5_BIT)| /* PWR_BACKLIGHT */                 \
         (0                   <<PA6_BIT)| /* MICROPHONE ENABLE */             \
         (0                   <<PA7_BIT)),/* BUZZER */                        \
        ((GPIOOUT_PULLUP      <<PB0_BIT)| /* KEY_ROW4 */                      \
         (1                   <<PB1_BIT)| /* SC1TXD / SDA */                  \
         (1                   <<PB2_BIT)| /* SC1RXD / SCL */                  \
         (0                   <<PB3_BIT)| /* IR2 */                           \
         (GPIOOUT_PULLUP      <<PB4_BIT)| /* KEY_ROW1 */                      \
         (0                   <<PB5_BIT)| /* ACC_INT */                       \
         (0                   <<PB6_BIT)| /* IR1 */                           \
         (GPIOOUT_PULLUP      <<PB7_BIT)),/* KEY_ROW6 */                      \
        ((GPIOOUT_PULLDOWN    <<PC0_BIT)| /* KEY_ROW5 / JTAG_JRST */          \
         (GPIOOUT_PULLUP      <<PC1_BIT)| /* KEY_ROW2 */                \
         (1                   <<PC2_BIT)| /* KEY_COL3 / JTAG_JTDO / SWO */    \
         (1                   <<PC3_BIT)| /* KEY_COL5 / JTAG_JTDI */          \
         (GPIOOUT_PULLDOWN    <<PC4_BIT)| /* KEY_ROW3 / JTAG_JTMS / SWDIO */  \
         (GPIOOUT_PULLUP      <<PC5_BIT)| /* KEY_ROW0 */                      \
         (0                   <<PC6_BIT)| /* KEY_COL0 */                      \
         (0                   <<PC7_BIT)) /* KEY_COL1 */                      \
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
 * @brief Set powerup GPIO output registers. LEDs are kept high if set
 */
#define SET_POWERUP_GPIO_OUTPUT_DATA_REGISTERS() \
  GPIO_PAOUT = gpioOutPowerUp[0] | (GPIO_PAOUT & (1<<PA4_BIT | 1 <<PA5_BIT));               \
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
 * @brief Set powerdown GPIO output registers. LEDs are kept high if set
 */
#define SET_POWERDOWN_GPIO_OUTPUT_DATA_REGISTERS() \
  GPIO_PAOUT = gpioOutPowerDown[0] | (GPIO_PAOUT & (1<<PA4_BIT | 1 <<PA5_BIT));\
  GPIO_PBOUT = gpioOutPowerDown[1];                \
  GPIO_PCOUT = gpioOutPowerDown[2];

/**
 * @brief Set resume GPIO configuration registers. Identical to SET_POWERUP
 */
#define SET_RESUME_GPIO_CFG_REGISTERS() \
  GPIO_PACFGL = gpioCfgPowerUp[0];       \
  GPIO_PACFGH = gpioCfgPowerUp[1];       \
  GPIO_PBCFGL = gpioCfgPowerUp[2];       \
  GPIO_PBCFGH = gpioCfgPowerUp[3];       \
  GPIO_PCCFGL = gpioCfgPowerUp[4];       \
  GPIO_PCCFGH = gpioCfgPowerUp[5];


/**
 * @brief Set resume GPIO output registers. LEDs are kept high if set
 */
#define SET_RESUME_GPIO_OUTPUT_DATA_REGISTERS() \
  GPIO_PAOUT = gpioOutPowerUp[0] | (GPIO_PAOUT & (1<<PA4_BIT | 1 <<PA5_BIT));               \
  GPIO_PBOUT = gpioOutPowerUp[1];               \
  GPIO_PCOUT = gpioOutPowerUp[2];


/**
 * @brief Set suspend GPIO configuration registers. Identical to SET_POWERDOWN
 */
#define SET_SUSPEND_GPIO_CFG_REGISTERS()   \
  GPIO_PACFGH = gpioCfgPowerDown[1];       \
  GPIO_PBCFGL = gpioCfgPowerDown[2];       \
  GPIO_PBCFGH = gpioCfgPowerDown[3];       \
  GPIO_PCCFGL = gpioCfgPowerDown[4];       \
  GPIO_PCCFGH = gpioCfgPowerDown[5];


/**
 * @brief Set suspend GPIO output registers. Identical to SET_POWERDOWN
 * except for LEDs which are left high if set
 */
#define SET_SUSPEND_GPIO_OUTPUT_DATA_REGISTERS() \
  GPIO_PAOUT = gpioOutPowerDown[0] | (GPIO_PAOUT & (1<<PA4_BIT | 1 <<PA5_BIT));             \
  GPIO_PBOUT = gpioOutPowerDown[1];              \
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
#define WAKE_ON_PA0   false  // PA0 - NOT USED
#define WAKE_ON_PA1   false  // KEY_COL6 / AUDIO_MCLK
#define WAKE_ON_PA2   false  // KEY_COL4 / AUDIO_BCLK
#define WAKE_ON_PA3   false  // KEY_COL2 / AUDIO_WCLK
#define WAKE_ON_PA4   false  // KEY_BACKLIGHT
#define WAKE_ON_PA5   false  // PWR_BACKLIGHT
#define WAKE_ON_PA6   false  // MICROPHONE ENABLE
#define WAKE_ON_PA7   false  // BUZZER
#define WAKE_ON_PB0   true   // KEY_ROW4
#define WAKE_ON_PB1   false  // SC1TXD
#define WAKE_ON_PB2   false  // SC1RXD
#define WAKE_ON_PB3   false  // IR2
#define WAKE_ON_PB4   true   // KEY_ROW1
#define WAKE_ON_PB5   true   // ACC_INT
#define WAKE_ON_PB6   false  // IR1
#define WAKE_ON_PB7   true   // KEY_ROW6
#define WAKE_ON_PC0   true   // KEY_ROW5 / JTAG_JRST
#define WAKE_ON_PC1   true   // KEY_ROW2
#define WAKE_ON_PC2   false  // KEY_COL3 / JTAG_JTDO / SWO
#define WAKE_ON_PC3   false  // KEY_COL5 / JTAG_JTDI
#define WAKE_ON_PC4   true   // KEY_ROW3 / JTAG_JTMS / SWDIO
#define WAKE_ON_PC5   true   // KEY_ROW0
#define WAKE_ON_PC6   false  // KEY_COL0
#define WAKE_ON_PC7   false  // KEY_COL1
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
#ifndef EZSP_UART
  #define halInternalInitBoard()                                  \
          do {                                                    \
            halInternalPowerUpBoard();                            \
            halInternalInitRadioHoldOff();                        \
            halInternalInitButton();                              \
         } while(0)
#else
  #define halInternalInitBoard()                                  \
          do {                                                    \
            halInternalPowerUpBoard();                            \
            halInternalInitRadioHoldOff();                        \
         } while(0)
#endif

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
 * @brief Suspend the board.  This function is called from
 * ::halSuspend().
 */
#define halInternalSuspendBoard()                   \
        do {                                          \
          /* Board peripheral deactivation */         \
          /* halInternalSleepAdc(); */                \
          SET_SUSPEND_GPIO_OUTPUT_DATA_REGISTERS()  \
          SET_SUSPEND_GPIO_CFG_REGISTERS()          \
        } while(0)

/**
 * @brief Power up the board.  This function is called from
 * ::halPowerUp().
 */
#define halInternalPowerUpBoard()                                  \
        do {                                                       \
          CONFIGURE_VREG_1V8_DISABLE();                            \
          SET_POWERUP_GPIO_OUTPUT_DATA_REGISTERS()                 \
          SET_POWERUP_GPIO_CFG_REGISTERS()                         \
          /*The radio GPIO should remain in the powerdown state */ \
          /*until the stack specifically powers them up. */        \
          halStackRadioPowerDownBoard();                           \
          CONFIGURE_EXTERNAL_REGULATOR_ENABLE()                    \
          /* Board peripheral reactivation */                      \
          halInternalInitAdc();                                    \
        } while(0)

/**
 * @brief Resume the board.  This function is called from
 * ::halResume().
 */
#define halInternalResumeBoard()                                  \
        do {                                                       \
          SET_RESUME_GPIO_OUTPUT_DATA_REGISTERS()                 \
          SET_RESUME_GPIO_CFG_REGISTERS()                         \
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
