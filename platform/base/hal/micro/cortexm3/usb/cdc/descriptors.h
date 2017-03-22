/***************************************************************************//**
 * @file descriptors.h
 * @brief USB descriptor prototypes for CDC serial example project.
 * @version 3.20.5
 *******************************************************************************
 * @section License
 * <b>(C) Copyright 2014 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/
#ifndef DESCRIPTORS_H
#define DESCRIPTORS_H

#include "hal/micro/cortexm3/usb/em_usb.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
  uint8_t  len;
  uint8_t  type;
  uint16_t name[17];
} __attribute__ ((packed)) tUSBCDC_iSerialNumber;

extern const USBD_Init_TypeDef  USBCDC_initStruct;
extern tUSBCDC_iSerialNumber    USBCDC_iSerialNumber;
int  USBCDC_SetupCmd(const USB_Setup_TypeDef *setup);
void USBCDC_stateChange(USBD_State_TypeDef oldState, USBD_State_TypeDef newState);

#ifdef __cplusplus
}
#endif

#endif /* DESCRIPTORS_H */
