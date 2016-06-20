#ifndef _LWIP_ARCH_CC_H_
#define _LWIP_ARCH_CC_H_

#include "em_device.h"

#include <limits.h>
#include <string.h>
#include <errno.h>
#ifndef ETH_USE_TINY_PRINTF
#include <stdio.h>
#else
#include "printf.h"
#endif
#include <stdint.h>
#include <stdlib.h>
#include "em_device.h"
#undef EDOM
#undef EILSEQ
#undef ENOENT
#undef ERANGE

/** Associating the basic types of lwIP with the compiler */
typedef uint8_t           u8_t;
typedef int8_t            s8_t;
typedef uint16_t          u16_t;
typedef int16_t           s16_t;
typedef uint32_t          u32_t;
typedef int32_t           s32_t;

/** Generic pointer type of 32 bits */
typedef u32_t            mem_ptr_t;

/** Associating the formaters for data types */
#define U16_F    "u"
#define S16_F    "d"
#define X16_F    "x"
#define U32_F    "lu"
#define S32_F    "ld"
#define X32_F    "lx"

/** Cortex M3 MCU is little endian */
#undef BYTE_ORDER
#define BYTE_ORDER                LITTLE_ENDIAN

/** Associating the byte-swapping methods */
#define LWIP_PLATFORM_BYTESWAP    1
#define LWIP_PLATFORM_HTONS(x)    ((uint16_t)__REV16(x))
#define LWIP_PLATFORM_HTONL(x)    (__REV(x))

/** Associating the checksum algorithm using Ethernet 802.3 standard
 * lwIP gives 3 algorithms
 * 1 load byte by byte,
 *   construct 16 bits word,
 *   add last byte
 * 2 load first byte if odd address,
 *   loop processing 16 bits words,
 *   add last byte.
 * 3 load first byte and word if not 4 byte aligned,
 *   loop processing 32 bits words,
 *   add last word/byte.
 */
#define LWIP_CHKSUM_ALGORITHM    3

/** Defining the packing structure
 *   due to the KSZ8851SNL setup we don't need to pack data
 *   TX padding is enabled
 */
#define PACK_STRUCT_FIELD(x)    x
#define PACK_STRUCT_STRUCT
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_END

#define nLWIP_NOASSERT

/** Define local assert methods */
#ifndef LWIP_NOASSERT
#define LWIP_PLATFORM_ASSERT(x)    do { printf("Assertion \"%s\" failed at line %d in %s\n", x, __LINE__, __FILE__); } while (0)
#else
#define LWIP_PLATFORM_ASSERT(x)
#endif

/** diagnostic output */
#ifdef LWIP_DEBUG
#define LWIP_PLATFORM_DIAG(x)      do { printf x; } while (0)
#else
#define LWIP_PLATFORM_DIAG(x)
#endif

/** Define local rand method */
#define LWIP_RAND    rand

#endif /* _LWIP_ARCH_CC_H_ */
