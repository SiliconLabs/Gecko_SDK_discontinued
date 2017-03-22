// Copyright 2014 Silicon Laboratories, Inc.

#ifndef __INFRARED_LED_DRIVER_SIRD_H__
#define __INFRARED_LED_DRIVER_SIRD_H__


#define HAL_INFRARED_LED_SIRD_DB_ENTRIES   ( 1)

typedef struct
{
  const uint8_t *pDat;
  const uint8_t uLen;
} HalInfraredLedSirdDb_t;

extern const HalInfraredLedSirdDb_t   halInfraredLedSirdDb[HAL_INFRARED_LED_SIRD_DB_ENTRIES];

int halInfraredLedsirdDecode( const uint8_t *irdPtr, uint8_t irdLen);


#endif // __INFRARED_LED_DRIVER_SIRD_H__
