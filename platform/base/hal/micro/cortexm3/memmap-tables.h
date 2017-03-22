/** @file hal/micro/cortexm3/memmap-tables.h
 * @brief EM300 series memory map address table definitions
 *
 * <!-- Copyright 2009 by Ember Corporation.             All rights reserved.-->
 */
#ifndef __MEMMAP_TABLES_H__
#define __MEMMAP_TABLES_H__

#include "hal/micro/cortexm3/common/ebl.h"

// The start of any EmberZNet image will always contain the following 
// data in flash:
//    Top of stack pointer            [4 bytes]
//    Reset vector                    [4 bytes]
//    NMI handler                     [4 bytes]
//    Hard Fault handler              [4 bytes]
//    Address Table Type              [2 bytes]
//    Address Table Version           [2 bytes]
//    Pointer to full vector table    [4 bytes]
//  Following this will be additional data depending on the address table type

// The address tables take the place of the standard cortex interrupt vector
// tables.  They are designed such that the first entries are the same as the 
// first entries of the cortex vector table.  From there, the address tables
// point to a variety of information, including the location of the full
// cortex vector table, which is remapped to this location in cstartup

// ****************************************************************************
// If any of these address table definitions ever need to change, it is highly
// desirable to only add new entries, and only add them on to the end of an
// existing address table... this will provide the best compatibility with
// any existing code which may utilize the tables, and which may not be able to 
// be updated to understand a new format (example: bootloader which reads the 
// application address table)

#define IMAGE_STAMP_SIZE 8

#define IMAGE_INFO_MAXLEN 32

// Description of the Application Address Table (AAT)
// The application address table recieves somewhat special handling, as the
//   first 128 bytes of it are treated as the EBL header with ebl images.
//   as such, any information required by the bootloader must be present in
//   those first 128 bytes.  Other information that is only used by applications 
//   may follow.
//   Also, some of the fields present within those first 128 bytes are filled
//   in by the ebl generation process, either as part of em3xx_convert or
//   when an s37 is loaded by em3xx_load.  An application using these
//   values should take caution in case the image was loaded in some way that
//   did not fill in the auxillary information.
typedef struct {
  HalBaseAddressTableType baseTable;
  // The following fields are used for ebl and bootloader processing.
  //   See the above description for more information.
  uint8_t platInfo;   // type of platform, defined in micro.h
  uint8_t microInfo;  // type of micro, defined in micro.h
  uint8_t phyInfo;    // type of phy, defined in micro.h
  uint8_t aatSize;    // size of the AAT itself
  uint16_t softwareVersion;  // EmberZNet SOFTWARE_VERSION
  uint16_t softwareBuild;  // EmberZNet EMBER_BUILD_NUMBER
  uint32_t timestamp; // Unix epoch time of .ebl file, filled in by ebl gen
  uint8_t imageInfo[IMAGE_INFO_MAXLEN];  // string, filled in by ebl generation
  uint32_t imageCrc;  // CRC over following pageRanges, filled in by ebl gen
  pageRange8bit_t pageRanges[NUM_AAT_PAGE_RANGES];  // Flash pages used by app.
                                                    // Filled in by ebl gen.
                                                    // 2 bytes per struct 

  void *simeeBottom;  // assumed to be 4 bytes on Cortex M3
  
  uint32_t customerApplicationVersion; // a version field for the customer

  void *internalStorageBottom;  // assumed to be 4 bytes on Cortex M3

  // A non-cryptographic hash of the entire on-chip image,
  // including AAT.  It uses AES-MMO, which is a cryptographic
  // hash, but because the length is only 64-bit there is a
  // greater chance of collisions.  It is not recommended to
  // use this to prove integrity in a cryptographic sense.
  // It is provided as a simple way to verify an EBL file
  // is the same as the one on-chip. 
  uint8_t imageStamp[IMAGE_STAMP_SIZE]; 

  uint8_t familyInfo; // type of family, defined in micro.h

  // reserve the remainder of the first 128 bytes of the AAT in case we need
  //  to go back and add any values that the bootloader will need to reference,
  //  since only the first 128 bytes of the AAT become part of the EBL header.
  uint8_t bootloaderReserved[35 - (NUM_AAT_PAGE_RANGES * sizeof(pageRange8bit_t))];
  
  //////////////
  // Any values after this point are still part of the AAT, but will not
  //   be included as part of the ebl header

  void *debugChannelBottom;
  void *noInitBottom;
  void *appRamTop;
  void *globalTop;
  void *cstackTop;  
  void *initcTop;
  void *codeTop;
  void *cstackBottom;
  void *heapTop;
  void *simeeTop;
  void *debugChannelTop;
} HalAppAddressTableV1Type;

typedef struct {
  HalBaseAddressTableType baseTable;
  // The following fields are used for ebl and bootloader processing.
  //   See the above description for more information.
  uint8_t platInfo;   // type of platform, defined in micro.h
  uint8_t microInfo;  // type of micro, defined in micro.h
  uint8_t phyInfo;    // type of phy, defined in micro.h
  uint8_t aatSize;    // size of the AAT itself
  uint16_t softwareVersion;  // EmberZNet SOFTWARE_VERSION
  uint16_t softwareBuild;  // EmberZNet EMBER_BUILD_NUMBER
  uint32_t timestamp; // Unix epoch time of .ebl file, filled in by ebl gen
  uint8_t imageInfo[IMAGE_INFO_MAXLEN];  // string, filled in by ebl generation
  uint32_t imageCrc;  // CRC over following pageRanges, filled in by ebl gen
  pageRange16bit_t pageRanges[NUM_AAT_PAGE_RANGES];  // Flash pages used by app.
                                                     // Filled in by ebl gen.
                                                     // 4 bytes per struct 

  void *simeeBottom;  // assumed to be 4 bytes on Cortex M3
  
  uint32_t customerApplicationVersion; // a version field for the customer

  void *internalStorageBottom;  // assumed to be 4 bytes on Cortex M3

  // A non-cryptographic hash of the entire on-chip image,
  // including AAT.  It uses AES-MMO, which is a cryptographic
  // hash, but because the length is only 64-bit there is a
  // greater chance of collisions.  It is not recommended to
  // use this to prove integrity in a cryptographic sense.
  // It is provided as a simple way to verify an EBL file
  // is the same as the one on-chip. 
  uint8_t imageStamp[IMAGE_STAMP_SIZE]; 

  uint8_t familyInfo; // type of family, defined in micro.h

  // reserve the remainder of the first 128 bytes of the AAT in case we need
  //  to go back and add any values that the bootloader will need to reference,
  //  since only the first 128 bytes of the AAT become part of the EBL header.
  uint8_t bootloaderReserved[35 - (NUM_AAT_PAGE_RANGES * sizeof(pageRange16bit_t))];
  
  //////////////
  // Any values after this point are still part of the AAT, but will not
  //   be included as part of the ebl header

  void *debugChannelBottom;
  void *noInitBottom;
  void *appRamTop;
  void *globalTop;
  void *cstackTop;  
  void *initcTop;
  void *codeTop;
  void *cstackBottom;
  void *heapTop;
  void *simeeTop;
  void *debugChannelTop;
} HalAppAddressTableV2Type;

#if defined(EMBER_AAT_USE_8BIT_PAGE_NUMBERS) || defined(LOADER)
  typedef HalAppAddressTableV1Type HalAppAddressTableType;
#elif defined(EMBER_AAT_USE_16BIT_PAGE_NUMBERS)
  typedef HalAppAddressTableV2Type HalAppAddressTableType;
#endif

extern const HalAppAddressTableType halAppAddressTable;

// Description of the Bootloader Address Table (BAT)
typedef struct {
  HalBaseAddressTableType baseTable;
  uint16_t bootloaderType;
  uint16_t bootloaderVersion;  
  const HalAppAddressTableType *appAddressTable;
  
  // plat/micro/phy info added in version 0x0104
  uint8_t platInfo;   // type of platform, defined in micro.h
  uint8_t microInfo;  // type of micro, defined in micro.h
  uint8_t phyInfo;    // type of phy, defined in micro.h
  uint8_t reserved;   // reserved for future use
  
  // moved to this location after plat/micro/phy info added in version 0x0104
  void (*eblProcessInit)(EblConfigType *config,
                         void *dataState,
                         uint8_t *tagBuf,
                         uint16_t tagBufLen,
                         bool returnBetweenBlocks);  
  BL_Status (*eblProcess)(const EblDataFuncType *dataFuncs, 
                          EblConfigType *config,
                          const EblFlashFuncType *flashFuncs);
  EblDataFuncType *eblDataFuncs; 
 
  // these eeprom routines happen to be app bootloader specific
  // added in version 0x0105
  uint8_t (*eepromInit)(void);
  uint8_t (*eepromRead)( uint32_t address, uint8_t *data, uint16_t len);
  uint8_t (*eepromWrite)( uint32_t address, uint8_t const *data, uint16_t len);
  void (*eepromShutdown)(void);
  const void *(*eepromInfo)(void);
  uint8_t (*eepromErase)(uint32_t address, uint32_t len);
  bool (*eepromBusy)(void);

  // These variables hold extended version information
  // added in version 0x0109
  uint16_t bootloaderBuild; // the build number associated with bootloaderVersion
  uint16_t reserved2;       // reserved for future use
  uint32_t customerBootloaderVersion; // hold a customer specific bootloader version
  
  //pointer to reset info area?

  // Left for reference.  These items were exposed on the 2xx. Do we want
  //  to add them to the 3xx?
  //void *        bootCodeBaseW;                  // $??CODE_LO
  //uint16_t        bootCodeSizeW;                  // $??CODE_SIZEW
  //void *        bootConstBaseW;                 // $??CONST_LO Relocated
  //uint16_t        bootConstSize;                  // $??CONST_SIZE
  //uint8_t *       bootName;                       //=>const uint8_t bootName[];

} HalBootloaderAddressTableType;

extern const HalBootloaderAddressTableType halBootloaderAddressTable;



// Description of the Ramexe Address Table (RAT)
typedef struct {
  HalBaseAddressTableType baseTable;
  void *startAddress;
  void *endAddress;
} HalRamexeAddressTableType;

extern const HalRamexeAddressTableType halRamAddressTable; 


#define APP_ADDRESS_TABLE_TYPE          (0x0AA7)
#define BOOTLOADER_ADDRESS_TABLE_TYPE   (0x0BA7)
#define RAMEXE_ADDRESS_TABLE_TYPE       (0x0EA7)

#define AAT_MAJOR_VERSION_1             (0x0100)
#define AAT_MAJOR_VERSION_2             (0x0200)

// The current versions of the address tables.
// Note that the major version should be updated only when a non-backwards
// compatible change is introduced (like removing or rearranging fields)
// adding new fields is usually backwards compatible, and their presence can
// be indicated by incrementing only the minor version
#if defined(LOADER)
  #define AAT_MAJOR_VERSION_MIN          AAT_MAJOR_VERSION_1
  #define AAT_MAJOR_VERSION_MAX          AAT_MAJOR_VERSION_2
#else
  #if defined(EMBER_AAT_USE_8BIT_PAGE_NUMBERS)
    #define AAT_VERSION                     (0x0109)
  #elif defined(EMBER_AAT_USE_16BIT_PAGE_NUMBERS)
    #define AAT_VERSION                     (0x0201)
  #endif

  #define AAT_MAJOR_VERSION              (AAT_VERSION & AAT_MAJOR_VERSION_MASK)
#endif
#define AAT_MAJOR_VERSION_MASK          (0xFF00)

// *** AAT Version history: ***
//0x0201 - Changes page numbers from 8 bits each to 16.
//0x0109 - Added family information (only used for EFR32)
//0x0108 - Add the simeeTop to the AAT so that we can place the simee wherever
//         we want to. This change also adds a pointer to the internalStorage
//         region to the beginning of the AAT so that bootloaders can confirm
//         that they have the same concept of internalStorage as the app.
//         Lastly, this version now includes the top of the debug channel so
//         that we can support unused RAM living above the debug channel.
//0x0107 - Changed a reserved field to the software build number and added a
//         customer application version to the first 128 bytes of the AAT.
//0x0106 - Moved everything from APP_RAM into the heap. Now the value stored
//         in appRamTop is not the APP_RAM's top. Convert needs to know this
//         to correctly compute the space being used for configuration data.
//0x0105 - Added stack and heap information now that we have a stack and
//         heap that can grow towards each other.
//0x0104 - Flash/ram usage information added for em3xx_convert
//0x0103 - Added debugchannel shared memory pointer to AAT
//0x0102 - Combined AAT and EBL header
//0x0101 - Initial version

#define BAT_NULL_VERSION                (0x0000)
#define BAT_VERSION                     (0x010A)
#define BAT_MAJOR_VERSION               (0x0100)
#define BAT_MAJOR_VERSION_MASK          (0xFF00)

// *** BAT Version history: ***
// 0x010A - First version to support IBRs.
// 0x0109 - Added the bootloader build number and a customer bootloader version
//          field to help track their versions as well.
// 0x0108 - Added halEepromInfo(), halEepromErase(), and halEepromBusy() APIs
//          Extended halEepromInit to return a status in case of failure
// 0x0107 - Added halEepromShutdown() API, Added support for reading/writing 
//          arbitrary addresses in halEepromRead/Write() APIs
// 0x0106 - Standalone bootloader ota support aded
// 0x0105 - Add function pointers for shared app bootloader dataflash drivers
// 0x0104 - PLAT/MICRO/PHY type information added
// 0x0103 - Add function pointers for shared ebl processing code part 2
// 0x0102 - Add function pointers for shared ebl processing code part 1
// 0x0101 - Initial version

#define BAT_MIN_IBR_VERSION             (0x010A)

#define RAT_VERSION                     (0x0101)

#include "memmap-fat.h"

#endif //__MEMMAP_TABLES_H__
