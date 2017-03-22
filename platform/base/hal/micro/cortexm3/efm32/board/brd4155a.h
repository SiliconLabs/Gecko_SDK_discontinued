/** @file hal/micro/cortexm3/efm32/board/brd4155a.h
 * BRD4155A-EFR32MG 2.4 GHz 13dBm / Si4468 868-915 MHz 14 dBm Radio Board
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

// Define board features

// #define DISABLE_PTI

// #define DISABLE_DCDC

// Enable COM port to use retarget serial configuration
#define COM_RETARGET_SERIAL
// Enable HW flow control
#define COM_USART0_HW_FC

// Define onboard external flash locations
#define EXTERNAL_FLASH_MOSI_PORT    MX25_PORT_MOSI
#define EXTERNAL_FLASH_MOSI_PIN     MX25_PIN_MOSI
#define EXTERNAL_FLASH_MISO_PORT    MX25_PORT_MISO
#define EXTERNAL_FLASH_MISO_PIN     MX25_PIN_MISO
#define EXTERNAL_FLASH_CLK_PORT     MX25_PORT_SCLK
#define EXTERNAL_FLASH_CLK_PIN      MX25_PIN_SCLK
#define EXTERNAL_FLASH_CS_PORT      MX25_PORT_CS
#define EXTERNAL_FLASH_CS_PIN       MX25_PIN_CS

// Define nHOST_INT to PF6 -> EXP7
#define NHOST_INT_PORT gpioPortF
#define NHOST_INT_PIN  6
// Define nWAKE to PF7 -> EXP9
#define NWAKE_PORT     gpioPortF
#define NWAKE_PIN      7

/**************    PRO2+ Transceiver Configuration Options    **************/

/*
 * PRO2+ GPIOs:
 * Pin - Signal       Use               PwrUpCfg  PwrDnCfg #define option
 * GP0 * RX_STATE     T/R switch        RX_STATE  (same)
 * GP1 - CTS          CTS option        CTS       (same)
 * GP2 - TX_STATE     T/R switch        TX_STATE  (same)
 * GP3 - PTI          1-wire PTI        PKT_TRACE (same)
 * NIRQ- NIRQ         NIRQ              NIRQ      (same)
 * SDO - SDO          SDO               SDO       (same)
 * GEN_CONFIG         Drive Strength    LOW       (same)
 */

/** @name PRO2+ Interface Definitions
 *
 * The following provide the spi-master and ext-device driver configurations
 * for interfacing the PRO2+ transceiver.
 */

/** @brief PRO2+ SPI-Master configuration
 */
#define PRO2_SPI_PORT USART1
#define PRO2_SPI_MHZ  1  // SPI clock rate in MHz //@TODO: Raise it to 10!

/** @brief PRO2+ Ext-Device configuration
 */
#define ENABLE_EXT_DEVICE       1
#ifndef BRD4155A_REV
  #define BRD4155A_REV 0xA01
#endif//BRD4155A_REV
#if     ((BRD4155A_REV & 0x0FF) == 0) // Tolerate 0 to mean A00, ...
  // SDN configuration
  #define RF_SDN_PORT           gpioPortC // PC11 (BRD4155A A00 compatible)
  #define RF_SDN_PIN            11
  // INT configuration
  #define RF_INT_PORT           gpioPortC // PC10 (BRD4155A A00 compatible)
  #define RF_INT_PIN            10
  // nSEL configuration
  #define RF_USARTRF_CS_PORT    gpioPortC // PC9  (BRD4155A A00 compatible)
  #define RF_USARTRF_CS_PIN     9
#else//!((BRD4155A_REV & 0x0FF) == 0)
  // SDN configuration
  #define RF_SDN_PORT           gpioPortD // PD12 (BRD4155A A01 compatible)
  #define RF_SDN_PIN            12
  // INT configuration
  #define RF_INT_PORT           gpioPortD // PD11 (BRD4155A A01 compatible)
  #define RF_INT_PIN            11
  // nSEL configuration
  #define RF_USARTRF_CS_PORT    gpioPortD // PD10 (BRD4155A A01 compatible)
  #define RF_USARTRF_CS_PIN     10
#endif//((BRD4155A_REV & 0x0FF) == 0)

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

/**
 * @brief Initialize PRO2 configuration variables.
 */
#define DEFINE_SPI_CONFIG_VARIABLES()                                         \
  const USART_TypeDef* pro2SpiPort = PRO2_SPI_PORT;                           \
  const uint8_t pro2SpiClockMHz = PRO2_SPI_MHZ;                               \
  const uint8_t pro2GpioCfgPowerUp[] = { PRO2_POWERUP_GPIO_CFG() };           \
  int8_t phy2PtiGpio = PRO2_PTI_GPIO;

#ifndef PRO2_SHUTDOWN_WHEN_ASLEEP
#define PRO2_SHUTDOWN_WHEN_ASLEEP 1 // 0=SLEEP PRO2+, 1=SDN PRO2+
#endif//PRO2_SHUTDOWN_WHEN_ASLEEP

#if     PRO2_SHUTDOWN_WHEN_ASLEEP
  // Override the below definitions to shut down the PRO2+ when sleeping
  #define halInternalPowerDownBoard() do {                           \
    extern void emRadioPowerDown(void);                              \
    emRadioPowerDown();                                              \
  } while(0)

  #define halInternalPowerUpBoard() do {                             \
    extern void emRadioPowerUp(void);                                \
    emRadioPowerUp();                                                \
  } while(0)
#endif//PRO2_SHUTDOWN_WHEN_ASLEEP

#if defined(PHY_PRO2PLUS) || defined(PHY_PRO2)
  // Don't configure EFR32 PTI; that radio is effectively disabled
  #define halInternalConfigPti() /* no-op */
#endif//defined(PHY_PRO2PLUS) || defined(PHY_PRO2)

#ifdef  EXTERNAL_FLASH_CS_PORT
  #define halInternalInitDataFlash() do {                            \
    /* Deselect DataFlash, in case there's no pullup on the board */ \
    GPIO_PinModeSet(EXTERNAL_FLASH_CS_PORT, EXTERNAL_FLASH_CS_PIN,   \
                    gpioModePushPull, 1);                            \
  } while(0)
#endif//EXTERNAL_FLASH_CS_PORT

// Include common definitions
#include "board/wstk-common.h"

#endif //__BOARD_H__
/** @} END Board Specific Functions */

/** @} END addtogroup */
