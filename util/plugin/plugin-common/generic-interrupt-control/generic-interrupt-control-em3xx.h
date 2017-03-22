// *****************************************************************************
// * Generic Interrupt Control Em35x.h
// *
// * This contains the architecture specific definition of the configuration
// * structure used to interface with the generic interrupt control plugin
// *
// * Copyright 2015 Silicon Laboratories, Inc.                              *80*
// *****************************************************************************

#ifndef __GENERIC_INTERRUPT_CONTROL_EM35X_H__
#define __GENERIC_INTERRUPT_CONTROL_EM35X_H__

//------------------------------------------------------------------------------
// Plugin public macro definitions

//------------------------------------------------------------------------------
// Plugin public struct and enum definitions
typedef struct tIrqCfg
{
  uint32_t irqPin;
  uint32_t *irqInReg;
  uint32_t *irqIntCfgReg;
  uint32_t irqIntEnBit;
  uint32_t irqFlagBit;
  uint32_t irqMissBit;
  uint32_t irqEdgeCfg;
  uint32_t irqSelBit;
  void (*irqISR)(void); //fxn pointer for non-default ISR
  EmberEventControl *irqEventHandler; //ember event for default delayed handling
} HalGenericInterruptControlIrqCfg;

#endif //__GENERIC_INTERRUPT_CONTROL_EM35X_H__

