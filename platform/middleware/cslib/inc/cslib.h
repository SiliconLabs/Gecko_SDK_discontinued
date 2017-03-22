/******************************************************************************
 * Copyright (c) 2016 by Silicon Laboratories Inc. All rights reserved.
 *
 * http://developer.silabs.com/legal/version/v11/Silicon_Labs_Software_License_Agreement.txt
 *****************************************************************************/

#ifndef _CSLIB_H
#define _CSLIB_H

#define CSLIB_FW_VERSION 0x1300

#include <stdint.h>
/**************************************************************************//**
 *
 * @addtogroup cslib_group Capacitive Sensing Library (CSLIB)
 * @{
 *
 * @brief Capacitive sensing firmware library for Silicon Labs MCUs and SoCs.
 *
 * # Introduction #
 * The capacitive sensing library provides pre-compiled code set that
 * performs touch qualification, filtering, and state maintanence for
 * capacitive sensing-enabled Silicon Laboratories MCUs and SoCs.
 *
 * The library functions using calls into a device layer that interfaces
 * with hardware peripherals.
 *
 *****************************************************************************/

/// Defines the depth of the raw buffer.  Note: this value should not be changed.
#define DEF_SENSOR_BUFFER_SIZE 2

/// Bit that is set when sensor is qualified active
#define DEBOUNCE_ACTIVE_MASK 0x80

/// Bit that is set when sensor is candidate active
#define SINGLE_ACTIVE_MASK   0x40


///Union used to address upper bytes of 32-bit exponential average easily
typedef union SI_UU32
{
  uint32_t u32;                   ///< The 4-byte value as a 32-bit unsigned integer.
  uint32_t s32;                   ///< The 4-byte value as a 32-bit signed integer.
  uint16_t uu16[2];              ///< The 4-byte value as a SI_UU16_t.
  uint16_t u16[2];               ///< The 4-byte value as two unsigned 16-bit integers.
  uint16_t s16[2];               ///< The 4-byte value as two signed 16-bit integers.
  uint8_t u8[4];                 ///< The 4-byte value as 4 unsigned 8-bit integers.
  uint8_t s8[4];                 ///< The 4-byte value as 4 signed 8-bit integers.
} SI_UU32_t;

/// Stores all runtime values for an enabled capacitive sensor.
typedef struct
{
  /// Newest sample taken from CS hardware, without any processing
  uint16_t rawBuffer[DEF_SENSOR_BUFFER_SIZE];
  /// Runtime estimate of untouched or inactive state of CS sensor input
  uint16_t currentBaseline;
  /// Describes the expected difference between untouched and touched CS value
  uint8_t touchDeltaDiv16;
  /// Bit array showing whether touch is qualified on sensor, uses DEBOUNCE_ACTIVE_MASK
  uint8_t activeIndicator;
  /// Stores consecutive CS values above or below active or inactive threshold
  uint8_t debounceCounts;
  /// Tracks trend of CS values to determine whether baseline rises or falls
  signed char baselineAccumulator;
  /// Filtered CS value with 16 bits of LSBs
  SI_UU32_t expValue;
} SensorStruct_t;

/**************************************************************************//**
 * Checks to see if any enabled sensor is single active
 *
 * This function checks the SINGLE_ACTIVE_MASK in the sensor node
 * structure for all enabled sensors.  If any sensor's SINGLE_ACTIVE_MASK
 * is set, meaning that the sensor is a candidate active sensor, the
 *  the function returns TRUE.  Otherwise it returns FALSE.
 *
 * @return 1 if any sensor is active, 0 otherwise
 *****************************************************************************/
uint8_t CSLIB_anySensorSingleActive(void);

/**************************************************************************//**
 * Checks if a sensor is single active
 * @param index into sensor node element to be checked
 *
 * This checks to see if any enabled sensor is a candidate active sensor.
 *
 * @return 1 if element selected by index is single active, 0 otherwise
 *****************************************************************************/
uint8_t CSLIB_isSensorSingleActive(uint8_t index);

/**************************************************************************//**
 * Checks to see if any enabled sensor is debounce active
 *
 * Checks to see if any enabled sensor is qualified active, meaning that
 * the data from the sensor on this input has risen above the active
 * threshold for a consecutive number of samples equaling at least
 * the value defined by DEF_DEBOUNCE_COUNTS.
 *
 * @return 1 if any enabled sensor is debounce active, 0 otherwise
 *****************************************************************************/
uint8_t CSLIB_anySensorDebounceActive(void);

/**************************************************************************//**
 * Checks if a sensor is debounce active
 * @param index into sensor node element to be checked
 *
 * This checks to see if any enabled sensor is a qualified active sensor.
 *
 * @return 1 if element selected by index is debounce active, 0 otherwise
 *****************************************************************************/
uint8_t CSLIB_isSensorDebounceActive(uint8_t index);

/**************************************************************************//**
 * Pushes new sensor sample into sensor node buffer
 * @param index index into sensor node array
 * @param newValue value to be pushed at index
 *
 * This function is used by the device layer of code that interfaces
 * with capacitive sensing hardware.  When @ref CSLIB_update() calls
 * into the device layer to convert a new sample set using @ref scanSensor(),
 * the device layer implementation of @ref scanSensor() should use this
 * routine to push newly converted samples into the sensor node structure
 * for processing within the library.
 *
 *****************************************************************************/
void CSLIB_nodePushRaw(uint8_t index, uint16_t newValue);

/**************************************************************************//**
 * Reads a node structure sensor's raw buffer value
 * @param sensorIndex The element of the sensor node structure
 * @param bufferIndex The element inside the sensor node element's raw data buffer
 *
 * This function can be used to read values in the raw data buffer at bufferIndex
 * of an enabled sensor specified by sensorIndex.
 *
 * @note sensorIndex should not exceed the value defined by @ref CSLIB_numSensors
 * @note bufferIndex should not exceed the value defined by @ref CSLIB_sensorBufferSize
 *
 * @return the value in the raw data buffer
 *****************************************************************************/
uint16_t CSLIB_nodeGetRaw(uint8_t sensorIndex, uint8_t bufferIndex);

/**************************************************************************//**
 * Read touch delta from sensor node struct and expand from 8-bit value
 * @param index Designates the sensor touch delta to be read
 *
 * This function returns the 8-bit compressed touch delta as an uncompressed
 * 16 bit value.
 *
 * @return 16-bit touch delta value of a sensor defined by index
 *****************************************************************************/
uint16_t CSLIB_getUnpackedTouchDelta(uint8_t index);

/**************************************************************************//**
 * Resets an element of the sensor node struct back to defaults
 * @param sensorIndex index of sensor node struct to reset
 * @param fillValue value to be used when filling raw data buffer
 *
 * This function is called during initialization to reset the sensor
 * at node sensorIndex back to back to its power-on state.
 *
 *****************************************************************************/
void CSLIB_resetSensorStruct(uint8_t sensorIndex, uint16_t fillValue);


/**************************************************************************//**
 * Initializes capacitive sensing-related peripherals
 *
 * This function calls into the library, which in turn calls into the device
 * layer functions initializing the sensing and timing hardware used during
 * capacitive sensing conversions.
 *
 *****************************************************************************/
void CSLIB_initHardware(void);

/**************************************************************************//**
 * Initializes capacitive sensing state variables
 *
 * This function resets all state variables in the sensor node struct
 * and library-internal state variables to default.
 *
 *****************************************************************************/
void CSLIB_initLibrary(void);

/**************************************************************************//**
 * Scans capacitive sensing inputs, processes and updates state variables
 *
 * This function should be called in a firmware project's main() loop.  The
 * function will execute a capacitive sensing scan sequence, filter samples,
 * examine samples for qualified touches, and update other state variables.
 *
 *****************************************************************************/
void CSLIB_update(void);

/**************************************************************************//**
 * Performs capacitive sensing conversion on a sensor
 * @param index The sensor to be scanned, corresponds to sensor node struct
 *
 * This device layer function is called from within library code after
 * @ref CSLIB_update() is called in the main loop.  The function is responsible
 * for any configuration of the capacitive sensing block necessary to perform
 * the scan.
 *
 * @return the newly converted capacitive sensing output
 *****************************************************************************/
uint16_t scanSensor(uint8_t index);

/**************************************************************************//**
 * Checks timing and possibly enters low power mode
 *
 * This function checks the time in the system relative to active mode
 * scan period timing.  It should be called in the firmware project's main()
 * while(1) loop.
 *
 * If FREE_RUN_SETTING is set to 0, the function will
 * enter low power until the next active mode scan period.
 *
 * If FREE_RUN_SETTING is set to 1, the function will exit, allowing for
 * another iteration in the main() loop.
 *
 * If @ref CSLIB_lowPowerUpdate() finds that no qualified touches have been
 * found in a time specified by COUNTS_BEFORE_SLEEP multiplied by
 * ACTIVE_MODE_SCAN_PERIOD, the function initializes the system to run
 * in its sleep scanning mode and enters a low power state.
 *
 *****************************************************************************/
void CSLIB_lowPowerUpdate(void);

/**************************************************************************//**
 * Returns raw or filtered sensor data, based on characterized interference
 * @param index Sensor node element whose data is to be read
 *
 * This function either returns the newest raw data for a sensor, or the
 * newest filtered data for a sensor, depending on which data type the
 * interference characterization algorithm determines to be necessary to
 * show current sensor state.  In low interference environments, raw data
 * will be returned.  In higher interference states, filtered data will
 * be returned.
 *
 * @return sensor data from sensor node struct defined index
 *****************************************************************************/
uint16_t CSLIB_getNoiseAdjustedSensorData(uint8_t index);

/// Stores interference characterization level, 0 being lowest and 3 being highest.
extern uint8_t noise_level;

/// Sensor node data structure
extern SensorStruct_t CSLIB_node[];

/// Bit that can be set to disable entrance to sleep, overriding timers.
extern uint8_t disable_sleep_and_stall;

/// Size of the sensor node struct, should only be changed at compile time by editing DEF_NUM_SENSORS
extern uint8_t CSLIB_numSensors;

/// Size of raw data buffers within sensor node struct elements, should not be changed
extern uint8_t CSLIB_sensorBufferSize;

/// @brief Sets the number of consecutive values above/below threshold before button is qualified/disqualified.
/// Defaults to DEF_BUTTON_DEBOUNCE
extern uint8_t CSLIB_buttonDebounce;

/// Sets the scan period in ms for active mode scanning.  Defaults to DEF_ACTIVE_MODE_PERIOD
extern uint16_t CSLIB_activeModePeriod;

/// @brief Describes average interference seen as a percentage of the average touch delta.
/// 100 would mean that average sample to sample interference is equal to the average
/// touch delta configured in the system.
extern uint16_t CSLIB_systemNoiseAverage;

/// Sets the scan period in ms for sleep mode scanning.  Defaults to DEF_ACTIVE_MODE_PERIOD
extern uint16_t CSLIB_sleepModePeriod;

/// Sets the number of consecutive scans without a single qualified touch before entering sleep mode.
extern uint16_t CSLIB_countsBeforeSleep;

/// @brief Configures whether system goes to sleep between scans in active mode.
/// 1 means that the system is in free run mode and will not go to sleep.  0 means that the system
/// will take a single scan within an active mode scan period and will then enter sleep once
/// CSLIB_lowPowerUpdate() is called. The system will stay asleep until the next active mode
/// scan period begins.
extern uint8_t CSLIB_freeRunSetting;

/// @brief Sets whether the system is allowed to ever use sleep mode scanning.
/// If set to 0, the system will always remain in active mode.  If set to 1, the system is allowed
/// to perform sleep mode scanning when conditions permit it.
extern uint8_t CSLIB_sleepModeEnable;

/** @} (end cslib_group) */

#endif


