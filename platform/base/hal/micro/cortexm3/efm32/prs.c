/***************************************************************************//**
 * @file prs.c
 * @brief This file implements a simple API for configuring a PRS channel.
 * @copyright Copyright 2016 Silicon Laboratories, Inc. http://www.silabs.com
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>

#include "em_device.h"
#include "em_cmu.h"
#include "em_bus.h"

/**
 * Configure the PRS to use a specific source and signal for the specified
 * channel/location combo.
 * @param channel The PRS channel to use for this signal.
 * @param loc The location to route this PRS channel to. See the datasheet for
 * more information. Set this to 0xFF to disable the output channel.
 * @param source The PRS source to use for this channel.
 * @param signal The signal in the above source to use for this channel.
 * @param invert Set this to true to invert the polarity of the output signal.
 */
void halConfigurePrs(uint8_t channel,
                     uint8_t loc,
                     uint8_t source,
                     uint8_t signal,
                     bool invert)
{
  volatile uint32_t *routeLocPtr;

  // Make sure the PRS is on and clocked
  CMU_ClockEnable(cmuClock_PRS, true);

  // Make sure this PRS channel is off while reconfiguring
  BUS_RegBitWrite(&PRS->ROUTEPEN,
                  _PRS_ROUTEPEN_CH0PEN_SHIFT + channel,
                  0);

  // If the location is set to an invalid value then exit early
  if (loc == 0xFF) {
    return;
  }

  PRS->CH[channel].CTRL = signal << _PRS_CH_CTRL_SIGSEL_SHIFT
                          | source << _PRS_CH_CTRL_SOURCESEL_SHIFT
                          | PRS_CH_CTRL_EDSEL_OFF;

  // Enable channel output inversion if requested
  if (invert) {
    PRS->CH[channel].CTRL |= PRS_CH_CTRL_INV;
  }

  // Configure the output location for this PRS channel
  routeLocPtr   = &PRS->ROUTELOC0 + (channel/4);
  *routeLocPtr &= ~(0xFF << (_PRS_ROUTELOC0_CH1LOC_SHIFT
                             * (channel % 4)));
  *routeLocPtr |= loc << (_PRS_ROUTELOC0_CH1LOC_SHIFT
                          * (channel % 4));

  // Set the enable bit for this PRS channel
  BUS_RegBitWrite(&PRS->ROUTEPEN,
                  _PRS_ROUTEPEN_CH0PEN_SHIFT + channel,
                  1);
}
