/**
 * File: hal/micro/unix/host/micro.h
 * Description: Interface definitions for HAL micro unix host.
 *
 * Copyright 2008 by Ember Corporation. All rights reserved.
 */

#ifndef __UNIX_MICRO_H__
#define __UNIX_MICRO_H__

#include "hal/micro/generic/em2xx-reset-defs.h"

#ifndef __MICRO_H__
#error do not include this file directly - include micro/micro.h
#endif

#define EM_NUM_SERIAL_PORTS 2

// These are defined to satisfy code that is compiled on multiple platforms.
// They are not used by the MICRO code specific to the Unix host.
#define RESET_UNKNOWN             0   
#define RESET_EXTERNAL            1   
#define RESET_POWERON             2  
#define RESET_WATCHDOG            3   
#define RESET_BROWNOUT            4   
#define RESET_JTAG                5   
#define RESET_ASSERT              6   
#define RESET_RSTACK              7   
#define RESET_CSTACK              8  
#define RESET_BOOTLOADER          9   
#define RESET_PC_ROLLOVER         10  
#define RESET_SOFTWARE            11  
#define RESET_PROTFAULT           12  
#define RESET_FLASH_VERIFY_FAIL   13  
#define RESET_FLASH_WRITE_INHIBIT 14  
#define RESET_BOOTLOADER_IMG_BAD  15  

void setMicroRebootHandler(void (*handler)(void));


// the number of ticks (as returned from halCommonGetInt32uMillisecondTick)
// that represent an actual second. This can vary on different platforms.
#define MILLISECOND_TICKS_PER_SECOND 1000UL

#endif //__UNIX_MICRO_H__
