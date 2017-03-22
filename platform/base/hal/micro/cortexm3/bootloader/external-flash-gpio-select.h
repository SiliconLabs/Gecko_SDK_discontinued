/** @file hal/micro/cortexm3/bootloader/external-flash-gpio-select.h
 *  @brief  Serial controller abstraction for EM3XX interface to external flash
 * 
 * This layer of abstraction allows the board header to specify which serial
 * controller to use when talking to the Flash IC. If EXTERNAL_FLASH_PORT is left
 * undefined, the old behavior is used (SC2 unless SPI_FLASH_SC1 is defined).
 *
 * When EEPROM_USES_SHUTDOWN_CONTROL is defined in the board header, logic is 
 * enabled in the EEPROM driver which drives a line high upon initialization.
 * In Ember reference designs, an enable pin must be driven high in order to use
 * the EEPROM. This option is intended to be enabled when running app-bootloader
 * on designs based on current Ember reference designs. PB7 is chosen as this
 * pin unless overridden.
 * 
 * <!-- Copyright 2014 Silicon Labs, Inc.                                *80*-->
 */

// Default to old behavior
#ifndef EXTERNAL_FLASH_SERIAL_CONTROLLER
  #ifdef SPI_FLASH_SC1
    #define EXTERNAL_FLASH_SERIAL_CONTROLLER  1
  #else
    #define EXTERNAL_FLASH_SERIAL_CONTROLLER  2
  #endif
#endif

// only serial controllers 1 through 4 are valid
#if !(    EXTERNAL_FLASH_SERIAL_CONTROLLER >= 1 \
       && EXTERNAL_FLASH_SERIAL_CONTROLLER <= 4 )
  #error "EXTERNAL_FLASH_SERIAL_CONTROLLER out of range (1-4)."
#endif

// only serial controllers actually present on the micro are valid
#if (EMBER_MICRO_SERIAL_CONTROLLERS_MASK & (1 << (EXTERNAL_FLASH_SERIAL_CONTROLLER - 1))) == 0

  #error EXTERNAL_FLASH_SERIAL_CONTROLLER specifies an invalid serial \
         controller for this micro.

#endif

// Abstract away registers to make access serial controller agnostic
#if EXTERNAL_FLASH_SERIAL_CONTROLLER == 1

  // GPIO assignments (SC1)
  // PB1: SC1MOSI / SC1SDA
  // PB2: SC1MISO / SC1SCL
  // PB3: SC1SCLK
  // PB4: SC1 chip select

  #if    !(EMBER_MICRO_PORT_B_GPIO & PB1) \
      || !(EMBER_MICRO_PORT_B_GPIO & PB2) \
      || !(EMBER_MICRO_PORT_B_GPIO & PB3) \
      || !(EMBER_MICRO_PORT_B_GPIO & PB4)

    #error EXTERNAL_FLASH_SERIAL_CONTROLLER requires invalid GPIO for this micro.

  #endif

  #define EXTERNAL_FLASH_SCx_DATA        SC1_DATA
  #define EXTERNAL_FLASH_SCx_MODE        SC1_MODE
  #define EXTERNAL_FLASH_SCx_RATELIN     SC1_RATELIN
  #define EXTERNAL_FLASH_SCx_RATEEXP     SC1_RATEEXP

  #define EXTERNAL_FLASH_MOSI_PIN        PORTB_PIN(1) // PB1
  #define EXTERNAL_FLASH_MISO_PIN        PORTB_PIN(2) // PB2
  #define EXTERNAL_FLASH_SCLK_PIN        PORTB_PIN(3) // PB3

  #if !defined(EXTERNAL_FLASH_CS_ACTIVE)
  // default to PB4 if it is not already defined
    #define EXTERNAL_FLASH_CS_ACTIVE()     do { GPIO_PBCLR = BIT(4); } while (0)
    #define EXTERNAL_FLASH_CS_INACTIVE()   do { GPIO_PBSET = BIT(4); } while (0)
    #define EXTERNAL_FLASH_nCS_PIN         PORTB_PIN(4) // PB4
  #endif

  #define EXTERNAL_FLASH_SCx_MODE_SPI    SC1_MODE_SPI
  #define EXTERNAL_FLASH_SCx_SPISTAT     SC1_SPISTAT
  #define EXTERNAL_FLASH_SCx_SPICFG      SC1_SPICFG

  #define EXTERNAL_FLASH_SDA_PIN         PORTB_PIN(1) // PB1
  #define EXTERNAL_FLASH_SCL_PIN         PORTB_PIN(2) // PB2

  #define EXTERNAL_FLASH_SCx_MODE_I2C    SC1_MODE_I2C
  #define EXTERNAL_FLASH_SCx_TWISTAT     SC1_TWISTAT
  #define EXTERNAL_FLASH_SCx_TWICTRL1    SC1_TWICTRL1
  #define EXTERNAL_FLASH_SCx_TWICTRL2    SC1_TWICTRL2

#elif EXTERNAL_FLASH_SERIAL_CONTROLLER == 2

  // GPIO assignments (SC2)
  // PA0: SC2MOSI
  // PA1: SC2MISO / SC2SDA
  // PA2: SC2SCLK / SC2SCL
  // PA3: SC2 chip select

  #if    !(EMBER_MICRO_PORT_A_GPIO & PA0) \
      || !(EMBER_MICRO_PORT_A_GPIO & PA1) \
      || !(EMBER_MICRO_PORT_A_GPIO & PA2) \
      || !(EMBER_MICRO_PORT_A_GPIO & PA3)

    #error EXTERNAL_FLASH_SERIAL_CONTROLLER requires invalid GPIO for this micro.

  #endif

  #define EXTERNAL_FLASH_SCx_DATA        SC2_DATA
  #define EXTERNAL_FLASH_SCx_MODE        SC2_MODE
  #define EXTERNAL_FLASH_SCx_RATELIN     SC2_RATELIN
  #define EXTERNAL_FLASH_SCx_RATEEXP     SC2_RATEEXP

  #define EXTERNAL_FLASH_MOSI_PIN        PORTA_PIN(0) // PA0
  #define EXTERNAL_FLASH_MISO_PIN        PORTA_PIN(1) // PA1
  #define EXTERNAL_FLASH_SCLK_PIN        PORTA_PIN(2) // PA2

  #if !defined(EXTERNAL_FLASH_CS_ACTIVE)
  // default to PA3 if it is not already defined
    #define EXTERNAL_FLASH_CS_ACTIVE()     do { GPIO_PACLR = BIT(3); } while (0)
    #define EXTERNAL_FLASH_CS_INACTIVE()   do { GPIO_PASET = BIT(3); } while (0)
    #define EXTERNAL_FLASH_nCS_PIN         PORTA_PIN(3) // PA3
  #endif
  
  #define EXTERNAL_FLASH_SCx_MODE_SPI    SC2_MODE_SPI
  #define EXTERNAL_FLASH_SCx_SPICFG      SC2_SPICFG
  #define EXTERNAL_FLASH_SCx_SPISTAT     SC2_SPISTAT

  #define EXTERNAL_FLASH_SDA_PIN         PORTA_PIN(1) // PA1
  #define EXTERNAL_FLASH_SCL_PIN         PORTA_PIN(2) // PA2

  #define EXTERNAL_FLASH_SCx_MODE_I2C    SC2_MODE_I2C
  #define EXTERNAL_FLASH_SCx_TWISTAT     SC2_TWISTAT
  #define EXTERNAL_FLASH_SCx_TWICTRL1    SC2_TWICTRL1
  #define EXTERNAL_FLASH_SCx_TWICTRL2    SC2_TWICTRL2

#elif EXTERNAL_FLASH_SERIAL_CONTROLLER == 3

  // GPIO assignments (SC3)
  // PD1: SC3MOSI / SC3SDA
  // PD2: SC3MISO / SC3SCL
  // PD3: SC3SCLK
  // PD4: SC3 chip select unless overwritten elsewhere

  #if    !(EMBER_MICRO_PORT_D_GPIO & PD1) \
      || !(EMBER_MICRO_PORT_D_GPIO & PD2) \
      || !(EMBER_MICRO_PORT_D_GPIO & PD3) \
      || !(EMBER_MICRO_PORT_D_GPIO & PD4)

    #error EXTERNAL_FLASH_SERIAL_CONTROLLER requires invalid GPIO for this micro.

  #endif

  #define EXTERNAL_FLASH_SCx_DATA        SC3_DATA
  #define EXTERNAL_FLASH_SCx_MODE        SC3_MODE
  #define EXTERNAL_FLASH_SCx_RATELIN     SC3_RATELIN
  #define EXTERNAL_FLASH_SCx_RATEEXP     SC3_RATEEXP

  #define EXTERNAL_FLASH_MOSI_PIN        PORTD_PIN(1) // PD1
  #define EXTERNAL_FLASH_MISO_PIN        PORTD_PIN(2) // PD2
  #define EXTERNAL_FLASH_SCLK_PIN        PORTD_PIN(3) // PD3

  #if !defined(EXTERNAL_FLASH_CS_ACTIVE)
  // default to PD4 if it is not already defined
    #define EXTERNAL_FLASH_CS_ACTIVE()   do { GPIO_PDCLR = BIT(4); } while (0)
    #define EXTERNAL_FLASH_CS_INACTIVE() do { GPIO_PDSET = BIT(4); } while (0)
    #define EXTERNAL_FLASH_nCS_PIN         PORTD_PIN(4) // PD4
  #endif

  #define EXTERNAL_FLASH_SCx_MODE_SPI    SC3_MODE_SPI
  #define EXTERNAL_FLASH_SCx_SPISTAT     SC3_SPISTAT
  #define EXTERNAL_FLASH_SCx_SPICFG      SC3_SPICFG

  #define EXTERNAL_FLASH_SDA_PIN         PORTD_PIN(1) // PD1
  #define EXTERNAL_FLASH_SCL_PIN         PORTD_PIN(2) // PD2

  #define EXTERNAL_FLASH_SCx_MODE_I2C    SC3_MODE_I2C
  #define EXTERNAL_FLASH_SCx_TWISTAT     SC3_TWISTAT
  #define EXTERNAL_FLASH_SCx_TWICTRL1    SC3_TWICTRL1
  #define EXTERNAL_FLASH_SCx_TWICTRL2    SC3_TWICTRL2

#elif EXTERNAL_FLASH_SERIAL_CONTROLLER == 4

  // GPIO assignments (SC4)
  // PE0: SC4MOSI
  // PE1: SC4MISO / SC4SDA
  // PE2: SC4SCLK / SC4SCL
  // PE3: SC4 chip select

  #if    !(EMBER_MICRO_PORT_E_GPIO & PE0) \
      || !(EMBER_MICRO_PORT_E_GPIO & PE1) \
      || !(EMBER_MICRO_PORT_E_GPIO & PE2) \
      || !(EMBER_MICRO_PORT_E_GPIO & PE3)

    #error EXTERNAL_FLASH_SERIAL_CONTROLLER requires invalid GPIO for this micro.

  #endif

  #define EXTERNAL_FLASH_SCx_DATA        SC4_DATA
  #define EXTERNAL_FLASH_SCx_MODE        SC4_MODE
  #define EXTERNAL_FLASH_SCx_RATELIN     SC4_RATELIN
  #define EXTERNAL_FLASH_SCx_RATEEXP     SC4_RATEEXP

  #define EXTERNAL_FLASH_MOSI_PIN        PORTE_PIN(0) // PE0
  #define EXTERNAL_FLASH_MISO_PIN        PORTE_PIN(1) // PE1
  #define EXTERNAL_FLASH_SCLK_PIN        PORTE_PIN(2) // PE2

  #if !defined(EXTERNAL_FLASH_CS_ACTIVE)
  // default to PE3 if it is not already defined
    #define EXTERNAL_FLASH_CS_ACTIVE()     do { GPIO_PECLR = BIT(3); } while (0)
    #define EXTERNAL_FLASH_CS_INACTIVE()   do { GPIO_PESET = BIT(3); } while (0)
    #define EXTERNAL_FLASH_nCS_PIN         PORTE_PIN(3) // PE3
  #endif

  #define EXTERNAL_FLASH_SCx_MODE_SPI    SC4_MODE_SPI
  #define EXTERNAL_FLASH_SCx_SPISTAT     SC4_SPISTAT
  #define EXTERNAL_FLASH_SCx_SPICFG      SC4_SPICFG

  #define EXTERNAL_FLASH_SDA_PIN         PORTE_PIN(1) // PE1
  #define EXTERNAL_FLASH_SCL_PIN         PORTE_PIN(2) // PE2

  #define EXTERNAL_FLASH_SCx_MODE_I2C    SC4_MODE_I2C
  #define EXTERNAL_FLASH_SCx_TWISTAT     SC4_TWISTAT
  #define EXTERNAL_FLASH_SCx_TWICTRL1    SC4_TWICTRL1
  #define EXTERNAL_FLASH_SCx_TWICTRL2    SC4_TWICTRL2

#else

  #error EXTERNAL_FLASH_SERIAL_CONTROLLER must be 1, 2, 3, or 4

#endif



#if defined(EEPROM_USES_SHUTDOWN_CONTROL)

  #if !defined(EEPROM_POWER_PORT_PIN)
    // Some micros don't have PB7, issue error when alternate wasn't specified
    #if !(EMBER_MICRO_PORT_B_GPIO & PB7)

      #error Default EEPROM enable pin (PB7) is invalid for this micro. \
             Please define CONFIGURE_EEPROM_POWER_PIN, SET_EEPROM_POWER_PIN, \
             and CLR_EEPROM_POWER_PIN as funciton-like macros.
    #endif

    #define EEPROM_POWER_PORT_PIN PORTB_PIN(7)

  #endif

  #define CONFIGURE_EEPROM_POWER_PIN()  do { halGpioSetConfig(EEPROM_POWER_PORT_PIN, GPIOCFG_OUT); } while (0)
  #define SET_EEPROM_POWER_PIN()        do { halGpioSet(EEPROM_POWER_PORT_PIN); } while (0)
  #define CLR_EEPROM_POWER_PIN()        do { halGpioClear(EEPROM_POWER_PORT_PIN); } while (0)

#else

  // Define empty macros when shutdown control is not used
  #define CONFIGURE_EEPROM_POWER_PIN()    (void)0
  #define SET_EEPROM_POWER_PIN()          (void)0
  #define CLR_EEPROM_POWER_PIN()          (void)0

#endif
