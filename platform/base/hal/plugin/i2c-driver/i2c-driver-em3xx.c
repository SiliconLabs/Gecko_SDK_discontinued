// Copyright 2014 Silicon Laboratories, Inc.
//------------------------------------------------------------------------------
// Implements an I2C Driver plugin that can be used by multiple plugins
// requiring common I2C support.
//
// NOTE: Functions are NOT RE-ENTRANT and CANNOT be called from ISR level.  All
//       functions MUST be call at main() (aka. event) level only!!!
//
//------------------------------------------------------------------------------
#include PLATFORM_HEADER
#include "stack/include/ember-types.h"
#include "hal/hal.h"
#include "hal/plugin/i2c-driver/i2c-driver.h"

//------------------------------------------------------------------------------
// Allow serial controller and GPIO port/pin to be defined elsewhere (ideally in
// board.h), but provide a default mapping to SC1, PB1/2 if no board.h macros
// are present.
#ifndef I2C_SC_PORT
#define I2C_SC_PORT   1
#endif
#ifndef I2C_SDA_PORT
#define I2C_SDA_PORT  B
#endif
#ifndef I2C_SDA_PIN
#define I2C_SDA_PIN   1
#endif
#ifndef I2C_SCL_PORT
#define I2C_SCL_PORT  B
#endif
#ifndef I2C_SCL_PIN
#define I2C_SCL_PIN   2
#endif

//-----------------------------------------------------------------------------
// Some preprocessor magic necessary to get macros to properly resolve
#define PASTE(a,b,c)       a##b##c
#define EVAL3(a,b,c)       PASTE(a,b,c)

//-----------------------------------------------------------------------------
// Simplify the code by supporting SPI only on SCx
#define SCx_REG(port, reg)            (EVAL3(SC,port,_##reg))
#define INT_SCx(port)                 (EVAL3(INT_SC,port,))
#define INT_SCxCFG(port)              (EVAL3(INT_SC,port,CFG))
#define INT_SCxFLAG(port)             (EVAL3(INT_SC,port,FLAG))

//------------------------------------------------------------------------------
//If we're on pin 0-3, we need to map to the port's GPIO_PxCFGL register
//Each pin is 4 bits of config in the reg, so we shift 0xf << 4*pin to get mask
//For pin 4-7, the CFG register is GPIO_PxCFGH, and the pin number needs to be
//subtracted by four to get the offset (4 is at lsb, 7 is at msb)
#if (I2C_SCL_PIN==0) || (I2C_SCL_PIN==1)|| (I2C_SCL_PIN==2) || (I2C_SCL_PIN==3)
#define I2C_SCL_GPIO_CFG        EVAL3(GPIO_P,I2C_SCL_PORT,CFGL)
#define I2C_SCL_GPIO_CFG_SHIFT  (I2C_SCL_PIN*4)
#define I2C_SCL_GPIO_CFG_MASK   (0xF << I2C_SCL_GPIO_CFG_SHIFT)
#else
#define I2C_SCL_GPIO_CFG        EVAL3(GPIO_P,I2C_SCL_PORT,CFGH)
#define I2C_SCL_GPIO_CFG_SHIFT  ((I2C_SCL_PIN-4)*4)
#define I2C_SCL_GPIO_CFG_MASK   (0xF << I2C_SCL_GPIO_CFG_SHIFT)
#endif

#if (I2C_SDA_PIN==0) || (I2C_SDA_PIN==1)|| (I2C_SDA_PIN==2) || (I2C_SDA_PIN==3)
#define I2C_SDA_GPIO_CFG        EVAL3(GPIO_P,I2C_SDA_PORT,CFGL)
#define I2C_SDA_GPIO_CFG_SHIFT  (I2C_SDA_PIN*4)
#define I2C_SDA_GPIO_CFG_MASK   (0xF << I2C_SDA_GPIO_CFG_SHIFT)
#else
#define I2C_SDA_GPIO_CFG        EVAL3(GPIO_P,I2C_SDA_PORT,CFGH)
#define I2C_SDA_GPIO_CFG_SHIFT  ((I2C_SDA_PIN-4)*4)
#define I2C_SDA_GPIO_CFG_MASK   (0xF << I2C_SDA_GPIO_CFG_SHIFT)
#endif

// Allow I2C timeout to be set by plugin config settings via ember desktop
#define TRANSACTION_TIMEOUT \
                        (EMBER_AF_PLUGIN_I2C_DRIVER_TRANSACTION_TIMEOUT*1000/16)

//-----------------------------------------------------------------------------
// Bitfield defines for various I2C register settings
#define SC_RATELIN_100K             14
#define SC_RATELIN_375K             15
#define SC_RATELIN_400K             14

#define SC_RATEEXP_100K             3
#define SC_RATEEXP_375K             1
#define SC_RATEEXP_400K             1

#define SC_MODE_DISABLED            0
#define SC_MODE_TWI                 3

#define SC_TWISTAT_TWICMDFIN        0x08
#define SC_TWISTAT_TWIRXFIN         0x04
#define SC_TWISTAT_TWITXFIN         0x02
#define SC_TWISTAT_TWIRXNAK         0x01

#define SC_TWICTRL1_TWISTOP          0x08
#define SC_TWICTRL1_TWISTART         0x04
#define SC_TWICTRL1_TWISEND          0x02
#define SC_TWICTRL1_TWIRECV          0x01

#define SC_TWICTRL2_SC_TWIACK_MASK   0x01
#define SC_TWICTRL2_SC_TWIACK_ACK    0x01
#define SC_TWICTRL2_SC_TWIACK_NAK    0x00

//-----------------------------------------------------------------------------
// Static flag to address potential multiple I2C initialization calls
// from plugin initcallback and each plugin using this I2C driver
static bool i2cInitialized = false;

// Static flag to preserve what GPIO pin cfg to return the SCL pins to after
// transaction has finished
static uint16_t sclGpioCfg = 0;
static uint16_t sdaGpioCfg = 0;
static uint16_t scMode     = 0;
static uint16_t scRatelin  = 0;
static uint16_t scRateexp  = 0;

void halI2cInitialize(void)
{
// Initialize SCx port to I2C with 100kHz SCL
// Can be called from other plugins or this plugin's initcallback
  if (i2cInitialized) {             // Already initialized?
    return;
  }

  SCx_REG(I2C_SC_PORT, MODE)    = SC_MODE_DISABLED; // Disable the port
  SCx_REG(I2C_SC_PORT, RATELIN) = SC_RATELIN_100K;  // Configure for 100kHz SCL
  SCx_REG(I2C_SC_PORT, RATEEXP) = SC_RATEEXP_100K;
  SCx_REG(I2C_SC_PORT, MODE)    = SC_MODE_TWI;      // Enable I2C

  i2cInitialized = true;            // Record initialized
}

// This initialization callback is for compatibility with AFV2.
void emberAfPluginI2cDriverInitCallback(void)
{
  halI2cInitialize();
}

static void i2cSaveAndConfigSC(void)
{
  // This function will save the GPIO and serial control peripherals' config
  // state and reconfigure them to be used for an I2C transaction based on the
  // macros defined in board.h.  This function is meant to be called before a bus
  // transaction occurs, so that the plugin can know what state to return the
  // peripherals to once the transaction is complete.

  // Read GPIO config register for SDA and SCL, and the SCx settings and save
  // them to global state variables
  sclGpioCfg = I2C_SCL_GPIO_CFG;
  sdaGpioCfg = I2C_SDA_GPIO_CFG;
  scMode     = SCx_REG(I2C_SC_PORT, MODE);
  scRatelin  = SCx_REG(I2C_SC_PORT, RATELIN);
  scRateexp  = SCx_REG(I2C_SC_PORT, RATEEXP);

  // Set the serial controller rate registers, then enable the SC in I2C mode
  SCx_REG(I2C_SC_PORT, MODE)    = SC_MODE_DISABLED; // Disable the port
  SCx_REG(I2C_SC_PORT, RATELIN) = SC_RATELIN_100K;  // Configure for 100kHz SCL
  SCx_REG(I2C_SC_PORT, RATEEXP) = SC_RATEEXP_100K;
  SCx_REG(I2C_SC_PORT, MODE)    = SC_MODE_TWI;

  // Set the config bits for SCL and SDA to be OUT_ALT_OD
  I2C_SCL_GPIO_CFG = (I2C_SCL_GPIO_CFG & ~I2C_SCL_GPIO_CFG_MASK)
                     | (GPIOCFG_OUT_ALT_OD << I2C_SCL_GPIO_CFG_SHIFT);

  I2C_SDA_GPIO_CFG = (I2C_SDA_GPIO_CFG & ~I2C_SDA_GPIO_CFG_MASK)
                     | (GPIOCFG_OUT_ALT_OD << I2C_SDA_GPIO_CFG_SHIFT);
}

static void i2cRestoreConfigSC(void)
{
  // This function will save the GPIO and serial control peripherals' config
  // state and reconfigure them to be used for an I2C transaction based on the
  // macros defined in board.h.  This function is meant to be called before a bus
  // transaction occurs, so that the plugin can know what state to return the
  // peripherals to once the transaction is complete.
  // Store current GPIO config for port with I2C and configure SCA and SCL lines
  // as Open drain alternate outputs

    // Restore the SDA and SCL GPIO config values
  I2C_SCL_GPIO_CFG = (I2C_SCL_GPIO_CFG & ~I2C_SCL_GPIO_CFG_MASK)
                     | (sclGpioCfg & I2C_SCL_GPIO_CFG_MASK);

  I2C_SDA_GPIO_CFG = (I2C_SDA_GPIO_CFG & ~I2C_SDA_GPIO_CFG_MASK)
                     | (sdaGpioCfg & I2C_SDA_GPIO_CFG_MASK);

  // Disable the serial controller before making any config changes
  SCx_REG(I2C_SC_PORT, MODE) = SC_MODE_DISABLED;

  // Restore the serial controller mode and rate registers
  SCx_REG(I2C_SC_PORT, RATELIN) = scRatelin;
  SCx_REG(I2C_SC_PORT, RATEEXP) = scRateexp;
  SCx_REG(I2C_SC_PORT, MODE)    = scMode;
}

uint8_t halI2cWriteBytes(uint8_t address,
                       const uint8_t *buffer,
                       uint8_t count)
{
// Execute I2C START
// Send 8-bit device address with RW bit set to write (0)
// Write count bytes from buffer to device
// Execute I2C STOP
// return 0 if I2C write was successful
  uint8_t i;
  uint32_t start;

  // Step 0: Save current configuration of the SC and GPIO peripherals, then
  // set them up for I2C operation
  i2cSaveAndConfigSC();

  // Step 1: Send the start command, then wait for the SC_TWISTAT_TWICMDFIN
  // bit to clear, signifying the START has completed
  SCx_REG(I2C_SC_PORT, TWICTRL1) = SC_TWICTRL1_TWISTART;
  start = halStackGetInt32uSymbolTick();  // get starting system time (16us LSB)
  while (!(SCx_REG(I2C_SC_PORT, TWISTAT) & SC_TWISTAT_TWICMDFIN)) {
    //
    // Timeout, in case something goes horribly wrong
    //
    if (halStackGetInt32uSymbolTick() > start + TRANSACTION_TIMEOUT) {
      i2cRestoreConfigSC();
      return I2C_DRIVER_ERR_TIMEOUT;         // Return a timeout error occurred
    }
  }

  // Step two: Send the address and direction, then wait for SC_TWISTAT_TWITXFIN
  // to clear, signifying the data has been transmitted
  // Note: Bits 7..1 are the address, bit 0 is cleared to signify a write
  SCx_REG(I2C_SC_PORT, DATA) = address & 0xFE;
  SCx_REG(I2C_SC_PORT, TWICTRL1) = SC_TWICTRL1_TWISEND;
  start = halStackGetInt32uSymbolTick(); // Get starting system time (16us LSB)
  while (!(SCx_REG(I2C_SC_PORT, TWISTAT) & SC_TWISTAT_TWITXFIN)) {
    if (halStackGetInt32uSymbolTick() > start + TRANSACTION_TIMEOUT) {
      //
      // Timeout, in case something goes horribly wrong
      //
      i2cRestoreConfigSC();
      return I2C_DRIVER_ERR_TIMEOUT;         // Return a timeout error occurred
    }
  }

  // Make sure the slave ack'd, signalled by SC_TWISTAT_TWIRXNAK being low
  if (SCx_REG(I2C_SC_PORT, TWISTAT) & SC_TWISTAT_TWIRXNAK) {
    i2cRestoreConfigSC();
    return I2C_DRIVER_ERR_ADDR_NAK;
  }

  // Step three: Send each data byte for byte until we have no more to send.
  // On each transaction, SC_TWISTAT_TWITXFIN will clear to signify data has
  // been sent.
  for (i = 0; i < count; i++) {
    SCx_REG(I2C_SC_PORT, DATA) = buffer[i];
    SCx_REG(I2C_SC_PORT, TWICTRL1) = SC_TWICTRL1_TWISEND;
    start = halStackGetInt32uSymbolTick(); // Get starting system time
    while (!(SCx_REG(I2C_SC_PORT, TWISTAT) & SC_TWISTAT_TWITXFIN)) {
      // Timeout, in case something goes horribly wrong
      if (halStackGetInt32uSymbolTick() > start + TRANSACTION_TIMEOUT) {
        i2cRestoreConfigSC();
        return I2C_DRIVER_ERR_TIMEOUT;
      }
    }

    // Make sure the slave ack'd, signalled by SC_TWISTAT_TWIRXNAK being low
    if (SCx_REG(I2C_SC_PORT, TWISTAT) & SC_TWISTAT_TWIRXNAK) {
      i2cRestoreConfigSC();
      return I2C_DRIVER_ERR_DATA_NAK;
    }
  }

  // Step four: Transaction complete, send the STOP signal and wait until the
  // SC_TWISTAT_TWICMDFIN bit to clear, indicating transaction complete
  SCx_REG(I2C_SC_PORT, TWICTRL1) = SC_TWICTRL1_TWISTOP;
  start = halStackGetInt32uSymbolTick();
  while (!(SCx_REG(I2C_SC_PORT, TWISTAT) & SC_TWISTAT_TWICMDFIN)) {
    // Timeout, in case something goes horribly wrong
    if (halStackGetInt32uSymbolTick() > start + TRANSACTION_TIMEOUT) {
      i2cRestoreConfigSC();
      return I2C_DRIVER_ERR_TIMEOUT;
    }
  }

  i2cRestoreConfigSC();
  return I2C_DRIVER_ERR_NONE;
}

uint8_t halI2cWriteBytesDelay(uint8_t address,
                         const uint8_t *buffer,
                         uint8_t count,
                         uint8_t delay)
{
// Perform I2C write
// Wait delay ms before return
// return 0 if I2C write was successful

  uint8_t retCode;

  retCode = halI2cWriteBytes(address,buffer,count); // Send I2C write

  if (delay) {             // Only delay if delay!=0
    halCommonDelayMicroseconds((uint16_t) delay*1000); // Opt delay ms after write
  }

  return retCode;
}

uint8_t halI2cReadBytes(uint8_t address, uint8_t *buffer, uint8_t count)
{
  // Execute I2C START
  // Send 8-bit device address with RW bit set to read (1)
  // Read count bytes from device to buffer (NACK last read, ACK all others)
  // Execute I2C STOP
  // return 0 if I2C read was successful

  uint8_t i;
  uint32_t start;

  // Step 0: Save current configuration of the SC and GPIO peripherals, then
  // set them up for I2C operation
  i2cSaveAndConfigSC();

  // Step 1: Send the start command, then wait for the SC_TWISTAT_TWICMDFIN
  // bit
  SCx_REG(I2C_SC_PORT, TWICTRL1) = SC_TWICTRL1_TWISTART;
  start = halStackGetInt32uSymbolTick(); // get starting system time (16us LSB)
  while (!(SCx_REG(I2C_SC_PORT, TWISTAT) & SC_TWISTAT_TWICMDFIN)) {
    //
    // Timeout, in case something goes horribly wrong
    //
    if (halStackGetInt32uSymbolTick() > start + TRANSACTION_TIMEOUT) {
      i2cRestoreConfigSC();
      return I2C_DRIVER_ERR_TIMEOUT;
    }
  }

  // Step two: Send the address and direction, then wait for SC_TWISTAT_TWITXFIN
  // to clear, signifying the data has been transmitted
  // Note: Bits 7..1 are the address, bit 0 is set to signify a read
  SCx_REG(I2C_SC_PORT, DATA) = address | 0x01;
  SCx_REG(I2C_SC_PORT, TWICTRL1) = SC_TWICTRL1_TWISEND;
  start = halStackGetInt32uSymbolTick(); // Get starting system time (16us LSB)
  while (!(SCx_REG(I2C_SC_PORT, TWISTAT) & SC_TWISTAT_TWITXFIN)) {
    // Timeout, in case something goes horribly wrong
    if (halStackGetInt32uSymbolTick() > start + TRANSACTION_TIMEOUT) {
      i2cRestoreConfigSC();
      return I2C_DRIVER_ERR_TIMEOUT;
    }
  }

  // Make sure the slave ack'd, signalled by SC_TWISTAT_TWIRXNAK being low
  if ((SCx_REG(I2C_SC_PORT, TWISTAT) & SC_TWISTAT_TWIRXNAK)) {
    i2cRestoreConfigSC();
    return I2C_DRIVER_ERR_ADDR_NAK;
  }

  // Step three: read and ACK each byte, then store it in the rx array.  NAK the
  // last byte to signal the slave to stop sending.  SC_TWISTAT_TWIRXFIN will
  // clear each time the receive transaction is complete.
  SCx_REG(I2C_SC_PORT, TWICTRL2) = SC_TWICTRL2_SC_TWIACK_ACK;
  for (i = 0; i < count; i++) {
    if (i == count-1) {
      SCx_REG(I2C_SC_PORT, TWICTRL2) = SC_TWICTRL2_SC_TWIACK_NAK;
    }
    SCx_REG(I2C_SC_PORT, TWICTRL1) = SC_TWICTRL1_TWIRECV;
    start = halStackGetInt32uSymbolTick(); // Get start system time (16us LSB)
    while (!(SCx_REG(I2C_SC_PORT, TWISTAT) & SC_TWISTAT_TWIRXFIN)) {
      if (halStackGetInt32uSymbolTick() > start + TRANSACTION_TIMEOUT) {
        i2cRestoreConfigSC();
        return I2C_DRIVER_ERR_TIMEOUT;
      }
    }
    buffer[i] = SCx_REG(I2C_SC_PORT, DATA); // Read and save I2C data
  }

  // Step four: Transaction complete, send the STOP signal and wait until the
  // SC_TWISTAT_TWICMDFIN bit to clear, indicating transaction complete
  SCx_REG(I2C_SC_PORT, TWICTRL1) = SC_TWICTRL1_TWISTOP;
  start = halStackGetInt32uSymbolTick();
  while (!(SCx_REG(I2C_SC_PORT, TWISTAT) & SC_TWISTAT_TWICMDFIN)) {
    if (halStackGetInt32uSymbolTick() > start + TRANSACTION_TIMEOUT) {
      i2cRestoreConfigSC();
      return I2C_DRIVER_ERR_TIMEOUT;
    }
  }

  i2cRestoreConfigSC();
  return I2C_DRIVER_ERR_NONE;
}
