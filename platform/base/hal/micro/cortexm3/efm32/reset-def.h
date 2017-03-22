/** @file hal/micro/cortexm3/reset-def.h
 * @brief Definitions for all the reset cause types
 *
 * <!-- Copyright 2009 by Ember Corporation.             All rights reserved.-->
 */
 
/** @addtogroup reset_def
 * Definitions for all the reset cause types.
 *
 * Reset cause types are built from a base definition and an extended.
 * definition. The base definitions allow working with entire categories of
 * resets while the extended definitions allow drilling down to very
 * specific causes.  The macros for the base and extended definitions are
 * combined for use in the code and equated to their combined numberical
 * equivalents.  In addition, exach base and extended definition is given
 * a corresponding 3 letter ASCII string to facilitate printing.  The ASCII
 * strings are best use with ::halGetExtendedResetString.
 *
 * For example:
 * \code
 * RESET_BASE_DEF(EXTERNAL,            0x03,     "EXT")
 *   RESET_EXT_DEF(EXTERNAL, UNKNOWN,    0x00,     "UNK")
 *   RESET_EXT_DEF(EXTERNAL, PIN,        0x01,     "PIN")
 * \endcode
 * results in enums which includes the entries:
 * \code
 * RESET_EXTERNAL = 0x03
 * RESET_EXTERNAL_PIN = 0x0301
 * \endcode
 * 
 * For a complete listing of all reset base and extended definitions, see
 * reset-def.h for source code.
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

// *******************
// This file is specifically kept wider than 80 columns in order to keep the information 
//   well organized and easy to read by glancing down the columns
// *******************

// The reset types of the EM300 series have both a base type and an 
//  extended type.  The extended type is a 16-bit value which has the base
//  type in the upper 8-bits, and the extended classification in the 
//  lower 8-bits
// For backwards compatibility with other platforms, only the base type is 
//  returned by the halGetResetInfo() API.  For the full extended type, the
//  halGetExtendedResetInfo() API should be called.


// RESET_BASE_DEF macros take the following parameters:
//    RESET_BASE_DEF(basename, value, string)  // description
//        basename - the name used in the enum definition, expanded as RESET_basename
//        value - the value of the enum definition
//        string - the reset string, must be 3 characters
//        description - a comment describing the cause

// RESET_EXT_DEF macros take the following parameters:
//    RESET_EXT_DEF(basename, extname, extvalue, string)  // description
//        basename - the base name used in the enum definition
//        extname - the extended name used in the enum definition, expanded as RESET_basename_extname
//        extvalue - the LSB value of the enum definition, combined with the value of the base value in the MSB
//        string - the reset string, must be 3 characters
//        description - a comment describing the cause

// *******************
// This file is specifically kept wider than 80 columns in order to keep the information 
//   well organized and easy to read by glancing down the columns
// (Yes, this comment is mentioned multiple times in the file)
// *******************

// This file wants to use the bareword BOOTLOADER, which is sometimes a symbol.
// To allow this we will undefine the symbol here and restore it at the end of
// this file.
#ifdef BOOTLOADER
  #define SAVED_BOOTLOADER BOOTLOADER
  #undef BOOTLOADER
#endif

//[[ 
//   The first 3 (unknown, fib, and bootloader) reset base types and their
//     extended values should never be changed, as they are used by 
//     bootloaders in the field which can never be changed.
//   The later reset base and extended types may be changed over time
//]]

RESET_BASE_DEF(UNKNOWN,             0x00,   "UNK")    // Underterminable cause
  RESET_EXT_DEF(UNKNOWN, UNKNOWN,     0x00,   "UNK")    // Undeterminable cause
  
RESET_BASE_DEF(FIB,                 0x01,   "FIB")      // Reset originated from the FIB bootloader
  RESET_EXT_DEF(FIB, GO,              0x00,   "GO ")      // FIB bootloader caused a reset to main flash 
  RESET_EXT_DEF(FIB, BOOTLOADER,      0x01,   "BTL")      // FIB bootloader is instructing ember bootloader to run
  RESET_EXT_DEF(FIB, GO2,             0x02,   "GO2")      // GO2 (unused)
  RESET_EXT_DEF(FIB, GO3,             0x03,   "GO3")      // GO3 (unused)
  RESET_EXT_DEF(FIB, GO4,             0x04,   "GO4")      // GO4 (unused)
  RESET_EXT_DEF(FIB, GO5,             0x05,   "GO5")      // GO5 (unused)
  RESET_EXT_DEF(FIB, GO6,             0x06,   "GO6")      // GO6 (unused)
  RESET_EXT_DEF(FIB, GO7,             0x07,   "GO7")      // GO7 (unused)
  RESET_EXT_DEF(FIB, GO8,             0x08,   "GO8")      // GO8 (unused)
  RESET_EXT_DEF(FIB, GO9,             0x09,   "GO9")      // GO9 (unused)
  RESET_EXT_DEF(FIB, GOA,             0x0A,   "GOA")      // GOA (unused)
  RESET_EXT_DEF(FIB, GOB,             0x0B,   "GOB")      // GOB (unused)
  RESET_EXT_DEF(FIB, GOC,             0x0C,   "GOC")      // GOC (unused)
  RESET_EXT_DEF(FIB, GOD,             0x0D,   "GOD")      // GOD (unused)
  RESET_EXT_DEF(FIB, GOE,             0x0E,   "GOE")      // GOE (unused)
  RESET_EXT_DEF(FIB, GOF,             0x0F,   "GOF")      // GOF (unused)
  RESET_EXT_DEF(FIB, JUMP,            0x10,   "JMP")      // FIB bootloader is jumping to a specific flash address
  RESET_EXT_DEF(FIB, BAUDRATE,        0x11,   "BDR")      // FIB bootloader detected a high baud rate, causes ember bootloader to run
  RESET_EXT_DEF(FIB, UNPROTECT,       0x12,   "UPR")      // Read protection disabled, flash should be erased
  RESET_EXT_DEF(FIB, BOOTMODE,        0x13,   "BTM")      // BOOTMODE was not held long enough
  RESET_EXT_DEF(FIB, MISMATCH,        0x14,   "MSM")      // MISMATCHED FIB Bootloader & Part Data
  RESET_EXT_DEF(FIB, FATAL,           0x15,   "FTL")      // FIB Fatal Error
   
RESET_BASE_DEF(BOOTLOADER,          0x02,     "BTL")    // Reset relates to an Ember bootloader
  RESET_EXT_DEF(BOOTLOADER, UNKNOWN,   0x00,     "UNK")    // Unknown bootloader cause (should never occur)
  RESET_EXT_DEF(BOOTLOADER, GO,        0x01,     "GO ")    // Bootloader caused reset telling app to run
  RESET_EXT_DEF(BOOTLOADER, BOOTLOAD,  0x02,     "BTL")    // Application requested that bootloader runs
  RESET_EXT_DEF(BOOTLOADER, BADIMAGE,  0x03,     "BAD")    // Application bootloader detect bad external upgrade image 
  RESET_EXT_DEF(BOOTLOADER, FATAL,     0x04,     "FTL")    // Fatal Error or assert in bootloader
  RESET_EXT_DEF(BOOTLOADER, FORCE,     0x05,     "FRC")    // Forced bootloader activation
  RESET_EXT_DEF(BOOTLOADER, OTAVALID,  0x06,     "OTA")    // OTA Bootloader mode activation
  RESET_EXT_DEF(BOOTLOADER, DEEPSLEEP, 0x07,     "DSL")    // Bootloader initiated deep sleep

//[[ -- Reset types below here may be changed in the future if absolutely necessary -- ]]

RESET_BASE_DEF(EXTERNAL,            0x03,     "EXT")    // External reset trigger
  RESET_EXT_DEF(EXTERNAL, UNKNOWN,    0x00,     "UNK")    // Unknown external cause (should never occur)
  RESET_EXT_DEF(EXTERNAL, PIN,        0x01,     "PIN")    // External pin reset
  RESET_EXT_DEF(EXTERNAL, EM4PIN,     0x02,     "EM4")    // Woken up from EM4 from a pin

RESET_BASE_DEF(POWERON,             0x04,     "PWR")    // Poweron reset type, supply voltage < power-on threshold
  RESET_EXT_DEF(POWERON, UNKNOWN,     0x00,     "UNK")    // Unknown poweron reset (should never occur)
  RESET_EXT_DEF(POWERON, HV,          0x01,     "HV ")    // High voltage poweron

RESET_BASE_DEF(WATCHDOG,            0x05,     "WDG")    // Watchdog reset occurred
  RESET_EXT_DEF(WATCHDOG, UNKNWON,    0x00,     "UNK")    // Unknown watchdog reset (should never occur)
  RESET_EXT_DEF(WATCHDOG, EXPIRED,    0x01,     "EXP")    // Watchdog timer expired
  RESET_EXT_DEF(WATCHDOG, CAUGHT,     0x02,     "LWM")    // Watchdog low watermark expired and caught extended info
  
RESET_BASE_DEF(SOFTWARE,            0x06,     " SW")    // Software triggered reset
  RESET_EXT_DEF(SOFTWARE, UNKNOWN,    0x00,     "UNK")    // Unknown software cause
  RESET_EXT_DEF(SOFTWARE, REBOOT,     0x01,     "RBT")    // General software reboot
  RESET_EXT_DEF(SOFTWARE, DEEPSLEEP,  0x02,     "DSL")    // App initiated deep sleep
  RESET_EXT_DEF(SOFTWARE, EM4,        0x03,     "EM4")    // App has been in EM4

RESET_BASE_DEF(CRASH,               0x07,     "CRS")    // Software crash
  RESET_EXT_DEF(CRASH, UNKNOWN,       0x00,     "UNK")    // Unknown crash
  RESET_EXT_DEF(CRASH, ASSERT,        0x01,     "AST")    // a self-check assert in the code failed
  
RESET_BASE_DEF(FLASH,               0x08,     "FSH")    // Flash failure cause reset
  RESET_EXT_DEF(FLASH, UNKNWON,       0x00,     "UNK")    // Unknown flash failure
  RESET_EXT_DEF(FLASH, VERIFY,        0x01,     "VFY")    // Flash write verify failure
  RESET_EXT_DEF(FLASH, INHIBIT,       0x02,     "INH")    // Flash write inhibited: already written
  
RESET_BASE_DEF(FATAL,               0x09,     "BAD")    // A non-recoverable fatal error occurred
  RESET_EXT_DEF(FATAL, UNKNOWN,       0x00,     "UNK")    // Unknown fatal error (should never occur)
  RESET_EXT_DEF(FATAL, LOCKUP,        0x01,     "LCK")    // CPU Core locked up
  RESET_EXT_DEF(FATAL, CRYSTAL,       0x02,     "XTL")    // 24MHz crystal failure
  RESET_EXT_DEF(FATAL, OPTIONBYTE,    0x03,     "OBF")    // option byte complement error

RESET_BASE_DEF(FAULT,               0x0A,     "FLT")    // A access fault occurred
  RESET_EXT_DEF(FAULT, UNKNOWN,       0x00,     "UNK")    // An unknown fault occurred
  RESET_EXT_DEF(FAULT, HARD,          0x01,     "HRD")    // Hard fault
  RESET_EXT_DEF(FAULT, MEM,           0x02,     "MEM")    // Memory protection violation
  RESET_EXT_DEF(FAULT, BUS,           0x03,     "BUS")    // Bus fault
  RESET_EXT_DEF(FAULT, USAGE,         0x04,     "USG")    // Usage fault
  RESET_EXT_DEF(FAULT, DBGMON,        0x05,     "DBG")    // Debug monitor fault
  RESET_EXT_DEF(FAULT, PROTDMA,       0x06,     "DMA")    // DMA RAM protection violation
  RESET_EXT_DEF(FAULT, BADVECTOR,     0x07,     "VCT")    // Uninitialized interrupt vector

RESET_BASE_DEF(BROWNOUT,                      0x0B,      "BRO")    // Brown out
  RESET_EXT_DEF(BROWNOUT, UNKNOWN,              0x00,     "UNK")    // An unknown fault occurred
  RESET_EXT_DEF(BROWNOUT, UNREGPOWER,           0x01,     "UP ")    // unregulated power.
  RESET_EXT_DEF(BROWNOUT, REGPOWER,             0x02,     "RP ")    // regulated power.
  RESET_EXT_DEF(BROWNOUT, AVDD0,                0x03,     "AP0")    // Analog Power Domain 0 (AVDD0).
  RESET_EXT_DEF(BROWNOUT, AVDD1,                0x04,     "AP1")    // Analog Power Domain 1 (AVDD1).
  RESET_EXT_DEF(BROWNOUT, AVDD,                 0x05,     "AP ")    // Analog Power Domain (AVDD).
  RESET_EXT_DEF(BROWNOUT, DVDD,                 0x06,     "DP ")    // Digital Power Domain (DVDD).
  RESET_EXT_DEF(BROWNOUT, DEC,                  0x06,     "DEC")    // Detector Decouple (DEC).
  RESET_EXT_DEF(BROWNOUT, BACKUP_VDD_DREG,      0x07,     "BVD")    // Backup BOD on VDD_DREG.
  RESET_EXT_DEF(BROWNOUT, BACKUP_BU_VIN,        0x08,     "BVI")    // Backup BOD on BU_VIN.
  RESET_EXT_DEF(BROWNOUT, BACKUP_UNREGPOWER,    0x09,     "BUP")    // Backup BOD on unregulated power
  RESET_EXT_DEF(BROWNOUT, BACKUP_REGPOWER,      0x10,     "BRP")    // Backup BOD on regulated power.
  RESET_EXT_DEF(BROWNOUT, BACKUP_MODE,          0x11,     "BM ")    // Backup Mode
// Restore the value of the BOOTLOADER symbol if we had to save it off in this
// file so that the word BOOTLOADER could be used.
#ifdef SAVED_BOOTLOADER
  #define BOOTLOADER SAVED_BOOTLOADER
  #undef SAVED_BOOTLOADER
#endif

#endif //DOXYGEN_SHOULD_SKIP_THIS

