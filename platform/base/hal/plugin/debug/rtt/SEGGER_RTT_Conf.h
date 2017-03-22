/*********************************************************************
*               SEGGER MICROCONTROLLER GmbH & Co. KG                 *
*       Solutions for real time microcontroller applications         *
**********************************************************************
*                                                                    *
*       (c) 2014 - 2016  SEGGER Microcontroller GmbH & Co. KG        *
*                                                                    *
*       www.segger.com     Support: support@segger.com               *
*                                                                    *
**********************************************************************
----------------------------------------------------------------------
File    : SEGGER_RTT_Conf.h
Purpose : Implementation of SEGGER real-time transfer (RTT) which 
          allows real-time communication on targets which support 
          debugger memory accesses while the CPU is running.
---------------------------END-OF-HEADER------------------------------
*/

#ifndef SEGGER_RTT_CONF_H
#define SEGGER_RTT_CONF_H

#include PLATFORM_HEADER

/*********************************************************************
*
*       Defines, configurable by users
*
**********************************************************************
*/

// Buffers:
// 0: Terminal
// 1: <reserved>/SysView
// 2: SilabsVuUp/SilabsVuDown

#define SEGGER_RTT_MAX_NUM_UP_BUFFERS     (3)     // Max. number of up-buffers (T->H) available on this target    (Default: 3)
#define SEGGER_RTT_MAX_NUM_DOWN_BUFFERS   (3)     // Max. number of down-buffers (H->T) available on this target  (Default: 3)

// Settings for Channel 0/Terminal
#define BUFFER_SIZE_UP                    (16)    // Size of the buffer for terminal output of target, up to host (Default: 1k)
#define BUFFER_SIZE_DOWN                  (16)    // Size of the buffer for terminal input to target from host (Usually keyboard input) (Default: 16)
#define SEGGER_RTT_MODE_DEFAULT           SEGGER_RTT_MODE_NO_BLOCK_SKIP // Mode for pre-initialized terminal channel (buffer 0)

/*********************************************************************
*
*       RTT lock configuration
*/
#define SEGGER_RTT_LOCK()                 \
        CORE_DECLARE_IRQ_STATE;           \
        CORE_ENTER_ATOMIC()

#define SEGGER_RTT_UNLOCK()     CORE_ExitAtomic(irqState)

/*********************************************************************
*
*       RTT control block configuration
*/
#define SEGGER_RTT_ALIGNMENT              1024

#endif
/*************************** End of file ****************************/
