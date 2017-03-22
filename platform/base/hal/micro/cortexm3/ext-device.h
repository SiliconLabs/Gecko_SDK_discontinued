/** @file hal/micro/cortexm3/ext-device.h
 *  @brief External Device GPIO driver API
 *
 * <!-- Copyright 2013 Silicon Laboratories, Inc.                        *80*-->
 */

#ifndef __EXT_DEVICE_H__
#define __EXT_DEVICE_H__

/** @brief API to access External Device GPIOs configured by BOARD_HEADER:
 *  - EXT_DEVICE_PWR  Power signal
 *  - EXT_DEVICE_RDY  Ready signal
 *  - EXT_DEVICE_SEL  Select signal
 *  - EXT_DEVICE_INT  Interrupt signal
 *  These should be defined in the BOARD_HEADER.
 *  @note Only supports one external device.
 */

//== DATA TYPES ==

/** @brief Device IRQ Callbacks (Handlers)
 */
typedef void (*HalExtDeviceIrqCB)(void);

enum {
  EXT_DEVICE_HAS_PWR     = 0x01,
  EXT_DEVICE_HAS_RDY     = 0x02,
  EXT_DEVICE_HAS_SEL     = 0x04,
  EXT_DEVICE_HAS_INT     = 0x08,
  EXT_DEVICE_HAS_RDY_IRQ = 0x20,
  EXT_DEVICE_HAS_INT_IRQ = 0x80,
};
typedef uint8_t HalExtDeviceConfig;

enum {
  EXT_DEVICE_INT_LEVEL_ON     = 0x00, // Interrupt is enabled
  EXT_DEVICE_INT_LEVEL_OFF    = 0x01, // Interrupt lowest level disabled
  //            :            // Values higher are disabled nesting depth
  EXT_DEVICE_INT_UNCONFIGURED = 0xFF, // Interrupt is not configured
};
typedef uint8_t HalExtDeviceIntLevel;


//== API FUNCTIONS ==

/** @brief Initialize External Device GPIOs
 * @param  deviceIntCB The callback routine for device general interrupt
 *                     (NULL to disable)
 * @param  deviceRdyCB The callback routine for device ready interrupt
 *                     (NULL to disable)
 * @return bitmask of configured features for this device
 * @note First initialization after bootup leaves the device powered down
 *       and unselected.  Subsequent inits don't touch the device powered
 *       or selected states and can be used to reconfigure callback(s),
 *       which always clears any stale/pending events.  For deviceRdyCB,
 *       its interrupt is enabled upon configuration; for deviceIntCB,
 *       halExtDeviceIntEnable() must subsequently be called to enable it.
 */
extern HalExtDeviceConfig halExtDeviceInit(HalExtDeviceIrqCB deviceIntCB,
                                           HalExtDeviceIrqCB deviceRdyCB);

/** @brief Power down the external device per GPIO
 */
extern void halExtDevicePowerDown(void);

/** @brief Power up the external device per GPIO
 */
extern void halExtDevicePowerUp(void);

/** @brief Indicate if the device is ready per GPIO
 * @return true if device is ready, false otherwise
 */
extern bool halExtDeviceIsReady(void);

/** @brief Indicate if the device is not ready per GPIO
 * @return true if device is not ready, false otherwise
 */
#define halExtDeviceNotReady()  (! halExtDeviceIsReady())

/** @brief Wait for the device to become ready per GPIO
 */
extern void halExtDeviceWaitReady(void);

/** @brief Select the external device
 */
extern void halExtDeviceSelect(void);

/** @brief Unselect the external device
 */
extern void halExtDeviceDeselect(void);

/** @brief Indicate if the device is selected
 */
extern bool halExtDeviceIsSelected(void);

/** @brief Indicate if the device is not selected
 */
#define halExtDeviceNotSelected()  (! halExtDeviceIsSelected())

/** @brief Indicate if device interrupt GPIO is pending
 * @return true if device interrupt is pending, false otherwise
 */
extern bool halExtDeviceIntPending(void);

/** @brief Indicate if device interrupt is not pending
 * @return true if device interrupt is not pending, false otherwise
 */
#define halExtDeviceIntNotPending()  (! halExtDeviceIntPending())

/** @brief Disable device interrupt and increment interrupt nesting level.
 * @return interrupt level prior to disabling (0=interrupts were enabled)
 */
extern HalExtDeviceIntLevel halExtDeviceIntDisable(void);

/** @brief Decrement interrupt nesting level and, if 0, enable device
 * interrupt.
 * @param  clearPending if true clears any "stale" pending interrupt just
 *         prior to enabling device interrupt.
 * @return interrupt level prior to enabling (0=interrupts were enabled)
 */
extern HalExtDeviceIntLevel halExtDeviceIntEnable(bool clearPending);

#endif//__EXT_DEVICE_H__
