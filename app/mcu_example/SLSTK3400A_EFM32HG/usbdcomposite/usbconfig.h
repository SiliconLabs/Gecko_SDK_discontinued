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

/****************************************************************************
**                                                                         **
** Specify total number of endpoints used (in addition to EP0).            **
**                                                                         **
*****************************************************************************/
#define NUM_EP_USED 4

/****************************************************************************
**                                                                         **
** Specify number of application timers you need.                          **
**                                                                         **
*****************************************************************************/
#define NUM_APP_TIMERS 3

/****************************************************************************
**                                                                         **
** Misc. definitions for the interfaces of the composite device.           **
**                                                                         **
*****************************************************************************/

/* Define interface numbers */
#define VUD_INTERFACE_NO        ( 0 )
#define HIDKBD_INTERFACE_NO     ( 1 )
#define CDC_CTRL_INTERFACE_NO   ( 2 )
#define CDC_DATA_INTERFACE_NO   ( 3 )
#define NUM_INTERFACES          ( 4 )

#define HID_NUM_EP_USED         ( 1 ) /* Number of EP's used by HID function */
#define VUD_NUM_EP_USED         ( 0 ) /* Number of EP's used by VUD function */
#define CDC_NUM_EP_USED         ( 3 ) /* Number of EP's used by CDC function */

/* Define USB endpoint addresses for the interfaces */
#define CDC_EP_DATA_OUT   ( 0x01 ) /* Endpoint for CDC data reception.       */
#define CDC_EP_DATA_IN    ( 0x81 ) /* Endpoint for CDC data transmission.    */
#define CDC_EP_NOTIFY     ( 0x82 ) /* The notification endpoint (not used).  */

#define HIDKBD_INTR_IN_EP_ADDR ( 0x83 ) /* Endpoint for HID data transmission.*/

/* Define timer ID's */
#define CDC_TIMER_ID         ( 1 )
#define HIDKBD_IDLE_TIMER    ( 0 )     /* Timer used to implement the idle-  */
                                       /* rate defined in the HID class spec.*/
#define HIDKBD_SCAN_TIMER_ID ( 2 )
#define HIDKBD_POLL_RATE       24      /* The bInterval reported with the    */
                                       /* interrupt IN endpoint descriptor.  */

/* Configuration options for the CDC driver. */
#define CDC_UART_TX_DMA_CHANNEL   ( 0 )
#define CDC_UART_RX_DMA_CHANNEL   ( 1 )
#define CDC_TX_DMA_SIGNAL         DMAREQ_USART1_TXBL
#define CDC_RX_DMA_SIGNAL         DMAREQ_USART1_RXDATAV
#define CDC_UART                  USART1
#define CDC_UART_CLOCK            cmuClock_USART1
#define CDC_UART_ROUTE            ( USART_ROUTE_RXPEN | \
                                    USART_ROUTE_TXPEN | \
                                    USART_ROUTE_LOCATION_LOC0 )
#define CDC_UART_TX_PORT          gpioPortC
#define CDC_UART_TX_PIN           0
#define CDC_UART_RX_PORT          gpioPortC
#define CDC_UART_RX_PIN           1
#define CDC_ENABLE_DK_UART_SWITCH()     /* Empty define, this is not a DK. */

#ifdef __cplusplus
}
#endif

#endif /* __SILICON_LABS_USBCONFIG_H__ */
