/** @file hal/micro/cortexm3/efm32/board/ezr32.h
 * See @ref board for detailed documentation.
 *
 * <!-- Copyright 2013 Silicon Laboratories, Inc.                        *80*-->
 */

/** @addtogroup board
 *  @brief Functions and definitions specific to the breakout board.
 *
 *@{
 */

#ifndef __BOARD_H__
#define __BOARD_H__

#include "bspconfig.h"
#include "em_device.h"

/** @brief PRO2+ SPI-Master configuration
 */
#if defined(CORTEXM3_EZR32_MICRO)
 #if defined(USARTRF0)
  #define PRO2_SPI_PORT USARTRF0
 #elif defined(USARTRF1)
  #define PRO2_SPI_PORT USARTRF1
 #else
  #error "No valid RF USART found"
 #endif
#elif defined(EFM32LG990F256)
 #define PRO2_SPI_PORT USART0
#endif

#if defined(PHY_EZR2)
 //TODO: for some reason the r55 in EZR32 won't operate properly at frequencies
 // higher than 1MHz.
 #define PRO2_SPI_MHZ    1      // EZR2 SPI clock rate in MHz
#elif defined(PHY_PRO2) || defined(PHY_PRO2PLUS)
 #define PRO2_SPI_MHZ    10     // PRO2 and PRO2+ SPI clock rate in MHz
#else
 #error Not supported PHY definition!
#endif

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

#include "hal/micro/cortexm3/efm32/board/ezr32-radio-boards.h"

//TODO: Fix once EBID/token available
#if    ( (RADIO_BOARD == BRD4503A) || (RADIO_BOARD == BRD4503B) || (RADIO_BOARD == BRD4503C) || \
         (RADIO_BOARD == BRD4504A) || (RADIO_BOARD == BRD4504B) || \
         (RADIO_BOARD == BRD4505A) || (RADIO_BOARD == BRD4505B) || \
         (RADIO_BOARD == BRD4543A) || (RADIO_BOARD == BRD4543B) || \
         (RADIO_BOARD == BRD4545A) )
// These EZR32 boards have an RF switch on GPIO2/3
 #define GPIO2_SETTING CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_TX_STATE
 #define GPIO3_SETTING CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_RX_STATE
#elif  ( (RADIO_BOARD == BRD4502A) || (RADIO_BOARD == BRD4502B) || \
         (RADIO_BOARD == BRD4502C) || (RADIO_BOARD == BRD4502D) || (RADIO_BOARD == BRD4502E) || \
         (RADIO_BOARD == BRD4506A) || \
         (RADIO_BOARD == BRD4542A) || (RADIO_BOARD == BRD4542B) || (RADIO_BOARD == BRD4542C) || \
         (RADIO_BOARD == BRD4544A) || \
         (RADIO_BOARD == BRD4546A) )
// These EZR32 boards have no RF switch on GPIO2/3
 #define GPIO2_SETTING CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_DONOTHING
 #define GPIO3_SETTING CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_DONOTHING
#elif (defined EMBER_STACK_WASP)  // Let it be configured to do nothing, 
                                  // as WASP stack will define its own 
                                  // GPIO configuration
 #define GPIO2_SETTING CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_DONOTHING
 #define GPIO3_SETTING CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_DONOTHING
#else// Something else - use default
  #error Please define the PRO2 radio board in RADIO_BOARD!
#endif

//TODO: Want to use PRO2_GPIO_3 from phy.h here but too painful to #include it
//TODO: So 0=none, 1=GPIO_0, 2=GPIO_1, 3=GPIO_2, 4=GPIO_3
#define PRO2_PTI_GPIO (1+0) // GPIO0 for PTI data
#define PRO2_POWERUP_GPIO_CFG()                                               \
                   8, /*Number of bytes in following PRO2+ command*/          \
                   CMD_GPIO_PIN_CFG,                                          \
        /*GPIO0*/  CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_DONOTHING,        \
        /*GPIO1*/  CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_DONOTHING,        \
        /*GPIO2*/  GPIO2_SETTING,                                             \
        /*GPIO3*/  GPIO3_SETTING,                                             \
        /*NIRQ */  CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_DONOTHING,        \
        /*SDO  */  CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_DONOTHING,        \
        /*GEN  */ (CMD_GPIO_PIN_CFG_ARG_GEN_CONFIG_DRV_STRENGTH_ENUM_HIGH     \
                   << CMD_GPIO_PIN_CFG_ARG_GEN_CONFIG_DRV_STRENGTH_LSB),      \
                   0, /*Number of bytes in following PRO2+ command*/          \
        /*(end of list)*/

// There's no need for a PRO2_POWERDOWN_GPIO_CFG because SDN will kill the PRO2+
//@} //END OF PRO2+ GPIO CONFIGURATION


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
  BOARDLED0 = 0,
  BOARDLED1 = 1,
  BOARDLED2 = BOARDLED0,
  BOARDLED3 = BOARDLED1,
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
#define BUTTON0_PORT    (BSP_GPIO_PB0_PORT)

/**
 * @brief The actual GPIO BUTTON1 is connected to.  This define should
 * be used whenever referencing BUTTON1, such as controlling if pieces
 * are compiled in.
 * Remember that other pieces that might want to use IRQC.
 */
#define BUTTON1         (BSP_GPIO_PB1_PIN)
/**
 * @brief The GPIO input register for BUTTON1.
 */
#define BUTTON1_PORT    (BSP_GPIO_PB1_PORT)
//@} //END OF BUTTON DEFINITIONS


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


/** @brief Radio HoldOff not supported
 */
#define halInternalInitRadioHoldOff() /* no-op */


#define halInternalPowerUpBoard()                             \
  do {                                                        \
    /* MISO pull down */                                      \
    GPIO_PinOutClear((GPIO_Port_TypeDef)RF_USARTRF_MISO_PORT, RF_USARTRF_MISO_PIN);                         \
  } while (0)

#define halInternalPowerDownBoard()                           \
  do {                                                        \
    /* MISO pull up */                                        \
    GPIO_PinOutSet((GPIO_Port_TypeDef)RF_USARTRF_MISO_PORT, RF_USARTRF_MISO_PIN);                           \
  } while (0)

/**
 * @brief Initialize GPIO powerup configuration variables.
 */
#define DEFINE_SPI_CONFIG_VARIABLES()                                       \
  const USART_TypeDef* pro2SpiPort = PRO2_SPI_PORT;                         \
  const uint8_t pro2SpiClockMHz = PRO2_SPI_MHZ;                               \
  const uint8_t pro2GpioCfgPowerUp[] = { PRO2_POWERUP_GPIO_CFG() };           \
  int8_t phy2PtiGpio = PRO2_PTI_GPIO;

/**
 * GPIO definitions for WSTK BC serial
 */
// #define BSP_BCC_CLK  => defined in bspconfig.h

/**
 * Initialize GPIOs direction and default state for VCP.
 */
#if defined(BSP_BCC_USART)
#define halInternalInitVCPPins()  do {                                      \
  /* Configure GPIO pin for UART TX */                                      \
  /* To avoid false start, configure output as high. */                     \
  GPIO_PinModeSet( BSP_BCC_TXPORT, BSP_BCC_TXPIN, gpioModePushPull, 1u );   \
  /* Configure GPIO pin for UART RX */                                      \
  GPIO_PinModeSet( BSP_BCC_RXPORT, BSP_BCC_RXPIN, gpioModeInput, 1u );      \
  /* Enable the switch that enables UART communication. */                  \
  GPIO_PinModeSet( BSP_BCC_ENABLE_PORT, BSP_BCC_ENABLE_PIN, gpioModePushPull, 1u ); \
  BSP_BCC_USART->ROUTE |= USART_ROUTE_RXPEN | USART_ROUTE_TXPEN | BSP_BCC_LOCATION; \
  } while (0)
#elif defined(BSP_BCC_LEUART)
#define halInternalInitVCPPins()  do {                                      \
  /* Configure GPIO pin for UART TX */                                      \
  /* To avoid false start, configure output as high. */                     \
  GPIO_PinModeSet( BSP_BCC_TXPORT, BSP_BCC_TXPIN, gpioModePushPull, 1u );   \
  /* Configure GPIO pin for UART RX */                                      \
  GPIO_PinModeSet( BSP_BCC_RXPORT, BSP_BCC_RXPIN, gpioModeInput, 1u );      \
  /* Enable the switch that enables UART communication. */                  \
  GPIO_PinModeSet( BSP_BCC_ENABLE_PORT, BSP_BCC_ENABLE_PIN, gpioModePushPull, 1u );    \
  BSP_BCC_LEUART->ROUTE |= LEUART_ROUTE_RXPEN | LEUART_ROUTE_TXPEN | BSP_BCC_LOCATION; \
  } while (0)
#endif
// Enable COM port to use retarget serial configuration
#define COM_RETARGET_SERIAL

#endif //__BOARD_H__

/** @} END Board Specific Functions */

/** @} END addtogroup */
