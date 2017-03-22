/***************************************************************************//**
 * @file base-version.h
 * @brief Versioning of Silicon Labs base repo
 *******************************************************************************
 * @section License
 * <b>(C) Copyright 2017 Silicon Labs, http://www.silabs.com</b>
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
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Silicon Labs has no
 * obligation to support this Software. Silicon Labs is providing the
 * Software "AS IS", with no express or implied warranties of any kind,
 * including, but not limited to, any implied warranties of merchantability
 * or fitness for any particular purpose or warranties against infringement
 * of any proprietary rights of a third party.
 *
 * Silicon Labs will not be liable for any consequential, incidental, or
 * special damages, or any other relief, or for any claim by any third party,
 * arising from your use of this Software.
 *
 ******************************************************************************/

// The 4 digit version: A.B.C.D
#define SL_BASE_MAJOR_VERSION     6
#define SL_BASE_MINOR_VERSION     1
#define SL_BASE_PATCH_VERSION     5
#define SL_BASE_SPECIAL_VERSION   0

// Version represented in 2 bytes as hexadecimal <Major><minor><patch><special>
#define SL_BASE_BUILD_NUMBER      0
#define SL_BASE_FULL_VERSION      (  (SL_BASE_MAJOR_VERSION << 12) \
                                   | (SL_BASE_MINOR_VERSION <<  8) \
                                   | (SL_BASE_PATCH_VERSION <<  4) \
                                   | (SL_BASE_SPECIAL_VERSION    ) )

//#define SL_BASE_VERSION_TYPE  SL_VERSION_TYPE_GA
