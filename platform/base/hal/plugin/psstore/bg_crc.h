#ifndef BG_CRC_H
#define BG_CRC_H
#include <stdint.h>

uint32_t bg_crc_calc(uint32_t initial,int len, const uint8_t*data);

#endif
