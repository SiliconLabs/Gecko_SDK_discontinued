/** @file hal/micro/cortexm3/efm32/board/brd4503b.h
 * See @ref board for detailed documentation.
 *
 * <!-- Copyright 2013 Silicon Laboratories, Inc.                        *80*-->
 */

#ifndef __BRD4503B_H__
#define __BRD4503B_H__

/* Compatibility mapping to similar radio board. Frequency, matching is the same. 
	Micro can be different though. */
#define RADIO_BOARD BRD4503A
#define EZR32_RADIO_XO_FREQ 30000000

// Include WSTK board header
#include "hal/micro/cortexm3/efm32/board/ezr32.h"

#endif /* __BRD4503B_H__ */
