/*
 * File: phy/pro2class/typedef.h
 * Description: Type definitions to map PRO2+ API to ember PHY
 *
 * Copyright 2013 Silicon Laboratories, Inc.                                *80*
 */

#ifndef __PHY_PRO2PLUS_TYPEDEF_H__
#define __PHY_PRO2PLUS_TYPEDEF_H__

#define  xdata
#define  code

typedef uint8_t  u8;
typedef uint16_t u16;
typedef int16_t i16;
typedef uint32_t u32;
typedef int32_t i32;

// N.B. This include file does not address endianness differences between
//      PRO2+ big-endian API and host environment if little-endian.

#endif//__PHY_PRO2PLUS_TYPEDEF_H__
