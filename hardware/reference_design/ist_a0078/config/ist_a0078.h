/** @file reference_design/ist_a0078/config/ist_a0078.h
 * EFR32 Occupancy Sensor Reference Design
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

// This macro can be uncommented to disable the watchdog for debug purposes.
// This can be especially useful when debugging the occupancy-pyd driver
//#define DISABLE_WATCHDOG

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
 * ::BOARD_ACTIVITY_LED and ::BOARD_HEARTBEAT_LED provide a further layer of
 * abstraction on top of the 3 LEDs for verbose coding.
 */
typedef enum HalBoardLedPins {
  BOARDLED0 = 0,
  BOARD_ACTIVITY_LED  = BOARDLED0
} HalBoardLedPins_t;

/**
 * @brief Inform the led-blink plugin that this board only has one LED
 */
#define MAX_LED_NUMBER        1

/**
 * @brief Inform the framework that the BOARD_ACTIVITY_LED should not be used
 * to show network traffic in this design
 */
#define NO_LED                1

/** @} END OF LED DEFINITIONS  */

/** @name Battery Monitor Definitions
 *
 * The following are used to aid in the abstraction with the battery
 * monitoring plugin.  These values should be used to set up a GPIO pin (that
 * should be left as a no connect from the MCU) to go hi when the radio is
 * transmitting data.
 */
/**
 * @brief The pin to be used to signal the radio is busy, and thus the battery
 * voltage at its most accurate state
 */
#define HAL_BATTERY_MONITOR_TX_ACTIVE_PIN          15
/**
 * @brief The port to be used to signal the radio is busy, and thus the battery
 * voltage at its most accurate state
 */#define HAL_BATTERY_MONITOR_TX_ACTIVE_PORT      gpioPortD
/**
  * @brief The PRS channel to be used to control the TX active functionality
 */
#define HAL_BATTERY_MONITOR_PRS_CHANNEL	           3
/**
 * @brief The pin to be used as the PRS output
 */
#define HAL_BATTERY_MONITOR_PRS_PIN_LOCATION       14

 /** @} END OF BATTERY MONITOR DEFINITIONS  */

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

#define BUTTON0_EMWU  GPIO_EXTILEVEL_EM4WU8

/**
 * @brief The actual GPIO BUTTON1 is connected to.  This define should
 * be used whenever referencing BUTTON1, such as controlling if pieces
 * are compiled in.
 */
#define BUTTON1         (BSP_GPIO_PB1_PIN)
/**
 * @brief The GPIO input register for BUTTON1.
 */
#define BUTTON1_PORT        (BSP_GPIO_PB1_PORT)

#define BUTTON1_EMWU  GPIO_EXTILEVEL_EM4WU12
/**
 * @brief The tamper switch is very bouncy, so software debounce will be used
 * to provide stable values
 */
#define DEBOUNCE 500
//@} //END OF BUTTON DEFINITIONS

/** @name Occupancy Sensor Definitions
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
 * @brief These define the port and pin that the Serial In pin of the occupancy
 * sensor
 */
#define OCCUPANCY_PYD1698_SERIN_PORT  gpioPortF
#define OCCUPANCY_PYD1698_SERIN_PIN   4
  /**
 * @brief These define the port and pin that the DLink pin of the occupancy
 * sensor
 */
#define OCCUPANCY_PYD1698_DLINK_PORT  gpioPortF
#define OCCUPANCY_PYD1698_DLINK_PIN   7
  /**
 * @brief This describes the EM4WU pin number that is associated with the DLINK
 * pin.  This is necessary to get the DLINK pin to act as a level triggered (as
 * opposed to edge triggered) interrupt.
 */
#define OCCUPANCY_PYD1698_DLINK_EM4_PIN        _GPIO_EXTILEVEL_EM4WU1_SHIFT
  /**
 * @brief These define the port and pin that will be polled to determine if the
 * occupancy sensor is currently in calibration mode
 */
#define OCCUPANCY_PYD1698_INSTALLATION_JP_PORT gpioPortB
#define OCCUPANCY_PYD1698_INSTALLATION_JP_PIN  11
//@} //END OF OCCUPANCY SENSOR DEFINITIONS

// Enable COM port to use retarget serial configuration
#define COM_RETARGET_SERIAL

// Define onboard external flash locations
#define EXTERNAL_FLASH_MOSI_PORT    MX25_PORT_MOSI
#define EXTERNAL_FLASH_MOSI_PIN     MX25_PIN_MOSI
#define EXTERNAL_FLASH_MISO_PORT    MX25_PORT_MISO
#define EXTERNAL_FLASH_MISO_PIN     MX25_PIN_MISO
#define EXTERNAL_FLASH_CLK_PORT     MX25_PORT_SCLK
#define EXTERNAL_FLASH_CLK_PIN      MX25_PIN_SCLK
#define EXTERNAL_FLASH_CS_PORT      MX25_PORT_CS
#define EXTERNAL_FLASH_CS_PIN       MX25_PIN_CS

// Define nHOST_INT to PD10
#define NHOST_INT_PORT gpioPortD
#define NHOST_INT_PIN  10
// Define nWAKE to PD11
#define NWAKE_PORT     gpioPortD
#define NWAKE_PIN      11

#define halInternalConfigPa() do {                                 \
  RADIO_PAInit_t paInit = RADIO_PA_2P4_INIT;                       \
  RADIO_PA_Init(&paInit);                                          \
} while(0)

/**
 * @name halInternalPowerUpBoard
 *
 * Call after coming out of sleep to restore any features which need to
 * power off for sleep
 *
 */
#define halInternalPowerUpBoard() do {                             \
  } while(0)

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

/**
 * @name halInternalInitBoard
 *
 * Called on startup from halInit(). Should contain calls to all board
 * specific startup sequences.
 *
 */
#define halInternalInitBoard() do {                                \
 halInternalInitButton();                                          \
 halInternalPowerUpBoard();                                        \
 halInternalConfigPti();                                           \
 halInternalConfigPa();                                            \
 halInternalEnableDCDC();                                          \
} while(0)

#endif //__BOARD_H__
/** @} END Board Specific Functions */

/** @} END addtogroup */
