/***************************************************************************//**
 * @file ebl.h
 * @brief Common ebl processing code
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
#ifndef __EBL_H__
#define __EBL_H__

#include "bootloader-common.h"  // for BL_Status
#include "bootloader-ccm.h"

/////////////////////
// Definitions related to the EBL file format

#define IMAGE_SIGNATURE 0xE350

#define EBL_MAX_TAG_SIZE         (2048+32)
#define EBL_MIN_TAG_SIZE         (128)

#define EBLTAG_HEADER           0x0000
#define EBLTAG_METADATA         0xF608
#define EBLTAG_PROG             0xFE01
#define EBLTAG_MFGPROG          0x02FE
#define EBLTAG_ERASEPROG        0xFD03
#define EBLTAG_END              0xFC04
// Tags for encrypted EBL files
#define EBLTAG_ENC_HEADER       0xFB05
#define EBLTAG_ENC_INIT         0xFA06
#define EBLTAG_ENC_EBL_DATA     0xF907  // Encrypted EBL data
#define EBLTAG_ENC_MAC          0xF709

typedef struct {
	uint16_t tagId;
  uint16_t structSize;
  uint16_t minSerializedLength;
  bool canBeBigger;
	const char* tagName;
} EblTagInfo;

// Definitions to control the encrypted EBL state machine
#define EBL_STATE_ENC_MASK              (0x80)
#define EBL_STATE_ENC_START             (EBL_STATE_ENC_MASK | 0)
#define EBL_STATE_ENC_INIT              (EBL_STATE_ENC_MASK | 1)
#define EBL_STATE_ENC_RCVDATA           (EBL_STATE_ENC_MASK | 2)
#define EBL_STATE_ENC_RCVDATA_RESUME    (EBL_STATE_ENC_MASK | 3)
#define EBL_STATE_ENC_MAC_DONE          (EBL_STATE_ENC_MASK | 4)
#define EBL_STATE_ENC_DONE              (EBL_STATE_ENC_MASK | 5)
#define EBL_STATE_ENC_INVALID           (EBL_STATE_ENC_MASK | 0xF)

// Current version of the ebl format.   The minor version (LSB) should be
//  incremented for any changes that are backwards-compatible.  The major
//  version (MSB) should be incremented for any changes that break 
//  backwards compatibility. The major version is verified by the ebl 
//  processing code of the bootloader.
#define EBL_VERSION              (0x0202)
#define EBL_MAJOR_VERSION        (0x0200)
#define EBL_MAJOR_VERSION_MASK   (0xFF00)

#define EBL_MIN_METADATA_VERSION (0x0202)

// Current version of the IBR format.   The minor version (LSB) should be
//  incremented for any changes that are backwards-compatible.  The major
//  version (MSB) should be incremented for any changes that break 
//  backwards compatibility. The major version is verified by the ibr 
//  processing code of the bootloader.
#define IBR_VERSION              (0x0000)
#define IBR_MAJOR_VERSION        (0x0000)
#define IBR_MAJOR_VERSION_MASK   (0xFF00)

#define IBR_HEADER "SLABIBR "
#define IBR_SIZE 32 // 32 bytes

typedef struct
{
  uint8_t header[8];       // offset 0
  uint16_t majVer;         // offset 8
  uint16_t minVer;         // offset 10
  uint8_t reserved[12];    // offset 12
  uint32_t eblAddr;        // offset 24
  uint32_t crc32;          // offset 28
} ibrInfo_t;

// EBL data fields are stored in network (big) endian order
//  however, fields referenced within the aat of the ebl header are stored
//  in native (little) endian order.

#define AAT_MAX_SIZE 128

typedef struct eblHdr3xx_s
{
  uint16_t      tag;            /* = EBLTAG_HEADER              */
  uint16_t      len;            /* =                            */
  uint16_t      version;        /* Version of the ebl format    */
  uint16_t      signature;      /* Magic signature: 0xE350      */
  uint32_t      flashAddr;      /* Address where the AAT is stored */
  uint32_t      aatCrc;         /* CRC of the ebl header portion of the AAT */
  // aatBuff is oversized to account for the potential of the AAT to grow in
  //  the future.  Only the first 128 bytes of the AAT can be referenced as
  //  part of the ebl header, although the AAT itself may grow to 256 total
  uint8_t       aatBuff[AAT_MAX_SIZE];   /* buffer for the ebl portion of the AAT    */
} eblHdr3xx_t;

typedef struct      eblMetadata3xx_s
{
  uint16_t      tag;            /* = EBLTAG_METADATA            */
  uint16_t      len;            /* = 1..65534                   */
  uint8_t*      metaData;
}                   eblMetadata3xx_t;

typedef struct      eblProg3xx_s
{
  uint16_t      tag;            /* = EBLTAG_[ERASE|MFG]PROG     */
  uint16_t      len;            /* = 2..65534                   */
  uint32_t      flashAddr;      /* Starting addr in flash       */
  uint8_t*      flashData;      /* must be multiple of 2 */
}                   eblProg3xx_t;

typedef struct      eblEnd_s
{
  uint16_t      tag;            /* = EBLTAG_END                 */
  uint16_t      len;            /* = 4                          */
  uint32_t      eblCrc;         /* .ebl file CRC -Little-Endian-*/
}                   eblEnd_t;

#define EBL_ENCRYPT_HEADER_LENGTH 6  // does not include tag/length overhead
typedef struct eblEncHdr3xx_s
{
  uint16_t      tag;            /* = EBLTAG_ENC_HEADER          */
  uint16_t      len;            /* =   6                         */
  uint16_t      version;        /* Version of the EBL format    */
  uint16_t      encType;        /* Type of encryption used      */
  uint16_t      signature;      /* Magic signature: 0xE350      */
}                   eblEncHdr3xx_t;

#define EBL_ENCRYPT_INIT_LENGTH (4 + NONCE_LENGTH)  // does not include tag/length overhead
typedef struct eblEncInit3xx_s
{
  uint16_t      tag;            /* = EBLTAG_ENC_INIT            */
  uint16_t      len;            /* =                            */
  uint32_t      msgLen;         /* Length of the cipher text in bytes */
  uint8_t       nonce[NONCE_LENGTH];  /* Random nonce used for this message */
  uint8_t*      associatedData; /* Data that is authenticated but unencrypted */
}                   eblEncInit3xx_t;

typedef struct      eblEncData3xx_s
{
  uint16_t      tag;            /* = EBLTAG_ENC_EBL_DATA */
  uint16_t      len;            /* = 4..65534                   */
  uint8_t*      data;           /* = encrypted data (must contain integral
                                     number of unencrypted tags) */
}                   eblEncData3xx_t;

typedef struct      eblEncMac_s
{
  uint16_t      tag;            /* = EBLTAG_ENC_MAC             */
  uint16_t      len;            /* = 16                          */
  uint8_t       eblMac[MAC_LENGTH]; /* = Message Authenticity Check of the data */
}                   eblEncMac_t;

// Define the types of encryption known for EBL files
#define ENC_TYPE_NONE  (0x0000)
#define ENC_TYPE_CCM   (0x0001)

#define EBL_SIZE_CAN_BE_BIGGER true
#define EBL_SIZE_IS_EXACT      false

#define EBL_HEADER_MIN_SIZE  12
#define EBL_METADATA_MIN_SIZE 1
#define EBL_PROGRAM_MIN_SIZE 6
#define EBL_END_MIN_SIZE     4
#define EBL_ENC_HDR_SIZE     6
#define EBL_INIT_HDR_SIZE    (4 + NONCE_LENGTH)
#define EBL_ENC_DATA_SIZE    4
#define EBL_ENC_MAC_SIZE     (MAC_LENGTH)

#define EBLTAG_INFO_STRUCT_CONTENTS                                     \
  { EBLTAG_HEADER,         sizeof(eblHdr3xx_t),      EBL_HEADER_MIN_SIZE,   EBL_SIZE_CAN_BE_BIGGER, "EBL Header" }, \
  { EBLTAG_METADATA,       sizeof(eblMetadata3xx_t), EBL_METADATA_MIN_SIZE, EBL_SIZE_CAN_BE_BIGGER, "Metadata" }, \
	{ EBLTAG_PROG,           sizeof(eblProg3xx_t),     EBL_PROGRAM_MIN_SIZE,  EBL_SIZE_CAN_BE_BIGGER, "Program Data" }, \
  { EBLTAG_MFGPROG,        sizeof(eblProg3xx_t),     EBL_PROGRAM_MIN_SIZE,  EBL_SIZE_CAN_BE_BIGGER, "MFG Program Data" }, \
  { EBLTAG_ERASEPROG,      sizeof(eblProg3xx_t),     EBL_PROGRAM_MIN_SIZE,  EBL_SIZE_CAN_BE_BIGGER, "Erase then Program Data" }, \
  { EBLTAG_END,            sizeof(eblEnd_t),         EBL_END_MIN_SIZE,      EBL_SIZE_IS_EXACT,      "EBL End Tag" }, \
	{ EBLTAG_ENC_HEADER,     sizeof(eblEncHdr3xx_t),   EBL_ENC_HDR_SIZE,      EBL_SIZE_IS_EXACT,      "EBL Header for Encryption" }, \
	{ EBLTAG_ENC_INIT,       sizeof(eblEncInit3xx_t),  EBL_INIT_HDR_SIZE,     EBL_SIZE_CAN_BE_BIGGER, "EBL Init for Encryption" }, \
	{ EBLTAG_ENC_EBL_DATA,   sizeof(eblEncData3xx_t),  EBL_ENC_DATA_SIZE,     EBL_SIZE_CAN_BE_BIGGER, "Encrypted EBL Data" }, \
  { EBLTAG_ENC_MAC,        sizeof(eblEncMac_t),      EBL_ENC_MAC_SIZE,      EBL_SIZE_IS_EXACT,      "MAC Data for Encryption"}, \
  { 0xFFFF,                0,                        0,                     false,                  NULL },


/////////////////////
// ebl.c APIs


typedef struct {
  BL_Status (*eblGetData)(void *state, uint8_t *dataBytes, uint16_t len);
  BL_Status (*eblDataFinalize)(void *state);
} EblDataFuncType;

typedef struct {
  uint32_t fileCrc;
  bool headerValid;
  eblHdr3xx_t eblHeader;
  uint16_t encType;
  uint8_t encEblStateMachine;
  bool decryptEnabled;
  uint32_t byteCounter;
  ccmState_t encState;
} EblProcessStateType;

typedef struct {
  void *dataState;
  uint8_t *tagBuf;
  uint16_t tagBufLen;
  bool returnBetweenBlocks;
  EblProcessStateType eblState;
} EblConfigType;

typedef uint8_t (*flashReadFunc)(uint32_t address);

// uint8_t is used as the return type below to avoid needing to include 
//   ember-types.h for EmberStatus
typedef struct {
  uint8_t (*erase)(uint8_t eraseType, uint32_t address);
  uint8_t (*write)(uint32_t address, uint16_t * data, uint32_t length);
  flashReadFunc read;
} EblFlashFuncType;


// passed in tagBuf/tagBufLen must be at least EBL_MIN_TAG_SIZE, need not be
//  larger than EBL_MAX_TAG_SIZE
void eblProcessInit(EblConfigType *config,
                    void *dataState,
                    uint8_t *tagBuf,
                    uint16_t tagBufLen,
                    bool returnBetweenBlocks);

BL_Status eblProcess(const EblDataFuncType *dataFuncs, 
                     EblConfigType *config,
                     const EblFlashFuncType *flashFuncs);

#endif //__EBL_H__
