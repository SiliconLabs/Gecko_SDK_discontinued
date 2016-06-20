/**************************************************************************//**
 * @file usb_debug.h
 * @brief USB device setup for HRM debugging.
 * @version 4.1.0
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2014 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/
#include "si114xHRM.h"
#include "em_usb.h"

#ifndef USBDEBUG_H_
#define USBDEBUG_H_

#ifdef __cplusplus
extern "C" {
#endif

#define CDC_BULK_EP_SIZE  USB_FS_BULK_EP_MAXSIZE/* This is the max. ep size. */

void USBDebug_Init(void);
int USBDebug_SetupCmd(const USB_Setup_TypeDef *setup);
void USBDebug_StateChangeEvent( USBD_State_TypeDef oldState, USBD_State_TypeDef newState );
bool USBDebug_IsUSBConfigured(void);
void USBDebug_ProcessUSBOutput(int8_t *data_buffer);
void USBDebug_ProcessConfigurationMessage(char *message, Si114xhrmConfiguration_t *configuration);

#ifdef __cplusplus
}
#endif

#endif /* USBDEBUG_H_ */
