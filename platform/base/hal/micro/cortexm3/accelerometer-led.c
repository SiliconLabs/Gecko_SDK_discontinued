// Copyright 2014 Silicon Laboratories, Inc.
//-----------------------------------------------------------------------------
// Implements a Bosch BMA222E Accelerometer to EM3xx Interface and sets a status
// LED to reflect accelerometer activity
//
// The accelerometer is connected to an EM3xx via
//   1. SC1 port for I2C control
//   2. PB5 GPIO for wake-up from EM3xx sleep
//
// The accelerometer is programmed to:
//   1. Drive INT1 into EM3xx pin when acceleration slope exceeds 156mg/50ms
//   2. Operate in low-power mode 1 (LPM1) to minimize power consumption
//
// The plugin also sets up an interrupt on the GPIO pin connected to the
// INT1 signal from the accelerometer. When the accelerometer detects activity
// the pin goes high and the GPIO ISR in the EM3xx sets a LED to indicate
// activity. A timout event is used to turn off the LED again when a
// defined period of time has passed without accelerometer activity (interrupt
// pin goes low).
//
// An accelerometer x, y, z readout function is supplied for testing
//
//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include PLATFORM_HEADER
#include "stack/include/ember-types.h"
#include "stack/include/event.h"
#include "hal/hal.h"
#include "hal/micro/accelerometer-led.h"
#include "hal/plugin/i2c-driver/i2c-driver.h"

//-----------------------------------------------------------------------------
// Local Macros
//-----------------------------------------------------------------------------
#define ACCELEROMETER_ADDR         0x32
#define NORMAL_MODE_DELAY_MS          2
#define ACCELEROMETER_RESET_DELAY_MS 10
#define XYZ_BUFFER_SIZE               6
#define X_BUFFER_INDEX                1
#define Y_BUFFER_INDEX                3
#define Z_BUFFER_INDEX                5

//-----------------------------------------------------------------------------
// Local Variables
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Local Constants
//-----------------------------------------------------------------------------
static const uint8_t accelerometerBgwPmuLpwNorm[] = {0x11, 0x18};
static const uint8_t accelerometerBgwPmuLpwLpm1[] = {0x11, 0x58};
static const uint8_t accelerometerBgwSoftReset[]  = {0x14, 0xB6};
static const uint8_t accelerometerIntEn0[]        = {0x16, 0x07};
static const uint8_t accelerometerIntMap0[]       = {0x19, 0x04};
static const uint8_t accelerometerInt6[]          = {0x28, 0x04};
static const uint8_t accelerometerFifoData[]      = {0x3F};

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------
EmberEventControl halAccelerometerLedTimeoutEventControl;

//-----------------------------------------------------------------------------
// Local Helper Functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Global API Functions
//-----------------------------------------------------------------------------
void halAccelerometerLedReadXyz(int8_t *x, int8_t *y, int8_t *z)
{
  uint8_t buffer[XYZ_BUFFER_SIZE];

  // Goto normal mode
  halI2cWriteBytesDelay(ACCELEROMETER_ADDR,
                        accelerometerBgwPmuLpwNorm,
                        sizeof(accelerometerBgwPmuLpwNorm),
                        NORMAL_MODE_DELAY_MS);

  // Set address to FIFO burst read
  halI2cWriteBytes(ACCELEROMETER_ADDR,
                   accelerometerFifoData,
                   sizeof(accelerometerFifoData));

  // Read six synchronized bytes for X, Y, and Z from FIFO
  halI2cReadBytes(ACCELEROMETER_ADDR,buffer,XYZ_BUFFER_SIZE);

  // Copy accelerometer xyz data to return variables
  *x = (int8_t)buffer[X_BUFFER_INDEX];
  *y = (int8_t)buffer[Y_BUFFER_INDEX];
  *z = (int8_t)buffer[Z_BUFFER_INDEX];

  // Return to LPM1 mode with 50ms sampling
  halI2cWriteBytes(ACCELEROMETER_ADDR,
                   accelerometerBgwPmuLpwLpm1,
                   sizeof(accelerometerBgwPmuLpwLpm1));
}

//-----------------------------------------------------------------------------
void halAccelerometerLedInitialize(void)
{
  // Setup interrupts for the GPIO pin connected to the accelerometer
  // Reset interrupt config to a known state first
  ACC_INT_INTCFG = 0;                  // Disable ACC_INT triggering
  INT_CFGCLR     = ACC_INT_INT_EN_BIT; // Clear ACC_INT top level int enable
  INT_GPIOFLAG   = ACC_INT_FLAG_BIT;   // Clear stale ACC_INT interrupt
  INT_MISS       = ACC_INT_MISS_BIT;   // Clear stale missed ACC_INT interrupt
  // Configure ACC_INT
  ACC_INT_SEL();                             // Point IRQ at the desired pin
  ACC_INT_INTCFG  = (0 << GPIO_INTFILT_BIT); // No filter
  ACC_INT_INTCFG |= (3 << GPIO_INTMOD_BIT);  // 3 = both edges

  INT_CFGSET = ACC_INT_INT_EN_BIT; // Set top level interrupt enable

  // Make sure I2C port is initialized
  halI2cInitialize();

  // Reset accelerometer
  halI2cWriteBytesDelay(ACCELEROMETER_ADDR,
                        accelerometerBgwSoftReset,
                        sizeof(accelerometerBgwSoftReset),
                        ACCELEROMETER_RESET_DELAY_MS);

  // Setup INT1 as unlatched interrupt on slope (delta acceleration) > 156mg
  // Set threshold to 156mg (10*15.6mg using default +/-2g range)
  halI2cWriteBytes(ACCELEROMETER_ADDR,
                   accelerometerInt6,
                   sizeof(accelerometerInt6));
  // Enable slope x, y, and z engines
  halI2cWriteBytes(ACCELEROMETER_ADDR,
                   accelerometerIntEn0,
                   sizeof(accelerometerIntEn0));
  // Enable INT1 from slope
  halI2cWriteBytes(ACCELEROMETER_ADDR,
                   accelerometerIntMap0,
                   sizeof(accelerometerIntMap0));
  // Goto LPM1 with 50ms sampling
  halI2cWriteBytes(ACCELEROMETER_ADDR,
                   accelerometerBgwPmuLpwLpm1,
                   sizeof(accelerometerBgwPmuLpwLpm1));
}

//-----------------------------------------------------------------------------
// Global Event Handlers
//-----------------------------------------------------------------------------
// After timeout period has passed since last accelerometer activity we
// turn off the LED
void halAccelerometerLedTimeoutEventHandler(void)
{
  if (ACC_INT_IN & ACC_INT_PINMASK) {
    // Accelerometer pin is still set, wait another period
    emberEventControlSetDelayMS(halAccelerometerLedTimeoutEventControl,
                                (ACC_INACTIVITY_TIMOUT_SEC
                                * MILLISECOND_TICKS_PER_SECOND));
  } else {
    // No motion after timeout, clear LED
    halClearLed(ACCELEROMETER_LED);
    emberEventControlSetInactive(halAccelerometerLedTimeoutEventControl);
  }
}

//-----------------------------------------------------------------------------
// Global Interrupt Handlers
//-----------------------------------------------------------------------------
// Turn on LED if accelerometer reports activity
// When activity is no longer detected we set a timout before
// before the LED is turned off again.
void ACC_INT_ISR(void)
{
  // Clear int before read to avoid potential of missing interrupt
  INT_MISS     = ACC_INT_MISS_BIT; // Clear missed interrupt flag
  INT_GPIOFLAG = ACC_INT_FLAG_BIT; // Clear top level interrupt flag

  if (ACC_INT_IN & BIT(ACC_INT_PIN&7)) { // High pin - Accelerometer activity
    halSetLed(ACCELEROMETER_LED);
  } else {                               // Low pin - Accelerometer inactivity
    emberEventControlSetDelayMS(halAccelerometerLedTimeoutEventControl,
                                (ACC_INACTIVITY_TIMOUT_SEC
                                * MILLISECOND_TICKS_PER_SECOND));
  }
}
