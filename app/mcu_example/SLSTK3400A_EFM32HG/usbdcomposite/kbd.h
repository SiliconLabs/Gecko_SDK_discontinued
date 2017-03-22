/***************************************************************************//**
 * @file  vud.h
 * @brief USB HID keyboard API.
 * @version 5.1.2
 *******************************************************************************
 * @section License
 * <b>Copyright 2015 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#ifndef __SILICON_LABS_KBD_H__
#define __SILICON_LABS_KBD_H__

#ifdef __cplusplus
extern "C" {
#endif

void KBD_Init( void );
int  KBD_SetupCmd( const USB_Setup_TypeDef *setup );
void KBD_StateChangeEvent( USBD_State_TypeDef oldState,
                           USBD_State_TypeDef newState );

#ifdef __cplusplus
}
#endif

#endif /* __SILICON_LABS_KBD_H__ */
