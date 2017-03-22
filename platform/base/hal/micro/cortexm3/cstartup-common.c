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
#include "hal/micro/cortexm3/mpu.h"
#include "hal/micro/micro.h"
#include "hal/micro/cortexm3/memmap.h"
#include "hal/micro/cortexm3/cstartup-common.h"
#include "hal/micro/cortexm3/internal-storage.h"

#include "stack/include/ember-types.h"
#include "hal/micro/bootloader-interface.h"

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
  #pragma message("The APP_BTL build option is deprecated.  Removing this option will build for any 8k bootloader type.")
#endif
#ifdef SERIAL_UART_BTL
  #pragma message("The SERIAL_UART_BTL build option is deprecated.  Removing this option will build for any 8k bootloader type.")
#endif
#ifdef SERIAL_OTA_BTL
  #pragma message("The SERIAL_UART_OTA build option is deprecated.  Removing this option will build for any 8k bootloader type.")
#endif
#ifdef NULL_BTL
  // Fully supported, no error
#endif
#ifdef SMALL_BTL
  #error SMALL_BTL is not supported
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
    #if (defined(EMBER_NO_STACK) || /* nodetest and friends */               \
         (! defined(EMBER_STACK_IP) && ! defined(EMBER_STACK_CONNECT)))
      // Pro Stack
      // Right now we define the stack size to be for the worst case scenario,
      // ECC.  The ECC 163k1 library  and the ECC 283k1 Library both use the stack 
      // for calculations. Empirically I have seen it use as much as 1900 bytes 
      // for the 'key bit generate' operation.
      // So we add a 25% buffer: 1900 * 1.25 = 2375
      #define CSTACK_SIZE  (600)  // *4 = 2400 bytes
    #else
      // IP Stack
      #define CSTACK_SIZE  (950)  // *4 = 3800 bytes
    #endif //defined(EMBER_NO_STACK) ...
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
VAR_AT_SEGMENT(NO_STRIPPING __no_init const HalFixedAddressTableType halFixedAddressTable, __FAT__);

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
#else
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
  0xFF,                                               //uint8_t familyInfo; (for EFM32, defined in micro.h)
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
    RESET_POWERON_HV,            // bit 0: RESET_PWRHV_BIT
    RESET_POWERON_LV,            // bit 1: RESET_PWRLV_BIT
    RESET_EXTERNAL_PIN,          // bit 2: RESET_NRESET_BIT
    RESET_WATCHDOG_EXPIRED,      // bit 3: RESET_WDOG_BIT
    RESET_SOFTWARE,              // bit 4: RESET_SW_BIT
    RESET_BOOTLOADER_DEEPSLEEP,  // bit 5: RESET_DSLEEP_BIT
    RESET_FATAL_OPTIONBYTE,      // bit 6: RESET_OPTBYTEFAIL_BIT
    RESET_FATAL_LOCKUP,          // bit 7: RESET_CPULOCKUP_BIT
  };

  // It is possible for RSTB and POWER_HV to be set at the same time, which
  // happens when RSTB releases between HV power good and LV power good. (All
  // other reset events are mutually exclusive.) When both RSTB and POWER_HV
  // are set, POWER_HV should be returned as the cause. The bit test order --
  // from LSB to MSB -- ensures that it will.
  uint16_t resetEvent = RESET_EVENT &
                        ( RESET_CPULOCKUP_MASK   |
                          RESET_OPTBYTEFAIL_MASK |
                          RESET_DSLEEP_MASK      |
                          RESET_SW_MASK          |
                          RESET_WDOG_MASK        |
                          RESET_NRESET_MASK      |
                          RESET_PWRLV_MASK       |
                          RESET_PWRHV_MASK );

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

//[[  Macro used with the interrupt latency debugging hooks
  #define ISR_TX_BIT(bit)  GPIO_PAOUT = (GPIO_PAOUT&(~PA7_MASK))|((bit)<<PA7_BIT)
//]]

//=============================================================================
// The halInternalDebuggingIsr will intercept all exceptions in order to
// set interrupt debugging IO flags so that interrupt latency and other timings
// may be measured with a logic analyzer
//=============================================================================
void halInternalIntDebuggingIsr(void)
{
  bool prevState = I_STATE(I_PORT,I_PIN);
  uint32_t exception;

  //[[ Additional debug output for printing, via printf, the name of the
  //   ISR that has been invoked and the time it took (in us).  The ISR must
  //   exceed a time threshold to print the name.
  #ifdef PRINT_ISR_NAME
  uint32_t beginTime = MAC_TIMER;
  uint32_t endTime;
  #endif //PRINT_ISR_NAME
  //]]

  I_SET(I_PORT, I_PIN);

  // call the actual exception we were supposed to go to.  The exception
  // number is conveniently available in the SCS_ICSR register
  exception = (SCS_ICSR & SCS_ICSR_VECACTIVE_MASK) >> SCS_ICSR_VECACTIVE_BIT;

  //[[  This is a little bit of additional debug output that serially shows
  //     which exception was triggered, so that long ISRs can be determined
  //     from a logic analyzer trace
  #if 0
  {
    uint32_t byte = (exception<<3) | 0x2;
    uint32_t i;
    for(i=0;i<11;i++) {
     ISR_TX_BIT(byte&0x1); //data bit
      byte = (byte>>1);
    }
    ISR_TX_BIT(1); //stop bit
  }
  #endif
  //]]

  __real_vector_table[exception].ptrToHandler();

  // In order to deal with the possibility of nesting, only clear the status
  // output if it was clear when we entered
  if(!prevState)
    I_CLR(I_PORT, I_PIN);
  //[[ Some additional debug output to show the ISR exited
  #if 0
  else {
    ISR_TX_BIT(0);
    ISR_TX_BIT(1);
  }
  #endif
  //]]

  //[[ Additional debug output for printing, via printf, the name of the
  //   ISR that has been invoked and the time it took (in us).  The ISR must
  //   exceed a time threshold to print the name.
  #ifdef PRINT_ISR_NAME
  endTime = MAC_TIMER;
  if(((endTime-beginTime)&MAC_TIMER_MAC_TIMER_MASK) > 150) {
    EmberStatus emberSerialGuaranteedPrintf(uint8_t port, PGM_P formatString, ...);
    emberSerialGuaranteedPrintf(1, "[%d:", (endTime-beginTime));
    switch(NVIC->IABR[0]) {
      case INT_DEBUG:
        emberSerialGuaranteedPrintf(1, "DEBUG");
      break;
      case INT_IRQD:
        emberSerialGuaranteedPrintf(1, "IRQD");
      break;
      case INT_IRQC:
        emberSerialGuaranteedPrintf(1, "IRQC");
      break;
      case INT_IRQB:
        emberSerialGuaranteedPrintf(1, "IRQB");
      break;
      case INT_IRQA:
        emberSerialGuaranteedPrintf(1, "IRQA");
      break;
      case INT_ADC:
        emberSerialGuaranteedPrintf(1, "ADC");
      break;
      case INT_MACRX:
        emberSerialGuaranteedPrintf(1, "MACRX");
      break;
      case INT_MACTX:
        emberSerialGuaranteedPrintf(1, "MACTX");
      break;
      case INT_MACTMR:
        emberSerialGuaranteedPrintf(1, "MACTMR");
      break;
      case INT_SEC:
        emberSerialGuaranteedPrintf(1, "SEC");
      break;
      case INT_SC2:
        emberSerialGuaranteedPrintf(1, "SC1");
      break;
      case INT_SC1:
        emberSerialGuaranteedPrintf(1, "SC1");
      break;
      case INT_SLEEPTMR:
        emberSerialGuaranteedPrintf(1, "SLEEPTMR");
      break;
      case INT_BB:
        emberSerialGuaranteedPrintf(1, "BB");
      break;
      case INT_MGMT:
        emberSerialGuaranteedPrintf(1, "MGMT");
      break;
      case INT_TIM2:
        emberSerialGuaranteedPrintf(1, "TIM2");
      break;
      case INT_TIM1:
        emberSerialGuaranteedPrintf(1, "TIM1");
      break;
    }
    emberSerialGuaranteedPrintf(1, "]");
  }
  #endif //PRINT_ISR_NAME
  //]]
}
#endif //INTERRUPT_DEBUGGING
