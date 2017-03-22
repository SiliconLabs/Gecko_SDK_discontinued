/** @file hal/micro/bootloader-eeprom.h
 * See @ref cbh_app for detailed documentation.
 * 
 * <!-- Copyright 2009 by Ember Corporation. All rights reserved.       *80* -->
 */

/** @addtogroup cbh_app
 * The file bootloader-eeprom.h defines generic EEPROM parameters. 
 *
 * Changing EEPROM size will change the size of the application image 
 * space without changing the size or relative location of the recovery and 
 * reserved sections. See eeprom.c for more information on modifying EEPROM
 * functionality.
 *
 * See bootloader-eeprom.h for source code.
 *@{
 */
#ifndef __BOOTLOADER_EEPROM_H__
#define __BOOTLOADER_EEPROM_H__

/** @brief Definition of an EEPROM page size, in bytes.  This definition is 
 *  deprecated, and should no longer be used.
 */
#define EEPROM_PAGE_SIZE      (128ul)

/** @brief Define the location of the first page in EEPROM.  This definition is 
 *  deprecated, and should no longer be used.
 */
#define EEPROM_FIRST_PAGE   (0)

/** @brief Define the location of the image start in EEPROM as a function of
 * the ::EEPROM_FIRST_PAGE and ::EEPROM_PAGE_SIZE.  This definition is 
 * deprecated, and should no longer be used.
 */
#define EEPROM_IMAGE_START  (EEPROM_FIRST_PAGE*EEPROM_PAGE_SIZE)

/** @brief Define EEPROM success status.
 */
#define EEPROM_SUCCESS 0

/** @brief Define EEPROM error status.
 */
#define EEPROM_ERR 1

/** @brief Define EEPROM error mask.
 */
#define EEPROM_ERR_MASK 0x80

/** @brief Define EEPROM page boundary error.
 */
#define EEPROM_ERR_PG_BOUNDARY 0x81

/** @brief Define EEPROM page size error.
 */
#define EEPROM_ERR_PG_SZ 0x82

/** @brief Define EEPROM write data error.
 */
#define EEPROM_ERR_WRT_DATA 0x83

/** @brief Define EEPROM image too large error.
 */
#define EEPROM_ERR_IMG_SZ 0x84

/** @brief Define EEPROM invalid address error.
 */
#define EEPROM_ERR_ADDR 0x85

/** @brief Define EEPROM chip initialization error.
 */
#define EEPROM_ERR_INVALID_CHIP 0x86

/** @brief Define EEPROM erase required error.
 */
#define EEPROM_ERR_ERASE_REQUIRED 0x87

/** @brief Define EEPROM error for no erase support.
 */
#define EEPROM_ERR_NO_ERASE_SUPPORT 0x88

/** @name EEPROM interaction functions.
 *@{
 */

/** @brief Initialize EEPROM.
 *         Note: some earlier drivers may assert instead of returning an error
 *          if initialization fails.
 *
 *  @return ::EEPROM_SUCCESS or ::EEPROM_ERR_INVALID_CHIP
 */
uint8_t halEepromInit(void);

/** @brief Shutdown the EEPROM to conserve power
 */
void halEepromShutdown(void);

/** @brief This structure defines a variety of information about the attached
 *          external EEPROM device.
 */
typedef struct {
  /** The version of this data structure */
  uint16_t version;
  /** A bitmask describing the capabilites of this particular external EEPROM */
  uint16_t capabilitiesMask;
  /** Maximum time it takes to erase a page. (in 1024Hz Milliseconds) */
  uint16_t pageEraseMs;
  /** Maximum time it takes to erase the entire part. (in 1024Hz Milliseconds) */
  uint16_t partEraseMs;
  /** The size of a single erasable page in bytes */
  uint32_t pageSize;
  /** The total size of the external EEPROM in bytes */
  uint32_t partSize;
  /** Pointer to a string describing the attached external EEPROM */
  const char * const partDescription;
  /** The number of bytes in a word for the external EEPROM **/
  uint8_t wordSizeBytes;
} HalEepromInformationType;

/** @brief The current version of the ::HalEepromInformationType data structure
 */
#define EEPROM_INFO_VERSION             (0x0102)
#define EEPROM_INFO_MAJOR_VERSION       (0x0100)
#define EEPROM_INFO_MAJOR_VERSION_MASK  (0xFF00)
// ***  Eeprom info version history: ***
// 0x0102 - Added a word size field to specify the number of bytes per flash
//          word in the EEPROM. Writes should always be aligned to the word
//          size and have a length that is a multiple of the word size.
// 0x0101 - Initial version
#define EEPROM_INFO_MIN_VERSION_WITH_WORD_SIZE_SUPPORT 0x0102

/** @brief Eeprom capabilites mask that indicates the erase API is supported
 */
#define EEPROM_CAPABILITIES_ERASE_SUPPORTED   (0x0001)

/** @brief Eeprom capabilites mask that indicates page erasing is required
 *          before new data can be written to a device
 */
#define EEPROM_CAPABILITIES_PAGE_ERASE_REQD   (0x0002)

/** @brief Eeprom capabilites mask that indicates that the write routine
 *          is blocking on this device
 */
#define EEPROM_CAPABILITIES_BLOCKING_WRITE    (0x0004)

/** @brief Eeprom capabilites mask that indicates that the erase routine
 *          is blocking on this device
 */
#define EEPROM_CAPABILITIES_BLOCKING_ERASE    (0x0008)

/** @brief Call this function to get information about the external EEPROM
 *          and its capabilities.
 *
 *         The format of this call must not be altered. However, the content 
 *          can be changed to work with a different device. 
 *
 *  @return A pointer to a HalEepromInformationType data structure, or NULL
 *           if the driver does not support this API
 */
const HalEepromInformationType *halEepromInfo(void);

/** @brief Return the size of the EEPROM.
 *
 *         The format of this call must not be altered. However, the content 
 *          can be changed to work with a different device. 
 *         Internal use only. No exposure to application
 *
 *  @return int32_t size
 */
uint32_t halEepromSize(void);

/** @brief Determine if the exernal EEPROM is still busy performing 
 *          the last operation, such as a write or an erase.
 *
 *         The format of this call must not be altered. However, the content 
 *          can be changed to work with a different device. 
 *
 *  @return true if still busy or false if not.
 */
bool halEepromBusy(void);

/** @brief Read from the external EEPROM. 
 *
 * This is the standard external EEPROM read function. The format of this
 * call must not be altered. However, the content can be changed to work with a
 * different device. 
 *         Note: Not all storage implementations support accesses that are
 *               not page aligned, refer to the HalEepromInformationType
 *               structure for more information.
 *
 * @param address  The address to start reading from.
 * @param data     A pointer to where read data is stored.
 * @param len      The length of data to read.
 * 
 * @return ::EEPROM_SUCCESS or ::EEPROM_ERR
 */
uint8_t halEepromRead(uint32_t address, uint8_t *data, uint16_t len);

/** @brief Write to the external EEPROM. 
 *
 * This is the standard external EEPROM write function. The format of this
 * call must not be altered. However, the content can be changed to work with a
 * different device. 
 *         Note: Not all storage implementations support accesses that are
 *               not page aligned, refer to the HalEepromInformationType
 *               structure for more information.
 *         Note: Some storage devices require contents to be erased before
 *               new data can be written, and will return an 
 *               ::EEPROM_ERR_ERASE_REQUIRED error if write is called on a
 *               location that is not already erased. Refer to the 
 *               HalEepromInformationType structure to see if the attached
 *               storage device requires erasing.
 *
 * @param address  The address to start writing to.
 * @param data     A pointer to the data to write.
 * @param len      The length of data to write.
 * 
 * @return EEPROM_SUCCESS or ::EEPROM_ERR
 */
uint8_t halEepromWrite(uint32_t address, const uint8_t *data, uint16_t len);

/** @brief Erases the specified region of the external EEPROM. 
 *
 *         The format of this call must not be altered. However, the content 
 *          can be changed to work with a different device. 
 *         Note: Most devices require the specified region to be page aligned, 
 *          and will return an error if an unaligned region is specified.
 *         Note: Many devices take an extremely long time to perform an erase
 *          operation.  When erasing a large region, it may be preferable to 
 *          make multiple calls to this API so that other application
 *          functionality can be performed while the erase is in progress.
 *          The ::halEepromBusy() API may be used to determine
 *          when the last erase operation has completed.  Erase timing
 *          information can be found in the HalEepromInformationType structure.
 *
 *  @param address  Address to start erasing
 *
 *  @param len      Length of the region to be erased
 *
 *  @return ::EEPROM_SUCCESS or ::EEPROM_ERR. 
 */
uint8_t halEepromErase(uint32_t address, uint32_t totalLength);


#ifndef DOXYGEN_SHOULD_SKIP_THIS

// This structure holds information about where we left off when last accessing
// the eeprom.
typedef struct {
  uint32_t address;
  uint16_t pages;
  uint16_t pageBufFinger;
  uint16_t pageBufLen;
  uint8_t pageBuf[EEPROM_PAGE_SIZE];
} EepromStateType;

#endif //DOXYGEN_SHOULD_SKIP_THIS

/**@} */

#endif //__BOOTLOADER_EEPROM_H__

/** @} END addtogroup */
