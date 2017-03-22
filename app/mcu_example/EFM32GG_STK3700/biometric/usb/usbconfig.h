/**************************************************************************//**
 * @file usbconfig.h
 * @brief USB configuration.
 * @version 5.1.2
 ******************************************************************************
 * @section License
 * <b>Copyright 2015 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#ifndef USBCONFIG_H_
#define USBCONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

#define USB_DEVICE        /* Compile stack for device mode. */
#define USB_PWRSAVE_MODE (USB_PWRSAVE_MODE_ONSUSPEND | USB_PWRSAVE_MODE_ONVBUSOFF)


/****************************************************************************
**                                                                         **
** Specify total number of endpoints used (in addition to EP0).            **
**                                                                         **
*****************************************************************************/
#define NUM_EP_USED 3

/****************************************************************************
**                                                                         **
** Specify number of application timers you need.                          **
**                                                                         **
*****************************************************************************/
#define NUM_APP_TIMERS 2


/*** Misc. definitions for the interfaces of the composite device. ***/

/* Define interface numbers */
#define CDC_CTRL_INTERFACE_NO   ( 0 )
#define CDC_DATA_INTERFACE_NO   ( 1 )
#define NUM_INTERFACES          ( 2 )

#define CDC_NUM_EP_USED         ( 3 ) /* Number of EP's used by CDC function */

/* Define USB endpoint addresses for the interfaces */
#define CDC_EP_DATA_OUT   ( 0x01 ) /* Endpoint for CDC data reception.       */
#define CDC_EP_DATA_IN    ( 0x81 ) /* Endpoint for CDC data transmission.    */
#define CDC_EP_NOTIFY     ( 0x82 ) /* The notification endpoint (not used).  */


/* Define timer ID's */
#define CDC_TIMER_ID        ( 1 )

#ifdef __cplusplus
}
#endif

#endif /* USBCONFIG_H_ */

