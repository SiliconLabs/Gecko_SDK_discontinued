// *****************************************************************************
// * occupancy-pyd1698-em3xx.c
// *
// * API for interfacing with a PYD-1698 occupancy sensor
// *
// * Copyright 2015 Silicon Laboratories, Inc.                              *80*
// *****************************************************************************

#include PLATFORM_HEADER
#include "stack/include/ember-types.h"
#include "stack/include/event.h"
#include "hal/hal.h"
#include "hal/micro/micro.h"
#include "occupancy-pyd1698.h"
#include EMBER_AF_API_GENERIC_INTERRUPT_CONTROL
#include EMBER_AF_API_OCCUPANCY

//------------------------------------------------------------------------------
// Plugin private macros

// helper macros, for shortening long plugin option macros
// Some of these have conversion addition or subtractions.  The plugin options
// are meant to control the resulting behavior, not the value that needs to be
// written to the corresponding register.  For example, the blind time register
// accepts 0..15, but corresponds to 1..16 half seconds, so the macro for
// DEFAULT_BLIND_TIME_HS subtracts 1 from the user entered plugin value
#define DEFAULT_THRESHOLD       EMBER_AF_PLUGIN_OCCUPANCY_PYD1698_THRESHOLD
#define DEFAULT_BLIND_TIME_HS \
          (EMBER_AF_PLUGIN_OCCUPANCY_PYD1698_BLIND_TIME - 1)
#define DEFAULT_WINDOW_TIME     EMBER_AF_PLUGIN_OCCUPANCY_PYD1698_WINDOW_TIME
#define DEFAULT_PULSE_COUNTER \
          (EMBER_AF_PLUGIN_OCCUPANCY_PYD1698_PULSE_COUNTER - 1)
#define DEFAULT_FILTER          EMBER_AF_PLUGIN_OCCUPANCY_PYD1698_FILTER_SOURCE
#define DEFAULT_MODE            EMBER_AF_PLUGIN_OCCUPANCY_PYD1698_OPERATION_MODE
#define OCCUPANCY_TIMEOUT_MINUTES \
          EMBER_AF_PLUGIN_OCCUPANCY_PYD1698_OCCUPANCY_TIMEOUT
#define OCCUPANCY_CALIBRATION_TIMEOUT_S \
          EMBER_AF_PLUGIN_OCCUPANCY_PYD1698_OCCUPANCY_CALIBRATION_TIMEOUT
#define FIRMWARE_BLIND_TIME_M \
          EMBER_AF_PLUGIN_OCCUPANCY_PYD1698_FIRMWARE_BLIND_TIME
#define SENSOR_WARMUP_TIME_QS   (4*5)

// Pin configuration checks.  These should be defined in the board.h file, but
// default definitions are provided here to match the IST-A49 reference design
#ifndef OCCUPANCY_PYD1698_SERIN_PORT
#error "OCCUPANCY_PYD1698_SERIN_PORT not defined in board.h.  Select a port by defining that macro to something like HAL_GIC_GPIO_PORT_B"
#endif

#ifndef OCCUPANCY_PYD1698_SERIN_PIN
#error "OCCUPANCY_PYD1698_SERIN_PIN not defined in board.h.  Select a port by defining that macro to something like 3"
#endif

#ifndef OCCUPANCY_PYD1698_DLINK_PORT
#error "OCCUPANCY_PYD1698_DLINK_PORT not defined in board.h.  Select a port by defining that macro to something like HAL_GIC_GPIO_PORT_C"
#endif

#ifndef OCCUPANCY_PYD1698_DLINK_PIN
#error "OCCUPANCY_PYD1698_DLINK_PIN not defined in board.h.  Select a port by defining that macro to something like 6"
#endif

#ifndef OCCUPANCY_PYD1698_INSTALLATION_JP_PORT
#error "OCCUPANCY_PYD1698_INSTALLATION_JP_PORT not defined in board.h.  Select a port by defining that macro to something like HAL_GIC_PORTB"
#endif

#ifndef OCCUPANCY_PYD1698_INSTALLATION_JP_PIN
#error "OCCUPANCY_PYD1698_INSTALLATION_JP_PIN not defined in board.h.  Select a port by defining that macro to something like 4"
#endif

// For the CFGL/H, IN, OUT, CLR, and SET GPIO registers, the offset between port
// registers will always be the same.  This offset will be used to determine
// what register to write to based on the OCCUPANCY_PYD port macros defined in
// the board.h
// Note that the _PORT macros are 1-3 (for A-C), so one must be subtracted from
// the port macro to get the correct offset
#define SERIN_GPIO_PORT_OFFSET \
          ((GPIO_PBCFGL_ADDR - GPIO_PACFGL_ADDR) \
           * (OCCUPANCY_PYD1698_SERIN_PORT - 1))
#define DLINK_GPIO_PORT_OFFSET \
          ((GPIO_PBCFGL_ADDR - GPIO_PACFGL_ADDR) \
           * (OCCUPANCY_PYD1698_DLINK_PORT - 1))
#define INSTALLATION_JP_PORT_OFFSET \
          ((GPIO_PBCFGL_ADDR - GPIO_PACFGL_ADDR) \
           * (OCCUPANCY_PYD1698_INSTALLATION_JP_PORT - 1))

#define SERIN_GPIO_SET *((volatile uint32_t *)(GPIO_PASET_ADDR \
                                             + SERIN_GPIO_PORT_OFFSET))
#define SERIN_GPIO_CLR *((volatile uint32_t *)(GPIO_PACLR_ADDR \
                                             + SERIN_GPIO_PORT_OFFSET))
#if OCCUPANCY_PYD1698_SERIN_PIN < 4
#define SERIN_GPIO_CFG *((volatile uint32_t *)(GPIO_PACFGL_ADDR \
                                             + SERIN_GPIO_PORT_OFFSET))
#else
#define SERIN_GPIO_CFG *((volatile uint32_t *)(GPIO_PACFGH_ADDR \
                                             + SERIN_GPIO_PORT_OFFSET))
#endif

#define DLINK_GPIO_SET *((volatile uint32_t *)(GPIO_PASET_ADDR \
                                             + DLINK_GPIO_PORT_OFFSET))
#define DLINK_GPIO_CLR *((volatile uint32_t *)(GPIO_PACLR_ADDR \
                                             + DLINK_GPIO_PORT_OFFSET))
#define DLINK_GPIO_IN *((volatile uint32_t *)(GPIO_PAIN_ADDR \
                                            + DLINK_GPIO_PORT_OFFSET))

#define INSTALLATION_JP_GPIO_IN *((volatile uint32_t *)(GPIO_PAIN_ADDR \
                                                 + INSTALLATION_JP_PORT_OFFSET))

// the config registers are special, because they have a high/low based on if
// the pin is 0-3 or 4-7
#if OCCUPANCY_PYD1698_DLINK_PIN < 4
#define DLINK_GPIO_CFG *((volatile uint32_t *)(GPIO_PACFGL_ADDR \
                                             + DLINK_GPIO_PORT_OFFSET))
#define DLINK_CFG_BIT (4 * OCCUPANCY_PYD1698_DLINK_PIN)
#define DLINK_CFG_MASK (0xF << (OCCUPANCY_PYD1698_DLINK_PIN*4)
#else
#define DLINK_GPIO_CFG *((volatile uint32_t *)(GPIO_PACFGH_ADDR \
                                             + DLINK_GPIO_PORT_OFFSET))
#define DLINK_CFG_BIT (4 * (OCCUPANCY_PYD1698_DLINK_PIN - 4))
#define DLINK_CFG_MASK (0xF << ((OCCUPANCY_PYD1698_DLINK_PIN-4)*4))
#endif

//------------------------------------------------------------------------------
// Plugin events
EmberEventControl emberAfPluginOccupancyPyd1698NotifyEventControl;
EmberEventControl emberAfPluginOccupancyPyd1698OccupancyTimeoutEventControl;
EmberEventControl emberAfPluginOccupancyPyd1698FirmwareBlindTimeoutEventControl;
EmberEventControl emberAfPluginOccupancyPyd1698InitEventControl;

//------------------------------------------------------------------------------
// Forward declaration of private functions
static uint32_t cfgMsgToData(HalPydCfg_t *cfgMsg);
static void ackSerIn(void);
static void ackDlink(void);
static bool handleJumperChange(bool isInit);
static void clearInterupt(void);
static void irqIsr(void);

//------------------------------------------------------------------------------
// Private global variables

// structure used to store irq configuration from GIC plugin
static HalGenericInterruptControlIrqCfg *irqConfig;

// structure used to track configuration data for sensor
static HalPydCfg_t currentPydCfg;

static bool initialized = false;

//------------------------------------------------------------------------------
// Plugin public APIs, callbacks, and event handlers

// Configure the GIC to call the occupancySensedEvent on IRQ from the PYD
// Send an initial configuration message to the occupancy sensor
void halOccupancyInit(void)
{
  // Set up GIC to interrupt on low to high transition of DATALINK/IRQ pin
  irqConfig = halGenericInterruptControlIrqCfgInitialize(
    OCCUPANCY_PYD1698_DLINK_PIN,
    OCCUPANCY_PYD1698_DLINK_PORT,
    OCCUPANCY_PYD1698_IRQ);

  halGenericInterruptControlIrqEventRegister(
    irqConfig,
    &emberAfPluginOccupancyPyd1698NotifyEventControl);

  halGenericInterruptControlIrqDisable(irqConfig);

  halGenericInterruptControlIrqIsrAssignFxn(irqConfig, irqIsr);

  halGenericInterruptControlIrqEdgeConfig(irqConfig, HAL_GIC_INT_CFG_LEVEL_POS);

  currentPydCfg.reserved = PYD_CONFIG_RESERVED_VALUE;
  currentPydCfg.filterSource = DEFAULT_FILTER;
  currentPydCfg.operationMode = DEFAULT_MODE;
  currentPydCfg.windowTime = DEFAULT_WINDOW_TIME;
  currentPydCfg.pulseCounter = DEFAULT_PULSE_COUNTER;
  currentPydCfg.blindTime = DEFAULT_BLIND_TIME_HS;
  currentPydCfg.sensitivity = DEFAULT_THRESHOLD;

  // check the state of the jumper, which could cause the blind time to change
  // from the above set default.  Note this will also trigger a write of the
  // configuration data to the occupancy sensor
  clearInterupt();
  handleJumperChange(true);

  INT_MISS = irqConfig->irqMissBit;
  INT_GPIOFLAG = irqConfig->irqFlagBit;
  halGenericInterruptControlIrqEnable(irqConfig);

  emberEventControlSetDelayQS(emberAfPluginOccupancyPyd1698InitEventControl,
                              SENSOR_WARMUP_TIME_QS);
}

void emberAfPluginOccupancyPyd1698InitEventHandler(void)
{
  emberEventControlSetInactive(emberAfPluginOccupancyPyd1698InitEventControl);
  initialized = true;
}

// This event will be called by the GIC plugin whenever an interrupt is detected
// from the occupancy sensor.
void emberAfPluginOccupancyPyd1698NotifyEventHandler(void)
{
  emberEventControlSetInactive(emberAfPluginOccupancyPyd1698NotifyEventControl);

  clearInterupt();
  if (!initialized) {
    return;
  }

  halOccupancyStateChangedCallback(HAL_OCCUPANCY_STATE_OCCUPIED);

  if (handleJumperChange(false) == true) {
    // Ensure that the device doesn't remain in firmware blind mode when the
    // installation mode jumper is in place
    halGenericInterruptControlIrqEnable(irqConfig);
    emberEventControlSetInactive(
      emberAfPluginOccupancyPyd1698FirmwareBlindTimeoutEventControl);
  } else {
    // If the jumper is not in place, enter the firmware blind mode
    halGenericInterruptControlIrqDisable(irqConfig);
    emberEventControlSetDelayMinutes(
      emberAfPluginOccupancyPyd1698FirmwareBlindTimeoutEventControl,
      FIRMWARE_BLIND_TIME_M);
  }
}

// This event is activated at the end of the firmware blind time.  It must
// determine if an occupancy event occurred during that blind time and act
// accordingly.
void emberAfPluginOccupancyPyd1698FirmwareBlindTimeoutEventHandler(void)
{
  emberEventControlSetInactive(
    emberAfPluginOccupancyPyd1698FirmwareBlindTimeoutEventControl);

  if (DLINK_GPIO_IN & BIT(OCCUPANCY_PYD1698_DLINK_PIN)){
    // If an occupancy event occurred while the device was sleeping, handle it.
    emberEventControlSetActive(emberAfPluginOccupancyPyd1698NotifyEventControl);
  } else {
    // Otherwise, enable interrupts, and the device will wake either the next
    // time the room is occupied or when enough time has passed for the room to
    // be considered unoccupied.
    halGenericInterruptControlIrqEnable(irqConfig);
  }
}

// This event is triggered when no occupancy events have occurred for a long
// enough time that the room should now be considered unoccupied
void emberAfPluginOccupancyPyd1698OccupancyTimeoutEventHandler(void)
{
  emberEventControlSetInactive(
    emberAfPluginOccupancyPyd1698OccupancyTimeoutEventControl);

  halOccupancyStateChangedCallback(
    HAL_OCCUPANCY_STATE_UNOCCUPIED);
}

// The init callback, which will be called by the framework on init.
void emberAfPluginOccupancyPyd1698InitCallback(void)
{
  halOccupancyInit();
}

HalOccupancySensorType halOccupancyGetSensorType(void)
{
  return HAL_OCCUPANCY_SENSOR_TYPE_PIR;
}

//------------------------------------------------------------------------------
// Private plugin functions

static void irqIsr(void)
{
  clearInterupt();
  INT_MISS = irqConfig->irqMissBit;
  INT_GPIOFLAG = irqConfig->irqFlagBit;
}

// returns true if the jumper is set, false if the jumper is not set.  isInit
// should be true if this is the first time the jumper is being queried.  This
// will activate the occupancyTimeout event, update the static jumper state
// variable, and write the new blind time if this is the first transition to a
// new jumper state.
static bool handleJumperChange(bool isInit)
{
  static bool jumperInserted;

  // on initialization, the jumper will be assumed to be set
  if (isInit) {
    jumperInserted = true;
  }

  if (INSTALLATION_JP_GPIO_IN & BIT(OCCUPANCY_PYD1698_INSTALLATION_JP_PIN)) {
    // if jumper is not in place
    if ((jumperInserted == true) || (isInit == true)) {
      // if the jumper just transitioned from in place to not in place, reset
      // the blind time to the plugin option specified value, standard operation
      // mode.
      currentPydCfg.blindTime = DEFAULT_BLIND_TIME_HS;
      halOccupancyPyd1698WriteConfiguration(&currentPydCfg);
    }
    jumperInserted = false;
    emberEventControlSetDelayMinutes(
      emberAfPluginOccupancyPyd1698OccupancyTimeoutEventControl,
      OCCUPANCY_TIMEOUT_MINUTES);
  } else {
    // if jumper is in place
    if ((jumperInserted == false) || (isInit == true)) {
      // if the jumper just transition from not in place to in place, the user
      // has entered  installation mode, so the blind time should be set to its
      // minimum value
      currentPydCfg.blindTime = 0;
      halOccupancyPyd1698WriteConfiguration(&currentPydCfg);
    }
    jumperInserted = true;
    emberEventControlSetDelayQS(
      emberAfPluginOccupancyPyd1698OccupancyTimeoutEventControl,
      OCCUPANCY_CALIBRATION_TIMEOUT_S*4);
  }
  return jumperInserted;
}

void halOccupancyPyd1698WriteConfiguration(HalPydCfg_t *cfgMsg)
{
  uint32_t outData;
  volatile uint8_t i;
  uint32_t startWait;

  outData = cfgMsgToData(cfgMsg);

  // The timing windows are very tight in this section.  Adding tens of cycles
  // between GPIO transitions is enough to exceed device tolerances.  As such,
  // all interrupts will be disabled during this time sensitive bit banged
  // transaction
ATOMIC(
  for (i=0; i<PYD_MESSAGE_WRITE_LENGTH_BITS; i++) {
    // Drive the SerIn pin hi for ~5 cycles, then low for ~5 cycles, informing
    // the PYD that the next bit is going to be written
    ackSerIn();

    // Set hi or low based on the data to be written
    if (outData & (1 << (PYD_MESSAGE_WRITE_LENGTH_BITS - 1 - i))) {
      SERIN_GPIO_SET = BIT(OCCUPANCY_PYD1698_SERIN_PIN);
    } else {
      SERIN_GPIO_CLR = BIT(OCCUPANCY_PYD1698_SERIN_PIN);
    }
    startWait = halStackGetInt32uSymbolTick();
    halResetWatchdog();
    // Delay long enough for the PYD to read the bit
    while (halStackGetInt32uSymbolTick() < startWait
                                           + PYD_MESSAGE_WRITE_BIT_DELAY_US);
  }
  SERIN_GPIO_CLR = BIT(OCCUPANCY_PYD1698_SERIN_PIN);
)
}

void halOccupancyPyd1698Read(HalPydInMsg_t *readMsg)
{
  volatile uint32_t j;
  uint8_t i;
  uint32_t oldCfg;
  uint32_t outCfg;
  uint32_t inCfg;
  uint32_t startWait;
  volatile uint64_t data;

  data = 0;

  // To initiate a read, first reconfigure the interrupt/ser_in pin as an output
  // Then, drive it high for 110-150 uS.
  // Then, switch to an input, wait for the input to be driven by the PYD, and
  //   read the input bit
  // Finally, switch back to an output and drive the signal high for another
  //   110-150 uS, signalling to the PYD that the bit was received and it can
  //   transmit another.

  // Since the IRQ bit doubles as the data in bit, we need to disable interrupts
  // until the read is complete in order to prevent the system from interrupting
  // on each bit read.
  halGenericInterruptControlIrqDisable(irqConfig);

  // Set the DLINK bit of the GPIO set register, to guarantee output will be
  // driven high once pin is enabled as output
  DLINK_GPIO_SET = BIT(OCCUPANCY_PYD1698_DLINK_PIN);

  // Save old config state, then set pin 6 to be output
  oldCfg = DLINK_GPIO_CFG;
  outCfg = oldCfg & ~(DLINK_CFG_MASK);
  inCfg = outCfg | (GPIOCFG_IN << DLINK_CFG_BIT);
  outCfg |= (GPIOCFG_OUT << DLINK_CFG_BIT);

  startWait = halStackGetInt32uSymbolTick();
  DLINK_GPIO_CFG = outCfg;

  // Hold hi for 110-150 uS
  while (halStackGetInt32uSymbolTick() < startWait + DATA_SETUP_MIN_US);

  // The timing windows are very tight in this section.  Adding tens of cycles
  // between GPIO transitions is enough to exceed device tolerances.  As such,
  // all interrupts will be disabled during this time sensitive bit banged
  //  transaction
ATOMIC(
  for (i=0; i<PYD_MESSAGE_READ_LENGTH_BITS; i++) {
    // Drive the output high for ~5 cycles, then low for ~5 cycles, triggering
    // the next bit of data from the PYD
    ackDlink();

    // Set the pin to input
    DLINK_GPIO_CFG = inCfg;

    // we need to wait long enough for the PYD to drive the signal low.  2 uS
    // should do, derived from experimentation.
    startWait = halStackGetInt32uSymbolTick();
    halResetWatchdog();
    while (halStackGetInt32uSymbolTick() < startWait
                                           + PYD_MESSAGE_READ_BIT_DELAY_US);

    // read in the data
    if (DLINK_GPIO_IN & BIT(OCCUPANCY_PYD1698_DLINK_PIN)) {
      data |= 1 << (PYD_MESSAGE_READ_LENGTH_BITS - 1 - i);
    }

    DLINK_GPIO_CLR = BIT(OCCUPANCY_PYD1698_DLINK_PIN);
    DLINK_GPIO_CFG = outCfg;
  }
)

  // convert the read message to configuration structure format
  readMsg->config->reserved = (data & PYD_CONFIG_RESERVED_MASK) >>
                             PYD_CONFIG_RESERVED_BIT;
  readMsg->config->filterSource = (data & PYD_CONFIG_FILTER_SRC_MASK) >>
                                PYD_CONFIG_FILTER_SRC_BIT;
  readMsg->config->operationMode = (data & PYD_CONFIG_OPERATION_MODE_MASK) >>
                                 PYD_CONFIG_OPERATION_MODE_BIT;
  readMsg->config->windowTime = (data & PYD_CONFIG_WINDOW_TIME_MASK) >>
                              PYD_CONFIG_WINDOW_TIME_BIT;
  readMsg->config->pulseCounter = (data & PYD_CONFIG_PULSE_COUNTER_MASK) >>
                                PYD_CONFIG_PULSE_COUNTER_BIT;
  readMsg->config->blindTime = (data & PYD_CONFIG_BLIND_TIME_MASK) >>
                             PYD_CONFIG_BLIND_TIME_BIT;
  readMsg->config->sensitivity = (data & PYD_CONFIG_SENSITIVITY_MASK) >>
                                PYD_CONFIG_SENSITIVITY_BIT;
  readMsg->AdcVoltage = ((data & PYD_ADC_VOLTAGE_MASK) >> PYD_ADC_VOLTAGE_BIT);

  // return the GPIO to its previous configuration
  DLINK_GPIO_CFG = oldCfg;

  // re-enable interrupt functionality
  halGenericInterruptControlIrqClear(irqConfig);
  halGenericInterruptControlIrqEnable(irqConfig);

  return;
}

void halOccupancyPyd1698GetCurrentConfiguration(HalPydCfg_t *config)
{
  config->reserved = currentPydCfg.reserved;
  config->filterSource = currentPydCfg.filterSource;
  config->operationMode = currentPydCfg.operationMode;
  config->windowTime = currentPydCfg.windowTime;
  config->pulseCounter = currentPydCfg.pulseCounter;
  config->blindTime = currentPydCfg.blindTime;
  config->sensitivity = currentPydCfg.sensitivity;
}

static uint32_t cfgMsgToData(HalPydCfg_t *cfgMsg)
{
  uint32_t retVal;

  retVal = 0;

  cfgMsg->reserved = PYD_CONFIG_RESERVED_VALUE;

  retVal |= (cfgMsg->reserved << PYD_CONFIG_RESERVED_BIT) &
             PYD_CONFIG_RESERVED_MASK;
  retVal |= (cfgMsg->filterSource << PYD_CONFIG_FILTER_SRC_BIT) &
             PYD_CONFIG_FILTER_SRC_MASK;
  retVal |= (cfgMsg->operationMode << PYD_CONFIG_OPERATION_MODE_BIT) &
             PYD_CONFIG_OPERATION_MODE_MASK;
  retVal |= (cfgMsg->windowTime << PYD_CONFIG_WINDOW_TIME_BIT) &
             PYD_CONFIG_WINDOW_TIME_MASK;
  retVal |= (cfgMsg->pulseCounter << PYD_CONFIG_PULSE_COUNTER_BIT) &
             PYD_CONFIG_PULSE_COUNTER_MASK;
  retVal |= (cfgMsg->blindTime << PYD_CONFIG_BLIND_TIME_BIT) &
             PYD_CONFIG_BLIND_TIME_MASK;
  retVal |= (cfgMsg->sensitivity << PYD_CONFIG_SENSITIVITY_BIT) &
             PYD_CONFIG_SENSITIVITY_MASK;

  return retVal;
}

static void clearInterupt(void)
{
  uint32_t oldCfg;
  uint32_t outCfg;

  // drive the DLink pin low to reset interrupt
  DLINK_GPIO_CLR = BIT(OCCUPANCY_PYD1698_DLINK_PIN);

  // Save old config state, then set the pin to be output
  oldCfg = DLINK_GPIO_CFG;
  outCfg = oldCfg & ~(DLINK_CFG_MASK);
  outCfg |= (GPIOCFG_OUT << DLINK_CFG_BIT);

  // Set the pin as output
  DLINK_GPIO_CFG = outCfg;

  // Wait two cycles of the PYD's clock using NO OP instructions
  asm("NOP");
  asm("NOP");
  asm("NOP");
  asm("NOP");

  // Revert the config register to what it was before we modified it
  DLINK_GPIO_CFG = oldCfg;
}

static void ackSerIn(void)
{
  SERIN_GPIO_CLR = BIT(OCCUPANCY_PYD1698_SERIN_PIN);
  asm("NOP");
  asm("NOP");
  asm("NOP");
  asm("NOP");
  asm("NOP");
  SERIN_GPIO_SET = BIT(OCCUPANCY_PYD1698_SERIN_PIN);
  asm("NOP");
  asm("NOP");
  asm("NOP");
  asm("NOP");
  asm("NOP");
}

static void ackDlink(void)
{
  DLINK_GPIO_CLR = BIT(OCCUPANCY_PYD1698_DLINK_PIN);
  asm("NOP");
  asm("NOP");
  asm("NOP");
  asm("NOP");
  asm("NOP");
  DLINK_GPIO_SET = BIT(OCCUPANCY_PYD1698_DLINK_PIN);
  asm("NOP");
  asm("NOP");
  asm("NOP");
  asm("NOP");
  asm("NOP");
}
