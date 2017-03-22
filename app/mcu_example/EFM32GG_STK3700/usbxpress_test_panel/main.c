/**************************************************************************//**
 * @file main.c
 * @brief Main routine for USBXpress Test Panel example.
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

/// Size of the buffer to hold USB data
#define USB_BUFFER_SIZE         8

#define PB0_PORT   gpioPortB              /// Pushbutton 0 port
#define PB0_PIN    9                      /// Pushbutton 0 pin
#define PB1_PORT   gpioPortB              /// Pushbutton 1 port
#define PB1_PIN    10                     /// Pushbutton 1 pin

// -----------------------------------------------------------------------------
// Function Prototypes

void usbx_callback(void);
void gpioSetup(void);

// -----------------------------------------------------------------------------
// Variable Definitions

/// Last packet received from host
USBX_BUF(outPacket, USB_BUFFER_SIZE);

/// Next packet to sent to host
USBX_BUF(inPacket, USB_BUFFER_SIZE);

/// State of PB0
bool pb0State = false;

/// State of PB1
bool pb1State = false;

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

/// Flag determining whether USB data should be transmitted
bool transmitUsbData = false;

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

  // Initialize LED's
  BSP_LedsInit();
  BSP_LedClear(0);
  BSP_LedClear(1);

  gpioSetup();

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
  }
}

// -------------------------------
// Interrupt handlers

/**************************************************************************//**
 * @brief USBXpress call-back function
 *
 * This function is called by USBXpress. In this example any received data
 * sent back up to the host.
 *
 *****************************************************************************/
void usbx_callback(void)
{
   uint32_t readLen;
   uint32_t intval = USBX_getCallbackSource();

   // Device Opened
   if (intval & USBX_DEV_OPEN)
   {
     USBX_blockRead(outPacket, USB_BUFFER_SIZE, &readLen);
     transmitUsbData = true;
   }

   // Device Closed or Suspended
   if (intval & (USBX_DEV_CLOSE | USBX_DEV_SUSPEND))
   {
     transmitUsbData = false;
     
     // Turn off LED's
     BSP_LedClear(0);
     BSP_LedClear(1);
   }

   // USB Read complete
   if (intval & USBX_RX_COMPLETE)
   {
     // Set the LED's based on the values sent from the host
     if (outPacket[0] == 1)
     {
       BSP_LedSet(0);
     }
     else
     {
       BSP_LedClear(0);
     }
     if (outPacket[1] == 1)
     {
       BSP_LedSet(1);
     }
     else
     {
       BSP_LedClear(1);
     }

     USBX_blockRead(outPacket, USB_BUFFER_SIZE, &readLen);
   }
}

/**************************************************************************//**
 * @brief SysTick_Handler
 * Interrupt Service Routine for system tick counter
 *****************************************************************************/
void SysTick_Handler(void)
{
  static uint32_t writeLen;
  static uint32_t msTicks = 0;

  if (msTicks++ >= 750)
  {
    msTicks = 0;
    inPacket[0] = pb0State;       // Send status of switch 0
    inPacket[1] = pb1State;       // and switch 1 to host

    // PB0 - PB3
    inPacket[2] = (GPIO_PortInGet(gpioPortB) & 0x0F);

    if (transmitUsbData)
    {
      USBX_blockWrite(inPacket, USB_BUFFER_SIZE, &writeLen);
    }
  }
}

/**************************************************************************//**
* @brief GPIO Interrupt handler (Pushbutton 0).
*****************************************************************************/
void GPIO_ODD_IRQHandler(void)
{
  // Acknowledge interrupt.
  GPIO_IntClear(1 << 9);

  pb0State = !pb0State;
}

/**************************************************************************//**
* @brief GPIO Interrupt handler (Pushbutton 1).
*****************************************************************************/
void GPIO_EVEN_IRQHandler(void)
{
  // Acknowledge interrupt.
  GPIO_IntClear(1 << 10);

  pb1State = !pb1State;
}

// -------------------------------
// Initialization Functions

/**************************************************************************//**
* @brief Setup GPIO interrupt to change demo mode.
*****************************************************************************/
void gpioSetup(void)
{
  // Enable GPIO in CMU.
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure PB9 as input and enable interrupt.
  GPIO_PinModeSet(PB0_PORT, PB0_PIN, gpioModeInputPull, 1);
  GPIO_IntConfig(PB0_PORT, PB0_PIN, false, true, true);

  NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
  NVIC_EnableIRQ(GPIO_ODD_IRQn);

  // Configure PB10 as input and enable interrupt.
  GPIO_PinModeSet(PB1_PORT, PB1_PIN, gpioModeInputPull, 1);
  GPIO_IntConfig(PB1_PORT, PB1_PIN, false, true, true);

  NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
  NVIC_EnableIRQ(GPIO_EVEN_IRQn);

  // Configure PB0, PB1, PB2, and PB3 as inputs
  GPIO_PinModeSet(gpioPortB, 0, gpioModeInput, 0);
  GPIO_PinModeSet(gpioPortB, 1, gpioModeInput, 0);
  GPIO_PinModeSet(gpioPortB, 2, gpioModeInput, 0);
  GPIO_PinModeSet(gpioPortB, 3, gpioModeInput, 0);

}
