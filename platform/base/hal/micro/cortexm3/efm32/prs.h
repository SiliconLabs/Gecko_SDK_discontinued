/***************************************************************************//**
 * @file prs.h
 * @brief This is the header for a simple PRS channel configuration API.
 * @copyright Copyright 2016 Silicon Laboratories, Inc. http://www.silabs.com
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>

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
                     bool invert);
