/** @file hal/plugin/sim-eeprom/sim-eeprom-cortexm3.h
 * @brief Simulated EEPROM system for wear leveling token storage across flash.
 * See @ref simeeprom2 for documentation.
 *
 * Copyright 2007-2011 by Ember Corporation. All rights reserved.           *80*
 */
#ifndef __PLAT_SIM_EEPROM_H__
#define __PLAT_SIM_EEPROM_H__

#include "hal/micro/cortexm3/memmap.h"
#include "hal/micro/cortexm3/flash.h"
 
/** @addtogroup simeeprom
 * By default, the EM35x Simulated EEPROM is designed to consume 8kB of
 * upper flash within which it will perform wear leveling.  It is possible
 * to reduce the reserved flash to only 4kB by defining EMBER_SIM_EEPROM_4KB
 * when building your application.
 *
 * While there is no specific ::EMBER_SIM_EEPROM_8KB, it is possible to use
 * the define ::EMBER_SIM_EEPROM_8KB for clarity in your application.
 *
 * @note Switching between an 8kB and 4kB Simulated EEPROM should be done
 * by rebuilding the entire application to ensure all sources recognize
 * the change.  The Simualted EEPROM is designed to seamlessly move
 * between 8kB and 4kB without erasing flash first.
 *
 * \b
 *
 * @note Switching between 8kB and 4kB Simulated EEPROM will <b>
 * automatically erase all </b> Simulated EEPROM tokens and reload
 * them from default values.
 */

/** @addtogroup simeeprom2
 * @note <b>Simulated EEPROM 2 is only available for use in the NCP.</b>
 * @note Simulated EEPROM and Simulated EEPROM 2 functions cannot be
 * intermixed; SimEE and SimEE2 are mutually exclusive.  The functions
 * in @ref simeeprom cannot be used with
 * SimEE2 and the functons in @ref simeeprom2 cannot be used
 * with SimEE.
 *
 * The Simulated EEPROM 2 system (typically referred to as SimEE2) is
 * designed to operate under the @ref tokens
 * API and provide a non-volatile storage system.  Since the flash write cycles
 * are finite, the SimEE2's primary purpose is to perform wear
 * leveling across several hardware flash pages, ultimately increasing the
 * number of times tokens may be written before a hardware failure.
 *
 * Compiling the application with the define USE_SIMEE2 will switch
 * the application from using the original SimEE to SimEE2.
 *
 * @note Only the NCP is capable of upgrading it's existing SimEE data
 * to SimEE2.  It's not possible to downgrade from SimEE2.
 *
 * The Simulated EEPROM 2 needs to periodically perform a page erase 
 * operation to reclaim storage area for future token writes.  The page
 * erase operation requires an ATOMIC block of 21ms.  Since this is such a long
 * time to not be able to service any interrupts, the page erase operation is
 * under application control providing the application the opportunity to
 * decide when to perform the operation and complete any special handling
 * that might be needed.
 *
 * @note The best, safest, and recommended practice is for the application
 * to regularly and always call the function halSimEepromErasePage() when the
 * application can expect and deal with the page erase delay.
 * halSimEepromErasePage() will immediately return if there is nothing to
 * erase.  If there is something that needs to be erased, doing so
 * as regularly and as soon as possible will keep the SimEE2 in the
 * healthiest state possible.
 *
 * SimEE2  differs from the
 * original SimEE in terms of size and speed.  SimEE2 holds more data
 * but at the expense of consuming more overall flash to support a new
 * wear levelling technique.  SimEE2's worst case execution time under
 * normal behavior is faster and finite, but the average execution time is
 * longer.
 *
 * See hal/plugin/sim-eeprom/sim-eeprom-cortexm3.h for source code.
 *@{ 
 */
 
/**@} // END simeeprom2 group
 */


#ifndef DOXYGEN_SHOULD_SKIP_THIS

// See the sim-eeprom-size.h file for the logic used to determine the ultimate
// size of the simeeprom.
#include "hal/plugin/sim-eeprom/sim-eeprom-size.h"

//This is confusing, so pay attention...  :-)
//The actual SimEE storage lives inside of simulatedEepromStorage and begins
//at the very bottom of simulatedEepromStorage and fills the entirety of
//this storage array.  The SimEE code, though, uses 16bit addresses for
//everything since it was originally written for the XAP2b.  On a 250,
//the base address was 0xF000 since this corresponded to the actual absolute
//address that was used in flash.  On a non-250, though, the base address
//is largely irrelevant since there is a translation shim layer that converts
//SimEE 16bit addresses into the real 32bit addresses needed to access flash.
//If you look at the translation shim layer in
//sim-eeprom-internal.c you'll see that the address used by the SimEE is
//subtracted by VPA_BASE (which is the same as SIMEE_BASE_ADDR_HW) to
//return back to the bottom of the simulatedEepromStorage area.
//[BugzId:14448 fix removes need for this to be anything but 0x0000]
#define SIMEE_BASE_ADDR_HW  0x0000

//Define a variable that holds the actual SimEE storage the linker will
//place at the proper location in flash.
extern uint8_t simulatedEepromStorage[SIMEE_SIZE_B];

//these parameters frame the sim-eeprom and are derived from the location
//of the sim-eeprom as defined in memmap.h
/**
 * @brief The size of a physical flash page, in SimEE addressing units.
 */
extern const uint16_t REAL_PAGE_SIZE;
/**
 * @brief The size of a Virtual Page, in SimEE addressing units.
 */
extern const uint16_t VIRTUAL_PAGE_SIZE;
/**
 * @brief The number of physical pages in a Virtual Page.
 */
extern const uint8_t REAL_PAGES_PER_VIRTUAL;
/**
 * @brief The bottom address of the Left Virtual Page.
 * Only used in Simulated EEPROM 1.
 */
extern const uint16_t LEFT_BASE;
/**
 * @brief The top address of the Left Virtual Page.
 * Only used in Simulated EEPROM 1.
 */
extern const uint16_t LEFT_TOP;
/**
 * @brief The bottom address of the Right Virtual Page.
 * Only used in Simulated EEPROM 1.
 */
extern const uint16_t RIGHT_BASE;
/**
 * @brief The top address of the Right Virtual Page.
 * Only used in Simulated EEPROM 1.
 */
extern const uint16_t RIGHT_TOP;
/**
 * @brief The bottom address of Virtual Page A.
 * Only used in Simulated EEPROM 2.
 */
extern const uint16_t VPA_BASE;
/**
 * @brief The top address of Virtual Page A.
 * Only used in Simulated EEPROM 2.
 */
extern const uint16_t VPA_TOP;
/**
 * @brief The bottom address of Virtual Page B.
 * Only used in Simulated EEPROM 2.
 */
extern const uint16_t VPB_BASE;
/**
 * @brief The top address of Virtual Page B.
 * Only used in Simulated EEPROM 2.
 */
extern const uint16_t VPB_TOP;
/**
 * @brief The bottom address of Virtual Page C.
 * Only used in Simulated EEPROM 2.
 */
extern const uint16_t VPC_BASE;
/**
 * @brief The top address of Virtual Page C.
 * Only used in Simulated EEPROM 2.
 */
extern const uint16_t VPC_TOP;
/**
 * @brief The memory address at which point erasure requests transition
 * from being "GREEN" to "RED" when the freePtr crosses this address.
 * Only used in Simulated EEPROM 1.
 */
extern const uint16_t ERASE_CRITICAL_THRESHOLD;


/** @brief Gets a pointer to the token data.  
 *
 * @note This function only works on Cortex-M3 chips since it requires
 * being able to return a pointer to flash memory.
 *
 * @note Only the public function should be called since the public
 * function provides the correct parameters.
 *
 * The Simulated EEPROM uses a RAM Cache
 * to hold the current state of the Simulated EEPROM, including the location
 * of the freshest token data.  This function simply uses the ID and
 * index parameters to access the pointer RAM Cache.  The absolute flash
 * address stored in the pointer RAM Cache is extracted and ptr is
 * set to this address. 
 *
 * Here is an example of calling this function:
 * @code
 * tokTypeStackCalData calData = {0,};
 * tokTypeStackCalData *calDataToken = &calData;
 * halStackGetIdxTokenPtrOrData(&calDataToken,
 *                              TOKEN_STACK_CAL_DATA,
 *                              index);
 * @endcode
 *
 * @param ptr  A pointer to where the absolute address of the data should 
 * be placed.
 *
 * @param ID    The ID of the token to get data from.  Since the token system
 * is designed to enumerate the token names down to a simple 8 bit ID,
 * generally the TOKEN_name is the parameter used when invoking this function.
 *
 * @param index If token being accessed is indexed, this parameter is
 * combined with the ID to formulate both the RAM Cache lookup as well
 * as the desired InfoWord to look for.  If the token is not an indexed token,
 * this parameter is ignored.
 *
 * @param len   The number of bytes being worked on.  This should always be
 * the size of the token (available from both the sizeof() intrinsic as well
 * as the tokenSize[] array).
 *
 */
void halInternalSimEeGetPtr(void *ptr, uint8_t compileId, uint8_t index, uint8_t len);

#endif //__PLAT_SIM_EEPROM_H__

#endif //DOXYGEN_SHOULD_SKIP_THIS

