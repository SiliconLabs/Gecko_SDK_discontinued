/** @file reference_design/ist_a0051/config/ist_a0051.h
 * EFR32 Smart Outlet Reference Design
 * See @ref board for detailed documentation.
 *
 * <!-- Copyright 2016 Silicon Laboratories, Inc.                        *80*-->
 */

/** @addtogroup board
 *  @brief Functions and definitions specific to the breakout board.
 *
 *@{
 */

#ifndef __BOARD_H__
#define __BOARD_H__
#include "bspconfig.h"
#include "mx25flash_config.h"


// This macro can be uncommented to set the DCDC regulator to bypass mode, which
// can be useful for board bringup and HW debugging, but is not recommended for
// release application usage due to the higher power consumption that will
// result from bypassing the DCDC regulator
//#define DISABLE_DCDC

/**
 * @brief Enable VCOM Pin, passthrough UART via the WSTK
 */
#define halEnableVCOM()  do {                                      \
    GPIO_PinModeSet(gpioPortA, 5, gpioModePushPull, 1);            \
    GPIO_PinOutSet(gpioPortA, 5);                                  \
  } while (0)
  
// Define the fact that we do not use the low frequency crystal
#define EMDRV_RTCDRV_USE_LFRCO

// Define board features

#define EMDRV_UARTDRV_HW_FLOW_CONTROL_ENABLE 0

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
 * \b Note: LEDs 0 and 1 are on the mainboard. There are no LEDs on the RCM
 *
 * \b Note: LEDs 2 and 3 simply redirects to LEDs 0 and 1.
 */
//@{

/**
 * @brief Assign each GPIO with an LED connected to a convenient name.
 * ::STATUS_LED and ::POWER_LED provide a further layer of
 * abstraction on top of the 3 LEDs for verbose coding.
 */
typedef enum HalBoardLedPins {
  BOARDLED0 = 0,
  BOARDLED1 = 1,
  STATUS_LED = BOARDLED0,
  POWER_LED = BOARDLED1
} HalBoardLedPins_t;

/**
 * @brief Inform the led-blink plugin that this board only has two LED
 */
#define MAX_LED_NUMBER        2

/**
 * @brief Inform the led-blink plugin that LEDs are active high
 */
#define LED_ACTIVE_HIGH

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
 */
//@{
/**
 * @brief The interrupt service routine for all buttons.
 */
#define BUTTON_ISR buttonIrqCallback
/**
 * @brief The actual GPIO BUTTON0 is connected to.  This define should
 * be used whenever referencing BUTTON0.
 */
#define BUTTON0         (BSP_GPIO_PB0_PIN)
/**
 * @brief The GPIO port for BUTTON0.
 */
#define BUTTON0_PORT        (BSP_GPIO_PB0_PORT)
//@} //END OF BUTTON DEFINITIONS

/** @name smart outlet configurations
 *
 * The following are used to abstract gpio contol of power replay and 
 * reset pin of the power sensor, it is specific for smart outlet project
 */
//@{
/**
 * @brief GPIO pin for relay on control
 */
#define P_RELAY_ON_PIN     (15)
#define P_RELAY_ON_PORT    (gpioPortB)
/**
 * @brief GPIO pin for relay off control
 */
#define P_RELAY_OFF_PIN     (14)
#define P_RELAY_OFF_PORT    (gpioPortB)
/**
 * @brief GPIO pin to reset the power sensor
 */
#define PWR_SEN_RESET_PIN    (5)
#define PWR_SEN_RESET_PORT   (gpioPortA)
//@} //END OF SMART OUTLET CONFIGURATION

/** @name SPI Definitions
 *
 * The following are used to aid in the abstraction with the SPI
 * peripheral. The microcontroller-specific sources use these
 * definitions so they are able to work across a variety of boards
 * which could have used different SPI ports. The names and ports/pins
 * used below are intended to match with a schematic of the system to
 * provide the abstraction.
 */
//@{
/**
 * @brief Specifies the SPI port(s) to be supported
 */

#define SPI_CONFIG                                                        \
{                                                                         \
  USART1,                       /* USART port                       */    \
  _USART_ROUTELOC0_TXLOC_LOC11, /* USART Tx pin location number     */    \
  _USART_ROUTELOC0_RXLOC_LOC11, /* USART Rx pin location number     */    \
  _USART_ROUTELOC0_CLKLOC_LOC11,/* USART Clk pin location number    */    \
  _USART_ROUTELOC0_CSLOC_LOC11, /* USART Cs pin location number     */    \
  50000,                        /* Bitrate                          */    \
  8,                            /* Frame length                     */    \
  0xFFFFFFFF,                   /* Dummy tx value for rx only funcs */    \
  spidrvMaster,                 /* SPI mode                         */    \
  spidrvBitOrderMsbFirst,       /* Bit order on bus                 */    \
  spidrvClockMode0,             /* SPI clock/phase mode             */    \
  spidrvCsControlApplication,   /* CS controlled by aplication      */    \
  spidrvSlaveStartImmediate     /* Slave start transfers immediately*/    \
}
/**
 * @brief GPIO port fot SPI 0 NSEL
 */
#define SPI_NSEL_PORT   (gpioPortF)
/**
 * @brief GPIO pin for SPI 0 NSEL
 */
#define SPI_NSEL_BIT    (7)
//@} //END OF SPI DEFINITIONS
//

#define halInternalConfigPa() do {                                 \
  RADIO_PAInit_t paInit = RADIO_PA_2P4_INIT;                       \
  RADIO_PA_Init(&paInit);                                          \
} while(0)

/** @name UART Definitions
 *
 * The following are used to aid in the abstraction with the UART.
 * The microcontroller-specific sources use these definitions so they are able 
 * to work across a variety of boards which could have different connections.  
 * The names and ports/pins used below are intended to match with a schematic 
 * of the system to provide the abstraction.
 *
 */
//@{
// Enable COM port to use retarget serial configuration
#define COM_RETARGET_SERIAL

//@} //END OF UART DEFINITIONS

/** @name External Flash Definitions
 *
 * The following are used to aid in the abstraction with the external flash.
 * The microcontroller-specific sources use these definitions so they are able 
 * to work across a variety of boards which could have different connections.  
 * The names and ports/pins used below are intended to match with a schematic 
 * of the system to provide the abstraction.
 *
 * This default configuration sets the location to the WSTK expansion header
 */
//@{
// Define external flash pins

#define EXTERNAL_FLASH_MOSI_PORT    MX25_PORT_MOSI
#define EXTERNAL_FLASH_MOSI_PIN     MX25_PIN_MOSI
#define EXTERNAL_FLASH_MISO_PORT    MX25_PORT_MISO
#define EXTERNAL_FLASH_MISO_PIN     MX25_PIN_MISO
#define EXTERNAL_FLASH_CLK_PORT     MX25_PORT_SCLK
#define EXTERNAL_FLASH_CLK_PIN      MX25_PIN_SCLK
#define EXTERNAL_FLASH_CS_PORT      MX25_PORT_CS
#define EXTERNAL_FLASH_CS_PIN       MX25_PIN_CS

//@} //END OF EXTERNAL FLASH DEFINITIONS
//

/**************            Common Configuration Options          **************/

/** @name Packet Trace
 *
 * Packet Trace Interface (PTI) will be turned on by default unless 
 * ::DISABLE_PTI is defined
 *
 * @note PTI uses PB11-13
 */
//@{
#ifndef DISABLE_PTI
#define halInternalConfigPti() do {                                \
  RADIO_PTIInit_t ptiInit = RADIO_PTI_INIT;                        \
  RADIO_PTI_Init(&ptiInit);                                        \
} while(0)
#else
#define halInternalConfigPti() do {                                \
  RADIO_PTIInit_t ptiInit = RADIO_PTI_INIT;                        \
  ptiInit.mode = RADIO_PTI_MODE_DISABLED;                          \
  RADIO_PTI_Init(&ptiInit);                                        \
} while(0)
#endif

//@} //END OF PACKET TRACE DEFINITIONS
//


/** @name DC-DC
 *
 * The DC to DC Interface will be turned on by default unless 
 * ::DISABLE_DCDC is defined, in which case the DC-DC will be put
 * in bypass mode.
 *
 * @note Use EMU_DCDCPowerOff() to poweroff the DC-DC if not connected
 * 
 * @note DC-DC configuration is write once on power-on, so a POR reset
 * is required to change DC-DC mode.
 */
//@{
#ifdef DISABLE_DCDC
#define halInternalEnableDCDC() do {                           \
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_WSTK_DEFAULT;   \
  dcdcInit.dcdcMode = emuDcdcMode_Bypass;                      \
  EMU_DCDCInit(&dcdcInit);                                     \
  } while(0)
#else
#define halInternalEnableDCDC() do {                           \
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_WSTK_DEFAULT;   \
  EMU_DCDCInit(&dcdcInit);                                     \
} while(0)
#endif
//@} //END OF DC-DC DEFINITIONS

/**************            Essential Board Functions             **************/

/**
 * @name halInternalInitBoard
 *
 * Called on startup from halInit(). Should contain calls to all board
 * specific startup sequences.
 * 
 */
#define halInternalInitBoard() do {                                \
 halInternalInitLed();                                             \
 halInternalInitButton();                                          \
 halInternalPowerUpBoard();                                        \
 halInternalConfigPti();                                           \
 halInternalConfigPa();                                            \
 halInternalEnableDCDC();                                          \
} while(0)

/**
 * @name halInternalPowerUpBoard
 *
 * Call after coming out of sleep to restore any features which need to
 * power off for sleep
 * 
 */
#ifndef halInternalPowerUpBoard
  #define halInternalPowerUpBoard() do {                             \
  } while(0)
#endif

/**
 * @name halInternalPowerDownBoard
 *
 * Call before going to sleep to power down any features which need to
 * off for sleep
 * 
 */
#ifndef halInternalPowerDownBoard
  #define halInternalPowerDownBoard() do {                           \
  } while(0)
#endif

/**
 *  Instantiate any variables that are used in the board header
 */
#ifndef DEFINE_BOARD_GPIO_CFG_VARIABLES
  #define DEFINE_BOARD_GPIO_CFG_VARIABLES()
#endif
/**
 * Define TEMP_SENSOR to satisfy Ember HAL. Should not be used
 */
#define TEMP_SENSOR_ADC_CHANNEL 0
#define TEMP_SENSOR_SCALE_FACTOR 0

/**************              Radio Module Options                **************/

#ifndef halInternalConfigPa
 #error Must define a PA configuration
#endif

#endif //__BOARD_H__
/** @} END Board Specific Functions */

/** @} END addtogroup */
