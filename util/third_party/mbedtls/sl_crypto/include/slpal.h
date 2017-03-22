/*
 *  Platform Abstraction Layer interface.
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

#ifndef MBEDTLS_SLPAL_H
#define MBEDTLS_SLPAL_H

#if !defined(MBEDTLS_CONFIG_FILE)
#include "config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined( MBEDTLS_FREERTOS )

#include "slpal_freertos.h"

#elif defined( MBEDTLS_UCOS3 )

#include "slpal_ucos3.h"

#else

/* Bare metal (no OS) implementation of SLPAL. */
#include "slpal_baremetal.h"

#endif

#endif /* MBEDTLS_SLPAL_H */
