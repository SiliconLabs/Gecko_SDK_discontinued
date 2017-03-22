/***************************************************************************//**
 * @file btl_comm_bgapi.h
 * @brief Communication plugin implementing the BGAPI UART DFU protocol
 * @author Silicon Labs
 * @version 1.0.0
 *******************************************************************************
 * # License
 * <b>Copyright 2016 Silicon Laboratories, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#ifndef BTL_COMM_BGAPI_H
#define BTL_COMM_BGAPI_H

#include "core/btl_util.h"

MISRAC_DISABLE
#include "em_common.h"
MISRAC_ENABLE

/***************************************************************************//**
 * @addtogroup Plugin
 * @{
 * @addtogroup Communication
 * @{
 * @page communication_bgapi BGAPI UART DFU
 *   By enabling the BGAPI communication plugin, the bootloader communication
 *   interface implements the UART DFU protocol using BGAPI commands. This
 *   plugin makes the bootloader compatible with the legacy UART bootloader that
 *   was previously released with the Silicon Labs Bluetooth stack.
 ******************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

SL_PACK_START(1)
typedef struct {
  uint8_t type;
  uint8_t len;
  uint8_t class;
  uint8_t command;
} SL_ATTRIBUTE_PACKED BgapiPacketHeader_t;
SL_PACK_END()

SL_PACK_START(1)
typedef struct {
  uint8_t mode;
} SL_ATTRIBUTE_PACKED BgapiDataCmdDfuReset_t ;
SL_PACK_END()

SL_PACK_START(1)
typedef struct {
  uint32_t address;
} SL_ATTRIBUTE_PACKED BgapiDataCmdDfuFlashSetAddress_t;
SL_PACK_END()

SL_PACK_START(1)
typedef struct {
  uint8_t len;
  uint8_t data[128];
} SL_ATTRIBUTE_PACKED BgapiDataCmdDfuFlashUpload_t;
SL_PACK_END()

SL_PACK_START(1)
typedef struct {
  uint16_t error;
} SL_ATTRIBUTE_PACKED BgapiDataRsp_t;
SL_PACK_END()

SL_PACK_START(1)
typedef struct {
  uint32_t bootloaderVersion;
} SL_ATTRIBUTE_PACKED BgapiDataEvtBoot_t;
SL_PACK_END()

SL_PACK_START(1)
typedef struct {
  uint16_t error;
} SL_ATTRIBUTE_PACKED BgapiDataEvtBootFailure_t;
SL_PACK_END()

SL_PACK_START(1)
typedef struct {
  uint8_t padding[3]; // 3 bytes padding to ensure DFU Flash Upload data[] array is word aligned
  BgapiPacketHeader_t header;
  union body {
    BgapiDataCmdDfuReset_t            reset;
    BgapiDataCmdDfuFlashSetAddress_t  flashSetAddress;
    BgapiDataCmdDfuFlashUpload_t      flashUpload;
    BgapiDataRsp_t                    response;
    BgapiDataEvtBoot_t                boot;
    BgapiDataEvtBootFailure_t         bootFailure;
  } body;
} SL_ATTRIBUTE_PACKED BgapiPacket_t;
SL_PACK_END()

// --------------------------------
// Commands

typedef enum {
  RESET = 0x00,
  FLASH_SET_ADDRESS = 0x01,
  FLASH_UPLOAD = 0x02,
  FLASH_UPLOAD_FINISH = 0x03
} BgapiDfuCommand_t;

#define BGAPI_PACKET_TYPE_COMMAND 0x20
#define BGAPI_PACKET_TYPE_EVENT   0xA0

#define BGAPI_PACKET_CLASS_DFU    0x00

// --------------------------------
// Events

#define BGAPI_EVENT_DFU_BOOT()                              \
{                                                           \
  .header = {0xA0, 0x04, 0x00, 0x00},                       \
  .body.boot.bootloaderVersion = 0x0                        \
}

#define BGAPI_EVENT_DFU_BOOT_FAILURE(error)                 \
{                                                           \
  .header = {0xA0, 0x02, 0x00, 0x01},                       \
  .body.response = {(error)}                                \
}

// --------------------------------
// Responses

#define BGAPI_RESPONSE_DEFAULT                              \
{                                                           \
  .header = {0x20, 0x02, 0x00, 0x00},                       \
  .body.response = {0x0000}                                 \
}

// --------------------------------
// Errors

#define BGAPI_ERROR_SUCCESS                       0x0000
#define BGAPI_ERROR_BUFFERS_FULL                  0x018E
#define BGAPI_ERROR_SECURITY_IMAGE_CHECKSUM_ERROR 0x0B03

/** @endcond */

/** @} addtogroup Communication */
/** @} addtogroup Plugin */

#endif // BTL_COMM_BGAPI_H
