/** @file hal/micro/cortexm3/memmap.h
 *
 * @brief
 * Memory map definitions
 *
 * THIS IS A GENERATED FILE.  DO NOT EDIT.
 *
 * <!-- Copyright 2016 Silicon Laboratories, Inc.                        *80*-->
 */

#ifndef __MEMMAP_H__
#define __MEMMAP_H__

#if defined(LOADER)
  // Loader does not include any chip specific memmap.h
#elif defined(CORTEXM3_OBSOLETE_CORTEXM3_28)
  #include "hal/micro/cortexm3/obsolete_cortexm3_28/memmap.h"
#elif defined(CORTEXM3_OBSOLETE_CORTEXM3_29)
  #include "hal/micro/cortexm3/obsolete_cortexm3_29/memmap.h"
#elif defined(CORTEXM3_OBSOLETE_CORTEXM3_7)
  #include "hal/micro/cortexm3/obsolete_cortexm3_7/memmap.h"
#elif defined(CORTEXM3_EFR32)
  #include "hal/micro/cortexm3/efm32/memmap.h"
#elif defined(CORTEXM3_EZR32HG)
  #include "hal/micro/cortexm3/efm32/memmap.h"
#elif defined(CORTEXM3_EZR32LG)
  #include "hal/micro/cortexm3/efm32/memmap.h"
#elif defined(CORTEXM3_EZR32WG)
  #include "hal/micro/cortexm3/efm32/memmap.h"
#elif defined(CORTEXM3_EM317)
  #include "hal/micro/cortexm3/em35x/em317/memmap.h"
#elif defined(CORTEXM3_EM341)
  #include "hal/micro/cortexm3/em35x/em341/memmap.h"
#elif defined(CORTEXM3_EM342)
  #include "hal/micro/cortexm3/em35x/em342/memmap.h"
#elif defined(CORTEXM3_EM346)
  #include "hal/micro/cortexm3/em35x/em346/memmap.h"
#elif defined(CORTEXM3_EM351)
  #include "hal/micro/cortexm3/em35x/em351/memmap.h"
#elif defined(CORTEXM3_EM355)
  #include "hal/micro/cortexm3/em35x/em355/memmap.h"
#elif defined(CORTEXM3_EM3555)
  #include "hal/micro/cortexm3/em35x/em3555/memmap.h"
#elif defined(CORTEXM3_EM356)
  #include "hal/micro/cortexm3/em35x/em356/memmap.h"
#elif defined(CORTEXM3_EM357) || defined(EMBER_TEST)
  #include "hal/micro/cortexm3/em35x/em357/memmap.h"
#elif defined(CORTEXM3_EM3581)
  #include "hal/micro/cortexm3/em35x/em3581/memmap.h"
#elif defined(CORTEXM3_EM3582)
  #include "hal/micro/cortexm3/em35x/em3582/memmap.h"
#elif defined(CORTEXM3_EM3585)
  #include "hal/micro/cortexm3/em35x/em3585/memmap.h"
#elif defined(CORTEXM3_EM3586)
  #include "hal/micro/cortexm3/em35x/em3586/memmap.h"
#elif defined(CORTEXM3_EM3587)
  #include "hal/micro/cortexm3/em35x/em3587/memmap.h"
#elif defined(CORTEXM3_EM3588)
  #include "hal/micro/cortexm3/em35x/em3588/memmap.h"
#elif defined(CORTEXM3_EM359)
  #include "hal/micro/cortexm3/em35x/em359/memmap.h"
#elif defined(CORTEXM3_EM3591)
  #include "hal/micro/cortexm3/em35x/em3591/memmap.h"
#elif defined(CORTEXM3_EM3592)
  #include "hal/micro/cortexm3/em35x/em3592/memmap.h"
#elif defined(CORTEXM3_EM3595)
  #include "hal/micro/cortexm3/em35x/em3595/memmap.h"
#elif defined(CORTEXM3_EM3596)
  #include "hal/micro/cortexm3/em35x/em3596/memmap.h"
#elif defined(CORTEXM3_EM3597)
  #include "hal/micro/cortexm3/em35x/em3597/memmap.h"
#elif defined(CORTEXM3_EM3598)
  #include "hal/micro/cortexm3/em35x/em3598/memmap.h"
#elif defined(CORTEXM3_SKY66107)
  #include "hal/micro/cortexm3/em35x/sky66107/memmap.h"
#else
  #error Unknown chip or build
#endif

//=============================================================================
// A union that describes the entries of the vector table.  The union is needed
// since the first entry is the stack pointer and the remainder are function
// pointers.
//
// Normally the vectorTable below would require entries such as:
//   { .topOfStack = x },
//   { .ptrToHandler = y },
// But since ptrToHandler is defined first in the union, this is the default
// type which means we don't need to use the full, explicit entry.  This makes
// the vector table easier to read because it's simply a list of the handler
// functions.  topOfStack, though, is the second definition in the union so
// the full entry must be used in the vectorTable.
//=============================================================================
typedef union
{
  void (*ptrToHandler)(void);
  void *topOfStack;
} HalVectorTableType;


// ****************************************************************************
// If any of these address table definitions ever need to change, it is highly
// desirable to only add new entries, and only add them on to the end of an
// existing address table... this will provide the best compatibility with
// any existing code which may utilize the tables, and which may not be able to 
// be updated to understand a new format (example: bootloader which reads the 
// application address table)

// Generic Address table definition which describes leading fields which
// are common to all address table types
typedef struct {
  void *topOfStack;
  void (*resetVector)(void);
  void (*nmiHandler)(void);
  void (*hardFaultHandler)(void);
  uint16_t type;
  uint16_t version;
  const HalVectorTableType *vectorTable;
  // Followed by more fields depending on the specific address table type
} HalBaseAddressTableType;

// If page number width wasn't specified, we pick a reasonable default
#if !defined(EMBER_AAT_USE_8BIT_PAGE_NUMBERS) \
     && !defined(EMBER_AAT_USE_16BIT_PAGE_NUMBERS) \
     && !defined(LOADER)
  #if (MFB_SIZE_B / MFB_PAGE_SIZE_B) > 256
    // If the part we are building for has more than 256 pages in the MFB.
    // default to 16 bits.
    #define EMBER_AAT_USE_16BIT_PAGE_NUMBERS
  #else
    // For now, default to 8 bits on smaller parts. This may change soon.
    #define EMBER_AAT_USE_8BIT_PAGE_NUMBERS
  #endif
#endif

typedef uint8_t AatPageNumber8bit_t;
#define UNUSED_AAT_8BIT_PAGE_NUMBER     0xFF

typedef uint16_t AatPageNumber16bit_t;
#define UNUSED_AAT_16BIT_PAGE_NUMBER    0xFFFF

typedef struct      pageRange8bit_s
{
  AatPageNumber8bit_t   begPg;          /* First flash page in range    */
  AatPageNumber8bit_t   endPg;          /* Last  flash page in range    */
}                   pageRange8bit_t;

typedef struct      pageRange16bit_s
{
  AatPageNumber16bit_t   begPg;          /* First flash page in range    */
  AatPageNumber16bit_t   endPg;          /* Last  flash page in range    */
}                   pageRange16bit_t;

#if defined(EMBER_AAT_USE_8BIT_PAGE_NUMBERS)
  typedef AatPageNumber8bit_t AatPageNumber_t;
  typedef pageRange8bit_t pageRange_t;
  #define UNUSED_AAT_PAGE_NUMBER        UNUSED_AAT_8BIT_PAGE_NUMBER
#elif defined(EMBER_AAT_USE_16BIT_PAGE_NUMBERS)
  typedef AatPageNumber16bit_t AatPageNumber_t;
  typedef pageRange16bit_t pageRange_t;
  #define UNUSED_AAT_PAGE_NUMBER        UNUSED_AAT_16BIT_PAGE_NUMBER
#endif

#define NUM_AAT_PAGE_RANGES       6

#ifdef MINIMAL_HAL
  // Minimal hal only references the FAT
  #include "memmap-fat.h"
#else
  // Full hal references all address tables
  #include "memmap-tables.h"
#endif

#endif // __MEMMAP_H__
