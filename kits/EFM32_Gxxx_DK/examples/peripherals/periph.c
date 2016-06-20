/**************************************************************************//**
 * @file
 * @brief Peripheral Toggle Example, enables peripherals. Verify by checking
 *        board LEDs
 * @version 4.1.0
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2014 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include "em_chip.h"
#include "em_device.h"
#include "em_cmu.h"
#include "bsp.h"
#include "bsp_trace.h"

volatile uint32_t msTicks; /* counts 1ms timeTicks */

/* Local prototypes */
void Delay(uint32_t dlyTicks);

/**************************************************************************//**
 * @brief SysTick_Handler
 * Interrupt Service Routine for system tick counter
 *****************************************************************************/
void SysTick_Handler(void)
{
  msTicks++;       /* increment counter necessary in Delay()*/
}

/**************************************************************************//**
 * @brief Delays number of msTick Systicks (typically 1 ms)
 * @param dlyTicks Number of ticks to delay
 *****************************************************************************/
void Delay(uint32_t dlyTicks)
{
  uint32_t curTicks;

  curTicks = msTicks;
  while ((msTicks - curTicks) < dlyTicks) ;
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  /* Chip revision alignment and errata fixes */
  CHIP_Init();

  /* Initialize DVK board register access */
  BSP_Init(BSP_INIT_DEFAULT);

  /* If first word of user data page is non-zero, enable eA Profiler trace */
  BSP_TraceProfilerSetup();

  /* Setup SysTick Timer for 1 msec interrupts  */
  if (SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 1000))
  {
    while (1) ;
  }

  /* "Silly" loop that just enables peripheral access to the EFM32, and then
   * disables them again. Verify that DVK LEDs light up when access is enabled */
  while (1)
  {
    BSP_PeripheralAccess(BSP_POTMETER, true); Delay(500);
    BSP_PeripheralAccess(BSP_AMBIENT, true); Delay(500);
    BSP_PeripheralAccess(BSP_IRDA, true); Delay(500);
    BSP_PeripheralAccess(BSP_AUDIO_OUT, true); Delay(500);
    BSP_PeripheralAccess(BSP_AUDIO_IN, true); Delay(500);
    BSP_PeripheralAccess(BSP_ANALOG_SE, true); Delay(500);
    BSP_PeripheralAccess(BSP_ANALOG_DIFF, true); Delay(500);
    BSP_PeripheralAccess(BSP_RS232B, true); Delay(500);
    BSP_PeripheralAccess(BSP_RS232A, true); Delay(500);
    BSP_PeripheralAccess(BSP_ACCEL, true); Delay(500);
    BSP_PeripheralAccess(BSP_SPI, true); Delay(500);
    BSP_PeripheralAccess(BSP_I2C, true); Delay(500);

    BSP_PeripheralAccess(BSP_POTMETER, false); Delay(500);
    BSP_PeripheralAccess(BSP_AMBIENT, false); Delay(500);
    BSP_PeripheralAccess(BSP_IRDA, false); Delay(500);
    BSP_PeripheralAccess(BSP_AUDIO_OUT, false); Delay(500);
    BSP_PeripheralAccess(BSP_AUDIO_IN, false); Delay(500);
    BSP_PeripheralAccess(BSP_ANALOG_SE, false); Delay(500);
    BSP_PeripheralAccess(BSP_ANALOG_DIFF, false); Delay(500);
    BSP_PeripheralAccess(BSP_RS232B, false); Delay(500);
    BSP_PeripheralAccess(BSP_RS232A, false); Delay(500);
    BSP_PeripheralAccess(BSP_ACCEL, false); Delay(500);
    BSP_PeripheralAccess(BSP_SPI, false); Delay(500);
    BSP_PeripheralAccess(BSP_I2C, false); Delay(500);
  }
}
