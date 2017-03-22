//   Copyright 2015 Silicon Laboratories, Inc.                              *80*

// Some implementations of this function may make use of ::EmberEventControl
// events.  If so, the application framework will generally manage the events
// automatically.  Customers who do not use the framework must ensure the events
// are run, by calling either ::emberRunEvents or ::emberRunTask.  See the
// specific implementation for more information.

#ifndef __HUMIDITY_H__
#define __HUMIDITY_H__

/** @brief Start a humidity read operation
 *
 * This function will perform whatever hardware interaction is necessary to
 * start a humidity measurement from the humdity sensor, it will also trigger a
 * event that will go back to take the measurement data through
 * halTemperatureReadingCompleteCallback();
 *
 */
void halHumidityStartRead(void);

/** @brief Initializes the humidity hardware, along with any hardware
 * peripherals necessary to interface with the hardware.  The application
 * framework will generally initialize this plugin automatically.  Customers who
 * do not use the framework must ensure the plugin is initialized by calling
 * this function.
 */
void halHumidityInit(void);

uint16_t halHumidityGetMaxMeasureableHumidityDeciPercent(void);
uint16_t halHumidityGetMinMeasureableHumidityDeciPercent(void);

/** @brief Humidity Reading Complete
 *
 * This callback is called when a humidity reading is complete.
 *
 * @param humidity The humidity in centi Percentage (0.01%)
 * @param readSuccess if the humidity Reading is successful
 */
void halHumidityReadingCompleteCallback(uint16_t humidity, bool readSuccess);
#endif //__HUMIDITY_H__
