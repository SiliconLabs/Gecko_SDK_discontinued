/** 
 * @file hal/micro/cortexm3/spi-protocol.h
 * @brief Internal SPI Protocol implementation for use below the EZSP
 * application.
 *
 * <!-- Author(s): Brooks Barrett -->
 * <!-- Copyright 2009 by Ember Corporation. All rights reserved.       *80*-->
 */

#ifndef __SPI_PROTOCOL_H__
#define __SPI_PROTOCOL_H__

// SPI prefix bytes
#define SPIP_RESET               0x00
#define SPIP_OVERSIZED_EZSP      0x01
#define SPIP_ABORTED_TRANSACTION 0x02
#define SPIP_MISSING_FT          0x03
#define SPIP_UNSUPPORTED_COMMAND 0x04
#define SPIP_VERSION             0x82
#define SPIP_ALIVE               0xC1

#define FRAME_TERMINATOR 0xA7

/**
 * @description  A pointer to the length byte at the start of the Payload.
 * Upper layers will read the command from this location after
 * halHostSerialTick(false) returns true. The upper layer will write the
 * response to this location before calling halHostSerialTick(true). This
 * pointer is the upper layers' primary access into the command/response buffer.
 */
extern uint8_t *halHostFrame;

/**
 * @description  A flag that is set to true when the Host initiates the wake
 * handshake.
 */
extern bool spipFlagWakeFallingEdge;


/** @description 
 * 
 * The SPIP Init routine will also set a flag in the SPIP indicating a
 * wakeup handshake should be performed.  The handshake should only be
 * performed on a SerialTick.  Upon the next Tick call, the SPIP can assume
 * we are fully booted and operational and then take control peforming the
 * full handshake.
 */
void halHostSerialInit(void);

/**
 * @description Reinitializes the SPI Protocol when coming out of sleep
 * (powerdown).
 */
void halHostSerialPowerup(void);

/**
 * @description Shuts down the SPI Protocol when entering sleep (powerdown).
 */
void halHostSerialPowerdown(void);

/** @description 
 *
 * When the upper application has a callback it needs to deliver to the Host, it
 * calls halHostCallback() at will with haveData set to true. The HAL will
 * indicate to the Host through the nHOST_INT singal that there is a callback
 * pending. The EZSP application must make another call with haveData set to
 * false when there are no more callbacks pending.  The SPIP is responsible
 * for latching this call, timing actual nHOST_INT manipulation, and
 * multiplexing it in with SPIP generated assertions.
 *
 * @param haveData: true indicates there is a callback and the SPIP should
 * schedule nHOST_INT assertion.  false says the SPIP and deassert nHOST_INT.
 */
void halHostCallback(bool haveData);

/**
 * @description Returns true if SPIP is busy.
 */
bool halHostSerialBusy(void);


/** @description Test function used by haltest. Nothing to see here...
 *  
 * @param : 
 *
 * @param : 
 */
void spipTest(uint16_t test, uint16_t params );

#endif // __SPI_PROTOCOL_H__

