/**************************************************************************//**
 * @file main.c
 * @brief Mass Storage Device example.
 * @version 5.1.2
 ******************************************************************************
 * @section License
 * <b>Copyright 2016 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/
#include <stdio.h>
#include "em_device.h"
#include "em_assert.h"
#include "em_chip.h"
#include "em_emu.h"
#include "em_cmu.h"
#include "em_core.h"
#include "em_gpio.h"
#include "bsp.h"

#include "em_usb.h"
#include "msdd.h"
#include "msddmedia.h"
#include "descriptors.h"


/**************************************************************************//**
 *
 * This example shows how a Mass Storage Device (MSD) can be implemented.
 *
 *****************************************************************************/

static void stateChangeEvent(USBD_State_TypeDef oldState,
                             USBD_State_TypeDef newState);

static const USBD_Callbacks_TypeDef callbacks =
{
  .usbReset        = NULL,
  .usbStateChange  = stateChangeEvent,
  .setupCmd        = MSDD_SetupCmd,
  .isSelfPowered   = NULL,
  .sofInt          = NULL
};

static const USBD_Init_TypeDef usbInitStruct =
{
  .deviceDescriptor    = &USBDESC_deviceDesc,
  .configDescriptor    = USBDESC_configDesc,
  .stringDescriptors   = USBDESC_strings,
  .numberOfStrings     = sizeof(USBDESC_strings)/sizeof(void*),
  .callbacks           = &callbacks,
  .bufferingMultiplier = USBDESC_bufferingMultiplier,
  .reserved            = 0
};

/**************************************************************************//**
 * @brief stateChangeEvent - Called by USB device stack when USB state
 *        has changed.
 *****************************************************************************/
static void stateChangeEvent(USBD_State_TypeDef oldState,
                             USBD_State_TypeDef newState)
{
  // Light LED 1 according to USB state.
  if (newState == USBD_STATE_CONFIGURED) {
    BSP_LedSet(1);
  } else {
    BSP_LedClear(1);
  }

  // Call MSD drivers state change event handler.
  MSDD_StateChangeEvent(oldState, newState);
}

/**************************************************************************//**
 * @brief main - the entrypoint after reset.
 *****************************************************************************/
int main(void)
{
  // Chip errata.
  CHIP_Init();

  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);
  CMU_OscillatorEnable(cmuOsc_LFXO, true, false);
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Initialize LED driver.
  BSP_LedsInit();

  // Initialize the Mass Storage Media.
  if (!MSDDMEDIA_Init()) {
    EFM_ASSERT(false);
    for( ;; ){}
  }

  // Initialize the Mass Storage Device driver.
  MSDD_Init(gpioPortF, 4);

  // Initialize and start USB device stack.
  USBD_Init(&usbInitStruct);

  for (;;) {
    if (MSDD_Handler()) {
      // There is no pending activity in the MSDD handler.
      // Enter sleep mode to conserve energy.

      if (USBD_SafeToEnterEM2()) {
        // Disable IRQ's and perform test again to avoid race conditions !
        CORE_ATOMIC_SECTION(
          if (USBD_SafeToEnterEM2()) {
            EMU_EnterEM2(true);
          }
        )
      } else {
        EMU_EnterEM1();
      }
    }
  }
}
