/** @file /hal/plugin/adc/adc.h
 * @brief Header for A/D converter.
 *
 * See @ref adc for documentation.
 *
 * <!-- Copyright 2008 by Ember Corporation.  All rights reserved.       *80*-->
 */

/** @addtogroup adc
 * Sample A/D converter driver.
 *
 * See adc.h for source code.
 *
 * @note EM35x ADC driver support is preliminary and may
 * change in a future release.
 *
 * @note The EmberZNet stack does use these functions.
 *
 * To use the ADC system, include this file and ensure that
 * ::halInternalInitAdc() is called whenever the microcontroller is
 * started.  Call ::halInternalSleepAdc() to sleep the module and
 * ::halInternalInitAdc() to wake up the module.
 *
 * A "user" is a separate thread of execution and usage.  That is,
 * internal Ember code is one user and clients are a different user.
 * But a client that is calling the ADC in two different functions
 * constitutes only one user, as long as the ADC access is not
 * interleaved.
 *
 * @note This code does not allow access to the continuous reading mode of
 * the ADC, which some clients may require.
 *
 * Many functions in this file return an ::EmberStatus value.  See
 * error-def.h for definitions of all ::EmberStatus return values.
 *
 *@{
 */

#ifndef __ADC_H__
#define __ADC_H__

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/** @brief ADC functions employ a user ID to keep different
 * users separate.
 *
 * Avoid many users because each user requires some
 * amount of state storage.
 *
 * @sa ::NUM_ADC_USERS
 */
enum ADCUser
#else
// A type for the ADC User enumeration.
typedef uint8_t ADCUser;
enum
#endif //DOXYGEN_SHOULD_SKIP_THIS
{
  /** LQI User ID. */
  ADC_USER_LQI = 0,
  /** Application User ID */
  ADC_USER_APP = 1,
  /** Application User ID */
  ADC_USER_APP2 = 2
};

/** @brief Be sure to update ::NUM_ADC_USERS if additional users are added
 * to the ::ADCUser list.
 */
#define NUM_ADC_USERS 3 // make sure to update if the above is adjusted

/** @brief A type for the channel enumeration
 */
typedef uint8_t ADCChannelType;

/** @brief A type for the reference voltage enumeration
 */
typedef uint8_t ADCReferenceType;

/** @brief A type for the sample rate enumeration
 */
typedef uint8_t ADCRateType;


#if defined(EMBER_TEST)
  #include "hal/micro/unix/simulation/adc.h"
#elif defined(CORTEXM3_EFM32_MICRO)
  // disabling until valid. search for CORTEXM3_EFM32_MICRO for more instances
  // #include "cortexm3/efm32/adc.h"
#elif defined(CORTEXM3)
  #include "hal/plugin/adc/adc-cortexm3.h"
#else
  // platform that doesn't have ADC support
#endif


/** @brief Initializes and powers-up the ADC.  Should also be
 * called to wake from sleep.  The ADC is required for EM250 stack operation
 * so this function must be called from halInit.
 */
void halInternalInitAdc(void);


/** @brief Shuts down the voltage reference and ADC system to
 * minimize power consumption in sleep.
 */
void halInternalSleepAdc(void);


/** @brief Starts an ADC conversion for the user specified by \c id.
 *
 * @appusage The application must set \c reference to the voltage
 * reference desired (see the ADC references enum, fixed at
 * ::ADC_REF_INT for the em250), set \c channel to the channel number
 * required (see the ADC channel enum), and set \c rate to reflect the
 * number of bits of accuracy desired (see the ADC rates enum, fixed
 * at ::ADC_CONVERSION_TIME_US_256 for the Atmega).
 *
 * @param id        An ADC user.
 *
 * @param reference Voltage reference to use, chosen from enum
 * ::ADCReferenceType (fixed at ::ADC_REF_INT for the EM250).
 *
 * @param channel   Microprocessor channel number.  For EM250 channels, see the
 * EM250 ADC channels enum.  For basic, single-ended Atmel channels, see the
 * Atmega single-ended ADC channels enum.  For more complex measurements on
 * Atmels (differential and amped channel numbers), see the Atmel datasheet
 * for your micro.
 *
 * @param rate      EM250 rate number (see the ADC EM250 rate enum).
 *
 * @return One of the following:
 * - EMBER_ADC_CONVERSION_DEFERRED   if the conversion is still waiting
 * to start.
 * - EMBER_ADC_CONVERSION_BUSY       if the conversion is currently taking
 * place.
 * - EMBER_ERR_FATAL                 if a passed parameter is invalid.
 */
EmberStatus halStartAdcConversion(ADCUser id,
                                  ADCReferenceType reference,
                                  ADCChannelType channel,
                                  ADCRateType rate);

/** @brief Returns the status of a pending conversion
 * previously started by ::halStartAdcConversion().  If the conversion
 * is complete, writes the raw register value of the conversion (the unaltered
 * value taken directly from the ADC's data register) into \c value.
 *
 * @param id     An ADC user.
 *
 * @param value  Pointer to an uint16_t to be loaded with the new value. Take
 * note that the Atmel's ADC only generates 8-bit values which are loaded into
 * the lower 8 bits of \c value.
 *
 * @return One of the following:
 * - ::EMBER_ADC_CONVERSION_DONE       if the conversion is complete.
 * - ::EMBER_ADC_CONVERSION_DEFERRED   if the conversion is still waiting
 * to start.
 * - ::EMBER_ADC_CONVERSION_BUSY       if the conversion is currently taking
 * place.
 * - ::EMBER_ADC_NO_CONVERSION_PENDING if \c id does not have a pending
 * conversion.
 */
EmberStatus halRequestAdcData(ADCUser id, uint16_t *value);


/** @brief Waits for the user's request to complete and then,
 * if a conversion was done, writes the raw register value of the conversion
 * (the unaltered value taken directly from the ADC's data register) into
 * \c value and returns ::EMBER_ADC_CONVERSION_DONE, or immediately
 * returns ::EMBER_ADC_NO_CONVERSION_PENDING.
 *
 * @param id     An ADC user.
 *
 * @param value  Pointer to an uint16_t to be loaded with the new value. Take
 * note that the Atmel's ADC only generates 8-bit values which are loaded into
 * the lower 8 bits of \c value.
 *
 * @return One of the following:
 * - ::EMBER_ADC_CONVERSION_DONE        if the conversion is complete.
 * - ::EMBER_ADC_NO_CONVERSION_PENDING  if \c id does not have a pending
 * conversion.
 */
EmberStatus halReadAdcBlocking(ADCUser id, uint16_t *value);


/** @brief Calibrates or recalibrates the ADC system.
 *
 * @appusage Use this function to (re)calibrate as needed. This function is
 * intended for the EM250 microcontroller, which requires proper calibration to calculate
 * a human readible value (a value in volts).  If the app does not call this
 * function, the first time (and only the first time) the function
 * ::halConvertValueToVolts() is called, this function is invoked.  To
 * maintain accurate volt calculations, the application should call this
 * whenever it expects the temperature of the micro to change.
 *
 * @param id  An ADC user.
 *
 * @return One of the following:
 * - ::EMBER_ADC_CONVERSION_DONE        if the calibration is complete.
 * - ::EMBER_ERR_FATAL                  if the calibration failed.
 */
EmberStatus halAdcCalibrate(ADCUser id);


/** @brief Convert the raw register value (the unaltered value taken
 * directly from the ADC's data register) into a signed fixed point value with
 * units 10^-4 Volts.  The returned value will be in the range -12000 to
 * +12000 (-1.2000 volts to +1.2000 volts).
 *
 * @appusage Use this function to get a human useful value.
 *
 * @param value  An uint16_t to be converted.
 *
 * @return Volts as signed fixed point with units 10^-4 Volts.
 */
int32_t halConvertValueToVolts(uint16_t value);


/** @brief Calibrates Vref to be 1.2V +/-10mV.
 *
 *  @appusage This function must be called from halInternalInitAdc() before
 *  making ADC readings.  This function is not intended to be called from any
 *  function other than halInternalInitAdc().  This function ensures that the
 *  master cell voltage and current bias values are calibrated before
 *  calibrating Vref.
 */
void emberCalibrateVref(void);

/** @brief Set ADC clock mode.
 *
 * @param slow  A bool to select slow or normal clock.
 */
void halAdcSetClock(bool slow);

/** @brief Get ADC clock mode.
 *
 * @return A true if the slow clock is selected.
 */
bool halAdcGetClock(void);


#endif // __ADC_H__

/** @} // END addtogroup
 */
