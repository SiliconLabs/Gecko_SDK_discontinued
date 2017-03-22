/**************************************************************************//**
 * @file main.c
 * @brief Main routine for USBXpress Echo example.
 * @version 5.1.2
 ******************************************************************************
 * @section License
 * <b>Copyright 2016 Silicon Laboratories, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

// -----------------------------------------------------------------------------
// Includes

#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "bsp.h"
#include "bsp_trace.h"
#include "em_usbxpress.h"

// -----------------------------------------------------------------------------
//  Constant Definitions

/** Size of the buffer to hold USB data **/
/** This should be a multiple of 64 **/
#define USB_BUFFER_SIZE         4096

// -----------------------------------------------------------------------------
// Variable Definitions

/// Buffer to hold USB data
USBX_BUF(usbBuffer, USB_BUFFER_SIZE);

/// Length of data in usbBuffer
uint32_t usbBufferLen;

/// USB Manufacturer String
USBX_STRING_DESC(MfrString,  'S','i','l','i','c','o','n', ' ', 'L','a','b','s');

/// USB Product String
USBX_STRING_DESC(ProdString, 'U','S','B','X','p','r','e','s','s');

/// USB Serial String
USBX_STRING_DESC(SerString,  '0','0','0','0','0','0', '0','0','1','2','3','4' );

/// USBXpress Initialization Structure
USBX_Init_t initStruct =
{
  .vendorId = 0x10C4,
  .productId = 0xEA61,
  .manufacturerString = &MfrString,
  .productString = &ProdString,
  .serialString = &SerString,
  .maxPower = 0x32,
  .powerAttribute = 0x80,
  .releaseBcd = 0x0100
};

/// Determines whether LED's will toggle
bool toggleLeds = false;

// -----------------------------------------------------------------------------
// Function Prototypes

void usbx_callback(void);

// -----------------------------------------------------------------------------
// Functions

/**************************************************************************//**
 * @brief Main loop
 *
 * The main loop sets up the device and then waits forever. All active tasks
 * are ISR driven.
 *
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  BSP_LedsInit();
  BSP_LedClear(0);
  BSP_LedClear(1);

  // Setup SysTick Timer for 1 ms interrupts
  if (SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 1000))
  {
    EFM_ASSERT(false);
  }

  // Initialize and start USB device stack.
  USBX_init(&initStruct);

  // Enable USBXpress API interrupts
  USBX_apiCallbackEnable(usbx_callback);

  while (1)
  {
    // Conserve energy
    EMU_EnterEM1();
  } // Spin forever
}

// -------------------------------
// Interrupt Handlers

/**************************************************************************//**
 * @brief USBXpress call-back function
 *
 * This function is called by USBXpress. In this example any received data
 * sent back up to the host.
 *
 *****************************************************************************/
void usbx_callback(void)
{
  /// Source of the interrupt which invoked the call-back
  uint32_t intval = USBX_getCallbackSource();

  // Device configured by USB host
  if (intval & USBX_DEV_CONFIGURED)
  {
    toggleLeds = true;
    BSP_LedSet(0);
    BSP_LedClear(1);
  }

  // Device suspended
  if (intval & USBX_DEV_SUSPEND)
  {
    toggleLeds = false;
    BSP_LedClear(0);
    BSP_LedClear(1);
  }

  // Device Opened
  if (intval & USBX_DEV_OPEN)
  {
    // Start first USB Read
    USBX_blockRead(usbBuffer, USB_BUFFER_SIZE, &usbBufferLen);
  }

  // USB Read complete
  if (intval & USBX_RX_COMPLETE)
  {
    USBX_blockWrite(usbBuffer, usbBufferLen, &usbBufferLen);
  }

  // USB Write complete
  if (intval & USBX_TX_COMPLETE)
  {
    // Start a new read
    USBX_blockRead(usbBuffer, USB_BUFFER_SIZE, &usbBufferLen);
  }
}

/**************************************************************************//**
 * @brief SysTick_Handler
 * Interrupt Service Routine for system tick counter
 *****************************************************************************/
void SysTick_Handler(void)
{
  static uint32_t msTicks = 0;

  if (toggleLeds)
  {
    msTicks++;

    if (msTicks >= 1000)
    {
      msTicks = 0;
      BSP_LedToggle(0);
      BSP_LedToggle(1);
    }
  }
}
