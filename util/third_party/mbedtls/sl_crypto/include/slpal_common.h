/*
 *  Common SLPAL definitions for all platforms.
 *
 *  Copyright (C) 2016, Silicon Labs, http://www.silabs.com
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifndef MBEDTLS_SLPAL_COMMON_H
#define MBEDTLS_SLPAL_COMMON_H

#include "mbedtls_ecode.h"

/*******************************************************************************
 ******************************* ERROR CODES  **********************************
 ******************************************************************************/

#define SLPAL_ERROR_TIMEOUT         ((int)MBEDTLS_ERR_SLPAL_BASE | 0x1)
#define SLPAL_ERROR_OS_SPECIFIC     ((int)MBEDTLS_ERR_SLPAL_BASE | 0x2)

/*******************************************************************************
 ******************************   Defines     **********************************
 ******************************************************************************/

/* Determine if executing at interrupt level on ARM Cortex-M. */
#define RUNNING_AT_INTERRUPT_LEVEL (SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk)

/*******************************************************************************
 *************************   TYPEDEFS   ****************************************
 ******************************************************************************/

/** Storage for PRIMASK or BASEPRI value used for SLPAL critical regions.
    SLPAL_CriticalEnter returns the current state, which is subsequently passed
    to SLPAL_CriticalExit in order to restore the state. */
typedef uint32_t SLPAL_irqState_t;

#endif /* MBEDTLS_SLPAL_COMMON_H */
