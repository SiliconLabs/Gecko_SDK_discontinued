/** @file reference_design/ist_a0085/config/ist_a0085.h
 * EFR32 lighting reference design.
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

// Define board features

// Enable COM port to use retarget serial configuration
#define COM_RETARGET_SERIAL

#define EXTERNAL_FLASH_MOSI_PORT    MX25_PORT_MOSI
#define EXTERNAL_FLASH_MOSI_PIN     MX25_PIN_MOSI
#define EXTERNAL_FLASH_MISO_PORT    MX25_PORT_MISO
#define EXTERNAL_FLASH_MISO_PIN     MX25_PIN_MISO
#define EXTERNAL_FLASH_CLK_PORT     MX25_PORT_SCLK
#define EXTERNAL_FLASH_CLK_PIN      MX25_PIN_SCLK
#define EXTERNAL_FLASH_CS_PORT      MX25_PORT_CS
#define EXTERNAL_FLASH_CS_PIN       MX25_PIN_CS

#define EXTERNAL_FLASH_HOLD_PORT    MX25_PORT_HOLD
#define EXTERNAL_FLASH_HOLD_PIN     MX25_PIN_HOLD
#define EXTERNAL_FLASH_WP_PORT      MX25_PORT_WP
#define EXTERNAL_FLASH_WP_PIN       MX25_PIN_WP

#define EXTERNAL_FLASH_USART0_TXLOC USART_ROUTELOC0_TXLOC_LOC30
#define EXTERNAL_FLASH_USART0_RXLOC USART_ROUTELOC0_RXLOC_LOC11
#define EXTERNAL_FLASH_USART0_CLKLOC USART_ROUTELOC0_CLKLOC_LOC12

#define halInternalConfigPa() do {                                 \
  RADIO_PAInit_t paInit = RADIO_PA_2P4_INIT;                       \
  RADIO_PA_Init(&paInit);                                          \
} while(0)

/**
 * @brief Enable VCOM Pin, passthrough UART via the WSTK
 */
#define halEnableVCOM()  do {                                      \
    GPIO_PinModeSet(gpioPortA, 5, gpioModePushPull, 1);            \
    GPIO_PinOutSet(gpioPortA, 5);                                  \
  } while (0)
  
// Define the fact that we do not use the low frequency crystal
#define EMDRV_RTCDRV_USE_LFRCO

// Define LED Bulb PWM locations
// The lighting board supports 6 channels of PWM:  3 for RGB, 2 for 
// color temperature (one of which is used for the dimmable bulb), and
// one for status.  
// Each channel needs 5 things defined:  
// BULB_PWM_XXX:        Software PWM channel handle.  Must be unique uint8_t.
// BULB_PWM_XXX_PORT:   Port number of the GPIO to drive the PWM channel.
// BULB_PWM_XXX_PIN:    Pin number of the GPIO to drive the PWM channel.
// BULB_PWM_XXX_TIMER:  Timer used for the PWM channel
// BULB_PWM_XXX_CHANNEL:Timer channel for the PWM channel.

/** @name BULB PWM Definitions
 *
 * The following defines are used to specify the channel and pin output of
 * the PWM drivers for the bulb.
 */
//@{
/**
 * @brief Software handle for the white PWM.  Note:  this can be any 8-bit
 * integer so long as it is unique among the PWM handles.  
 */
#define BULB_PWM_WHITE          1
/**
 * @brief GPIO port of the white PWM.
 */
#define BULB_PWM_WHITE_PORT     gpioPortB
/**
 * @brief GPIO pin of the white PWM.
 */
#define BULB_PWM_WHITE_PIN      15
/**
 * @brief Timer used by the white PWM.
 */
#define BULB_PWM_WHITE_TIMER    TIMER0
/**
 * @brief Timer channel of the white PWM.
 */
#define BULB_PWM_WHITE_CHANNEL  0

/**
 * @brief Software handle for the low temperature PWM.  Note:  this can be 
 * any 8-bit integer so long as it is unique among the PWM handles.  
 */
#define BULB_PWM_LOWTEMP          2
/**
 * @brief GPIO port of the low temperature PWM.
 */
#define BULB_PWM_LOWTEMP_PORT     gpioPortB
/**
 * @brief GPIO pin of the low temperature PWM.
 */
#define BULB_PWM_LOWTEMP_PIN      14
/**
 * @brief Timer used by the low temperature PWM.
 */
#define BULB_PWM_LOWTEMP_TIMER    TIMER0
/**
 * @brief Timer channel of teh low temperature PWM.
 */
#define BULB_PWM_LOWTEMP_CHANNEL  1

/**
 * @brief Software handle for the status PWM.  Note:  this can be any 8-bit
 * integer so long as it is unique among the PWM handles.  
 */
#define BULB_PWM_STATUS           3
/**
 * @brief GPIO port of the status PWM.
 */
#define BULB_PWM_STATUS_PORT     gpioPortB
/**
 * @brief GPIO pin of the status PWM.
 */
#define BULB_PWM_STATUS_PIN      11
/**
 * @brief Timer used by the status PWM.
 */
#define BULB_PWM_STATUS_TIMER    TIMER0
/**
 * @brief Timer channel of the status PWM.
 */
#define BULB_PWM_STATUS_CHANNEL  2

/**
 * @brief Software handle for the red PWM.  Note:  this can be any 8-bit
 * integer so long as it is unique among the PWM handles.  
 */
#define BULB_PWM_RED          4
/**
 * @brief GPIO port of the red PWM.
 */
#define BULB_PWM_RED_PORT     gpioPortC
/**
 * @brief GPIO pin of the red PWM.
 */
#define BULB_PWM_RED_PIN      10
/**
 * @brief Timer used by the red PWM.
 */
#define BULB_PWM_RED_TIMER    TIMER1
/**
 * @brief Timer channel of the red PWM.
 */
#define BULB_PWM_RED_CHANNEL  0

/**
 * @brief Software handle for the green PWM.  Note:  this can be any 8-bit
 * integer so long as it is unique among the PWM handles.  
 */
#define BULB_PWM_GREEN          5
/**
 * @brief GPIO port of the green PWM.
 */
#define BULB_PWM_GREEN_PORT     gpioPortD
/**
 * @brief GPIO pin of the green PWM.
 */
#define BULB_PWM_GREEN_PIN      15
/**
 * @brief Timer used by the green PWM.
 */
#define BULB_PWM_GREEN_TIMER    TIMER1
/**
 * @brief Timer channel of the green PWM.
 */
#define BULB_PWM_GREEN_CHANNEL  1

/**
 * @brief Software handle for the blue PWM.  Note:  this can be any 8-bit
 * integer so long as it is unique among the PWM handles.  
 */
#define BULB_PWM_BLUE          6
/**
 * @brief GPIO port of the blue PWM.
 */
#define BULB_PWM_BLUE_PORT     gpioPortC
/**
 * @brief GPIO pin of the blue PWM.
 */
#define BULB_PWM_BLUE_PIN      11
/**
 * @brief Timer used by the blue PWM.
 */
#define BULB_PWM_BLUE_TIMER    TIMER1
/**
 * @brief Timer channel of the blue PWM.
 */
#define BULB_PWM_BLUE_CHANNEL  2

/**
 * @brief Specifies that we are using TIMER 0 for the PWMs.
 */
#define BULB_PWM_USING_TIMER0

/**
 * @brief Direct timer 0 channel 0 to PB 14
 */
#define TIMER0_CHANNEL0_LOC  TIMER_ROUTELOC0_CC0LOC_LOC9   // PB 14
/**
 * @brief Direct timer 0 channel 1 to PD 15
 */
#define TIMER0_CHANNEL1_LOC  TIMER_ROUTELOC0_CC1LOC_LOC9   // PD 15
/**
 * @brief Direct timer 0 channel 2 to PB 11
 */
#define TIMER0_CHANNEL2_LOC  TIMER_ROUTELOC0_CC2LOC_LOC4   // PB 11

/**
 * @brief Specifies that we are using TIMER 1 for the PWMs.
 */
#define BULB_PWM_USING_TIMER1

/**
 * @brief Direct timer 1 channel 0 to PC 10
 */
#define TIMER1_CHANNEL0_LOC  TIMER_ROUTELOC0_CC0LOC_LOC15  // PC 10
/**
 * @brief Direct timer 1 channel 1 to PD 15
 */
#define TIMER1_CHANNEL1_LOC  TIMER_ROUTELOC0_CC1LOC_LOC22  // PD 15
/**
 * @brief Direct timer 1 channel 2 to PC 11
 */
#define TIMER1_CHANNEL2_LOC  TIMER_ROUTELOC0_CC2LOC_LOC14  // PC 11

// default frequency of the PWM
#define PWM_DEFAULT_FREQUENCY 1000

// Include common definitions
#include "board/wstk-common.h"

#endif //__BOARD_H__
/** @} END Board Specific Functions */

/** @} END addtogroup */
