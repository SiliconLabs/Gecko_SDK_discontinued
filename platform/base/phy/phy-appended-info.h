/*
 * File: phy/phy-appended-info.h
 * Description: APPENDED_INFO byte definitions for RX packets.
 *
 * Copyright 2013 Silicon Laboratories, Inc.                                *80*
 */

#ifndef __PHY_APPENDED_INFO__
#define __PHY_APPENDED_INFO__

// Appended Info
// Stored as Big endian fields in a byte array.
#define NUM_APPENDED_INFO_BYTES 8
#define APPENDED_INFO_FRAME_STATUS_BYTE_1_INDEX 0 // MSByte
#define APPENDED_INFO_FRAME_STATUS_BYTE_0_INDEX 1 // LSByte
#define APPENDED_INFO_RSSI_BYTE_INDEX           2
#define APPENDED_INFO_LQI_BYTE_INDEX            3
#define APPENDED_INFO_GAIN_BYTE_INDEX           4
#define APPENDED_INFO_MAC_TIMER_BYTE_2_INDEX    5 // MSByte
#define APPENDED_INFO_MAC_TIMER_BYTE_1_INDEX    6
#define APPENDED_INFO_MAC_TIMER_BYTE_0_INDEX    7 // LSByte
// After the frame status word has been examined, the RX ISR partially
// overwrites it with the channel that the packet was received on.
// This stack uses the channel during active scans.
#define APPENDED_INFO_CHANNEL_OVERWRITE_BYTE_INDEX 0
// Multi-network: we overwrite the second byte of the frame status with the
// network index. This will make the stack aware at higher levels of the network
// on which the packet was received.
#define APPENDED_INFO_NETWORK_OVERWRITE_BYTE_INDEX 1
#define APPENDED_INFO_NETWORK_OVERWRITE_NET_ID_MASK 0x0F
#define APPENDED_INFO_NETWORK_OVERWRITE_NET_ID_SHIFT 0
#define APPENDED_INFO_NETWORK_OVERWRITE_PHY_ID_MASK 0xF0
#define APPENDED_INFO_NETWORK_OVERWRITE_PHY_ID_SHIFT 4

#define NUM_APPENDED_INFO_WORDS (NUM_APPENDED_INFO_BYTES>>1)
#define APPENDED_INFO_FRAME_STATUS_WORD_INDEX    0
#define APPENDED_INFO_LQI_RSSI_WORD_INDEX        1
#define APPENDED_INFO_GAIN_UPPER_TIME_WORD_INDEX 2
#define APPENDED_INFO_LOWER_TIME_WORD_INDEX      3

// Appended Info Frame Status bits
#define FRAME_STATUS_RX_ACK          BIT(15)
#define FRAME_STATUS_CRC_PASS        BIT(14)
#define FRAME_STATUS_FILTER_COMPLETE BIT(13)
#define FRAME_STATUS_ERROR_RX_CORR   BIT(12)
#define FRAME_STATUS_BB_LENGTH_ERROR BIT(11)
#define FRAME_STATUS_TX_COLL_RX      BIT(10)
#define FRAME_STATUS_ILLEGAL_FORMAT  BIT(9)
#define FRAME_STATUS_ILLEGAL_LENGTH  BIT(8)
#define FRAME_STATUS_RX_MIN_LENGTH   BIT(7)
#define FRAME_STATUS_WRONG_FORMAT    BIT(6)
#define FRAME_STATUS_UNEXPECTED_ACK  BIT(5)
#define FRAME_STATUS_DST_PAN_ID      BIT(4)
#define FRAME_STATUS_DST_SHORT_ADDR  BIT(3)
#define FRAME_STATUS_DST_EXT_ADDR    BIT(2)
#define FRAME_STATUS_SRC_SHORT_ADDR  BIT(1)
#define FRAME_STATUS_WRONG_ADDR      BIT(0)

#endif // __PHY_APPENDED_INFO__
