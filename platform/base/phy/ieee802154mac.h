#ifndef __IEEE802154MAC_H__
#define __IEEE802154MAC_H__


//------------------------------------------------------------------
// 15.4 Frame Control Register definitions

// For use in debugging (it's hard to match up the BIT() definitions with
// hexidecimal output):
// Remember that 15.4 sends low byte first.
//
// MAC_FRAME_TYPE_BEACON            0x0000
// MAC_FRAME_TYPE_DATA              0x0001
// MAC_FRAME_TYPE_ACK               0x0002
// MAC_FRAME_TYPE_CONTROL           0x0003
// MAC_FRAME_TYPE_MASK              0x0007
//
// MAC_FRAME_FLAG_SECURITY_ENABLED  0x0008
// MAC_FRAME_FLAG_FRAME_PENDING     0x0010
// MAC_FRAME_FLAG_ACK_REQUIRED      0x0020
// MAC_FRAME_FLAG_INTRA_PAN         0x0040
//
// MAC_FRAME_DESTINATION_MODE_NONE  0x0000
// MAC_FRAME_DESTINATION_MODE_SHORT 0x0800
// MAC_FRAME_DESTINATION_MODE_LONG  0x0C00
// MAC_FRAME_DESTINATION_MODE_MASK  0x0C00
//
// MAC_FRAME_SOURCE_MODE_NONE       0x0000
// MAC_FRAME_SOURCE_MODE_SHORT      0x8000
// MAC_FRAME_SOURCE_MODE_LONG       0xC000
// MAC_FRAME_SOURCE_MODE_MASK       0xC000

#define MAC_FRAME_TYPE_BEACON               0
#define MAC_FRAME_TYPE_DATA                 BIT(0)
#define MAC_FRAME_TYPE_ACK                  BIT(1)
#define MAC_FRAME_TYPE_CONTROL              (BIT(0)|BIT(1))
#define MAC_FRAME_TYPE_MASK                 (BIT(0)|BIT(1)|BIT(2))
#define MAC_FRAME_TYPE_RESERVED_MASK        BIT(2)

#define MAC_FRAME_FLAG_SECURITY_ENABLED     BIT(3)
#define MAC_FRAME_FLAG_FRAME_PENDING        BIT(4)
#define MAC_FRAME_FLAG_ACK_REQUIRED         BIT(5)
#define MAC_FRAME_FLAG_INTRA_PAN            BIT(6)
// Bit 7 is reserved
#define MAC_FRAME_FLAG_RESERVED             BIT(7)
// 802.15.4e-2012: Bit 8 is sequence number suppression
#define MAC_FRAME_FLAG_SEQ_SUPPRESSION      ((uint16_t)(BIT(8)))
// 802.15.4e-2012: Bit 9 is presence of information elements
#define MAC_FRAME_FLAG_IE_LIST_PRESENT      BIT(9)

#define MAC_FRAME_DESTINATION_MODE_NONE     0
#define MAC_FRAME_DESTINATION_MODE_RESERVED ((uint16_t)BIT(10))
#define MAC_FRAME_DESTINATION_MODE_SHORT    ((uint16_t)BIT(11))
#define MAC_FRAME_DESTINATION_MODE_LONG     ((uint16_t)(BIT(10)|BIT(11)))
#define MAC_FRAME_DESTINATION_MODE_MASK     ((uint16_t)(BIT(10)|BIT(11)))
// 802.15.4e-2012
#define MAC_FRAME_DESTINATION_MODE_BYTE     MAC_FRAME_DESTINATION_MODE_RESERVED

#define MAC_FRAME_VERSION_MASK              ((uint16_t)(BIT(12) | BIT(13)))

#define MAC_FRAME_SOURCE_MODE_NONE          0
#define MAC_FRAME_SOURCE_MODE_RESERVED      ((uint16_t)BIT(14))
#define MAC_FRAME_SOURCE_MODE_SHORT         ((uint16_t)BIT(15))
#define MAC_FRAME_SOURCE_MODE_LONG          ((uint16_t)(BIT(14)|BIT(15)))
#define MAC_FRAME_SOURCE_MODE_MASK          ((uint16_t)(BIT(14)|BIT(15)))
// 802.15.4e-2012
#define MAC_FRAME_SOURCE_MODE_BYTE          MAC_FRAME_SOURCE_MODE_RESERVED

#define MAC_FRAME_VERSION_2003             ((uint16_t)(0))
#define MAC_FRAME_VERSION_2006             ((uint16_t)(BIT(12)))
// For some reason in 802.15.4 this version is just called "IEEE STD 802.15.4",
// which is very confusing.
#define MAC_FRAME_VERSION_2012             ((uint16_t)(BIT(13)))
#define MAC_FRAME_VERSION_RESERVED         ((uint16_t)(BIT(12)|BIT(13)))

//------------------------------------------------------------------------
// Information Elements fields

// There are Header IEs and Payload IEs.  Header IEs are authenticated
// if MAC Security is enabled.  Payload IEs are both authenticated and
// encrypted if MAC security is enabled.

// Header and Payload IEs have slightly different formats and different
// contents based on the 802.15.4 spec.

// Both are actually a list of IEs that continues until a termination
// IE is seen.

#define MAC_FRAME_HEADER_INFO_ELEMENT_LENGTH_MASK 0x007F // bits 0-6
#define MAC_FRAME_HEADER_INFO_ELEMENT_ID_MASK     0x7F80 // bits 7-14
#define MAC_FRAME_HEADER_INFO_ELEMENT_TYPE_MASK   0x8000 // bit  15

#define MAC_FRAME_HEADER_INFO_ELEMENT_ID_SHIFT 7

#define MAC_FRAME_PAYLOAD_INFO_ELEMENT_LENGTH_MASK   0x07FF  // bits 0 -10
#define MAC_FRAME_PAYLOAD_INFO_ELEMENT_GROUP_ID_MASK 0x7800  // bits 11-14
#define MAC_FRAME_PAYLOAD_INFO_ELEMENT_TYPE_MASK     0x8000  // bit  15

#define MAC_FRAME_PAYLOAD_INFO_ELEMENT_ID_SHIFT      11

// This "type" field indicates header vs. payload IE.  However there is 
// also a Header IE List terminator which would imply the IE list
// that follows is only payload IEs.  
#define MAC_FRAME_INFO_ELEMENT_TYPE_MASK             0x8000

// Header Termination ID 1 is used when there are Payload IEs that follow.
// Header Termination ID 2 is used when there are no Payload IEs and the
//   next field is the MAC payload.
#define MAC_FRAME_HEADER_TERMINATION_ID_1 0x7E
#define MAC_FRAME_HEADER_TERMINATION_ID_2 0x7F
#define MAC_FRAME_PAYLOAD_TERMINATION_ID 0x0F


#endif //__IEEE802154MAC_H__
