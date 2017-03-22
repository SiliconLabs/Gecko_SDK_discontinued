/**************************************************************************//**
 * @file
 * @brief LCD controller and Energy Mode/RTC demo for EFM32G_DK3550
 * @version 5.1.2
 ******************************************************************************
 * @section License
 * <b>Copyright 2015 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "em_device.h"
#include "em_cmu.h"
#include "em_chip.h"
#include "bsp.h"
#include "bsp_trace.h"
#include "rtcdriver.h"
#include "vddcheck.h"
#include "segmentlcd.h"
#include "lcdtest.h"

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  bool vboost = false;

  /* Chip revision alignment and errata fixes */
  CHIP_Init();

  /* Initialize DK board register access, use SPI since EBI and LCD */
  /* cannot be used at the same time */
  BSP_Init(BSP_INIT_DK_SPI);

  /* If first word of user data page is non-zero, enable eA Profiler trace */
  BSP_TraceProfilerSetup();

  /* Initialize RTC timer. */
  RTCDRV_Init();
  RTCDRV_AllocateTimer( &xTimerForWakeUp);

  /* Initialize board specific registers */
  VDDCHECK_Init();

  /* Check if voltage is below 3V, if so use voltage boost */
  if (VDDCHECK_LowVoltage(2.9))
    vboost = true;

  /* Disable Voltage Comparator */
  VDDCHECK_Disable();

  /* Run Energy Mode with LCD demo, see lcdtest.c */
  SegmentLCD_Init(vboost);

  /* Display a message if vboost is enabled */
  if ( vboost )
  {
    SegmentLCD_Write("vboost");
    RTCDRV_Delay(5000);
  }
  Test();

  return 0;
}
