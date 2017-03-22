/***************************************************************************//**
 * @file bootloader-ccm.h
 * @brief This file contains the definitions to various CCM encryption functions
 * used in the secure bootloader.
 * @version 3.20.2
 *******************************************************************************
 * @section License
 * <b>(C) Copyright 2015 Silicon Labs, http://www.silabs.com</b>
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
#ifndef BOOTLOADER_CCM_H
#define BOOTLOADER_CCM_H

// Length defines
#define NONCE_LENGTH (12)
#define COUNTER_LENGTH (3)
#define MAC_LENGTH (16)
#define SECURITY_BLOCK_SIZE 16
#define PAYLOAD_SIZE_BYTES 3   // Number of bytes used to hold the payload size

// Offsets of various bits of CCM data after formatting
#define CCM_FLAGS_INDEX (0)
#define CCM_NONCE_INDEX (1)
#define CCM_COUNTER_INDEX (CCM_NONCE_INDEX + NONCE_LENGTH)

// The CCM state struct holds all configuration information needed to
// decrypt and validate a CCM message.
typedef struct ccmState_s
{
  uint32_t msgLen;                     /* Length of the encrypted data */
  uint8_t nonce[NONCE_LENGTH];         /* The random nonce for this message */
  uint8_t mac[SECURITY_BLOCK_SIZE];    /* The full rolling MAC value */
  uint32_t blockCount;                 /* Current AES block we're processing in this message */
  uint8_t blockOffset;                 /* Offset within the current AES block [0, SECURITY_BLOCK_SIZE] */
  uint8_t macOffset;                   /* Last byte written to in the MAC buffer */
} ccmState_t;

// Initialize the CCM state structure
void initCcmState(ccmState_t *ccmState);

// Verify that the secure bootloader key is set and accessbile
bool validateSecureBootloaderKey(void);

// Functions for creating the MAC
void initCcmMac(ccmState_t *ccmState, uint32_t aDataLen);
void updateCcmMac(ccmState_t *ccmState, uint8_t *data, uint32_t len);
void finalizeCcmMac(ccmState_t *ccmState);
bool verifyCcmMac(ccmState_t *ccmState, uint8_t *mac, uint8_t macLen);
void encryptDecryptCcmMac(ccmState_t *ccmState, uint8_t *mac);

// Define functions for authenticating unencrypted data. In CCM these
// are really just updating the MAC and finalizing, but use these names
// to make porting easier in the future 
#define processAssociatedData updateCcmMac
#define finalizeAssociatedData finalizeCcmMac

// Counter mode decrypt functions
void decryptCcmBlock(ccmState_t *ccmState, uint8_t *data, uint32_t len);

#endif // BOOTLOADER_CCM_H

