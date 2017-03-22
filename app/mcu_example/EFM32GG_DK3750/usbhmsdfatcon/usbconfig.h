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

#ifndef __USBCONFIG_H
#define __USBCONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#define USB_HOST            /* Compile stack for host mode. */

/****************************************************************************
**                                                                         **
** Specify number of host channels used (in addition to EP0).              **
**                                                                         **
*****************************************************************************/
#define NUM_HC_USED 2       /* Not counting default control ep which  */
                            /* is assigned to host channels 0 and 1   */

/****************************************************************************
**                                                                         **
** Configure serial port debug output.                                     **
**                                                                         **
*****************************************************************************/
/* Define a function for transmitting a single char on the serial port. */
extern int RETARGET_WriteChar(char c);
#define USER_PUTCHAR  RETARGET_WriteChar

/* Debug USB API functions (illegal input parameters etc.)  */
#define DEBUG_USB_API              /* Uncomment to turn on  */

/*
 * Some utility functions in the API needs printf. These
 * functions have "print" in their name. This macro enables
 * these functions.
 */
#define USB_USE_PRINTF             /* Uncomment to enable   */

#ifdef __cplusplus
}
#endif

#endif /* __USBCONFIG_H */
