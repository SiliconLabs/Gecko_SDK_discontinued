/***************************************************************************//**
 * @file
 * @brief USB protocol stack library, internal type definitions.
 * @author Nathaniel Ting
 * @version 3.20.3
 * <!-- Copyright 2013 by Silicon Laboratories. All rights reserved.     *80*-->
 *****************************************************************************/
#ifndef __EM_USBTYPES_H
#define __EM_USBTYPES_H

// #include "em_device.h"
// #include "em_usb.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/* Limits imposed by the USB peripheral */
#define NP_RX_QUE_DEPTH       8
#define HP_RX_QUE_DEPTH       8
// #define MAX_XFER_LEN          524287L         /* 2^19 - 1 bytes             */
// #define MAX_PACKETS_PR_XFER   1023            /* 2^10 - 1 packets           */
#define MAX_NUM_TX_FIFOS      6               /* In addition to EP0 Tx FIFO */
#define MAX_NUM_IN_EPS        6               /* In addition to EP0         */
#define MAX_NUM_OUT_EPS       6               /* In addition to EP0         */

/* Limit imposed by the USB standard */
#define MAX_USB_EP_NUM      12




/* Macros for selecting a hardware timer. */
#define USB_TIMER0 0
#define USB_TIMER1 1

// #ifndef NUM_EP_USED
// #define NUM_EP_USED 0
// #endif


typedef enum
{
  D_EP_IDLE          = 0,
  D_EP_TRANSMITTING  = 1,
  D_EP_RECEIVING     = 2,
  D_EP_STATUS        = 3
} USBD_EpState_TypeDef;

typedef struct
{
  bool                        in;
  uint8_t                     zlp;
  uint8_t                     num;
  uint8_t                     addr;
  uint8_t                     type;
  uint8_t                     txFifoNum;
  uint8_t                     *buf;
  uint16_t                    packetSize;
  uint16_t                    mask;
  uint32_t                    remaining;
  uint32_t                    xferred;
  uint32_t                    hwXferSize;
  uint32_t                    fifoSize;
  USBD_EpState_TypeDef        state;
  USB_XferCompleteCb_TypeDef  xferCompleteCb;
} USBD_Ep_TypeDef;

typedef struct
{
  USB_Setup_TypeDef                     *setup;
  USB_Setup_TypeDef                     setupPkt[3];
  uint8_t                               configurationValue; /* Must be DWORD aligned */
  bool                                  remoteWakeupEnabled;
  uint8_t                               numberOfStrings;
  USBD_State_TypeDef                    state;
  USBD_State_TypeDef                    savedState;
  USBD_State_TypeDef                    lastState;
  const USB_DeviceDescriptor_TypeDef    *deviceDescriptor;
  const USB_ConfigurationDescriptor_TypeDef *configDescriptor;
  const void * const                    *stringDescriptors;
  const USBD_Callbacks_TypeDef          *callbacks;
  USBD_Ep_TypeDef                       ep[ NUM_EP_USED + 1 ];
  uint8_t                               inEpAddr2EpIndex[  MAX_USB_EP_NUM + 1 ];
  uint8_t                               outEpAddr2EpIndex[ MAX_USB_EP_NUM + 1 ];
} USBD_Device_TypeDef;


/** @endcond */

#ifdef __cplusplus
}
#endif

#endif /* __EM_USBTYPES_H */
