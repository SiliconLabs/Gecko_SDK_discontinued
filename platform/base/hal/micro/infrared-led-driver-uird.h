// Copyright 2014 Silicon Laboratories, Inc.

#ifndef __INFRARED_LED_DRIVER_UIRD_H__
#define __INFRARED_LED_DRIVER_UIRD_H__

#define HAL_INFRARED_LED_UIRD_DB_STRUCT_SIZE             ( 80)
#define HAL_INFRARED_LED_UIRD_ENCR_DB_STRUCT_SIZE        ( 52)

#define HAL_INFRARED_LED_UIRD_DB_ENTRIES                 ( 3)
#define HAL_INFRARED_LED_UIRD_ENCR_DB_ENTRIES            ( 3)


extern const uint8_t halInfraredLedUirdDatabase[ HAL_INFRARED_LED_UIRD_DB_ENTRIES][ HAL_INFRARED_LED_UIRD_DB_STRUCT_SIZE];
extern const uint8_t halInfraredLedUirdEncryptDatabase[ HAL_INFRARED_LED_UIRD_ENCR_DB_ENTRIES][ HAL_INFRARED_LED_UIRD_ENCR_DB_STRUCT_SIZE];


int  halInfraredLedUirdDecode(const uint8_t *irdPtr, uint8_t irdLen);

void halInfraredLedUirdSetDecryptKey( uint32_t decryptKey);
int  halInfraredLedUirdDecrypt(uint8_t dstPtr[], const uint8_t srcPtr[], int srcLen);


#endif // __INFRARED_LED_DRIVER_UIRD_H__
