/** @file hal/micro/cortexm3/micro.h
 * @brief Utility and convenience functions for EM35x microcontroller.
 * See @ref micro for documentation.
 *
 * <!-- Copyright 2013 Silicon Laboratories, Inc.        All rights reserved.-->
 */

/** @addtogroup micro
 * See also hal/micro/cortexm3/micro.h for source code.
 *@{
 */

#ifndef __EM3XX_MICRO_H__
#define __EM3XX_MICRO_H__

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#ifndef __MICRO_H__
#error do not include this file directly - include micro/micro.h
#endif

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#ifndef __EMBERSTATUS_TYPE__
#define __EMBERSTATUS_TYPE__
  //This is necessary here because halSetRadioHoldOff returns an
  //EmberStatus and not adding this typedef to this file breaks a
  //whole lot of builds due to include mis-ordering
  typedef uint8_t EmberStatus;
#endif //__EMBERSTATUS_TYPE__
#endif // DOXYGEN_SHOULD_SKIP_THIS

// Micro specific serial defines
#define EM_NUM_SERIAL_PORTS 4
#define EMBER_SPI_MASTER 4
#define EMBER_SPI_SLAVE 5
#define EMBER_I2C 6

// Define the priority registers of system handlers and interrupts.
// This example shows how to save the current ADC interrupt priority and
// then set it to 24:
//    uint8_t oldAdcPriority = INTERRUPT_PRIORITY_REGISTER(ADC);
//    INTERRUPT_PRIORITY_REGISTER(ADC) = 24;

// For Cortex-M3 faults and exceptions
#define HANDLER_PRIORITY_REGISTER(handler) \
  (*( ((uint8_t *)SCS_SHPR_7to4_ADDR) + handler##_VECTOR_INDEX - 4) )

// For EM3XX-specific interrupts
#define INTERRUPT_PRIORITY_REGISTER(interrupt) \
  (* ( ((uint8_t *)NVIC_IPR_3to0_ADDR) + interrupt##_VECTOR_INDEX - 16) )


// The reset types of the EM300 series have both a base type and an
//  extended type.  The extended type is a 16-bit value which has the base
//  type in the upper 8-bits, and the extended classification in the
//  lower 8-bits
// For backwards compatibility with other platforms, only the base type is
//  returned by the halGetResetInfo() API.  For the full extended type, the
//  halGetExtendedResetInfo() API should be called.

#define RESET_BASE_TYPE(extendedType)   ((uint8_t)(((extendedType) >> 8) & 0xFF))
#define RESET_EXTENDED_FIELD(extendedType) ((uint8_t)(extendedType & 0xFF))
#define RESET_VALID_SIGNATURE           (0xF00F)
#define RESET_INVALID_SIGNATURE         (0xC33C)

// Define the base reset cause types
#define RESET_BASE_DEF(basename, value, string)  RESET_##basename = value,
#define RESET_EXT_DEF(basename, extname, extvalue, string)  /*nothing*/
enum {
  #include "reset-def.h"
  NUM_RESET_BASE_TYPES
};
#undef RESET_BASE_DEF
#undef RESET_EXT_DEF

// Define the extended reset cause types
#define RESET_EXT_VALUE(basename, extvalue)  \
  (((RESET_##basename)<<8) + extvalue)
#define RESET_BASE_DEF(basename, value, string)  /*nothing*/
#define RESET_EXT_DEF(basename, extname, extvalue, string)  \
  RESET_##basename##_##extname = RESET_EXT_VALUE(basename, extvalue),
enum {
  #include "reset-def.h"
};
#undef RESET_EXT_VALUE
#undef RESET_BASE_DEF
#undef RESET_EXT_DEF

// These define the size of the GUARD region configured in the MPU that
// sits between the heap and the stack
#define HEAP_GUARD_REGION_SIZE       (SIZE_32B)
#define HEAP_GUARD_REGION_SIZE_BYTES (1<<(HEAP_GUARD_REGION_SIZE+1))

// Define a value to fill the guard region between the heap and stack.
#define HEAP_GUARD_FILL_VALUE (0xE2E2E2E2)

// Resize the CSTACK to add space to the 'heap' that exists below it
uint32_t halStackModifyCStackSize(int32_t stackSizeDeltaWords);

// Initialize the CSTACK/Heap region and the guard page in between them
void halInternalInitCStackRegion(void);

// Helper functions to get the location of the stack/heap
uint32_t halInternalGetCStackBottom(void);
uint32_t halInternalGetHeapTop(void);
uint32_t halInternalGetHeapBottom(void);

#endif // DOXYGEN_SHOULD_SKIP_THIS

/** @name Vector Table Index Definitions
 * These are numerical definitions for vector table. Indices 0 through 15 are
 * Cortex-M3 standard exception vectors and indices 16 through 35 are EM3XX
 * specific interrupt vectors.
 *
 *@{
 */

/**
 * @brief A numerical definition for a vector.
 */
#define STACK_VECTOR_INDEX          0 // special case: stack pointer at reset
#define RESET_VECTOR_INDEX          1
#define NMI_VECTOR_INDEX            2
#define HARD_FAULT_VECTOR_INDEX     3
#define MEMORY_FAULT_VECTOR_INDEX   4
#define BUS_FAULT_VECTOR_INDEX      5
#define USAGE_FAULT_VECTOR_INDEX    6
#define RESERVED07_VECTOR_INDEX     7
#define RESERVED08_VECTOR_INDEX     8
#define RESERVED09_VECTOR_INDEX     9
#define RESERVED10_VECTOR_INDEX     10
#define SVCALL_VECTOR_INDEX         11
#define DEBUG_MONITOR_VECTOR_INDEX  12
#define RESERVED13_VECTOR_INDEX     13
#define PENDSV_VECTOR_INDEX         14
#define SYSTICK_VECTOR_INDEX        15
#define TIMER1_VECTOR_INDEX         16
#define TIMER2_VECTOR_INDEX         17
#define MANAGEMENT_VECTOR_INDEX     18
#define BASEBAND_VECTOR_INDEX       19
#define SLEEP_TIMER_VECTOR_INDEX    20
#define SC1_VECTOR_INDEX            21
#define SC2_VECTOR_INDEX            22
#define SECURITY_VECTOR_INDEX       23
#define MAC_TIMER_VECTOR_INDEX      24
#define MAC_TX_VECTOR_INDEX         25
#define MAC_RX_VECTOR_INDEX         26
#define ADC_VECTOR_INDEX            27
#define IRQA_VECTOR_INDEX           28
#define IRQB_VECTOR_INDEX           29
#define IRQC_VECTOR_INDEX           30
#define IRQD_VECTOR_INDEX           31
#define DEBUG_VECTOR_INDEX          32
#define SC3_VECTOR_INDEX            33
#define SC4_VECTOR_INDEX            34
#define USB_VECTOR_INDEX            35

/**
 * @brief Number of vectors.
 */
#define VECTOR_TABLE_LENGTH         36
/** @}  Vector Table Index Definitions */

/**
 * @brief Records the specified reset cause then forces a reboot.
 */
void halInternalSysReset(uint16_t extendedCause);

/**
 * @brief Returns the Extended Reset Cause information
 *
 * @return A 16-bit code identifying the base and extended cause of the reset
 */
uint16_t halGetExtendedResetInfo(void);

/** @brief Calls ::halGetExtendedResetInfo() and supplies a string describing
 *  the extended cause of the reset.  halGetResetString() should also be called
 *  to get the string for the base reset cause
 *
 * @appusage Useful for diagnostic printing of text just after program
 * initialization.
 *
 * @return A pointer to a program space string.
 */
PGM_P halGetExtendedResetString(void);

//[[ ram vectors are not public
#ifndef DOXYGEN_SHOULD_SKIP_THIS

/** @brief Install the given address at the given vector number, overriding
 *  the default value found in Flash.
 *
 * The first RAM Vector to be Registered will automatically cause the RAM
 * Vector table to be filled from the Flash Vector table and the RAM table
 * will be enabled.
 *
 * @param vectorNumber  The vector number that is being changed.  There are
 * 31 valid vectors, 2-32, and the list can be found in nvic-config.h.
 *
 * @param newVector  The 32-bit address to install into the vector.  This
 * is generally the address of the function that should be called when
 * the vector is triggered.
 *
 * @return The 32-bit address of the previous vector in the RAM Table that
 * is being replaced.
 */
uint32_t halRegisterRamVector(uint8_t vectorNumber, uint32_t newVector);

/** @brief Uninstall the given vector number, restoring the vector from the
 *  Flash vector table.
 *
 * The last RAM Vector to be UnRegistered will automatically cause the RAM
 * Vector table to be deactivated and Flash Vector table will regain control.
 *
 * @param vectorNumber  The vector number that is being changed.  There are
 * 31 valid vectors, 2-32, and the list can be found in nvic-config.h.
 *
 * @return The 32-bit address of the previous vector in the RAM Table that
 * is being replaced.
 */
uint32_t halUnRegisterRamVector(uint8_t vectorNumber);

#endif // DOXYGEN_SHOULD_SKIP_THIS
//]]

/** @brief Enables or disables Radio HoldOff support
 *
 * @param enable  When true, configures ::RHO_GPIO in BOARD_HEADER
 * as an input which, when asserted, will prevent the radio from
 * transmitting.  When false, configures ::RHO_GPIO for its original
 * default purpose.
 *
 * @return EMBER_SUCCESS if Radio HoldOff was configured as desired
 * or EMBER_BAD_ARGUMENT if requesting it be enabled but RHO has not
 * been configured by the BOARD_HEADER.
 */
EmberStatus halSetRadioHoldOff(bool enable);

/** @brief Returns whether Radio HoldOff has been enabled or not.
 *
 * @return true if Radio HoldOff has been enabled, false otherwise.
 */
bool halGetRadioHoldOff(void);

/** @brief To assist with saving power when the radio automatically powers
 * down, this function allows the stack to tell the HAL to put pins
 * specific to radio functionality in their powerdown state.  The pin
 * state used is the state used by halInternalPowerDownBoard, but applied
 * only to the pins identified in the global variable gpioRadioPowerBoardMask.
 * The stack will automatically call this function as needed, but it
 * will only change GPIO state based on gpioRadioPowerBoardMask.  Most
 * commonly, the bits set in gpioRadioPowerBoardMask petain to using a
 * Front End Module.  This function is often called from interrupt context.
 */
void halStackRadioPowerDownBoard(void);

/** @brief To assist with saving power when radio2 automatically powers
 * down, this function allows the stack to tell the HAL to put pins
 * specific to radio functionality in their powerdown state.  The pin
 * state used is the state used by halInternalPowerDownBoard, but applied
 * only to the pins identified in the global variable gpioRadioPowerBoardMask.
 * The stack will automatically call this function as needed, but it
 * will only change GPIO state based on gpioRadioPowerBoardMask.  Most
 * commonly, the bits set in gpioRadioPowerBoardMask petain to using a
 * Front End Module.  This function is often called from interrupt context.
 */
void halStackRadio2PowerDownBoard(void);

/** @brief To assist with saving power when the radio automatically powers
 * up, this function allows the stack to tell the HAL to put pins
 * specific to radio functionality in their powerup state.  The pin
 * state used is the state used by halInternalPowerUpBoard, but applied
 * only to the pins identified in the global variable gpioRadioPowerBoardMask.
 * The stack will automatically call this function as needed, but it
 * will only change GPIO state based on gpioRadioPowerBoardMask.  Most
 * commonly, the bits set in gpioRadioPowerBoardMask petain to using a
 * Front End Module.  This function can be called from interrupt context.
 */
void halStackRadioPowerUpBoard(void);

/** @brief To assist with saving power when radio2 automatically powers
 * up, this function allows the stack to tell the HAL to put pins
 * specific to radio functionality in their powerup state.  The pin
 * state used is the state used by halInternalPowerUpBoard, but applied
 * only to the pins identified in the global variable gpioRadioPowerBoardMask.
 * The stack will automatically call this function as needed, but it
 * will only change GPIO state based on gpioRadioPowerBoardMask.  Most
 * commonly, the bits set in gpioRadioPowerBoardMask petain to using a
 * Front End Module.  This function can be called from interrupt context.
 */
void halStackRadio2PowerUpBoard(void);

/** @brief This function is called automatically by the stack prior to Radio
 * power-up and after Radio power-down.  It can be used to prepare for the
 * radio being powered on and to clean up after it's been powered off.  Unlike
 * halStackRadioPowerUpBoard() and halStackRadioPowerDownBoard(), which can
 * be called from interrupt context, this function is only called from
 * main-line context.
 */
void halStackRadioPowerMainControl(bool powerUp);

/**
 * @brief Handler called in main context prior to radio being powered on
 */
void halRadioPowerUpHandler(void);

/**
 * @brief Handler called in main context after radio has been powered off
 */
void halRadioPowerDownHandler(void);

#include "micro-common.h"

// Enable PTA Support
#define PTA_SUPPORT
#define PTA_GPIOCFG_INPUT     GPIOCFG_IN_PUD
#define PTA_GPIOCFG_OUTPUT    GPIOCFG_OUT
#define PTA_GPIOCFG_WIRED_OR  GPIOCFG_OUT_OD
#define PTA_GPIOCFG_WIRED_AND GPIOCFG_OUT_OD

#endif //__EM3XX_MICRO_H__

/**@} // END micro group
 */

