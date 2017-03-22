// *****************************************************************************
// * generic-interrupt-control-em3xx.c
// *
// * Provides an interface for a generic interrupt controller.  Users specify
// * The port, pin, and irq number of the interrupt, and this plugin will
// * handle all configuration to enable the interrupt handler, call a user
// * specified interrupt service routine, and activate a user specified event
// *
// * Copyright 2015 Silicon Laboratories, Inc.                              *80*
// *****************************************************************************

#include PLATFORM_HEADER
#include CONFIGURATION_HEADER
#include "stack/include/ember-types.h"
#include "stack/include/event.h"
#include EMBER_AF_API_GENERIC_INTERRUPT_CONTROL
#include "hal/hal.h"
#include "hal/micro/micro-common.h"
#include "hal/micro/cortexm3/micro-common.h"

//------------------------------------------------------------------------------
// Plugin private macro definitions

//------------------------------------------------------------------------------
// Forward Declaration of private functions
static void genericIsr(HalGenericInterruptControlIrqCfg* irqCfg);

//------------------------------------------------------------------------------
// private global variables
static HalGenericInterruptControlIrqCfg irqACfg;
static HalGenericInterruptControlIrqCfg irqBCfg;
static HalGenericInterruptControlIrqCfg irqCCfg;
static HalGenericInterruptControlIrqCfg irqDCfg;

//------------------------------------------------------------------------------
// private plugin functions

// This function will be called whenever the hardware conditions are met to
// trigger an interrupt on a GIC controlled irq.  If the user has defined an
// interrupt subroutine using the @gicIrqISRAssignFxn, that function will be
// called.  If the user has defined an event to be associated with the irq using
// the @gicIRQEventRegister function, that event will be activated and run (in a
// non-interrupt context), regardless of whether an ISR function was also
// assigned to the IRQ.
//
// If no ISR has been assigned to the IRQ, this function will clear the
// interrupt flags and activate the user defined ember event (unless no
// event has been defined in which case no action will be taken).
static void genericIsr(HalGenericInterruptControlIrqCfg* irqCfg)
{
  // If the user hasn't defined an interrupt handler, the plugin will clear the
  // missed interrupt and top level interrupt flags, then activate the user
  // specified event (if one was specified)
  if (irqCfg->irqISR == NULL)
  {
      INT_MISS = irqCfg->irqMissBit;
      INT_GPIOFLAG = irqCfg->irqFlagBit;
  } else {
    irqCfg->irqISR();
  }

  // Activate the user specified event
  if (irqCfg->irqEventHandler != NULL) {
    emberEventControlSetActive(*(irqCfg->irqEventHandler));
  }
}

//------------------------------------------------------------------------------
// IrqA interrupt service routine
//
// !!!!!!!!!!!!!! THIS FUNCTION WILL RUN IN INTERRUPT CONTEXT !!!!!!!!!!!!!!!!!!
#ifdef HAL_GIC_USE_IRQA_ISR
void halIrqAIsr(void)
{
  genericIsr(&irqACfg);
}
#endif

//------------------------------------------------------------------------------
// IrqB interrupt service routine
//
// !!!!!!!!!!!!!! THIS FUNCTION WILL RUN IN INTERRUPT CONTEXT !!!!!!!!!!!!!!!!!!
#ifdef HAL_GIC_USE_IRQB_ISR
void halIrqBIsr(void)
{
  genericIsr(&irqBCfg);
}
#endif

//------------------------------------------------------------------------------
// IrqC interrupt service routine
//
// !!!!!!!!!!!!!! THIS FUNCTION WILL RUN IN INTERRUPT CONTEXT !!!!!!!!!!!!!!!!!!
#ifdef HAL_GIC_USE_IRQC_ISR
void halIrqCIsr(void)
{
  genericIsr(&irqCCfg);
}
#endif

//------------------------------------------------------------------------------
// IrqD interrupt service routine
//
// !!!!!!!!!!!!!! THIS FUNCTION WILL RUN IN INTERRUPT CONTEXT !!!!!!!!!!!!!!!!!!
#ifdef HAL_GIC_USE_IRQD_ISR
void halIrqDIsr(void)
{
  genericIsr(&irqDCfg);
}
#endif

//------------------------------------------------------------------------------
// public plugin functions

HalGenericInterruptControlIrqCfg* halGenericInterruptControlIrqCfgInitialize(
  uint8_t irqPin,
  uint8_t irqPort,
  uint8_t irqNum)
{
  HalGenericInterruptControlIrqCfg *config;

  if (irqNum == HAL_GIC_IRQ_NUMA) {
    config = &irqACfg;
  } else if (irqNum == HAL_GIC_IRQ_NUMB) {
    config = &irqBCfg;
  } else if (irqNum == HAL_GIC_IRQ_NUMC) {
    config = &irqCCfg;
  } else {
    config = &irqDCfg;
  }

  // First, initialize variables based on GPIO port:
  //    irqInReg
  //    irqPin
  if (irqPort == HAL_GIC_GPIO_PORTA) {
    config->irqInReg = (uint32_t *)GPIO_PAIN_ADDR;
    config->irqPin = irqPin;
    config->irqSelBit = PORTA_PIN(irqPin);
  } else if (irqPort == HAL_GIC_GPIO_PORTB) {
    config->irqInReg = (uint32_t *)GPIO_PBIN_ADDR;
    config->irqPin = irqPin;
    config->irqSelBit = PORTB_PIN(irqPin);
  } else if (irqPort == HAL_GIC_GPIO_PORTC) {
    config->irqInReg = (uint32_t *)GPIO_PCIN_ADDR;
    config->irqPin = irqPin;
    config->irqSelBit = PORTC_PIN(irqPin);
  }

  // Next, initialize variables based on IRQ number (a-d)
  //    irqIntCfgReg
  //    irqIntEnBit
  //    irqFlagBit
  //    irqMissBit
  //    for irq C-D, set GPIO_IRQ<x> port/pin select register
  if (irqNum == HAL_GIC_IRQ_NUMA) {
    config = &irqACfg;
    config->irqIntCfgReg = (uint32_t *)GPIO_INTCFGA_ADDR;
    config->irqIntEnBit = INT_IRQA;
    config->irqFlagBit = INT_IRQAFLAG;
    config->irqMissBit = INT_MISSIRQA;
  } else if (irqNum == HAL_GIC_IRQ_NUMB) {
    config = &irqBCfg;
    config->irqIntCfgReg = (uint32_t *)GPIO_INTCFGB_ADDR;
    config->irqIntEnBit = INT_IRQB;
    config->irqFlagBit = INT_IRQBFLAG;
    config->irqMissBit = INT_MISSIRQB;
  } else if (irqNum == HAL_GIC_IRQ_NUMC) {
    config = &irqCCfg;
    config->irqIntCfgReg = (uint32_t *)GPIO_INTCFGC_ADDR;
    config->irqIntEnBit = INT_IRQC;
    config->irqFlagBit = INT_IRQCFLAG;
    config->irqMissBit = INT_MISSIRQC;
    GPIO_IRQCSEL = config->irqSelBit;
  } else if (irqNum == HAL_GIC_IRQ_NUMD) {
    config = &irqDCfg;
    config->irqIntCfgReg = (uint32_t *)GPIO_INTCFGD_ADDR;
    config->irqIntEnBit = INT_IRQD;
    config->irqFlagBit = INT_IRQDFLAG;
    config->irqMissBit = INT_MISSIRQD;
    GPIO_IRQDSEL = config->irqSelBit;
  }

  // Finally, initialize all default variables:
  //    irqISR (user can define their own ISR if they so desire)
  //    irqEventHandler (user can define event to active on interrupt)
  //    irqEdgeCfg (by default, we trigger on all edges)
  config->irqISR = NULL;
  config->irqEventHandler = NULL;
  config->irqEdgeCfg = HAL_GIC_INT_CFG_EDGE_BOTH;

  // Now that the config struct is populated, use the information therein to
  // configure the IRQ.  Do not enable yet.
  *(config->irqIntCfgReg) = 0;
  INT_CFGCLR = config->irqIntEnBit;
  INT_GPIOFLAG = config->irqFlagBit;
  INT_MISS = config->irqMissBit;

  // Disable digital filtering on the GPIO by default
  *(config->irqIntCfgReg) = (0 << GPIO_INTFILT_BIT);

  // Set the interrupt edge to the default value of all edges
  *(config->irqIntCfgReg) |= (config->irqEdgeCfg << GPIO_INTMOD_BIT);

  return(config);
}

void halGenericInterruptControlIrqEdgeConfig(
  HalGenericInterruptControlIrqCfg *config,
  uint8_t edge)
{
  uint32_t oldRegValue;
  uint32_t newRegValue;
  config->irqEdgeCfg = edge;
  oldRegValue = *(config->irqIntCfgReg);
  newRegValue = oldRegValue & ~GPIO_INTMOD_MASK;
  newRegValue |=  (config->irqEdgeCfg << GPIO_INTMOD_BIT);
  *(config->irqIntCfgReg) = newRegValue;
}

void halGenericInterruptControlIrqIsrAssignFxn(
  HalGenericInterruptControlIrqCfg *config,
  void (*isr)(void))
{
  config->irqISR = isr;
}

void halGenericInterruptControlIrqIsrRemoveFxn(
  HalGenericInterruptControlIrqCfg *config)
{
  config->irqISR = NULL;
}

void halGenericInterruptControlIrqEventRegister(
  HalGenericInterruptControlIrqCfg *config, EmberEventControl *event)
{
  config->irqEventHandler = event;
}

void halGenericInterruptControlIrqClear(
  HalGenericInterruptControlIrqCfg *config)
{
  INT_MISS = config->irqMissBit;
  INT_GPIOFLAG = config->irqFlagBit;
}

void halGenericInterruptControlIrqEnable(
  HalGenericInterruptControlIrqCfg *config)
{
  uint32_t oldRegValue;
  uint32_t newRegValue;

  // First, configure the GPIO_INTCFG register
  oldRegValue = *(config->irqIntCfgReg);
  newRegValue = oldRegValue & ~GPIO_INTMOD_MASK;
  newRegValue |=  (config->irqEdgeCfg << GPIO_INTMOD_BIT);
  *(config->irqIntCfgReg) = newRegValue;

  // Next, enable the top level interrupt
  INT_CFGSET |= config->irqIntEnBit;
}

void halGenericInterruptControlIrqDisable(
  HalGenericInterruptControlIrqCfg *config)
{
  // First, disable the top level interrupt
  INT_CFGSET &= ~(config->irqIntEnBit);

  // Next, set disable the GPIO level interrupt
  *(config->irqIntCfgReg) &= ~GPIO_INTMOD_MASK;
}

uint8_t halGenericInterruptControlIrqReadGpio(
  HalGenericInterruptControlIrqCfg *config)
{
  uint32_t inVal;
  uint8_t retVal;
  inVal = *(config->irqInReg);
  retVal = (uint8_t)inVal & BIT(config->irqPin);
  return retVal;
}

