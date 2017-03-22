/***************************************************************************//**
 * @file usbconfig.h
 * @brief USB protocol stack library, application supplied configuration options.
 * @author Energy Micro AS
 * @version 1.00
 *******************************************************************************
 * @section License
 * <b>(C) Copyright 2012 Energy Micro AS, http://www.energymicro.com</b>
 *******************************************************************************
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 * 4. The source and compiled code may only be used on Energy Micro "EFM32"
 *    microcontrollers and "EFR4" radios.
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Energy Micro AS has no
 * obligation to support this Software. Energy Micro AS is providing the
 * Software "AS IS", with no express or implied warranties of any kind,
 * including, but not limited to, any implied warranties of merchantability
 * or fitness for any particular purpose or warranties against infringement
 * of any proprietary rights of a third party.
 *
 * Energy Micro AS will not be liable for any consequential, incidental, or
 * special damages, or any other relief, or for any claim by any third party,
 * arising from your use of this Software.
 *
 *****************************************************************************/
#ifndef __USBCONFIG_H
#define __USBCONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

  
/* Compile stack for device mode. */
// #define USB_DEVICE        
  
/* Specify number of endpoints used (in addition to EP0) */
#ifndef NUM_EP_USED
  #if EM_SERIAL3_ENABLED
    #define NUM_EP_USED 3
  #else
    #define NUM_EP_USED 2
  #endif
#endif

/* Select TIMER0 to be used by the USB stack. This timer
 * must not be used by the application. */
// #define USB_TIMER USB_TIMER0   
  
  
  
/********************************************************** 
 * Define the endpoint addresses. Bits [3:0] define
 * endpoint number. Bit 7 defines direction (1 = IN). 
 **********************************************************/
  
/* Endpoint for USB data IN  (device to host).    */  
#define EP_IN             0x81  
  
/* Endpoint for USB data OUT (host to device).    */
#define EP_OUT            0x01  
  

  
/********************************************************** 
 * Debug Configuration. Enable the stack to output
 * debug messages to a console. This example is
 * configured to output messages over UART.
 **********************************************************/

/* Enable debug output from the stack */
// #define DEBUG_USB_API

/* Enable printf calls in stack */
// #define USB_USE_PRINTF   

/* Function declaration for the low-level printing of a 
 * character. This function must be implemented by the 
 * application. */
// int RETARGET_WriteChar(char c);
// #define USER_PUTCHAR  RETARGET_WriteChar      



/********************************************************** 
 * Power saving configuration. Select low frequency 
 * clock and power saving mode.
 **********************************************************/

/* Select the clock used when USB is in low power mode */
// #define USB_USBC_32kHz_CLK   USB_USBC_32kHz_CLK_LFXO
 
/* Select the power saving mode. Enter power save on Suspend and
 * when losing power on VBUS. Let the application handle when to
 * enter EM2 */
// #define USB_PWRSAVE_MODE (USB_PWRSAVE_MODE_ONSUSPEND | USB_PWRSAVE_MODE_ONVBUSOFF)



  

#ifdef __cplusplus
}
#endif

#endif /* __USBCONFIG_H */
