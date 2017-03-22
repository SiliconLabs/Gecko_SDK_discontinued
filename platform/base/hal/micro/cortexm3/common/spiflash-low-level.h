// File: spiflash-class1-common.h
// 
// Description: This header file contains the prototypes for functions
//				needed in all spiflash-class1.c files in order for
//				them to work with spiflash-common.c
// 
// Copyright 2015 Silicon Laboratories, Inc.                                *80*

#ifndef __SPIFLASH_CLASS1_COMMON_H__
#define __SPIFLASH_CLASS1_COMMON_H__

void halSpiPush8(uint8_t txData);

uint8_t halSpiPop8(void);

void setFlashCSActive(void);

void setFlashCSInactive(void);

uint8_t halEepromInit(void);

void configureEepromPowerPin(void);

void setEepromPowerPin(void);

void clearEepromPowerPin(void);

void halEepromConfigureGPIO(void);

void halEepromConfigureFlashController(void);

void halEepromDelayMicroseconds(uint32_t timeToDelay);

#endif // __SPIFLASH_CLASS1_COMMON_H__
