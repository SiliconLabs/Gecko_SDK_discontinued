/** 
 * @file hal/micro/cortexm3/spi-protocol2.h
 * @brief SPI Protocol implementation for use below the ip-modem-app-spi (ZIP
 * NCP) application.  This is distinct from EZSP-SPI which was used with ZNet
 * NCPs.
 *
 * <!-- Author(s): Ryan Meador, API borrowed heavily from Brooks Barrett -->
 * Copyright 2013 by Silicon Laboratories. All rights reserved.             *80*
 */

#ifndef __SPI_PROTOCOL2_H__
#define __SPI_PROTOCOL2_H__

// bits of a protocol command
#define NCP_SPI_PKT_CMD_PAT_MASK  0xE0
#define NCP_SPI_PKT_CMD_PAT       0x20  // Pattern for all commands
#define NCP_SPI_PKT_CMD_PAY       0x10  // Payload (length) is present
#define NCP_SPI_PKT_CMD_RST       0x08  // Reset happened
#define NCP_SPI_PKT_CMD_NAK       0x04  // we are ignoring RX
#define NCP_SPI_PKT_CMD_RSP       0x01  // Direction is response (NCP-to-Host)

#define NCP_SPI_ACK_ERROR         0xFF  // <ack> value that indicates error
#define NCP_SPI_LEN_ERROR         0xFF  // <len> value that indicates error

#define DIRECTION_NCP_TO_HOST     1
#define DIRECTION_HOST_TO_NCP     0

#define NCP_SPI_PKT_HEADER_LEN    2   // <cmd> <len>
#define NCP_SPI_PKT_TAIL_LEN      4   // bytes to pad out the FIFO so we can be
                                      // sure the whole DMA buffer was sent
#define NCP_SPI_PKT_OVERHEAD      ( NCP_SPI_PKT_HEADER_LEN \
                                  + NCP_SPI_PKT_TAIL_LEN )
#define NCP_SPI_MAX_PAYLOAD       254 // Max payload length per SPI transaction
#define NCP_SPI_RX_SLOP           5   // Rx tolerance of initial pad bytes
#define NCP_SPI_BUFSIZE           ( NCP_SPI_RX_SLOP \
                                  + NCP_SPI_PKT_OVERHEAD \
                                  + NCP_SPI_MAX_PAYLOAD )

#define NCP_SPI_CMD_INDEX         0
#define NCP_SPI_LENGTH_INDEX      1
#define MIN(a, b) ((a) < (b) ? (a) : (b))

/** @description 
 * Initialize SPI link
 */
void halHostSerialInit(void);

void halHostSerialTick(void);

/**
 * @description Reinitializes the SPI Protocol when coming out of sleep
 * (powerup).
 */
void halHostSerialPowerup(void);

/**
 * @description Shuts down the SPI Protocol when entering sleep (powerdown).
 */
void halHostSerialPowerdown(void);

/**
 * enqueue data for TX to host. Will cause a host callback (assert nHOST_INT).
 * Safe to call outside ATOMIC().
 */
uint16_t halHostEnqueueTx(const uint8_t* data, uint16_t length);

/**
 * check available space in the TX queue.  Must be called from within ATOMIC().
 */
uint16_t halHostTxSpaceAvailable(void);

/**
 * check if TX is idle (the queue is empty).  Must be called from within ATOMIC().
 */
bool halHostTxIsIdle(void);

/**
 * clear all data in RX and TX buffers without processing or transmitting it.
 * Also causes NCP to silently drop RX'ed data without processing (and without
 * indicaitng a NAK to the host) until the link is reinitialized with
 * halHostSerialInit().
 */
void halHostFlushBuffers(void);

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
#endif // __SPI_PROTOCOL2_H__

