/** @file hal/plugin/antenna/antenna.h
 * @brief Antenna mode control functions.
 * See @ref micro for documentation.
 *
 * <!-- Copyright 2016 Silicon Laboratories, Inc.                        *80*-->
 */

/** @addtogroup micro
 * See also hal/plugin/antenna/antenna.h for source code.
 *@{
 */

#ifndef __ANTENNA_H__
#define __ANTENNA_H__

// Include AppBuilder generated header
#if   defined(ZA_GENERATED_HEADER)
  #include ZA_GENERATED_HEADER  // AFV2
#endif

// Let AppBuilder contribute coexistence configurations.
#ifdef EMBER_AF_API_ANTENNA
  #define USE_GPIO_PORT_LETTER
  #define ANTENNA_DIVERSITY_DEFAULT_ENABLED
#endif

#ifdef EMBER_AF_PLUGIN_ANTENNA_ANTENNA_DIVERSITY_ENABLED
  #ifdef EMBER_AF_PLUGIN_ANTENNA_ANTENNA_SELECT_GPIO_PORT
  #define ANTENNA_SELECT_GPIO_PORT EMBER_AF_PLUGIN_ANTENNA_ANTENNA_SELECT_GPIO_PORT
  #endif //EMBER_AF_PLUGIN_ANTENNA_ANTENNA_SELECT_GPIO_PORT

  #ifdef EMBER_AF_PLUGIN_ANTENNA_ANTENNA_SELECT_GPIO_PIN
  #define ANTENNA_SELECT_GPIO_PIN EMBER_AF_PLUGIN_ANTENNA_ANTENNA_SELECT_GPIO_PIN
  #endif //EMBER_AF_PLUGIN_ANTENNA_ANTENNA_SELECT_GPIO_PIN
#endif //EMBER_AF_PLUGIN_ANTENNA_ANTENNA_DIVERSITY_ENABLED

#ifdef EMBER_AF_PLUGIN_ANTENNA_ANTENNA_N_DIVERSITY_ENABLED
  #ifdef EMBER_AF_PLUGIN_ANTENNA_ANTENNA_N_SELECT_GPIO_PORT
  #define ANTENNA_nSELECT_GPIO_PORT EMBER_AF_PLUGIN_ANTENNA_ANTENNA_N_SELECT_GPIO_PORT
  #endif //EMBER_AF_PLUGIN_ANTENNA_ANTENNA_N_SELECT_GPIO_PORT

  #ifdef EMBER_AF_PLUGIN_ANTENNA_ANTENNA_N_SELECT_GPIO_PIN
  #define ANTENNA_nSELECT_GPIO_PIN EMBER_AF_PLUGIN_ANTENNA_ANTENNA_N_SELECT_GPIO_PIN
  #endif //EMBER_AF_PLUGIN_ANTENNA_ANTENNA_N_SELECT_GPIO_PIN
#endif //EMBER_AF_PLUGIN_ANTENNA_ANTENNA_N_DIVERSITY_ENABLED

#ifdef DEBUG_nANT_DIV
#define ANTENNA_nSELECT_GPIO  PORTC_PIN(10)
#define DEBUG_ANT_DIV
#endif //DEBUG_nANT_DIV


#ifdef DEBUG_ANT_DIV
#define ANTENNA_DIVERSITY_DEFAULT_ENABLED
#define ANTENNA_SELECT_GPIO  PORTC_PIN(11)
#endif //DEBUG_ANT_DIV

#ifdef ANTENNA_DIVERSITY_DEFAULT_ENABLED
#define HAL_ANTENNA_MODE_DEFAULT HAL_ANTENNA_MODE_DIVERSITY
#else
#define HAL_ANTENNA_MODE_DEFAULT HAL_ANTENNA_MODE_ENABLE1
#endif

/** @name Antenna Selection
 *
 * Define the antenna select pins ANTENNA_SELECT_GPIO and ANTENNA_nSELECT_GPIO
 * ANTENNA_SELECT_GPIO - low,  ANTENNA_nSELECT_GPIO - low: undefined
 * ANTENNA_SELECT_GPIO - low,  ANTENNA_nSELECT_GPIO - high: select antenna 1
 * ANTENNA_SELECT_GPIO - high, ANTENNA_nSELECT_GPIO - low: select antenna 2
 * ANTENNA_SELECT_GPIO - high, ANTENNA_nSELECT_GPIO - high: undefined
 *
 * @note The antenna select mode can be configured at runtime
 * via halSetAntennaMode().
 */
//@{

/**
 * @brief Enable antenna diversity by default.
 */
//#define ANTENNA_DIVERSITY_DEFAULT_ENABLED

#ifndef GPIO_PIN_DEF
// Define GPIO macros based on external port and pin macros
#define GPIO_PIN_DEF(gpio) GPIO_PIN_DEF_(gpio ## _PORT, gpio ## _PIN)
#define GPIO_PIN_DEF_(port,pin) GPIO_PIN_DEF__(port,pin)
#endif //GPIO_PIN_DEF

#ifdef USE_GPIO_PORT_LETTER
//Use port letter definition
#define GPIO_PIN_DEF__(port,pin) (PORT ## port ## _PIN(pin))
#else //!USE_GPIO_PORT_LETTER
//Use numeric port definition
#define GPIO_PIN_DEF__(port,pin) ((port<<4)|pin)
#endif //USE_GPIO_PORT_LETTER

/**
 * @brief GPIO used to control antenna select(low for antenna 1, high for antenna 2).
 */
#if defined(ANTENNA_SELECT_GPIO_PORT) && defined(ANTENNA_SELECT_GPIO_PIN)
#define ANTENNA_SELECT_GPIO GPIO_PIN_DEF(ANTENNA_SELECT_GPIO)
#endif

#if defined(ANTENNA_SELECT_GPIO_PORT) && defined(ANTENNA_SELECT_GPIO_PIN)
#define ANTENNA_SELECT_GPIO_CFG      gpioModePushPull
#define halInternalInitAntennaSelect() do {                          \
  halGpioSetConfig(ANTENNA_SELECT_GPIO, ANTENNA_SELECT_GPIO_CFG);    \
  halSetAntennaMode(HAL_ANTENNA_SELECT_DEFAULT);                     \
  halSetAntennaMode(HAL_ANTENNA_MODE_DEFAULT);                       \
} while(0)
#else//!(defined(ANTENNA_SELECT_GPIO_PORT) && defined(ANTENNA_SELECT_GPIO_PIN))
#define halInternalInitAntennaSelect() do {                          \
} while(0)
#endif//(defined(ANTENNA_SELECT_GPIO_PORT) && defined(ANTENNA_SELECT_GPIO_PIN))

/**
 * @brief GPIO used to control inverted antenna select(high for antenna 1, low for antenna 2).
 */
#if defined(ANTENNA_nSELECT_GPIO_PORT) && defined(ANTENNA_nSELECT_GPIO_PIN)
#define ANTENNA_nSELECT_GPIO GPIO_PIN_DEF(ANTENNA_nSELECT_GPIO)
#endif

#ifdef ANTENNA_nSELECT_GPIO
#define ANTENNA_nSELECT_GPIO_CFG     gpioModePushPull
#define halInternalInitAntennaNSelect() do {                         \
  halGpioSetConfig(ANTENNA_nSELECT_GPIO, ANTENNA_nSELECT_GPIO_CFG);  \
} while(0)
#else//!ANTENNA_nSELECT_GPIO
#define halInternalInitAntennaNSelect() do {                         \
} while(0)
#endif//ANTENNA_nSELECT_GPIO

/**
 * @brief Configure default antenna select at powerup.
 */
#define HAL_ANTENNA_SELECT_DEFAULT   HAL_ANTENNA_MODE_ENABLE1

#define halInternalInitAntennaDiversity() do {                 \
  halInternalInitAntennaNSelect();                             \
  halInternalInitAntennaSelect();                              \
} while(0)
//@} //END OF ANTENNA SELECTION

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#ifndef __EMBERSTATUS_TYPE__
#define __EMBERSTATUS_TYPE__
  //This is necessary here because halSetAntennaMode returns an
  //EmberStatus and not adding this typedef to this file breaks a
  //whole lot of builds due to include mis-ordering
  typedef uint8_t EmberStatus;
#endif //__EMBERSTATUS_TYPE__
#endif // DOXYGEN_SHOULD_SKIP_THIS

typedef enum {
  HAL_ANTENNA_MODE_ENABLE1   = 0,
  HAL_ANTENNA_MODE_ENABLE2   = 1,
  HAL_ANTENNA_MODE_DIVERSITY = 2
} HalAntennaMode;

/** @brief Set antenna mode
 *
 * @param mode HAL_ANTENNA_MODE_ENABLE1 - enable antenna 1
 * HAL_ANTENNA_MODE_ENABLE2 - enable antenna 2
 * HAL_ANTENNA_MODE_DIVERSITY - toggle antenna if tx ack fails
 *
 * @return EMBER_SUCCESS if antenna mode is configured as desired
 * or EMBER_BAD_ARGUMENT if antenna mode is unsupported
 * been configured by the BOARD_HEADER.
 */
EmberStatus halSetAntennaMode(HalAntennaMode mode);


/** @brief Returns the current antenna mode.
 *
 * @return the current antenna mode.
 */
HalAntennaMode halGetAntennaMode(void);

/** @brief Toggle the enabled antenna
 *
 * @return EMBER_SUCCESS if antenna was toggled, EMBER_ERR_FATAL otherwise
 */
EmberStatus halToggleAntenna(void);

#endif //__ANTENNA_H__
/**@} // END micro group
 */

