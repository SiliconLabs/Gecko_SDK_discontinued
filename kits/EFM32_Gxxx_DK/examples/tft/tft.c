/**************************************************************************//**
 * @file
 * @brief TFT example for EFM32_G2xx_DK using EBI access
 * @version 4.2.0
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

#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "bsp.h"
#include "bsp_trace.h"
#include "tftdraw.h"

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
  uint16_t aemState  = 0;
  int      firstRun  = 1;
  int      toggleLED = 0;

  /* Chip revision alignment and errata fixes */
  CHIP_Init();

  /* Use 32MHZ HFXO as core clock frequency */
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);

  /* Initialize DVK board register access */
  BSP_Init(BSP_INIT_DEFAULT);

  /* If first word of user data page is non-zero, enable eA Profiler trace */
  BSP_TraceProfilerSetup();

  /* Setup SysTick Timer for 1 msec interrupts  */
  if (SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 1000))
  {
    while (1) ;
  }

  /* Update TFT display forever */
  while (1)
  {
    /* Check if we should control TFT display instead of
     * AEM/board control application. Read state of AEM pushbutton */
    aemState = BSP_RegisterRead(BC_AEMSTATE);
    if (aemState == 1)
    {
      /* Make sure TFT init is run at least once */
      if ((BSP_RegisterRead(BC_BUS_CFG) == BC_BUS_CFG_FSMC) || firstRun)
      {
        TFT_init();
        firstRun = 0;
      }
      /* Update display */
      TFT_displayUpdate();
    }

    /* Toggle led after each TFT_displayUpdate iteration */
    if (toggleLED)
    {
      BSP_LedsSet(0x0000);
      toggleLED = 0;
    }
    else
    {
      BSP_LedsSet(0x000f);
      toggleLED = 1;
    }
  }
}
