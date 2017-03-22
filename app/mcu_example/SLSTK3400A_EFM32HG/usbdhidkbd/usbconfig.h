/***************************************************************************//**
 * @file usbconfig.h
 * @brief USB protocol stack library, application supplied configuration options.
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

#ifndef __SILICON_LABS_USBCONFIG_H__
#define __SILICON_LABS_USBCONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif

#define USB_DEVICE        /* Compile stack for device mode. */

#define USB_CORECLK_HFRCO /* Use HFRCO as core clock, default is HFXO. */

#define USB_PWRSAVE_MODE ( USB_PWRSAVE_MODE_ONVBUSOFF       \
                           | USB_PWRSAVE_MODE_ONSUSPEND     \
                           | USB_PWRSAVE_MODE_ENTEREM2 )

/****************************************************************************
**                                                                         **
** Specify number of endpoints used (in addition to EP0).                  **
**                                                                         **
*****************************************************************************/
#define NUM_EP_USED 1

/****************************************************************************
**                                                                         **
** Specify number of application timers you need.                          **
**                                                                         **
*****************************************************************************/
#define NUM_APP_TIMERS 2

/****************************************************************************
**                                                                         **
** USB HID keyboard class device driver definitions.                       **
**                                                                         **
*****************************************************************************/
#define HIDKBD_INTERFACE_NO     0
#define HIDKBD_INTR_IN_EP_ADDR  0x81    /* Bit 7 must be set.                 */
#define HIDKBD_POLL_RATE        24      /* The bInterval reported with the    */
                                        /* interrupt IN endpoint descriptor.  */
#define HIDKBD_IDLE_TIMER       0       /* Timer used to implement the idle-  */
                                        /* rate defined in the HID class spec.*/

#ifdef __cplusplus
}
#endif

#endif /* __SILICON_LABS_USBCONFIG_H__ */
