//   Copyright 2015 Silicon Laboratories, Inc.                              *80*

// Some implementations of this function may make use of ::EmberEventControl
// events.  If so, the application framework will generally manage the events
// automatically.  Customers who do not use the framework must ensure the events
// are run, by calling either ::emberRunEvents or ::emberRunTask.  See the
// specific implementation for more information.

#ifndef __ILLUMINANCE_H__
#define __ILLUMINANCE_H__

/** @brief Initiate an illuminance read
 *
 * This function will initiate a read of the illuminance sensor.  This read will
 * then trigger a driver specific callback when the read has completed.
 *
 * @param multiplier the muliplier needed to calculate normalized reading for lower
 * layer plugin, input 0 for using the default value.
 *
 * @return 0 if no errors were encountered on read, a driver specific error
 * code otherwise.
 */
uint16_t halIlluminanceStartRead(uint8_t multiplier);

/** @brief Initializes the illuminance hardware, along with any hardware
 * peripherals necessary to interface with the hardware.  The application
 * framework will generally initialize this plugin automatically.  Customers who
 * do not use the framework must ensure the plugin is initialized by calling
 * this function.
 */
void halIlluminanceInit(void);

/** @brief Calibrate the illuminance sensor
 *
 * This function will take in an external expected illuminance value.  It will
 * compare that to the value currently observed by the sensor and calculate the
 * gain necessary to calibrate the sensor to generate a reading to match the
 * provided external reading.
 *
 * @param extLux The externally measured illuminance value, in lux.
 * @return multiplier gain parameter
 */
uint8_t halIlluminanceCalibrate(uint32_t extLux);

uint16_t halIlluminanceGetMaxMeasureableIlluminanceLogLux(void);
uint16_t halIlluminanceGetMinMeasureableIlluminanceLogLux(void);

/** @brief Illuminance Reading Complete
 *
 * This callback is called when a illuminance reading is complete.
 *
 * @param logLux The illuminance in Lux, in log scale.  This value is calculated
 * using the following formula: loxLux = 10,000 * log10(Illuminance in Lux + 1)
 */
void halIlluminanceReadingCompleteCallback(uint16_t logLux);
#endif //__ILLUMINANCE_H__
