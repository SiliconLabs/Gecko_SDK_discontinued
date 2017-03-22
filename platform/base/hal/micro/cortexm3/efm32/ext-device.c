/** @file hal/micro/cortexm3/ext-device.c
 *  @brief External Device GPIO driver
 *
 * <!-- Copyright 2013 Silicon Laboratories, Inc.                        *80*-->
 */

#include PLATFORM_HEADER
#include "stack/include/ember-types.h"
#include "stack/include/error.h"
#include "hal/hal.h"

#include "em_device.h"
#include "em_gpio.h"
#include "gpiointerrupt.h"
#include "em_cmu.h"
#include "em_prs.h"

#include "hal/micro/cortexm3/ext-device.h"         // Implement this API

#ifdef  ENABLE_EXT_DEVICE       // Driver is enabled


//== LOCAL STATE ==

static HalExtDeviceIrqCB halExtDeviceIntCB = NULL;
static HalExtDeviceIrqCB halExtDeviceRdyCB = NULL;
static uint8_t halExtDeviceIntLevel = EXT_DEVICE_INT_UNCONFIGURED;

//== FUNCTION PROTOTYPE ==
void halIrqxIsr(uint8_t pin);

//== HELPER FUNCTIONS ==

/** @brief Configure and enable/disable the device ready IRQ
 */
static void halExtDeviceRdyCfgIrq(void)
{
}

/** @brief Configure and enable/disable the device interrupt IRQ
 */
static void halExtDeviceIntCfgIrq(void)
{
    /* Configure nIRQ signal to trigger Port Pin ISR */
    GPIO_PinModeSet((GPIO_Port_TypeDef) RF_INT_PORT, RF_INT_PIN, gpioModeInput, 1u);
    GPIO_InputSenseSet(GPIO_INSENSE_INT, GPIO_INSENSE_INT);
    GPIOINT_CallbackRegister(RF_INT_PIN, halIrqxIsr);
    GPIO_IntConfig((GPIO_Port_TypeDef) RF_INT_PORT, RF_INT_PIN, false, true, true);

    if (halExtDeviceIntCB == NULL) {
      halExtDeviceIntLevel = EXT_DEVICE_INT_UNCONFIGURED;
    } else {
      halExtDeviceIntLevel = EXT_DEVICE_INT_LEVEL_OFF;
      // Callers need to use halExtDeviceIntEnable() to enable top level
    }
}


//== INTERNAL ISRS ==

void halIrqxIsr(uint8_t pin)
{
  UNUSED_VAR(pin);

  /* Clear IT flag */
  GPIO_IntClear(1 << RF_INT_PIN);

  /* Call IT handler function if defined */
  if (halExtDeviceIntCB != NULL) {
    (*halExtDeviceIntCB)();
  }

  if (halExtDeviceIntPending()) {
    // Repend this INT
    GPIO_IntSet(1 << RF_INT_PIN);
  }
}



//== API FUNCTIONS ==

//-- External Device Initialization --

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
HalExtDeviceConfig halExtDeviceInit(HalExtDeviceIrqCB deviceIntCB,
                                    HalExtDeviceIrqCB deviceRdyCB)
{
  UNUSED_VAR(halExtDeviceRdyCB); // Work around potential compiler warnings
  UNUSED_VAR(halExtDeviceIntCB); // Work around potential compiler warnings
  halExtDeviceRdyCB = deviceRdyCB;
  halExtDeviceIntCB = deviceIntCB; 

  CMU_ClockEnable(cmuClock_PRS, true);

  /* Pin is configured to Push-pull: SDN */
  GPIO_PinModeSet((GPIO_Port_TypeDef) RF_SDN_PORT, RF_SDN_PIN, gpioModePushPull, 1u);

  /* Pin is configured to Push-pull: nSEL */
  GPIO_PinModeSet((GPIO_Port_TypeDef) RF_USARTRF_CS_PORT, RF_USARTRF_CS_PIN, gpioModePushPull, 1u);

  /* Pin PE13 is configured input: nIRQ */
  //GPIO_PinModeSet(RF_INT_PORT, RF_INT_PIN, gpioModeInput, 0u);

  // EZR32LG and EZR32WG have the GPIO0/1 pins connected to MCU GPIO.
  // Setup passthrough using PRS.
#if defined(_EZR32_LEOPARD_FAMILY) || defined(_EZR32_WONDER_FAMILY)
  /* Pin PA15 and PE14 are connected to GPIO0 and GPIO1 respectively. */
  GPIO_PinModeSet((GPIO_Port_TypeDef) RF_GPIO0_PORT, RF_GPIO0_PIN, gpioModeInput, 0);
  GPIO_PinModeSet((GPIO_Port_TypeDef) RF_GPIO1_PORT, RF_GPIO1_PIN, gpioModeInput, 0);

  /* Pin PA0 and PA1 are output the GPIO0 and GPIO1 via PRS */
  GPIO_PinModeSet(gpioPortA, 0, gpioModePushPull, 0);
  GPIO_PinModeSet(gpioPortA, 1, gpioModePushPull, 0);

  /* Configure INT/PRS channels */
  GPIO_IntConfig((GPIO_Port_TypeDef) RF_GPIO0_PORT, RF_GPIO0_PIN, false, false, false);
  GPIO_IntConfig((GPIO_Port_TypeDef) RF_GPIO1_PORT, RF_GPIO1_PIN, false, false, false);

  /* Setup PRS */
  PRS_SourceAsyncSignalSet(0, PRS_CH_CTRL_SOURCESEL_GPIOH, PRS_CH_CTRL_SIGSEL_GPIOPIN15);
  PRS_SourceAsyncSignalSet(1, PRS_CH_CTRL_SOURCESEL_GPIOH, PRS_CH_CTRL_SIGSEL_GPIOPIN14);
  PRS->ROUTE = (PRS_ROUTE_CH0PEN | PRS_ROUTE_CH1PEN);

  /* Make sure PRS sensing is enabled (should be by default) */
  GPIO_InputSenseSet(GPIO_INSENSE_PRS, GPIO_INSENSE_PRS);
#endif // defined(_EZR32_LEOPARD_FAMILY) || defined(_EZR32_WONDER_FAMILY)

  /* TODO: Check whether the removed part is required 
     for the EZR32 implementation */
  halExtDeviceRdyCfgIrq();
  halExtDeviceIntCfgIrq();
  
  return 0;
}

//-- External Device Power --

/** @brief Power down the external device per GPIO
 */
void halExtDevicePowerDown(void)
{
  /* SDN high */
    GPIO_PinOutSet((GPIO_Port_TypeDef) RF_SDN_PORT, RF_SDN_PIN);
}

/** @brief Power up the external device per GPIO
 */
void halExtDevicePowerUp(void)
{
  /* SDN low */
    GPIO_PinOutClear((GPIO_Port_TypeDef) RF_SDN_PORT, RF_SDN_PIN);
}

//-- External Device Ready --

/** @brief Indicate if the device is ready per GPIO
 * @return true if device is ready, false otherwise
 */
bool halExtDeviceIsReady(void)
{
  return 0;
}

/** @brief Wait for the device to become ready per GPIO
 */
void halExtDeviceWaitReady(void)
{
  halResetWatchdog();
  while (! halExtDeviceIsReady()) {
    // spin
  }
  halResetWatchdog();
}

//-- External Device Selection --

/** @brief Select the external device
 */
void halExtDeviceSelect(void)
{
  /* nSEL low */
    GPIO_PinOutClear((GPIO_Port_TypeDef) RF_USARTRF_CS_PORT, RF_USARTRF_CS_PIN);
}

/** @brief Unselect the external device
 */
void halExtDeviceDeselect(void)
{
  /* nSEL high */
    GPIO_PinOutSet((GPIO_Port_TypeDef) RF_USARTRF_CS_PORT, RF_USARTRF_CS_PIN);
}

/** @brief Indicate if the device is selected
 */
bool halExtDeviceIsSelected(void)
{
  return 0;
}

//-- External Device Interrupts --

/** @brief Indicate if device interrupt GPIO is pending
 * @return true if device interrupt is pending, false otherwise
 */
bool halExtDeviceIntPending(void)
{
  return (GPIO_PinInGet((GPIO_Port_TypeDef) RF_INT_PORT, RF_INT_PIN) == false);
}

/** @brief Disable device interrupt and increment interrupt nesting level.
 * @return interrupt level prior to disabling (0=interrupts were enabled)
 */
HalExtDeviceIntLevel halExtDeviceIntDisable(void)
{
  uint8_t origLevel;

  // Disable interrupt with the given pin.
  GPIO_IntDisable(1 << RF_INT_PIN);

  // We don't bother with 2nd-level here
  ATOMIC( // ATOMIC because these routines might be called from other ISRs
    origLevel = halExtDeviceIntLevel;
    if (origLevel != EXT_DEVICE_INT_UNCONFIGURED) {
      halExtDeviceIntLevel += 1;
    }
  );

  return origLevel;
}

/** @brief Decrement interrupt nesting level and, if 0, enable device
 * interrupt.
 * @param  clearPending if true clears any "stale" pending interrupt just
 *         prior to enabling device interrupt.
 * @return interrupt level prior to enabling (0=interrupts were enabled)
 */
HalExtDeviceIntLevel halExtDeviceIntEnable(bool clearPending)
{
  uint8_t origLevel;
  bool justEnabled = false;

  ATOMIC( // ATOMIC because these routines might be called from other ISRs
      origLevel = halExtDeviceIntLevel;
      if (origLevel != EXT_DEVICE_INT_UNCONFIGURED) {
        if (origLevel > EXT_DEVICE_INT_LEVEL_ON) { // Peg at LEVEL_ON
          halExtDeviceIntLevel -= 1;
          justEnabled = (halExtDeviceIntLevel == EXT_DEVICE_INT_LEVEL_ON);
        }
      }
  );

  if (clearPending) {
    // Clear out any stale state
    GPIO_IntClear(1 << RF_INT_PIN);
  }

  if (justEnabled) {
    if (halExtDeviceIntPending()) { // in case we missed edge of level int
      // Pend this INT
      GPIO_IntSet(1 << RF_INT_PIN);
    }
    GPIO_IntEnable(1 << RF_INT_PIN);
  }
  return origLevel;
}

#endif//ENABLE_EXT_DEVICE       // Driver is enabled
