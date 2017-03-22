/*
 * File: spiflash-common.c
 * Description: SPIFlash driver that supports a variety of SPI flash parts
 *   Including: Spansion S25FL208K (8Mbit)
                Winbond W25X20BV (2Mbit), W25Q80BV (8Mbit)
 *              Macronix MX25L2006E (2Mbit), MX25L4006E (4Mbit), MX25L8006E (8Mbit)
 *              MX25R8035F (8Mbit low power), MX25L1606E (16Mbit), MX25U1635E (16Mbit 2Volt), 
 *              MX25R6435F (64Mbit low power)
 *              Atmel/Adesto AT25DF041A (4Mbit), AT25SF041 (4Mbit), AT25DF081A (8Mbit)
 *              Numonyx/Micron M25P20 (2Mbit), M25P40 (4Mbit),
 *                             M25P80 (8Mbit), M25P16 (16Mbit)
 *              ISSI IS25LQ025B (256Kbit), IS25LQ512B (512Kbit),
 *                   IS25LQ010B (1Mbit), IS25LQ020B (2Mbit), IS25LQ040B (4Mbit)
 *   It could be easily extended to support other parts that talk the
 *    same protocol as these.
 *
 *   This driver does not support any write protection functionality
 *
 * Copyright 2015 Silicon Laboratories, Inc.                                *80*
 *
 */

/*
 * When SPI_FLASH_SC1 is defined, serial controller one will be used to
 * communicate with the external flash.
 * - This must be enabled in the case of app bootloading over USB, since USB
 *   uses SC2. Also ensure that the board jumpers are in the right place and
 *   the debugger is not connected to any SC1 pins.
 * - This must be enabled in the case of PRO2+-connected devices since SC2
 *   is used to interact with the PRO2+ radio.
 */

#include PLATFORM_HEADER
#include "hal/micro/bootloader-eeprom.h"
#include "bootloader-common.h"
#include "bootloader-serial.h"
#include "hal/micro/micro.h"
#include "hal/micro/cortexm3/memmap.h"
#include BOARD_HEADER

#include "hal/micro/cortexm3/common/spiflash-low-level.h"

#define DEVICE_SIZE_256K                    (32768L)
#define DEVICE_SIZE_512K                    (65536L)
#define DEVICE_SIZE_1M                      (131072L)
#define DEVICE_SIZE_2M                      (262144L)
#define DEVICE_SIZE_4M                      (524288L)
#define DEVICE_SIZE_8M                      (1048576L)
#define DEVICE_SIZE_16M                     (2097152L)
#define DEVICE_SIZE_64M                     (8388608L)

// Pages are the write buffer granularity
#define DEVICE_PAGE_SIZE                    (256)
#define DEVICE_PAGE_MASK                    (255)
// Sectors are the erase granularity
// *except* for Numonyx parts which only support BLOCK erase granularity
#define DEVICE_SECTOR_SIZE                  (4096)
#define DEVICE_SECTOR_MASK                  (4095)
// Blocks define a larger erase granularity
#define DEVICE_BLOCK_SIZE_64K               (65536L)
#define DEVICE_BLOCK_MASK_64K               (65535L)
#define DEVICE_BLOCK_SIZE_32K               (32768L)
#define DEVICE_BLOCK_MASK_32K               (32767L)
// The flash word size in bytes
#define DEVICE_WORD_SIZE                    (1)

// JEDEC Manufacturer IDs
#define MFG_ID_SPANSION                     (0x01)
#define MFG_ID_WINBOND                      (0xEF)
#define MFG_ID_MACRONIX                     (0xC2)
#define MFG_ID_ATMEL                        (0x1F)
#define MFG_ID_NUMONYX                      (0x20)
#define MFG_ID_ISSI                         (0x9D)

// JEDEC Device IDs
#define DEVICE_ID_SPANSION_8M               (0x4014)
#define DEVICE_ID_WINBOND_2M                (0x3012)
#define DEVICE_ID_WINBOND_4M                (0x3013)
#define DEVICE_ID_WINBOND_8M                (0x4014)
#define DEVICE_ID_MACRONIX_2M               (0x2012)
#define DEVICE_ID_MACRONIX_4M               (0x2013)
#define DEVICE_ID_MACRONIX_8M               (0x2014)
#define DEVICE_ID_MACRONIX_8M_LP            (0x2814) 
#define DEVICE_ID_MACRONIX_16M              (0x2015)
#define DEVICE_ID_MACRONIX_16M_2V           (0x2535)
#define DEVICE_ID_MACRONIX_64M_LP           (0x2817)
#define DEVICE_ID_ATMEL_4M                  (0x4401)
#define DEVICE_ID_ATMEL_4M_S                (0x8401)
#define DEVICE_ID_ATMEL_8M                  (0x4501)
#define DEVICE_ID_NUMONYX_2M                (0x2012)
#define DEVICE_ID_NUMONYX_4M                (0x2013)
#define DEVICE_ID_NUMONYX_8M                (0x2014)
#define DEVICE_ID_NUMONYX_16M               (0x2015)
#define DEVICE_ID_ISSI_256K                 (0x4009)
#define DEVICE_ID_ISSI_512K                 (0x4010)
#define DEVICE_ID_ISSI_1M                   (0x4011)
#define DEVICE_ID_ISSI_2M                   (0x4012)
#define DEVICE_ID_ISSI_4M                   (0x4013)

// Protocol commands
#define CMD_WRITE_ENABLE                    (0x06)
#define CMD_WRITE_DISABLE                   (0x04)
#define CMD_READ_STATUS                     (0x05)
#define CMD_WRITE_STATUS                    (0x01)
#define CMD_READ_DATA                       (0x03)
#define CMD_PAGE_PROG                       (0x02)
#define CMD_ERASE_SECTOR                    (0x20)
#define CMD_ERASE_BLOCK                     (0xD8)
#define CMD_ERASE_CHIP                      (0xC7)
#define CMD_POWER_DOWN                      (0xB9)
#define CMD_POWER_UP                        (0xAB)
#define CMD_JEDEC_ID                        (0x9F)
#define CMD_UNIQUE_ID                       (0x4B)

// Bitmasks for status register fields
#define STATUS_BUSY_MASK                    (0x01)
#define STATUS_WEL_MASK                     (0x02)

// These timings represent the worst case out of all chips supported by this
//  driver.  Some chips may perform faster.
// (in general Winbond is faster than Macronix is faster than Numonyx)
#define TIMING_POWERON_MAX_US               (10000)
#define TIMING_SLEEP_MAX_US                 (10)
#define TIMING_WAKEUP_MAX_US                (30)
#define TIMING_PROG_MAX_US                  (5000)
#define TIMING_WRITE_STATUS_MAX_US          (40000)
// (MS units are 1024Hz based)
#define TIMING_ERASE_SECTOR_MAX_MS          (410)
#define TIMING_ERASE_BLOCK_MAX_MS           (3072)

#define TIMING_ERASE_4K_MAX_MS              (410)
#define TIMING_ERASE_64K_MAX_MS             (3072)
#define TIMING_ERASE_SPANSION_8M_MAX_MS     (15360)
#define TIMING_ERASE_WINBOND_2M_MAX_MS      (2048)
#define TIMING_ERASE_WINBOND_4M_MAX_MS      (4096)
#define TIMING_ERASE_WINBOND_8M_MAX_MS      (6144)
#define TIMING_ERASE_MACRONIX_2M_MAX_MS     (3892)
#define TIMING_ERASE_MACRONIX_4M_MAX_MS     (7680)
#define TIMING_ERASE_MACRONIX_8M_MAX_MS     (15360)
#define TIMING_ERASE_MACRONIX_8M_LP_MAX_MS  (30720)
#define TIMING_ERASE_MACRONIX_16M_MAX_MS    (30720)
#define TIMING_ERASE_MACRONIX_16M_2V_MAX_MS (20480)
#define TIMING_ERASE_MACRONIX_64M_LP_MAX_MS (245760)
#define TIMING_ERASE_ATMEL_4M_MAX_MS        (7168)
#define TIMING_ERASE_ATMEL_4M_S_MAX_MS      (12288)
#define TIMING_ERASE_ATMEL_8M_MAX_MS        (28672)
#define TIMING_ERASE_NUMONYX_2M_MAX_MS      (6144)
#define TIMING_ERASE_NUMONYX_4M_MAX_MS      (10240)
#define TIMING_ERASE_NUMONYX_8M_MAX_MS      (20480)
#define TIMING_ERASE_NUMONYX_16M_MAX_MS     (40960)
#define TIMING_ERASE_ISSI_256K_MAX_MS       (512)
#define TIMING_ERASE_ISSI_512K_MAX_MS       (1024)
#define TIMING_ERASE_ISSI_1M_MAX_MS         (1536)
#define TIMING_ERASE_ISSI_2M_MAX_MS         (2048)
#define TIMING_ERASE_ISSI_4M_MAX_MS         (3072)

#if defined(SPIFLASH_SPANSION_DEVICES)
#define SPIFLASH_SPANSION_S25FL208K  // 8MB
#endif

#if defined(SPIFLASH_WINBOND_DEVICES)
#define SPIFLASH_WINBOND_W25X20BV    // 2MB
#define SPIFLASH_WINBOND_W25Q80BV    // 8MB
#endif

#if defined(SPIFLASH_MACRONIX_DEVICES)
#define SPIFLASH_MACRONIX_MX25L2006E // 2MB
#define SPIFLASH_MACRONIX_MX25L4006E // 4MB
#define SPIFLASH_MACRONIX_MX25L8006E // 8MB
#define SPIFLASH_MACRONIX_MX25R8035F // 8MB Low Power
#define SPIFLASH_MACRONIX_MX25L1606E // 16MB
#define SPIFLASH_MACRONIX_MX25U1635E // 16MB 2V
#define SPIFLASH_MACRONIX_MX25R6435F // 64MB Low Power
#endif

#if defined(SPIFLASH_ATMEL_DEVICES)
#define SPIFLASH_ATMEL_AT25DF041A    // 4MB
#define SPIFLASH_ATMEL_AT25SF041     // 4MB
#define SPIFLASH_ATMEL_AT25DF081A    // 8MB
#endif

#if defined(SPIFLASH_NUMONYX_DEVICES)
#define SPIFLASH_NUMONYX_M25P20      // 2MB
#define SPIFLASH_NUMONYX_M25P40      // 4MB
#define SPIFLASH_NUMONYX_M25P80      // 8MB
#define SPIFLASH_NUMONYX_M25P16      // 16MB
#endif

#if defined(SPIFLASH_ISSI_DEVICES)
#define SPIFLASH_ISSI_IS25LQ025B     // 256kB
#define SPIFLASH_ISSI_IS25LQ512B     // 512kB
#define SPIFLASH_ISSI_IS25LQ010B     // 1MB
#define SPIFLASH_ISSI_IS25LQ020B     // 2MB
#define SPIFLASH_ISSI_IS25LQ040B     // 4MB
#endif

#if defined(SPIFLASH_ALL_DEVICES) || \
 (!defined(SPIFLASH_SPANSION_DEVICES) && \
  !defined(SPIFLASH_SPANSION_S25FL208K) && \
  !defined(SPIFLASH_WINBOND_DEVICES) && \
  !defined(SPIFLASH_WINBOND_W25X20BV) && \
  !defined(SPIFLASH_WINBOND_W25Q80BV) && \
  !defined(SPIFLASH_MACRONIX_DEVICES) && \
  !defined(SPIFLASH_MACRONIX_MX25L2006E) && \
  !defined(SPIFLASH_MACRONIX_MX25L4006E) && \
  !defined(SPIFLASH_MACRONIX_MX25L8006E) && \
  !defined(SPIFLASH_MACRONIX_MX25R8035F) && \
  !defined(SPIFLASH_MACRONIX_MX25L1606E) && \
  !defined(SPIFLASH_MACRONIX_MX25U1635E) && \
  !defined(SPIFLASH_MACRONIX_MX25R6435F) && \
  !defined(SPIFLASH_ATMEL_DEVICES) && \
  !defined(SPIFLASH_ATMEL_AT25DF041A) && \
  !defined(SPIFLASH_ATMEL_AT25DF081A) && \
  !defined(SPIFLASH_NUMONYX_DEVICES) && \
  !defined(SPIFLASH_NUMONYX_M25P20) && \
  !defined(SPIFLASH_NUMONYX_M25P40) && \
  !defined(SPIFLASH_NUMONYX_M25P80) && \
  !defined(SPIFLASH_NUMONYX_M25P16) && \
  !defined(SPIFLASH_ISSI_DEVICES) && \
  !defined(SPIFLASH_ISSI_IS25LQ025B) && \
  !defined(SPIFLASH_ISSI_IS25LQ512B) && \
  !defined(SPIFLASH_ISSI_IS25LQ010B) && \
  !defined(SPIFLASH_ISSI_IS25LQ020B) && \
  !defined(SPIFLASH_ISSI_IS25LQ040B))
#define SPIFLASH_SPANSION_S25FL208K  // 8MB
#define SPIFLASH_WINBOND_W25X20BV    // 2MB
#define SPIFLASH_WINBOND_W25Q80BV    // 8MB
#define SPIFLASH_MACRONIX_MX25L2006E // 2MB
#define SPIFLASH_MACRONIX_MX25L4006E // 4MB
#define SPIFLASH_MACRONIX_MX25L8006E // 8MB
#define SPIFLASH_MACRONIX_MX25R8035F // 8MB Low Power
#define SPIFLASH_MACRONIX_MX25L1606E // 16MB
#define SPIFLASH_MACRONIX_MX25U1635E // 16MB 2V
#define SPIFLASH_MACRONIX_MX25R6435F // 64MB Low Power
#define SPIFLASH_ATMEL_AT25DF041A    // 4MB
#define SPIFLASH_ATMEL_AT25SF041     // 4MB
#define SPIFLASH_ATMEL_AT25DF081A    // 8MB
#define SPIFLASH_NUMONYX_M25P20      // 2MB
#define SPIFLASH_NUMONYX_M25P40      // 4MB
#define SPIFLASH_NUMONYX_M25P80      // 8MB
#define SPIFLASH_NUMONYX_M25P16      // 16MB
#define SPIFLASH_ISSI_IS25LQ025B     // 256kB
#define SPIFLASH_ISSI_IS25LQ512B     // 512kB
#define SPIFLASH_ISSI_IS25LQ010B     // 1MB
#define SPIFLASH_ISSI_IS25LQ020B     // 2MB
#define SPIFLASH_ISSI_IS25LQ040B     // 4MB
#endif

#if defined(SPIFLASH_SPANSION_S25FL208K)
static const HalEepromInformationType spansion8MInfo = {
  EEPROM_INFO_VERSION,
  EEPROM_CAPABILITIES_ERASE_SUPPORTED | EEPROM_CAPABILITIES_PAGE_ERASE_REQD,
  TIMING_ERASE_4K_MAX_MS,
  TIMING_ERASE_SPANSION_8M_MAX_MS,
  DEVICE_SECTOR_SIZE,
  DEVICE_SIZE_8M,
  "S25FL208K",
  DEVICE_WORD_SIZE // word size in bytes
};
#endif

#if defined(SPIFLASH_WINBOND_W25X20BV)
static const HalEepromInformationType windbond2MInfo = {
  EEPROM_INFO_VERSION,
  EEPROM_CAPABILITIES_ERASE_SUPPORTED | EEPROM_CAPABILITIES_PAGE_ERASE_REQD,
  TIMING_ERASE_SECTOR_MAX_MS,
  TIMING_ERASE_WINBOND_2M_MAX_MS,
  DEVICE_SECTOR_SIZE,
  DEVICE_SIZE_2M,
  "W25X20BV",
  DEVICE_WORD_SIZE // word size in bytes
};
#endif

#if defined(SPIFLASH_WINBOND_W25Q80BV)
static const HalEepromInformationType windbond8MInfo = {
  EEPROM_INFO_VERSION,
  EEPROM_CAPABILITIES_ERASE_SUPPORTED | EEPROM_CAPABILITIES_PAGE_ERASE_REQD,
  TIMING_ERASE_SECTOR_MAX_MS,
  TIMING_ERASE_WINBOND_8M_MAX_MS,
  DEVICE_SECTOR_SIZE,
  DEVICE_SIZE_8M,
  "W25Q80BV",
  DEVICE_WORD_SIZE // word size in bytes
};
#endif

#if defined(SPIFLASH_MACRONIX_MX25L2006E)
static const HalEepromInformationType macronix2MInfo = {
  EEPROM_INFO_VERSION,
  EEPROM_CAPABILITIES_ERASE_SUPPORTED | EEPROM_CAPABILITIES_PAGE_ERASE_REQD,
  TIMING_ERASE_SECTOR_MAX_MS,
  TIMING_ERASE_MACRONIX_2M_MAX_MS,
  DEVICE_SECTOR_SIZE,
  DEVICE_SIZE_2M,
  "MX25L2006E",
  DEVICE_WORD_SIZE // word size in bytes
};
#endif

#if defined(SPIFLASH_MACRONIX_MX25L4006E)
static const HalEepromInformationType macronix4MInfo = {
  EEPROM_INFO_VERSION,
  EEPROM_CAPABILITIES_ERASE_SUPPORTED | EEPROM_CAPABILITIES_PAGE_ERASE_REQD,
  TIMING_ERASE_4K_MAX_MS,
  TIMING_ERASE_MACRONIX_4M_MAX_MS,
  DEVICE_SECTOR_SIZE,
  DEVICE_SIZE_4M,
  "MX25L4006E",
  DEVICE_WORD_SIZE // word size in bytes
};
#endif

#if defined(SPIFLASH_MACRONIX_MX25L8006E)
static const HalEepromInformationType macronix8MInfo = {
  EEPROM_INFO_VERSION,
  EEPROM_CAPABILITIES_ERASE_SUPPORTED | EEPROM_CAPABILITIES_PAGE_ERASE_REQD,
  TIMING_ERASE_SECTOR_MAX_MS,
  TIMING_ERASE_MACRONIX_8M_MAX_MS,
  DEVICE_SECTOR_SIZE,
  DEVICE_SIZE_8M,
  "MX25L8006E",
  DEVICE_WORD_SIZE // word size in bytes
};
#endif

#if defined(SPIFLASH_MACRONIX_MX25R8035F)
static const HalEepromInformationType macronix8MLPInfo = {
  EEPROM_INFO_VERSION,
  EEPROM_CAPABILITIES_ERASE_SUPPORTED | EEPROM_CAPABILITIES_PAGE_ERASE_REQD,
  TIMING_ERASE_SECTOR_MAX_MS,
  TIMING_ERASE_MACRONIX_8M_LP_MAX_MS,
  DEVICE_SECTOR_SIZE,
  DEVICE_SIZE_8M,
  "MX25R8035F",
  DEVICE_WORD_SIZE // word size in bytes
};
#endif

#if defined(SPIFLASH_MACRONIX_MX25L1606E)
static const HalEepromInformationType macronix16MInfo = {
  EEPROM_INFO_VERSION,
  EEPROM_CAPABILITIES_ERASE_SUPPORTED | EEPROM_CAPABILITIES_PAGE_ERASE_REQD,
  TIMING_ERASE_SECTOR_MAX_MS,
  TIMING_ERASE_MACRONIX_16M_MAX_MS,
  DEVICE_SECTOR_SIZE,
  DEVICE_SIZE_16M,
  "MX25L1606E",
  DEVICE_WORD_SIZE // word size in bytes
};
#endif

#if defined(SPIFLASH_MACRONIX_MX25U1635E)
static const HalEepromInformationType macronix16M2VInfo = {
  EEPROM_INFO_VERSION,
  EEPROM_CAPABILITIES_ERASE_SUPPORTED | EEPROM_CAPABILITIES_PAGE_ERASE_REQD,
  TIMING_ERASE_SECTOR_MAX_MS,
  TIMING_ERASE_MACRONIX_16M_2V_MAX_MS,
  DEVICE_SECTOR_SIZE,
  DEVICE_SIZE_16M,
  "MX25U1635E",
  DEVICE_WORD_SIZE // word size in bytes
};
#endif

#if defined(SPIFLASH_MACRONIX_MX25R6435F)
static const HalEepromInformationType macronix64MLPInfo = {
  EEPROM_INFO_VERSION,
  EEPROM_CAPABILITIES_ERASE_SUPPORTED | EEPROM_CAPABILITIES_PAGE_ERASE_REQD,
  TIMING_ERASE_SECTOR_MAX_MS,
  TIMING_ERASE_MACRONIX_64M_LP_MAX_MS,
  DEVICE_SECTOR_SIZE,
  DEVICE_SIZE_64M,
  "MX25R6435F",
  DEVICE_WORD_SIZE // word size in bytes
};
#endif

#if defined(SPIFLASH_ATMEL_AT25DF041A)
static const HalEepromInformationType atmel4MInfo = {
  EEPROM_INFO_VERSION,
  EEPROM_CAPABILITIES_ERASE_SUPPORTED | EEPROM_CAPABILITIES_PAGE_ERASE_REQD,
  TIMING_ERASE_SECTOR_MAX_MS,
  TIMING_ERASE_ATMEL_4M_MAX_MS,
  DEVICE_SECTOR_SIZE,
  DEVICE_SIZE_4M,
  "AT25DF041A",
  DEVICE_WORD_SIZE // word size in bytes
};
#endif

#if defined(SPIFLASH_ATMEL_AT25SF041)
static const HalEepromInformationType atmel4MSInfo = {
  EEPROM_INFO_VERSION,
  EEPROM_CAPABILITIES_ERASE_SUPPORTED | EEPROM_CAPABILITIES_PAGE_ERASE_REQD,
  TIMING_ERASE_SECTOR_MAX_MS,
  TIMING_ERASE_ATMEL_4M_S_MAX_MS,
  DEVICE_SECTOR_SIZE,
  DEVICE_SIZE_4M,
  "AT25SF041",
  DEVICE_WORD_SIZE // word size in bytes
};
#endif

#if defined(SPIFLASH_ATMEL_AT25DF081A)
static const HalEepromInformationType atmel8MInfo = {
  EEPROM_INFO_VERSION,
  EEPROM_CAPABILITIES_ERASE_SUPPORTED | EEPROM_CAPABILITIES_PAGE_ERASE_REQD,
  TIMING_ERASE_SECTOR_MAX_MS,
  TIMING_ERASE_ATMEL_8M_MAX_MS,
  DEVICE_SECTOR_SIZE,
  DEVICE_SIZE_8M,
  "AT25DF081A",
  DEVICE_WORD_SIZE // word size in bytes
};
#endif

#if defined(SPIFLASH_NUMONYX_M25P20)
static const HalEepromInformationType numonyx2MInfo = {
  EEPROM_INFO_VERSION,
  EEPROM_CAPABILITIES_ERASE_SUPPORTED | EEPROM_CAPABILITIES_PAGE_ERASE_REQD,
  TIMING_ERASE_BLOCK_MAX_MS,
  TIMING_ERASE_NUMONYX_2M_MAX_MS,
  DEVICE_BLOCK_SIZE_64K, // Numonyx does not support smaller sector erase
  DEVICE_SIZE_2M,
  "M25P20",
  DEVICE_WORD_SIZE // word size in bytes
};
#endif

#if defined(SPIFLASH_NUMONYX_M25P40)
static const HalEepromInformationType numonyx4MInfo = {
  EEPROM_INFO_VERSION,
  EEPROM_CAPABILITIES_ERASE_SUPPORTED | EEPROM_CAPABILITIES_PAGE_ERASE_REQD,
  TIMING_ERASE_BLOCK_MAX_MS,
  TIMING_ERASE_NUMONYX_4M_MAX_MS,
  DEVICE_BLOCK_SIZE_64K, // Numonyx does not support smaller sector erase
  DEVICE_SIZE_4M,
  "M25P40",
  DEVICE_WORD_SIZE // word size in bytes
};
#endif

#if defined(SPIFLASH_NUMONYX_M25P80)
static const HalEepromInformationType numonyx8MInfo = {
  EEPROM_INFO_VERSION,
  EEPROM_CAPABILITIES_ERASE_SUPPORTED | EEPROM_CAPABILITIES_PAGE_ERASE_REQD,
  TIMING_ERASE_BLOCK_MAX_MS,
  TIMING_ERASE_NUMONYX_8M_MAX_MS,
  DEVICE_BLOCK_SIZE_64K, // Numonyx does not support smaller sector erase
  DEVICE_SIZE_8M,
  "M25P80",
  DEVICE_WORD_SIZE // word size in bytes
};
#endif

#if defined(SPIFLASH_NUMONYX_M25P16)
static const HalEepromInformationType numonyx16MInfo = {
  EEPROM_INFO_VERSION,
  EEPROM_CAPABILITIES_ERASE_SUPPORTED | EEPROM_CAPABILITIES_PAGE_ERASE_REQD,
  TIMING_ERASE_BLOCK_MAX_MS,
  TIMING_ERASE_NUMONYX_16M_MAX_MS,
  DEVICE_BLOCK_SIZE_64K, // Numonyx does not support smaller sector erase
  DEVICE_SIZE_16M,
  "M25P16",
  DEVICE_WORD_SIZE // word size in bytes
};
#endif

#if defined(SPIFLASH_ISSI_IS25LQ025B)
static const HalEepromInformationType issi256KInfo = {
  EEPROM_INFO_VERSION,
  EEPROM_CAPABILITIES_ERASE_SUPPORTED | EEPROM_CAPABILITIES_PAGE_ERASE_REQD,
  TIMING_ERASE_SECTOR_MAX_MS,
  TIMING_ERASE_ISSI_256K_MAX_MS,
  DEVICE_SECTOR_SIZE,
  DEVICE_SIZE_256K,
  "IS25LQ025B",
  DEVICE_WORD_SIZE // word size in bytes
};
#endif

#if defined(SPIFLASH_ISSI_IS25LQ512B)
static const HalEepromInformationType issi512KInfo = {
  EEPROM_INFO_VERSION,
  EEPROM_CAPABILITIES_ERASE_SUPPORTED | EEPROM_CAPABILITIES_PAGE_ERASE_REQD,
  TIMING_ERASE_SECTOR_MAX_MS,
  TIMING_ERASE_ISSI_512K_MAX_MS,
  DEVICE_SECTOR_SIZE,
  DEVICE_SIZE_512K,
  "IS25LQ512B",
  DEVICE_WORD_SIZE // word size in bytes
};
#endif

#if defined(SPIFLASH_ISSI_IS25LQ010B)
static const HalEepromInformationType issi1MInfo = {
  EEPROM_INFO_VERSION,
  EEPROM_CAPABILITIES_ERASE_SUPPORTED | EEPROM_CAPABILITIES_PAGE_ERASE_REQD,
  TIMING_ERASE_SECTOR_MAX_MS,
  TIMING_ERASE_ISSI_1M_MAX_MS,
  DEVICE_SECTOR_SIZE,
  DEVICE_SIZE_1M,
  "IS25LQ010B",
  DEVICE_WORD_SIZE // word size in bytes
};
#endif

#if defined(SPIFLASH_ISSI_IS25LQ020B)
static const HalEepromInformationType issi2MInfo = {
  EEPROM_INFO_VERSION,
  EEPROM_CAPABILITIES_ERASE_SUPPORTED | EEPROM_CAPABILITIES_PAGE_ERASE_REQD,
  TIMING_ERASE_SECTOR_MAX_MS,
  TIMING_ERASE_ISSI_2M_MAX_MS,
  DEVICE_SECTOR_SIZE,
  DEVICE_SIZE_2M,
  "IS25LQ020B",
  DEVICE_WORD_SIZE // word size in bytes
};
#endif

#if defined(SPIFLASH_ISSI_IS25LQ040B)
static const HalEepromInformationType issi4MInfo = {
  EEPROM_INFO_VERSION,
  EEPROM_CAPABILITIES_ERASE_SUPPORTED | EEPROM_CAPABILITIES_PAGE_ERASE_REQD,
  TIMING_ERASE_SECTOR_MAX_MS,
  TIMING_ERASE_ISSI_4M_MAX_MS,
  DEVICE_SECTOR_SIZE,
  DEVICE_SIZE_4M,
  "IS25LQ040B",
  DEVICE_WORD_SIZE // word size in bytes
};
#endif

typedef enum {
  UNKNOWN_DEVICE,
  SPANSION_8M_DEVICE,
  WINBOND_2M_DEVICE,
  WINBOND_8M_DEVICE,
  MACRONIX_2M_DEVICE,
  MACRONIX_4M_DEVICE,
  MACRONIX_8M_DEVICE,
  MACRONIX_8M_LP_DEVICE,
  MACRONIX_16M_DEVICE,
  MACRONIX_16M_2V_DEVICE,
  MACRONIX_64M_LP_DEVICE,
  ATMEL_4M_DEVICE,
  ATMEL_4M_S_DEVICE,
  ATMEL_8M_DEVICE,
  //N.B. If add more ATMEL_ devices, update halEepromInit() accordingly
  NUMONYX_2M_DEVICE,
  NUMONYX_4M_DEVICE,
  NUMONYX_8M_DEVICE,
  NUMONYX_16M_DEVICE,
  //N.B. If add more NUMONYX_ devices, update halEepromErase() accordingly
  ISSI_256K_DEVICE,
  ISSI_512K_DEVICE,
  ISSI_1M_DEVICE,
  ISSI_2M_DEVICE,
  ISSI_4M_DEVICE,
} DeviceType;

//
// ~~~~~~~~~~~~~~~~~~~~ GENERIC SPI FUNCTIONS ~~~~~~~~~~~~~~~~~~~~
//
void halSpiPushN8(uint8_t n) 
{
  while(n--) {
    halSpiPush8(0xFF);
  }
}

void halSpiPopN8(uint8_t n) 
{
  while(n--) {
    halSpiPop8();
  }
}

void halSpiPush24MSB(uint32_t txData)
{
  halSpiPush8((txData >> 16) & 0xFF);
  halSpiPush8((txData >> 8) & 0xFF);
  halSpiPush8(txData & 0xFF);
}

uint16_t halSpiPop16MSB(void)
{
  uint16_t result;
  result = ((uint16_t)halSpiPop8()) << 8;
  result |= ((uint16_t)halSpiPop8());
  return result;
}

void halSpiWrite8(uint8_t txData)
{
  halSpiPush8(txData);
  halSpiPop8();
}

uint8_t halSpiRead8(void)
{
  halSpiPush8(0xFF);
  return halSpiPop8();
}

//
// ~~~~~~~~~~~~~~~~~~~~~~~~ Generic Device Interface ~~~~~~~~~~~~~~~~~~~~~~~~~~
//

// Note: This driver does not support block write protection features

static void waitNotBusy(void)
{
  while(halEepromBusy()) {
    // Do nothing
  }
}

static DeviceType getDeviceType(void)
{
  uint8_t mfgId;
  uint16_t deviceId;

  // cannot check for busy in this API since it is used by
  //  init.  Callers must verify not busy individually.
  setFlashCSActive();
  // following implementation takes smaller buffer (3) of efm into account
  halSpiWrite8(CMD_JEDEC_ID);
  mfgId = halSpiRead8();
  halSpiPushN8(2);
  deviceId = halSpiPop16MSB();
  setFlashCSInactive();

  switch(mfgId) {
    case MFG_ID_SPANSION:
      switch(deviceId) {
        #if defined(SPIFLASH_SPANSION_S25FL208K)
        case DEVICE_ID_SPANSION_8M:
          return SPANSION_8M_DEVICE;
        #endif
        default:
          return UNKNOWN_DEVICE;
      }
    case MFG_ID_WINBOND:
      switch(deviceId) {
        #if defined(SPIFLASH_WINBOND_W25X20BV)
        case DEVICE_ID_WINBOND_2M:
          return WINBOND_2M_DEVICE;
        #endif
        #if defined(SPIFLASH_WINBOND_W25Q80BV)
        case DEVICE_ID_WINBOND_8M:
          return WINBOND_8M_DEVICE;
        #endif
        default:
          return UNKNOWN_DEVICE;
      }
    case MFG_ID_MACRONIX:
      switch(deviceId) {
        #if defined(SPIFLASH_MACRONIX_MX25L2006E)
        case DEVICE_ID_MACRONIX_2M:
          return MACRONIX_2M_DEVICE;
        #endif
        #if defined(SPIFLASH_MACRONIX_MX25L4006E)
        case DEVICE_ID_MACRONIX_4M:
          return MACRONIX_4M_DEVICE;
        #endif
        #if defined(SPIFLASH_MACRONIX_MX25L8006E)
        case DEVICE_ID_MACRONIX_8M:
          return MACRONIX_8M_DEVICE;
        #endif
        #if defined(SPIFLASH_MACRONIX_MX25R8035F)
        case DEVICE_ID_MACRONIX_8M_LP:
          return MACRONIX_8M_LP_DEVICE;
        #endif
        #if defined(SPIFLASH_MACRONIX_MX25L1606E)
        case DEVICE_ID_MACRONIX_16M:
          return MACRONIX_16M_DEVICE;
        #endif
        #if defined(SPIFLASH_MACRONIX_MX25U1635E)
        case DEVICE_ID_MACRONIX_16M_2V:
          return MACRONIX_16M_2V_DEVICE;
        #endif
        #if defined(SPIFLASH_MACRONIX_MX25R6435F)
        case DEVICE_ID_MACRONIX_64M_LP:
          return MACRONIX_64M_LP_DEVICE;
        #endif
        default:
          return UNKNOWN_DEVICE;
      }
    case MFG_ID_ATMEL:
      switch(deviceId)
      {
        #if defined(SPIFLASH_ATMEL_AT25DF041A)
        case DEVICE_ID_ATMEL_4M:
          return ATMEL_4M_DEVICE;
        #endif
        #if defined(SPIFLASH_ATMEL_AT25SF041)
        case DEVICE_ID_ATMEL_4M_S:
          return ATMEL_4M_S_DEVICE;
        #endif
        #if defined(SPIFLASH_ATMEL_AT25DF081A)
        case DEVICE_ID_ATMEL_8M:
          return ATMEL_8M_DEVICE;
        #endif
        default:
          return UNKNOWN_DEVICE;
      }
    case MFG_ID_NUMONYX:
      switch(deviceId) {
        #if defined(SPIFLASH_NUMONYX_M25P20)
        case DEVICE_ID_NUMONYX_2M:
          return NUMONYX_2M_DEVICE;
        #endif
        #if defined(SPIFLASH_NUMONYX_M25P40)
        case DEVICE_ID_NUMONYX_4M:
          return NUMONYX_4M_DEVICE;
        #endif
        #if defined(SPIFLASH_NUMONYX_M25P80)
        case DEVICE_ID_NUMONYX_8M:
          return NUMONYX_8M_DEVICE;
        #endif
        #if defined(SPIFLASH_NUMONYX_M25P16)
        case DEVICE_ID_NUMONYX_16M:
          return NUMONYX_16M_DEVICE;
        #endif
        default:
          return UNKNOWN_DEVICE;
      }
    case MFG_ID_ISSI:
      switch(deviceId) {
        #if defined(SPIFLASH_ISSI_IS25LQ025B)
        case DEVICE_ID_ISSI_256K:
          return ISSI_256K_DEVICE;
        #endif
        #if defined(SPIFLASH_ISSI_IS25LQ512B)
        case DEVICE_ID_ISSI_512K:
          return ISSI_512K_DEVICE;
        #endif
        #if defined(SPIFLASH_ISSI_IS25LQ010B)
        case DEVICE_ID_ISSI_1M:
          return ISSI_1M_DEVICE;
        #endif
        #if defined(SPIFLASH_ISSI_IS25LQ020B)
        case DEVICE_ID_ISSI_2M:
          return ISSI_2M_DEVICE;
        #endif
        #if defined(SPIFLASH_ISSI_IS25LQ040B)
        case DEVICE_ID_ISSI_4M:
          return ISSI_4M_DEVICE;
        #endif
        default:
          return UNKNOWN_DEVICE;
      }
    default:
      return UNKNOWN_DEVICE;
  }
}

static void setWEL(void)
{
  setFlashCSActive();
  halSpiWrite8(CMD_WRITE_ENABLE);
  setFlashCSInactive();
}

const HalEepromInformationType *halEepromInfo(void)
{
  waitNotBusy();
  switch(getDeviceType()) {
    #if defined(SPIFLASH_SPANSION_S25FL208K)
    case SPANSION_8M_DEVICE:
      return &spansion8MInfo;
    #endif
    #if defined(SPIFLASH_WINBOND_W25X20BV)
    case WINBOND_2M_DEVICE:
      return &windbond2MInfo;
    #endif
    #if defined(SPIFLASH_WINBOND_W25Q80BV)
    case WINBOND_8M_DEVICE:
      return &windbond8MInfo;
    #endif
    #if defined(SPIFLASH_MACRONIX_MX25L2006E)
    case MACRONIX_2M_DEVICE:
      return &macronix2MInfo;
    #endif
    #if defined(SPIFLASH_MACRONIX_MX25L4006E)
    case MACRONIX_4M_DEVICE:
      return &macronix4MInfo; 
    #endif
    #if defined(SPIFLASH_MACRONIX_MX25L8006E)
    case MACRONIX_8M_DEVICE:
      return &macronix8MInfo;
    #endif
    #if defined(SPIFLASH_MACRONIX_MX25R8035F)
    case MACRONIX_8M_LP_DEVICE:
      return &macronix8MLPInfo;
    #endif
    #if defined(SPIFLASH_MACRONIX_MX25L1606E)
    case MACRONIX_16M_DEVICE:
      return &macronix16MInfo;
    #endif
    #if defined(SPIFLASH_MACRONIX_MX25U1635E)
    case MACRONIX_16M_2V_DEVICE:
      return &macronix16M2VInfo;
    #endif
    #if defined(SPIFLASH_MACRONIX_MX25R6435F)
    case MACRONIX_64M_LP_DEVICE:
      return &macronix64MLPInfo;
    #endif
    #if defined(SPIFLASH_ATMEL_AT25DF041A)
    case ATMEL_4M_DEVICE:
      return &atmel4MInfo;
    #endif
    #if defined(SPIFLASH_ATMEL_AT25SF041)
    case ATMEL_4M_S_DEVICE:
      return &atmel4MSInfo;
    #endif
    #if defined(SPIFLASH_ATMEL_AT25DF081A)
    case ATMEL_8M_DEVICE:
      return &atmel8MInfo;
    #endif
    #if defined(SPIFLASH_NUMONYX_M25P20)
    case NUMONYX_2M_DEVICE:
      return &numonyx2MInfo;
    #endif
    #if defined(SPIFLASH_NUMONYX_M25P40)
    case NUMONYX_4M_DEVICE:
      return &numonyx4MInfo;
    #endif
    #if defined(SPIFLASH_NUMONYX_M25P80)
    case NUMONYX_8M_DEVICE:
      return &numonyx8MInfo;
    #endif
    #if defined(SPIFLASH_NUMONYX_M25P16)
    case NUMONYX_16M_DEVICE:
      return &numonyx16MInfo;
    #endif
    #if defined(SPIFLASH_ISSI_IS25LQ025B)
    case ISSI_256K_DEVICE:
      return &issi256KInfo;
    #endif
    #if defined(SPIFLASH_ISSI_IS25LQ512B)
    case ISSI_512K_DEVICE:
      return &issi512KInfo;
    #endif
    #if defined(SPIFLASH_ISSI_IS25LQ010B)
    case ISSI_1M_DEVICE:
      return &issi1MInfo;
    #endif
    #if defined(SPIFLASH_ISSI_IS25LQ020B)
    case ISSI_2M_DEVICE:
      return &issi2MInfo;
    #endif
    #if defined(SPIFLASH_ISSI_IS25LQ040B)
    case ISSI_4M_DEVICE:
      return &issi4MInfo;
    #endif
    default:
      return NULL;
  }
}

static uint32_t getDeviceSize(DeviceType *pDeviceType)
{
  DeviceType deviceType;
  waitNotBusy();
  if(pDeviceType == NULL) {
    deviceType = getDeviceType();
  } else {
    deviceType = *pDeviceType;
    if(deviceType == UNKNOWN_DEVICE) {
      deviceType = getDeviceType();
      *pDeviceType = deviceType;
    }
  }
  switch(deviceType) {
    case ISSI_256K_DEVICE:
      return DEVICE_SIZE_256K;
    case ISSI_512K_DEVICE:
      return DEVICE_SIZE_512K;
    case ISSI_1M_DEVICE:
      return DEVICE_SIZE_1M;
    case WINBOND_2M_DEVICE:
    case NUMONYX_2M_DEVICE:
    case MACRONIX_2M_DEVICE:
    case ISSI_2M_DEVICE:
      return DEVICE_SIZE_2M;
    case ATMEL_4M_DEVICE:
    case ATMEL_4M_S_DEVICE:
    case NUMONYX_4M_DEVICE:
    case ISSI_4M_DEVICE:
    case MACRONIX_4M_DEVICE: 
      return DEVICE_SIZE_4M;
    case SPANSION_8M_DEVICE:
    case WINBOND_8M_DEVICE:
    case ATMEL_8M_DEVICE:
    case MACRONIX_8M_DEVICE:
    case MACRONIX_8M_LP_DEVICE:
    case NUMONYX_8M_DEVICE:
      return DEVICE_SIZE_8M;
    case MACRONIX_16M_DEVICE:
    case MACRONIX_16M_2V_DEVICE:
    case NUMONYX_16M_DEVICE:
      return DEVICE_SIZE_16M;
    case MACRONIX_64M_LP_DEVICE:
      return DEVICE_SIZE_64M;
    default:
      return 0;
  }
}

uint8_t halEepromInit(void)
{
  DeviceType deviceType;

  halEepromConfigureFlashController();

  halEepromConfigureGPIO();

  // Set EEPROM_POWER_PIN high as part of EEPROM init
  configureEepromPowerPin();
  setEepromPowerPin();

  // Ensure the device is ready to access after applying power
  // We delay even if shutdown control isn't used to play it safe
  // since we don't know how quickly init may be called after boot
  halEepromDelayMicroseconds(TIMING_POWERON_MAX_US);

  // Release the chip from powerdown mode
  setFlashCSActive();
  halSpiWrite8(CMD_POWER_UP);
  setFlashCSInactive();

  halEepromDelayMicroseconds(TIMING_POWERON_MAX_US);

  deviceType = getDeviceType();
  if(deviceType == UNKNOWN_DEVICE) {
    return EEPROM_ERR_INVALID_CHIP;
  }
  // For Atmel devices, need to unprotect them because default is protected
  if(deviceType >= ATMEL_4M_DEVICE && deviceType <= ATMEL_8M_DEVICE) {
    setWEL();
    setFlashCSActive();
    halSpiWrite8(CMD_WRITE_STATUS);
    halSpiWrite8(0); // No protect bits set
    setFlashCSInactive();
  }
  return EEPROM_SUCCESS;
}

static bool verifyAddressRange(uint32_t address, uint16_t length,
                                  DeviceType *pDeviceType)
{
  // all parts support addresses less than DEVICE_SIZE_2M
  if( (address + length) <= DEVICE_SIZE_2M )
    return true;

  // if address is greater than DEVICE_SIZE_2M, need to query the chip
  if( (address + length) <= getDeviceSize(pDeviceType) )
    return true;

  // out of range
  return false;
}

void spiWriteCommandAtAddress(uint8_t command, uint32_t address){
  halSpiWrite8(command);
  halSpiPush24MSB(address);
  halSpiPopN8(3);
}

uint8_t halEepromRead(uint32_t address, uint8_t *data, uint16_t totalLength)
{
  if( !verifyAddressRange(address, totalLength, NULL) )
    return EEPROM_ERR_ADDR;

  waitNotBusy();

  setFlashCSActive();
  spiWriteCommandAtAddress(CMD_READ_DATA, address);

  while(totalLength--) {
    *data++ = halSpiRead8();
  }
  setFlashCSInactive();

  return EEPROM_SUCCESS;
}

static bool verifyErased(uint32_t address, uint16_t len)
{
  waitNotBusy();

  setFlashCSActive();
  spiWriteCommandAtAddress(CMD_READ_DATA, address);

  while(len--) {
    if(halSpiRead8() != 0xFF) {
      return false;
    }
  }
  setFlashCSInactive();
  return true;
}

static void writePage(uint32_t address, const uint8_t *data, uint16_t len)
{
  waitNotBusy();
  setWEL();

  setFlashCSActive();
  spiWriteCommandAtAddress(CMD_PAGE_PROG, address);

  while(len--) {
    halSpiWrite8(*data++);
  }
  setFlashCSInactive();
}

uint8_t halEepromWrite(uint32_t address, const uint8_t *data, uint16_t totalLength)
{
  uint32_t nextPageAddr;
  uint16_t len;

  if( !verifyAddressRange(address, totalLength, NULL) )
    return EEPROM_ERR_ADDR;

  if(!verifyErased(address, totalLength)) {
    return EEPROM_ERR_ERASE_REQUIRED;
  }

  if( address & DEVICE_PAGE_MASK) {
    // handle unaligned first block
    nextPageAddr = (address & (~DEVICE_PAGE_MASK)) + DEVICE_PAGE_SIZE;
    if((address + totalLength) < nextPageAddr){
      // fits all within first block
      len = totalLength;
    } else {
      len = (uint16_t) (nextPageAddr - address);
    }
  } else {
    len = (totalLength>DEVICE_PAGE_SIZE)? DEVICE_PAGE_SIZE : totalLength;
  }
  while(totalLength) {
    writePage(address, data, len);
    totalLength -= len;
    address += len;
    data += len;
    len = (totalLength>DEVICE_PAGE_SIZE)? DEVICE_PAGE_SIZE : totalLength;
  }

  return EEPROM_SUCCESS;
}

void halEepromShutdown(void)
{
  // wait for any outstanding operations to complete before pulling the plug
  waitNotBusy();

  // always enter low power mode, even if using shutdown control
  //  since sometimes leakage prevents shutdown control from
  //  completely turning off the part.
  setFlashCSActive();
  halSpiWrite8(CMD_POWER_DOWN);
  setFlashCSInactive();

  // pull power if using shutdown control
  clearEepromPowerPin();
}

uint32_t halEepromSize(void)
{
  return halEepromInfo()->partSize;
}

static void doEraseCmd(uint8_t command, uint32_t address)
{
  waitNotBusy();
  setWEL();
  setFlashCSActive();
  spiWriteCommandAtAddress(command, address);
  setFlashCSInactive();
}

uint8_t halEepromErase(uint32_t address, uint32_t totalLength)
{
  DeviceType deviceType = UNKNOWN_DEVICE;
  uint32_t sectorMask = DEVICE_SECTOR_MASK;
  uint32_t deviceSize = getDeviceSize(&deviceType);
  uint32_t deviceBlockSize = DEVICE_BLOCK_SIZE_64K;
  uint32_t deviceBlockMask = DEVICE_BLOCK_MASK_64K;
  // Numonyx/Micron parts only support block erase, not sector
  if( (deviceType >= NUMONYX_2M_DEVICE)
    &&(deviceType <= NUMONYX_16M_DEVICE) ) {
    sectorMask = DEVICE_BLOCK_MASK_64K;
  }
  else if ( (deviceType >= ISSI_256K_DEVICE)
          &&(deviceType <= ISSI_512K_DEVICE) ) {
    deviceBlockSize = DEVICE_BLOCK_SIZE_32K;
    deviceBlockMask = DEVICE_BLOCK_MASK_32K;
  }
  // Length must be a multiple of the sector size
  if( totalLength & sectorMask )
    return EEPROM_ERR_PG_SZ;
  // Address must be sector aligned
  if( address & sectorMask )
    return EEPROM_ERR_PG_BOUNDARY;
  // Address and length must be in range
  if( !verifyAddressRange(address, totalLength, &deviceType) )
    return EEPROM_ERR_ADDR;

  // Test for full chip erase
  if( (address == 0) && (totalLength == deviceSize) ) {
    waitNotBusy();
    setWEL();
    setFlashCSActive();
    halSpiWrite8(CMD_ERASE_CHIP);
    setFlashCSInactive();
    return EEPROM_SUCCESS;
  }

  // first handle leading partial blocks
  while(totalLength && (address & deviceBlockMask)) {
    doEraseCmd(CMD_ERASE_SECTOR, address);
    address += DEVICE_SECTOR_SIZE;
    totalLength -= DEVICE_SECTOR_SIZE;
  }
  // handle any full blocks
  while(totalLength >= deviceBlockSize) {
    doEraseCmd(CMD_ERASE_BLOCK, address);
    address += deviceBlockSize;
    totalLength -= deviceBlockSize;
  }
  // finally handle any trailing partial blocks
  while(totalLength) {
    doEraseCmd(CMD_ERASE_SECTOR, address);
    address += DEVICE_SECTOR_SIZE;
    totalLength -= DEVICE_SECTOR_SIZE;
  }
  return EEPROM_SUCCESS;
}

bool halEepromBusy(void)
{
  uint8_t status;

  setFlashCSActive();
  halSpiWrite8(CMD_READ_STATUS);
  status = halSpiRead8();
  setFlashCSInactive();
  if( (status & STATUS_BUSY_MASK) == STATUS_BUSY_MASK )
    return true;
  else
    return false;
}
