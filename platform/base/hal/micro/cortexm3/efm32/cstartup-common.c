//=============================================================================
// FILE
//   cstartup.c - Startup and low-level utility code for Ember's Cortex based
//                SOCs when using the IAR toolchain.
//
// DESCRIPTION
//   This file defines the basic information needed to go from reset up to
//   the main() found in C code.
//
//   Copyright 2013 Silicon Laboratories, Inc.                             *80*
//=============================================================================

#include PLATFORM_HEADER
#include "hal/micro/cortexm3/diagnostic.h"
#include "hal/micro/cortexm3/efm32/mpu.h"
#include "hal/micro/micro.h"
#include "hal/micro/cortexm3/memmap.h"
#include "hal/micro/cortexm3/cstartup-common.h"
#include "hal/micro/cortexm3/internal-storage.h"

#include "stack/include/ember-types.h"
#include "hal/micro/bootloader-interface.h"
#include "em_device.h"
#include "em_rmu.h"

// TODO: comments in this file relate to the em3xx instead of efr32. Should
// be cleaned up.

// Pull in the SOFTWARE_VERSION and EMBER_BUILD_NUMBER from the stack
#include "stack/config/config.h"

// Define the CUSTOMER_APPLICATION_VERSION if it wasn't set
#ifndef CUSTOMER_APPLICATION_VERSION
  #define CUSTOMER_APPLICATION_VERSION 0
#endif
// Define the CUSTOMER_BOOTLOADER_VERSION if it wasn't set
#ifndef CUSTOMER_BOOTLOADER_VERSION
  #define CUSTOMER_BOOTLOADER_VERSION 0
#endif

// Verify the various bootloader options that may be specified.  Use of some
//   options is now deprecated and will be removed in a future release.
// On the 35x platform, the use of these options is only important to specify
//   the size of the bootloader, rather than the bootloader type.
// By default, the lack of any option will indicate an 8k bootloader size
// The NULL_BTL option indicates no bootloader is used.
#ifdef APP_BTL
  #pragma message("The APP_BTL build option is deprecated.  Removing this option will build for any 16k bootloader type.")
#endif
#ifdef SERIAL_UART_BTL
  #pragma message("The SERIAL_UART_BTL build option is deprecated.  Removing this option will build for any 16k bootloader type.")
#endif
#ifdef SERIAL_OTA_BTL
  #pragma message("The SERIAL_UART_OTA build option is deprecated.  Removing this option will build for any 16k bootloader type.")
#endif
#ifdef NULL_BTL
  // Fully supported, no error
#endif
#ifdef SMALL_BTL
  #error SMALL_BTL is not supported
#endif

#if defined GECKO_INFO_PAGE_BTL \
    || defined APP_GECKO_INFO_PAGE_BTL \
    || defined STA_GECKO_INFO_PAGE_BTL \
    || defined LOCAL_STORAGE_GECKO_INFO_PAGE_BTL
  #define NO_BAT
#endif


//=============================================================================
// Define the size of the call stack and define a block of memory for it.
//
// Place the cstack area in a segment named CSTACK.  This segment is
// defined soley for the purpose of placing the stack.  Refer to reset handler
// for the initialization code and iar-cfg-common.icf for segment placement
// in memory.
//
// halResetInfo, used to store crash information and bootloader parameters, is
// overlayed on top of the base of this segment so it can be overwritten by the
// call stack.
// This assumes that the stack will not go that deep between reset and
// use of the crash or the bootloader data.
//=============================================================================
#ifndef CSTACK_SIZE
  #ifdef RTOS
    // The RTOS will handle the actual CSTACK sizing per-task, but we must
    // still allocate some space for startup and exceptions.
    #define CSTACK_SIZE (128)  // *4 = 512 bytes
  #else
    #if (! defined(EMBER_STACK_IP))
      // Pro Stack

    // Right now we define the stack size to be for the worst case scenario,
    // ECC.  The ECC 163k1 library  and the ECC 283k1 Library both use the stack 
    // for calculations. Empirically I have seen it use as much as 1900 bytes 
    // for the 'key bit generate' operation.
    // So we add a 25% buffer: 1900 * 1.25 = 2375
    #define CSTACK_SIZE  (600)  // *4 = 2400 bytes

    #else
      // IP Stack
      #define CSTACK_SIZE (950) // *4 = 3800 bytes
    #endif // !EMBER_STACK_IP
  #endif
#endif
VAR_AT_SEGMENT(NO_STRIPPING uint32_t cstackMemory[CSTACK_SIZE], __CSTACK__);

#ifndef HTOL_EM3XX
  // Create an array to hold space for the guard region. Do not actually use this
  // array in code as we will move the guard region around programatically. This
  // is only here so that the linker takes into account the size of the guard
  // region when configuring the RAM.
  ALIGNMENT(HEAP_GUARD_REGION_SIZE_BYTES)  
  VAR_AT_SEGMENT(NO_STRIPPING uint8_t guardRegionPlaceHolder[HEAP_GUARD_REGION_SIZE_BYTES], __GUARD_REGION__);
#endif

// Reset cause and crash info live in a special RAM segment that is
// not modified during startup.  This segment is overlayed on top of the
// bottom of the cstack.
VAR_AT_SEGMENT(NO_STRIPPING HalResetInfoType halResetInfo, __RESETINFO__);

// If space is needed in the flash for data storage like for the local storage
// bootloader then create an array here to hold a place for this data.
#if INTERNAL_STORAGE_SIZE_B > 0
  // Define the storage region as an uninitialized array in the
  // __INTERNAL_STORAGE__ region which the linker knows how to place.  
  VAR_AT_SEGMENT(NO_STRIPPING uint8_t internalStorage[INTERNAL_STORAGE_SIZE_B], __INTERNAL_STORAGE__);
#endif

//=============================================================================
// Declare the address tables which will always live at well known addresses
//=============================================================================
#ifdef NULL_BTL
// In the case of a NULL_BTL application, we define a dummy BAT
VAR_AT_SEGMENT(NO_STRIPPING const HalBootloaderAddressTableType halBootloaderAddressTable, __BAT_INIT__) = {
  { _CSTACK_SEGMENT_END,
    halEntryPoint,
    halNmiIsr,
    halHardFaultIsr,
    BOOTLOADER_ADDRESS_TABLE_TYPE,
    BAT_NULL_VERSION,
    NULL                    // No other vector table.
  },
  BL_EXT_TYPE_NULL,           //uint16_t bootloaderType;
  BOOTLOADER_INVALID_VERSION, //uint16_t bootloaderVersion;
  &halAppAddressTable,
  PLAT,   //uint8_t platInfo;   // type of platform, defined in micro.h
  MICRO,  //uint8_t microInfo;  // type of micro, defined in micro.h
  PHY,    //uint8_t phyInfo;    // type of phy, defined in micro.h
  0,      //uint8_t reserved;   // reserved for future use
  NULL,   // eblProcessInit
  NULL,   // eblProcess
  NULL,   // eblDataFuncs
  NULL,   // eepromInit
  NULL,   // eepromRead
  NULL,   // eepromWrite
  NULL,   // eepromShutdown
  NULL,   // eepromInfo
  NULL,   // eepromErase
  NULL,   // eepromBusy
  EMBER_BUILD_NUMBER, // uint16_t softwareBuild;
  0,                  // uint16_t reserved2;
  CUSTOMER_BOOTLOADER_VERSION  // uint32_t customerBootloaderVersion;
};
#elif !defined NO_BAT
// otherwise we just define a variable that maps to the real bootloader BAT
VAR_AT_SEGMENT(NO_STRIPPING __no_init const HalBootloaderAddressTableType halBootloaderAddressTable, __BAT__);
#endif

VAR_AT_SEGMENT(NO_STRIPPING const HalAppAddressTableType halAppAddressTable, __AAT__) = {
  { _CSTACK_SEGMENT_END,
    halEntryPoint,
    halNmiIsr,
    halHardFaultIsr,
    APP_ADDRESS_TABLE_TYPE,
    AAT_VERSION,
    __vector_table
  },
  PLAT,  //uint8_t platInfo;   // type of platform, defined in micro.h
  MICRO, //uint8_t microInfo;  // type of micro, defined in micro.h
  PHY,   //uint8_t phyInfo;    // type of phy, defined in micro.h
  sizeof(HalAppAddressTableType),  // size of aat itself
  SOFTWARE_VERSION,   // uint16_t softwareVersion
  EMBER_BUILD_NUMBER, // uint16_t softwareBuild
  0,  //uint32_t timestamp; // Unix epoch time of .ebl file, filled in by ebl gen
  "", //uint8_t imageInfo[IMAGE_INFO_SZ];  // string, filled in by ebl generation
  0,  //uint32_t imageCrc;  // CRC over following pageRanges, filled in by ebl gen
  { {UNUSED_AAT_PAGE_NUMBER, UNUSED_AAT_PAGE_NUMBER},   //pageRange_t pageRanges[NUM_AAT_PAGE_RANGES];
    {UNUSED_AAT_PAGE_NUMBER, UNUSED_AAT_PAGE_NUMBER},   // Flash pages used by app, filled in by ebl gen
    {UNUSED_AAT_PAGE_NUMBER, UNUSED_AAT_PAGE_NUMBER},
    {UNUSED_AAT_PAGE_NUMBER, UNUSED_AAT_PAGE_NUMBER},
    {UNUSED_AAT_PAGE_NUMBER, UNUSED_AAT_PAGE_NUMBER},
    {UNUSED_AAT_PAGE_NUMBER, UNUSED_AAT_PAGE_NUMBER}
  },
  _SIMEE_SEGMENT_BEGIN,                               //void *simeeBottom;
  CUSTOMER_APPLICATION_VERSION,                       //uint32_t customerApplicationVersion;
  _INTERNAL_STORAGE_SEGMENT_BEGIN,                    //void *internalStorageBottom;
  { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0XFF }, // image stamp (filled in by em3xx_convert)
  FAMILY,                                             //uint8_t familyInfo; (defined in micro.h)
  { 0 },                                              //uint8_t bootloaderReserved[] (zero fill, previously was 0xFF fill)
  _DEBUG_CHANNEL_SEGMENT_BEGIN,                       //void *debugChannelBottom;
  _NO_INIT_SEGMENT_BEGIN,                             //void *noInitBottom;
  _BSS_SEGMENT_END,                                   //void *appRamTop; NO LONGER USED! (set to __BSS__ for 3xx convert)
  _BSS_SEGMENT_END,                                   //void *globalTop;
  _CSTACK_SEGMENT_END,                                //void *cstackTop;
  _DATA_INIT_SEGMENT_END,                             //void *initcTop;
  _TEXT_SEGMENT_END,                                  //void *codeTop;
  _CSTACK_SEGMENT_BEGIN,                              //void *cstackBottom;
  _EMHEAP_OVERLAY_SEGMENT_END,                        //void *heapTop;
  _SIMEE_SEGMENT_END,                                 //void *simeeTop;
  _DEBUG_CHANNEL_SEGMENT_END                          //void *debugChannelTop;
};



//=============================================================================
// Define the vector table as a HalVectorTableType.  NO_STRIPPING ensures the
// compiler will not strip the table.  const ensures the table is placed into 
// flash. The VAR_AT_SEGMENT() macro tells the compiler/linker to place the
// vector table in the INTVEC segment which holds the reset/interrupt vectors
// at address 0x00000000.
//
// All Handlers point to a corresponding ISR.  The ISRs are prototyped above.
// The file isr-stubs.s79 provides a weak definition for all ISRs.  To
// "register" its own ISR, an application simply has to define the function
// and the weak stub will be overridden.
//
// The list of handlers are extracted from the NVIC configuration file.  The
// order of the handlers in the NVIC configuration file is critical since it
// translates to the order they are placed into the vector table here.
//=============================================================================
VAR_AT_SEGMENT(NO_STRIPPING const HalVectorTableType __vector_table[], __INTVEC__) =
{
  { .topOfStack = _CSTACK_SEGMENT_END },
  #ifndef INTERRUPT_DEBUGGING
    #define EXCEPTION(vectorNumber, functionName, priorityLevel, subpriority) \
      functionName,
  #else //INTERRUPT_DEBUGGING
    // The interrupt debug behavior inserts a special shim handler before
    // the actual interrupt.  The shim handler then redirects to the
    // actual table, defined below
    #define EXCEPTION(vectorNumber, functionName, priorityLevel, subpriority) \
      halInternalIntDebuggingIsr,
    // PERM_EXCEPTION is used for any vectors that cannot be redirected
    // throught the shim handler.  (such as the reset vector)
    #define PERM_EXCEPTION(vectorNumber, functionName, priorityLevel) \
      functionName,
  #endif //INTERRUPT_DEBUGGING
  #include NVIC_CONFIG
  #undef  EXCEPTION
  #undef PERM_EXCEPTION
};

// halInternalClassifyReset() records the cause of the last reset and any 
// assert information here. If the last reset was not due to an assert,
// the saved assert filename and line number will be NULL and 0 respectively.
static uint16_t savedResetCause;
static HalAssertInfoType savedAssertInfo;

void halInternalClassifyReset(void)
{
  // Table used to convert from RESET_EVENT register bits to reset types
  static const uint16_t resetEventTable[] = {
  #ifdef _EZR_DEVICE
    RESET_POWERON_HV,                  // bit  0: PORST
    RESET_BROWNOUT_UNREGPOWER,         // bit  1: BODUNREGRST
    RESET_BROWNOUT_REGPOWER,           // bit  2: BODREGRST
    RESET_EXTERNAL_PIN,                // bit  3: EXTRST
    RESET_WATCHDOG_EXPIRED,            // bit  4: WDOGRST
    RESET_FATAL_LOCKUP,                // bit  5: LOCKUPRST
    RESET_SOFTWARE,                    // bit  6: SYSREQRST
    RESET_SOFTWARE_EM4,                // bit  7: EM4RST
    RESET_EXTERNAL_EM4PIN,             // bit  8: EM4WURST
    RESET_BROWNOUT_AVDD0,              // bit  9: BODAVDD0
    RESET_BROWNOUT_AVDD1,              // bit 10: BODAVDD1
  #elif defined _EFR_DEVICE
    RESET_POWERON_HV,                  // bit  0: PORST
    RESET_UNKNOWN_UNKNOWN,             // bit  1: RESERVED
    RESET_BROWNOUT_AVDD,               // bit  2: AVDDBOD
    RESET_BROWNOUT_DVDD,               // bit  3: DVDDBOD
    RESET_BROWNOUT_DEC,                // bit  4: DECBOD
    RESET_UNKNOWN_UNKNOWN,             // bit  5: RESERVED
    RESET_UNKNOWN_UNKNOWN,             // bit  6: RESERVED
    RESET_UNKNOWN_UNKNOWN,             // bit  7: RESERVED
    RESET_EXTERNAL_PIN,                // bit  8: EXTRST
    RESET_FATAL_LOCKUP,                // bit  9: LOCKUPRST
    RESET_SOFTWARE,                    // bit 10: SYSREQRST
    RESET_WATCHDOG_EXPIRED,            // bit 11: WDOGRST
    RESET_UNKNOWN_UNKNOWN,             // bit 12: RESERVED
    RESET_UNKNOWN_UNKNOWN,             // bit 13: RESERVED
    RESET_UNKNOWN_UNKNOWN,             // bit 14: RESERVED
    RESET_UNKNOWN_UNKNOWN,             // bit 15: RESERVED
    RESET_SOFTWARE_EM4,                // bit 16: EM4RST
  #endif
  };
  
  uint32_t resetEvent = RMU_ResetCauseGet();
  RMU_ResetCauseClear();
  uint16_t cause = RESET_UNKNOWN;
  uint16_t i;

  for (i = 0; i < sizeof(resetEventTable)/sizeof(resetEventTable[0]); i++) {
    if (resetEvent & (1 << i)) {
      cause = resetEventTable[i];
      break;
    }
  }

  if (cause == RESET_SOFTWARE) {
    if((halResetInfo.crash.resetSignature == RESET_VALID_SIGNATURE) &&
       (RESET_BASE_TYPE(halResetInfo.crash.resetReason) < NUM_RESET_BASE_TYPES)) {
      // The extended reset cause is recovered from RAM
      // This can be trusted because the hardware reset event was software
      //  and additionally because the signature is valid
      savedResetCause = halResetInfo.crash.resetReason;
    } else {
      savedResetCause = RESET_SOFTWARE_UNKNOWN;
    }
    // mark the signature as invalid
    halResetInfo.crash.resetSignature = RESET_INVALID_SIGNATURE;
  } else if (    (cause == RESET_BOOTLOADER_DEEPSLEEP)
              && (halResetInfo.crash.resetSignature == RESET_VALID_SIGNATURE)
              && (halResetInfo.crash.resetReason == RESET_BOOTLOADER_DEEPSLEEP)) {
    // Save the crash info for bootloader deep sleep (even though it's not used
    // yet) and invalidate the resetSignature.
    halResetInfo.crash.resetSignature = RESET_INVALID_SIGNATURE;
    savedResetCause = halResetInfo.crash.resetReason;
  } else {
    savedResetCause = cause;
  }
  // If the last reset was due to an assert, save the assert info.
  if (savedResetCause == RESET_CRASH_ASSERT) {
    savedAssertInfo = halResetInfo.crash.data.assertInfo;
  }
}

uint8_t halGetResetInfo(void)
{
  return RESET_BASE_TYPE(savedResetCause);
}

uint16_t halGetExtendedResetInfo(void)
{
  return savedResetCause;
}

const HalAssertInfoType *halGetAssertInfo(void)
{
  return &savedAssertInfo;
}

#ifdef INTERRUPT_DEBUGGING
//=============================================================================
// If interrupt debugging is enabled, the actual ISRs are listed in this
// secondary interrupt table.  The halInternalIntDebuggingIsr will use this
// table to jump to the appropriate handler
//=============================================================================
NO_STRIPPING const HalVectorTableType __real_vector_table[] =
{
  { .topOfStack = _CSTACK_SEGMENT_END },
  #define EXCEPTION(vectorNumber, functionName, priorityLevel, subpriority) \
    functionName,
    #include NVIC_CONFIG
  #undef EXCEPTION
};

#error INTERRUPT_DEBUGGING not yet supported on efm32!

#endif //INTERRUPT_DEBUGGING
