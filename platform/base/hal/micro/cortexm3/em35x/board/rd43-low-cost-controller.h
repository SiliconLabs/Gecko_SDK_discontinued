/** @file hal/micro/cortexm3/em35x/board/rd43-low-cost-controller.h
 * See @ref board for detailed documentation.
 *
 * <!-- Copyright 2014 Silicon Laboratories, Inc.                        *80*-->
 */

/** @addtogroup board
 *  @brief Functions and definitions specific to the RD43 Low Cost Controller
 *  reference design
 *
 * 1. Infrared LED
 *    PB6 has been remapped from BUTTON0 to the IR LED.
 *    PB1 is reserved as IR_EN
 * 2. 7x7 Keypad             0   1   2   3   4   5   6
 *    Key Rows:     KR[0..6]=PC5,PC6,PC7,PC3,PB0,PC0,PB7 (outputs)
 *    Key Columns:  KC[0..6]=PA4,PA5,PA1,PA6,PA0,PB2,PB4 (inputs)
 * 3. LEDs
 *    PC6 has been remapped from BUTTON1 to Green LED
 *    PC7 has been remapped from TEMP_EN to Red LED
 *
 * Low Cost Controller
 * - PA0 - *KC4
 * - PA1 - *KC2
 * - PA2 - *LED_G
 * - PA3 - *LED_R
 * - PA4 - *KC0(PTI_EN)
 * - PA5 - *KC1(PTI_DATA)
 * - PA6 - *KC3
 * - PA7 - (open)
 * - PB0 - KR4
 * - PB1 - IR_EN(SC1TXD1)
 * - PB2 - *KC5(SC1RXD)
 * - PB3 - (grounded)(IN)
 * - PB4 - *KC6
 * - PB5 - (grounded)(IN)
 * - PB6 - IR_LED
 * - PB7 - KR6
 * - PC0 - KR5<JRST>
 * - PC1 - (grounded)(IN)
 * - PC2 - *<JTDO>
 * - PC3 - *KR3<JTDI>
 * - PC4 - *<JTMS>
 * - PC5 -  KR0
 * - PC6 - *KR1
 * - PC7 - *KR2
 *
 * Note: "<>" marked for "<Debug Pins>"
 * Note: "()" marked for optional function(s)
 * Note: "*"  marked for GPIO assignment changes in revB hardware
 *@{
 */

#ifndef __BOARD_H__
#define __BOARD_H__

/** @name General Application Defines
 *
 * The following defines are used to control
 */

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
  GREEN_LED   = PORTA_PIN(2),
  RED_LED     = PORTA_PIN(3),
  IR_LED      = PORTB_PIN(6),
  BOARDLED2   = RED_LED, // Used by identification client
};

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

/** @name Key Matrix Definitions
 *
 * @brief The actual GPIO KO[0..6] and KI[0..6] are connected to. This should
 * be used whenever referencing KEYOUT0..KEYOUT6 and KEYIN0..KEYIN6, such as
 * controlling if pieces are compiled in.
 * Remember that other pieces that might want to use IRQC.
 * 7x7 Keypad                 0   1   2   3   4   5   6
 *    Key Rows:     KR[0..6]=PC5,PC6,PC7,PC3,PB0,PC0,PB7 (outputs)
 *    Key Columns:  KC[0..6]=PA4,PA5,PA1,PA6,PA0,PB2,PB4 (inputs)
 */

/* Period between key matrix scans in active mode.
 * In deep sleep, the keys are not scanned but will wake-up on presses.
 */
#define KEY_MATRIX_IO_WAIT_TIME_US	20	// in us: wait time from column out stable to read
#define KEY_SCAN_PERIOD_MS          25  // in ms: time interval for key scanning


// Key-matrix dimension
#define KEY_MATRIX_NUM_COLUMNS  7
#define KEY_MATRIX_NUM_ROWS     7
#define KEY_MATRIX_NUM_KEYS     (KEY_MATRIX_NUM_COLUMNS*KEY_MATRIX_NUM_ROWS)

// Key row and column GPIO pins
#define KEY_ROW_PINS { \
  PORTC_PIN(5), PORTC_PIN(6), PORTC_PIN(7), PORTC_PIN(3), PORTB_PIN(0), PORTC_PIN(0), PORTB_PIN(7) \
}
#define KEY_COLUMN_PINS { \
  PORTA_PIN(4), PORTA_PIN(5), PORTA_PIN(1), PORTA_PIN(6), PORTA_PIN(0), PORTB_PIN(2), PORTB_PIN(4) \
}

// 7x7 matrix   7     6     5     4     3     2     1     0
//     Rows:   ---   PB7, *PC0,  PB0, *PC3,  PC7,  PC6,  PC5 (outputs)
//  Columns:   ---   PB4, #PB2,  PA0,  PA6,  PA1, *PA5, *PA4 (inputs)
#define KEY_ROW_PINS_MASK      0x7F // 7 key row pins
#define KEY_COLUMN_PINS_MASK   0x7F // 7 key column pins
#define KEY_ROW_DEBUG_PINS     0x28 // KR3(JTDI),KR5(JRST) are debug pins
#if (APP_SERIAL==1)                 // UART1 as application serial port
#define KEY_COLUMN_DEBUG_PINS  0x23 // KC0(PTI_EN),KC1(PTI_DATA), KC5(SCR1RXD) are debug pins
#else
#define KEY_COLUMN_DEBUG_PINS  0x03 // KC0(PTI_EN),KC1(PTI_DATA) are debug pins
#endif


// Key matrix scan code definition: range from 0 to (Num_Rows x Num_Columns)-1
// K[row,column] scancode mappings
/*  K[0,0]   K[0,1]   K[0,2]   K[0,3]   K[0,4]   K[0,5]   K[0,6]
 *  K_ENTER  K_3      K_STOP   K_NXT_M  K_INPUT  K_INFO   ---
 *  K[1,0]   K[1,1]   K[1,2]   K[1,3]   K[1,4]   K[1,5]   K[1,6]
 *  K_9      K_C      K_FSTFW  K_CH_DN  K_CH_UP  K_EXIT   K_STB
 *  K[2,0]   K[2,1]   K[2,2]   K[2,3]   K[2,4]   K[2,5]   K[2,6]
 *  K_0      K_6      K_D      K_PAUSE  K_RIGHT  K_LAST   K_SETUP
 *  K[3,0]   K[3,1]   K[3,2]   K[3,3]   K[3,4]   K[3,5]   K[3,6]
 *  K_8      K_2      K_PLAY   K_DOWN   K_UP     K_GUIDE  K_POWER
 *  K[4,0]   K[4,1]   K[4,2]   K[4,3]   K[4,4]   K[4,5]   K[4,6]
 *  K_5      K_B      K_REWND  K_OK     K_LEFT   K_MUTE   K_TV
 *  K[5,0]   K[5,1]   K[5,2]   K[5,3]   K[5,4]   K[5,5]   K[5,6]
 *  K_PAIR   K_4      K_A      K_PEV_M  K_VOLUP  K_MENU   ---
 *  K[6,0]   K[6,1]   K[6,2]   K[6,3]   K[6,4]   K[6,5]   K[6,6]
 *  K_7      K_1      K_REC    K_VOLDN  K_FMT    K_LIST   ---
 */
enum {
  K_ENTER = 0,  // 0: ENTER
  K_3,          // 1: '3'
  K_STOP,       // 2: STOP
  K_NXT_M,      // 3: NextMedia(>>|)
  K_INPUT,      // 4: INPUT
  K_INFO,       // 5: INFO
  K_9 = 7,      // 7: '9'
  K_C,          // 8: 'C'(Red)
  K_FSTFW,      // 9: FastForward(>>)
  K_CH_DN,      //10: Channel-
  K_CH_UP,      //11: Channel+
  K_EXIT,       //12: EXIT
  K_STB,        //13: STB
  K_0,          //14: '0'
  K_6,          //15: '6'
  K_D,          //16: 'D'(Green)
  K_PAUSE,      //17: PAUSE
  K_RIGHT,      //18: RIGHT
  K_LAST,       //19: LAST
  K_SETUP,      //20: SETUP
  K_8,          //21: '8'
  K_2,          //22: '2'
  K_PLAY,       //23: PLAY
  K_DOWN,       //24: DOWN
  K_UP,         //25: UP
  K_GUIDE,      //26: GUIDE
  K_POWER,      //27: POWER
  K_5,          //28: '5'
  K_B,          //29: 'B'(Blue)
  K_REWND,      //30: Rewind(<<)
  K_OK,         //31: OK
  K_LEFT,       //32: LEFT
  K_MUTE,       //33: MUTE
  K_TV,         //34: TV
  K_PAIR,       //35: PAIR
  K_4,          //36: '4'
  K_A,          //37: 'A'(Yellow)
  K_PEV_M,      //38: PreviousMedia(|<<)
  K_VOLUP,      //39: Volume+
  K_MENU,       //40: MENU
  K_7 = 42,     //42: '7'
  K_1,          //43: '1'
  K_REC,        //44: RECORD
  K_VOLDN,      //45: Volume-
  K_FMT,        //46: FORMAT
  K_LIST,       //47: LIST
};

// User SETUP key as pairing key in debug mode as regular PAIR key is used
// by the debug lines.
#define DBG_KEY_PAIR    ((GPIO_DBGSTAT & GPIO_FORCEDBG)? K_SETUP : K_PAIR)

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
#define DEFINE_POWERUP_GPIO_CFG_VARIABLES()                                         \
uint16_t gpioCfgPowerUp[6] = {                                                        \
                            ((GPIOCFG_IN_PUD     <<PA0_CFG_BIT)| /* KC4           */\
                             (GPIOCFG_IN_PUD     <<PA1_CFG_BIT)| /* KC2           */\
                             (GPIOCFG_OUT        <<PA2_CFG_BIT)| /* LED_G         */\
                             (GPIOCFG_OUT        <<PA3_CFG_BIT)),/* LED_R         */\
                            ((PWRUP_CFG_PTI_EN   <<PA4_CFG_BIT)| /* PTI_EN(KC0)   */\
                             (PWRUP_CFG_PTI_DATA <<PA5_CFG_BIT)| /* PTI_DATA(KC1) */\
                             (GPIOCFG_IN_PUD     <<PA6_CFG_BIT)| /* KC3           */\
                             (GPIOCFG_OUT        <<PA7_CFG_BIT)),/*(LED-RCM)      */\
                            ((GPIOCFG_OUT        <<PB0_CFG_BIT)| /* KR4           */\
                             (PWRUP_CFG_SC1_TXD  <<PB1_CFG_BIT)| /* SC1TXD(IR_EN) */\
                             (GPIOCFG_IN_PUD     <<PB2_CFG_BIT)| /* SC1RXD(KC5)   */\
                             (GPIOCFG_IN         <<PB3_CFG_BIT)),/*(To GND)       */\
                            ((GPIOCFG_IN_PUD     <<PB4_CFG_BIT)| /* KC6           */\
                             (GPIOCFG_IN         <<PB5_CFG_BIT)| /*(To GND)       */\
                             (GPIOCFG_OUT_ALT    <<PB6_CFG_BIT)| /* IR_LED        */\
                             (GPIOCFG_OUT        <<PB7_CFG_BIT)),/* KR6           */\
                            ((GPIOCFG_IN         <<PC0_CFG_BIT)| /* JRST(KR5)     */\
                             (GPIOCFG_IN         <<PC1_CFG_BIT)| /*(To GND)       */\
                             (GPIOCFG_OUT_ALT    <<PC2_CFG_BIT)| /* JTDO          */\
                             (GPIOCFG_IN         <<PC3_CFG_BIT)),/* JTDI(KR3)     */\
                            ((GPIOCFG_IN         <<PC4_CFG_BIT)| /* JTMS          */\
                             (GPIOCFG_OUT        <<PC5_CFG_BIT)| /* KR0           */\
                             (GPIOCFG_OUT        <<PC6_CFG_BIT)| /* KR1           */\
                             (GPIOCFG_OUT        <<PC7_CFG_BIT)) /* KR2           */\
                           }

/**
 * @brief Initialize GPIO powerup output variables.
 */
#define DEFINE_POWERUP_GPIO_OUTPUT_DATA_VARIABLES()                            \
uint8_t gpioOutPowerUp[3] = {                                                    \
                           ((GPIOOUT_PULLDOWN   <<PA0_BIT)| /* KC4           */\
                            (GPIOOUT_PULLDOWN   <<PA1_BIT)| /* KC2           */\
                            (1                  <<PA2_BIT)| /* LED_G         */\
                            (1                  <<PA3_BIT)| /* LED_R         */\
                            (PWRUP_OUT_PTI_EN   <<PA4_BIT)| /* PTI_EN(KC0)   */\
                            (PWRUP_OUT_PTI_DATA <<PA5_BIT)| /* PTI_DATA(KC1) */\
                            (GPIOOUT_PULLDOWN   <<PA6_BIT)| /* KC3           */\
                            (1                  <<PA7_BIT)),/*(LED-RCM)      */\
                           ((1                  <<PB0_BIT)| /* KR4           */\
                            (1                  <<PB1_BIT)| /* SC1TXD(IR_EN) */\
                            (1                  <<PB2_BIT)| /* SC1RXD(KC5)   */\
                            (0                  <<PB3_BIT)| /*(To GND)       */\
                            (GPIOOUT_PULLDOWN   <<PB4_BIT)| /* KC6           */\
                            (0                  <<PB5_BIT)| /*(To GND)       */\
                            (0                  <<PB6_BIT)| /* IR_LED        */\
                            (1                  <<PB7_BIT)),/* KR6           */\
                           ((0                  <<PC0_BIT)| /* JRST(KR5)     */\
                            (0                  <<PC1_BIT)| /*(To GND)       */\
                            (1                  <<PC2_BIT)| /* JTDO          */\
                            (0                  <<PC3_BIT)| /* JTDI(KR3)     */\
                            (0                  <<PC4_BIT)| /* JTMS          */\
                            (1                  <<PC5_BIT)| /* KR0           */\
                            (1                  <<PC6_BIT)| /* KR1           */\
                            (1                  <<PC7_BIT)) /* KR2           */\
                          }

/**
 * @brief Initialize powerdown GPIO configuration variables.
 */
#define DEFINE_POWERDOWN_GPIO_CFG_VARIABLES()                                       \
uint16_t gpioCfgPowerDown[6] = {                                                      \
                            ((GPIOCFG_IN_PUD     <<PA0_CFG_BIT)| /* KC4           */\
                             (GPIOCFG_IN_PUD     <<PA1_CFG_BIT)| /* KC2           */\
                             (GPIOCFG_OUT        <<PA2_CFG_BIT)| /* LED_G         */\
                             (GPIOCFG_OUT        <<PA3_CFG_BIT)),/* LED_R         */\
                            ((PWRDN_CFG_PTI_EN   <<PA4_CFG_BIT)| /* PTI_EN(KC0)   */\
                             (PWRDN_CFG_PTI_DATA <<PA5_CFG_BIT)| /* PTI_DATA(KC1) */\
                             (GPIOCFG_IN_PUD     <<PA6_CFG_BIT)| /* KC3           */\
                             (GPIOCFG_OUT        <<PA7_CFG_BIT)),/*(LED-RCM)      */\
                            ((GPIOCFG_OUT        <<PB0_CFG_BIT)| /* KR4           */\
                             (GPIOCFG_OUT        <<PB1_CFG_BIT)| /* SC1TXD(IR_EN) */\
                             (GPIOCFG_IN_PUD     <<PB2_CFG_BIT)| /* SC1RXD(KC5)   */\
                             (GPIOCFG_IN         <<PB3_CFG_BIT)),/*(To GND)       */\
                            ((GPIOCFG_IN_PUD     <<PB4_CFG_BIT)| /* KC6           */\
                             (GPIOCFG_IN         <<PB5_CFG_BIT)| /*(To GND)       */\
                             (GPIOCFG_OUT        <<PB6_CFG_BIT)| /* IR_LED        */\
                             (GPIOCFG_OUT        <<PB7_CFG_BIT)),/* KR6           */\
                            ((GPIOCFG_IN_PUD     <<PC0_CFG_BIT)| /* JRST(KR5)     */\
                             (GPIOCFG_IN         <<PC1_CFG_BIT)| /*(To GND)       */\
                             (GPIOCFG_OUT        <<PC2_CFG_BIT)| /* JTDO          */\
                             (GPIOCFG_OUT        <<PC3_CFG_BIT)),/* JTDI(KR3)     */\
                            ((GPIOCFG_IN_PUD     <<PC4_CFG_BIT)| /* JTMS          */\
                             (GPIOCFG_OUT        <<PC5_CFG_BIT)| /* KR0           */\
                             (GPIOCFG_OUT        <<PC6_CFG_BIT)| /* KR1           */\
                             (GPIOCFG_OUT        <<PC7_CFG_BIT)) /* KR2           */\
                           }

/**
 * @brief Initialize powerdown GPIO output variables.
 */
#define DEFINE_POWERDOWN_GPIO_OUTPUT_DATA_VARIABLES()                          \
uint8_t gpioOutPowerDown[3] = {                                                  \
                           ((GPIOOUT_PULLDOWN   <<PA0_BIT)| /* KC4           */\
                            (GPIOOUT_PULLDOWN   <<PA1_BIT)| /* KC2           */\
                            (1                  <<PA2_BIT)| /* LED_G         */\
                            (1                  <<PA3_BIT)| /* LED_R         */\
                            (PWRDN_OUT_PTI_EN   <<PA4_BIT)| /* PTI_EN(KC0)   */\
                            (PWRDN_OUT_PTI_DATA <<PA5_BIT)| /* PTI_DATA(KC1) */\
                            (GPIOOUT_PULLDOWN   <<PA6_BIT)| /* KC3           */\
                            (1                  <<PA7_BIT)),/*(LED-RCM)      */\
                           ((1                  <<PB0_BIT)| /* KR4           */\
                            (1                  <<PB1_BIT)| /* SC1TXD(IR_EN) */\
                            (GPIOOUT_PULLUP     <<PB2_BIT)| /* SC1RXD(KC5)   */\
                            (0                  <<PB3_BIT)| /*(To GND)       */\
                            (GPIOOUT_PULLDOWN   <<PB4_BIT)| /* KC6           */\
                            (0                  <<PB5_BIT)| /*(To GND)       */\
                            (0                  <<PB6_BIT)| /* IR_LED        */\
                            (1                  <<PB7_BIT)),/* KR6           */\
                           ((GPIOOUT_PULLDOWN   <<PC0_BIT)| /* JRST(KR5)     */\
                            (0                  <<PC1_BIT)| /*(To GND)       */\
                            (1                  <<PC2_BIT)| /* JTDO          */\
                            (1                  <<PC3_BIT)| /* JTDI(KR3)     */\
                            (GPIOOUT_PULLDOWN   <<PC4_BIT)| /* JTMS          */\
                            (1                  <<PC5_BIT)| /* KR0           */\
                            (1                  <<PC6_BIT)| /* KR1           */\
                            (1                  <<PC7_BIT)) /* KR2           */\
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
 * @brief Set powerup GPIO output registers. LEDs are kept low if cleared.
 */
#define SET_POWERUP_GPIO_OUTPUT_DATA_REGISTERS() \
  GPIO_PAOUT = gpioOutPowerUp[0] & (GPIO_PAOUT | ~(1<<PA2_BIT | 1<<PA3_BIT));  \
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
 * @brief Set powerdown GPIO output registers. LEDs are kept low if cleared.
 */
#define SET_POWERDOWN_GPIO_OUTPUT_DATA_REGISTERS() \
  GPIO_PAOUT = gpioOutPowerDown[0] & (GPIO_PAOUT | ~(1<<PA2_BIT | 1<<PA3_BIT));\
  GPIO_PBOUT = gpioOutPowerDown[1];                \
  GPIO_PCOUT = gpioOutPowerDown[2];


/**
 * @brief Set resume GPIO configuration registers. Identical to SET_POWERUP
 */
#define SET_RESUME_GPIO_CFG_REGISTERS()  \
  GPIO_PACFGL = gpioCfgPowerUp[0];       \
  GPIO_PACFGH = gpioCfgPowerUp[1];       \
  GPIO_PBCFGL = gpioCfgPowerUp[2];       \
  GPIO_PBCFGH = gpioCfgPowerUp[3];       \
  GPIO_PCCFGL = gpioCfgPowerUp[4];       \
  GPIO_PCCFGH = gpioCfgPowerUp[5];


/**
 * @brief Set resume GPIO output registers. LEDs are kept high if set
 */
#define SET_RESUME_GPIO_OUTPUT_DATA_REGISTERS()  \
  GPIO_PAOUT = gpioOutPowerUp[0] & (GPIO_PAOUT | ~(1<<PA2_BIT | 1<<PA3_BIT));  \
  GPIO_PBOUT = gpioOutPowerUp[1];                \
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
 */
#define SET_SUSPEND_GPIO_OUTPUT_DATA_REGISTERS()   \
  GPIO_PAOUT = gpioOutPowerDown[1] & (GPIO_PAOUT | ~(1<<PA2_BIT | 1<<PA3_BIT));\
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
// it is fine to set all key row and columns pins as wake up pins
#define WAKE_ON_PA0   true  // KC4
#define WAKE_ON_PA1   true  // KC2
#define WAKE_ON_PA2   false // LED_G
#define WAKE_ON_PA3   false // LED_R
#define WAKE_ON_PA4   true  // KC0[PTI_EN]
#define WAKE_ON_PA5   true  // KC1[PTI_DATA]
#define WAKE_ON_PA6   true  // KC3
#define WAKE_ON_PA7   false //(debug LED)
#define WAKE_ON_PB0   true  // KR4
#define WAKE_ON_PB1   false // IR_EN*
#if defined(SLEEPY_EZSP_UART) || SLEEPY_IP_MODEM_UART
  #define WAKE_ON_PB2 true  // KC5(SC1RXD)
#else
  #define WAKE_ON_PB2 true  // KC5(SC1RXD)
#endif
#define WAKE_ON_PB3   false //(GND)
#define WAKE_ON_PB4   true  // KC6
#define WAKE_ON_PB5   false //(GND)
#define WAKE_ON_PB6   false // IR_LED
#define WAKE_ON_PB7   true  // KR6
#define WAKE_ON_PC0   true  // KR5<JRST>
#define WAKE_ON_PC1   false //(GND)
#define WAKE_ON_PC2   false // <JTDO>
#define WAKE_ON_PC3   true  // KR3<JTDI>
#define WAKE_ON_PC4   false // <JTMS>
#define WAKE_ON_PC5   true  // KR0
#define WAKE_ON_PC6   true  // KR1
#define WAKE_ON_PC7   true  // KR2
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
#ifndef EZSP_ASH
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
#define halInternalResumeBoard()                                   \
        do {                                                       \
          SET_RESUME_GPIO_OUTPUT_DATA_REGISTERS()                  \
          SET_RESUME_GPIO_CFG_REGISTERS()                          \
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

